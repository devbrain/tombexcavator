package base;


import base.BinaryLevelInfo.BinaryTileData.TileLine;
import base.associations.ObjectAssociation;
import base.layer.*;
import sys.*;

import java.io.File;
import java.io.IOException;
import java.awt.*;
import java.awt.image.BufferedImage;
	
import java.util.*;
import java.io.*;


public class LevelData implements EditableData, EditorRenderable, Renderable

{
	private String m_copperbar_class;
	private String m_level_class;
	private String m_level_music;
	private String m_palette_set_file = ParameterParser.UNDEFINED_STRING;
	private ObjectAssociationSet m_association_set;
	private HashMap<String,ShopContents> m_shops = new HashMap<String,ShopContents>();
	private LinkedList<AnimatedFrames> m_breaking_blocks = new  LinkedList<AnimatedFrames>();
	//private LinkedList<ControlObject> m_added_blocks = new LinkedList<ControlObject>();
	private Renderable m_breaking_blocks_renderable = new BreakingBlocks();
	private HostileWaveParametersSet m_hostile_params = new HostileWaveParametersSet();
	protected Rectangle m_view_bounds;
	private String m_bin_info_file;
	private String m_tile_image_file;
	private BinaryLevelInfo m_bin_info;
	
	private String m_project_file;
	
	private boolean m_flag_layer_visible = false;
	private TileGrid m_tile_grid;
	private HeightLayer m_height_layer;
	private GfxObjectLayer m_object_layer;
	private EnemyObjectLayer m_misc_layer;
	private ControlObjectLayer m_control_layer;
	
	private boolean m_modified;

	
	private int m_hostile_damage_divisor = 1;
	
	private int m_shield_level = 0;
	
	private int m_translated_y = 0;
	private BufferedImage m_copper_image = null;
	
	private void debug(String m)
	{
		if (Debug.active)
		{
			System.out.println(this.getClass().getName()+": "+m);
		}
	}
	
	private class BreakingBlocks implements Renderable
	{
		public void render(Graphics2D g) 
		{
			for (AnimatedFrames af : m_breaking_blocks)
			{
				af.render(g);
			}
		}
		
		public void update(long elapsed_time) 
		{
			ListIterator<AnimatedFrames> it = m_breaking_blocks.listIterator();
			
			while (it.hasNext())
			{
				AnimatedFrames af = it.next();
				
				af.update(elapsed_time);
				if (af.is_done())
				{
					it.remove();
				}
			}
		}
		
	}
	
	public GfxPalette get_level_palette()
	{
		return m_tiles_palette;
	}
	public void export(String s)
	{
		BufferedImage bi = m_tile_grid.render(null, null);
		ImageLoadSave.save_png(bi, s);
	}
	
	public int get_damage_divisor()
	{
		return m_hostile_damage_divisor;
	}
	
	public void set_damage_divisor(int d)
	{
		m_hostile_damage_divisor = d;
	}
	
	public int get_shield_level()
	{
		return m_shield_level;
	}
	
	public void set_shield_level(int sl)
	{
		m_shield_level = sl;
	}
	
	
	public void editor_render(Graphics g) 
	{
		if (m_control_layer != null)
		{
			m_control_layer.set_hostile_parameters(/*get_hostile_params()*/null,m_other_palettes);		
			m_control_layer.editor_render(g);
			
		}
	}

	public static final int SHOW_VISIBLE_OBJECTS = 1;
	public static final int SHOW_HIDDEN_OBJECTS = 1<<1;
	
	private int m_object_view_filter = 0xff;
	private boolean m_tile_overwrite_mode = false;
	
	
	
	public int get_width()
	{
		return m_tile_grid.get_tile_width() * m_tile_grid.get_nb_cols();
	}
	
	public int get_height()
	{
		return m_tile_grid.get_tile_height() * m_tile_grid.get_nb_rows();
	}
	



	public TileGrid get_grid()
	{
		return m_tile_grid;
	}
	
	public void add_control_object_filter(ControlObject.Type t)
	{
		m_control_layer.add_filter(t);
	}
	
	public void remove_control_object_filter(ControlObject.Type t)
	{
		m_control_layer.remove_filter(t);
	}
	
	public void show_flags_layer(boolean f)
	{
		m_flag_layer_visible = f;
	}
	public void set_object_view_filter(int vf)
	{
		m_object_view_filter = vf;
	}
	
