package base;

import java.io.IOException;
import java.util.*;
import java.awt.Color;
import java.awt.Graphics;
import java.awt.image.BufferedImage;

import sys.*;

public class GfxFrameSet implements EditorRenderable, Nameable<GfxFrameSet>, Modifiable
{
	public class Properties implements Describable, Cloneable
	{
		public String get_block_name() 
		{
			return "PROPERTIES";
		}
		public void serialize(ParameterParser fw) throws IOException 
		{
			fw.startBlockWrite(get_block_name());
			fw.write("name", name);
			fw.write("type",type.toString());
			fw.write("points", points);
			fw.write("update_rate", update_rate);
			fw.write("code",code.equals("") ? ParameterParser.UNDEFINED_STRING : code);
			fw.write("description", description.equals("") ? ParameterParser.UNDEFINED_STRING : description);
			fw.write("animation_type", animation_type.toString());
			fw.endBlockWrite();
			
		}
		public Properties()
		{}
		
		public void parse(ParameterParser fr) throws IOException 
		{
			fr.startBlockVerify(get_block_name());
			name = fr.readString("name");
			type = Type.valueOf(fr.readString("type"));
			points = fr.readInteger("points");
			update_rate = fr.readInteger("update_rate");
			code = fr.readString("code",true);
			description = fr.readString("description",true);
			animation_type = AnimatedFrames.Type.valueOf(fr.readString("animation_type"));
			fr.endBlockVerify();
			
		}
		
		public Properties clone() throws CloneNotSupportedException
		{
			return (Properties)super.clone();
		}
		
		public String toString()
		{
			return status_bar_value;
		}
		
		
		
		private String status_bar_value;
		
		public String name = "";
		public GfxFrameSet.Type type = Type.other;
		public int points = 0;
		public int update_rate = 0;
		public String code = ParameterParser.UNDEFINED_STRING;
		public String description = ParameterParser.UNDEFINED_STRING;
		public boolean first_appearance = true;
		public AnimatedFrames.Type animation_type = AnimatedFrames.Type.CUSTOM;
	}
	
	int distance_to_zero()
	{
		int rval = 0;
		
		if (!m_frames.isEmpty())
		{
			GfxFrame g = get_first_frame();

			rval =  (g.get_src_x()*g.get_src_x())/64 + (g.get_src_y()*g.get_src_y())*100000;
		}
		
		return rval;
	}
	public int compareTo(GfxFrameSet o)
	{
		return get_name().compareTo(o.get_name());
	}

	private Vector<GfxFrame> m_frames = new Vector<GfxFrame>();
	private boolean m_modified = false;
	private Properties m_properties = new Properties();
	
	public Properties get_properties()
	{
		return m_properties;
	}
	
	public enum Type { other, background_tile, foreground_tile,teleport_helmet,
		enemy, hero, weapon, bonus, special_bonus, open_face_door, activable, background_item,
		breakable_block }
	

	public static final Type [] TILES = { Type.background_tile, Type.foreground_tile, 
		Type.background_item, Type.breakable_block };
	public static final Type [] ALL_ITEMS = { Type.activable, Type.bonus, Type.background_item, Type.special_bonus };
	public static final Type [] BONUSES = {Type.bonus, Type.special_bonus };

	public boolean is_bonus()
	{
		boolean rval = false;

		switch (m_properties.type)
		{
		case activable:
		case bonus:
		case teleport_helmet:
			rval = true;
			break;
		}

		return rval;
	}
	public void append(GfxFrameSet source)
	{
		m_frames.addAll(source.get_frames());
		m_modified = true;
	}
	
	public boolean is_modified()
	{
		return m_modified;
	}
	
	public void set_modified(boolean m)
	{
		m_modified = m;
	}
	
	public GfxFrameSet()
	{
		m_properties = new Properties();
	}
	
	public GfxFrame get_first_frame()
	{
		return get_frame(1);
	}
	public GfxFrame get_frame(int counter)
	{
		GfxFrame rval = null;
		if (m_frames.size() >= counter)
		{
		rval = m_frames.elementAt(counter - 1);
		}
		return rval;
	}
	
	public int get_width()
	{
		return toImage().getWidth();
	}
	public int get_height()
	{
		return toImage().getHeight();
	}
	
	public BufferedImage toImage()
	{
		return m_frames.isEmpty() ? null : m_frames.elementAt(0).toImage();
		
	}

