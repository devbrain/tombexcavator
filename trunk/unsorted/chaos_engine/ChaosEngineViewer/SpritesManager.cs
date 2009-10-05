using System;
using System.Collections.Generic;
using System.Text;
using ChaosEngineViewer.ACHAOS;
using System.Drawing;
using ChaosEngineViewer.LEVSDAT_BIN;
using System.IO;
using ChaosEngineViewer.Properties;

namespace ChaosEngineViewer {
  public static class SpritesManager {
    private static Dictionary<string, SpriteBin> [] _sprites;
    private static Dictionary<int, SpritesEnum> [] _spriteFileDefAddresses;
    private static Dictionary<SpritesEnum, int> _defaultSpriteIndex;
    //private static Dictionary<string, SpriteBin> _spritesHeroesHead = new Dictionary<string, SpriteBin> ();
    //public static SpriteBin _panelBackgrounds;
    //public static SpriteBin _panelPowerBackgrounds;
    //public static SpriteBin _panelNumbersSmall;
    //public static SpriteBin _panelPowerBars;
    //public static SpriteBin _panelNumbersMedium;
    //public static SpriteBin _panelPowers;

    public enum SpritesEnum {
      // missings
      getready_bin,
      tranmask_bin,
      expl3232_bin,
      brigand_bin,
      mercenry_bin,
      gentlman_bin,
      navvie_bin,
      thug_bin,
      preacher_bin,
      sp8x8_bin,
      emplgun_bin,
      barongun_bin,
      panel_bin, // bug, sprites with different height

