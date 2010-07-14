using System;
using System.Collections.Generic;
using System.Text;
using System.Drawing;
using ChaosEngineViewer.LEVSDAT_BIN;

namespace ChaosEngineViewer {
  public class Entity {
    private VersionEnum _version;

    public int mapX;
    public int mapY;
    public int unk04_b_health_damage;
    public int unk05_b;
    public int unk06_b;
    public int unk07_b;
    public int linkId08_b;
    public int unk09_b;
    public int unk10_b;
    public int entityClassIndex;
    public int unk12_b;
    public int unk13_b;
    public int unk14_b;
    public int unk15_b;
    public int unk16_b;
    public int linkId17_b;
    public int unk18_w;

    public int? spriteIndex;
    public int score;

    public Entity (VersionEnum version, Dump dump, int address, int levelIndex) {
      _version = version;
      // size: 20 bytes
      mapX = dump.ReadWord (address);
      address += 2;
      mapY = dump.ReadWord (address);
      address += 2;
      unk04_b_health_damage = dump.ReadByte (address);
      address += 1;
      unk05_b = dump.ReadByte (address);
      address += 1;
      unk06_b = dump.ReadByte (address);
      address += 1;
      unk07_b = dump.ReadByte (address);
      address += 1;
      linkId08_b = dump.ReadByte (address);
      address += 1;
      unk09_b = dump.ReadByte (address);
      address += 1;
      unk10_b = dump.ReadByte (address);
      address += 1;
      entityClassIndex = dump.ReadByte (address);
      address += 1;
      unk12_b = dump.ReadByte (address);
      address += 1;
      unk13_b = dump.ReadByte (address);
      address += 1;
      unk14_b = dump.ReadByte (address);
      address += 1;
      unk15_b = dump.ReadByte (address);
      address += 1;
      unk16_b = dump.ReadByte (address);
      unk16_b = dump.ReadByte (address);
      address += 1;
      linkId17_b = dump.ReadByte (address);
      address += 1;
      unk18_w = dump.ReadWord (address);

      Init (levelIndex);
    }

    public Entity (VersionEnum version, int x_, int y_, int linkId08_b_, int entityClassIndex_, int linkId17_b_, int levelIndex) {
      _version = version;

      mapX = x_;
      mapY = y_;
      linkId08_b = linkId08_b_;
      entityClassIndex = entityClassIndex_;
      linkId17_b = linkId17_b_;

      Element00_A_EntityClass entityClass = GetEntityClass (levelIndex);
      unk04_b_health_damage = entityClass.unk02_w_health_damage & 0xFF; // ??
      unk05_b = entityClass.unk04_w;
      unk06_b = entityClass.unk14_w;
      unk07_b = entityClass.unk18_w;
      unk09_b = entityClass.unk16_w;
      unk10_b = entityClass.GetSpriteDef (version).unk16_w;
      unk12_b = entityClass.unk06_w;
      unk13_b = entityClass.unk08_w * 25 / 17; // ~ 0x13EC8
      unk14_b = entityClass.unk12_b;
      unk15_b = entityClass.unk13_b;
      unk16_b = entityClass.unk10_w;
      unk18_w = 0;

      Init (levelIndex);
    }

    private void Init (int levelIndex) {
      //string fileName = GetEntityClass (levelIndex).SpriteDef.SpriteFileDef.fileName;
      //if (_defaultSpriteIndex.ContainsKey (fileName)) {
      //  spriteIndex = _defaultSpriteIndex [fileName];
      //}
      score = LevsdatBin.block00_Levels [(int) _version].elements00 [levelIndex].GetElement00_D_EntityClassesScore (entityClassIndex).score;
    }

    public Element00_A_EntityClass GetEntityClass (int levelIndex) {
      return (LevsdatBin.block00_Levels [(int) _version].elements00 [levelIndex].elements00_A_EntityClasses [entityClassIndex]);
    }

    //private SpriteBin GetSpriteBin (int levelIndex, int paletteIndex) {
    //  return (GetEntityClass (levelIndex).GetSpriteBin (levelIndex, paletteIndex));
    //}

    public void GetSize (int levelIndex, int paletteIndex, out int w, out int h) {
      SpriteBin spriteBin = SpritesManager.GetSpriteBinA (_version, GetEntityClass (levelIndex).GetSpriteDef (_version).spriteFileDefAddress, levelIndex, paletteIndex);
      //Bitmap bitmap = .GetBitmap (0);
      //lock (bitmap) {
      w = spriteBin.WidthSingle;
      h = spriteBin.HeightSingle;
      //}
    }

    public Bitmap GetBitmap (int levelIndex, int paletteIndex, out bool isFirstSprite) {
      return (SpritesManager.GetSprite (_version, entityClassIndex, levelIndex, paletteIndex, spriteIndex, out isFirstSprite));
    }

    public string Description {
      get {
        StringBuilder sb = new StringBuilder ();
        sb.AppendLine (string.Format ("***** ENTITY *****"));
        sb.AppendLine (string.Format ("mapX: {0}", mapX));
        sb.AppendLine (string.Format ("mapY: {0}", mapY));
        sb.AppendLine (string.Format ("linkId08_b: {0}", linkId08_b));
        sb.AppendLine (string.Format ("entityClassIndex: {0}", entityClassIndex));
        sb.AppendLine (string.Format ("linkId17_b: {0}", linkId17_b));
        sb.AppendLine (string.Format ("unk04_b_health_damage: {0}", unk04_b_health_damage));
        sb.AppendLine (string.Format ("unk05_b: {0}", unk05_b));
        sb.AppendLine (string.Format ("unk06_b: {0}", unk06_b));
        sb.AppendLine (string.Format ("unk07_b: {0}", unk07_b));
        sb.AppendLine (string.Format ("unk09_b: {0}", unk09_b));
        sb.AppendLine (string.Format ("unk10_b: {0}", unk10_b));
        sb.AppendLine (string.Format ("unk12_b: {0}", unk12_b));
        sb.AppendLine (string.Format ("unk13_b: {0}", unk13_b));
        sb.AppendLine (string.Format ("unk14_b: {0}", unk14_b));
        sb.AppendLine (string.Format ("unk15_b: {0}", unk15_b));
        sb.AppendLine (string.Format ("unk16_b: {0}", unk16_b));
        sb.AppendLine (string.Format ("unk18_w: {0}", unk18_w));
        sb.AppendLine (string.Format ("score: {0}", score));
        return (sb.ToString ());
      }
    }

    //public int? GetDefaultSpriteIndex () {
    //  if (!_defaultSpriteIndex.ContainsKey (fileName)) {
    //    return (null);
    //  }
    //  return (_defaultSpriteIndex [fileName]);
    //}
  }
}