	public void bitset_object_view_filter(int m)
	{
		m_object_view_filter |= m;
	}
	public void bitclr_object_view_filter(int m)
	{
		m_object_view_filter &= ~m;
	}
	
	public int get_object_view_filter()
	{
		return m_object_view_filter;
	}
	
	
	public ControlObjectLayer get_control_layer()
	{
		return m_control_layer;
	}
	
	public GfxObjectLayer get_object_layer()
	{
		return m_object_layer;
	}
	public EnemyObjectLayer get_enemy_layer()
	{
		return m_misc_layer;
	}
	
	public void add_object(GfxObject go)
	{
		get_object_layer().get_items().add(go);
	}
	public void remove_object(GfxObject go)
	{
		get_object_layer().get_items().remove(go);
	}
	
	public void drop_object(GfxObject go)
	{
		get_object_layer().get_items().add(go);
	}
	
	public void init(String name, String level_tile_file) throws IOException
	{
			new_project(name);	
			debug("loading level tiles");
			//m_tiles_palette.load(level_tile_file);
	}
	
	
	private boolean compute_tile_size()
	{
		Dimension d = new Dimension(16,16);//palette.get_default_tile_dimension();

		
		if (d != null)
		{
			int w = (int)d.width;
			int h = (int)d.height;
			
			m_tile_grid = new TileGrid(m_bin_info.get_tile_data().rows(),m_tiles_palette);
			m_height_layer = new HeightLayer(m_bin_info.get_tile_data().rows());
			m_object_layer = new GfxObjectLayer(w,h);
			m_misc_layer = new EnemyObjectLayer(w,h);
			m_control_layer = new ControlObjectLayer(w,h/2);
			
		}
		return (d != null);
	}
	
	public LevelData()
	{
		reset();
	}
	
	
	public void update(long elapsed_time)
	{
		m_object_layer.update(elapsed_time);
		
		
		m_breaking_blocks_renderable.update(elapsed_time);
	}
	
	public BufferedImage render_tiles(BufferedImage img, Rectangle bounds)
	{
		return m_tile_grid.render(img, bounds);
	}
	
	public BufferedImage render_all_layers(BufferedImage img, Rectangle bounds)
	{
		BufferedImage bi = render_tiles(img, bounds);

		if (m_flag_layer_visible)
		{
			bi = m_height_layer.render(bi);
		}
		
		boolean show_visible = ((m_object_view_filter & SHOW_VISIBLE_OBJECTS) == SHOW_VISIBLE_OBJECTS);
		boolean show_hidden = ((m_object_view_filter & SHOW_HIDDEN_OBJECTS) == SHOW_HIDDEN_OBJECTS);

		m_object_layer.set_view_filter(show_visible,show_hidden);
	
		if (bi != null)
		{
			m_object_layer.render((Graphics2D)bi.getGraphics());
			//m_misc_layer.render((Graphics2D)bi.getGraphics()); //disabled for now
		}		
		
		return bi;
	}
	
	public GfxFrame get_tile(int x, int y)
	{
		return m_tile_grid.get(x, y);
	}
	
	public void set_tile(int x, int y, GfxFrame gf)
	{
		m_tile_grid.set(x, y, gf);
		set_modified(true);
	}
	
	public GfxObject get_bonus(int x, int y)
	{
		return m_object_layer.get(x, y);
	}
	
	public GfxObject get_bonus(String name)
	{
		return m_object_layer.get(name);
	}
	public GfxObject get_enemy(int x, int y)
	{
		return m_misc_layer.get(x, y);
	}
	
	public GfxObject get_enemy(String name)
	{
		return m_misc_layer.get(name);
	}
	

	public GfxObject set_bonus(int x, int y, String name, GfxFrameSet bonus)
	{
		set_modified(true);
		return m_object_layer.set(x, y, name, bonus);
	}
	
	public ControlObject get_control_object(int x, int y)
	{
		return m_control_layer.get(x,y);
	}
	public ControlObject get_control_object(String name)
	{
		return m_control_layer.get(name);
	}
	
	public ControlObject set_control_object(int x, int y, String name, ControlObject.Type tt)
	{
		set_modified(true);
		
		ControlObject co = m_control_layer.set(x, y, name, tt);
		
		//m_hostile_params.remove(co);
		
		return co;
	}
	
	
	
	private void reset()
	{
		m_tiles_palette = new GfxPalette();
		m_association_set = new ObjectAssociationSet();
		m_modified = false;
		m_tile_overwrite_mode = false;
	}
	
