// Copyright (C) 2004-2006 Crestez Leonard    cleonard@go.ro
//
// This file is part of Alpha
//
// Alpha is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// Alpha is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Alpha; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

using System;
using System.Drawing;

using Alpha.Util;
using Alpha.Graphics;

namespace Alpha.UI
{
    // TextLayout widget. This is used to arrange some text for printing.
    // It represents square area of text. You can choose things like alignment,
    // WordWrap and various other stuff.
    // This widget is not meant for direct use by the user.
    //
    // This has to be optimized like hell.
    //
    // TODO: Word wrap, Justify.
    public class TextLayout: Widget
    {
        // Font used to draw the text.
        // Can't be null, for obvious reasons.
        private IFont font;
        public IFont Font {
            get {
                return font;
            }
            set {
                if (value == null) {
                    throw new ArgumentNullException("value");
                }
                this.font = value;
            }
        }

        // The carret drawer. carret drawing will fail without it.
        private ICarretDrawer carretDrawer;
        public ICarretDrawer CarretDrawer {
            get {
                return carretDrawer;
            }
            set {
                carretDrawer = value;
            }
        }

        // The text to draw.
        private string text = "";
        public string Text {
            get {
                return text;
            }
            set {
                if (value == null) {
                    throw new ArgumentException("Don't use null, use an empty string.");
                }
                text = value;
                CarretPosition = CarretPosition;
                RecalculateChildSizes();
            }
        }

        // The position in the string to draw a carret at.
        // the carret is placed *between* two characters.
        // Clamped to 0, Text.Length inclusive.
        //
        // Note: the carret is not visible by default.
        private int carretPosition;
        public int CarretPosition {
            get {
                return carretPosition;
            }
            set {
                carretPosition = Math.Max(Math.Min(value, Text.Length), 0);
            }
        }

        // If this is true draw a carret at CarretPosition.
        // defaults to false.
        private bool showCarret = false;
        public bool ShowCarret {
            get {
                return showCarret;
            }
            set {
                showCarret = value;
            }
        }

        // Convert a point to a carret position.
        // Result is between 0 and Text.Length, inclusive.
        // FIXME: always returns 0.
        public int PointToCarret(PointD point)
        {
            string[] lines = Text.Split('\n');
            int line, i, cnt;
            double cx, cy;
            string str;

            // First of all determine the line.
            if (VerticalAlignment == VerticalTextAlignment.Top) {
                cy = 0;
            } else if (VerticalAlignment == VerticalTextAlignment.Center) {
                cy = (Height - Font.GetHeight() * lines.Length) / 2;
            } else if (VerticalAlignment == VerticalTextAlignment.Bottom) {
                cy = Height - Font.GetHeight() * lines.Length;
            } else {
                throw new Exception();
            }

            // Before the text.
            if (point.Y < cy) {
                return 0;
            }
            // After the text.
            if (point.Y > cy + Font.GetHeight() * lines.Length) {
                return Text.Length;
            }

            // Found the line.
            line = (int)Math.Floor((point.Y - cy) / Font.GetHeight());
            cnt = 0;
            str = lines[line];
            for (i = 0; i < line; ++i) {
                cnt += lines[i].Length;
            }

            // Find the start of the line.
            if (horAlign == HorizontalTextAlignment.Left) {
                cx = 0;
            } else if (horAlign == HorizontalTextAlignment.Center) {
                cx = Width / 2 - FontServices.GetStringWidth(Font, str) / 2;
            } else if (horAlign == HorizontalTextAlignment.Right) {
                cx = Width - FontServices.GetStringWidth(Font, str);
            } else {
                throw new Exception();
            }

            // Now let's go character by character.
            for (i = 0; i < str.Length; ++i) {
                if (point.X < cx + Font.GetCharWidth(str[i]) / 2) {
                    return cnt + i;
                }
                cx += Font.GetCharWidth(str[i]);
            }
            return cnt + str.Length;
        }

