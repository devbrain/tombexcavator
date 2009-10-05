package base.layer;


import base.*;
import sys.*;

import java.awt.image.BufferedImage;
import java.io.IOException;
import java.util.*;
import java.awt.*;

public class TileGrid extends GridLayer
{
	private int m_leftover_counter = 0;
	private GfxPalette m_palette = null;
	
	

	public String get_block_name() 
	{		
		return "TILE_GRID";
	}
	
	public GfxPalette get_palette()
	{
		return m_palette;
	}
	
	public TileGrid(Vector<BinaryLevelInfo.BinaryTileData.TileLine> vtl, GfxPalette palette)
	{
		m_palette = palette;
		import_from_binary(vtl);
		
	}
	public TileGrid(ParameterParser fr, GfxPalette palette) throws IOException
	{
		m_leftover_counter = 0;
		m_palette = palette;
		load(fr,palette);
	}
	

	public TileGrid(int tile_width, int tile_height, GfxPalette palette)
	{
		super(tile_width, tile_height, 1, 1);
		m_palette = palette;
		m_leftover_counter = 0;
	}
	

	public int get_rounded_x(double x)
	{
		int rval = (int)(Math.round(x / m_horiz_resolution) * m_horiz_resolution);
				
		return rval;
	}
	public int get_rounded_y(double y)
	{
		int rval = (int)(Math.round(y / m_vert_resolution) * m_vert_resolution);
				
		return rval;
	}

	public int get_rounded_y(double y, boolean ceil)
	{
		double rval = ceil ? Math.ceil(y / m_vert_resolution) : Math.floor(y / m_vert_resolution);
			
		return (int)(rval * m_vert_resolution);
	}
	
	public int get_rounded_x(double x, boolean ceil)
	{
		double rval = ceil ? Math.ceil(x / m_horiz_resolution) : Math.floor(x / m_horiz_resolution);
		
		return (int)(rval * m_horiz_resolution);
	}
	
 	
 	

	private GfxFrame find_matching_tile(BufferedImage tile_buffer, Collection<GfxFrameSet> gfs_list,
			int rgb_tolerance, boolean monochrome)
	{		
		int idx=0;
		int transparent = GfxPalette.BACKGROUND_COLOR.getRGB() | 0xFF000000;
		
		for (GfxFrameSet gfs : gfs_list)
		{	
			for (GfxFrame gf : gfs.get_frames())
			{
				BufferedImage bi = gf.toImage();
				
				boolean same = true;
				
				if ((bi.getWidth() >= tile_buffer.getWidth()) && (bi.getHeight() >= tile_buffer.getHeight()))
				{
					for (int i = 0; i < tile_buffer.getWidth() && same; i++)
					{
						for (int j = 0; j < tile_buffer.getHeight() && same; j++)
						{
							try
							{
								int rgb_buffer = tile_buffer.getRGB(i, j);
								int rgb_tile = bi.getRGB(i, j);

								// transparency test first
								
								same = (rgb_buffer == transparent) && (rgb_tile == 0);
								
								if (!same)
								{
									if (!monochrome)
									{
										int mask = 0xff;

										for (int k = 0; k < 3 && same; k++)
										{
											int shift = k*8;
											int c1 = ((rgb_buffer & mask) >> shift);
											int c2 = ((rgb_tile & mask) >> shift);
											same = (Math.abs(c1-c2) <= rgb_tolerance);

											mask = mask << 8;							
										}
									}
									else
									{
										// try to match using only gray values
										int gray_tile = MiscUtils.get_gray_average_value(rgb_tile);
										int gray_buffer = MiscUtils.get_gray_average_value(rgb_buffer);

										same = Math.abs(gray_tile - gray_buffer) < rgb_tolerance;
									}

								}
							}
							catch (ArrayIndexOutOfBoundsException e)
							{
								System.out.println(gf.toString()+": out of bounds: "+i+","+j);
								throw(e);
							}
						}
					}
					idx++;

					if (same)
					{
						return gf;
					}
				}
			}
		}
		
		return null;
	}
	
	

