using System;
using System.Collections.Generic;
using System.IO;
using System.Text;
using System.Runtime.InteropServices;

namespace OpenNoxLibrary.Media
{
    /// <summary>
    /// Provides application interface for quering and modifying the contents of game's packed graphics data files.
    /// </summary>
    public class VideoBag
    {
        // Binary data container path.
        string bagPath;
        // Index container path.
        string idxPath;
        // Skips reading of unnecessary information from index entries, saving some time and memory.
        bool skipUnnecessary;

        // List of all sections that contain file indexes
        List<Section> sections;
        // List of ALL file indexes in ALL sections
        List<FileIndex> indexes;

        public int SectionCount
        {
            get
            {
                return sections.Count;
            }
        }

        public int IndexCount
        {
            get
            {
                return indexes.Count;
            }
        }

        public string IdxPath
        {
            get
            {
                return idxPath;
            }
        }

        public string BagPath
        {
            get
            {
                return bagPath;
            }
        }

        public uint Unknown1 { get; set; } // From all what I have seen, it's always 0x8000
        public uint Flags2 { get; set; } // 0x8000 for 8-bit files, any other value otherwise
        public uint InitialIndexes { get; set; } // Seems to be very close to index count

        const int SECTION_CACHE_SIZE = 8;
        // Section cache for quick video data retrieval
        UncompressedSectionCache readSectionCache;

        // Used for real-time reading/writing from video.bag
        FileStream videoBagStream;

        /// <summary>
        /// Constructs a new interface for pair of files. If specified file paths do not exist, no error is generated.
        /// </summary>
        public VideoBag(string bagPath, string idxPath, bool skipUnnecessary = false)
        {
            this.bagPath = bagPath;
            this.idxPath = idxPath;
            this.skipUnnecessary = skipUnnecessary;

            videoBagStream = null;
            readSectionCache = new UncompressedSectionCache();
            sections = new List<Section>();
        }

        /// <summary>
        /// Allows to actually read or write packed graphics data. Otherwise you are restricted to reading image indexes. 
        /// </summary>
        public void OpenDataStream()
        {
            if (videoBagStream != null)
                throw new InvalidOperationException("VideoBag stream is already open");

            videoBagStream = File.Open(bagPath, FileMode.OpenOrCreate, FileAccess.ReadWrite);
        }

        public void CloseDataStream()
        {
            if (videoBagStream == null)
                throw new InvalidOperationException("VideoBag stream is not open");

            videoBagStream.Close();
            videoBagStream = null;
        }

        /// <summary>
        /// Returns all image indexes in all sections
        /// </summary>
        public FileIndex[] GetAllFileIndexes()
        {
            return indexes.ToArray();
        }

        /// <summary>
        /// Represents a packedimage index record
        /// </summary>
        public struct FileIndex
        {
            // Tracks which section this file belongs to
            public int SectionId;
            // Offset in uncompressed section data
            public uint SectionOffset;
            // General purpose
        	
            public string Filename;
        	public byte DataType;
        	public uint DataLength;
            // It's half of colorspace (0x422/0x844) size for tiles (logic: of entire 46x46 square, 23x23 pixels are left blank), idk what this is for others 
            // Most probably, this is 'clean' source file size before packing, or some kind of buffer length
        	public uint Unknown;
        	
            /// <summary>
            /// Constructs empty index record
            /// </summary>
            public FileIndex(string Filename, byte DataType, uint Size, uint Unknown) 
            {
                SectionId = 0;
                SectionOffset = 0;

                this.Filename = Filename;
                this.DataType = DataType;
                this.DataLength = Size;
                this.Unknown = Unknown;
            }

            /// <summary>
            /// Constructs index from an incoming data buffer
            /// </summary>
            public FileIndex(BinaryReader br, bool skipUnnecessary)
        	{
                SectionId = 0;
                SectionOffset = 0;

                Filename = null;
                if (skipUnnecessary) 
                    br.ReadBytes(br.ReadByte());
                else
                {
                    byte len = br.ReadByte();
                    Filename = Encoding.ASCII.GetString(br.ReadBytes(len));
                    Filename = Filename.TrimEnd('\0'); // Idk why did they include the null-terminator, if string is length prefixed
                }

                DataType = br.ReadByte();
                DataLength = br.ReadUInt32();
                Unknown = br.ReadUInt32();
        	}

            public void Write(BinaryWriter bw)
            {
                if (Filename == null)
                {
                    bw.Write((byte)1);
                    bw.Write((byte)0);
                }
                else
                {
                    bw.Write((byte)(Filename.Length + 1));
                    bw.Write(Encoding.ASCII.GetBytes(Filename));
                    bw.Write((byte)0);
                }

                bw.Write(DataType);
                bw.Write(DataLength);
                bw.Write(Unknown);
            }
        }

