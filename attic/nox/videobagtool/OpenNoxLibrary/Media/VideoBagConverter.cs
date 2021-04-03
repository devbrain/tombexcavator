using System;
using System.IO;
using System.Collections.Generic;
using System.Diagnostics;
using System.Runtime.InteropServices;
using FileIndex = OpenNoxLibrary.Media.VideoBag.FileIndex;

namespace OpenNoxLibrary.Media
{
	/// <summary>
	/// A class that provides real-time access to packed graphics data
	/// </summary>
	public class VideoBagConverter
	{
        public const string VIDEO_BAG = "video.bag";
        public const string VIDEO_IDX = "video.idx";
        public const string VIDEO8_BAG = "video8.bag";
        public const string VIDEO8_IDX = "video8.idx";
        public const string VIDEO_PAL = "default.pal";
        
        /// <summary>
        /// 8-bit palette contents
        /// </summary>
        protected uint[] Palette8Bit = new uint[256];
        
        /// <summary>
        /// Used by type 4, 6 images
        /// </summary>
        public uint[] Type46Colors = new uint[] { 0xFF3F3F3F, 0xFF3F3FFF, 0xFF3FFFFF, 0xFFFF3F3F, 0xFFFFFFFF };

        /// <summary>
        /// Reference to video.idx/video.bag worker
        /// </summary>
        protected VideoBag videoBag;

        /// <summary>
        /// Hints the decoder/encoder to use the 8bit palette
        /// </summary>
        protected bool is8Bit;

        /// <summary>
        /// Hints the decoder to be more precise when up-scaling color values, at the expense of cpu-time
        /// </summary>
        protected bool preciseUpscale = true;

        /// <summary>
        /// Omits loading of unnecessary data from video.idx/video.bag to memory
        /// </summary>
        protected bool skipUnnecessary = false;

        /// <summary>
        /// If true, palette must be loaded
        /// </summary>
        public bool Is8Bit
        {
            get
            {
                return is8Bit;
            }
        }

        /// <summary>
        /// Represents an uncompressed Nox image bitmap data. 
        /// </summary>
        public struct NoxImageData
        {
            public int Width;
            public int Height;
            public int OffsX;
            public int OffsY;

            /// <summary>
            /// Color bitmap, in 32-bit BGRA/ARGB format.
            /// </summary>
            public uint[] ColorData;
        }

		public VideoBagConverter(string bagPath, string idxPath)
		{
            videoBag = new VideoBag(bagPath, idxPath, skipUnnecessary);

            videoBag.ReadIDXEntries();
            videoBag.OpenDataStream();

            this.is8Bit = videoBag.Flags2 == 0x8000;
		}

        public VideoBag GetBaseFile()
        {
            return videoBag;
        }

        public void Close()
        {
            videoBag.CloseDataStream();
        }
		
		/// <summary>
		/// Reads 256-bit palette used for decoding images from PAL file.
		/// </summary>
		public void LoadPalette(string palettePath)
		{
			var br = new BinaryReader(File.OpenRead(palettePath));
            br.BaseStream.Seek(7, SeekOrigin.Begin); // PALETTE

            for (int i = 0; i < 256; i++)
            {
                byte r = br.ReadByte();
                byte g = br.ReadByte();
                byte b = br.ReadByte();
                Palette8Bit[i] = (uint)0xFF000000 | (uint)(r << 16) | (uint)(g << 8) | b;
            }
            br.Close();
		}

        public void LoadPalette(uint[] pal)
        {
            if (pal.Length < 256) throw new ArgumentException("Palette is too short");

            Palette8Bit = pal;
        }
		
