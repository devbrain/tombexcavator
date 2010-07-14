package base.layer;

import base.*;
import base.BinaryLevelInfo.BinaryEnemyPositions;
import base.BinaryLevelInfo.BinaryItemPositions;

import sys.ParameterParser;

import java.awt.Graphics;
import java.awt.Rectangle;
import java.awt.image.BufferedImage;
import java.io.IOException;

import java.util.*;

public class EnemyObjectLayer extends SparseLayer<Enemy> 
{
	private boolean m_show_visible = true;
	private boolean m_show_hidden = true;
	private LevelData m_levelData = null;
	
	public void load(BinaryLevelInfo.BinaryEnemyPositions bip, GfxPaletteSet palette) throws IOException 
	{	

		
		for (BinaryLevelInfo.BinaryEnemyPositions.ItemPosition ip : bip.items)
		{
			add_item(ip,m_x_resolution,m_y_resolution,palette);
			
		}
		
		
	}
	public Collection<GfxObject> get_items(GfxFrameSet.Type [] types)
	{
		LinkedList<GfxObject> rval = new LinkedList<GfxObject>();
		
		for (GfxFrameSet.Type t : types)
		{
			get_items(t,rval);
		}
		
		return rval;
	}
	public Collection<GfxObject> get_items(GfxFrameSet.Type [] types, boolean visible)
	{
		LinkedList<GfxObject> rval = new LinkedList<GfxObject>();
		
		for (GfxFrameSet.Type t : types)
		{
			get_items(t,rval, visible);
		}
		
		return rval;
	}
	
	public void get_items(GfxFrameSet.Type t, Collection<GfxObject> outlist, boolean visible)
	{
		for (GfxObject go : m_object_list)
		{
			if ((go.is_visible() == visible) && (go.get_source_set().get_type() == t))
			{
				outlist.add(go);
			}
		}
		
	}

	public void get_items(GfxFrameSet.Type t, Collection<GfxObject> outlist)
	{
		for (GfxObject go : m_object_list)
		{
			if (go.get_source_set().get_type() == t)
			{
				outlist.add(go);
			}
		}
	}
	
	public void set_view_filter(boolean show_visible, boolean show_hidden)
	{
		m_show_visible = show_visible;
		m_show_hidden = show_hidden;
	}
	
	public void editor_render(Graphics g)
	{
		// do nothing
	}
	/*@Override
	protected Enemy add_item(ParameterParser fr, int x_res, int y_res, GfxPaletteSet palette) throws IOException
	{
		GfxObject rval = new GfxObject(fr,x_res,y_res,palette);
		m_object_list.add(rval);
		return rval;
	}*/

	protected Enemy add_item(BinaryEnemyPositions.ItemPosition ip, int x_res, int y_res, GfxPaletteSet palette) throws IOException
	{
		Enemy rval = new Enemy(ip,x_res,y_res,palette);
		m_object_list.add(rval);
		return rval;
	}

	private Rectangle m_work_rectangle = new Rectangle();
	
	public void update(long elapsed_time)
	{
		// compute a slightly wider rectangle
		Rectangle r = m_levelData.get_view_bounds();
		int tw = m_levelData.get_grid().get_tile_width();
		int th = tw * 2;
		
		m_work_rectangle.x = r.x - tw;
		m_work_rectangle.y = r.y - th;
		m_work_rectangle.width = r.width + 2*tw;
		m_work_rectangle.height = r.height + 2*th;
		
		for (GfxObject go : m_object_list)
		{
			go.update(elapsed_time,m_levelData,m_work_rectangle);
		}
	}

	public void render(java.awt.Graphics2D g) 
	{
		for (GfxObject go : m_object_list)
		{
			BufferedImage bi = go.toImage();
			if (bi != null)
			{
				if ((m_show_hidden) && (!go.is_visible()))
				{
					go.draw_image(g,bi);
				}
				if ((m_show_visible) && (go.is_visible()))
				{
					go.draw_image(g,bi);
				}
				
			}
		}
	}

	public String get_block_name() 
	{		
		return "GFX_OBJECT_LAYER";
	}
	
	public EnemyObjectLayer(ParameterParser fr, GfxPaletteSet palette, 
			int x_resolution, int y_resolution, LevelData levelData) throws IOException
	{
		this(x_resolution,y_resolution);
		load(fr,palette);
		m_levelData = levelData;
	}
	
	public EnemyObjectLayer(int x_resolution, int y_resolution)
	{
		super(x_resolution,y_resolution);
	}
	
	public Enemy get(String name)
	{
		return (Enemy)super.get(name);
	}
	
	public Enemy get(int x, int y)
	{
		LinkedList<Enemy> lgo = get_items(x,y);
		Enemy rval = null;
		
		if (!lgo.isEmpty())
		{
			rval = lgo.getFirst();
		}
		return rval;
	}
	
	public Enemy set(int x, int y, String name, GfxFrameSet frame_set)
	{
		Enemy go = get(x,y);
		if (go != null)
		{
			m_object_list.remove(go);
		}
		if (frame_set != null)
		{
			go = new Enemy(x,y,m_x_resolution,m_y_resolution,name,frame_set);
			m_object_list.add(go);
		}
		
		return go;
	}
	@Override
	protected Enemy add_item(ParameterParser fr, int x_res, int y_res,
			GfxPaletteSet palette) throws IOException {
		// TODO Auto-generated method stub
		return null;
	}
}
