package cryo_core;

import java.awt.Image;
import java.awt.Toolkit;
import java.awt.image.BufferedImage;
import java.util.Date;
import java.util.Random;

public class CryoSprite {

	protected int[] palette = new int[768];
	
	protected int[] pixelBuffer = {255}; //initialization value : one pixel
	protected int width = 1;
	protected int height = 1;
	
	public CryoSprite(ByteFile file, int offset, int width, int height) {
		pixelBuffer = new int[width*height];
		
		//debug
		for (int i=0; i<width*height; i++) pixelBuffer[i] = 0;
		
		this.width= width;
		this.height = height;
		
		
		loadPalette(".\\ressource\\default.pal");
		
		file.reset();
		
		try {
		//skip offset
		for (int i=0; i <offset;i++)
			file.readByte();
		
		//read sprite
		for (int j=0; j<height;j++) {
			for (int i=0; i<width;i++) {
				pixelBuffer[width*j+i] = file.readByte();
			}
		}

		} catch (Exception ex) {
			//do nothing; buffer will not be properly filled
		}
	}
	
	
	public Image toImage(int zoomFactor) {
		
		int finalPixelWidth  = width*zoomFactor;
		int finalPixelHeight = height*zoomFactor;
		//allocate RGB buffer
		int[] rgbs = new int[finalPixelWidth*finalPixelHeight];
		
		

		for (int i=0; i<finalPixelWidth*finalPixelHeight; i++) rgbs[i] = 0;	//debug
		
		//fill RGB buffer (palette is not taken in account yet)
		int value;
		for (int j=0; j<height;j++) {
			for (int i=0; i<width;i++) {
				value = pixelBuffer[width*j+i];
				int r = palette[3*value];
				int g = palette[3*value+1];
				int b = palette[3*value+2];
				
				for (int j2=0; j2<zoomFactor;j2++) {
					for (int i2=0; i2<zoomFactor;i2++) {
						int x = i*zoomFactor+i2;
						int y = j*zoomFactor+j2;
						
						rgbs[finalPixelWidth*y+x] = (255<<24)+(r<<16)+(g<<8)+ b; ; //   FFh,R,G,B 
					}
					
				}
						
						
				
			}
		}
		
		//create buffered image using RGB buffer :
		BufferedImage bufferedImage = new BufferedImage(
				finalPixelWidth,
				finalPixelHeight,
				BufferedImage.TYPE_INT_RGB );
		
		//set buffer
		bufferedImage.setRGB(0 ,0, finalPixelWidth, finalPixelHeight, rgbs, 0, finalPixelWidth);
		
		//create final image from buffered image
		Image result = Toolkit.getDefaultToolkit().createImage(bufferedImage.getSource());
		
		
		return result;
	}
	
	
	protected void loadPalette(String fileName)  {
//		try {
//			ByteFile palFile = new ByteFile(fileName);
//			for (int i=0; i<768;i++) {
//				this.palette[i] = palFile.readByte();
//			}
//		} catch (Exception ex) {
			System.err.println("Couldn't load default palette");
			
			/*
			//palette will be filled with random values
			Random r = new Random(new Date().getTime());
			for (int i=0; i<768;i++) {
				palette[i] = r.nextInt();
			}
			*/
			//palette is fillled incrementaly
			for (int i=0; i<768;i++) {
				palette[i] = (int)(i/3);
			}
//		}
	}
	
	public static void main(String[] args) {

		

	}

}
