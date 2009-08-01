package cryo_core;

import java.io.BufferedReader;
import java.io.File;
import java.io.InputStreamReader;
import java.util.StringTokenizer;

public class HSQ {


	//runs UNHSQ.COM as an external program and retrieves result as an array of bytes
	public static ByteFile externalUnHSQ(File inFile) throws HSQException{
		
		ByteFile result;
		
		try {
			//get location of file to unhsq
			String path = inFile.getParent()+"\\";
			System.err.println("trying to unhsq file from directory "+path);
			
			//get name of file (without extension)
			StringTokenizer tk = new StringTokenizer(inFile.getName(), ".");
			String absoluteFileName = "ERROR";
			
			for (int i=0; i<tk.countTokens()-1; i++)
				absoluteFileName = tk.nextToken();
			System.err.println("absolute file name : "+absoluteFileName.toUpperCase());
			
			//copy unhsq to dune directory
			//--> Needed because unhsq reads directly the file
			// (name 8 characters long + ext.) from local directory !
			copyUnhsq(path);
			
			//execute unhsq externaly
			String cmdline = path+"unhsq.com "+inFile.getName();
			System.err.println("Trying to execute external command "+cmdline+" in "+path);
			
			//process to be used to launch unhsq externally :
			Process p;
			try {
				String line;
				//executing unhsq with no particular environment but
				//using "path" as working directory
				p = Runtime.getRuntime().exec(cmdline, null, new File(path));
				BufferedReader input = new BufferedReader(new InputStreamReader(p.getInputStream()));
				while ((line = input.readLine()) != null) {
					System.out.println(line);
				}
				input.close();
			} 
			catch (Exception err)
			{
				err.printStackTrace();
				throw new HSQException("couldn't execute unhsq externaly");
			}
			
			//waiting for external process to terminate
			System.err.println("Waiting for external process to terminate");
			p.waitFor();
			
			//retrieve result
			String outFileName = path+absoluteFileName+".___";
			System.err.println("Trying to read from uncompressed file : "+outFileName);
			result = new ByteFile(outFileName);
		}
		catch (Exception ex) {
			throw new HSQException("Error during external UNHSQ process : "+ex.getMessage());
		}
		return result;
	}
	
	public static byte[] unHSQ() {
		// TODO : tranlate unhsq from assembly to Java
		return new byte[1];
	}
	
	
	//copies unhsq.com to the specified directory
	public static void copyUnhsq(String destPath) throws HSQException {
		try {
			System.err.println("Copying UNHSQ.COM to "+destPath);
			String sourceFile = "./ressource/unhsq.com";
			String destFile = destPath+"unhsq.com";
			if (!copyFile(new File(sourceFile), new File(destFile)))
				throw new HSQException("");
		} catch (Exception ex) {
			throw new HSQException("Failed to copy UNHSQ.COM to Dune directory");
		}
	}
	
	public static boolean copyFile( File source, File destination ) throws HSQException
	{
	        boolean result = false;
	        // Declaration des flux
	        java.io.FileInputStream sourceFile=null;
	        java.io.FileOutputStream destinationFile=null;
	        try {
	                // Création du fichier :
	                destination.createNewFile();
	                // Ouverture des flux
	                sourceFile = new java.io.FileInputStream(source);
	                destinationFile = new java.io.FileOutputStream(destination);
	                // Lecture par segment de 0.5Mo 
	                byte buffer[]=new byte[512*1024];
	                int counterRead;
	                while( (counterRead = sourceFile.read(buffer)) != -1 ) {
	                        destinationFile.write(buffer, 0, counterRead);
	                } 
	                // Copie réussie
	                result = true;
	        } catch( java.io.FileNotFoundException f ) {
	        } catch( java.io.IOException e ) {
	        } finally {
	                // Quoi qu'il arrive, on ferme les flux
	                try {
	                        sourceFile.close();
	                } catch(Exception e) { }
	                try {
	                        destinationFile.close();
	                } catch(Exception e) { }
	                
	        } 
	        return( result );
	}
	
	
	public static void main(String[] args) {
		try {
			HSQ.externalUnHSQ(new File("C:\\Documents and Settings\\Administrateur\\Bureau\\loisirs\\dune\\Dune_Fr\\WATER.HSQ"));
		} catch (HSQException ex) {
			System.err.println(ex.getMessage());
		}
	}

}
