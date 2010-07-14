using System;
using System.Collections.Generic;
using System.Text;
using System.Drawing;
using System.Threading;
using System.Drawing.Text;
using ChaosEngineViewer.X1CHAOS_CAS;
using System.Drawing.Imaging;
using System.Drawing.Drawing2D;

namespace ChaosEngineViewer {
  public class GridMap {
    private Bitmap _bitmapBase;
    private Graphics _gBase;
    private Bitmap _bitmapFrontBuffer;
    private Graphics _gFrontBuffer;
    private Bitmap _bitmapBackBuffer;
    private Graphics _gBackBuffer;
    private int _spacing;
    private Graphics _g;
    private StringDrawer _stringDrawer = new StringDrawer ();
    //private ColorMatrix _matrix;
    //public ImageAttributes _attributes;

    public delegate void MapChangedHandler ();
    public event MapChangedHandler MapChangedEvent;

    private AutoResetEvent _waitThreadDrawMap = new AutoResetEvent (false);
    private AutoResetEvent _waitDrawMap = new AutoResetEvent (false);
    private AutoResetEvent _waitDrawMapDone = new AutoResetEvent (false);
    private Object _redrawMap = new object ();
    private Thread _threadDrawMap;
    private bool _abort = false;

    private X1chaosCas _x1chaosCas;

    private AdjustableArrowCap _arrow = new AdjustableArrowCap (4, 6, true);

    public GridMap (X1chaosCas x1chaosCas) {
      _x1chaosCas = x1chaosCas;


      //_matrix = new ColorMatrix ();
      //_matrix.Matrix33 = 0.5f; //opacity 0 = completely transparent, 1 = completely opaque
      //_attributes = new ImageAttributes ();
      //_attributes.SetColorMatrix (_matrix, ColorMatrixFlag.Default, ColorAdjustType.Bitmap);

      Spacing = 2;


      _threadDrawMap = new Thread (DrawMapThread);
      _threadDrawMap.Start ();
      _waitThreadDrawMap.WaitOne ();
    }

    public bool _mapBaseDirty = false;
    private void DrawMapThread () {
      _waitThreadDrawMap.Set ();
      for (; ; ) {
        _waitDrawMap.WaitOne ();
        if (_abort) {
          break;
        }
        if (_mapBaseDirty) {
          if (_spacingChanged) {
            _spacingChanged = false;
            if (_bitmapBase != null) {
              _bitmapBase.Dispose ();
            }
            _bitmapBase = new Bitmap (_x1chaosCas._mapWidth * (16 + _spacing) + _spacing, _x1chaosCas._mapHeight * (16 + _spacing) + _spacing, PixelFormat.Format32bppPArgb);
            _gBase = Graphics.FromImage (_bitmapBase);

            if (_bitmapFrontBuffer != null) {
              _bitmapFrontBuffer.Dispose ();
            }
            _bitmapFrontBuffer = new Bitmap (_x1chaosCas._mapWidth * (16 + _spacing) + _spacing, _x1chaosCas._mapHeight * (16 + _spacing) + _spacing, PixelFormat.Format32bppPArgb);
            _gFrontBuffer = Graphics.FromImage (_bitmapFrontBuffer);

            if (_bitmapBackBuffer != null) {
              _bitmapBackBuffer.Dispose ();
            }
            _bitmapBackBuffer = new Bitmap (_x1chaosCas._mapWidth * (16 + _spacing) + _spacing, _x1chaosCas._mapHeight * (16 + _spacing) + _spacing, PixelFormat.Format32bppPArgb);
            _gBackBuffer = Graphics.FromImage (_bitmapBackBuffer);

            _g = _gBackBuffer;
          }

          _mapBaseDirty = false;
          _x1chaosCas.DrawMapBase ();
          CopyBackBufferToBase ();
        }
        _x1chaosCas.DrawMap ();
        SwapBuffers ();
        if (MapChangedEvent != null) {
          MapChangedEvent ();
        }
        _waitDrawMapDone.Set ();
      }
    }