      // ok
      sewer_bin,
      beetle_bin,
      nodes_bin, // same as static.bin
      dhturret_bin,
      blob_bin,
      rockman_bin,
      spider_bin,
      thumper_bin,
      compost_bin,
      misslink_bin,
      fire_man_bin,
      emplacmt_bin,
      mechanic_bin,
      gyro_big_bin,
      frogger_bin,
      lizman_bin,
      slug_bin,
      guard_bin,
      dustdevl_bin,
      kangaroo_bin,
      beast_bin,
      steamjet_bin,
      stonewat_bin,
      cocoon_bin,
      lobber_bin,
      hand_bin,
      mace_bin,
      rat_bin,
      engyball_bin,
      robot_bin,
      halftrak_bin,
      revdome_bin,
      expl1616_bin,
      thebaron_bin
    }
    /*
    = {
      // missings
      0x17FFA, 0x18060, 0x1804A, 0x17F7E, 0x17F92, 0x17FA8, 0x17FBE, 0x17FD2, 0x17FE4, 0x18038, 0x1826A,
      0x18322, 0x18076,
      // ok
      0x18162, 0x1809C, 0x18026, 0x18136, 0x18100, 0x18088, 0x181F2, 0x180EC,
      0x180B0, 0x1821A, 0x181A0, 0x18254, 0x182E0, 0x182A4, 0x180C4, 0x180D8,
      0x18230, 0x18112, 0x1814C, 0x1818A, 0x18124, 0x18174, 0x181C8, 0x181DE,
      0x18206, 0x181B6, 0x18242, 0x182BA, 0x182F6, 0x1827E, 0x182CA, 0x18290,
      0x18010, 0x1830C
    };
    */
    static SpritesManager () {
      _sprites = new Dictionary<string, SpriteBin> [3];
      _sprites [0] = new Dictionary<string, SpriteBin> ();
      _sprites [1] = new Dictionary<string, SpriteBin> ();
      _sprites [2] = new Dictionary<string, SpriteBin> ();

      _spriteFileDefAddresses = new Dictionary<int, SpritesEnum> [3];

      _spriteFileDefAddresses [0] = new Dictionary<int, SpritesEnum> ();
      _spriteFileDefAddresses [0].Add (0x17FFA, SpritesEnum.getready_bin);
      _spriteFileDefAddresses [0].Add (0x18060, SpritesEnum.tranmask_bin);
      _spriteFileDefAddresses [0].Add (0x1804A, SpritesEnum.expl3232_bin);

      _spriteFileDefAddresses [0].Add (0x17F7E, SpritesEnum.brigand_bin);
      _spriteFileDefAddresses [0].Add (0x17F92, SpritesEnum.mercenry_bin);
      _spriteFileDefAddresses [0].Add (0x17FA8, SpritesEnum.gentlman_bin);
      _spriteFileDefAddresses [0].Add (0x17FBE, SpritesEnum.navvie_bin);
      _spriteFileDefAddresses [0].Add (0x17FD2, SpritesEnum.thug_bin);
      _spriteFileDefAddresses [0].Add (0x17FE4, SpritesEnum.preacher_bin);

      _spriteFileDefAddresses [0].Add (0x18038, SpritesEnum.sp8x8_bin);
      _spriteFileDefAddresses [0].Add (0x1826A, SpritesEnum.emplgun_bin);
      _spriteFileDefAddresses [0].Add (0x18322, SpritesEnum.barongun_bin);
      _spriteFileDefAddresses [0].Add (0x18076, SpritesEnum.panel_bin);

      _spriteFileDefAddresses [0].Add (0x18162, SpritesEnum.sewer_bin);
      _spriteFileDefAddresses [0].Add (0x1809C, SpritesEnum.beetle_bin);
      _spriteFileDefAddresses [0].Add (0x18026, SpritesEnum.nodes_bin);
      _spriteFileDefAddresses [0].Add (0x18136, SpritesEnum.dhturret_bin);
      _spriteFileDefAddresses [0].Add (0x18100, SpritesEnum.blob_bin);
      _spriteFileDefAddresses [0].Add (0x18088, SpritesEnum.rockman_bin);
      _spriteFileDefAddresses [0].Add (0x181F2, SpritesEnum.spider_bin);
      _spriteFileDefAddresses [0].Add (0x180EC, SpritesEnum.thumper_bin);
      _spriteFileDefAddresses [0].Add (0x180B0, SpritesEnum.compost_bin);
      _spriteFileDefAddresses [0].Add (0x1821A, SpritesEnum.misslink_bin);
      _spriteFileDefAddresses [0].Add (0x181A0, SpritesEnum.fire_man_bin);
      _spriteFileDefAddresses [0].Add (0x18254, SpritesEnum.emplacmt_bin);
      _spriteFileDefAddresses [0].Add (0x182E0, SpritesEnum.mechanic_bin);
      _spriteFileDefAddresses [0].Add (0x182A4, SpritesEnum.gyro_big_bin);
      _spriteFileDefAddresses [0].Add (0x180C4, SpritesEnum.frogger_bin);
      _spriteFileDefAddresses [0].Add (0x180D8, SpritesEnum.lizman_bin);
      _spriteFileDefAddresses [0].Add (0x18230, SpritesEnum.slug_bin);
      _spriteFileDefAddresses [0].Add (0x18112, SpritesEnum.guard_bin);
      _spriteFileDefAddresses [0].Add (0x1814C, SpritesEnum.dustdevl_bin);
      _spriteFileDefAddresses [0].Add (0x1818A, SpritesEnum.kangaroo_bin);
      _spriteFileDefAddresses [0].Add (0x18124, SpritesEnum.beast_bin);
      _spriteFileDefAddresses [0].Add (0x18174, SpritesEnum.steamjet_bin);
      _spriteFileDefAddresses [0].Add (0x181C8, SpritesEnum.stonewat_bin);
      _spriteFileDefAddresses [0].Add (0x181DE, SpritesEnum.cocoon_bin);
      _spriteFileDefAddresses [0].Add (0x18206, SpritesEnum.lobber_bin);
      _spriteFileDefAddresses [0].Add (0x181B6, SpritesEnum.hand_bin);
      _spriteFileDefAddresses [0].Add (0x18242, SpritesEnum.mace_bin);
      _spriteFileDefAddresses [0].Add (0x182BA, SpritesEnum.rat_bin);
      _spriteFileDefAddresses [0].Add (0x182F6, SpritesEnum.engyball_bin);
      _spriteFileDefAddresses [0].Add (0x1827E, SpritesEnum.robot_bin);
      _spriteFileDefAddresses [0].Add (0x182CA, SpritesEnum.halftrak_bin);
      _spriteFileDefAddresses [0].Add (0x18290, SpritesEnum.revdome_bin);
      _spriteFileDefAddresses [0].Add (0x18010, SpritesEnum.expl1616_bin);
      _spriteFileDefAddresses [0].Add (0x1830C, SpritesEnum.thebaron_bin);

      _spriteFileDefAddresses [1] = new Dictionary<int, SpritesEnum> ();
      _spriteFileDefAddresses [1].Add (0x12D16, SpritesEnum.getready_bin);
      _spriteFileDefAddresses [1].Add (0x12D72, SpritesEnum.tranmask_bin);
      _spriteFileDefAddresses [1].Add (0x12D5E, SpritesEnum.expl3232_bin);

      _spriteFileDefAddresses [1].Add (0x12CA6, SpritesEnum.brigand_bin);
      _spriteFileDefAddresses [1].Add (0x12CB8, SpritesEnum.mercenry_bin);
      _spriteFileDefAddresses [1].Add (0x12CCC, SpritesEnum.gentlman_bin);
      _spriteFileDefAddresses [1].Add (0x12CE0, SpritesEnum.navvie_bin);
      _spriteFileDefAddresses [1].Add (0x12CF2, SpritesEnum.thug_bin);
      _spriteFileDefAddresses [1].Add (0x12D02, SpritesEnum.preacher_bin);

      _spriteFileDefAddresses [1].Add (0x12D4E, SpritesEnum.sp8x8_bin);
      _spriteFileDefAddresses [1].Add (0x12F48, SpritesEnum.emplgun_bin);
      _spriteFileDefAddresses [1].Add (0x12FEE, SpritesEnum.barongun_bin);
      _spriteFileDefAddresses [1].Add (0x12D86, SpritesEnum.panel_bin);

      _spriteFileDefAddresses [1].Add (0x12E5A, SpritesEnum.sewer_bin);
      _spriteFileDefAddresses [1].Add (0x12DA8, SpritesEnum.beetle_bin);
      _spriteFileDefAddresses [1].Add (0x12D3E, SpritesEnum.nodes_bin);
      _spriteFileDefAddresses [1].Add (0x12E32, SpritesEnum.dhturret_bin);
      _spriteFileDefAddresses [1].Add (0x12E02, SpritesEnum.blob_bin);
      _spriteFileDefAddresses [1].Add (0x12D96, SpritesEnum.rockman_bin);
      _spriteFileDefAddresses [1].Add (0x12EDC, SpritesEnum.spider_bin);
      _spriteFileDefAddresses [1].Add (0x12DF0, SpritesEnum.thumper_bin);
      _spriteFileDefAddresses [1].Add (0x12DBA, SpritesEnum.compost_bin);
      _spriteFileDefAddresses [1].Add (0x12F00, SpritesEnum.misslink_bin);
      _spriteFileDefAddresses [1].Add (0x12E92, SpritesEnum.fire_man_bin);
      _spriteFileDefAddresses [1].Add (0x12F34, SpritesEnum.emplacmt_bin);
      _spriteFileDefAddresses [1].Add (0x12FB2, SpritesEnum.mechanic_bin);
      _spriteFileDefAddresses [1].Add (0x12F7C, SpritesEnum.gyro_big_bin);
      _spriteFileDefAddresses [1].Add (0x12DCC, SpritesEnum.frogger_bin);
      _spriteFileDefAddresses [1].Add (0x12DDE, SpritesEnum.lizman_bin);
      _spriteFileDefAddresses [1].Add (0x12F14, SpritesEnum.slug_bin);
      _spriteFileDefAddresses [1].Add (0x12E12, SpritesEnum.guard_bin);
      _spriteFileDefAddresses [1].Add (0x12E46, SpritesEnum.dustdevl_bin);
      _spriteFileDefAddresses [1].Add (0x12E7E, SpritesEnum.kangaroo_bin);
      _spriteFileDefAddresses [1].Add (0x12E22, SpritesEnum.beast_bin);
      _spriteFileDefAddresses [1].Add (0x12E6A, SpritesEnum.steamjet_bin);
      _spriteFileDefAddresses [1].Add (0x12EB6, SpritesEnum.stonewat_bin);
      _spriteFileDefAddresses [1].Add (0x12ECA, SpritesEnum.cocoon_bin);
      _spriteFileDefAddresses [1].Add (0x12EEE, SpritesEnum.lobber_bin);
      _spriteFileDefAddresses [1].Add (0x12EA6, SpritesEnum.hand_bin);
      _spriteFileDefAddresses [1].Add (0x12F24, SpritesEnum.mace_bin);
      _spriteFileDefAddresses [1].Add (0x12F90, SpritesEnum.rat_bin);
      _spriteFileDefAddresses [1].Add (0x12FC6, SpritesEnum.engyball_bin);
      _spriteFileDefAddresses [1].Add (0x12F5A, SpritesEnum.robot_bin);
      _spriteFileDefAddresses [1].Add (0x12F9E, SpritesEnum.halftrak_bin);
      _spriteFileDefAddresses [1].Add (0x12F6A, SpritesEnum.revdome_bin);
      _spriteFileDefAddresses [1].Add (0x12D2A, SpritesEnum.expl1616_bin);
      _spriteFileDefAddresses [1].Add (0x12FDA, SpritesEnum.thebaron_bin);

      _spriteFileDefAddresses [2] = new Dictionary<int, SpritesEnum> ();
      _spriteFileDefAddresses [2].Add (0x112C0 - 0x800, SpritesEnum.getready_bin);
      _spriteFileDefAddresses [2].Add (0x1131C - 0x800, SpritesEnum.tranmask_bin);
      _spriteFileDefAddresses [2].Add (0x11308 - 0x800, SpritesEnum.expl3232_bin);

      _spriteFileDefAddresses [2].Add (0x11250 - 0x800, SpritesEnum.brigand_bin);
      _spriteFileDefAddresses [2].Add (0x11262 - 0x800, SpritesEnum.mercenry_bin);
      _spriteFileDefAddresses [2].Add (0x11276 - 0x800, SpritesEnum.gentlman_bin);
      _spriteFileDefAddresses [2].Add (0x1128A - 0x800, SpritesEnum.navvie_bin);
      _spriteFileDefAddresses [2].Add (0x1129C - 0x800, SpritesEnum.thug_bin);
      _spriteFileDefAddresses [2].Add (0x112AC - 0x800, SpritesEnum.preacher_bin);

      _spriteFileDefAddresses [2].Add (0x112F8 - 0x800, SpritesEnum.sp8x8_bin);
      _spriteFileDefAddresses [2].Add (0x114F2 - 0x800, SpritesEnum.emplgun_bin);
      _spriteFileDefAddresses [2].Add (0x11598 - 0x800, SpritesEnum.barongun_bin);
      _spriteFileDefAddresses [2].Add (0x11330 - 0x800, SpritesEnum.panel_bin);

      _spriteFileDefAddresses [2].Add (0x11404 - 0x800, SpritesEnum.sewer_bin);
      _spriteFileDefAddresses [2].Add (0x11352 - 0x800, SpritesEnum.beetle_bin);
      _spriteFileDefAddresses [2].Add (0x112E8 - 0x800, SpritesEnum.nodes_bin);
      _spriteFileDefAddresses [2].Add (0x113DC - 0x800, SpritesEnum.dhturret_bin);
      _spriteFileDefAddresses [2].Add (0x113AC - 0x800, SpritesEnum.blob_bin);
      _spriteFileDefAddresses [2].Add (0x11340 - 0x800, SpritesEnum.rockman_bin);
      _spriteFileDefAddresses [2].Add (0x11486 - 0x800, SpritesEnum.spider_bin);
      _spriteFileDefAddresses [2].Add (0x1139A - 0x800, SpritesEnum.thumper_bin);
      _spriteFileDefAddresses [2].Add (0x11364 - 0x800, SpritesEnum.compost_bin);
      _spriteFileDefAddresses [2].Add (0x114AA - 0x800, SpritesEnum.misslink_bin);
      _spriteFileDefAddresses [2].Add (0x1143C - 0x800, SpritesEnum.fire_man_bin);
      _spriteFileDefAddresses [2].Add (0x114DE - 0x800, SpritesEnum.emplacmt_bin);
      _spriteFileDefAddresses [2].Add (0x1155C - 0x800, SpritesEnum.mechanic_bin);
      _spriteFileDefAddresses [2].Add (0x11526 - 0x800, SpritesEnum.gyro_big_bin);
      _spriteFileDefAddresses [2].Add (0x11376 - 0x800, SpritesEnum.frogger_bin);
      _spriteFileDefAddresses [2].Add (0x11388 - 0x800, SpritesEnum.lizman_bin);
      _spriteFileDefAddresses [2].Add (0x114BE - 0x800, SpritesEnum.slug_bin);
      _spriteFileDefAddresses [2].Add (0x113BC - 0x800, SpritesEnum.guard_bin);
      _spriteFileDefAddresses [2].Add (0x113F0 - 0x800, SpritesEnum.dustdevl_bin);
      _spriteFileDefAddresses [2].Add (0x11428 - 0x800, SpritesEnum.kangaroo_bin);
      _spriteFileDefAddresses [2].Add (0x113CC - 0x800, SpritesEnum.beast_bin);
      _spriteFileDefAddresses [2].Add (0x11414 - 0x800, SpritesEnum.steamjet_bin);
      _spriteFileDefAddresses [2].Add (0x11460 - 0x800, SpritesEnum.stonewat_bin);
      _spriteFileDefAddresses [2].Add (0x11474 - 0x800, SpritesEnum.cocoon_bin);
      _spriteFileDefAddresses [2].Add (0x11498 - 0x800, SpritesEnum.lobber_bin);
      _spriteFileDefAddresses [2].Add (0x11450 - 0x800, SpritesEnum.hand_bin);
      _spriteFileDefAddresses [2].Add (0x114CE - 0x800, SpritesEnum.mace_bin);
      _spriteFileDefAddresses [2].Add (0x1153A - 0x800, SpritesEnum.rat_bin);
      _spriteFileDefAddresses [2].Add (0x11570 - 0x800, SpritesEnum.engyball_bin);
      _spriteFileDefAddresses [2].Add (0x11504 - 0x800, SpritesEnum.robot_bin);
      _spriteFileDefAddresses [2].Add (0x11548 - 0x800, SpritesEnum.halftrak_bin);
      _spriteFileDefAddresses [2].Add (0x11514 - 0x800, SpritesEnum.revdome_bin);
      _spriteFileDefAddresses [2].Add (0x112D4 - 0x800, SpritesEnum.expl1616_bin);
      _spriteFileDefAddresses [2].Add (0x11584 - 0x800, SpritesEnum.thebaron_bin);

      _defaultSpriteIndex = new Dictionary<SpritesEnum, int> ();
      _defaultSpriteIndex.Add (SpritesEnum.sewer_bin, 19);
      _defaultSpriteIndex.Add (SpritesEnum.beetle_bin, 13);
      _defaultSpriteIndex.Add (SpritesEnum.nodes_bin, 0);
      _defaultSpriteIndex.Add (SpritesEnum.dhturret_bin, 0);
      _defaultSpriteIndex.Add (SpritesEnum.blob_bin, 0);
      _defaultSpriteIndex.Add (SpritesEnum.rockman_bin, 16);
      _defaultSpriteIndex.Add (SpritesEnum.spider_bin, 14);
      _defaultSpriteIndex.Add (SpritesEnum.thumper_bin, 0);
      _defaultSpriteIndex.Add (SpritesEnum.compost_bin, 0);
      _defaultSpriteIndex.Add (SpritesEnum.misslink_bin, 19);
      _defaultSpriteIndex.Add (SpritesEnum.fire_man_bin, 19);
      _defaultSpriteIndex.Add (SpritesEnum.emplacmt_bin, 4);
      _defaultSpriteIndex.Add (SpritesEnum.mechanic_bin, 17);
      _defaultSpriteIndex.Add (SpritesEnum.gyro_big_bin, 15);
      _defaultSpriteIndex.Add (SpritesEnum.frogger_bin, 24);
      _defaultSpriteIndex.Add (SpritesEnum.lizman_bin, 16);
      _defaultSpriteIndex.Add (SpritesEnum.slug_bin, 16);
      _defaultSpriteIndex.Add (SpritesEnum.guard_bin, 19);
      _defaultSpriteIndex.Add (SpritesEnum.dustdevl_bin, 19);
      _defaultSpriteIndex.Add (SpritesEnum.kangaroo_bin, 16);
      _defaultSpriteIndex.Add (SpritesEnum.beast_bin, 19);
      _defaultSpriteIndex.Add (SpritesEnum.steamjet_bin, 0);
      _defaultSpriteIndex.Add (SpritesEnum.stonewat_bin, 3);
      _defaultSpriteIndex.Add (SpritesEnum.cocoon_bin, 4);
      _defaultSpriteIndex.Add (SpritesEnum.lobber_bin, 16);
      _defaultSpriteIndex.Add (SpritesEnum.hand_bin, 8);
      _defaultSpriteIndex.Add (SpritesEnum.mace_bin, 0);
      _defaultSpriteIndex.Add (SpritesEnum.rat_bin, 17);
      _defaultSpriteIndex.Add (SpritesEnum.engyball_bin, 16);
      _defaultSpriteIndex.Add (SpritesEnum.robot_bin, 22);
      _defaultSpriteIndex.Add (SpritesEnum.halftrak_bin, 17);
      _defaultSpriteIndex.Add (SpritesEnum.revdome_bin, 8);
      _defaultSpriteIndex.Add (SpritesEnum.expl1616_bin, 0);
      _defaultSpriteIndex.Add (SpritesEnum.thebaron_bin, 1);

      int heroSpriteIndex = 4;
      _defaultSpriteIndex.Add (SpritesEnum.brigand_bin, heroSpriteIndex);
      _defaultSpriteIndex.Add (SpritesEnum.mercenry_bin, heroSpriteIndex);
      _defaultSpriteIndex.Add (SpritesEnum.gentlman_bin, heroSpriteIndex);
      _defaultSpriteIndex.Add (SpritesEnum.navvie_bin, heroSpriteIndex);
      _defaultSpriteIndex.Add (SpritesEnum.thug_bin, heroSpriteIndex);
      _defaultSpriteIndex.Add (SpritesEnum.preacher_bin, heroSpriteIndex);
    }