	public GfxFrameSet(GfxFrameSet symmetric, GfxFrame.SymmetryType st)
	{
		m_properties.name = symmetric.get_name();
		switch (st)
		{
		case mirror_left:
			m_properties.name = symmetric.get_name().replace("right", "left");
		case no_op_clone:
		case no_op_same_ref:
			m_properties.type = symmetric.get_type();
			
			// symmetrize frames
			
			for (GfxFrame fr : symmetric.get_frames())
			{
				GfxFrame gf = new GfxFrame(fr,st);
				gf.set_source_set(this);
				m_frames.add(gf);
			}
		break;
			
		}
		
		try {
			m_properties = symmetric.m_properties.clone();
		} catch (CloneNotSupportedException e) {
			e.printStackTrace();
		}
	}
	
	public GfxFrameSet(BufferedImage source_2x,GfxFrameSet frame_set_1x)
	{

		try {
			m_properties = frame_set_1x.m_properties.clone();
		} catch (CloneNotSupportedException e) {
			e.printStackTrace();
		}
		
		for (GfxFrame fr : frame_set_1x.get_frames())
		{
			add(new GfxFrame(source_2x,fr));
		}
	}
	
	public GfxFrameSet(String name, Type type)
	{

		m_properties = new Properties();
		m_properties.name = name;
		m_properties.type = type;
	}
	
	public String toString()
	{
		return m_properties.name+" ("+m_properties.type+")";
	}
	public void set_name(String name)
	{
		if (!m_properties.name.equals(name))
		{
			set_modified(true);
		}
		
		m_properties.name = name;
	}
	
	public void set_type(Type i)
	{
		if (i != m_properties.type)
		{
			set_modified(true);
		}
		
		m_properties.type = i;
	}
	
	public Type get_type()
	{
		return m_properties.type;
	}
	
	public GfxFrameSet(BufferedImage source,ParameterParser fr) throws java.io.IOException
	{
		fr.startBlockVerify(get_block_name());
		
		m_properties.parse(fr);
		
		
		int nb_frames = fr.readInteger("nb_frames");
		
		for (int i = 0; i < nb_frames; i++)
		{
			GfxFrame gf = new GfxFrame(this,source,fr,i+1);
			add(gf);
		}
		
		
		fr.endBlockVerify();
		
	}
	
	public int get_nb_frames()
	{
		return m_frames.size();		
	}
	
	public Vector<GfxFrame> get_frames()
	{
		return m_frames;
	}
	
	public String get_block_name()
	{
		return "FRAME_SET";
	}
	public void serialize(ParameterParser fw) throws java.io.IOException
	{
		fw.startBlockWrite(get_block_name());
		
		m_properties.serialize(fw);

		fw.write("nb_frames",m_frames.size());
		
		for (GfxFrame gf : m_frames)
		{
			gf.serialize(fw);
		}
		
		
		fw.endBlockWrite();
		
		set_modified(false);
	}
	
	public String get_name()
	{
		return m_properties.name;
	}
	
	
	public void remove(GfxFrame gf)
	{		
		if (gf != null)
		{
			if (m_frames.remove(gf))
			{
				set_modified(true);
				int i = 1;
				for (GfxFrame g : m_frames)
				{
					g.set_counter(i++);
				}
			}
		}
	}
	public void add(GfxFrame gf)
	{
		gf.set_counter(m_frames.size()+1);
		if (m_frames.add(gf))
		{
			set_modified(true);
		}
	}
	

	public void editor_render(Graphics g)
	{
		Color c = Color.BLUE;

		switch (m_properties.type)
		{
		case background_tile:
			c = Color.RED;
		case foreground_tile:
			c = Color.GREEN;
			break;
		case enemy:
			c = Color.ORANGE;
			break;
		case hero:
			c = Color.CYAN;
			break;
		case weapon:
			c = Color.DARK_GRAY;
			break;
		}
		
		g.setColor(c);
		for (GfxFrame gf : m_frames)
		{
			gf.editor_render(g);
		}
	}
	
	// x & y can be inaccurate
	
	public GfxFrame lookup_frame(int x, int y)
	{
		GfxFrame rval = null;
		
		for (GfxFrame gf : m_frames)
		{
			if (gf.get_bounds().contains(x,y))
			{
				rval = gf;
				break;
			}
		}
		
		return rval;
	}
}
