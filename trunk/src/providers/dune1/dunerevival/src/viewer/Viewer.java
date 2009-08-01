package viewer;

import files_recognizer.FileRecognizer;

public class Viewer {

	public FileRecognizer recognizer;
	
	public void load() throws LoadException {
		recognizer = new FileRecognizer("cryofiles_index.xml");			
	}
}
