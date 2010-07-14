package base;

import java.awt.image.BufferedImage;
import java.awt.Rectangle;
import java.util.LinkedList;

import base.BinaryLevelInfo.BinaryEnemyPositions;
import base.BinaryLevelInfo.BinaryItemPositions;

import sys.*;


public class Enemy extends GfxObject
{

	public static final String UNNAMED_PREFIX = "_unnamed_";
	
	public int compareTo(Object arg0) 
	{
		return toString().compareTo(arg0.toString());
	}
	
	protected void warn(String m)
	{
		if (Debug.active)
		{
			System.out.println(this.getClass().getName()+": Warning: "+m);
		}
	}	
	private GfxFrameSet m_frame_set;
	private int m_current_frame = 1;
	
	private int m_nb_frames = 1;
	

		
	public BufferedImage toImage()
	{
		BufferedImage rval = null;
		if (m_frame_set != null)
		{
			GfxFrame gf = m_frame_set.get_frame(m_current_frame);
			if (gf != null)
			{
				rval = gf.toImage();
			}
		}
		return rval;
	}
	
	
	public void set_current_frame(int df)
	{
		if (df <= m_nb_frames)
		{
			m_current_frame = df;
		}
	}
	
	public int get_current_frame()
	{
		return m_current_frame;
	}
	
	

	
	
	@Override
	public boolean set_x(double x)
	{
		double old_x = m_x;
		
		if (m_x_resolution == 1)
		{
			m_x = x;
		}
		else
		{
			m_x = Math.round(x / m_x_resolution) * m_x_resolution;
		}
		
		return (old_x != m_x);
	}
	
	@Override
	public boolean set_y(double y)
	{
		double old_y = m_y;
		
		if (m_y_resolution == 1)
		{
			m_y = y;
		}
		else
		{
			m_y = Math.round(y / m_y_resolution) * m_y_resolution;
		}
		
		return (old_y != m_y);
	}
	
	public void set_visible(boolean v)
	{
		
		m_visible = v;
	}


	
	
	public GfxFrameSet.Properties get_properties()
	{
		return m_frame_set.get_properties();
	}
	public GfxFrameSet get_source_set()
	{
		return m_frame_set;
	}
	
	public String toString()
	{
		String rval = null;
		
		if (m_name.equals(""))
		{
			rval = "unnamed " + get_class_name();
		}
		else
		{
			rval = m_name + " (" + get_class_name() + ")";
		}
		if (!m_visible)
		{
			rval += " (hidden)";
		}
		return rval;
	}
	
	public String get_name()
	{
		return m_name;
	}
	
	public void set_name(String name)
	{
		m_name = name;
	}
	
	public String get_class_name()
	{
		return m_frame_set.get_name();
	}

	
	public Enemy(int x, int y, int x_resolution, int y_resolution, String name, 
			GfxFrameSet frame_set)
	{
		super(x_resolution,y_resolution);
		
		m_frame_set = frame_set;
		m_name = name;
		
		if (m_frame_set != null)
		{
			m_nb_frames = m_frame_set.get_nb_frames();
			BufferedImage im = toImage();
			m_width = im.getWidth();
			m_height = im.getHeight();
		}
		
		
		set_coordinates(x, y);
		
	}
		

	
	public void update(long elapsed_time, LevelData level_data, Rectangle animation_bounds)
	{
		
	}
	protected void debug(String m)
	{
		System.out.println(this.getClass().getName()+": "+m);
	}
	
	public Enemy(BinaryEnemyPositions.ItemPosition ip, int x_resolution, int y_resolution, 
			GfxPaletteSet palette) throws java.io.IOException
			{		
		super(x_resolution,y_resolution);
		
		m_name = ""+ip.enemy_type+ip.comment;
		
		

		m_visible = true;


		m_frame_set = null;//palette.lookup_frame_set(ip.item_code);

		m_x = ip.x * m_x_resolution;
		m_y = ip.y * m_y_resolution;

		m_nb_frames = 0;

		if (m_frame_set == null)
		{
			//debug("Frame set with code 0x"+Integer.toHexString(ip.item_code)+ " not found in palette");
			m_frame_set = palette.create_dummy_frame_set(ip.enemy_type);
			m_width = 16;
			m_height = 16;
		}
		else
		{
			m_nb_frames = m_frame_set.get_nb_frames();

			BufferedImage im = toImage();
			if (im != null)
			{
				m_width = im.getWidth();
				m_height = im.getHeight();
			}
		}

			}
	

	
	public String get_block_name() 
	{
		return "ENEMY";
	}

	public void serialize(ParameterParser fw) throws java.io.IOException
	{
		fw.startBlockWrite(get_block_name());
		

		fw.endBlockWrite();
	}


}
