using System;
using System.Collections.Generic;
using System.Text;
using System.Drawing;
using System.Drawing.Imaging;
using ChaosEngineViewer.ACHAOS;

namespace ChaosEngineViewer {
  public class Palette {
    private int _index;
    private int _size;
    private int [] _colorsR;
    private int [] _colorsG;
    private int [] _colorsB;
    private uint [] _rawARGB;

    private Color [] _colors;
    private Brush [] _brushes;

    private Bitmap _bitmap;

    //private int [] _colorsPatch = {
    //  0x00000000, 0x00008300, 0x00FFE36B, 0x00D77F17, 0x00A76700, 0x0087473B, 0x006B3B00, 0x00572B2B,
    //  0x00FFFFCF, 0x00FFAB00, 0x00AFA7CF, 0x008B83AF, 0x006F638F, 0x0053476F, 0x003B2F4F, 0x00231B33,
    //  0x00000000, 0x00004F00, 0x00CF976F, 0x009F5300, 0x00733700, 0x00531300, 0x003B0700, 0x00230000,
    //  0x00C7C7B7, 0x00CF8B00, 0x0073639B, 0x005B4F7B, 0x00433B5B, 0x002B273B, 0x0017131F, 0x00000000
    //};
    private static int [] _lookUp = { 0, 2, 4, 6, 8, 10, 12, 14, 1, 3, 5, 7, 9, 11, 13, 15 };
    //private static int [] _lookUp = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15 };

    public Palette (VersionEnum version, int index) {
      Dump dump = Achaos.GetDump (version);

      _index = index;
      int palettesBase;
      int paletteAddress;
      if (version == VersionEnum.AmigaCD32) {
        palettesBase = 0x222D8;
        paletteAddress = dump.ReadLong (palettesBase + 4 * index);
        if (paletteAddress == 0) {
          _size = 0;
        }
        else {
          _size = dump.ReadWord (paletteAddress);
        }
        paletteAddress += 2;
      }
      else if (version == VersionEnum.AmigaOCS) {
        palettesBase = 0x1C980;
        paletteAddress = palettesBase + 32 * index;
        if (index >= 11) {
          _size = 32;
        }
        else {
          _size = 16;
        }
      }
      else if (version == VersionEnum.AtariST) {
        palettesBase = 0x1A10A - 0x800;
        paletteAddress = palettesBase + 32 * index;
        _size = 16;
      }
      else {
        throw new Exception ();
      }
      _colorsR = new int [_size];
      _colorsG = new int [_size];
      _colorsB = new int [_size];
      _rawARGB = new uint [_size];
      _colors = new Color [_size];
      _brushes = new SolidBrush [_size];
      for (int i = 0; i < _size; i++) {
        int r, v, b;
        if (version == VersionEnum.AmigaCD32) {
          r = dump.ReadByte (paletteAddress++);
          v = dump.ReadByte (paletteAddress++);
          b = dump.ReadByte (paletteAddress++);
        }
        else if ((version == VersionEnum.AmigaOCS) || (version == VersionEnum.AtariST)) {
          int rvb = dump.ReadWord (paletteAddress);
          paletteAddress += 2;
          if (rvb > 0X0FFF) {
            int z = 0;
          }
          if (_size == 16) {
            r = _lookUp [(rvb >> 8) & 0x000F];
            v = _lookUp [(rvb >> 4) & 0x000F];
            b = _lookUp [(rvb >> 0) & 0x000F];
          }
          else {
            r = (rvb >> 8) & 0x000F;
            v = (rvb >> 4) & 0x000F;
            b = (rvb >> 0) & 0x000F;
          }
          r = (r << 4) + 8;
          v = (v << 4) + 8;
          b = (b << 4) + 8;
        }
        else {
          throw new Exception ();
        }
        _colorsR [i] = r;
        _colorsG [i] = v;
        _colorsB [i] = b;
        //_rawBGRA [i] = (uint) ((b << 24) | (v << 16) | (r << 8));
        _rawARGB [i] = (uint) ((0xFF << 24) | (r << 16) | (v << 8) | (b << 0));
        //if ((_colors1 [i] == 91) || (_colors2 [i] == 91) | (_colors3 [i] == 91)) {
        //  Console.WriteLine ("{0} {1} {2}", _colors1 [i], _colors2 [i], _colors3 [i]);
        //  int z = 0;
        //}
        //if (i < 32) {
        //  _colors1 [i] = (_colorsPatch [i] >> 16) & 0xFF;
        //  _colors2 [i] = (_colorsPatch [i] >> 8) & 0xFF;
        //  _colors3 [i] = (_colorsPatch [i] >> 0) & 0xFF;
        //}
        _colors [i] = Color.FromArgb (_colorsR [i], _colorsG [i], _colorsB [i]);
        _brushes [i] = new SolidBrush (_colors [i]);
      }

      _bitmap = new Bitmap (16 * 17 + 1, ((_size - 1) / 16 + 1) * 17 + 1, PixelFormat.Format32bppPArgb);
      Graphics g = Graphics.FromImage (_bitmap);
      g.Clear (Color.Black);
      for (int y = 0; y < 16; y++) {
        for (int x = 0; x < 16; x++) {
          if ((y * 16 + x) >= _size) {
            y = 16;
            break;
          }
          g.FillRectangle (_brushes [y * 16 + x], x * 17 + 1, y * 17 + 1, 16, 16);
        }
      }
    }

    public Bitmap Bitmap {
      get {
        return (_bitmap);
      }
    }

    public int Size {
      get {
        return (_size);
      }
    }

    public Color GetColor (int i) {
      if ((i >= 0) && (i < _colors.Length)) {
        return (_colors [i]);
      }
      else {
        return (Color.Red);
      }
    }

    public uint GetRawARGB (int i) {
      if ((i >= 0) && (i < _rawARGB.Length)) {
        return (_rawARGB [i]);
      }
      else {
        return (0xFFFF0000);
      }
    }

    public Brush GetBrush (int i) {
      if ((i >= 0) && (i < _brushes.Length)) {
        return (_brushes [i]);
      }
      else {
        return (Brushes.Red);
      }
    }

    public int Index {
      get {
        return (_index);
      }
    }
  }
}
