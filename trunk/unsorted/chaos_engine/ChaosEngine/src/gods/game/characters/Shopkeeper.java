package gods.game.characters;


import gods.base.*;


public class Shopkeeper extends AnimatedFrames {

	private GfxFrameSet [] m_frames = new GfxFrameSet[2];
	private int m_walk_counter = 0;
	private boolean m_met;
	
	private Hero m_hero;
	
	public String get_name() {
		// TODO Auto-generated method stub
		return "shopkeeper";
	}

	public void set_name(String n) {
		// TODO Auto-generated method stub
		
	}

	public Shopkeeper(GfxPalette palette, Hero hero) 
	{
		m_frames[0] = palette.lookup_frame_set("shopkeeper_right");		
		m_frames[1] = palette.get_left_frame_set(m_frames[0]);
		m_hero = hero;
	}
	
	public void init()
	{
		int wd = m_hero.get_walk_direction();
		
		GfxFrameSet current = m_frames[1];
		if (wd < 0)
		{
			current = m_frames[0];
		}
				
		init(current,0,Type.CUSTOM);
		set_coordinates(m_hero.get_x() + wd * 320,m_hero.get_y());
		
		m_walk_counter = 0;
		m_met = false;
	}
	
	public boolean hero_met()
	{
		return m_met;
	}
	
	public void update(long elapsed_time) 
	{
		if (!m_met)
		{

			m_walk_counter++;

			if (m_walk_counter == 4)
			{
				
				next_frame(1);
				
				m_walk_counter = 0;

				int wd = m_hero.get_walk_direction();


				if (wd == 1)
				{
					if (m_x > m_hero.get_x() + m_hero.get_width())
					{
						m_x -= wd * Hero.LATERAL_STEP;
					}
					else
					{
						m_met = true;
					}
				}
				else
				{
					if (m_x + m_width < m_hero.get_x())
					{
						m_x -= wd * Hero.LATERAL_STEP;
					}
					else
					{
						m_met = true;
					}
				}
			}
		}
	}

}
