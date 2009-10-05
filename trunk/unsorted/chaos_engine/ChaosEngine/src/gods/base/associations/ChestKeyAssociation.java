package gods.base.associations;

import gods.base.GfxObject;
import gods.base.LevelData;
import gods.sys.ParameterParser;

public class ChestKeyAssociation extends ObjectAssociation {

	public ChestKeyAssociation()
	{
		super(Type.Chest_Key);
		
	}
	
	@Override
	public String describe()
	{
		String key_string = make_list("or",1,get_nb_keys());
		
		return "if hero owns "+key_string+" and is over \""+get_chest().get_name()+"\" then open chest";
	}
	
	@Override
	public void parse(ParameterParser fr, LevelData ld) throws java.io.IOException
	{
		for (int i = 0; i < m_associated_objects.size(); i++)
		{
			set_object(i,ld.get_bonus(fr.readString("object")));
		}
	}
	
	public GfxObject get_chest()
	{
		return (GfxObject)get_object(0);		
	}
	
	public int get_nb_keys()
	{
		return m_associated_objects.size()-1;
	}
	public GfxObject get_key(int i)
	{
		return (GfxObject)get_object(i+1);
	}

	@Override
	public boolean is_multi() 
	{
		return true;
	}
}
