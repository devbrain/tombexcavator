package base.layer;

import java.awt.Graphics;
import java.io.IOException;
import java.util.*;

import base.ControlObject;
import base.GfxFrame;
import base.GfxFrameSet;
import base.GfxPaletteSet;
import base.HostileParameters;
import base.HostileParametersSet;
import sys.ParameterParser;

public class ControlObjectLayer extends SparseLayer<ControlObject> {



	private HashMap<ControlObject.Type,Object> m_view_filter = new HashMap<ControlObject.Type,Object>();
	private HostileParametersSet m_hostile_params;
	private GfxPaletteSet m_level_palette;
	
	private void init_filter()
	{
		// default: view everything
		
		for (ControlObject.Type t : ControlObject.Type.values())
		{
			m_view_filter.put(t,t);
		}
	}
	protected ControlObject add_item(ParameterParser fr, int x_res, int y_res, GfxPaletteSet palette) throws IOException 
	{
		ControlObject rval = new ControlObject(fr,x_res,y_res);
		m_object_list.add(rval);
		return rval;
		
	}

	public void add_filter(ControlObject.Type t)
	{
		m_view_filter.put(t,t);
	}
	
	public void remove_filter(ControlObject.Type t)
	{
		m_view_filter.remove(t);		
	}
	
	public ControlObject get(int x, int y) 
	{
		ControlObject rval = null;
		
		LinkedList<ControlObject> lco = get_items(x, y);
		if (!lco.isEmpty())
		{
			for (ControlObject co : lco)
			{
				if (m_view_filter.get(co.get_type()) != null)
				{
					rval = co;
					break;
				}
			}
		}
		
		return rval;
	}
	public ControlObject get(String name) {
		
		return (ControlObject)super.get(name);
	}

	public ControlObject set(int x, int y, String name, ControlObject.Type cot)
	{
		ControlObject go = get(x,y);
		if (go != null)
		{
			m_object_list.remove(go);
		}
		if (name != null)
		{
			go = new ControlObject(x,y,m_x_resolution,m_y_resolution,name,cot);
			m_object_list.add(go);
		}
		
		return go;
	}
	protected void load(ParameterParser fr) throws IOException {
		super.load(fr, null);
	}

	public ControlObjectLayer(int x_res, int y_res)
	{
		super(x_res,y_res);
		
		init_filter();
	}
	public ControlObjectLayer(ParameterParser fr,int x_res, int y_res) throws IOException
	{
		super(x_res,y_res);
		load(fr);
		
		init_filter();
	}
	
	public void update(long elapsed_time)
	{
		
	}
	
	public void render(java.awt.Graphics2D g) {
		// do nothing
		
	}
	
	public void set_hostile_parameters(HostileParametersSet hwps, GfxPaletteSet lp)
	{
		m_hostile_params = hwps;
		m_level_palette = lp;
	
	}
	public Collection<ControlObject> get_items(ControlObject.Type [] tl)
	{
		LinkedList<ControlObject> rval = new LinkedList<ControlObject>();
		
		for (ControlObject.Type t : tl)
		{
			get_items(t,rval);
		}
		
		return rval;
		
	}
	public void get_items(ControlObject.Type t, Collection<ControlObject> outlist)
	{
		for (ControlObject go : m_object_list)
		{
			if (go.get_type() == t)
			{
				outlist.add(go);
			}
		}
	}
	

	public String get_block_name() {
		
		return "CONTROL_OBJECT_LAYER";
	}

	public void editor_render(Graphics g) 
	{
		
		for (ControlObject co : m_object_list)
		{
			if (m_view_filter.get(co.get_type()) != null)
			{
				boolean render_outline = true;
			
				if (co.get_type() == ControlObject.Type.Enemy)
				{
				// render the hostiles if possible
				
					HostileParameters hp = m_hostile_params.get(co);
					if (hp != null)
					{
						render_outline = false;
						
						// draw the hostile
						
						GfxFrameSet gfs = m_level_palette.lookup_frame_set(hp.frame_set_name);
						
						if (gfs != null)
						{
							GfxFrame gf = gfs.get_first_frame();
							
							if (hp.direction == HostileParameters.Direction.Right)
							{
								gf = new GfxFrame(gf,GfxFrame.SymmetryType.mirror_left);
							}
							g.drawImage(gf.toImage(),co.get_x(),co.get_y(),null);					
						}
						
						
					}
				}
				if (render_outline)		;
				co.editor_render(g);
			}
		}
	}

}
