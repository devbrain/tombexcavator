using System;
using System.Collections.Generic;
using System.Text;
using System.Drawing;
using System.Drawing.Text;

namespace ChaosEngineViewer {
  public class StringDrawer {
    private static PrivateFontCollection _pfc;
    private static FontFamily _ff;
    public static Font _font;

    private StringFormat _stringFormat = new StringFormat (StringFormat.GenericTypographic);

    static StringDrawer () {
      _pfc = new PrivateFontCollection ();
      _pfc.AddFontFile ("04B_03__.TTF");
      _ff = _pfc.Families [0];
      _font = new Font (_ff, 6f, FontStyle.Regular);
    }

    public void SetTextAlignment (StringAlignment x, StringAlignment y) {
      _stringFormat.Alignment = x;
      _stringFormat.LineAlignment = y;
    }

    public void DrawString (Graphics g, Brush brush, string s, int x, int y) {
      g.DrawString (s, _font, Brushes.Black, x - 1, y, _stringFormat);
      g.DrawString (s, _font, Brushes.Black, x + 1, y, _stringFormat);
      g.DrawString (s, _font, Brushes.Black, x, y - 1, _stringFormat);
      g.DrawString (s, _font, Brushes.Black, x, y + 1, _stringFormat);
      g.DrawString (s, _font, brush, x, y, _stringFormat);
    }
  }
}