    public static SpriteBin GetSpriteBinA (VersionEnum version, int spriteFileDefAddress, int levelIndex, int paletteIndex) {
      if (spriteFileDefAddress == 0) {
        throw new Exception ();
      }
      SpriteFileDef spriteFileDef = Achaos.GetSpriteFileDef (version, spriteFileDefAddress);
      int paletteOffset = spriteFileDef.GetPaletteOffset (levelIndex);
      return (GetSpriteBinB (version, spriteFileDefAddress, paletteIndex, paletteOffset));
      //lock (_sprites) {
      //  if (!_sprites.ContainsKey (GetKey (spriteFileDefAddress, paletteIndex, paletteOffset))) {
      //    LoadSpriteBin (spriteFileDefAddress, levelIndex, paletteIndex, paletteOffset);
      //  }
      //}
      //return (_sprites [GetKey (spriteFileDefAddress, paletteIndex, paletteOffset)]);
    }

    public static SpriteBin GetSpriteBinB (VersionEnum version, int spriteFileDefAddress, int paletteIndex, int paletteOffset) {
      if (spriteFileDefAddress == 0) {
        throw new Exception ();
      }
      SpriteFileDef spriteFileDef = Achaos.GetSpriteFileDef (version, spriteFileDefAddress);
      //int paletteOffset = spriteFileDef.GetPaletteOffset (levelIndex);
      lock (_sprites) {
        if (!_sprites [(int) version].ContainsKey (GetKey (spriteFileDefAddress, paletteIndex, paletteOffset))) {
          LoadSpriteBin (version, spriteFileDefAddress, paletteIndex, paletteOffset);
        }
      }
      return (_sprites [(int) version] [GetKey (spriteFileDefAddress, paletteIndex, paletteOffset)]);
    }

