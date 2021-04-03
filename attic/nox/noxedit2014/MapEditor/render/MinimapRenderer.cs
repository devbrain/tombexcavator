/*
 * MapEditor
 * Пользователь: AngryKirC
 * Copyleft - Public Domain
 * Дата: 15.11.2014
 */
using System;
using System.Drawing;
using System.Runtime.InteropServices;
using NoxShared;

namespace MapEditor.render
{
	/// <summary>
	/// Класс, обеспечивающий быструю отрисовку миникарты (стен и плиток пола)
	/// </summary>
	public class MinimapRenderer : BitmapShader
	{
		private readonly Map map;
		
		public MinimapRenderer(Bitmap bitmap, Map theMap) : base(bitmap)
		{
			map = theMap;
		}
		
		public void DrawMinimap(int minimapZoom)
		{
			if (locked)
			{
				// black array
				byte[] bitarray = new byte[bitData.Stride * bitData.Height];
				Marshal.Copy(bitData.Scan0, bitarray, 0, bitarray.Length);
				
				int pxI, x, y; Map.Tile tile; int tilePlotZoom = minimapZoom + 2;
				// draw tiles
				foreach (Point loc in map.Tiles.Keys)
            	{
					x = loc.X * minimapZoom;
					y = loc.Y * minimapZoom;
					if (x < 0 || y < 0) return;
					tile = map.Tiles[loc];
					
					for (int rx = x; rx < x + tilePlotZoom; rx++)
					{
						for (int ry = y; ry < y + tilePlotZoom; ry++)
						{
							pxI = ((ry * bitData.Width) + rx) * 4;
							if (pxI + 3 < bitarray.Length)
							{
								bitarray[pxI] = tile.col.B;
								bitarray[pxI + 1] = tile.col.G;
								bitarray[pxI + 2] = tile.col.R;
								bitarray[pxI + 3] = 0xFF;
							}
						}
					}
				}
				// draw walls
				foreach (Point loc in map.Walls.Keys)
				{
					x = loc.X * minimapZoom;
					y = loc.Y * minimapZoom;
					for (int rx = x; rx < x + minimapZoom; rx++)
					{
						for (int ry = y; ry < y + minimapZoom; ry++)
						{
							pxI = ((ry * bitData.Width) + rx) * 4;
							if (pxI + 3 < bitarray.Length)
							{
								bitarray[pxI] = 0xFF;
								bitarray[pxI + 1] = 0xFF;
								bitarray[pxI + 2] = 0xFF;
								bitarray[pxI + 3] = 0xFF;
							}
						}
					}
				}
				// copy BitmapData
				Marshal.Copy(bitarray, 0, bitData.Scan0, bitarray.Length);
			}
		}
	}
}
