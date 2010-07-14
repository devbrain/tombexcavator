package gods.base;

import gods.sys.ParameterParser;

import java.io.IOException;
import java.util.Vector;

public class ShopContents implements Describable
{
	public String get_block_name() {
		return "SHOP_CONTENTS";
	}

	public ShopContents()
	{
		
	}
	
	public ShopContents(ParameterParser fr) throws IOException 
	{
		fr.startBlockVerify(get_block_name());
		
		int nb_items = fr.readInteger("nb_items");
		
		fr.readVector("items", m_items, nb_items);
		
		fr.endBlockVerify();
	}
	public void serialize(ParameterParser fw) throws IOException 
	{
		fw.startBlockWrite(get_block_name());
		
		fw.write("nb_items", m_items.size());
		
		fw.write("items", m_items);
		
		fw.endBlockWrite();
	}

	private Vector<String> m_items = new Vector<String>();
	
	public void add_item(String item)
	{
		m_items.add(item);
	}
	
	public boolean remove_item(String item)
	{
		return m_items.remove(item);
	}

	public Vector<String> get_items() 
	{
		return m_items;
	}
}
