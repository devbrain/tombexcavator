package base;

import sys.*;

import java.util.*;
import java.awt.image.BufferedImage;
import java.awt.*;


public class GfxPalette implements EditorRenderable, EditableData
{
	private static HashMap<String, GfxPalette> m_cache = new HashMap<String, GfxPalette>();
	
	private boolean m_modified = false;
	
	private boolean m_view_defined = true;
	private boolean m_view_undefined = true;

	private BufferedImage m_1x = null;
	private LinkedList<GfxFrameSet> m_frame_set_list = new LinkedList<GfxFrameSet>();
	private HashMap<GfxFrameSet,GfxFrameSet> m_symmetrical_frame_set_list = new HashMap<GfxFrameSet,GfxFrameSet>();
	private String m_project_file;
	
	public static Color BACKGROUND_COLOR = Color.MAGENTA;
	
	private int m_tile_default_width, m_tile_default_height;
	
	public void set_tile_dimension(Dimension d)
	{
		m_tile_default_width = d.width;
		m_tile_default_height = d.height;
	}
	public void export(String s)
	{
		// not implemented yet
	}
	
	public void toggle_view_defined()
	{
		m_view_defined = !m_view_defined;
	}
	public void toggle_view_undefined()
	{
		m_view_undefined = !m_view_undefined;
	}
	
	public void editor_render(Graphics g) 
	{
		if (m_view_defined)
		{
			for (GfxFrameSet gfs : m_frame_set_list)
			{
				gfs.editor_render(g);
			}
		}
		else
		{
			g.setColor(Color.BLACK);
			
			for (GfxFrameSet gfs : m_frame_set_list)
			{
				for (GfxFrame gf : gfs.get_frames())
				{
					Rectangle r = gf.get_bounds();
					
					g.fillRect(r.x, r.y, r.width, r.height);
				}
			}
			
		}
	}
	
	public Collection<GfxFrameSet> get_frame_sets()
	{
		return m_frame_set_list;
	}
	public void add(GfxFrameSet gfs)
	{
		m_frame_set_list.add(gfs);
		m_modified = true;
	}
	
	public void remove(GfxFrameSet gfs)
	{
		m_frame_set_list.remove(gfs);
	}
	

	public GfxFrameSet get_left_frame_set(GfxFrameSet other)
	{
		GfxFrameSet rval = m_symmetrical_frame_set_list.get(other);
		
		// cache to avoid too much image processing during game
		
		if (rval == null)
		{
			rval = new GfxFrameSet(other,GfxFrame.SymmetryType.mirror_left);
			m_symmetrical_frame_set_list.put(other, rval);
		}
		
		return rval;
	}
	
	public GfxFrame get_left_frame(GfxFrame other)
	{
		GfxFrame rval;
		
		rval = new GfxFrame(other,GfxFrame.SymmetryType.mirror_left);
		
		return rval;
	}
	
	public GfxFrameSet lookup_frame_set(int x, int y)
	{
		GfxFrameSet rval = null;
		
		for (GfxFrameSet gfs : m_frame_set_list)
		{
			if (gfs.lookup_frame(x,y) != null)
			{
				rval = gfs;
				break;
			}
		}
		
		return rval;
	}
	
	public GfxFrameSet lookup_frame_set(String name)
	{
		GfxFrameSet rval = null;
		
		for (GfxFrameSet gfs : m_frame_set_list)
		{
			if (gfs.get_name().equals(name))
			{
				rval = gfs;
				break;
			}
		}
		
		return rval;
	}
	public GfxFrameSet lookup_frame_set(int code)
	{
		GfxFrameSet rval = null;
		String code_str = "0x" + Integer.toHexString(code);
		
		for (GfxFrameSet gfs : m_frame_set_list)
		{
			String [] values = gfs.get_properties().code.split(",");
			for (int i = 0; i <values.length; i++)
			{
				if (values[i].equalsIgnoreCase(code_str))
				{
					rval = gfs;
					break;
				}
			}
			if (rval != null) break;
		}
		
		return rval;
	}
	
	private Dimension m_default_tile_dimension = null;
	
	public Dimension get_default_tile_dimension()
	{
	if (m_default_tile_dimension == null)
	{
	m_default_tile_dimension = new Dimension(32,32);
	}
	
	return m_default_tile_dimension;
	}
	
