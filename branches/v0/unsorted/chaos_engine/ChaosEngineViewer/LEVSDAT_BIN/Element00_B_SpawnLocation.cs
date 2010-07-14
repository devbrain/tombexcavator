using System;
using System.Collections.Generic;
using System.Text;

namespace ChaosEngineViewer.LEVSDAT_BIN {
  public class Element00_B_SpawnLocation {
    public int x;
    public int y;

    public Element00_B_SpawnLocation (Dump dump, int address) {
      x = dump.ReadWord (address);
      address += 2;
      y = dump.ReadWord (address);
      address += 2;
    }
  }
}
