package base;

import base.associations.ObjectAssociation;
import sys.ParameterParser;

import java.io.IOException;
import java.util.*;

public class ObjectAssociationSet implements Describable 
{
	private HashMap<String,ObjectAssociation> m_associations = new HashMap<String,ObjectAssociation>();
	
	public String get_block_name() {
		
		return "OBJECT_ASSOCIATION_SET";
	}

	ObjectAssociation add(ObjectAssociation o)
	{
		return m_associations.put(o.get_name(), o);
	}
	
	void remove(ObjectAssociation o)
	{
		m_associations.remove(o.get_name());
	}
	

	public ObjectAssociationSet()
	{
		
	}
	
	public ObjectAssociationSet(ParameterParser fr, LevelData ld) throws IOException
	{
		fr.startBlockVerify(get_block_name());
		
		int nb_associations = fr.readInteger("nb_associations");
		
		for (int i = 0; i < nb_associations; i++)
		{
			ObjectAssociation oa = ObjectAssociation.create(fr, ld);
			if (oa != null)
			{
				add(oa);
			}
		}
		fr.endBlockVerify();
		
	}
	public ObjectAssociation get(String name)
	{
		return m_associations.get(name);
	}
	public Collection<ObjectAssociation> items()
	{
		return m_associations.values();
	}
	
	public void serialize(ParameterParser fw) throws IOException 
	{
		fw.startBlockWrite(get_block_name());
	
		fw.write("nb_associations", m_associations.size());
		
		for (ObjectAssociation o : items())
		{
			o.serialize(fw);
		}
		
		fw.endBlockWrite();
	}

}
