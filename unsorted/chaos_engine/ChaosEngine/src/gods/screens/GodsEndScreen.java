package gods.screens;
import java.awt.*;
import java.awt.image.BufferedImage;
import java.io.File;

import gods.base.DirectoryBase;
import gods.base.GameOptions;
import gods.base.GfxPalette;
import gods.game.GameState;
import gods.game.StatusBar;
import gods.sys.*;


public class GodsEndScreen extends GameState {

	private BufferedImage m_background;
	private boolean m_first_message_printed = false;
	private StatusBar m_status_bar;
	
	public GodsEndScreen(GfxPalette common_palette)
	{
		set_fadeinout_time(250, 250,0);
		m_background = common_palette.lookup_frame_set("winner").get_first_frame().toImage();
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
		 if (is_fadeout_done())
		 {
			 Mp3Play.stop();
			 
			 set_next(get_next_default_screen());
		 }
		 
		 if (!m_first_message_printed)
		 {
			 m_status_bar.print("WIN_TEXT_2");
			 m_first_message_printed = true;
		 }
		 
		 m_status_bar.update(get_elapsed_time());
		 
	 }
	 
	 protected void p_init()
	 {
		 m_status_bar = StatusBar.instance(getWidth(),getHeight());

		 if (GameOptions.instance().get_music_state())
		 {
			 play_music("misc" + File.separator + "win.mp3");
		 }
	 }
	 

	 protected void p_render(Graphics2D g)
	 {
		 g.drawImage(m_background,0,0,null);
		 m_status_bar.render(g);
	 }
}