    //public static SpriteBin GetSpriteBin (int spriteDefIndex, int levelIndex, int paletteIndex) {
    //  SpriteDef spriteDef = Achaos.GetSpriteDef (spriteDefIndex);
    //  return (GetSpriteBinFromAddress (spriteDef.spriteFileDefAddress, levelIndex, paletteIndex));
    //}

    private static int GetSpriteFileDefAddresses (VersionEnum version, SpritesEnum spriteEnum) {
      int address = 0;
      foreach (KeyValuePair<int, SpritesEnum> kvp in _spriteFileDefAddresses [(int) version]) {
        if (kvp.Value == spriteEnum) {
          address = kvp.Key;
          break;
        }
      }
      if (address == 0) {
        throw new Exception ();
      }
      return (address);
    }

    public static SpriteBin GetSpriteBinA (VersionEnum version, SpritesEnum spriteEnum, int levelIndex, int paletteIndex) {
      int spriteFileDefAddress = GetSpriteFileDefAddresses (version, spriteEnum);
      return (GetSpriteBinA (version, spriteFileDefAddress, levelIndex, paletteIndex));
    }

    public static SpriteBin GetSpriteBinB (VersionEnum version, SpritesEnum spriteEnum, int paletteIndex, int paletteOffset) {
      int spriteFileDefAddress = GetSpriteFileDefAddresses (version, spriteEnum);
      return (GetSpriteBinB (version, spriteFileDefAddress, paletteIndex, paletteOffset));
    }