    public void RedrawMap () {
      _waitDrawMap.Set ();
    }

    public void RedrawMapWait () {
      _waitDrawMap.Set ();
      _waitDrawMapDone.WaitOne ();
    }

    private void SwapBuffers () {
      Bitmap bitmap = _bitmapBackBuffer;
      Graphics g = _gBackBuffer;

      //lock (_bitmapBackBuffer) {
      //  lock (_gBackBuffer) {
      //Thread.Sleep (2000);
      lock (_bitmapFrontBuffer) {
        lock (_gFrontBuffer) {
          _bitmapBackBuffer = _bitmapFrontBuffer;
          _gBackBuffer = _gFrontBuffer;
          _bitmapFrontBuffer = bitmap;
          _gFrontBuffer = g;
          _g = _gBackBuffer;
        }
      }
      //}
      //}
    }

    public void CopyBackBufferToBase () {
      _gBase.DrawImage (_bitmapBackBuffer, 0, 0);
    }

    public void RestoreBackground () {
      _gBackBuffer.Clear (Color.Black);
      _gBackBuffer.DrawImage (_bitmapBase, 0, 0);
    }

    public Bitmap Bitmap {
      get {
        return (_bitmapFrontBuffer);
      }
    }

    public Bitmap BitmapBase {
      get {
        return (_bitmapBase);
      }
    }

    public void WindowToMap (int windowX, int windowY, out int mapX, out int mapY) {
      //mapX = (mouseX - cursorX * (16 + _space)) + cursorX * 16;
      //mapY = (mouseY - cursorY * (16 + _space)) + cursorY * 16;

      //int cursorX = mouseX / (16 + _space);
      //int cursorY = mouseY / (16 + _space);
      //mapX = mouseX - cursorX * _space;
      //mapY = mouseY - cursorY * _space;

      mapX = (windowX * 16) / (16 + _spacing);
      mapY = (windowY * 16) / (16 + _spacing);
    }

    public void MapToWindow (int mapX, int mapY, out int windowX, out int windowY) {
      //mouseX = (mapX - cursorX * (16 + _space)) + cursorX * 16;
      //mouseY = (mapY - cursorY * (16 + _space)) + cursorY * 16;
      //int i = ((mapX / 16) + 1) * _space + mapX;
      windowX = mapX + ((mapX >> 4) + 1) * _spacing;
      windowY = mapY + ((mapY >> 4) + 1) * _spacing;
    }

    public void TileToWindow (int tileX, int tileY, out int windowX, out int windowY) {
      windowX = tileX * (16 + _spacing) + _spacing;
      windowY = tileY * (16 + _spacing) + _spacing;
    }

    public void WindowToTile (int windowX, int windowY, out int tileX, out int tileY) {
      tileX = (windowX - _spacing / 2) / (16 + _spacing);
      tileY = (windowY - _spacing / 2) / (16 + _spacing);
    }

    public void TileToMap (int tileX, int tileY, out int mapX, out int mapY) {
      mapX = tileX * 16 + 8;
      mapY = tileY * 16 + 8;
    }

    public void MapToTile (int mapX, int mapY, out int tileX, out int tileY) {
      tileX = mapX >> 4;
      tileY = mapY >> 4;
    }

    public void DrawStringTile (Brush brush, string s, int tileX, int tileY, int dx, int dy) {
      int windowX, windowY;
      TileToWindow (tileX, tileY, out windowX, out windowY);
      //dy -= 1;
      DrawStringWindow (brush, s, windowX + dx, windowY + dy);
    }

    public void DrawStringMap (Brush brush, string s, int mapX, int mapY) {
      int windowX, windowY;
      MapToWindow (mapX, mapY, out windowX, out windowY);
      DrawStringWindow (brush, s, windowX, windowY);
    }