        /// <summary>
        /// Looks up and decodes 32-bit ARGB image data by specified global index.
        /// </summary>
		public unsafe NoxImageData PullByIndex(int index)
        {
            var imageEntry = videoBag.PullFileIndex(index);
            byte[] imageData = videoBag.PullSectionDataWithCache(imageEntry.SectionId);

            NoxImageData result = new NoxImageData();

			fixed (byte* dataPtr = imageData)
			{
				switch (imageEntry.DataType)
				{
					case 0:
						// Tiles
						result.Width = 46; result.Height = 46;
						result.ColorData = DecodeType0Image(imageEntry, dataPtr);
						break;
					case 1:
						// Tile edges
						result.Width = 46; result.Height = 46;
                        result.ColorData = DecodeType1Image(imageEntry, dataPtr);
                        break;
					case 3:
						// kinda PCX	
	  				case 4:
					case 5:
					case 6:
						// Same as 3 but with dynamic colors
						uint moff = imageEntry.SectionOffset;
                        result.Width = *((int*)(dataPtr + moff));
                        result.Height = *((int*)(dataPtr + moff + 4));
                        result.OffsX = *((int*)(dataPtr + moff + 8));
                        result.OffsY = *((int*)(dataPtr + moff + 12));
						result.ColorData = DecodeType3456Image(imageEntry, dataPtr);
						break;	
				}
			}
			
			return result;
		}
		
		private uint[] CreateBitArray(int width, int height)
		{
			uint[] result = new uint[width * height];
			return result;
		}

        /// <summary>
        /// Converts 15-bit color value to 32-bit color space
        /// </summary>
        private uint Expand15Color(ushort src)
        {
            // Extract 5 bit color values
            uint r = (byte)((src & 0xFC00) >> 10);
            uint g = (byte)((src & 0x03E0) >> 5);
            uint b = (byte)(src & 0x1F);

            // Normalize color space 
            if (preciseUpscale) // Slower and more precise version
            {
                r = (uint)(r / 31F * 255F);
                g = (uint)(g / 31F * 255F);
                b = (uint)(b / 31F * 255F);
            }
            else
            {
                r <<= 3;
                g <<= 3;
                b <<= 3;
            }

            return (uint)0xFF000000 | ((uint)r << 16) | ((uint)g << 8) | (uint)b;
        }
		
		private unsafe uint[] DecodeType0Image(FileIndex fe, byte* data)
		{
            int offs = (int)fe.SectionOffset;
			uint[] bitmap = CreateBitArray(46, 46);

            // all tile/edge images are being packed pixel-by-pixel starting from center (23) column, up->down, left->right
            int i = 1; int c = 23; 

            for (int row = 0; row < 46; row++)
            {
            	// Boundary check
            	if ((data + offs) > (data + fe.SectionOffset + fe.DataLength)) 
                    return null;
            	
            	for (int col = 0; col < i; col++)
            	{
                    if (is8Bit)
                    {
                        bitmap[(col + c) + row * 46] = Palette8Bit[*(offs + data)];
                        offs++;
                    }
                    else
                    {
                        ushort clr = (ushort)(*(data + offs) | (*(data + offs + 1) << 8));
                        offs += 2;

                        bitmap[(col + c) + row * 46] = Expand15Color(clr);
                    }
            	}
            	if (row < 22)
            	{
            		i += 2;
            		c--;
            	}
            	else if (row > 22)
            	{
            		i -= 2;
            		c++;
            	}
            }
			return bitmap;
		}

        private unsafe uint[] DecodeType1Image(FileIndex fe, byte* data)
        {
            int offs = (int)fe.SectionOffset;

            uint[] bitmap = CreateBitArray(46, 46);
            uint[] buffer = new uint[256]; 
            byte buflen = 0, bufpos = 0;

            byte startX = *(data + offs);
            byte endX = *(data + offs + 1);
            offs += 2;

            int c = 23, ci = 1; // offset from l/r, and column index
            
            for (int row = 0; row <= endX; row++)
            {
                if (row >= startX)
                {
                    for (int col = 0; col < ci; col++)
                    {
                        // Fill colorbuffer
                        if (bufpos >= buflen)
                        {
                            bufpos = 0;
                            byte op = *(data + offs); offs++;
                            buflen = *(data + offs); offs++;

                            switch (op)
                            {
                                case 4: // Underlying tile most probably
                                case 1: 
                                    // Transparent pixels
                                    for (int i = 0; i < buflen; i++) buffer[i] = 0;

                                    break;
                                case 2: 
                                    // Static colors
                                    for (int i = 0; i < buflen; i++) 
                                    {
                                        if (is8Bit)
                                        {
                                            buffer[i] = Palette8Bit[*(data + offs)];
                                            offs++;
                                        }
                                        else
                                        {
                                            ushort clr = (ushort)(*(data + offs) | (*(data + offs + 1) << 8));
                                            offs += 2;

                                            buffer[i] = Expand15Color(clr);
                                        }
                                    }

                                    break;
                                case 3: 
                                    // Copy from source / upper tile
                                    for (int i = 0; i < buflen; i++)
                                        buffer[i] = 0xFFFF00FF;

                                    break;
                                default:
                                    throw new Exception("Unexpected RLE code when decoding Type1 entry");
                            }
                        }

                        // Copy from buffer
                        bitmap[(col + c) + row * 46] = buffer[bufpos];
                        bufpos++;
                    }
                }
                if (row < 22)
                {
                    ci += 2;
                    c--;
                }
                else if (row > 22)
                {
                    ci -= 2;
                    c++;
                }
            }

            return bitmap;
        }

