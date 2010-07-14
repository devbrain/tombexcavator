using System;
using System.Collections.Generic;
using System.Text;
using System.Drawing;
using System.Drawing.Imaging;
using System.IO;
using ChaosEngineViewer.Properties;

namespace ChaosEngineViewer {
  public class LevxbakBin {
    //private Dump _dump;

    private Bitmap [] _bitmaps;
    private Bitmap _bitmap;

    public LevxbakBin (VersionEnum version, int mapTileSet, Palette palette, int paletteOffset) {
      string baseFolder = Settings.Default.BaseFolderCD32;
      if (version == VersionEnum.AmigaCD32) {
        baseFolder = Settings.Default.BaseFolderCD32;
      }
      else if (version == VersionEnum.AmigaOCS) {
        baseFolder = Settings.Default.BaseFolderOCS;
      }
      else if (version == VersionEnum.AtariST) {
        baseFolder = Settings.Default.BaseFolderST;
      }
      else {
        throw new Exception ();
      }
      string fileName = Path.Combine (baseFolder, string.Format ("LEV{0}BAK.BIN", mapTileSet));
      Dump dump = new Dump (FilesManager.GetFile (version, fileName));

      Init (dump, palette, paletteOffset);
    }

    public LevxbakBin (Dump dump, Palette palette, int paletteOffset) {
      Init (dump, palette, paletteOffset);
    }

    private void Init (Dump dump, Palette palette, int paletteOffset) {
      if ((dump.Size % 128) != 0) {
        throw new Exception ();
      }
      int nb = dump.Size / 128; // 128 bytes per sprite, 8 bytes per line
      //_bitmaps = new Bitmap [nb];
      //for (int i = 0; i < nb; i++) {
      //  _bitmaps [i] = SpriteBin.ReadBitmap4_Word (dump, 128 * i, 16, 16, palette, paletteOffset, false);
      //}
      //_bitmaps = SpriteBin.ReadMultipleInterleaved_8Bytes16Pixels (dump, nb, 16, 16, palette, paletteOffset, 0);
      _bitmaps = SpriteBin.CreateBitmaps (dump, 0, nb, 16, 16, 4, SpriteBin.ModesEnum.Interleaved, 2, palette, paletteOffset, SpriteBin.MasksEnum.Opaque);

      int nbPerRow = 20;
      _bitmap = new Bitmap (nbPerRow * 17 + 1, (_bitmaps.Length / nbPerRow + 1) * 17 + 1, PixelFormat.Format32bppPArgb);
      Graphics g = Graphics.FromImage (_bitmap);
      for (int i = 0; i < _bitmaps.Length; i++) {
        int x = i % nbPerRow;
        int y = i / nbPerRow;
        g.DrawImage (_bitmaps [i], x * 17 + 1, y * 17 + 1);
      }
    }

    public Bitmap GetBitmap (int i) {
      if ((i >= 0) && (i < _bitmaps.Length)) {
        return (_bitmaps [i]);
      }
      else {
        return (null);
      }
    }

    public Bitmap Bitmap {
      get {
        return (_bitmap);
      }
    }
  }
}