    public void DrawStringWindow (Brush brush, string s, int windowX, int windowY) {
      //_sFormat.Alignment = StringAlignment.Center;
      //_sFormat.LineAlignment = StringAlignment.Center;
      _stringDrawer.DrawString (_g, brush, s, windowX, windowY);
      //_g.DrawString (s, _font, Brushes.Black, windowX - 1, windowY, _sFormat);
      //_g.DrawString (s, _font, Brushes.Black, windowX + 1, windowY, _sFormat);
      //_g.DrawString (s, _font, Brushes.Black, windowX, windowY - 1, _sFormat);
      //_g.DrawString (s, _font, Brushes.Black, windowX, windowY + 1, _sFormat);
      //_g.DrawString (s, _font, brush, windowX, windowY, _sFormat);
    }

    public void SetTextAlignment (StringAlignment x, StringAlignment y) {
      _stringDrawer.SetTextAlignment (x, y);
    }

    public void DrawRectangleTile (Pen pen, int tileX, int tileY, int tileW, int tileH) {
      int windowX, windowY;
      TileToWindow (tileX, tileY, out windowX, out windowY);
      if (_spacing > 1) {
        DrawRectangleWindow (pen, windowX - 1, windowY - 1, tileW * 16 + (tileW - 1) * _spacing + 2 - 1, tileH * 16 + (tileH - 1) * _spacing + 2 - 1);
      }
      else {
        DrawRectangleWindow (pen, windowX, windowY, tileW * 16 + (tileW - 1) * _spacing - 1, tileH * 16 + (tileH - 1) * _spacing - 1);
      }
    }

    public void DrawRectangleMap (Pen pen, int mapX, int mapY, int mapW, int mapH) {
      int windowX, windowY;
      MapToWindow (mapX, mapY, out windowX, out windowY);
      DrawRectangleWindow (pen, windowX - 1, windowY - 1, mapW + 1, mapH + 1);
    }

    public void DrawRectangleMap (Pen pen, int mapX, int mapY, int mapW, int mapH, ImageAlignment alignmentX, ImageAlignment alignmentY) {
      int windowX, windowY;
      MapToWindow (mapX, mapY, out windowX, out windowY);
      if (alignmentX == ImageAlignment.Far) {
        windowX += ((mapW >> 4) - 1) * _spacing;
      }
      if (alignmentY == ImageAlignment.Far) {
        windowY += ((mapH >> 4) - 1) * _spacing;
      }
      DrawRectangleWindow (pen, windowX - 1, windowY - 1, mapW + 1, mapH + 1);
    }

    public void DrawRectangleWindow (Pen pen, int windowX, int windowY, int windowW, int windowH) {
      _g.DrawRectangle (pen, windowX, windowY, windowW, windowH);
    }

    public void FillRectangleTile (Brush brush, int tileX, int tileY, int tileW, int tileH) {
      int windowX, windowY;
      TileToWindow (tileX, tileY, out windowX, out windowY);
      FillRectangleWindow (brush, windowX, windowY, tileW * 16 + (tileW - 1) * _spacing, tileH * 16 + (tileH - 1) * _spacing);
    }

    public void FillRectangleMap (Brush brush, int mapX, int mapY, int mapW, int mapH) {
      int windowX, windowY;
      MapToWindow (mapX, mapY, out windowX, out windowY);
      FillRectangleWindow (brush, windowX - 1, windowY - 1, mapW + 1, mapH + 1);
    }

    public void FillRectangleMap (Brush brush, int mapX, int mapY, int mapW, int mapH, ImageAlignment alignmentX, ImageAlignment alignmentY) {
      int windowX, windowY;
      MapToWindow (mapX, mapY, out windowX, out windowY);
      if (alignmentX == ImageAlignment.Far) {
        windowX += ((mapW >> 4) - 1) * _spacing;
      }
      if (alignmentY == ImageAlignment.Far) {
        windowY += ((mapH >> 4) - 1) * _spacing;
      }
      FillRectangleWindow (brush, windowX - 1, windowY - 1, mapW + 1, mapH + 1);
    }

