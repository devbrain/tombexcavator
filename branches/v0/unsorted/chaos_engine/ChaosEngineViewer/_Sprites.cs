using System;
using System.Collections.Generic;
using System.Text;
using System.Drawing;
using System.Drawing.Imaging;

namespace ChaosEngineViewer {
  public class Sprites {
    private Dump _dump;

    private Bitmap [] _bitmaps;

    private int [] _palette;
    public static Color [] Colors;

    public Sprites (Dump dump, int type) {
      _dump = dump;
      _palette = new int [16] { 0x0000, 0x0533, 0x0000, 0x0700, 0x0030, 0x0060, 0x0210, 0x0421,
                                0x0740, 0x0770, 0x0003, 0x0007, 0x0057, 0x0222, 0x0444, 0x0666 };
      Colors = new Color [16];
      for (int i = 0; i < 16; i++) {
        int color = _palette [i];
        Colors [i] = Color.FromArgb (16 + ((color >> 8) & 0x000F) * 32,
                                     16 + ((color >> 4) & 0x000F) * 32,
                                     16 + ((color >> 0) & 0x000F) * 32);
      }

      //int nb = (int) (dump.Size) / 128;
      if (type == -1) {
        int nb = dump.Size / 128;
        _bitmaps = new Bitmap [nb];
        for (int i = 0; i < nb; i++) {
          _bitmaps [i] = Sprites.ReadBitmap4_Word (_dump, i * 128, 16, 16, false);
        }
      }
        // player
      else if (type == 0) {
        int nb = (0x6600 - 0x00) / 512;
        _bitmaps = new Bitmap [nb];
        for (int i = 0; i < nb; i++) {
          _bitmaps [i] = Sprites.ReadBitmap4_Long (_dump, 0x00 + i * 512, 32, 32, true);
        }
      }
      // lifts
      else if (type == 1) {
        int nb = (0x6E00 - 0x6600) / 256;
        _bitmaps = new Bitmap [nb];
        for (int i = 0; i < nb; i++) {
          _bitmaps [i] = Sprites.ReadBitmap4_Long (_dump, 0x6600 + i * 256, 32, 16, true);
        }
      }
      // others
      else if (type == 2) {
        int nb = (0x18900 - 0x6E00) / 128;
        _bitmaps = new Bitmap [nb];
        for (int i = 0; i < nb; i++) {
          _bitmaps [i] = Sprites.ReadBitmap4_Word (_dump, 0x6E00 + i * 128, 16, 16, true);
        }
      }
      // map
      else if (type == 3) {
        int nb = 1;
        _bitmaps = new Bitmap [1];
        for (int i = 0; i < nb; i++) {
          _bitmaps [i] = Sprites.ReadBitmap4_Word (_dump, 0x18900, 320, 160, false);
        }
      }
    }

    public static Bitmap ReadBitmap4_Word (Dump dump, long address, int width, int height, bool mask) {
      Bitmap bitmap = new Bitmap (width, height, PixelFormat.Format32bppPArgb);
      Color transparent = Color.FromArgb (0, 255, 0, 255);
      for (int y = 0; y < height; y++) {
        for (int i = 0; i < width / 16; i++) {
          int word1 = dump.ReadWord (address + 0);
          int word2 = dump.ReadWord (address + 2);
          int word3 = dump.ReadWord (address + 4);
          int word4 = dump.ReadWord (address + 6);
          for (int x = 0; x < 16; x++) {
            int bit1 = (word1 >> (15 - x)) & 0x0001;
            int bit2 = (word2 >> (15 - x)) & 0x0001;
            int bit3 = (word3 >> (15 - x)) & 0x0001;
            int bit4 = (word4 >> (15 - x)) & 0x0001;
            byte color = (byte) ((bit4 << 3) + (bit3 << 2) + (bit2 << 1) + bit1);
            //byte color = (byte) ((bit1 << 1) + (bit2 << 2) + (bit3 << 1) + bit4);
            //Color color = palette [bit4 * 8 + bit3 * 4 + bit2 * 2 + bit1].Color;
            //sprite.SetPixel (x, y, color);
            if ((color != 0) || !mask) {
              bitmap.SetPixel (x + i * 16, y, Sprites.Colors [color]);
            }
            else {
              bitmap.SetPixel (x + i * 16, y, transparent);
            }
          }
          address += 8;
        }
      }
      if (mask) {
        bitmap.MakeTransparent (transparent);
      }
      return (bitmap);
    }

    public static Bitmap ReadBitmap4_Long (Dump dump, long address, int width, int height, bool mask) {
      Bitmap bitmap = new Bitmap (width, height, PixelFormat.Format32bppPArgb);
      Color transparent = Color.FromArgb (0, 255, 0, 255);
      for (int y = 0; y < height; y++) {
        for (int i = 0; i < width / 32; i++) {
          long long1 = dump.ReadLong (address + 0);
          long long2 = dump.ReadLong (address + 4);
          long long3 = dump.ReadLong (address + 8);
          long long4 = dump.ReadLong (address + 12);
          for (int x = 0; x < 32; x++) {
            long bit1 = (long1 >> (31 - x)) & 0x0001;
            long bit2 = (long2 >> (31 - x)) & 0x0001;
            long bit3 = (long3 >> (31 - x)) & 0x0001;
            long bit4 = (long4 >> (31 - x)) & 0x0001;
            byte color = (byte) ((bit4 << 3) + (bit3 << 2) + (bit2 << 1) + bit1);
            //byte color = (byte) ((bit1 << 1) + (bit2 << 2) + (bit3 << 1) + bit4);
            //Color color = palette [bit4 * 8 + bit3 * 4 + bit2 * 2 + bit1].Color;
            //sprite.SetPixel (x, y, color);
            if ((color != 0) || !mask) {
              bitmap.SetPixel (x + i * 32, y, Sprites.Colors [color]);
            }
            else {
              bitmap.SetPixel (x + i * 32, y, transparent);
            }
          }
          address += 16;
        }
      }
      if (mask) {
        bitmap.MakeTransparent (transparent);
      }
      return (bitmap);
    }

    public Bitmap [] Bitmaps {
      get {
        return (_bitmaps);
      }
    }

    public static void DrawString (string s, int x, int y, Graphics g, Font font, Brush brush) {
      g.DrawString (s, font, Brushes.Black, x + 1, y);
      g.DrawString (s, font, Brushes.Black, x - 1, y);
      g.DrawString (s, font, Brushes.Black, x, y + 1);
      g.DrawString (s, font, Brushes.Black, x, y - 1);
      g.DrawString (s, font, brush, x, y);
    }

    public static void DrawSprites (Graphics g, Sprites sprites, int width) {
      int x = 0;
      int y = 0;
      for (int i = 0; i < sprites.Bitmaps.Length; i++) {
        Bitmap bitmap = sprites.Bitmaps [i];
        int yMax = 0;
        g.DrawImage (bitmap, x, y);
        //Sprites.DrawString (i.ToString (), x - 1, y + 1, g, _font2, Brushes.White);
        x += bitmap.Width + 1;
        yMax = Math.Max (bitmap.Height, yMax);
        if ((x + bitmap.Width) >= width) {
          x = 0;
          y += yMax + 1;
          yMax = 0;
        }
      }
    }
  }
}