	public String get_project_file() 
	{
		return m_project_file;
	}
	
	public static String get_level_class(String project_file) throws IOException
	{
		LevelData l = new LevelData();
		l.load_start(project_file).close();
		
		return l.get_level_class();		
	}
	// "light" open of the file only to get class name
	
	private ParameterParser load_start(String project_file) throws IOException
	{
		set_project_file(project_file);
		String project_file_path = DirectoryBase.get_levels_path() + m_project_file + DirectoryBase.LEVEL_EXTENSION;
		ParameterParser fr = ParameterParser.open(project_file_path);

		fr.startBlockVerify(get_block_name());

		return fr;
	}
	
	private void create_tiles_palette(String tile_image_file)
	{
		BufferedImage map_image = ImageLoadSave.load(tile_image_file);
		
		m_tiles_palette = new GfxPalette();
		Rectangle r = new Rectangle();
		r.width = 16;
		r.height = 16;
		int nb_cols = map_image.getWidth() / r.width;
		int nb_rows = map_image.getHeight() / r.height;

		
		
		int idx = 0;
		for (int i = 0; i < nb_rows; i++)
		{
			r.y = i * r.height;
			
			for (int j = 0; j < nb_cols; j++)
			{
				r.x = j * r.width;
				GfxFrameSet gfs = new GfxFrameSet(""+idx,GfxFrameSet.Type.background_tile);
				GfxFrame gf = new GfxFrame(gfs,map_image,r,0);
				gfs.add(gf);
				m_tiles_palette.add(gfs);
				idx++;
			}
		}
	}
	public void load(String project_file) throws IOException 
	{		
		m_tile_overwrite_mode = false;
		
		ParameterParser fr = load_start(project_file);

		m_tile_image_file = fr.readString("tile_image");
		
		m_bin_info_file = fr.readString("bin_info");
		
		m_bin_info = new BinaryLevelInfo();
		
		File f = new File(project_file);

		m_bin_info.load(f.getParent() + File.separator + m_bin_info_file + ".CAS");
		
		create_tiles_palette(f.getParent() + File.separator + m_tile_image_file + ".png");
		
		compute_tile_size();
		
		debug("loading other tiles");
		
		m_palette_set_file = fr.readString("palette_set");
		
		m_other_palettes = new GfxPaletteSet();
		
		ParameterParser palette_handle = ParameterParser.open(f.getParent() + File.separator + m_palette_set_file + ".pas");
		
		m_other_palettes.load(palette_handle);
		
		palette_handle.close();
		
		m_other_palettes.add(m_tiles_palette);

		m_object_layer.load(m_bin_info.get_item_positions(), m_other_palettes);
		
		//m_association_set = new ObjectAssociationSet(fr,this);
		m_misc_layer.load(m_bin_info.get_enemy_positions(), m_other_palettes);
	
		//m_control_layer.load(m_bin_info.get_enemy_positions());
		
		//m_hostile_params = new HostileWaveParametersSet(fr,this);
		
		debug("all done OK");
		
		fr.endBlockVerify();
		fr.close();
		
		set_modified(false);
		
	}
	

	public void new_project(String file) 
	{
		reset();
		set_project_file(file);
		
	}
	
	public Rectangle get_view_bounds()
	{
		return m_view_bounds;
	}
	
	public boolean save() throws IOException 
	{
		boolean rval = false;
		/*
		String project_file_path = DirectoryBase.get_levels_path() + get_project_file() + DirectoryBase.LEVEL_EXTENSION;
		String backup_file_path = project_file_path + "~";

		// create backup file just in case...
		
		File fb = new File(backup_file_path);
		fb.delete();
		File f = new File(project_file_path);
		f.renameTo(fb);

		ParameterParser fw = ParameterParser.create(project_file_path);
		
		try
		{
			serialize(fw);
			fw.close();
			fw = null;
			
			rval = true;
		}
		catch (RuntimeException r)
		{
			// print the exception
			r.printStackTrace();
			if (fw != null)
			{
				fw.close();
			}
		}
		
		*/
		set_modified(false);
		
		return rval;
	}
	
	public void set_project_file(String file) 
	{		
		m_project_file = DirectoryBase.rework_path(file,DirectoryBase.get_levels_path(),true,true);
		
	}
	
