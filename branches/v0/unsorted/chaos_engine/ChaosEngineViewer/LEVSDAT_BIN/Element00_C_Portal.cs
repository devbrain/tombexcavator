using System;
using System.Collections.Generic;
using System.Text;

namespace ChaosEngineViewer.LEVSDAT_BIN {
  public class Element00_C_Portal {
    private int _dstLevelIndexRaw;
    public int spawnLocationIndex;
    public int enteringHeroEffect;

    public Element00_C_Portal (Dump dump, int address) {
      _dstLevelIndexRaw = dump.ReadWord (address);
      address += 2;
      spawnLocationIndex = dump.ReadWord (address);
      address += 2;
      enteringHeroEffect = dump.ReadWord (address);
      address += 2;

      if (_dstLevelIndexRaw < 65) {
        throw new Exception ();
      }
      if (_dstLevelIndexRaw > 80) {
        throw new Exception ();
      }
      if (enteringHeroEffect != 0) {
        //throw new Exception ();
      }
    }

    public int LevelIndex {
      get {
        return (_dstLevelIndexRaw - 65);
      }
    }
  }
}
