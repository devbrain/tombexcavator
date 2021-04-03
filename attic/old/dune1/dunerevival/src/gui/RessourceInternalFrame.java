package gui;

import java.awt.BorderLayout;
import java.awt.Color;
import java.awt.Graphics;
import java.awt.Image;
import java.awt.Toolkit;
import java.io.File;

import javax.swing.ImageIcon;
import javax.swing.JInternalFrame;
import javax.swing.JPanel;
import javax.swing.event.InternalFrameAdapter;
import javax.swing.event.InternalFrameEvent;

import entryPoint.DuneViewer;



public class RessourceInternalFrame extends JInternalFrame {

	class BackPanel extends JPanel{
	    private ImageIcon image = null;
	    
	    public BackPanel() {
	        super();
	    }
	    
	    //public BackPanel (ImageIcon image) {
		public BackPanel (String imageFileName) {
			super();
			
			Image back = Toolkit.getDefaultToolkit().getImage(imageFileName);
	        this.image = new ImageIcon(back);
	        
	        // if the image was not created correctly :
	        if (image.getIconWidth() == -1) {
	        	image = null;
	        	System.err.println("warning : back image of internal frame couldn't be loaded : "+imageFileName);
	        }
	    }
	    
	    public void paintComponent (Graphics g)  {
	        super.paintComponent(g);
	        
	        if (image != null) {
	            int x = 0;
	            int y = 0;
	            int incx = image.getIconWidth();
	            int incy = image.getIconHeight();
	            int totalx = this.getWidth();
	            int totaly = this.getHeight(); 
	            while (y < totaly) {
	                while (x < totalx) {
	                    image.paintIcon(this, g, x, y);
	                    x+=incx;
	                }
	                x = 0;
	                y+=incy;
	            }
	        }
	    }
	}
	
	class RessourceInternalFrameAdapter extends InternalFrameAdapter{
		DuneViewer mainWindow;
		
		public RessourceInternalFrameAdapter(DuneViewer mainWindow) {
			super();
			this.mainWindow = mainWindow;
		}
		
		public void internalFrameClosing(InternalFrameEvent e)
		{
			//DuneViewer mainWindow = (DuneViewer)( (BackgroundImageDesktopPane)(((RessourceInternalFrame)e.getSource())).getParent() ).getParent();
			mainWindow.onCloseRessourceWindow((RessourceInternalFrame)e.getSource());
		}

		public void internalFrameActivated(InternalFrameEvent e)
		{
			//DuneViewer mainWindow = (DuneViewer)( (BackgroundImageDesktopPane)(((RessourceInternalFrame)e.getSource())).getParent() ).getParent();
			mainWindow.onSelectRessourceInternalFrame((RessourceInternalFrame)e.getSource());
		}

	}
	
	BackPanel backPanel;
	DuneViewer mainWindow;
	
	protected RessourceInternalFrame(File fileToOpen, DuneViewer mainWindow) {
		super(fileToOpen.getName(), true, true, true, true);
		
		this.mainWindow = mainWindow;
		
		addInternalFrameListener(new RessourceInternalFrameAdapter(mainWindow));

		backPanel = new BackPanel("./ressource/back2.jpg");;		
		backPanel.setBackground(new Color(0.8f, 0.8f,0.9f));
		
		setSize(400, 400);
		
        getContentPane().add(backPanel, BorderLayout.CENTER);
        
 	}
	
}
