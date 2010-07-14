using System;
using System.Collections.Generic;
using System.Text;
using System.Drawing;

namespace ChaosEngineViewer.X1CHAOS_CAS {
  public class Item {
    private int _00w_x;
    private int _02w_y;
    private Tile _04w_backedUpTile;
    // tile is always of type 6 (item)
    private Tile _06w_currentTile;

    //public int itemIndex;

    //public string description;

    public Item (Dump dump, int address) {
      _00w_x = dump.ReadWord (address);
      address += 2;
      _02w_y = dump.ReadWord (address);
      address += 2;
      _04w_backedUpTile = new Tile (dump.ReadWord (address));
      address += 2;
      _06w_currentTile = new Tile (dump.ReadWord (address));
      address += 2;
    }

    //public int ItemIndex {
    //  get {
    //    return (currentTile.Index - 380);
    //  }
    //}

    public string Description {
      get {
        StringBuilder sb = new StringBuilder ();
        sb.AppendLine (string.Format ("***** ITEM *****"));
        sb.AppendLine (string.Format ("x: {0}", _00w_x));
        sb.AppendLine (string.Format ("y: {0}", _02w_y));
        sb.AppendLine (string.Format ("backedUpTile: 0x{0:X4}", _04w_backedUpTile._00w_tile));
        sb.AppendLine (_04w_backedUpTile.Description);
        sb.AppendLine (string.Format ("currentTile: 0x{0:X4}", _06w_currentTile._00w_tile));
        sb.AppendLine (_06w_currentTile.Description);
        //sb.AppendLine (string.Format ("itemIndex: {0}", ItemIndex));
        sb.AppendLine ();
        return (sb.ToString ());
      }
    }

    public int X {
      get {
        return (_00w_x);
      }
    }

    public int Y {
      get {
        return (_02w_y);
      }
    }

    public Tile BackedUpTile {
      get {
        return (_04w_backedUpTile);
      }
    }

    public Tile CurrentTile {
      get {
        return (_06w_currentTile);
      }
    }
  }
}