        /// <summary>
        /// Represents a section that encapsulates some number of media files. AKA chunk in data/archive terminology
        /// </summary>
        public struct Section
        {
            /// <summary>
            /// In-file offset for this section's compressed data
            /// </summary>
            public uint VideoBagOffset; 

            /// <summary>
            /// Size/offset in compressed data
            /// </summary>
            public uint LengthCompressed;

            /// <summary>
            /// Image data size when uncompressed
            /// </summary>
            public uint LengthUncompressed;

            /// <summary>
            /// Constructs a section from an incoming data buffer
            /// </summary>
            public Section(BinaryReader br)
            {
                br.ReadUInt32(); // IDX section entry length, not needed for now
                VideoBagOffset = 0;
                LengthUncompressed = br.ReadUInt32();
                LengthCompressed = br.ReadUInt32();
            }

            public void Write(BinaryWriter bw)
            {
                bw.Write((uint)0xFFFFFFFF); // idx section length, will be rewritten later
                bw.Write(LengthUncompressed);
                bw.Write(LengthCompressed);
            }
        }

        /// <summary>
        /// Reads all index entries, storing them in memory.
        /// </summary>
        public void ReadIDXEntries()
        {
            var videoIdxStream = new BinaryReader(File.Open(idxPath, FileMode.Open), Encoding.ASCII);

            uint magic = videoIdxStream.ReadUInt32();

            if (magic == 0xFAEDBCEB)
            {
                uint Length = videoIdxStream.ReadUInt32();
                uint SectionCount = videoIdxStream.ReadUInt32();

                Unknown1 = videoIdxStream.ReadUInt32();
                Flags2 = videoIdxStream.ReadUInt32();
                InitialIndexes = videoIdxStream.ReadUInt32();

                // Now sections/chunks follow
                sections = new List<Section>();
                // And inside each section, there are index entries
                indexes = new List<FileIndex>();

                // Accumulates offset for every section in video.bag
                uint sectionBagOffsetAcc = 0;

                // Read sections
                for (int sid = 0; sid < SectionCount; sid++)
                {
                    // All reading operations are done in constructor
                    Section section = new Section(videoIdxStream);

                    // Calculate offset in video.bag
                    section.VideoBagOffset = sectionBagOffsetAcc;
                    sectionBagOffsetAcc += section.LengthCompressed;

                    // Accumulates offset for each file data in uncompressed data
                    uint sectionOffsetAcc = 0;

                    // Now read file indexes
                    uint indexCount = videoIdxStream.ReadUInt32();
                    if (indexCount == 0xFFFFFFFF)
                        indexCount = 1; // Maybe defines modified section? But that's wrong

                    for (int i = 0; i < indexCount; i++)
                    {
                        // All reading operations are done in constructor
                        var fe = new FileIndex(videoIdxStream, skipUnnecessary);
                        fe.SectionId = sid;

                        // Calculate file offset in section data
                        fe.SectionOffset = sectionOffsetAcc;
                        sectionOffsetAcc += fe.DataLength;

                        indexes.Add(fe);
                    }

                    sections.Add(section);
                }
                videoIdxStream.Close();
            }
            else
            {
                videoIdxStream.Close();
                throw new ApplicationException(String.Format("Wrong VideoBag header: expected 0xFAEDBCEB, got 0x{0:X}", magic));
            }
        }

        /// <summary>
        /// Re-writes IDX file contents with actual section and index data.
        /// </summary>
        public void WriteIDXEntries()
        {
            var videoIdxStream = new BinaryWriter(File.Open(idxPath, FileMode.Create), Encoding.ASCII);

            // Write header magic
            videoIdxStream.Write((uint)0xFAEDBCEB);

            // Write length stub and section count
            videoIdxStream.Write((uint)0);
            videoIdxStream.Write(SectionCount);

            // Write header values
            videoIdxStream.Write(Unknown1);
            videoIdxStream.Write(Flags2);
            videoIdxStream.Write(InitialIndexes);

            // Write sections
            for (int sid = 0; sid < SectionCount; sid++)
            {
                sections[sid].Write(videoIdxStream);
                videoIdxStream.Write((uint)0xFFFFFFFF); // Will be overwritten with index count
                // Store offsets for this section's length and index count
                long posIndexCount = videoIdxStream.BaseStream.Position - 4;
                long posSectionLen = videoIdxStream.BaseStream.Position - 16;

                // Write indexes
                uint counter = 0;
                foreach (var index in indexes)
                {
                    if (index.SectionId == sid)
                    {
                        counter++;
                        index.Write(videoIdxStream);
                    }
                }

                long posEnd = videoIdxStream.BaseStream.Position;
                uint length = (uint)(posEnd - posSectionLen - 16);
                // Write count
                if (counter != 1) // 1 = 0xFFFFFFFF (though I have seen casual ones too)
                {
                    videoIdxStream.BaseStream.Seek(posIndexCount, SeekOrigin.Begin);
                    videoIdxStream.Write(counter);
                } 
                
                // Write length
                videoIdxStream.BaseStream.Seek(posSectionLen, SeekOrigin.Begin);
                videoIdxStream.Write(length);
                videoIdxStream.BaseStream.Seek(posEnd, SeekOrigin.Begin);
            }

            // Write total length
            uint len = (uint)videoIdxStream.BaseStream.Position;
            videoIdxStream.BaseStream.Seek(4, SeekOrigin.Begin);
            videoIdxStream.Write(len);

            videoIdxStream.Close();
        }

