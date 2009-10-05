using System;
using System.Collections.Generic;
using System.Text;
using System.Drawing;

namespace ChaosEngineViewer.X1CHAOS_CAS {
  public class Tile {
    public int _00w_tile;
    //public int unk;
    //public int hasTrigger;
    //public int type;
    //public int height;
    //public int index;

    //public string typeString;

    //public string description;

    private static string [] _typesString = { "Empty", "Wall", "Stair", "Scripted", "Unk1", "Cover", "Item", "Unk2" };
    public enum TypesEnum {
      Empty, Wall, Stair, Scripted, Unk1, Cover, Item, Unk2
    }

    public Tile (int tile) {
      _00w_tile = tile;
      ////                                         76543210 76543210
      //unk = (_rawTile >> 15) & 0x0001;        // A------- -------- used to find cover
      //hasTrigger = (_rawTile >> 14) & 0x0001; // -B------ --------
      //type = (_rawTile >> 11) & 0x0007;       // --CCC--- --------
      ////  0: empty floor, 1: wall, 2: stairs, 3: scripted, 4: ?, 5: cover, 6: item, 7: ?
      //height = (_rawTile >> 9) & 0x0003;      // -----EE- --------
      //index = (_rawTile >> 0) & 0x01FF;       // -------F FFFFFFFF

    }

    public int Unk {
      get {
        return ((_00w_tile >> 15) & 0x0001);
      }
      set {
        _00w_tile &= ~(0x0001 << 15);
        _00w_tile |= ((value & 0x0001) << 15);
      }
    }

    public int HasTrigger {
      get {
        return ((_00w_tile >> 14) & 0x0001);
      }
      set {
        _00w_tile &= ~(0x0001 << 14);
        _00w_tile |= ((value & 0x0001) << 14);
      }
    }

    public int Type {
      get {
        return ((_00w_tile >> 11) & 0x0007);
      }
      set {
        _00w_tile &= ~(0x0007 << 11);
        _00w_tile |= ((value & 0x0007) << 11);
      }
    }

    public int Height {
      get {
        return ((_00w_tile >> 9) & 0x0003);
      }
      set {
        _00w_tile &= ~(0x0003 << 9);
        _00w_tile |= ((value & 0x0003) << 9);
      }
    }

    public int Index {
      get {
        return ((_00w_tile >> 0) & 0x01FF);
      }
      set {
        _00w_tile &= ~(0x01FF << 0);
        _00w_tile |= ((value & 0x01FF) << 0);
      }
    }

    public string TypeString {
      get {
        return (_typesString [Type]);
      }
    }

    public string Description {
      get {
        StringBuilder sb = new StringBuilder ();
        sb.AppendLine (string.Format ("***** TILE *****"));
        if (_00w_tile < 0xFFFE) {
          sb.AppendLine (string.Format ("00w tile: 0x{0:X4}", _00w_tile));
          sb.AppendLine (string.Format ("Unk: {0}", Unk));
          sb.AppendLine (string.Format ("HasTrigger: {0}", HasTrigger));
          sb.AppendLine (string.Format ("Type: {0} ({1})", Type, TypeString));
          sb.AppendLine (string.Format ("Height: {0}", Height));
          sb.AppendLine (string.Format ("Index: {0}", Index));
        }
        else {
          sb.AppendLine ("Empty");
        }
        return (sb.ToString ());
      }
    }

    public void Draw (int x, int y, X1chaosCas x1chaosCas, GridMap gridMap) {
      if (_00w_tile < 0xFFFE) {
        Bitmap bitmap = x1chaosCas.GetBitmapFromTile (Index);
        if (bitmap != null) {
          gridMap.DrawImageTile (bitmap, x, y);
          //DrawString (string.Format ("{0:X2}", tile.index), x, y, 0, 4, Brushes.White);
          //DrawString (tile.height.ToString (), x, y, 0, 4, Brushes.White);
          //DrawString (tile.type.ToString (), x, y, 0, 4, Brushes.White);
          //int offset = _offsetBlockA_Tiles + 6 + (y * _mapWidth + x) * 2;
          //if ((offset % 4) == 0) {
          //  DrawString (string.Format ("{0:X}", offset), x, y, -4, 4, Brushes.White);
          //}
          // 0x2600 in file cas

          //Pen pen = null;
          //if (Type == 2) {
          //  pen = Pens.Blue;
          //}
          //else if (Type == 3) {
          //  pen = Pens.Orange;
          //}
          //else if (Type == 5) {
          //  pen = Pens.LightGreen;
          //}
          //else if (Type == 6) {
          //  pen = Pens.LightPink;
          //}
          //if (pen != null) {
          //  gridMap.DrawRectangleTile (pen, x, y, 1, 1);
          //}

        }
        else {
          //g.FillRectangle (Brushes.Purple, x * (16 + space) + space, y * (16 + space) + space, 16, 16);
          gridMap.FillRectangleTile (Brushes.Purple, x, y, 1, 1);
        }
      }
      if (Unk != 0) {
        //throw new Exception ();
      }
      //if (type == 4) {
      //  int i;
      //}
      //X1CHAOS_CAS.DrawString (string.Format ("{0}", isTrigger), x, y, 0, 0, Brushes.White);
      //X1CHAOS_CAS.DrawString (string.Format ("{0}", type), x, y, 0, 0, Brushes.White);
      //X1CHAOS_CAS.DrawString (string.Format ("{0}", isWall), x, y, 0, 8, Brushes.Yellow);
      //X1CHAOS_CAS.DrawString (string.Format ("{0:X2}", (raw >> 8) & 0x00FF), x, y, 0, 0, Brushes.White);
      //X1CHAOS_CAS.DrawString (string.Format ("{0:X2}", raw & 0x00FF), x, y, 0, 8, Brushes.Yellow);
    }
  }
}
