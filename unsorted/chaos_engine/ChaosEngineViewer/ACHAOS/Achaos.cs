using System;
using System.Collections.Generic;
using System.Text;
using ChaosEngineViewer.Properties;

namespace ChaosEngineViewer.ACHAOS {
  public static class Achaos {
    private static Dump [] _dumps = new Dump [3];

    private static ItemType [] [] [] _packsItemsTypes;
    public static SpriteDef [] [] _spriteDefs;
    private static Dictionary<int, SpriteFileDef> [] _spriteFileDefs;
    private static Palette [] [] _palettes;

    static Achaos () {
      _packsItemsTypes = new ItemType [3] [] [];
      _packsItemsTypes [0] = new ItemType [4] [];
      _packsItemsTypes [1] = new ItemType [4] [];
      _packsItemsTypes [2] = new ItemType [4] [];

      _palettes = new Palette [3] [];
      _palettes [0] = new Palette [75];
      _palettes [1] = new Palette [41];
      _palettes [2] = new Palette [38];

      _spriteDefs = new SpriteDef [3] [];

      _spriteFileDefs = new Dictionary<int, SpriteFileDef> [3];
      _spriteFileDefs [0] = new Dictionary<int, SpriteFileDef> ();
      _spriteFileDefs [1] = new Dictionary<int, SpriteFileDef> ();
      _spriteFileDefs [2] = new Dictionary<int, SpriteFileDef> ();
    }

    public static Dump GetDump (VersionEnum version) {
      if (_dumps [(int) version] == null) {
        if (version == VersionEnum.AmigaCD32) {
          _dumps [(int) version] = new Dump (string.Format ("{0}\\ACHAOS", Settings.Default.BaseFolderCD32), 32);
        }
        else if (version == VersionEnum.AmigaOCS) {
          _dumps [(int) version] = new Dump (string.Format ("{0}\\ACHAOS", Settings.Default.BaseFolderOCS), 0);
        }
        else if (version == VersionEnum.AtariST) {
          _dumps [(int) version] = new Dump (string.Format ("{0}\\CHAOS.BIN", Settings.Default.BaseFolderST), 0);
        }
        else {
          throw new Exception ();
        }
      }
      return (_dumps [(int) version]);
    }

    public static ItemType GetItemType (VersionEnum version, int pack, int index) {
      if (_packsItemsTypes [(int) version] [pack] == null) {
        _packsItemsTypes [(int) version] [pack] = new ItemType [40];
        for (int i = 0; i < 40; i++) {
          _packsItemsTypes [(int) version] [pack] [i] = new ItemType (version, pack, i);
        }
      }
      return (_packsItemsTypes [(int) version] [pack] [index]);
    }

    public static SpriteDef GetSpriteDef (VersionEnum version, int index) {
      if (_spriteDefs [(int) version] == null) {
        _spriteDefs [(int) version] = new SpriteDef [43];
        for (int i = 0; i < 43; i++) {
          _spriteDefs [(int) version] [i] = new SpriteDef (version, i);
        }
      }
      return (_spriteDefs [(int) version] [index]);
    }

    public static SpriteFileDef GetSpriteFileDef (VersionEnum version, int address) {
      if (!_spriteFileDefs [(int) version].ContainsKey (address)) {
        _spriteFileDefs [(int) version].Add (address, new SpriteFileDef (version, address));
      }
      return (_spriteFileDefs [(int) version] [address]);
    }

    public static Palette GetPalette (VersionEnum version, int index) {
      if (_palettes [(int) version] [index] == null) {
        _palettes [(int) version] [index] = new Palette (version, index);
      }
      return (_palettes [(int) version] [index]);
    }

    public static int GetBlockSize (VersionEnum version, int type) {
      return (GetDump (version).ReadWord (0x213D8 + type * 2));
    }
  }
}
