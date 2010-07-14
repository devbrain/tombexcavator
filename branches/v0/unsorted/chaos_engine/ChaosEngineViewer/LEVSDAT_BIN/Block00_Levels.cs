using System;
using System.Collections.Generic;
using System.Text;

namespace ChaosEngineViewer.LEVSDAT_BIN {
  public class Block00_Levels {
    private int [] addressElements00;
    public Element00 [] elements00;

    public Block00_Levels (Dump dump, int address) {
      int nbElements00 = 20;
      addressElements00 = new int [nbElements00];
      elements00 = new Element00 [nbElements00];
      for (int i = 0; i < nbElements00; i++) {
        addressElements00 [i] = dump.ReadLong (address + i * 4);
        elements00 [i] = new Element00 (dump, addressElements00 [i]);
      }
    }
  }
}
