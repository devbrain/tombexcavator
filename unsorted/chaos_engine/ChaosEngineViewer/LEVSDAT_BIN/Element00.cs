using System;
using System.Collections.Generic;
using System.Text;

namespace ChaosEngineViewer.LEVSDAT_BIN {
  public class Element00 {
    private int addressA;
    private int addressB_SpawnLocations;
    private int addressC_Portals;
    public int tileSet;
    private int addressD;

    public Element00_A_EntityClass [] elements00_A_EntityClasses;
    public Element00_B_SpawnLocation [] elements00_B_SpawnLocations;
    private Dump _dump;
    private Dictionary<int, Element00_C_Portal> _elements00_C_Portals;
    private Dictionary<int, Element00_D_EntityClassesScore> _elements00_D_EntityClassIndexesScores;

    public Element00 (Dump dump, int address) {
      addressA = dump.ReadLong (address);
      address += 4;
      addressB_SpawnLocations = dump.ReadLong (address);
      address += 4;
      addressC_Portals = dump.ReadLong (address);
      address += 4;
      tileSet = dump.ReadWord (address); // level tile set (1-5) 1: 1, 2: 2, 3: 3, 4: 4+5, 5: 4+6
      address += 2;
      addressD = dump.ReadLong (address);
      address += 4;

      int nbElements00_A = 0;
      int tempAddress = addressA;
      for (; ; ) {
        int end = dump.ReadWordSigned (tempAddress);
        if (end < 0) {
          break;
        }
        nbElements00_A++;
        tempAddress += 20;
      }

      elements00_A_EntityClasses = new Element00_A_EntityClass [nbElements00_A];
      for (int i = 0; i < nbElements00_A; i++) {
        Element00_A_EntityClass entityClass = new Element00_A_EntityClass (dump, addressA + i * 20);
        elements00_A_EntityClasses [i] = entityClass;
      }

      int nbElements00_B_SpawnLocations = (addressC_Portals - addressB_SpawnLocations) / 4;
      elements00_B_SpawnLocations = new Element00_B_SpawnLocation [nbElements00_B_SpawnLocations];
      for (int i = 0; i < nbElements00_B_SpawnLocations; i++) {
        elements00_B_SpawnLocations [i] = new Element00_B_SpawnLocation (dump, addressB_SpawnLocations + i * 4);
      }

      _dump = dump;
      _elements00_C_Portals = new Dictionary<int, Element00_C_Portal> ();
      _elements00_D_EntityClassIndexesScores = new Dictionary<int, Element00_D_EntityClassesScore> ();
    }

    public void Dump (VersionEnum version, int levelIndex) {
      foreach (Element00_A_EntityClass entityClass in elements00_A_EntityClasses) {
        Console.WriteLine (string.Format ("{0}: {1} {2}", levelIndex, entityClass.GetSpriteDef (version).SpriteFileDef._fileName, entityClass.GetSpriteDef (version).SpriteFileDef.GetPaletteOffset (levelIndex)));
      }
    }

    public Element00_C_Portal GetElement00_C_Portal (int portalIndex) {
      lock (_elements00_C_Portals) {
        if (!_elements00_C_Portals.ContainsKey (portalIndex)) {
          _elements00_C_Portals.Add (portalIndex, new Element00_C_Portal (_dump, addressC_Portals + portalIndex * 6));
        }
      }
      return (_elements00_C_Portals [portalIndex]);
    }

    public Element00_D_EntityClassesScore GetElement00_D_EntityClassesScore (int entityClassIndex) {
      lock (_elements00_D_EntityClassIndexesScores) {
        if (!_elements00_D_EntityClassIndexesScores.ContainsKey (entityClassIndex)) {
          _elements00_D_EntityClassIndexesScores.Add (entityClassIndex, new Element00_D_EntityClassesScore (_dump, addressD + entityClassIndex * 2));
        }
      }
      return (_elements00_D_EntityClassIndexesScores [entityClassIndex]);
    }
  }
}
