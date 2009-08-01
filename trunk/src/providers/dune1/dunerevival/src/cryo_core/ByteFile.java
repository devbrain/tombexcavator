/**
 * 
 */
package cryo_core;

import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.io.InputStream;
import java.nio.ByteBuffer;
import java.nio.CharBuffer;
import java.nio.charset.Charset;
import java.nio.charset.CharsetDecoder;
import java.util.ArrayList;

/**
 * @author Administrateur
 *
 */
public class ByteFile extends File {


	private static final long serialVersionUID = 1534365156512823671L;
	
	//protected String fileName;
	protected int readOffset = 0;
	protected byte[] fileContent;


	public ByteFile(String fileName)  throws ByteFileException  {
		super(fileName);

		InputStream is;
		try {
			System.err.println("Opening '"+toString()+"'");
			is = new FileInputStream(this);
			
	        long length = length();	    
	        // buffer qui simule la lecture du fichier
	        fileContent = new byte[(int)length];
	    
	        for (int i=0; i<length; i++) fileContent[i] = 0;	//debug
	        
	        int offset = 0;
	        int numRead = 0;
	        while (offset <  fileContent.length
	               && (numRead=is.read( fileContent, offset,  fileContent.length-offset)) >= 0) {
	            offset += numRead;
	        }
	    
	        // tous les bytes ont été lus ?
	        if (offset <  fileContent.length) {
	            throw new IOException("Could not completely read file "+fileName);
	        }
	        is.close();
	        
	        //on remet le curseur au début
	        readOffset = 0;
	        
		} catch (FileNotFoundException ex) {
			throw new ByteFileException("ERROR : can't find '"+fileName+"' for open; terminating.");
		} catch (Exception ex) {
			throw new ByteFileException("ERROR : can't read '"+fileName+"' for unknown reason : " +ex.getMessage());
		}
	
	
	}
	
	public void reset() {
		readOffset=0;
	}
	
	public int readByte() throws ByteFileException {
		if (readOffset > fileContent.length-1) {
			throw new ByteFileException("End of File : "+getName());
		}
		byte byteValue =  fileContent[readOffset];
		readOffset++;
		
		int result = byteValue;
		if (result < 0)
			result+=256;
		return result;
	}
	
	public int readWord()  throws ByteFileException {
		if (readOffset > fileContent.length-2) {
			throw new ByteFileException("End of File : "+getName());
		}
		//sur le disque les deux octets sont inversés !
		int octetPoidsFaible = readByte();
		int octetPoidsFort = readByte();
		int result = octetPoidsFort*256 + octetPoidsFaible;
		//file.readOffset+=2;
		return result;
	}
}
