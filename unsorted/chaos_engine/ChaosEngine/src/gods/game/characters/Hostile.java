package gods.game.characters;

import gods.base.*;
import gods.game.SfxSet.Sample;
import gods.game.characters.weapons.*;
import java.awt.Rectangle;

public abstract class Hostile extends LivingCharacter
{
	public abstract boolean is_in_background();
		
	public HostileParameters get_params()
	{
		return m_params;
	}
	
	protected HostileParameters m_params;
	protected Rectangle m_work_rectangle = new Rectangle();
	protected Rectangle m_work_rectangle_2 = new Rectangle();
	//private TreeSet<HeroWeapon> m_received_blows = new TreeSet<HeroWeapon>();
	protected boolean m_frozen = false;
	protected boolean m_appearing_animation = true;
	protected double m_appearing_x, m_appearing_y;
	protected void debug(String message)
	{
		if (Debug.active)
		{
		System.out.println(this.getClass().getName()+" ("+toString()+"): "+message);
		}
	}
	
	public boolean is_in_screen(int margin)
	{
		boolean rval = false;
		
		if (margin == 0)
		{
			// optimization
			
			rval = m_params.view_bounds.contains(get_x_center(),get_y_center());
		}
		else
		{
			m_work_rectangle.x = m_params.view_bounds.x - margin;
			m_work_rectangle.y = m_params.view_bounds.y - margin;
			m_work_rectangle.width = m_params.view_bounds.width + margin * 2;
			m_work_rectangle.height = m_params.view_bounds.height + margin * 2;
			
			rval = m_work_rectangle.contains(get_x_center(),get_y_center());
		}
		return rval;
	}
	
	
	protected boolean may_shoot()
	{
		return is_in_screen(m_width);
	}
	
	public String toString()
	{
		return get_name();
	}
	public void handle_health(long elapsed_time)
	{
		
	}

	public ControlObject get_location()
	{
		return m_params.location;
	}
	
	public Sample get_exploding_sound() 
	{
		return Sample.explosion_ground;
	}
	
	protected Sample get_appearing_sound() 
	{
        // stereo
		int dx = get_x() - m_params.hero.get_x();

		return (dx < 0 ? Sample.hostile_appearing_left : Sample.hostile_appearing_right);
	}		
	
	public int get_max_health()
	{
		return m_params.max_health_points;
	}
	
	public int get_health(boolean instantaneous)
	{
		return m_params.health_points;
	}
	
	public HostileWaveParameters get_dropped_hostile()
	{
		HostileWaveParameters rval = m_params.hostile_to_drop;
		
		if (rval != null)
		{
			ControlObject co = rval.location;
			co.set_coordinates(this,true);
			
			m_params.hostile_to_drop = null;
		}
		
		return rval;
	}
	public void set_frozen(boolean frozen)
	{
		m_frozen = frozen;
	}
	public GfxObject get_dropped_item()
	{
		GfxObject rval = m_params.object_to_drop;
		
		if (rval != null)
		{
			rval.set_coordinates(this,true);
			m_params.object_to_drop = null;
		}
		
		return rval;
	}
	
	public GfxObject get_held_item()
	{
		GfxObject rval = m_params.object_held;
		
		if (rval != null)
		{
			rval.set_coordinates(this,true);
			m_params.object_held = null;
		}
		
		return rval;
	}
	

	
	public String get_name()
	{
		return m_params.location.get_name();
	}
	
	private static final int MIN_DAMAGE = 3;
	private static final int MAX_DAMAGE = 34;
	
	public void collision(Familiar f)
	{
		int familiar_damage = 1;
		f.hurt(familiar_damage);
		if (f.has_power_claws())
		{
			familiar_damage *= 2;
		}
		m_params.health_points -= familiar_damage;
		if (m_params.health_points <= 0)
		{
			die();
		}

	}
	
	public boolean collision_test(Rectangle other)
	{
		get_bounds(m_work_rectangle);
		return (other.intersects(m_work_rectangle));
	}
	public void collision(Hero h) 
	{
		// compute actual damage divisor (make up for the boosted hostile health values,
		// except in first level where everything amounts to divisions by 1, so it's would
		// have been a lot simpler but it is not, unless someone gives me the correct
		// algorithm to compute player damage, health points, damage points when weapon
		// hits (possibly many times) an enemy...
		
		int damage_divisor = 
			(m_params.level.get_damage_divisor() * m_params.health_enhancement_times_4) / 4;
		
		// damage is divided by damage divisor, dependent of the current level
		
		int max_hostile_damage = m_params.max_health_points / damage_divisor;
		int max_damage = MAX_DAMAGE / m_params.level.get_damage_divisor();
		
		int damage = (int)(Math.ceil((double)m_params.health_points / damage_divisor));

		if (damage > max_damage)
		{
			// very tough monster: make it proportional to maximum damage
			damage = (damage * max_damage)/max_hostile_damage;
		}
		
		h.hurt(Math.max(MIN_DAMAGE,damage));
		h.add_score(m_params.score_points, false);
		die();
	}	
		
	public int get_score_points()
	{
		return m_params.score_points;
	}
	
	public void init(HostileParameters p)
	{
		m_params = p;
		
		//m_appearing_animation = true; //p.view_bounds.contains(m_x,m_y);
		
		if (p.appearing_delay == 0)
		{
			m_life_state = m_appearing_animation ? LifeState.APPEARING : LifeState.ALIVE;
		}
		else
		{
			m_life_state = LifeState.DELAYED;
		}

		GfxObject o = p.object_to_drop;
		
		// try to remove object if was in the level data
		// (will be re-inserted on hostile death)
		
		if (o != null)
		{
			p.level.remove_object(o);
		}
				
		m_x = p.location.get_x();
		m_y = p.location.get_y();
		m_appearing_x = m_x;
		m_appearing_y = m_y;
		
		// default width and height, most of the time overridden
		
		m_width = p.location.get_width();
		m_height = p.location.get_height();
		
		// increase points value according to speed, may_fire, etc...
		// this is empiric and not verified
		//
		// the verified fact is that number of shots required to kill the
		// hostile times 100 equals the score (roughly)
		
		int base_points = p.score_points;
		
		
		switch (p.fire_type)
		{
		case None:
			break;
		case Straight:
			p.score_points += base_points;
			break;
		case Directional:
			p.score_points += base_points;
			break;
		}
		
		if (p.original_health_points > 1)
		{
			p.score_points += base_points * (p.original_health_points - 1);
		}
	}
	
	public void die()
	{
		hit(null);
	}
	
	/*protected void disable_weapon(HeroWeapon hw)
	{
		m_received_blows.add(hw);
	}*/
	
	public boolean hit(HeroWeapon hw)
	{		
		if (m_life_state == LifeState.ALIVE)
		{
			if (hw == null)
			{
				m_params.health_points = 0;
				set_life_state(LifeState.EXPLODING);
			}
			else
			{
				double damage = hw.get_power();

				m_params.health_points -= damage;
				
				if (m_params.health_points <= 0)
				{
					set_life_state(LifeState.EXPLODING);
				}				
			
				hw.set_state(Projectile.State.HURTING_HOSTILE);				
			}
		}
		return true;
	}
}
