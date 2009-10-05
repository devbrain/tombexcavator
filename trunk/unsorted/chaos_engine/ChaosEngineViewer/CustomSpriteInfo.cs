using System;
using System.Collections.Generic;
using System.Text;
using System.Xml.Serialization;

namespace ChaosEngineViewer {
  public class CustomSpriteInfo {
    [XmlAttribute ("id")]
    public string _id;
    [XmlIgnore]
    public string Id {
      get {
        return (_id);
      }
    }
    [XmlAttribute ("com")]
    public string _com;
    [XmlAttribute ("file")]
    public string _file;
    [XmlIgnore]
    public int? _archive;
    [XmlAttribute ("archive")]
    public string Archive {
      get {
        if (_archive == null) {
          return ("");
        }
        else {
          return (_archive.ToString ());
        }
      }
      set {
        if (value == "") {
          _archive = null;
        }
        else {
          _archive = int.Parse (value);
        }
      }
    }
    [XmlIgnore]
    public int _offset;
    //[XmlIgnore]
    //public string _offsetString;
    [XmlAttribute ("offset")]
    public string Offset {
      get {
        return (string.Format ("0x{0:X6}", _offset));
      }
      set {
        _offset = int.Parse (value.Substring (2), System.Globalization.NumberStyles.AllowHexSpecifier);
        //_offsetString = value;
      }
    }
    [XmlAttribute ("nb")]
    public int _nb;
    [XmlAttribute ("width")]
    public int _width;
    [XmlAttribute ("height")]
    public int _height;
    [XmlAttribute ("bp")]
    public int _bp;
    [XmlAttribute ("mode")]
    public SpriteBin.ModesEnum _mode;
    [XmlAttribute ("interleave")]
    public int _interleave;
    [XmlAttribute ("palIndex")]
    public int _palIndex;
    [XmlAttribute ("palOffsetX16")]
    public int _palOffsetX16;
    [XmlAttribute ("mask")]
    public SpriteBin.MasksEnum _mask;
    [XmlAttribute ("cols")]
    public int _cols;

    public CustomSpriteInfo () {
    }

    public CustomSpriteInfo (string name, string com, string file, int? archive, int offset, int nb, int width, int height, int bp, SpriteBin.ModesEnum mode, int interleave, int palIndex, int palOffsetX16, SpriteBin.MasksEnum mask, int cols) {
      _id = name;
      _com = com;
      _file = file;
      _archive = archive;
      _offset = offset;
      _nb = nb;
      _width = width;
      _height = height;
      _bp = bp;
      _mode = mode;
      _interleave = interleave;
      _palIndex = palIndex;
      _palOffsetX16 = palOffsetX16;
      _mask = mask;
      _cols = cols;
    }
  }
}