    public void FillRectangleWindow (Brush brush, int windowX, int windowY, int windowW, int windowH) {
      _g.FillRectangle (brush, windowX, windowY, windowW, windowH);
    }

    public void DrawLineTile (Pen pen, int tileX1, int tileY1, int tileX2, int tileY2) {
      int windowX1, windowY1, windowX2, windowY2;
      TileToWindow (tileX1, tileY1, out windowX1, out windowY1);
      TileToWindow (tileX2, tileY2, out windowX2, out windowY2);
      DrawLineWindow (pen, windowX1 + 8, windowY1 + 8, windowX2 + 8, windowY2 + 8);
    }

    public void DrawLineMap (Pen pen, int mapX1, int mapY1, int mapX2, int mapY2) {
      int windowX1, windowY1, windowX2, windowY2;
      MapToWindow (mapX1, mapY1, out windowX1, out windowY1);
      MapToWindow (mapX2, mapY2, out windowX2, out windowY2);
      DrawLineWindow (pen, windowX1, windowY1, windowX2, windowY2);
    }

    public void DrawLineWindow (Pen pen, int windowX1, int windowy1, int windowX2, int windowY2) {
      _g.DrawLine (Pens.Black, windowX1 - 1, windowy1, windowX2 - 1, windowY2);
      _g.DrawLine (Pens.Black, windowX1 + 1, windowy1, windowX2 + 1, windowY2);
      _g.DrawLine (Pens.Black, windowX1, windowy1 - 1, windowX2, windowY2 - 1);
      _g.DrawLine (Pens.Black, windowX1, windowy1 + 1, windowX2, windowY2 + 1);
      _g.DrawLine (pen, windowX1, windowy1, windowX2, windowY2);
    }

    public void DrawCircleTile (Pen pen, int tileX, int tileY, int radius) {
      int windowX, windowY;
      TileToWindow (tileX, tileY, out windowX, out windowY);
      DrawCircleWindow (pen, windowX + 8, windowY + 8, radius);
    }

    public void DrawCircleMap (Pen pen, int mapX, int mapY, int radius) {
      int windowX, windowY;
      MapToWindow (mapX, mapY, out windowX, out windowY);
      DrawCircleWindow (pen, windowX, windowY, radius);
    }

    public void DrawCircleWindow (Pen pen, int windowX, int windowY, int radius) {
      _g.DrawEllipse (Pens.Black, windowX - radius / 2 - 1, windowY - radius / 2, radius, radius);
      _g.DrawEllipse (Pens.Black, windowX - radius / 2 + 1, windowY - radius / 2, radius, radius);
      _g.DrawEllipse (Pens.Black, windowX - radius / 2, windowY - radius / 2 - 1, radius, radius);
      _g.DrawEllipse (Pens.Black, windowX - radius / 2, windowY - radius / 2 + 1, radius, radius);
      _g.DrawEllipse (pen, windowX - radius / 2, windowY - radius / 2, radius, radius);
    }

    public void FillCircleMap (Brush brush, int mapX, int mapY, int radius) {
      int windowX, windowY;
      MapToWindow (mapX, mapY, out windowX, out windowY);
      FillCircleWindow (brush, windowX, windowY, radius);
    }

    public void FillCircleWindow (Brush brush, int windowX, int windowY, int radius) {
      _g.FillEllipse (brush, windowX - radius / 2, windowY - radius / 2, radius, radius);
    }

    public enum ImageAlignment {
      Near,
      Far
    }

    public void DrawImageTile (Bitmap bitmap, int tileX, int tileY) {
      int windowX, windowY;
      TileToWindow (tileX, tileY, out windowX, out windowY);
      //_g.DrawImage (bitmap, tileX * (16 + _space) + _space, tileY * (16 + _space) + _space);
      DrawImageWindow (bitmap, windowX, windowY);
    }

