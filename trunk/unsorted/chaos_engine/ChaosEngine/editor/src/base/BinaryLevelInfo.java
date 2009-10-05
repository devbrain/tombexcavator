package base;
import java.io.*;
import java.util.*;

import base.BinaryLevelInfo.BinaryChunk.DecodeException;

public class BinaryLevelInfo 
{
	public class BinaryChunk
	{
		
		public BinaryChunk(String name)
		{
			data = null;
			m_name = name;
		}
		
		
		private String m_name;
		public String get_name()
		{
			return m_name;
		}
		private FileInputStream m_input_stream;
		protected byte [] data;
		private int offset = 0;
		
		public void sub_chunk(BinaryChunk src, int start, int end) throws DecodeException
		{
			System.out.println("subchunk "+get_name()+" start=0x"+Integer.toHexString(start)+ " end=0x"+Integer.toHexString(end)+", size="+Integer.toHexString(end-start));
		   data = Arrays.copyOfRange(src.data, start, end);
		   decode();
		}
		
		public void rewind()
		{
			offset = 0;
		}
		
		public int current_pos()
		{
			return offset;
		}
		
		public int get_byte()
		{
			int rval = (int)data[offset++];
			// cast from byte to int carries sign: we don't want that
			
			if (rval < 0)
			{
				rval += 0x100;
			}
			return rval;
		}
		
		public int get_word()
		{
			int rval = (get_byte() << 8);
			rval += get_byte(); 
			
			return rval;
		}
		public int get_long()
		{
			int rval = (get_byte() << 24);
			rval += (get_byte() << 16);
			rval += (get_byte() << 8);
			rval += get_byte(); 
			
			return rval;
		}
		public void skip(int o)
		{
			offset += o;
		}
		
		public void set_offset(int o)
		{
			offset = o;
		}
		
		protected void decode() throws DecodeException
		{
			// do nothing
		}
		
		public class DecodeException extends IOException
		{
			/**
			 * 
			 */
			private static final long serialVersionUID = 1L;

			public DecodeException(String msg)
			{
				super(get_name()+": "+msg);
			}
		}
		public void load(String file) throws IOException
		{
			File f = new File(file);
			long sz = f.length();
			data = new byte[(int)sz];
			
			m_input_stream = new FileInputStream(f);
			m_input_stream.read(data);
			m_input_stream.close();
			
			decode();
		}
	}
	
	public class BinaryChunkWithHeader extends BinaryChunk
	{
		protected int m_struct_size;
		protected int m_nb_items;
		
		// pass -1 to cancel checks
		
		public BinaryChunkWithHeader(String name, int struct_size)
		{
			super(name);
			m_struct_size = struct_size;
		}
		@Override
		protected void decode() throws DecodeException
		{
			super.decode();
			m_nb_items = get_word();
			int struct_size = get_word();
			if ((struct_size != m_struct_size) && (m_struct_size != -1))
			{
				throw(new DecodeException("struct size mismatch: read "+struct_size+" expected "+m_struct_size)); 
			}
			m_struct_size = struct_size;
			
		}
	}
	public class BinaryEnemyPositions extends BinaryChunkWithHeader
	{
		public class ItemPosition
		{
			public int x,y;
			public int enemy_type;
			public int points;
			public int f5,f6,f7,fe,f38;
			public String comment;
		}
		public Vector<ItemPosition> items = new Vector<ItemPosition>();		
		@Override
		protected void decode() throws DecodeException 
		{
			super.decode();
			
			for (int i = 0; i < m_nb_items; i++)
			{
				ItemPosition ip = new ItemPosition();
				ip.x = get_word();
				ip.y = get_word();
				ip.enemy_type = get_byte();
				ip.f5 = get_byte();
				ip.f6 = get_byte();
				ip.f7 = get_byte();
				
				ip.points = get_word();
				get_word();
				get_word(); // 136C
				ip.fe = get_word();
				skip(0x28);
				ip.f38 = get_word();
				get_word();
				get_word();
				
				ip.comment = " " + ip.points + " 0x"+Integer.toHexString(ip.f5) + 
				" 0x"+Integer.toHexString(ip.f6) + " 0x"+Integer.toHexString(ip.f7) + 
				" 0x"+Integer.toHexString(ip.fe) + " 0x"+Integer.toHexString(ip.f38);
				
				System.out.println(ip.comment);
				items.add(ip);
			}
			
		}

