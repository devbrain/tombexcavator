using System;
using System.Collections.Generic;
using System.Text;

namespace ChaosEngineViewer.LEVSDAT_BIN {
  public class Element00_D_EntityClassesScore {
    public int score;

    public Element00_D_EntityClassesScore (Dump dump, int address) {
      score = dump.ReadWord (address);
      address += 2;
    }
  }
}