    public void DrawImageMap (Bitmap bitmap, int mapX, int mapY) {
      int windowX, windowY;
      MapToWindow (mapX, mapY, out windowX, out windowY);
      DrawImageWindow (bitmap, windowX, windowY);
    }

    public void DrawImageMap (Bitmap bitmap, int mapX, int mapY, ImageAlignment alignmentX, ImageAlignment alignmentY) {
      int windowX, windowY;
      MapToWindow (mapX, mapY, out windowX, out windowY);
      if (alignmentX == ImageAlignment.Far) {
        windowX += ((bitmap.Width >> 4) - 1) * _spacing;
      }
      if (alignmentY == ImageAlignment.Far) {
        windowY += ((bitmap.Height >> 4) - 1) * _spacing;
      }
      DrawImageWindow (bitmap, windowX, windowY);
    }

    public void DrawImageWindow (Bitmap bitmap, int windowX, int windowY) {
      //lock (bitmap) {
      _g.DrawImage (bitmap, windowX, windowY);
      //}
    }

    public void DrawArrowTile (Pen pen, int tileX1, int tileY1, int tileX2, int tileY2) {
      int windowX1, windowY1, windowX2, windowY2;
      TileToWindow (tileX1, tileY1, out windowX1, out windowY1);
      TileToWindow (tileX2, tileY2, out windowX2, out windowY2);
      DrawArrowWindow (pen, windowX1 + 8, windowY1 + 8, windowX2 + 8, windowY2 + 8);
    }

    public void DrawArrowMap (Pen pen, int mapX1, int mapY1, int mapX2, int mapY2) {
      int windowX1, windowY1, windowX2, windowY2;
      MapToWindow (mapX1, mapY1, out windowX1, out windowY1);
      MapToWindow (mapX2, mapY2, out windowX2, out windowY2);
      DrawArrowWindow (pen, windowX1, windowY1, windowX2, windowY2);
    }

    private Pen _blackPen = new Pen (Color.Black);
    private void DrawArrowWindow (Pen pen, int windowX1, int windowY1, int windowX2, int windowY2) {
      //Pen pen = new Pen (color, thickness);
      //Pen pen;
      //Pen blackPen = Pens.Black;

      //if (dash) {
      //  pen = _pensDashCustom [color];
      //  blackPen = _pensDashCustom [(int) Sprites.ColorEnum.Black];
      //}
      //else {
      //  pen = _pens [color];
      //  blackPen = _pens [(int) Sprites.ColorEnum.Black];
      //}
      //if (cap) {
      pen.CustomEndCap = _arrow;
      _blackPen.CustomEndCap = _arrow;
      //}
      //else {
      //  pen.EndCap = LineCap.NoAnchor;
      //  blackPen.EndCap = LineCap.NoAnchor;
      //}
      //pen.Width = thickness;
      //blackPen.Width = thickness;
      _g.DrawLine (_blackPen, windowX1 - 1, windowY1, windowX2 - 1, windowY2);
      _g.DrawLine (_blackPen, windowX1 + 1, windowY1, windowX2 + 1, windowY2);
      _g.DrawLine (_blackPen, windowX1, windowY1 - 1, windowX2, windowY2 - 1);
      _g.DrawLine (_blackPen, windowX1, windowY1 + 1, windowX2, windowY2 + 1);
      _g.DrawLine (pen, windowX1, windowY1, windowX2, windowY2);
    }

    public void Dispose () {
      try {
        _abort = true;
        _waitDrawMap.Set ();
      }
      catch {
      }
    }

    public void Clear (Color color) {
      _gBackBuffer.Clear (color);
    }

    bool _spacingChanged = false;
    public int Spacing {
      get {
        return (_spacing);
      }
      set {
        _mapBaseDirty = true;
        _spacingChanged = true;
        _spacing = value;
      }
    }
  }
}
