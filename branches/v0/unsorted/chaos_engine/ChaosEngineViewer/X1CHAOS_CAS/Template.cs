using System;
using System.Collections.Generic;
using System.Text;
using System.Drawing;

namespace ChaosEngineViewer.X1CHAOS_CAS {
  public class Template {
    public Tile [,] tiles;

    public string description;

    public Template (Dump dump, int address) {
      tiles = new Tile [3, 3];

      StringBuilder sb = new StringBuilder ();
      for (int y = 0; y < 3; y++) {
        for (int x = 0; x < 3; x++) {
          int tile = dump.ReadWord (address);
          //if (tile < 0xFFFE) {
          tiles [x, y] = new Tile (tile);
          //}
          address += 2;

          sb.AppendLine (tiles [x, y].Description);
        }
      }
      description = sb.ToString ();
    }

    public void Draw (int xo, int yo, X1chaosCas x1chaosCas, GridMap gridMap) {
      for (int y = 0; y < 3; y++) {
        for (int x = 0; x < 3; x++) {
          Tile tile = tiles [x, y];
          if (tile._00w_tile < 0xFFFE) {
            Bitmap bitmap = x1chaosCas.GetBitmapFromTile (tile.Index);
            if (bitmap != null) {
              //g.DrawImage (bitmap, (xo + x) * (16 + space) + space, (yo + y) * (16 + space) + space);
              gridMap.DrawImageTile (bitmap, xo + x, yo + y);
            }
            else {
              //g.FillRectangle (Brushes.Purple, (xo + x) * (16 + space) + space, (yo + y) * (16 + space) + space, 16, 16);
              gridMap.FillRectangleTile (Brushes.Purple, xo + x, yo + y, 1, 1);
            }
          }
        }
      }
    }
  }
}
