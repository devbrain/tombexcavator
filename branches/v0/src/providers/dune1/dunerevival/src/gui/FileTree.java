package gui;

import java.awt.GridLayout;
import java.awt.event.MouseAdapter;
import java.awt.event.MouseEvent;
import java.io.File;

import javax.swing.JPanel;
import javax.swing.JScrollPane;
import javax.swing.JTextArea;
import javax.swing.JTree;
import javax.swing.event.TreeSelectionEvent;
import javax.swing.event.TreeSelectionListener;
import javax.swing.tree.TreeModel;
import javax.swing.tree.TreePath;
import javax.swing.tree.TreeSelectionModel;

import entryPoint.DuneViewer;
import files_recognizer.CryoFile;



public class FileTree extends JPanel implements TreeSelectionListener{
	private File root;
	private TreeModel modele;
	private JTree tree; 
	private JTextArea infos;
	
	private DuneViewer mainWindow;

	class TreeClickMouseAdapter extends MouseAdapter {
		private DuneViewer mainWindow;
		
		public TreeClickMouseAdapter(DuneViewer mainWindow) {
			super();
			this.mainWindow = mainWindow;
		}
	     public void mousePressed(MouseEvent e) {
	         int selRow = tree.getRowForLocation(e.getX(), e.getY());
	         TreePath selPath = tree.getPathForLocation(e.getX(), e.getY());
	         if(selRow != -1) {
	             if(e.getClickCount() == 1) {
	            	 //do nothing
	             }
	             else if(e.getClickCount() == 2) {
	                 //myDoubleClick(selRow, selPath);
	            	 File openedFile  = (File) selPath.getLastPathComponent();
	            	 mainWindow.openRessource(openedFile);
	             }
	         }
	     }
	}

	public FileTree(String repertoire, DuneViewer mainWindow){
		super();	
		
		this.mainWindow = mainWindow;


		root = new File(repertoire);
		modele = new FileTreeModel(root);
		tree = new JTree(modele);
		tree.setCellRenderer(new FileTreeRenderer());
		tree.addTreeSelectionListener(this);
		//tree.addTreeSelectionListener(mainWindow);
		tree.getSelectionModel().setSelectionMode(TreeSelectionModel.SINGLE_TREE_SELECTION);
		tree.addMouseListener(new TreeClickMouseAdapter(mainWindow));
		
		infos = new JTextArea();
		infos.setEditable(false);
		infos.setLineWrap(false);
		infos.setWrapStyleWord(true);
		


		setLayout(new GridLayout(1,2));
		add(new JScrollPane(tree));
		add(new JScrollPane(infos));
	}


	public void valueChanged(TreeSelectionEvent e){
		System.err.println("changed file-tree selection");
		TreePath path = e.getPath();
		File file = (File)path.getLastPathComponent();
		String s = "";
		//s += "Can read : \n   "+file.canRead()+"\n";
		//s += "Can write : \n   "+file.canWrite()+"\n";
		//s += "Parent : \n   "+file.getParent()+"\n";
		s += "Name : \n   "+file.getName()+"\n";
		s += "Length : \n   "+file.length()+"\n";
		//s += "Last modified : \n   "+new Date(file.lastModified())+"\n";
		s += "Is Cryo File : \n   ";
		CryoFile cryoFile = mainWindow.viewerCore.recognizer.getCryoFile(file.getName());
		if (cryoFile != null) {
			s+= "YES\n";
		} else {
			s+= "NO\n";
		}
		infos.setText(s);
	}	

}