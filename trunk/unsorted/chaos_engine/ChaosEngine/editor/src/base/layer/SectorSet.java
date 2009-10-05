package base.layer;

import base.*;

import java.util.*;

public class SectorSet<T extends Locatable>
{

	
	private Vector< Sector<T> > m_sector_list;
	private int m_width;
	
	private Sector<T> sector(int i)
	{
		return m_sector_list.elementAt(Math.max(i,0) / m_width);
	}
	
	public SectorSet(int nb_sectors,int width)
	{
		m_sector_list = new Vector<Sector <T> >();
		
		for (int i = 0; i < nb_sectors; i++)
		{
			m_sector_list.add(new Sector<T>());
		}
		
		m_width = width;
	}
	
	public boolean remove(T item)
	{
		boolean rval = false;
		if (item != null)
		{
			Sector<T> s = sector(item.get_x());
			if (s != null)
			{
				rval = s.remove(item);
			}
		}
		
		return rval;
	}
	
	public void add(Collection<T> list)
	{
		for (T t : list)
		{
			add(t);
		}
	}
	
	public void add(T item)
	{
		sector(item.get_x()).add(item);
	}
	
	public Sector<T>.Distance closest(Locatable other)
	{
		int sector_index = other.get_x() / m_width;
		
		Sector<T>.Distance t1 = m_sector_list.elementAt(sector_index).closest(other);
		
		if (sector_index > 0)
		{
			Sector<T>.Distance t2 = m_sector_list.elementAt(sector_index-1).closest(other);
			
			if (t1.square_distance > t2.square_distance)
			{
				t1 = t2;
			}
		}
		if (sector_index < m_sector_list.size() - 1)
		{
			Sector<T>.Distance t2 = m_sector_list.elementAt(sector_index+1).closest(other);
			
			if (t1.square_distance > t2.square_distance)
			{
				t1 = t2;
			}
		}
		
		return t1;
	}
	
}