	public Collection<GfxFrameSet> lookup_by_type(GfxFrameSet.Type [] matching_types)
	{
		Set<GfxFrameSet> rval = new TreeSet<GfxFrameSet>();
		
		for (GfxFrameSet.Type t : matching_types)
		{
			lookup_by_type(t,rval);
		}
		return rval;
	}
	public Collection<GfxFrameSet> lookup_by_type(GfxFrameSet.Type matching_type)
	{
		Set<GfxFrameSet> rval = new TreeSet<GfxFrameSet>();
		lookup_by_type(matching_type,rval);
		
		return rval;
	}
	
	public void lookup_by_type(GfxFrameSet.Type matching_type,Collection<GfxFrameSet> list)
	{
		
		for (GfxFrameSet gfs : m_frame_set_list)
		{
			if (gfs.get_type() == matching_type)
			{
				list.add(gfs);
			}
		}
				
	}
	
	private void reset()
	{
		m_modified = false;
		m_frame_set_list.clear();
		m_project_file = null;
		m_1x = null;
	}
	public void new_project(String image_file)
	{
		reset();
		
		m_project_file = DirectoryBase.rework_path(image_file, DirectoryBase.get_tiles_path(), true, true);
	}
	
	public BufferedImage get_1x_image()
	{
		if ((m_1x == null) && (m_project_file != null))
		{
			m_1x = ImageLoadSave.load_png(DirectoryBase.get_tiles_path() + m_project_file,Color.MAGENTA);
		}
		return m_1x;
	}
	
	public String get_project_file()
	{
		return m_project_file;
	}
	
	public static void clear_cache()
	{
		m_cache.clear();
		System.gc();
	}
	
	public void load(String project_file) throws java.io.IOException
	{		

		reset();

		set_project_file(project_file);

		String s = get_project_filename();

		ParameterParser fr = ParameterParser.open(s);

		fr.startBlockVerify(get_block_name());

		m_tile_default_width = fr.readInteger("default_tile_width");
		m_tile_default_height = fr.readInteger("default_tile_height");
		
		int nb_frame_sets = fr.readInteger("nb_frame_sets");

		BufferedImage img = null;

		img = get_1x_image();

		for (int i = 0; i < nb_frame_sets; i++)
		{			
			GfxFrameSet gfs = new GfxFrameSet(img,fr);
			add(gfs);
		}

		fr.endBlockVerify();

		m_modified = false;
	}

	public String get_block_name()
	{
		return "GFX_PALETTE";
	}
	public boolean save() throws java.io.IOException
	{
		boolean rval = false;
		String s = get_project_filename();
		ParameterParser fw = ParameterParser.create(s);

		try
		{
			serialize(fw);

			fw.close();
			fw = null;
			
			rval = true;
		}
		catch (RuntimeException e)
		{
			if (fw != null)
			{
				fw.close();
			}
		}

		m_modified = false;
		
		return rval;
	}
	
	public void serialize(ParameterParser fw) throws java.io.IOException
	{
		fw.startBlockWrite(get_block_name());
		
		fw.write("default_tile_width",m_tile_default_width);
		fw.write("default_tile_height",m_tile_default_height);
		
		fw.write("nb_frame_sets", m_frame_set_list.size());
		
		for (GfxFrameSet gfs : m_frame_set_list)
		{
			gfs.serialize(fw);
		}
		fw.endBlockWrite();
	}
	
	public boolean is_modified()
	{
		return m_modified;
	}
	
	public void set_modified(boolean m)
	{
		m_modified = m;
	}
	
	public void set_project_file(String project_file)
	{
		m_project_file = DirectoryBase.rework_path(project_file, 
				DirectoryBase.get_tiles_path(), true, true);
	}
	
	public Rectangle compute_gfx_frame(int x, int y)
	{
		BufferedImage img = get_1x_image();
		Rectangle rval = null;
		
		if (img != null)
		{
			int x_pos = (x / m_tile_default_width) * m_tile_default_width;
			int y_pos = (y / m_tile_default_height) * m_tile_default_height;
			
			rval = new Rectangle(x_pos,y_pos,m_tile_default_width,m_tile_default_height);
		}
		
		return rval;
	}
	
	private String get_project_filename()
	{
		return DirectoryBase.get_tiles_path() + m_project_file + DirectoryBase.GFX_OBJECT_SET_EXTENSION;
	}
}
