package base.layer;

import base.*;

import sys.MiscUtils;
import sys.ParameterParser;

import java.awt.*;
import java.awt.font.GlyphVector;
import java.awt.geom.Rectangle2D;
import java.awt.image.BufferedImage;
import java.io.IOException;
import java.util.Vector;

public class HeightLayer implements Describable
{
	
	protected int [][] m_data;
	protected int m_tile_width = 0;
	protected int m_tile_height = 0;
	protected int m_horiz_resolution = 0;
	protected int m_vert_resolution = 0;
	protected int m_nb_rows = 0;
	protected int m_nb_cols = 0;
	protected int m_pixel_width = 0;
	protected int m_pixel_height = 0;
	
	protected void init_grid()
	{
		m_data = new int[m_nb_cols][];
	
		for (int j = 0; j < m_nb_cols; j++)
		{
			m_data[j] = new int[m_nb_rows];
			for (int i = 0; i < m_nb_rows; i++)
			{
				m_data[j][i] = -1;
			}
		}
	}
	protected void debug(String m)
	{
		System.out.println(this.getClass().getName()+": "+m);
	}

	public void set_dimension(int nb_rows, int nb_cols, 
			boolean on_left, boolean on_top)
	{
		if ((nb_rows != m_nb_rows) || (nb_cols != m_nb_cols))
		{
			
			int old_nb_rows = m_nb_rows;
			int old_nb_cols = m_nb_cols;
			
			int [][] old_data = m_data;
			
			m_nb_rows = nb_rows;
			m_nb_cols = nb_cols;
	
			int row_offset = on_top ? old_nb_rows - nb_rows : 0;
			int col_offset = on_left ? old_nb_cols - nb_cols : 0;
						
			
			int min_rows = Math.min(old_nb_rows, nb_rows);
			int min_cols = Math.min(old_nb_cols, nb_cols);
			
			init_grid();
			
			if (old_data != null)
			{
				for (int j = 0; j < m_nb_cols; j++)
				{
					if (j < min_cols)
					{
						for (int i = 0; i < m_nb_rows; i++)
						{
							if (i < min_rows)
							{
								int j_src = j + col_offset;
								int i_src = i + row_offset;
								
								if ((j_src >= 0) && (i_src >= 0))
								{
									m_data[j][i] = old_data[j_src][i_src];
								}
							}
						}
					}
				}
			}
			
			init_dim_params();
			
		}
		
	}
	private void init_dim_params()
	{
		Dimension d = new Dimension(16,16);//palette.get_default_tile_dimension();
		
		m_tile_width = d.width;
		m_tile_height = d.height;
		m_horiz_resolution = m_tile_width;
		m_vert_resolution = m_tile_height;
		
		m_pixel_width =  m_nb_cols * m_tile_width;
		m_pixel_height = m_nb_rows * m_tile_height;
	}
	
	private void import_from_binary(Vector<BinaryLevelInfo.BinaryTileData.TileLine> vtl)
	{
		m_horiz_resolution = 16;
		m_vert_resolution = 16;
		
		m_nb_rows = vtl.size();
		m_nb_cols = vtl.firstElement().flags.length;
		
		m_pixel_width = m_nb_cols * m_horiz_resolution;
		m_pixel_height = m_nb_rows * m_vert_resolution;

		init_dim_params();
		init_grid();

		int i = 0;
		
		for (BinaryLevelInfo.BinaryTileData.TileLine tl : vtl)
		{
		  
			for (int j = 0; j < m_nb_cols; j++)
			{
				m_data[j][i] = tl.flags[j];
			}
			i++;
		}
				
			
	}
	protected HeightLayer()
	{
		
	}

	public int get_tile_width()
	{
		return m_tile_width;
	}
	
	public int get_tile_height()
	{
		return m_tile_height;
	}
	
	public HeightLayer(Vector<BinaryLevelInfo.BinaryTileData.TileLine> vtl)
	{
		import_from_binary(vtl);
	}
	
	public HeightLayer(int tile_width, int tile_height, int nb_items_per_tile_h, int nb_items_per_tile_v)
	{
		m_tile_width = tile_width;
		m_tile_height = tile_height;
		m_horiz_resolution = tile_width / nb_items_per_tile_h;
		m_vert_resolution = tile_height / nb_items_per_tile_v;
	}

	// in-editor render
	
	public BufferedImage render(BufferedImage img_arg)
	{
		BufferedImage img = img_arg;
		
		if ((img == null) || 
				(img.getWidth() < m_pixel_width) || 
				(img.getHeight() < m_pixel_height))
		{
			if ((m_pixel_width > 0) && (m_pixel_height > 0))
			{
				img = new BufferedImage(m_pixel_width,m_pixel_height,BufferedImage.TYPE_INT_ARGB);
			}
		}
		

		
		if (img != null)
		{
			Graphics2D g = (Graphics2D)img.getGraphics();

			for (int j = 0; j < m_nb_cols; j++)
			{
				
				for (int i = 0; i < m_nb_rows; i++)
				{
					int flags = m_data[j][i];
					int height = flags & 0x7;
					
					boolean stairs = (flags & 0x8) != 0;
					boolean door = (flags & 0x10) != 0;
					boolean item = (flags & 0x20) != 0;
				
					
					if (door)
					{
						g.setColor(Color.YELLOW);
					}
					else if (item)
					{
						g.setColor(Color.RED);
					}
					else
					{
						g.setColor(stairs ? Color.GREEN : Color.WHITE);
					}
					int x = j*m_tile_width;
					int y = i*m_tile_height;
					
					//g.drawRect(x, y, m_tile_width, m_tile_height);
					MiscUtils.centered_draw_string(g,""+height,x + m_tile_width/2,y+m_tile_height);
				}
			}
			
			
		}
		
		return img;

	}
	


	
	public int get_nb_rows()
	{
		return m_nb_rows;
	}
	
	public int get_nb_cols()
	{
		return m_nb_cols;
	}
	

	public void serialize(ParameterParser fw) throws IOException 
	{
		
		
	}

	public int get_column(int x)
	{
		return x / m_horiz_resolution;
	}
	
	public int get_row(int y)
	{
		return y / m_vert_resolution;
	}
	

	public int get(int x, int y)
	{
		int i = x / m_horiz_resolution;
		int j = y / m_vert_resolution;
		
		int rval = 0;
		
		if ((i < m_nb_cols) && (j < m_nb_rows) && (i >= 0) && (j >= 0))
		{
			rval = m_data[i][j];
		}
		
		return rval;
	}
	

	public void set(int x, int y,int gf)
	{
		int i = x / m_horiz_resolution;
		int j = y / m_vert_resolution;

		if ((i < m_nb_cols) && (j < m_nb_rows) && (i >= 0) && (j >= 0))
		{
			m_data[i][j] = gf;
		}
	}
	public String get_block_name() {
		// TODO Auto-generated method stub
		return "HEIGHT_LAYER";
	}
	
	
}
