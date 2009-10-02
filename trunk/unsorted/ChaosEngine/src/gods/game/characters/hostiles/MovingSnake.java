package gods.game.characters.hostiles;

import gods.game.characters.*;
import gods.game.characters.weapons.Projectile;

public class MovingSnake extends GroundMonster 
{
	
	/*@Override
	protected boolean is_vertical_way_blocked() 
	{
		int x_limit = m_right_left == 0 ? (m_x + 3*m_width/4) : (m_x + m_width/4);
		
		return m_params.level.is_vertical_way_blocked(x_limit, m_y + m_height, m_half_width);
	}*/

	public MovingSnake()
	{
		super(false,null);
	}
	
	@Override
	public void init(HostileParameters p) 
	{	
		super.init(p);
		// round to avoid collision/fall/stuck in floor problems
		
		m_width = p.level.get_grid().get_rounded_x(m_width);
		m_height = p.level.get_grid().get_rounded_y(m_height);

		// recompute falling flag
		
		m_falling = !is_vertical_way_blocked();
		
		// set score points to a fixed value
		
		m_params.score_points = 1000;
		
		// set damage points to a fixed value
		
		m_params.health_points = 15;
		
		// add offset
		
		m_y += m_height;
		m_x += m_width * (Math.random()-0.5) * 0.5;
		
		m_actual_jump.init((int)(m_width * 1.5 * Math.random()), m_height, 5);
		
		set_jump(m_actual_jump);
	}

	@Override
	public void set_life_state(LifeState s) 
	{
		if (s == LifeState.EXPLODING)
		{
			// change y so explosion is a little higher
			
			m_y -= m_exploding_frame_set.get_height() - m_height;
		}
		super.set_life_state(s);
	}

	@Override
	protected void move(long elapsed_time) 
	{
		super.move(elapsed_time);
	
		double x = m_x;
		
		if (is_in_air())
		{
			lateral_move(elapsed_time);
			
			if (is_lateral_way_blocked())
			{
				m_x = x;
			}
		}
	}

	@Override
	protected Projectile shoot() 
	{
		// do nothing
		return null;
	}

}
