using System;
using System.Collections.Generic;
using System.Text;

namespace ChaosEngineViewer.LEVSDAT_BIN {
  public class Block16_ScriptsClasses {
    public int addressBlock16;
    public int [] offsetElements16_A;
    public int [] offsetElements16_B;
    public int [] offsetElements16_C;
    public int [] offsetElements16_D;
    public Element16_ScriptClass [] elements16;

    public Block16_ScriptsClasses (Dump dump, int address) {
      addressBlock16 = address;
      int nbElements16 = 61;
      offsetElements16_A = new int [nbElements16];
      offsetElements16_B = new int [nbElements16];
      offsetElements16_C = new int [nbElements16];
      offsetElements16_D = new int [nbElements16];
      elements16 = new Element16_ScriptClass [nbElements16];
      for (int i = 0; i < nbElements16; i++) {
        offsetElements16_A [i] = dump.ReadLong (address);
        address += 4;
        offsetElements16_B [i] = dump.ReadLong (address);
        address += 4;
        offsetElements16_C [i] = dump.ReadLong (address);
        address += 4;
        offsetElements16_D [i] = dump.ReadLong (address);
        address += 4;
        elements16 [i] = new Element16_ScriptClass (dump, i, addressBlock16, offsetElements16_A [i], offsetElements16_B [i], offsetElements16_C [i], offsetElements16_D [i]);

        //int hasSpawnMonster = elements16 [i].script2.HasSpawnMonster ();
      }
    }
  }
}