        public unsafe byte[] EncodeType35Image(int w, int h, int offx, int offy)
        {
            return null;
        }
		
        // FIXME: type 5 images from video8.bag found in Nox demo version do not get decoded properly
		private unsafe uint[] DecodeType3456Image(FileIndex fe, byte* data)
		{
			byte op = 3; uint index = 0, pixLength = 0, pos = fe.SectionOffset;
			int width = *((int*) (data + pos));
			int height = *((int*) (data + pos + 4));
			
			uint[] bitmap = CreateBitArray(width, height);
			pos += 17; // Width, Height, OffsetX, OffsetY, Unknown
			long length = fe.SectionOffset + fe.DataLength;
			
			while (op != 0 && pos < length)
			{
				op = *(data + pos); pos++;
				// RLE codes
				switch (op)
				{
					case 0:
						// End
						break;
					case 1:
						// Skip x pixels
						index += *(data + pos); pos++;
						break;
					case 2:
						// Read x pixels
                        pixLength = *(data + pos); pos++;

                        while (pixLength > 0)
                        {
                            if (is8Bit)
                            {
                                bitmap[index] = Palette8Bit[*(data + pos)];
                                pos++;
                            }
                            else
                            {
                                ushort col = (ushort)(*(data + pos) | (*(data + pos + 1) << 8));
                                pos += 2;

                                bitmap[index] = Expand15Color(col);
                            }

                            pixLength--;
                            index++;
                        }
						break;
					case 3:
						// Begin
						break;
					case 5:
						// Half transparent (Type5)
						pixLength = *(data + pos); pos++;

                        while (pixLength > 0)
                        {
                            ushort col = (ushort)(*(data + pos) | (*(data + pos + 1) << 8));
                            pos += 2;

                            byte r, g, b, a;
                            if (preciseUpscale)
                            {
                                r = (byte)(((col >> 12) & 0xF) / 15F * 255F);
                                g = (byte)(((col >> 8) & 0xF) / 15F * 255F);
                                b = (byte)(((col >> 4) & 0xF) / 15F * 255F);
                                a = (byte)((col & 0xF) / 15F * 255F);
                            }
                            else
                            {
                                r = (byte)(((col >> 12) & 0xF) << 4);
                                g = (byte)(((col >> 8) & 0xF) << 4);
                                b = (byte)(((col >> 4) & 0xF) << 4);
                                a = (byte)((col & 0xF) << 4);
                            }

                            bitmap[index] = (uint)(a << 24 | r << 16 | g << 8 | b);

                            index++;
                            pixLength--;
                        }
						break;
					default:
						// Dynamic colors (Type46)
						if ((op & 4) >= 4)
						{
							pixLength = *(data + pos); pos++;

							//byte r, g, b;
							while (pixLength > 0)
							{   
                                /*
								float intensity = *(data + pos);
                                pos++;

                                if (is8Bit)
                                    intensity /= 32F;
                                else
                                    intensity /= 255F;
								
								// 5(last) color is never used, actually
								int ind = ((op >> 2) - 1) / 5;
                                uint color = Type46Colors[ind];
                                */

                                byte intensity = *(data + pos);
                                pos++;

                                // This way we store both MaterialColor channel that was used for this pixel, AND the intensity value
                                bitmap[index] = ((uint)0x80000000 | (uint)(op << 24) | (uint)(intensity << 16) | (uint)(intensity << 8) | (uint)intensity);
                                

								index++;
                                pixLength--;
							}
						}
						break;
				}
			}
			return bitmap;
		}
	}
}