    public static Bitmap GetSprite (VersionEnum version, int entityClassIndex, int levelIndex, int paletteIndex, int? spriteIndex, out bool isFirstSprite) {
      Element00_A_EntityClass entityClass = LevsdatBin.block00_Levels [(int) version].elements00 [levelIndex].elements00_A_EntityClasses [entityClassIndex];
      SpriteBin spriteBin = GetSpriteBinA (version, entityClass.GetSpriteDef (version).spriteFileDefAddress, levelIndex, paletteIndex);
      isFirstSprite = false;
      if (spriteIndex == null) {
        //if (version == 0) {
        SpritesEnum spriteEnum = _spriteFileDefAddresses [(int) version] [entityClass.GetSpriteDef (version).spriteFileDefAddress];
        if (_defaultSpriteIndex.ContainsKey (spriteEnum)) {
          spriteIndex = _defaultSpriteIndex [spriteEnum];
        }
        else {
          spriteIndex = 0;
          isFirstSprite = true;
        }
        //}
        //else {
        //  spriteIndex = 0;
        //  isFirstSprite = true;
        //}
      }
      return (spriteBin.GetBitmap ((int) spriteIndex));
    }

    private static void LoadSpriteBin (VersionEnum version, int spriteFileDefAddress, int paletteIndex, int paletteOffset) {
      SpriteFileDef spriteFileDef = Achaos.GetSpriteFileDef (version, spriteFileDefAddress);
      //Console.WriteLine (string.Format ("Creating sprite {0} with palette index {1}", spriteFileDef.fileName, paletteIndex));
      //SpriteBin spriteBin = new SpriteBin (spriteFileDef.fileName, spriteFileDef.type, spriteFileDef.nbBlocks, paletteIndex, 0);
      //int paletteOffset = spriteFileDef.GetPaletteOffset (levelIndex);
      SpriteBin spriteBin = spriteFileDef.CreateSpriteBin (paletteIndex, paletteOffset);
      _sprites [(int) version] [GetKey (spriteFileDefAddress, paletteIndex, paletteOffset)] = spriteBin;
    }

