package gods.screens;
import java.awt.*;

import gods.game.*;
import gods.base.GameOptions;
import gods.base.GfxPalette;
import gods.base.LevelSet;
import gods.sys.*;

public class GodsOptionsMenuScreen1 extends GameState 
{
	private int m_cursor_pos = 0;
	private final static int Y_STEP = 66;	
	private final static int Y_START = 32+Y_STEP;
	private static final int ARROW_OFFSET = 60;

	private String [] MENU_OPTIONS = new String[5];
	private StatusBar m_status_bar;
	private GameState m_next_screen = null;

	private GameOptions m_options = GameOptions.instance();
	
	private String [] m_languages;
	
	public GodsOptionsMenuScreen1()
	{
		set_fadeinout_time(1000,1000,0);

		m_languages = Localizer.get_available_languages();

		update_language_option();
		
		
	}

	 private void update_level_option()
	 {
		 LevelSet ls = m_options.get_current_level_set();
		 
		 MENU_OPTIONS[0] = Localizer.value("SET",true)+": "+ ls.get_name().toUpperCase();
		 MENU_OPTIONS[1] = Localizer.value("LEVEL",true)+": "+ ls.get_start_level();
	 }
	 
	 private void update_sound_option()
	 {
		 MENU_OPTIONS[2] = Localizer.value("SFX",true)+": "+
		 Localizer.value(m_options.get_audio_mode().toString().replace('_', ' '),true);
		 
		 
		 switch (m_options.get_audio_mode())
		 {
		 case FULL_SOUND:
		 case MUSIC_ONLY:
			 if (!Mp3Play.is_playing())
			 {
				 Mp3Play.replay();
			 }
			 break;
		 default:
			 Mp3Play.stop();
		 }
	 }
	 

	 
	 private void update_language_option()
	 {
		 String l = m_languages[m_options.get_language()];
		 
		 MENU_OPTIONS[3] = Localizer.value("LANGUAGE",true)+": "+
		 Localizer.value(l,true).toUpperCase();

		 update_level_option();
		 update_sound_option();
	 }
	 protected GameState get_next_default_screen()
	 {
		 return new GodsMainMenuScreen();
	 }
	 

	 protected void p_update()
	 {
		 if (is_fire_pressed())
		 {
			 fadeout();
		 }
		 
		 m_controls.read();
		 
		 if (m_controls.key_down)
		 {
			 
			 m_cursor_pos++;
			 if (m_cursor_pos == MENU_OPTIONS.length)
			 {
				 m_cursor_pos = 0;
			 }
			 
		 }
	
	 
		 
		 if (m_controls.key_up)
		 {

			 if (m_cursor_pos == 0)
			 {
				 m_cursor_pos = MENU_OPTIONS.length;
			 }
			 m_cursor_pos--;
		 }
		
		 
		 if (is_fadeout_done())
		 {
			 GameOptions.instance().save();

			 if (m_next_screen != null)
			 {
				 set_next(m_next_screen);
			 }
			 else
			 {
				 set_next(get_next_default_screen());
			 }
		 }
		 
		 
		 switch (m_cursor_pos)
		 {
		 case 0:
			 if (m_controls.key_left)
			 {
				 m_options.previous_level_set();
				 update_level_option();
			 }
			 else if (m_controls.key_right)
			 {
				 m_options.next_level_set();				

				 update_level_option();
			 }
			 break;
		 case 1:
			 if (m_controls.key_left)
			 {
				 m_options.prev_level_in_current_level_set();
				 update_level_option();
			 }
			 else if (m_controls.key_right)
			 {
				 m_options.next_level_in_current_level_set();
				 update_level_option();
			 }
			 break;
		 case 2:			 
			 if (m_controls.key_left)
			 {
				 m_options.prev_audio_mode();
				 update_sound_option();
			 }
			 else if (m_controls.key_right)
			 {
				 m_options.next_audio_mode();
				 update_sound_option();
			 }
			 break;

		 
		 case 3:
			 if (m_controls.key_left)
			 {
				m_options.prev_language();
			 }
			 else if (m_controls.key_right)
			 {
				 m_options.next_language();
			 }
			 update_language_option();
			 
			 break;
		 
		 case 4:
			 if (m_controls.key_left || m_controls.key_right)
			 {
			 m_next_screen = new GodsOptionsMenuScreen2();
			 fadeout();
			 }
	 }
		 m_status_bar.update(get_elapsed_time());
		 
		
		 
	 }
	
	 
	 
	 protected void p_init()
	 {
		 m_status_bar = StatusBar.instance(getWidth(),getHeight());
		 MENU_OPTIONS[4] = Localizer.value("MORE OPTIONS",true);
	 }
	 
	 protected void p_render(Graphics2D g)
	 {
		 int h = getHeight();
		 int y_limit = h-m_status_bar.get_height()-4;
		 
		 g.drawImage(m_helmet, 0, 0, getWidth(), y_limit, 
				 0,0, getWidth(), y_limit,null);
		 
		 g.setColor(Color.BLACK);
		 g.fillRect(0, y_limit, getWidth(), h);
		 
		 for (int i = 0; i < MENU_OPTIONS.length; i++)
		 {
			 GOLDEN_BIG_FONT.write_line(g,Localizer.value(MENU_OPTIONS[i]),getWidth()/2,
					 Y_START + Y_STEP * i,-2,true,true);
		 }
		 int ypos = Y_START + Y_STEP * m_cursor_pos;
		 
		 GOLDEN_BIG_FONT.write_line(g,")",ARROW_OFFSET,ypos,0,true,true);
		 GOLDEN_BIG_FONT.write_line(g,"(",getWidth()-ARROW_OFFSET,ypos,0,true,true);
		 
		 m_status_bar.render(g);
	 }
}
