using System;
using System.Collections.Generic;
using System.Text;
using System.Drawing;
using ChaosEngineViewer.X1CHAOS_CAS;

namespace ChaosEngineViewer.ACHAOS {
  public class ItemType {
    //private X1chaosCas _x1chaosCas;
    public int variableToIncrease;
    public int unk02_cantpickup;
    public int quantity;
    public int function;
    public int unk05Block08_Sound;
    public int score; // < 128
    public int unk07;

    //private int _index;
    //private Bitmap _bitmap;

    private static string [] variables = { "None", "Unk4", "Unk8", "Money", "Silver Key", "Gold Key" };

    public ItemType (VersionEnum version, int pack, int index) {
      Dump dump = Achaos.GetDump (version);

      int baseAddress;
      if (version == VersionEnum.AmigaCD32) {
        baseAddress = 0x205B2;
      }
      else if (version == VersionEnum.AmigaOCS) {
        baseAddress = 0x1AC4A;
      }
      else if (version == VersionEnum.AtariST) {
        baseAddress = 0x180E4;
      }
      else {
        throw new Exception ();
      }
      variableToIncrease = dump.ReadByte (dump.ReadLong (baseAddress + 4 * (pack * 7 + 0)) + index);
      unk02_cantpickup = dump.ReadByte (dump.ReadLong (baseAddress + 4 * (pack * 7 + 1)) + index);
      quantity = dump.ReadByte (dump.ReadLong (baseAddress + 4 * (pack * 7 + 2)) + index);
      function = dump.ReadLong (dump.ReadLong (baseAddress + 4 * (pack * 7 + 3)) + index * 4);
      unk05Block08_Sound = dump.ReadWord (dump.ReadLong (baseAddress + 4 * (pack * 7 + 4)) + index * 2);
      score = dump.ReadByte (dump.ReadLong (baseAddress + 4 * (pack * 7 + 5)) + index);
      unk07 = dump.ReadByte (dump.ReadLong (baseAddress + 4 * (pack * 7 + 6)) + index);

      //_index = index;
      //_bitmap = _x1chaosCas._itemsBin.GetBitmap (index);
    }

    public void Draw (Bitmap bitmap, int x, int y, GridMap gridMap) {
      gridMap.DrawImageWindow (bitmap, x, y);
      gridMap.SetTextAlignment (StringAlignment.Near, StringAlignment.Near);
      gridMap.DrawStringWindow (Brushes.White, string.Format ("VariableToIncrease: {0}", variables [variableToIncrease >> 2]), x, y + 16 + 8 * 0);
      gridMap.DrawStringWindow (Brushes.White, string.Format ("unk02: {0}", unk02_cantpickup), x, y + 16 + 8 * 1);
      gridMap.DrawStringWindow (Brushes.White, string.Format ("Quantity: {0}", quantity), x, y + 16 + 8 * 2);
      gridMap.DrawStringWindow (Brushes.White, string.Format ("Function: {0:X6}", function), x, y + 16 + 8 * 3);
      gridMap.DrawStringWindow (Brushes.White, string.Format ("Unk05Block08: {0}", unk05Block08_Sound), x, y + 16 + 8 * 4);
      gridMap.DrawStringWindow (Brushes.White, string.Format ("Score: {0}", score), x, y + 16 + 8 * 5);
      gridMap.DrawStringWindow (Brushes.White, string.Format ("unk07: {0}", unk07), x, y + 16 + 8 * 6);
    }
  }
}