		public BinaryEnemyPositions() 
		{
			super("enemy_positions", 0x3E);
			
		}	
		}
	public class BinaryItemPositions extends BinaryChunkWithHeader
	{
		public class ItemPosition
		{
			public int x,y;
			public int item_code;
			public int flag_4;
			public String comment;
		}
		public Vector<ItemPosition> items = new Vector<ItemPosition>();		
		
		@Override
		protected void decode() throws DecodeException 
		{
			// TODO Auto-generated method stub
			super.decode();
			
			for (int i = 0; i < m_nb_items; i++)
			{
				ItemPosition ip = new ItemPosition();
				ip.x = get_word();
				ip.y = get_word();
				ip.flag_4 = get_byte();
				
				int trigger_id = get_byte();
				ip.comment = " 0x"+Integer.toHexString(trigger_id);
				get_byte(); // skip redundant height information
				if ((ip.flag_4 & 0x40) != 0);
				{
				   ip.comment += " (trigger)";
				}
				ip.item_code = get_byte();
				items.add(ip);
			}
			
		}

		public BinaryItemPositions() 
		{
			super("item_positions", 8);
			
		}
	
	}
	public class BinaryTileData extends BinaryChunkWithHeader
	{
		public class TileLine
		{
			public TileLine(int sz)
			{
				index = new int[sz];
				flags = new int[sz];
			}
			public int [] index;
			public int [] flags;
		}
	
		
		public Vector<TileLine> rows()
		{
			return m_rows;
		}
		private Vector<TileLine> m_rows = new Vector<TileLine>();
		
		public BinaryTileData()
		{
			super("tile_data",-1);
		}
		@Override
		protected void decode() throws DecodeException
		{
			super.decode();
			get_byte(); // world number (1 -> 4): unused ?
			get_byte(); // unknown
			
			int key_mask = 0x1FF;
			
			int key_shift = -1;
			int kmp1 = key_mask+1;
			while (kmp1 != 0)
			{
				key_shift++;
				kmp1 = kmp1 >> 1;
			}
			
			int nb_rows = m_struct_size;
			int nb_cols = m_nb_items;
			
			// create a bi-dimensional array
			for (int i = 0; i < nb_rows; i++)
			{
				TileLine tl = new TileLine(nb_cols);
				
				for (int j = 0; j < nb_cols; j++)
				{
					int v = get_word();
					tl.index[j] = v & key_mask;
					tl.flags[j] = v >> key_shift;
				}
			
				m_rows.add(tl);
			}
		}
	}
	
	private BinaryChunk m_header = new BinaryChunk("header");
	private BinaryTileData m_tile_data = new BinaryTileData();
	private BinaryChunk m_unkn_2 = new BinaryChunk("unkn_2");
	private BinaryEnemyPositions m_enemy_positions = new BinaryEnemyPositions();
	private BinaryChunkWithHeader m_event_positions = new BinaryChunkWithHeader("event_positions",8);
	private BinaryChunkWithHeader m_special_zones = new BinaryChunkWithHeader("special_zones",0x1E);
	private BinaryItemPositions m_item_positions = new BinaryItemPositions();
	private BinaryChunk m_unkn_7 = new BinaryChunk("unkn_7");
	
	public BinaryTileData get_tile_data()
	{
		return m_tile_data;
	}
	public BinaryItemPositions get_item_positions()
	{
		return m_item_positions;
	}
	public BinaryEnemyPositions get_enemy_positions()
	{
		return m_enemy_positions;
	}
	
	public void load(String file) throws IOException
	{
		BinaryChunk whole = new BinaryChunk("whole");
		whole.load(file);
		int start = 0;
		int end = whole.get_long();
		m_header.sub_chunk(whole,start, end);
		start = end;
		end = whole.get_long();
		m_tile_data.sub_chunk(whole,start, end);
		start = end;
		end = whole.get_long();
		m_unkn_2.sub_chunk(whole,start, end);
		start = end;
		end = whole.get_long();
		m_enemy_positions.sub_chunk(whole,start, end);
		start = end;
		end = whole.get_long();
		m_event_positions.sub_chunk(whole,start, end);
		start = end;
		end = whole.get_long();
		m_special_zones.sub_chunk(whole,start, end);
		start = end;
		end = whole.get_long();
		m_item_positions.sub_chunk(whole,start, end);
		start = end;
		end = whole.get_long();
		m_unkn_7.sub_chunk(whole,start, end);
 		
	}
}
