package gods.game;

//import com.golden.gamedev.*;
//import com.golden.gamedev.engine.*;

import java.awt.*;
import java.awt.event.KeyEvent;
import java.io.*;

import micromod.*;

import gods.base.Debug;
import gods.base.DirectoryBase;
import gods.base.GameOptions;
import gods.sys.*;

import gods.screens.*;


public class GodsGame extends GameEngine
{
	private MicromodPlayer m_player = null;
	private GameState m_state = null;
	private int m_screen_height;

	public void finish()
	{
		GameOptions.instance().save();

		super.finish();
	}
	
	public GodsGame(Rectangle window_bounds, Rectangle useful_bounds, boolean full_screen) throws Exception
  {
		

	  m_screen_height = window_bounds.height;
	  
	  setup(window_bounds,useful_bounds,Localizer.value("window title")+
		" - "+Localizer.value("version")+" "+Version.STRING_VALUE,
		KeyEvent.VK_F10,
		Color.BLACK,
		full_screen,
		true);
  }


	

	
	public int get_music_pos()
	{
		int r = 0;

		if (m_player != null)
		{
			r = m_player.get_position();
		}
		return r;
	}
	public void stop_music()
	{
		if (m_player != null)
		{
			m_player.stop();
			m_player = null;
		}

	}

	public boolean is_music_playing()
	{
		return m_player != null;
	}
	public void load_music(String music_file)
	{
		load_music(music_file,0,false);
	}
	public void load_music(String music_file, int initial_position, boolean loop)
	{
		if (GameOptions.instance().get_music_state())
		{
			String path = gods.base.DirectoryBase.get_mod_path() + music_file;
		  try
		  {
			  stop_music();
			  m_player = new MicromodPlayer(new File(path).toURI().toURL());
			  m_player.setloop(loop);
			  m_player.set_start_offset(initial_position);
			  new Thread(m_player).start();
		  }
		  catch (Exception e)
		  {
			  //m_state.show_error(path+": "+e.getMessage());
		  }
	  }
	  else
	  {
		  stop_music();
	  }
  }

  public void initResources()
  {
	  GameOptions go = GameOptions.instance();
	  

    System.setProperty("user.dir",gods.base.DirectoryBase.get_root());
 
    hide_cursor();

     //setFPS(go.get_max_fps_value());
    

    if (go.with_intro)
    {
    	m_state = new JotdScreen();
    	//m_state = new GodsHistoryScreen(true);
    	//m_state = new GodsCreditsScreen();
    }
    else
    {
    	
    	if (GameOptions.instance().direct_game)
    	{
    		try 
    		{
    			m_state = GodsLevel.create(null);
    			
    			// TEMP
    			//StatusBar sb = new StatusBar(get_width(), get_height());
    			//gods.game.characters.Hero h = new gods.game.characters.Hero(level.get_data(),null,sb,level.get_sfx_set(),level.get_view_bounds());
    			//h.level_init(null, null);
    			//h.set_health(18);
    			//h.add_money(40000);

    			//m_state = new Shop(GodsClassicLevel1.SHOP_CONTENTS_W3_START,h,32,level.get_data().get_common_palette(),
    			//	level.get_data().get_level_palette(),sb,null);

    		} catch (Exception e) {
    			// TODO Auto-generated catch block
    			e.printStackTrace();
    		}	
    	}
    	else
    	{
    		m_state = new GodsMainMenuScreen();
    	}
		
  

    }

    m_state.init(this);

  }

  public void render(Graphics2D g)
  {
	  if (m_screen_height > 0)
	  {
		  // first time
		  g.setColor(Color.BLACK);
		  g.fillRect(0,0,get_width(),m_screen_height);
		  m_screen_height = 0;
	  }
	  m_state.render(g);
  }

  public void update(long elapsed)
  {
	 
	  GameState s = m_state.update(elapsed);

	  if (s != null)
	  {
		  // screen change
		  m_state = s;
		  m_state.init(this);
	  }
  }
  /*
  public void OLDupdate(long elapsed)
  {
	  // if elapsed time is too big (slow cpu)
	  // call it with smaller values in order to avoid
	  // "tunnel" effects with weapons, trains, and cars
	  
	  long elapsed_to_go = elapsed;
	  GameState s = null;
	  
	  while ((elapsed_to_go > MAX_ELAPSED_TIME) && (s == null))
	  {
		  s = m_state.update(elapsed_to_go);
		  elapsed_to_go -= MAX_ELAPSED_TIME;
	  }
	  if ((s == null) && (elapsed_to_go > 0))
	  {
		  s = m_state.update(elapsed_to_go);
		  
	  }
	  if (s != null)
	  {
		  // screen change
		  m_state = s;
		  m_state.init(this);
	  }
  }
*/
 
  
 
  public boolean return_pressed()
  {
	  return false; // TODO keyPressed(KeyEvent.VK_ENTER);
  }

  
  
  /****************************************************************************/
/***************************** START-POINT **********************************/
/****************************************************************************/

  private static void usage()
  {
	  System.out.println("Gods -Deluxe- valid options: ");
	  System.out.println("");
	  System.out.println("   -full-screen: runs the game in full screen");
	  System.out.println("   -no-intro: skips introduction");
	  System.out.println("   -direct-game: runs with last selected level");
	  System.out.println("   -no-level-cache: disables level caching to save memory");
	  
  }
   public static void main(String[] args) throws Exception
   {
	   DirectoryBase.env_check();
	   
	   GameOptions opts = GameOptions.instance();
	   
	   Rectangle useful_bounds = new Rectangle(640,400);
	   Rectangle window_bounds = (Rectangle)useful_bounds.clone();

	   int i = 0;
	   while (i < args.length)
     {
    	 String arg = args[i];
    	 /*String nextarg="";
    	 if (i < args.length - 1)
    	 {
    		 nextarg = args[i+1];
    	 }*/
    	 
    	 if (arg.charAt(0) == '-')
    	 {
       		 if (arg.equalsIgnoreCase("-direct-game"))
    		 {
       			 opts.with_intro = false;
       			 opts.direct_game = true;
    		 }
     		 else if (arg.equalsIgnoreCase("-full-screen"))
    		 {
    			 opts.full_screen = true;
    			 window_bounds.height = 480;
    		 }
    		 else if (arg.equalsIgnoreCase("-no-intro"))
    		 {
    			 opts.with_intro = false;
    		 }
      		 else if (arg.equalsIgnoreCase("-debug"))
    		 {
    			 Debug.active = true;
    		 }
      		 else if (arg.equalsIgnoreCase("-no-level-cache"))
    		 {
    			 opts.with_level_cache = false;
    		 }
      		 else if (arg.equalsIgnoreCase("-unlock-levels"))
      		 {
      			 opts.unlock_levels = true;
      		 }
     		 else
    		 {
    			 usage();
    			 System.exit(1);
    		 }
    	 }
    	 i++;
     }
	   GodsGame scgame = new GodsGame(window_bounds,useful_bounds,opts.full_screen);

	   scgame.start();
   }

}
