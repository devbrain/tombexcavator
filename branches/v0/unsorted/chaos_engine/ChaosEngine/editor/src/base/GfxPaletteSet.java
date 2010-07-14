package base;

import java.awt.Graphics2D;
import java.awt.Rectangle;
import java.awt.image.BufferedImage;
import java.io.IOException;
import java.util.Collection;
import java.util.Vector;
import java.util.HashSet;

import base.GfxFrameSet.Type;

import sys.MiscUtils;
import sys.ParameterParser;

public class GfxPaletteSet 
{
	public String get_block_name() 
	{
		return "PALETTE_SET";
	}
	
	public void add(GfxPalette p)
	{
		m_items.add(p);
	}
	
	public Vector<GfxPalette> get_items()
	{
		return m_items;
	}
	
	public void load(ParameterParser fr) throws IOException
	{
		m_items = new Vector<GfxPalette>();
		fr.startBlockVerify(get_block_name());
		int nb_palettes = fr.readInteger("nb_palettes");

		for (int i = 0; i < nb_palettes; i++)
		{
			GfxPalette p = new GfxPalette();
			p.load(fr.readString("name"));
			m_items.add(p);
		}
		fr.endBlockVerify();
	}
	/*public void serialize(ParameterParser fw) throws IOException 
	{
		fw.startBlockWrite(get_block_name());
		
		fw.write("nb_palettes",m_items.size());
		
		for (GfxPalette p : m_items)
		{
			fw.write("name", p.get_project_file());
		}
		
		fw.endBlockWrite();

	}*/
	
	public GfxFrameSet create_dummy_frame_set(int code)
	{
		GfxFrameSet rval = new GfxFrameSet(""+code,Type.bonus);
		BufferedImage img = new BufferedImage(16,16,BufferedImage.TYPE_INT_ARGB);
		
		MiscUtils.centered_draw_string((Graphics2D)img.getGraphics(), ""+Integer.toHexString(code),8,16);
		GfxFrame gf = new GfxFrame(rval,img,new Rectangle(0,0,img.getWidth(),img.getHeight()),0);
		
		rval.add(gf);
		// add the item in the first palette (maybe not very good but who cares?)
		m_items.elementAt(0).add(rval);
		return rval;
	}
	
	public GfxFrameSet lookup_frame_set(int code)
	{
		GfxFrameSet rval = null;
		
		for(GfxPalette gp : m_items)
		{
			rval = gp.lookup_frame_set(code);
			if (rval != null)
			{
				break;
			}
		}
		
		return rval;
	}
	public GfxFrameSet lookup_frame_set(String name)
	{
		GfxFrameSet rval = null;
		
		for(GfxPalette gp : m_items)
		{
			rval = gp.lookup_frame_set(name);
			if (rval != null)
			{
				break;
			}
		}
		
		return rval;
	}
	public Collection<GfxFrameSet> lookup_by_type(GfxFrameSet.Type t)
	{
		Collection<GfxFrameSet> rval = new HashSet<GfxFrameSet>();

		for(GfxPalette gp : m_items)
		{
			gp.lookup_by_type(t, rval);
		}
	

		return rval;
	}
	private Vector<GfxPalette> m_items;
}
