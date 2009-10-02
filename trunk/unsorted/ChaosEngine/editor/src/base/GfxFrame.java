package base;

import sys.ParameterParser;

import java.awt.*;
import java.awt.image.BufferedImage;
import java.io.IOException;

public class GfxFrame implements EditorRenderable, Describable
{
	private BufferedImage m_image;
	private int m_src_x;
	private int m_src_y;
	private int m_counter;
	private GfxFrameSet m_source_set = null;
	private boolean m_has_transparence = false;
	
	public enum SymmetryType { mirror_left, flip_up, no_op_clone, no_op_same_ref }

	public boolean has_transparence()
	{
		return m_has_transparence;
	}
	

	public String toString()
	{
		return m_counter == 1 ? m_source_set.toString() : m_source_set.toString()+" ["+m_counter+"]";
	}
	

	public void serialize(ParameterParser fw) throws IOException 
	{
		fw.startBlockWrite(get_block_name());
	
		fw.write("source_x", m_src_x);
		fw.write("source_y", m_src_y);
		fw.write("width", m_image.getWidth());
		fw.write("height", m_image.getHeight());
		
		fw.endBlockWrite();
	}
	
	public GfxFrame(GfxFrame symmetric, SymmetryType st)
	{
		m_counter = symmetric.get_counter();
		m_source_set = symmetric.get_source_set();
		m_has_transparence = symmetric.has_transparence();
		
		m_src_x = 0;
		m_src_y = 0;
		int w = symmetric.get_width();
		int h = symmetric.get_height();
		
		
		BufferedImage source_image = new BufferedImage(w,h,symmetric.toImage().getType());
		
		switch(st)
		{
		case mirror_left:
		
			source_image.getGraphics().drawImage(symmetric.toImage(), 
					w, 0, 0, h, 
					0, 0, w, h, null);
			break;
		case flip_up:
			source_image.getGraphics().drawImage(symmetric.toImage(), 
					0, h, w, 0, 
					0, 0, w, h, null);
			break;
		case no_op_clone:
			// same, but use a different buffer
			m_source_set = symmetric.get_source_set();
			source_image.getGraphics().drawImage(symmetric.toImage(),0,0,null);
			break;
		case no_op_same_ref:
			// same, and use the same buffer
			m_source_set = symmetric.get_source_set();
			source_image = symmetric.toImage();
			break;
		}
		
		create_from_source(source_image,w,h,false);

	}
	
	void set_source_set(GfxFrameSet gfs)
	{
		m_source_set = gfs;
	}
	public GfxFrameSet get_source_set()
	{
		return m_source_set;
	}

	public String get_block_name()
	{
		return "GFX_FRAME";
	}
	
	public int get_width()
	{
		return m_image.getWidth();
	}
	
	public int get_height()
	{
		return m_image.getHeight();
	}
	
	public Rectangle get_bounds()
	{
		return new Rectangle(m_src_x,m_src_y,get_width(),get_height());
	}
	public GfxFrame(BufferedImage source_2x,GfxFrame frame_1x)
	{
		m_counter = frame_1x.get_counter();
		
		m_src_x = frame_1x.get_src_x() * 2;
		m_src_y = frame_1x.get_src_y() * 2;
		
		m_source_set = frame_1x.get_source_set();
		
		create_from_source(source_2x,frame_1x.get_width()*2, frame_1x.get_height()*2,true);
	}
	
	public GfxFrame(GfxFrameSet source_set, BufferedImage source_image,
			ParameterParser fr, int counter) throws IOException
	{
		m_counter = counter;
		m_source_set = source_set;
		
		fr.startBlockVerify(get_block_name());

		m_src_x = fr.readInteger("source_x");
		m_src_y = fr.readInteger("source_y");
		int w = fr.readInteger("width");
		int h = fr.readInteger("height");
		

		fr.endBlockVerify();
		
		create_from_source(source_image,w,h,false);
		
		
	}
	

	public GfxFrame(GfxFrameSet source_set,BufferedImage img,Rectangle r)
	{
		this(source_set,img,r,1);
	}
	
	public void set_coordinates(int x, int y)
	{
		m_src_x = x;
		m_src_y = y;
	}
	public BufferedImage toImage()
	{
		return m_image;
	}
	
	public int get_src_x()
	{
		return m_src_x;
	}
	public int get_src_y()
	{
		return m_src_y;
	}
	
	public void set_counter(int c)
	{
		m_counter = c;
	}
	public int get_counter()
	{
		return m_counter;
	}
	
	public GfxFrame(GfxFrameSet source_set, BufferedImage img,Rectangle r,int counter)
	{
		m_source_set = source_set;
		
		m_src_x = r.x;
		m_src_y = r.y;

		create_from_source(img,r.width,r.height,false);
		m_counter = counter;
	}
	
	private void create_from_source(BufferedImage source, int width, int height, boolean handle_transparence)
	{
		
		m_image = new BufferedImage(width,height,BufferedImage.TYPE_INT_ARGB);
		Graphics g = m_image.getGraphics();
		
		g.drawImage(source, 0, 0, m_image.getWidth(), m_image.getHeight(), 
				m_src_x, m_src_y, m_src_x+width, m_src_y+height, null);
		
		if (handle_transparence)
		{
			for (int i = 0; i < m_image.getWidth(); i++)
			{
				for (int j = 0; j < m_image.getHeight(); j++)
				{
					int rgb = m_image.getRGB(i, j);
					int red = ((rgb & 0xFF0000) >> 16);
					int green = ((rgb & 0xFF00) >> 8);
					int blue = (rgb & 0xFF);

					if ((Math.abs(red-0x61) <= 2) && 
							(Math.abs(green-0x40) <= 2) && (Math.abs(blue) <= 2))
					{
						m_has_transparence = true;
						m_image.setRGB(i, j, 0);
					}

				}
			}
		}

	}
	public void editor_render(Graphics g) 
	{
		g.drawRect(m_src_x, m_src_y, m_image.getWidth(), m_image.getHeight());
		g.drawString(""+m_counter, m_src_x+2, m_src_y+m_image.getHeight()/2);
	}
}
