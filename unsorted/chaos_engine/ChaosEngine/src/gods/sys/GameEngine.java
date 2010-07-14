package gods.sys;

import java.awt.*;
import java.awt.image.BufferedImage;
import java.awt.image.VolatileImage;

import javax.swing.JRootPane;


public abstract class GameEngine 
{
	private int m_max_fps = 50;
	private static final int WAIT_MILLIS = 10;
	
	private Frame m_window;
	private Canvas m_canvas;
	private Rectangle m_useful_bounds;
	private Graphics2D m_graphics;
	private VolatileImage m_buffer;
	private int m_width,m_height;
	private boolean m_cursor_state = true;
	private UserInputListener m_key_listener = null;
	private int m_exit_key;
	private int m_old_key_code = 0;
	
	private static final GraphicsDevice GD = GraphicsEnvironment.getLocalGraphicsEnvironment().getDefaultScreenDevice();
	private static final GraphicsConfiguration GC = GD.getDefaultConfiguration();
	
	public abstract void initResources();

	public abstract void render(Graphics2D arg0);

	public abstract void update(long arg0);
	
	
	public void hide_cursor()
	{
		set_cursor_state(false);
	}
	public void show_cursor()
	{
		set_cursor_state(true);
	}
	
	public void finish()
	{
		System.exit(0);
	}
	
	
	public boolean is_key_pressed(int key_code)
	{
		boolean rval = (m_old_key_code != key_code);
		boolean key_match = m_key_listener.key_code == key_code;
		
		if (rval)
		{
			rval = key_match;
			if (rval)
			{
				m_old_key_code = key_code;
			}			
		}
		else
		{
			if (!key_match)
			{
				m_old_key_code = 0;
			}
		}
		return rval;
	}
	
	public boolean is_key_down(int key_code)
	{		
		return m_key_listener.key_table[key_code & 0xff];
	}
	
	private void set_cursor_state(boolean flag)
	{
		if(m_cursor_state != flag)
		{
			m_cursor_state = flag;
			if(!m_cursor_state)
			{				
				BufferedImage bufferedimage = new BufferedImage(1,1, BufferedImage.TYPE_INT_ARGB);
				m_window.setCursor(Toolkit.getDefaultToolkit().createCustomCursor(bufferedimage, 
						new Point(0, 0), "empty"));
			}
			else
			{
				m_window.setCursor(Cursor.getDefaultCursor());
			}
		}
	}
	public int get_width()
	{
		return m_useful_bounds.width;
	}
	
	public int get_height()
	{
		return m_useful_bounds.height;
	}
	public Rectangle get_display_bounds()
	{
		return m_useful_bounds;
	}
	
	public void setup(Rectangle window_bounds, Rectangle useful_bounds, String title, int exit_key, 
			Color background, boolean full_screen_requested, boolean double_buffering)
	{
		boolean full_screen = full_screen_requested && GD.isFullScreenSupported();
		
		m_exit_key = exit_key;
		
		m_useful_bounds = useful_bounds;
		
		m_width = window_bounds.width;
		m_height = window_bounds.height;
				
		m_window = new Frame(title,GC);
		
	
		m_canvas = new Canvas(GC);		
		m_canvas.setSize(m_width, m_height);
		m_canvas.setIgnoreRepaint(true);
		m_canvas.setBackground(background);
		
		m_window.setIgnoreRepaint(true);
		m_key_listener = new UserInputListener();
		m_canvas.addKeyListener(m_key_listener);
		
		
		m_window.add(m_canvas);
		
		m_window.addWindowListener(new WindowCloseListener());

		if (full_screen)
		{		      
			m_window.setUndecorated(true);
			//m_window.dispose();
			GD.setFullScreenWindow(m_window);
			if(!GD.isDisplayChangeSupported())
			{
				GD.setFullScreenWindow(null);
				m_window.setUndecorated(false);
				full_screen = false;
			}
			else
			{
				// find best display mode matching resolution
				
				DisplayMode [] dms = GD.getDisplayModes();
				DisplayMode best_dm = null;
				
				for (DisplayMode dm : dms)
				{
					if ((dm.getHeight() == m_height) && (dm.getWidth() == m_width))
					{
						if ((best_dm == null) || (dm.getBitDepth() > best_dm.getBitDepth()))
						{
							best_dm = dm;
						}								
					}						
				}
				if (best_dm != null)
				{
					GD.setDisplayMode(best_dm);
				}
			}
		}
		m_window.setVisible(true);
		
		if (!full_screen)
		{
			Dimension screenSize = Toolkit.getDefaultToolkit().getScreenSize();

			Insets insets = m_window.getInsets();
			m_window.setVisible(false);

			m_window.setSize(m_width + insets.left + insets.right,m_height + insets.top + insets.bottom);
			m_window.setResizable(false);

			Dimension frameSize = m_window.getSize();
			
			if (frameSize.height > screenSize.height) 				
			{
				frameSize.height = screenSize.height;
			}
			
			if (frameSize.width > screenSize.width) 
			{
				frameSize.width = screenSize.width;
			}
			m_window.setLocation((screenSize.width - frameSize.width) / 2, (screenSize.height - frameSize.height) / 2);
		}

		if (double_buffering)
		{
			m_buffer = GC.createCompatibleVolatileImage(m_useful_bounds.width,m_useful_bounds.height);
			
			m_graphics = (Graphics2D)m_buffer.getGraphics();
		}
		else
		{
			m_graphics = (Graphics2D)m_canvas.getGraphics();
		}
	    
		
		
		m_window.setVisible(true);
		

		m_canvas.requestFocus();
	}

    public void start()
    {
    	long millis_interval = 1000 / m_max_fps;
    	
    	long clock1;
    	long clock2;
    	long elapsed_time = millis_interval;
    	long accumulated = 0;
    	
    	// init the resources
    	
    	initResources();
    	
    	// main loop
    	
   		clock1 = System.currentTimeMillis();
   		
  
   		while(true)
    	{
   			accumulated += millis_interval;
   			
   			if (is_key_down(m_exit_key))
   			{
   				finish();
   			}
   			
    		update(elapsed_time);
    		
    		if (accumulated >= 0)
    		{
    			// reset all alpha channel & clip properties before calling
    			// custom render
    			
    			m_graphics.setComposite(AlphaComposite.SrcOver);
    			m_graphics.setClip(m_useful_bounds);
   			
    			// call custom render method
    			
     			render(m_graphics);

    			if (m_buffer != null)
    			{
    				m_canvas.getGraphics().drawImage(m_buffer,0,0,null);
     			}
    		}
    		
    		clock2 = System.currentTimeMillis();
    		
    		elapsed_time = clock2 - clock1;
    		
     		accumulated -= elapsed_time;
    		
    		
       		try 
    		{
        		if (accumulated > WAIT_MILLIS)
        		{
        			Thread.sleep(WAIT_MILLIS);
        			accumulated -= WAIT_MILLIS;
        		}
    		} 
    		catch (InterruptedException e) 
    		{


    		}
       		clock2 = System.currentTimeMillis();
       		
       		elapsed_time = clock2 - clock1;
       		
       		clock1 = clock2;
       		
       		// safety to avoid too much delay between 2 updates
       		
       		if (elapsed_time > WAIT_MILLIS * 20)
       		{
       			elapsed_time = WAIT_MILLIS * 20;
       			accumulated = 0;
       		}

    	}
    }
}
