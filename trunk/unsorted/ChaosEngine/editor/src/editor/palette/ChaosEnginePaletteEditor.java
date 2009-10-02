package editor.palette;

import javax.swing.JFileChooser;
import javax.swing.JMenu;
import javax.swing.JCheckBoxMenuItem;
import javax.swing.JMenuItem;
import javax.swing.JOptionPane;
import javax.swing.JPanel;
import javax.swing.UIManager;

import base.DirectoryBase;
import base.GfxPalette;
import editor.GfxObjectSetFileFilter;
import editor.ChaosEngineEditor;
import editor.ImageFileFilter;
import editor.ScrolledCanvas;

import java.awt.Dimension;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.io.File;
import java.util.Observable;
import javax.swing.filechooser.FileFilter;

public class ChaosEnginePaletteEditor extends ChaosEngineEditor 
{
	private static final long serialVersionUID = 1L;

	private GfxPalette palette =  new GfxPalette();
	private EditGfxPalette palette_editor;
	
	private JMenu jMenuView = new JMenu("View");
	private JCheckBoxMenuItem jMenuViewDefined = new JCheckBoxMenuItem("Defined tiles");
	private JMenuItem jMenuViewChangeDimension = new JMenuItem("Change default dimension");
	public ChaosEnginePaletteEditor() 
	{
		super("ChaosEngine Palette editor", "GFXOBJECT");
		
		palette_editor = new EditGfxPalette(this,palette);
		
		ScrolledCanvas<? extends JPanel> sw = new PaletteCanvas(palette_editor);
		
		init(palette,sw);
	}
	protected String get_default_path()
	{
		return DirectoryBase.get_tiles_path();
	}
	

	protected void init_widgets() throws Exception
	{
		super.init_widgets();
		
		jMenuViewDefined.setSelected(true);
		
		jMenuViewDefined.addActionListener(new ViewActionListener(ViewActionType.TOGGLE_DEFINED));
		jMenuViewChangeDimension.addActionListener(new ViewActionListener(ViewActionType.CHANGE_DIMENSION));
		jMenuView.add(jMenuViewDefined);
		jMenuView.add(jMenuViewChangeDimension);
		
		add_main_menu(jMenuView);
		
	}
	
	private GfxPalette get_data()
	{
		return (GfxPalette)m_data;
	}
	protected FileFilter get_project_file_filter()
	{
		return new GfxObjectSetFileFilter();
	}
	
	
	
	private Dimension change_tile_dimension()
	{
		GfxPalette gp = get_data();
		
		Dimension rval = gp.get_default_tile_dimension();
		
		try 
		{
			String s = JOptionPane.showInputDialog("default tile width", ""+rval.width);
			if (s != null)
			{
				rval.width = Integer.parseInt(s);
			}
			s = JOptionPane.showInputDialog("default tile height", ""+rval.height);
			if (s != null)
			{

				rval.height = Integer.parseInt(s);
			}		
		}
		catch (NumberFormatException e) 
		{

		}
		
		return rval;
	}
	protected void new_project()
	{
		// first ask dimension
		
		Dimension d = change_tile_dimension();
		
		int tile_width = d.width;
		int tile_height = d.height;


		if ((tile_width > 0) && (tile_height > 0))
		{
			String tiles_1x = DirectoryBase.get_tiles_path();
			JFileChooser fc = new JFileChooser(tiles_1x);
			ImageFileFilter ff = new ImageFileFilter();
			fc.setFileFilter(ff);

			if (fc.showOpenDialog(this) == JFileChooser.APPROVE_OPTION) 
			{		
				int option = 0;
				String image_file = fc.getSelectedFile().getAbsolutePath();

				if (m_data.is_modified())
				{
					option = JOptionPane.showConfirmDialog(this,
							"Current project has not been saved. New project anyway?", "Confirm",
							JOptionPane.OK_OPTION, JOptionPane.QUESTION_MESSAGE);
				}
				if (option == 0)
				{
					int idx = image_file.lastIndexOf('.');
					String project_file = image_file.substring(0, idx) +
					DirectoryBase.GFX_OBJECT_SET_EXTENSION;

					if (new File(project_file).exists())
					{
						option = JOptionPane.showConfirmDialog(this,
								"Project file already exists, create new one?", "Confirm",
								JOptionPane.OK_OPTION, JOptionPane.QUESTION_MESSAGE);
					}
				}
				if (option == 0)
				{	
					m_data.new_project(image_file);
					get_data().set_tile_dimension(d);
					load_image();
				}
			}
		}

	}

	public static void main(String[] args) {
		  try {
			UIManager.setLookAndFeel(UIManager.getSystemLookAndFeelClassName());
		}
		catch(Exception e) {
			e.printStackTrace();
		}
		
		new ChaosEnginePaletteEditor();
		
	}	
	
	private enum ViewActionType { TOGGLE_DEFINED, CHANGE_DIMENSION }
	
	private class ViewActionListener implements ActionListener
	{

		ViewActionType m_vat;

		public ViewActionListener(ViewActionType vat) 
		{
			m_vat = vat;
		}

		public void actionPerformed(ActionEvent evt) 
		{
			switch (m_vat)
			{
			case TOGGLE_DEFINED:			
				palette_editor.toggle_view_defined();
				break;
			case CHANGE_DIMENSION:
				Dimension d = change_tile_dimension();
				get_data().set_tile_dimension(d);
				break;
			}
		}

	}

	public void update(Observable o, Object arg) {
		// TODO Auto-generated method stub
		
	}
}
