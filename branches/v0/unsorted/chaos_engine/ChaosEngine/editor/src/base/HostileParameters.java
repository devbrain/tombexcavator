package base;

import sys.ParameterParser;

import java.io.IOException;


public class HostileParameters implements Describable, Cloneable
{
	public enum Direction { Left, Right }
	
	// not used by flying creatures
	
	public enum AttackDistance { None, // ground monsters: move back & forth
		Close, // ground monster attacks when close to the player
		Closer, // ground monster attacks when less close to the player
		Always // ground monster attacks hero
		}
	
	private void debug(String m)
	{
		if (Debug.active)
		{
			if (location != null)
			{
				System.out.println(this.getClass().getName()+" ("+location+"): "+m);
			}
			else
			{
				System.out.println(this.getClass().getName()+": "+m);
			}
		}
	}
	
	public String get_block_name() 
	{
		return "HOSTILE_PARAMETERS";
	}
	
	public HostileParameters clone() throws CloneNotSupportedException
	{
		HostileParameters rval = (HostileParameters)super.clone();
		
		
		
		if (location != null) rval.location = (ControlObject)location.clone();
		if (frame_set_name != null) rval.frame_set_name = new String(frame_set_name);
		
		return rval;
	}

	
	
	public HostileParameters(ControlObject co, LevelData ld)
	{
		level = ld;
		location = co;
	}
	
	public HostileParameters(ParameterParser fr, LevelData ld) throws IOException
	{
		fr.startBlockVerify(get_block_name());
		
		debug("loading general params");
		
		level = ld;
		location = ld.get_control_object(fr.readString("location"));
		frame_set_name = fr.readString("frame_set_name",true);
		fires = fr.readBoolean("fires");
	
		
		health_points = fr.readInteger("health_points");
		direction = Direction.valueOf(fr.readString("direction"));
		attack_distance = AttackDistance.valueOf(fr.readString("attack_distance"));
		move_speed = Speed.valueOf(fr.readString("speed"));		
		steals_toy = fr.readBoolean("steals_toy");
		jump_height = JumpHeight.valueOf(fr.readString("jump_height"));
		jump_width = JumpWidth.valueOf(fr.readString("jump_width"));
		jump_threshold = JumpThreshold.valueOf(fr.readString("jump_threshold"));
		
		attach_to_level = fr.readInteger("attach_to_level");
		
		fr.endBlockVerify();
		debug("OK");
	}
	
	public void serialize(ParameterParser fw) throws IOException 
	{
		fw.startBlockWrite(get_block_name());
		
		fw.write("location",location.get_name());
		fw.write("frame_set_name",frame_set_name);
		fw.write("fires",fires);
		
		fw.write("health_points",health_points);
		fw.write("direction", direction.toString());
		fw.write("attack_distance", attack_distance.toString());
		fw.write("speed", move_speed.toString());
		
		fw.write("steals_toy", steals_toy);
		fw.write("jump_height", jump_height.toString());
		fw.write("jump_width", jump_width.toString());
		fw.write("jump_threshold", jump_threshold.toString());
			
		fw.write("attach_to_level",attach_to_level);	
		fw.endBlockWrite();
		
	}
	


	public LevelData level;
	
	public static final int [] GROUND_SPEED_TABLE = {2,6,7,10,13};
	public static final int [] FLYING_SPEED_TABLE = {3,8,10,12,14};
	public static final int [] SHOOT_SPEED_TABLE = {3,8,10,14,18};
	public static final int [] PERIOD_TABLE = {6400,3200,2400,1600,800}; // milli-seconds
	public static final int [] JUMP_HEIGHT_TABLE = {0,4,7};
	public static final int [] JUMP_WIDTH_TABLE = {1,2,4};
	public static final int [] JUMP_WALL_THRESHOLD_TABLE = {1,3,5};
	
	public enum Speed { Very_Slow, Slow, Normal, Fast, Very_Fast }
	public enum JumpHeight { None, Normal, High }
	public enum JumpWidth { Short, Normal, Wide }
	public enum JumpThreshold { Close, Normal, Far }
	
	public ControlObject location;
	public String frame_set_name;

	public Direction direction = Direction.Right;
	public boolean fires = false;
	
	public int health_points = 1;
	public int attach_to_level = 1;
	//public boolean shoot_shield = false;
	public Speed move_speed = Speed.Normal;
	public AttackDistance attack_distance = AttackDistance.None;
	public JumpHeight jump_height = JumpHeight.Normal;
	public JumpWidth jump_width = JumpWidth.Normal;
	public JumpThreshold jump_threshold = JumpThreshold.Normal;
	public boolean steals_toy = false;
	
}