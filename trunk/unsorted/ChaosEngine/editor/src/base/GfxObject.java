package base;

import java.awt.image.BufferedImage;
import java.awt.Rectangle;
import java.util.LinkedList;

import base.BinaryLevelInfo.BinaryItemPositions;

import sys.*;


public class GfxObject extends Locatable implements LayerObject<Object>, Comparable<Object>
{

	public static final String UNNAMED_PREFIX = "_unnamed_";
	
	private String m_comment = "";
	
	public String get_comment()
	{
		return m_comment;
	}
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
	private LinkedList<GfxObject> m_linked_objects = null;
	
	protected int m_x_resolution, m_y_resolution;
	private GfxFrameSet m_frame_set;
	protected String m_name;
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
	
	

	
	public LinkedList<GfxObject> get_linked_objects()
	{
		return m_linked_objects;
	}
	
	public void link_object(GfxObject go)
	{
		if (m_linked_objects == null)
		{
			m_linked_objects = new LinkedList<GfxObject>();
		}
		
		m_linked_objects.add(go);
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

	protected GfxObject(int x_resolution, int y_resolution)
	{
		m_x_resolution = x_resolution;
		m_y_resolution = y_resolution;
	}
	
	public GfxObject(int x, int y, int x_resolution, int y_resolution, String name, 
			GfxFrameSet frame_set)
	{
		this(x_resolution,y_resolution);
		
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
	public GfxObject(BinaryItemPositions.ItemPosition ip, int x_resolution, int y_resolution, 
			GfxPaletteSet palette) throws java.io.IOException
			{		

		m_x_resolution = x_resolution;
		m_y_resolution = y_resolution;
		m_name = ""+ip.item_code+ip.comment;
		
		

		m_visible = true;


		m_frame_set = palette.lookup_frame_set(ip.item_code);

		m_x = ip.x * m_x_resolution;
		m_y = ip.y * m_y_resolution;

		m_nb_frames = 0;

		if (m_frame_set == null)
		{
			debug("Frame set with code 0x"+Integer.toHexString(ip.item_code)+ " not found in palette");
			m_frame_set = palette.create_dummy_frame_set(ip.item_code);
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
	
	public GfxObject(ParameterParser fr, int x_resolution, int y_resolution,GfxPaletteSet palette) throws java.io.IOException
	{		

		m_x_resolution = x_resolution;
		m_y_resolution = y_resolution;

		fr.startBlockVerify(get_block_name());

		m_name = fr.readString("name");

		if (m_name.equals("_unnamed_"))
		{
			m_name = "";			
		}

		m_visible = fr.readBoolean("visible");

		m_x = fr.readInteger("x");
		m_y = fr.readInteger("y");		



		String frame_set = fr.readString("frame_set",true);

		m_frame_set = palette.lookup_frame_set(frame_set);

		m_nb_frames = 0;

		if (m_frame_set == null)
		{
			System.out.println("Frame set "+frame_set+ " not found in palette");
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

		fr.endBlockVerify();
			}
	
	public String get_block_name() 
	{
		return "OBJECT";
	}

	public void serialize(ParameterParser fw) throws java.io.IOException
	{
		fw.startBlockWrite(get_block_name());
		
		if (m_name.equals(""))
		{
			fw.write("name", "_unnamed_");			
		}
		else
		{
			fw.write("name", m_name);
		}
		
		fw.write("visible", m_visible);
		
		fw.write("x", get_x());
		fw.write("y", get_y());
		
		if (m_frame_set != null)
		{
			fw.write("frame_set",m_frame_set.get_name());
		}
		else
		{
			fw.write("frame_set", ParameterParser.UNDEFINED_STRING);
		}
		
		fw.endBlockWrite();
	}


}
