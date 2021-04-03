package files_recognizer;

import java.util.ArrayList;
import java.util.Iterator;

import org.xml.sax.XMLReader;
import org.xml.sax.helpers.XMLReaderFactory;

import viewer.LoadException;

public class FileRecognizer {

	
	private ArrayList<String> knownExtensions = new ArrayList<String>();
	private ArrayList<CryoGame> knownGames = new ArrayList<CryoGame>();
	private ArrayList<CryoFile> knownFiles = new ArrayList<CryoFile>();
	
    public FileRecognizer(String uri) throws LoadException {
        try {
	        XMLReader saxReader = XMLReaderFactory.createXMLReader("org.apache.xerces.parsers.SAXParser");
	        saxReader.setContentHandler(new FileRecognizerContentHandler(this));       
         	saxReader.parse(uri);
        } catch (Exception ex) {
        	throw new LoadException("couldn't load Cryo files definitions or file corrupted : "+uri+"\n\nDetails : "+ex.getMessage());
        }
        
    }

	
	public CryoFile getCryoFile(String fileStr) {
		fileStr = fileStr.toLowerCase();
		Iterator<CryoFile> it = knownFiles.iterator();
		while (it.hasNext()) {
			CryoFile file = it.next();
			if (file.name.equals(fileStr)) {
				//System.err.println(fileStr+" equals "+file.name);
				return file;
			} else {
				//System.err.println(fileStr+" is different from "+file.name);
			}
		}
		return null;
	}
	
	public CryoGame getCryoGame(String name) {
		name = name.toLowerCase();
		Iterator<CryoGame> it = knownGames.iterator();
		while (it.hasNext()) {
			CryoGame game = it.next();
			if (game.name.equals(name)) {
				return game;
			} else {
				//System.err.println(game.name+" is different from "+name.toLowerCase());
			}
		}
		return null;
	}
	
	
	public boolean isCryoExtension(String ext) {
		return knownExtensions.contains(ext);
	}
	
	public void addExtension(String ext) {
		ext= ext.toLowerCase();
		knownExtensions.add(ext);
		System.err.println("registering Cryo file extension : "+ext);
	}

	public void addGame(String name) {
		name = name.toLowerCase();
		CryoGame game = new CryoGame();
		game.name = name;		
		knownGames.add(game);
		System.err.println("registering Cryo game : "+name);
	}

	public void addFile(String name, String gameStr, String type) {
		
		gameStr = gameStr.toLowerCase();
		type = type.toLowerCase();
		name = name.toLowerCase();
		
		CryoGame game = getCryoGame(gameStr);
		if (game != null && isCryoExtension(type)) {
			System.err.println("registering Cryo game file : "+name);
			CryoFile file = new CryoFile();
			file.name = name;
			file.game = game;
			file.type = type;
			knownFiles.add(file);
		}
		else
		{
			System.err.println("Cryo files index : File '"+name+"' doesn't seem to be a valid Cryo File.");
		}
	}
}