    private static string GetKey (int address, int paletteIndex, int paletteOffset) {
      return (string.Format ("{0:X8}/{1}/{2}", address, paletteIndex, paletteOffset));
    }

    //private static string GetKeyHeroHead (SpritesEnum spriteEnum, int paletteIndex) {
    //  //return (string.Format ("{0}/{1}", (int) spriteEnum, paletteIndex));
    //  return (string.Format ("{0}", (int) spriteEnum, paletteIndex));
    //}

    //public static void PreloadHeroesHead (int paletteIndex) {
    //  GetHeroeHead (SpritesEnum.brigand_bin, paletteIndex);
    //  GetHeroeHead (SpritesEnum.mercenry_bin, paletteIndex);
    //  GetHeroeHead (SpritesEnum.gentlman_bin, paletteIndex);
    //  GetHeroeHead (SpritesEnum.navvie_bin, paletteIndex);
    //  GetHeroeHead (SpritesEnum.thug_bin, paletteIndex);
    //  GetHeroeHead (SpritesEnum.preacher_bin, paletteIndex);
    //}

    //public static Dictionary<string, SpriteBin> GetHeroesHead (int paletteIndex) {
    //  PreloadHeroesHead (paletteIndex);
    //  return (_spritesHeroesHead);
    //}

    // static.bin (size: 0xAF38)
    // 0x0000 getready.bin (! paused is different)
    // 0x1500 expl1616.bin (! some different sprites at the end ABC)
    // 0x4880 nodes.bin
    // 0x5A80 expl3232.bin
    // 0x7E80 tranmask.bin
    // 0x8280 panel.bin
    // 0x9AB8 sp8x8.bin (but uses another palette offset)
    public static SpriteBin CreateBasedOnExisting (VersionEnum version, SpritesEnum spriteEnum, int levelIndex, int paletteIndex, string fileNameShort, int? paletteOffset) {
      int spriteFileDefAddress = GetSpriteFileDefAddresses (version, spriteEnum);
      SpriteFileDef spriteFileDef = (SpriteFileDef) Achaos.GetSpriteFileDef (version, spriteFileDefAddress).Clone ();
      if (fileNameShort != null) {
        spriteFileDef._fileName = fileNameShort;
      }
      if (paletteOffset == null) {
        paletteOffset = spriteFileDef.GetPaletteOffset (levelIndex);
      }
      SpriteBin spriteBin = spriteFileDef.CreateSpriteBin (paletteIndex, (int) paletteOffset);
      return (spriteBin);
    }