        /// <summary>
        /// Represents uncompressed section data buffer. Preserves hard disk reading and decompression resources.
        /// </summary>
        protected class UncompressedSectionCache
        {
            Dictionary<int, byte[]> Cache = new Dictionary<int,byte[]>();

            public UncompressedSectionCache()
            {
                Cache.Clear();
            }

            public byte[] LookupCache(int section)
            {
                if (!Cache.ContainsKey(section))
                    return null;

                return Cache[section];
            }

            public void AddToCache(int section, byte[] data)
            {
                if (Cache.Count > SECTION_CACHE_SIZE) Cache.Clear();
                Cache.Add(section, data);
            }
        }

        /// <summary>
        /// Pulls packedfile index record by specified numerical index
        /// </summary>
        public FileIndex PullFileIndex(int globalIndex)
        {
            if (globalIndex >= indexes.Count) throw new ArgumentOutOfRangeException("Index not found in this file / too large value");

            return indexes[globalIndex];
        }

        public byte[] PullSectionData(int sectionId)
        {
            if (videoBagStream == null)
                throw new InvalidOperationException("VideoBag stream is not yet open, call OpenDataStream() first");

            if (sectionId >= sections.Count) throw new ArgumentOutOfRangeException("Section ID not found in this file / too large value");

            Section sect = sections[sectionId];

            videoBagStream.Seek(sections[sectionId].VideoBagOffset, SeekOrigin.Begin);
            byte[] compressed = new byte[sect.LengthCompressed];
            byte[] uncompressed = new byte[sect.LengthUncompressed];

            videoBagStream.Read(compressed, 0, (int)sect.LengthCompressed);

            Util.NoxLzCompression.Decompress(compressed, uncompressed);
            return uncompressed;
        }

        /// <summary>
        /// Pulls entire section data, sometimes saving cpu-time on copy/decompress operations
        /// </summary>
        public byte[] PullSectionDataWithCache(int sectionId)
        {
            if (videoBagStream == null)
                throw new InvalidOperationException("VideoBag stream is not yet open, call OpenDataStream() first");

            Section sect = sections[sectionId];

            byte[] uncompressed = readSectionCache.LookupCache(sectionId);
            // Look-up section in the cache first, if it's not there, then read, decompress and store
            if (uncompressed == null)
            {
                uncompressed = PullSectionData(sectionId);
                readSectionCache.AddToCache(sectionId, uncompressed);
            }

            return uncompressed;
        }

        /// <summary>
        /// Adds another section containing specified image data at the end of the video.bag file.
        /// </summary>
        public Section AppendWriteNewSection(byte[] datatypes, string[] filenames, byte[][] imagez)
        {
            if (datatypes.Length != imagez.Length) throw new ArgumentException("Expected pairs of image index and data as arguments.");
            if (filenames.Length != imagez.Length) throw new ArgumentException("Expected pairs of image index and data as arguments.");
            if (videoBagStream == null)
                throw new InvalidOperationException("VideoBag stream is not yet open, call OpenDataStream() first");

            var sect = new Section();
            
            // This will be an Index/ID of a newly added section
            int sectionId = sections.Count;

            // All large byte-array operations must be done on disk, if you want it right.
            uint totalSize = 0;
            foreach (byte[] idata in imagez)
            {
                totalSize += (uint) idata.Length;
            }

            // But for now, it should just work.
            byte[] uncompressed = new byte[totalSize];
            sect.LengthUncompressed = totalSize;

            int ndx = 0;
            foreach (byte[] idata in imagez)
            {
                int len = idata.Length;
                Array.Copy(idata, 0, uncompressed, ndx, len);
                ndx += len;
            }
            // Yes, it should be veeeery slow.
            byte[] compressed = Util.NoxLzCompression.Compress(uncompressed);
            sect.LengthCompressed = (uint) compressed.Length;

            // And here we add another chunk of data.
            videoBagStream.Write(compressed, 0, compressed.Length);

            // And register a newly added section.
            sections.Add(sect);

            // Also we need to register new file indexes.
            for (int i = 0; i < imagez.Length; i++)
            {
                var nind = new FileIndex();
                nind.DataLength = (uint) imagez[i].Length;
                nind.DataType = datatypes[i];
                nind.Filename = filenames[i];
                nind.Unknown = nind.DataLength;

                nind.SectionId = sectionId; // The crucial part
                indexes.Add(nind);
            }

            // And now update the IDX.
            WriteIDXEntries();

            return sect;
        }
    }
}