	private void import_from_binary(Vector<BinaryLevelInfo.BinaryTileData.TileLine> vtl)
	{
		m_horiz_resolution = 16;
		m_vert_resolution = 16;
		
		m_nb_rows = vtl.size();
		m_nb_cols = vtl.firstElement().index.length;
		
		m_pixel_width = m_nb_cols * m_horiz_resolution;
		m_pixel_height = m_nb_rows * m_vert_resolution;

		debug("nb_cols = "+m_nb_cols+", nb_rows = "+m_nb_rows);

		init_grid();

		int i = 0;
		
		for (BinaryLevelInfo.BinaryTileData.TileLine tl : vtl)
		{
		  
			for (int j = 0; j < m_nb_cols; j++)
			{
				int idx = tl.index[j];
				
				GfxFrameSet gfs = m_palette.lookup_frame_set(""+idx);
				
				if (gfs != null)
				{
					m_data[j][i] = gfs.get_first_frame();
				}
				
			}
			i++;
		}
				
	}
	public BufferedImage import_from_image(BufferedImage map_image, GfxPalette palette, int rgb_tolerance)
	{		
		BufferedImage leftover_tiles = null;
		
		
		m_pixel_width = (map_image.getWidth() / m_tile_width) * m_tile_width;
		m_pixel_height = (map_image.getHeight() / m_tile_height) * m_tile_height;
		
		m_nb_cols = map_image.getWidth() / m_horiz_resolution;
		m_nb_rows = map_image.getHeight() / m_vert_resolution;

		debug("nb_cols = "+m_nb_cols+", nb_rows = "+m_nb_rows);

		init_grid();
		m_leftover_counter = 0;

		Collection<GfxFrameSet> gfs = palette.lookup_by_type(GfxFrameSet.TILES);
		
		Collection<BufferedImage> leftovers = new LinkedList<BufferedImage>();
		
		
		BufferedImage tile_buffer = null;
		
		for (int i = 0; i < m_nb_cols; i++)
		{
			int sx1 = i * m_horiz_resolution;
			int sx2 = sx1 + m_tile_width;
			
			for (int j = 0; j < m_nb_rows; j++)
			{
				int sy1 = j * m_vert_resolution;
				int sy2 = sy1 + m_tile_height;
				
				if (tile_buffer == null)
				{
					tile_buffer = new BufferedImage(m_tile_width,m_tile_height,BufferedImage.TYPE_INT_RGB);
				}
								
				tile_buffer.getGraphics().drawImage(map_image, 0, 0, m_tile_width, m_tile_height, 
						sx1, sy1, sx2, sy2, null);
				
				GfxFrame gf = null;
				
				// increase rgb tolerance progressively to avoid matching the wrong tile
				// this way, tolerance can be high without a risk of mismatch
				
				int tolerance = 0;
				int tolerance_step = rgb_tolerance / 4;
				
				do
				{
					gf = find_matching_tile(tile_buffer, gfs, tolerance, false);
					tolerance += tolerance_step;
				}
				while ((gf == null) && (tolerance <= rgb_tolerance));
				
				
				if (gf != null)
				{
					m_data[i][j] = gf;
				}
				else
				{
					gf = find_matching_tile(tile_buffer, gfs, rgb_tolerance/2, true);

					if (gf != null)
					{
						m_data[i][j] = gf;
					}
					else
					{
						debug("unknown tile at x="+sx1+", y="+sy1);
						GfxFrameSet gfs_leftovers = new GfxFrameSet("leftover_"+m_leftover_counter,GfxFrameSet.Type.background_tile);

						m_leftover_counter++;

						gf = new GfxFrame(gfs_leftovers,tile_buffer,
								new Rectangle(0,0,m_tile_width,m_tile_height));

						gfs_leftovers.add(gf);

						gfs.add(gfs_leftovers);

						m_data[i][j] = gf;

						// tile not found: put in in the leftovers list
						leftovers.add(tile_buffer);
						tile_buffer = null; // so it is not written over
					}

				}
			}
		}
		
		int nb_leftovers = leftovers.size();
		
		if (nb_leftovers > 0)
		{
			int nb_pics_per_col = Math.min(nb_leftovers, 3);
			int nb_pics_per_row = (nb_leftovers / nb_pics_per_col);
			int w = nb_pics_per_row * m_tile_width;
			int h = nb_pics_per_col * m_tile_height;
			
			leftover_tiles = new BufferedImage(w,h,
					BufferedImage.TYPE_INT_RGB);
			
			Graphics g = leftover_tiles.getGraphics();
			
			g.fillRect(0,0,leftover_tiles.getWidth(),leftover_tiles.getHeight());
			
			int x = 0, y = 0;
			
			for (BufferedImage bi : leftovers)				
			{
				g.drawImage(bi, x, y, null);
				x += m_tile_width;
				if (x >= w)
				{
					x = 0;
					y += m_tile_height;
				}
			}
		}
		
		return leftover_tiles;
	}
	


	
}