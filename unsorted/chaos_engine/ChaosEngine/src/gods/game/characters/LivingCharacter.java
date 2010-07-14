package gods.game.characters;

import gods.base.*;

public abstract class LivingCharacter extends Locatable implements Renderable
{
	protected LifeState m_life_state;
	
	public void set_life_state(LifeState s)
	{
		m_life_state = s;
	}
	public LifeState get_life_state()
	{
		return m_life_state;
	}
	
	public void set_name(String n) {
		
		
	}	
	public enum LifeState { STAND_BY, DELAYED, APPEARING, EXPLODING, ALIVE, DEAD };
	

	public abstract int get_health(boolean instantaneous);
	public abstract int get_max_health();
	
	public abstract void handle_health(long elapsed_time);
}
