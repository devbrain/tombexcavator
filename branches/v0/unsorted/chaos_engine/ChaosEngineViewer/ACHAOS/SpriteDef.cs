using System;
using System.Collections.Generic;
using System.Text;

namespace ChaosEngineViewer.ACHAOS {
  public class SpriteDef {
    private VersionEnum _version;
    //private Dump _dump;

    public int address00;
    public int pFunction;
    public int spriteFileDefAddress;
    public int offset12_binAddressforImage;
    public int unk14_b;
    public int unk15_b;
    public int unk16_w;
    public int xOffset;
    public int yOffset;
    public int address22;
    public int address26;
    private static Random _random = new Random ();

    public SpriteDef (VersionEnum version, int index) {
      _version = version;
      Dump dump = Achaos.GetDump (version);

      int address;
      if (version == VersionEnum.AmigaCD32) {
        address = 0x216C6;
      }
      else if (version == VersionEnum.AmigaOCS) {
        address = 0x1BD5E;
      }
      else if (version == VersionEnum.AtariST) {
        address = 0x19282;
      }
      else {
        throw new Exception ();
      }
      address += index * 30;

      address00 = dump.ReadLong (address);
      address += 4;
      pFunction = dump.ReadLong (address);
      address += 4;
      spriteFileDefAddress = dump.ReadLong (address);
      address += 4;
      offset12_binAddressforImage = dump.ReadWord (address);
      address += 2;
      unk14_b = dump.ReadByte (address);
      address += 1;
      unk15_b = dump.ReadByte (address);
      address += 1;
      unk16_w = dump.ReadWord (address);
      address += 2;
      xOffset = dump.ReadWordSigned (address);
      address += 2;
      yOffset = dump.ReadWordSigned (address);
      address += 2;
      address22 = dump.ReadLong (address);
      address += 4;
      address26 = dump.ReadLong (address);
      address += 4;

      // patch for ingame initialization
      if (index == 7) { // nodes_bin
        if (version == VersionEnum.AmigaCD32) {
          spriteFileDefAddress = 0x18026;
        }
        else if (version == VersionEnum.AmigaOCS) {
          spriteFileDefAddress = 0x12D3E;
        }
        else if (version == VersionEnum.AtariST) {
          spriteFileDefAddress = 0x112E8;
        }
        else {
          throw new Exception ();
        }
      }
      if (index == 8) { // nodes_bin activated?
        if (version == VersionEnum.AmigaCD32) {
          spriteFileDefAddress = 0x18026;
        }
        else if (version == VersionEnum.AmigaOCS) {
          spriteFileDefAddress = 0x12D3E;
        }
        else if (version == VersionEnum.AtariST) {
          spriteFileDefAddress = 0x112E8;
        }
        else {
          throw new Exception ();
        }
      }

      // patch for enemy cloning player1
      if (index == 42) {
        int heroIndex = _random.Next (5);
        int heroeSpriteDefAddress;
        if (version == VersionEnum.AmigaCD32) {
          heroeSpriteDefAddress = dump.ReadLong (0x21BE0 + 12 * heroIndex + 8);
        }
        else if (version == VersionEnum.AmigaOCS) {
          heroeSpriteDefAddress = dump.ReadLong (0x1C278 + 12 * heroIndex + 8);
        }
        else if (version == VersionEnum.AtariST) {
          heroeSpriteDefAddress = dump.ReadLong (0x1979C + 12 * heroIndex + 8);
        }
        else {
          throw new Exception ();
        }
        spriteFileDefAddress = heroeSpriteDefAddress;
      }

      if (version == VersionEnum.AtariST) {
        spriteFileDefAddress -= 0x800;
      }
    }

    //public SpriteBin GetSpriteBin (int levelIndex, int paletteIndex) {
    //  return (SpritesManager.GetSpriteBinFromAddress (spriteFileDefAddress, levelIndex, paletteIndex));
    //}

    public SpriteFileDef SpriteFileDef {
      get {
        return (Achaos.GetSpriteFileDef (_version, spriteFileDefAddress));
      }
    }
  }
}
