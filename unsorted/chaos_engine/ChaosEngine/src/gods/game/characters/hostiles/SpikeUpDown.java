package gods.game.characters.hostiles;

import java.awt.*;
import java.awt.image.BufferedImage;

import gods.base.*;
import gods.game.SfxSet.Sample;
import gods.game.characters.Hero;
import gods.game.characters.Hostile;
import gods.game.characters.HostileParameters;
import gods.game.characters.weapons.HeroWeapon;


public class SpikeUpDown extends Hostile
{
	@Override
	public boolean is_in_background() {
		
		return true;
	}
	@Override
	public void get_bounds(Rectangle r) 
	{
		r.x = get_x();
		r.y = m_variable_y;
		r.width = m_width;
		r.height = m_height;
	}


	private GfxFrameSet m_frame_set;
	private int m_counter;
	private int m_max_counter;
	private int m_explosion_counter;
	private GfxFrameSet  m_exploding_frame_set;
	private AnimatedFrames m_appearing_frames;
	private int m_timer, m_variable_y;
	private int m_min_height, m_max_height;
	private int m_sign, m_max_y, m_min_y;
	private static final int UPDATE_RATE = 50;
	private BufferedImage m_rod;
	

	public SpikeUpDown()
	{
		
	}
	
	@Override
	public Sample get_appearing_sound() 
	{
		return null;
	}

	@Override
	public void collision(Hero h) 
	{
		h.hurt(m_params.health_points);
	}

	@Override
	public void init(HostileParameters p)
	{
		p.health_points = 1; // force it to 1

		// only bombs destroy the spikes anyway
		
		super.init(p);
		
		m_frame_set = m_params.level.get_level_palette().lookup_frame_set(p.frame_set_name);
		m_timer = 0;
		m_counter = 0;
		
		m_min_height = m_frame_set.get_frame(2).get_height();
		m_max_height = m_frame_set.get_first_frame().get_height();
		
		m_appearing_frames = new AnimatedFrames();
		
		m_appearing_frames.init(m_params.level.get_common_palette().lookup_frame_set("bonus_taken_smoke"),
				UPDATE_RATE,AnimatedFrames.Type.REVERSE);

		m_appearing_frames.set_coordinates(this);
		
		m_variable_y = get_y(); // y stays fixed
		m_min_y = m_variable_y;
		m_max_y = m_variable_y + m_max_height;
		m_width = m_frame_set.get_width();
		m_height = m_max_height;
		m_max_counter = m_frame_set.get_nb_frames() * 2;
		m_sign = 1;

		if (m_params.total_nb_hostiles_of_wave > 1)
		{
			int cow = (m_params.location.get_width() - m_width) / (m_params.total_nb_hostiles_of_wave - 1);

			m_x += cow * p.index;
		}
		m_exploding_frame_set = m_params.level.get_common_palette().lookup_frame_set("ground_enemy_death");

		m_rod = m_frame_set.get_first_frame().toImage();
		
		m_life_state = LifeState.ALIVE;
	}
	
	public void set_life_state(LifeState s)
	{
		super.set_life_state(s);
		if (s == LifeState.EXPLODING)
		{
			m_explosion_counter = 0;
			
			m_x -= (m_exploding_frame_set.toImage().getWidth() - m_width) / 2;
		}
	}
	

	@Override
	public boolean hit(HeroWeapon hw)
	{
		boolean rval = false;
		
		if (hw == null)
		{
			set_life_state(LifeState.EXPLODING);
		}
		else
		{
			// case of bomb
			if (hw.can_destroy_spikes())
			{
				rval = super.hit(hw);
			}
		}
		return rval;
	}
	
	public void render(Graphics2D g) 
	{
		switch (m_life_state)
		{
		case ALIVE:
			g.drawImage(m_rod, (int)m_x, m_variable_y + m_min_height, get_x()+ m_width, m_max_y, 
					0, m_min_height, m_width, m_height, null);
			
			g.drawImage(m_frame_set.get_frame((m_counter/2)+1).toImage(), get_x(), m_variable_y, get_x()+ m_width, m_variable_y + m_min_height, 
					0, 0, m_width, m_min_height, null);
			if (!m_appearing_frames.is_done())
			{
				m_appearing_frames.render(g);
			}
		break;
		case EXPLODING:
			g.drawImage(m_exploding_frame_set.get_frame(m_explosion_counter/2+1).toImage(), get_x(), m_min_y, null);
			break;
		}
		
		
	}
	

	public void update(long elapsed_time) 
	{
		m_timer += elapsed_time;
		if (m_timer > UPDATE_RATE)
		{
			m_timer -= UPDATE_RATE;
			
			switch (m_life_state)
			{
			case ALIVE:
				if (!m_appearing_frames.is_done())
				{
					m_appearing_frames.update(elapsed_time);
				}
				m_height -= m_sign;
				m_variable_y += m_sign;
				if ((m_height == m_min_height) || (m_height == m_max_height))
				{
					m_sign *= -1;
				}			

				m_counter++;
				if (m_counter == m_max_counter)
				{
					m_counter = 0;
				}
				break;
			case EXPLODING:
				m_explosion_counter++;
				if (m_explosion_counter/2+1 == m_exploding_frame_set.get_nb_frames())
				{
					m_life_state = LifeState.DEAD;
				}
				break;
			}
			
		}

	}

}
