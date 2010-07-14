using System;
using System.Collections.Generic;
using System.Text;
using System.IO;
using ChaosEngineViewer.Properties;
using ChaosEngineViewer.ACHAOS;
using System.Xml.Serialization;
using System.Windows.Forms;

namespace ChaosEngineViewer {
  public static class CustomSpritesManager {
    private static Dictionary<string, SpriteBin> [] _sprites = new Dictionary<string, SpriteBin> [3];
    //private static List<SpriteBin> _spritesList = new List<SpriteBin> ();
    private static CustomSpriteInfos [] _infos = new CustomSpriteInfos [3];

    public enum CustomSpritesEnums {
      Test
    }

    static CustomSpritesManager () {
      _sprites [0] = new Dictionary<string, SpriteBin> ();
      _sprites [1] = new Dictionary<string, SpriteBin> ();
      _sprites [2] = new Dictionary<string, SpriteBin> ();
      try {
        XmlSerializer s = new XmlSerializer (typeof (CustomSpriteInfos));

        //_infos = new CustomSpriteInfos ();
        //_infos._infos.Add (new CustomSpriteInfo ("name", "desc", "file", null, 0x11a38, 14, 16, 16, 0, SpriteBin.ModesEnum.Planar, 76, 80, 90, SpriteBin.MasksEnum.Transparent, 5));
        //TextWriter w = new StreamWriter ("list.xml");
        //s.Serialize (w, _infos);
        //w.Close ();

        TextReader r0 = new StreamReader (Path.Combine (Application.StartupPath, "CustomSpriteInfosCD32.xml"));
        _infos [0] = (CustomSpriteInfos) s.Deserialize (r0);
        r0.Close ();

        TextReader r1 = new StreamReader (Path.Combine (Application.StartupPath, "CustomSpriteInfosOCS.xml"));
        _infos [1] = (CustomSpriteInfos) s.Deserialize (r1);
        r1.Close ();

        TextReader r2 = new StreamReader (Path.Combine (Application.StartupPath, "CustomSpriteInfosST.xml"));
        _infos [2] = (CustomSpriteInfos) s.Deserialize (r2);
        r2.Close ();

      }
      catch (Exception e) {
        throw e;
      }
    }

    //public static void PreLoad () {
    //  foreach (CustomSpriteInfo info in _infos._infos) {
    //    if (!_sprites.ContainsKey (info._name)) {
    //      LoadCustomSprite (info);
    //    }
    //  }
    //}

    //public static void PreLoad (int paletteIndex, int paletteOffsetX16) {
    //  _sprites = new Dictionary<string, SpriteBin> ();
    //  foreach (CustomSpriteInfo info in _infos._infos) {
    //    info._palIndex = paletteIndex;
    //    info._palOffsetX16 = paletteOffsetX16;
    //    LoadCustomSprite (info);
    //  }
    //}

    //public static List<SpriteBin> Sprites {
    //  get {
    //    return (_spritesList);
    //  }
    //}

    public static SpriteBin GetCustomSprite (VersionEnum version, int index) {
      if (index >= GetNbCustomSprites (version)) {
        throw new Exception ();
      }
      string name = GetCustomSpriteInfo (version, index)._id;
      return (GetCustomSprite (version, name));
    }

    public static SpriteBin GetCustomSprite (VersionEnum version, string id) {
      lock (_sprites [(int) version]) {
        if (!_sprites [(int) version].ContainsKey (id)) {
          CustomSpriteInfo info = GetCustomSpriteInfo (version, id);
          LoadCustomSprite (version, info);
        }
      }
      return (_sprites [(int) version] [id]);
    }

    public static CustomSpriteInfo GetCustomSpriteInfo (VersionEnum version, int index) {
      if (index >= GetNbCustomSprites (version)) {
        throw new Exception ();
      }
      return (_infos [(int) version]._infos [index]);
    }

    public static CustomSpriteInfo GetCustomSpriteInfo (VersionEnum version, string id) {
      CustomSpriteInfo info = null;
      foreach (CustomSpriteInfo info2 in _infos [(int) version]._infos) {
        if (info2._id == id) {
          info = info2;
          break;
        }
      }
      if (info == null) {
        throw new Exception ();
      }
      return (info);
    }

    public static List<CustomSpriteInfo> GetCustomSpriteInfos (VersionEnum version) {
      return (_infos [(int) version]._infos);
    }

    //public static Dictionary<string, SpriteBin> CustomSprites {
    //  get {
    //    return (_sprites);
    //  }
    //}

    public static int GetNbCustomSprites (VersionEnum version) {
      return (_infos [(int) version]._infos.Count);
    }

    public static void LoadCustomSprite (VersionEnum version, CustomSpriteInfo info) {
      SpriteBin spriteBin = CreateCustomSprite (version, info);
      _sprites [(int) version] [info._id] = spriteBin;
      //if (!_sprites.ContainsKey (info._name)) {
      //  _spritesList.Add (spriteBin);
      //}
    }

    public static SpriteBin CreateCustomSprite (VersionEnum version, CustomSpriteInfo info) {
      string fileName;
      if (version == VersionEnum.AmigaCD32) {
        fileName = Path.Combine (Settings.Default.BaseFolderCD32, info._file);
      }
      else if (version == VersionEnum.AmigaOCS) {
        fileName = Path.Combine (Settings.Default.BaseFolderOCS, info._file);
      }
      else if (version == VersionEnum.AtariST) {
        fileName = Path.Combine (Settings.Default.BaseFolderST, info._file);
      }
      else {
        throw new Exception ();
      }
      Palette palette = Achaos.GetPalette (version, info._palIndex);
      string title = string.Format ("Id: {0} File: {1} Offset: {2} {4}x{5} Comment: {3}", info._id, (info._archive == null) ? info._file : (info._file + "/" + info._archive), info.Offset, (info._com == "") ? "<none>" : info._com, info._width, info._height);
      SpriteBin spriteBin = new SpriteBin (version, fileName, title, info._archive, info._offset, info._nb, info._width, info._height, info._bp, info._mode, info._interleave, palette, info._palOffsetX16 * 16, info._mask, info._cols);
      return (spriteBin);
    }

    public static void ClearCache () {
      foreach (Dictionary<string, SpriteBin> sprites in _sprites) {
        foreach (KeyValuePair<string, SpriteBin> kvp in sprites) {
          kvp.Value.Dispose ();
        }
        sprites.Clear ();
      }
    }

    //private static void CreateCustomSprite (CustomSpriteInfo info) {
    //  string fileName = Path.Combine (Settings.Default.BaseFolder, info._file);
    //  Palette palette = Achaos.GetPalette (info._palIndex);

    //  SpriteBin spriteBin = new SpriteBin (fileName, info._archive, info._offset, info._nb, info._width, info._height, info._bp, info._mode, info._interleave, palette, info._palOffsetX16 * 16, info._mask, info._cols);
    //  _sprites [info._name] = spriteBin;
    //}
  }
}
