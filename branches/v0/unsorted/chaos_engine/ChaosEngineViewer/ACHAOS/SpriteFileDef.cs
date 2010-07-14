using System;
using System.Collections.Generic;
using System.Text;
using System.IO;
using ChaosEngineViewer.Properties;

namespace ChaosEngineViewer.ACHAOS {
  public class SpriteFileDef : ICloneable {
    private VersionEnum _version;

    public int type;
    public int nbBlocks;
    public int unkIndex;
    private int paletteOffsetX16; // *16
    private int paletteOffsetX16Unk;
    public string _fileName;

    public SpriteFileDef (VersionEnum version, int address) {
      _version = version;
      Dump dump = Achaos.GetDump (version);

      type = dump.ReadWord (address);
      address += 2;
      nbBlocks = dump.ReadWord (address);
      address += 2;
      unkIndex = dump.ReadWord (address);
      address += 2;
      if (version == VersionEnum.AmigaCD32) {
        paletteOffsetX16 = dump.ReadByteSigned (address);
        address += 1;
        paletteOffsetX16Unk = dump.ReadByte (address);
        address += 1;
      }

      _fileName = dump.ReadString (address);
    }

    public int GetPaletteOffset (int levelIndex) {
      if (paletteOffsetX16 < 0) { // only for thumper.bin?
        if ((levelIndex >> 2) == 0) { // world 1
          return (15 << 4);
        }
        else {
          return (11 << 4);
        }
      }
      // hack for sp8x8, coin is 9, bullets are initial (10), ...
      //if (fileName == "sp8x8.bin") {
      //  return (9 * 16);
      //}
      return (paletteOffsetX16 << 4);
    }

    public SpriteBin CreateSpriteBin (int paletteIndex, int paletteOffset) {
      Console.WriteLine (string.Format ("Creating sprite {0} (palette index: {1}, palette offset: {2})", _fileName, paletteIndex, paletteOffset));
      string basefolder;
      if (_version == VersionEnum.AmigaCD32) {
        basefolder = Settings.Default.BaseFolderCD32;
      }
      else if (_version == VersionEnum.AmigaOCS) {
        basefolder = Settings.Default.BaseFolderOCS;
      }
      else if (_version == VersionEnum.AtariST) {
        basefolder = Settings.Default.BaseFolderST;
      }
      else {
        throw new Exception ();
      }
      string fileName = Path.Combine (basefolder, _fileName);
      Palette palette = Achaos.GetPalette (_version, paletteIndex);

      // hack for static.bin
      if (_version == VersionEnum.AmigaOCS) {
        if (_fileName == "nodes.bin") {
          return (new SpriteBin (_version, Path.Combine (basefolder, "static.bin"), null, null, 0x4880, 12, 16, 48, 4, SpriteBin.ModesEnum.Scanline, 0, palette, 0, SpriteBin.MasksEnum.Transparent, 0));
        }
        if (_fileName == "expl1616.bin") {
          return (new SpriteBin (_version, Path.Combine (basefolder, "static.bin"), null, null, 0x1500, 103, 16, 16, 4, SpriteBin.ModesEnum.Scanline, 0, palette, 0, SpriteBin.MasksEnum.Transparent, 0));
        }
      }
      else if (_version == VersionEnum.AtariST) {
        if (_fileName == "nodes.bin") {
          return (new SpriteBin (_version, Path.Combine (basefolder, "static.bin"), null, null, 0x4B00, 10, 16, 48, 4, SpriteBin.ModesEnum.Interleaved, 2, palette, 0, SpriteBin.MasksEnum.Transparent, 0));
        }
        if (_fileName == "expl1616.bin") {
          return (new SpriteBin (_version, Path.Combine (basefolder, "static.bin"), null, null, 0x1500, 108, 16, 16, 4, SpriteBin.ModesEnum.Interleaved, 2, palette, 0, SpriteBin.MasksEnum.Transparent, 0));
        }
      }
      return (new SpriteBin (_version, fileName, type, nbBlocks, palette, paletteOffset, SpriteBin.MasksEnum.Transparent, 0));
    }

    public object Clone () {
      return (this.MemberwiseClone ());
    }
  }
}