	public BufferedImage import_from_image(String image_file, int rgb_tolerance)
	{
		BufferedImage bi = ImageLoadSave.load(image_file);
		BufferedImage rval = null;
		
		if (bi != null)
		{			
			debug("importing tiles from "+image_file);
			rval = m_tile_grid.import_from_image(bi,m_tiles_palette,rgb_tolerance);			
		}
		return rval;
	}
	
	public boolean is_modified() {
		return m_modified;
	}
	public void set_modified(boolean m) {
		m_modified = m;
	}
	public String get_block_name() {		
		return "CHAOS_ENGINE_LEVEL";
	}
	public void serialize(ParameterParser fw) throws IOException 
	{
		/*fw.startBlockWrite(get_block_name());
		fw.write("level_class",m_level_class);
		fw.write("level_music", m_level_music);
		
		fw.write("level_tiles",m_tiles_palette.get_project_file());
		fw.write("copperbar_class",m_copperbar_class);
		
		fw.write("hostile_damage_divisor", m_hostile_damage_divisor);
		
		fw.write("shield_level", m_shield_level);
		
		m_tile_grid.serialize(fw);		
		m_object_layer.serialize(fw);
		m_control_layer.serialize(fw);
		m_association_set.serialize(fw);
		m_hostile_params.serialize(fw);
		
		fw.endBlockWrite();*/
		
		fw.endBlockWrite();		
	}
	
	private GfxPalette m_tiles_palette = null;	
	private GfxPaletteSet m_other_palettes = new GfxPaletteSet();	
	
	public void render(Graphics2D g) 
	{
		// draw background
		
		m_tile_grid.render(g, m_view_bounds, m_translated_y, m_copper_image);
		
	// draw breaking blocks
		
		m_breaking_blocks_renderable.render(g);
		
		// draw items
		
		m_object_layer.set_view_filter(true,false);

		m_object_layer.render(g);
		
	}


	public ObjectAssociationSet get_association_set() 
	{
		return m_association_set;
	}

	public HostileWaveParametersSet get_hostile_params()
	{
		return m_hostile_params;
	}
	
	public boolean add_association(ObjectAssociation o)
	{
		boolean rval = false;

		if (o != null)
		{
			// put ours (add returns null: it worked)
			rval = m_association_set.add(o) == null;
			if (rval)
			{
				set_modified(true);
			}
		}
		
		return rval;
	}
	public void remove_association(ObjectAssociation o)
	{
		m_association_set.remove(o);
		
		set_modified(true);
		
	}

	public String get_copperbar_class() {
		return m_copperbar_class;
	}

	public void set_copperbar_class(String copperbar_class) {
		this.m_copperbar_class = copperbar_class;
	}

	public String get_level_class() 
	{
		return m_level_class;
	}

	public String get_level_music()
	{
		return m_level_music;
	}

	public void set_level_class(String level_class) {
		this.m_level_class = level_class;
	}

	public void set_level_music(String level_music)
	{
		this.m_level_music = level_music;
	}

	// resize
	
	public void set_dimension(int nb_rows, int nb_cols, boolean on_left, boolean on_top)
	{
		int old_nb_rows = m_tile_grid.get_nb_rows();
		int old_nb_cols = m_tile_grid.get_nb_cols();
		
		int left_offset = on_left ? nb_cols - old_nb_cols : 0;
		int top_offset = on_top ? nb_rows - old_nb_rows : 0;
		
		left_offset *= m_tile_grid.get_tile_width();
		top_offset *= m_tile_grid.get_tile_height();
		
		m_object_layer.move_objects(left_offset,top_offset);
		m_control_layer.move_objects(left_offset,top_offset);
				
		m_tile_grid.set_dimension(nb_rows,nb_cols,on_left,on_top,m_tile_grid.get_palette());

		m_object_layer.remove_invisible_objects(get_width(),get_height());
		m_control_layer.remove_invisible_objects(get_width(),get_height());
	}


	public boolean is_tile_overwrite_mode() {
		return m_tile_overwrite_mode;
	}

	public void set_tile_overwrite_mode(boolean tile_overwrite_mode) {
		this.m_tile_overwrite_mode = tile_overwrite_mode;
	}

	public void add_shop(String name, ShopContents sc)
	{
		m_shops.put(name,sc);
	}
	
	public void remove_shop(String name)
	{
		m_shops.remove(name);
	}
	
	public void notify_x_move_object(GfxObject o, int old_x)
	{
	    // do nothing
	}
}
