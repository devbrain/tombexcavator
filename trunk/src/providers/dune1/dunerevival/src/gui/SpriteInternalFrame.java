package gui;

import entryPoint.DuneViewer;

import java.awt.BorderLayout;
import java.awt.Graphics;
import java.awt.Image;
import java.io.File;

import javax.swing.JComponent;
import javax.swing.JOptionPane;

import cryo_core.ByteFile;
import cryo_core.CryoSprite;
import cryo_core.HSQ;

public class SpriteInternalFrame extends RessourceInternalFrame {

	public class ImagePanel extends JComponent {

        private Image bg;

        public ImagePanel(Image bg) {
        	super();
        	this.bg = bg;
        }
        
         public void paintComponent(Graphics g) {
                g.drawImage(bg,0,0,null);
        } 
	}

	

	public SpriteInternalFrame(File fileToOpen, DuneViewer mainWindow) {
		super(fileToOpen, mainWindow);
		
		//BOTA.HSQ
		//int width = 18;
		//int height = 100;
		//int offset = 2700;

		//BAGDAD.HSQ
		//int width = 5;
//		int width = 115;
//		int height = 500;
//		int offset = 0;
		
		//BACK.HSQ
		int width = 57;
		int height = 100;
		int offset = 0;
		//5, 14

		//open sprite to display
		try {
			//create sprite from file given in argument	
			ByteFile uncompressedFile = HSQ.externalUnHSQ(fileToOpen);
			CryoSprite sprite = new CryoSprite(uncompressedFile, offset, width,height);
			
			//convert sprite to java image
			//ImageIcon image = new ImageIcon(sprite.toImage());
			
			int zoomFactor = 5;
			
			//put image as frame component
			this.add(new ImagePanel(sprite.toImage(zoomFactor)), BorderLayout.CENTER);
			
			setSize(width*zoomFactor, height*zoomFactor);
			
		} catch (Exception ex) {
			JOptionPane.showMessageDialog(mainWindow, "failed to render cryo sprite in frame : "+ ex.getMessage());
		}
	}

}
