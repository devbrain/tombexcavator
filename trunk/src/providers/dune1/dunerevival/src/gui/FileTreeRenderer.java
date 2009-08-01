package gui;

import javax.swing.*; 
import javax.swing.tree.*; 
import javax.swing.filechooser.*; 
import java.awt.*; 
import java.io.*; 

public class FileTreeRenderer extends DefaultTreeCellRenderer{
	public FileTreeRenderer(){
		super();	
	}	
	public Component getTreeCellRendererComponent(JTree tree,Object value
			,boolean selected,boolean expanded
			,boolean leaf,int row,boolean hasFocus){
		JLabel label = (JLabel)super.getTreeCellRendererComponent(tree,value,selected,expanded,leaf,row,hasFocus);
		File fichier = (File)value;
		try {
			FileSystemView sys = FileSystemView.getFileSystemView();
			label.setText(sys.getSystemDisplayName(fichier));
			label.setIcon(sys.getSystemIcon(fichier));
		} catch (Exception ex) {
			label.setText("ERROR");
		}
		return label;				
	}	
}