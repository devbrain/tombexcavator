using System;
using System.Collections.Generic;
using System.Text;
using ChaosEngineViewer.ACHAOS;
using System.Drawing;

namespace ChaosEngineViewer.LEVSDAT_BIN {
  public class Element00_A_EntityClass {
    public int spriteDefIndex;
    public int unk02_w_health_damage;
    public int unk04_w;
    public int unk06_w;
    public int unk08_w;
    public int unk10_w;
    public int unk12_b;
    public int unk13_b;
    public int unk14_w;
    public int unk16_w;
    public int unk18_w;

    public Element00_A_EntityClass (Dump dump, int address) {
      spriteDefIndex = dump.ReadWord (address);
      address += 2;
      unk02_w_health_damage = dump.ReadWord (address);
      address += 2;
      unk04_w = dump.ReadWord (address);
      address += 2;
      unk06_w = dump.ReadWord (address);
      address += 2;
      unk08_w = dump.ReadWord (address);
      address += 2;
      unk10_w = dump.ReadWord (address);
      address += 2;
      unk12_b = dump.ReadByte (address);
      address += 1;
      unk13_b = dump.ReadByte (address);
      address += 1;
      unk14_w = dump.ReadWord (address);
      address += 2;
      unk16_w = dump.ReadWord (address);
      address += 2;
      unk18_w = dump.ReadWord (address);
      address += 2;
    }

    public SpriteDef GetSpriteDef (VersionEnum version) {
        return (Achaos.GetSpriteDef (version, spriteDefIndex));
    }

    //public Bitmap GetDefaultSprite (int paletteIndex) {
    //      //if (spriteBin != null) {
    //  int? defaultSpriteIndex = SpriteDef.SpriteFileDef.GetDefaultSpriteIndex ();
    //  if (defaultSpriteIndex == null) {
    //    return (null);
    //  }
    //  SpriteBin spriteBin = SpritesManager.GetSpriteBin (spriteDefIndex, paletteIndex);
    //  return (spriteBin.GetBitmap ((int) defaultSpriteIndex));
    //}

    //public SpriteBin GetSpriteBin (int levelIndex, int paletteIndex) {
    //  return (SpritesManager.GetSpriteBin (spriteDefIndex, levelIndex, paletteIndex));
    //}

    public string Description {
      get {
        StringBuilder sb = new StringBuilder ();
        sb.AppendLine (string.Format ("***** ENTITY CLASS *****"));
        sb.AppendLine (string.Format ("spriteDefIndex: {0}", spriteDefIndex));
        return (sb.ToString ());
      }
    }
  }
}