    public static void Dump () {
      foreach (Dictionary<string, SpriteBin> sprites in _sprites) {
        foreach (KeyValuePair<string, SpriteBin> kvp in sprites) {
          Console.WriteLine ("{0} {1}", kvp.Key, kvp.Value._fileName);
        }
      }
    }

    //public static SpriteBin GetHeroeHead (SpritesEnum spriteEnum, int paletteIndex) {
    //  string key = GetKeyHeroHead (spriteEnum, paletteIndex);
    //  lock (_spritesHeroesHead) {
    //    if (!_spritesHeroesHead.ContainsKey (key)) {
    //      int spriteFileDefAddress = GetSpriteFileDefAddresses (spriteEnum);
    //      SpriteFileDef spriteFileDef = Achaos.GetSpriteFileDef (spriteFileDefAddress);
    //      string fileName = Path.Combine (Settings.Default.BaseFolder, spriteFileDef._fileName);
    //      Palette palette = Achaos.GetPalette (paletteIndex);
    //      SpriteBin spriteBin = new SpriteBin (fileName, 0x6000, 1, 32, 32, -1,  SpriteBin.ModesEnum.Chunky, -1, palette, 2 * 16, SpriteBin.MasksEnum.Opaque, 0);
    //      //Dump dump = new Dump (FilesManager.GetFile (fileName));
    //      //Bitmap bitmap = SpriteBin.ReadChunky_1Byte2Pixels (dump, 0x6000, 32, 32, palette, 32, 0);
    //      _spritesHeroesHead.Add (key, spriteBin);
    //    }
    //  }
    //  return (_spritesHeroesHead [key]);
    //}