        // Horizontal text alignment
        private HorizontalTextAlignment horAlign = HorizontalTextAlignment.Left;
        public HorizontalTextAlignment HorizontalAlignment {
            get {
                return horAlign;
            }
            set {
                if (value != HorizontalTextAlignment.Left &&
                    value != HorizontalTextAlignment.Right &&
                    value != HorizontalTextAlignment.Center) {
                    throw new ArgumentException("value");
                }
                horAlign = value;
            }
        }
        
        // Vertical text alignment
        private VerticalTextAlignment verAlign = VerticalTextAlignment.Center;
        public VerticalTextAlignment VerticalAlignment {
            get {
                return verAlign;
            }
            set {
                if (value != VerticalTextAlignment.Top &&
                    value != VerticalTextAlignment.Bottom &&
                    value != VerticalTextAlignment.Center) {
                    throw new ArgumentException("value");
                }
                verAlign = value;
            }
        }

        // Draw a tiny carret at x, y
        private void DrawCarret(double x, double y)
        {
            if (CarretDrawer != null) {
                CarretDrawer.DrawCarret(Font, x, y);
            }
        }

        // Draw the text.
        // TODO: optimize.
        public override void Draw(RectangleD ClipRect, RectangleD WindowClipRect)
        {
            double cx, cy;
            int i, j, k;
            string[] lines = Text.Split('\n');

            k = 0;
            cx = cy = 0;

            if (VerticalAlignment == VerticalTextAlignment.Top) {
                cy = 0;
            } else if (VerticalAlignment == VerticalTextAlignment.Center) {
                cy = (Height - Font.GetHeight() * lines.Length) / 2;
            } else if (VerticalAlignment == VerticalTextAlignment.Bottom) {
                cy = Height - Font.GetHeight() * lines.Length;
            } else {
                throw new Exception();
            }

            //DebugClipping(ClipRect);
            //Console.WriteLine("clip rect {0} win clip rect {1} bounds {2}",
            //        ClipRect, WindowClipRect, GetBoundsInWindow(ClipRect, WindowClipRect),
            PushSelfScissor(ClipRect, WindowClipRect);
            
            for (i = 0; i < lines.Length; ++i) {
                // Handle horizontal alignment.
                if (horAlign == HorizontalTextAlignment.Left) {
                    cx = 0;
                } else if (horAlign == HorizontalTextAlignment.Center) {
                    cx = Width / 2 - FontServices.GetStringWidth(Font, lines[i]) / 2;
                } else if (horAlign == HorizontalTextAlignment.Right) {
                    cx = Width - FontServices.GetStringWidth(Font, lines[i]);
                } else {
                    throw new Exception();
                }

                for (j = 0; j < lines[i].Length; ++j) {
                    Font.DrawChar(lines[i][j], cx, cy);
                    if (ShowCarret && k == CarretPosition) {
                        DrawCarret(cx, cy);
                    }
                    cx += Font.GetCharWidth(lines[i][j]);
                    ++k;
                }
                cy += Font.GetHeight();
            }
            if (ShowCarret && k == CarretPosition) {
                DrawCarret(cx, cy - Font.GetHeight());
            }
            GL.PopScissor();

            base.Draw(ClipRect, WindowClipRect);
        }

        // Default empty constructor.
        public TextLayout(IFont f)
        {
            Font = f;
        }

        // Constructor that initializes text.
        public TextLayout(IFont f, string text): this(f)
        {
            Text = text;
        }

        // Get the desired height.
        public override double GetDesiredHeight()
        {
            return Text.Split('\n').Length * Font.GetHeight();
        }

        // Get the desired width.
        // Returns the length of the longest string.
        public override double GetDesiredWidth()
        {
            string[] lines = Text.Split('\n');
            double res = 0;
            foreach (string s in lines) {
                res = Math.Max(res, FontServices.GetStringWidth(Font, s));
            }
            return res;
        }
    }
}

