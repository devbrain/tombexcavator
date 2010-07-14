using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Text;
using System.Windows.Forms;

namespace ChaosEngineViewer {
  public partial class DrawingSurface : UserControl {
    public DrawingSurface () {
      InitializeComponent ();

      Margin = Padding.Empty;
    }

    protected override void OnPaintBackground (PaintEventArgs e) {
    }
    //protected override void OnPaint (PaintEventArgs e) {
    //  Console.WriteLine ("paint: " + DoubleBuffered);
    //}
    //protected override void OnScroll (ScrollEventArgs se) {
    //}
  }
}