    // panel.bin (size: 0x1838)
    // 0x0000 16x24 panel background (0x0900 = 12 * 192)
    // 0x0900 32x24 panel power backgrounds off/on (0x0180 = 2 * 384)
    // 0x0C00 16x7  panel numbers small (0x0230 = 10 * 56)
    // 0x0E30 16x2  panel power bars on/off (0x0028 = 2 * 40, to 0x0E68?!)
    // 0x0E68 16x9  panel numbers medium (0x02D0 = 10 * 72)
    // 0x1138 16x16 panel powers (0x0700 = 14 * 128)
    //private static void LoadPanel (int levelIndex, int paletteIndex) {
    //  int spriteFileDefAddress = GetSpriteFileDefAddresses (SpritesEnum.panel_bin);
    //  SpriteFileDef spriteFileDef = Achaos.GetSpriteFileDef (spriteFileDefAddress);
    //  string fileName = Path.Combine (Settings.Default.BaseFolder, spriteFileDef._fileName);
    //  Palette palette = Achaos.GetPalette (paletteIndex);

    //  _panelBackgrounds = new SpriteBin (fileName, 0x0000, 12, 16, 24, palette, 2 * 16, SpriteBin.MethodsEnum.Interleaved_16Pixels_08Bytes, SpriteBin.MasksEnum.Opaque);
    //  _panelPowerBackgrounds = new SpriteBin (fileName, 0x0900, 2, 32, 24, palette, 2 * 16, SpriteBin.MethodsEnum.Interleaved_32Pixels_16Bytes, SpriteBin.MasksEnum.Opaque);
    //  _panelNumbersSmall = new SpriteBin (fileName, 0x0C00, 10, 16, 7, palette, 2 * 16, SpriteBin.MethodsEnum.Interleaved_16Pixels_08Bytes, SpriteBin.MasksEnum.Opaque);
    //  _panelPowerBars = new SpriteBin (fileName, 0x0E30, 2, 16, 2, palette, 2 * 16, SpriteBin.MethodsEnum.Interleaved_16Pixels_08Bytes, SpriteBin.MasksEnum.Opaque);
    //  _panelNumbersMedium = new SpriteBin (fileName, 0x0E68, 10, 16, 9, palette, 2 * 16, SpriteBin.MethodsEnum.Interleaved_16Pixels_08Bytes, SpriteBin.MasksEnum.Opaque);
    //  _panelPowers = new SpriteBin (fileName, 0x1138, 14, 16, 16, palette, 2 * 16, SpriteBin.MethodsEnum.Interleaved_16Pixels_08Bytes, SpriteBin.MasksEnum.Transparent);
    //}

    //public static SpriteBin GetPanelBackgrounds (int levelIndex, int paletteIndex) {
    //  if (_panelBackgrounds == null) {
    //    LoadPanel (levelIndex, paletteIndex);
    //  }
    //  return (_panelBackgrounds);
    //}

    //public static SpriteBin GetPanelPowerBackgrounds (int levelIndex, int paletteIndex) {
    //  if (_panelPowerBackgrounds == null) {
    //    LoadPanel (levelIndex, paletteIndex);
    //  }
    //  return (_panelPowerBackgrounds);
    //}

    //public static SpriteBin GetPanelNumbersSmall (int levelIndex, int paletteIndex) {
    //  if (_panelNumbersSmall == null) {
    //    LoadPanel (levelIndex, paletteIndex);
    //  }
    //  return (_panelNumbersSmall);
    //}

    //public static SpriteBin GetPanelPowerBars (int levelIndex, int paletteIndex) {
    //  if (_panelPowerBars == null) {
    //    LoadPanel (levelIndex, paletteIndex);
    //  }
    //  return (_panelPowerBars);
    //}

    //public static SpriteBin GetPanelNumbersMedium (int levelIndex, int paletteIndex) {
    //  if (_panelNumbersMedium == null) {
    //    LoadPanel (levelIndex, paletteIndex);
    //  }
    //  return (_panelNumbersMedium);
    //}

    //public static SpriteBin GetPanelPowers (int levelIndex, int paletteIndex) {
    //  if (_panelPowers == null) {
    //    LoadPanel (levelIndex, paletteIndex);
    //  }
    //  return (_panelPowers);
    //}
  }
}
