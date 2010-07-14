package files_recognizer;


import org.xml.sax.*;
import org.xml.sax.helpers.LocatorImpl;



public class FileRecognizerContentHandler implements ContentHandler {

		FileRecognizer recognizer;
		
		int step = -1;
		
		public static int STEP_EXTENSIONS 	= 0;
		public static int STEP_GAMES 		= 1;
		public static int STEP_FILENAMES	= 2;
	
       public FileRecognizerContentHandler(FileRecognizer recognizer) {
            super();
            
            this.recognizer = recognizer;
            // On definit le locator par defaut.
            locator = new LocatorImpl();
        }

       public void setDocumentLocator(Locator value) {
                locator =  value;
        }

        public void startDocument() throws SAXException {
                //System.err.println("Debut de l'analyse du document");
        }

        public void endDocument() throws SAXException {
                //System.err.println("Fin de l'analyse du document" );
        }

        public void startPrefixMapping(String prefix, String URI) throws SAXException {
                System.err.println("Traitement de l'espace de nommage : " + URI + ", prefixe choisi : " + prefix);
        }

        public void endPrefixMapping(String prefix) throws SAXException {
                System.err.println("Fin de traitement de l'espace de nommage : " + prefix);
        }

        public void startElement(String nameSpaceURI, String localName, String rawName, Attributes attributs) throws SAXException {
                System.err.println("<" + localName+ ">");

                if ( ! "".equals(nameSpaceURI)) { // espace de nommage particulier
                        System.err.println("  appartenant a l'espace de nom : "  + nameSpaceURI);
                }

                if ("files_types".equals(localName.toLowerCase())) {
                	step = STEP_EXTENSIONS;
                }
                else if ("games".equals(localName.toLowerCase())) {
                	step = STEP_GAMES;
                }
                else if ("files_names".equals(localName.toLowerCase())) {
                	step = STEP_FILENAMES;
                }
                
                if (attributs.getLength() > 0) {
                	//list attributes
                	System.err.println("  Attributes : ");
	                for (int index = 0; index < attributs.getLength(); index++) { // on parcourt la liste des attributs
	                    System.err.println("     - " +  attributs.getLocalName(index) + " = " + attributs.getValue(index));
	                }
	                
	                //use attributes
	                if (step == STEP_EXTENSIONS) {
	                	recognizer.addExtension(attributs.getValue(0).toLowerCase());
	                } else if (step == STEP_GAMES) {
	                	recognizer.addGame(attributs.getValue(0).toLowerCase());
	                } else if (step == STEP_FILENAMES) {
	                	recognizer.addFile(	attributs.getValue(0).toLowerCase(),
	                						attributs.getValue(1).toLowerCase(),
	                						attributs.getValue(2).toLowerCase());
	                }
                }
                
        }

        public void endElement(String nameSpaceURI, String localName, String rawName) throws SAXException {
            	System.err.println("</" + localName+ ">");

                //step = -1;
                
                if ( ! "".equals(nameSpaceURI)) { // name space non null
                        System.err.print("appartenant a l'espace de nommage : " + localName);
                }

                System.err.println();
        }

        public void characters(char[] ch, int start, int end) throws SAXException {
                //System.err.println("#PCDATA : " + new String(ch, start, end));
        }

        public void ignorableWhitespace(char[] ch, int start, int end) throws SAXException {
                //System.err.println("espaces inutiles rencontres : ..." + new String(ch, start, end) +  "...");
        }

        public void processingInstruction(String target, String data) throws SAXException {
                //System.err.println("Instruction de fonctionnement : " + target);
                //System.err.println("  dont les arguments sont : " + data);
        }

       public void skippedEntity(String arg0) throws SAXException {
                // Je ne fais rien, ce qui se passe n'est pas franchement normal.
                // Pour eviter cet evenement, le mieux est quand meme de specifier une dtd pour vos
                // documents xml et de les faire valider par votre parser.              
        }

        private Locator locator;

}

 