package gui;

import java.awt.Graphics;
import java.awt.Image;
import java.awt.Toolkit;

import javax.swing.ImageIcon;
import javax.swing.JDesktopPane;

public class BackgroundImageDesktopPane extends JDesktopPane {
    
    private ImageIcon image;
    
    public BackgroundImageDesktopPane() {
        super();
    }
    
    public BackgroundImageDesktopPane (String imageName) {
    	Image back = Toolkit.getDefaultToolkit().getImage(imageName);
    	this.image = new ImageIcon(back);
        
        if (image.getIconWidth() == -1) {
        	image = null;
        	System.err.println("Warning : couldn't load background : "+imageName);
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