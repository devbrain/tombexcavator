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
using System.Drawing.Imaging;

namespace Alpha.Graphics
{
    // This enumeration represents the type of alignment to be used when drawing text
    public enum HorizontalTextAlignment {
        Left = 0,
        Center = 1,
        Right = 2,
    }

    public enum VerticalTextAlignment {
        Top = 0,
        Center = 1,
        Bottom = 2,
    }

    // This is the basic interface for a font. It can measure and draw a character.
    // All characters are supposed to have the same height, and characters can be
    // drawn independently. Should be enough for western languages.
    //
    // This class is meant for the final consumer of the font, this is why it's immutable
    // and contains height.
    public interface IFont
    {
        // Get the height of a font.
        double GetHeight();

        // Get the Width of a font.
        double GetCharWidth(char c);

        // Draw character c with the upper left corner at x, y.
        void DrawChar(char c, double x, double y);
    }

    // Static class with usefull font functions.
    public sealed class FontServices 
    {
        // Get the width of a string using the specified font.
        // Return the width of the widest line.
        public static double GetStringWidth(IFont font, string s)
        {
            double tw, lw; // Total width, line width.
            tw = lw = 0;
            foreach (char c in s) {
                if (c == '\n') {
                    if (lw > tw) {
                        tw = lw;
                    }
                    lw = 0;
                } else {
                    lw += font.GetCharWidth(c);
                }
            }
            return Math.Max(lw, tw);
        }

        // Draw the string at x, y.
        // Depending on the values of HorAlign and VerAlign x and y
        // are Left/Right/Top/Bottom/Center.
        public static void DrawString(IFont font, string s, double x, double y,
                HorizontalTextAlignment horAlign, VerticalTextAlignment verAlign)
        {
            string[] lines = s.Split('\n');
            double cx, cy;
            int i, j;

            // Handle vertical alignment.
            if (verAlign == VerticalTextAlignment.Top) {
                cy = y;
            } else if (verAlign == VerticalTextAlignment.Center) {
                cy = y - lines.Length * font.GetHeight() / 2;
            } else if (verAlign == VerticalTextAlignment.Bottom) {
                cy = y - lines.Length * font.GetHeight();
            } else {
                throw new ArgumentException("verAlign");
            }

            for (i = 0; i < lines.Length; ++i) {
                // Handle horizontal alignment.
                if (horAlign == HorizontalTextAlignment.Left) {
                    cx = x;
                } else if (horAlign == HorizontalTextAlignment.Center) {
                    cx = x - GetStringWidth(font, lines[i]) / 2;
                } else if (horAlign == HorizontalTextAlignment.Right) {
                    cx = x - GetStringWidth(font, lines[i]);
                } else {
                    throw new ArgumentException("horAlign");
                }

                for (j = 0; j < lines[i].Length; ++j) {
                    font.DrawChar(lines[i][j], cx, cy);
                    cx += font.GetCharWidth(lines[i][j]);
                }
                cy += font.GetHeight();
            }
        }

        // Draw a top-left aligned string (towards +x, +y)
        public static void DrawString(IFont f, string s, double x, double y)
        {
            DrawString(f, s, x, y, HorizontalTextAlignment.Left, VerticalTextAlignment.Top);
        }

        // Draw a string centered on x, y
        public static void DrawStringCenter(IFont f, string s, double x, double y)
        {
            DrawString(f, s, x, y, HorizontalTextAlignment.Center, VerticalTextAlignment.Center);
        }
    }

    // IFont implementation for simple image fonts.
    // The real work is done in a FontData that can be shared between several fonts.
    //
    // This keeps size and color.
    public class SimpleFont: IFont
    {
        // We keep the font data (only the texture and Draw function in FontData
        public readonly FontData Data;

        // The size of the font.
        private double height;
        public double Height {
            get {
                return height; 
            }
            set {
                if (value <= 0) {
                    throw new ArgumentException("Font.Height must be positive");
                }
                height = value;
            }
        }

        // The color of a font.
        public Color Color;

        // Create a font using the specified data, with the specified height and color.
        public SimpleFont(FontData data, double height, Color color)
        {
            Data = data;
            Height = height;
            Color = color;
        }

        // Create a font using the specified data, with the specified height and the color white.
        public SimpleFont(FontData data, double height): this(data, height, Color.White)
        {
        }

        // IFont.DrawChar
        public void DrawChar(char c, double x, double y)
        {
            GL.Color(Color);
            Data.DrawChar(c, x, y, Height);
        }

        // IFont.GetHeight
        public double GetHeight()
        {
            return Height;
        }

        // IFont.GetCharWidth
        public double GetCharWidth(char c)
        {
            return Data.GetCharWidth(c) * Height;
        }
    }

    // This class contains data about a font.
    //
    // Right now we only support fonts with INDEPENDENT chars, this means that
    // Chars have a fixed sized no matter what comes before or after them.
    // We do support different widths per-character.
    //
    // Since our fonts are freely scalable, by default if it's not specified we
    // assume a height of 1.0 on fonts, but different (positive) widths.
    //
    // Everything is done floating-point.
    public abstract class FontData
    {
        // Get the width of a char, assuming a height of 1.0.
        public abstract double GetCharWidth(char c);

        // Draw a char of char c at x, y with height h.
        // This in fact draws the hex code in a little square, of instance
        // unicode 0x1234 would draw as
        // 12
        // 34
        //
        // This can serve as a fallback for DrawChar.
        // It uses DrawChar to draw the actual hex, so you have to at least
        // be able to draw 0-9 and A-F.
        protected void DrawCharHex(char c, double x, double y, double h)
        {
            double w = 0;
            string str = "0123456789ABCDEF";
            foreach (char z in str) {
                w = Math.Max(w, GetCharWidth(z));
            }
            w *= h;
            GL.Begin(DrawMode.LineLoop);
            GL.Vertex(x, y);
            GL.Vertex(x + w, y);
            GL.Vertex(x + w, y + h);
            GL.Vertex(x, y + h);
            GL.End();
            DrawCharCenter(str[(c >> 12) & 0xF], x + w / 4, y + h / 4, h * 0.4);
            DrawCharCenter(str[(c >> 8) & 0xF], x + w * 3 / 4, y + h / 4, h * 0.4);
            DrawCharCenter(str[(c >> 4) & 0xF], x + w / 4, y + h * 3 / 4, h * 0.4);
            DrawCharCenter(str[(c >> 0) & 0xF], x + w * 3 / 4, y + h * 3 / 4, h * 0.4);
        }
       
        // Draw a char of char c at x, y of height h.
        // x, y is the upper left corner, it's drawn towards +x, +y
        public abstract void DrawChar(char c, double x, double y, double h);

        // Draw a char of char c centered on x, y of height h.
        // x, y is the center of the char, the char is drawn towards +x, +y
        private void DrawCharCenter(char c, double x, double y, double h)
        {
            double w = GetCharWidth(c) * h;
            DrawChar(c, x - w / 2, y - h / 2, h);
        }
    }

    // This is an image-grid-based implementation of FontData.
    // The texture is a 16 * 16 grid of characters.
    public class ImageGridFontData: FontData
    {
        // The texture which contains the font pixel information
        private Texture texture;

        // Ratio = Width / Height. used to scale the font
        // This has no effect on the size of the cells
        // Instead, a cell is placed inside a rect on the screen
        // with this specified ratio.
        private double ratio = 1;
        public double Ratio {
            get {
                return ratio;
            }
            set {
                ratio = value;
            }
        }
        
        // Each character cell gets a small margin, relative
        // to the size of the cell, in order to avoid overlapping.
        private double margin = 0;
        public double Margin {
            get {
                return margin;
            }
            set {
                margin = value;
            }
        }

        // Translate each glyph by a given amount (relative to height).
        // This can compensate for off-centered image grids.
        private double xTranslate;
        public double XTranslate {
            get {
                return xTranslate;
            }
            set {
                xTranslate = value;
            }
        }

        // Get the width of one char.
        public override double GetCharWidth(char c)
        {
            return Ratio;
        }
        
        // Implementation of FontData.DrawChar
        public override void DrawChar(char c, double x, double y, double height)
        {
            double width = height * Ratio, m;
            int cx, cy;

            if (c > 255) {
                DrawCharHex(c, x, y, height);
                return;
            }
            
            m = Margin / 16.0;

            texture.Bind();
            GL.PushMatrix();
            GL.Translate(x + XTranslate * height, y, 0);
            GL.Begin(DrawMode.Quads);

            cx = (c - 32) % 16;
            cy = (c - 32) / 16;
            GL.TexCoord(cx / 16.0 + m, cy / 16.0 + m);
            GL.Vertex(0, 0, 0);
            GL.TexCoord((cx + 1) / 16.0 - m, cy / 16.0 + m);
            GL.Vertex(width, 0, 0);
            GL.TexCoord((cx + 1) / 16.0 - m, (cy + 1) / 16.0 - m);
            GL.Vertex(width, height, 0);
            GL.TexCoord(cx / 16.0 + m, (cy + 1) / 16.0 - m);
            GL.Vertex(0, height, 0);

            GL.End();
            texture.Unbind();
            GL.Disable(Feature.Texture);
            GL.PopMatrix();
        }

        public ImageGridFontData(Texture tex)
        {
            texture = tex;
        }

        // Wrappers for the above constructor
        public ImageGridFontData(string file): this (new Texture(file))
        {
        }

        // Create an imagegrid font using FontFamily.GenericMonospace.
        // width and height are the size of the image
        public static ImageGridFontData CreateFromSystemFont()
        {
            System.Drawing.Font font = new System.Drawing.Font(FontFamily.GenericMonospace, 10, FontStyle.Bold);
            Bitmap bmp = new Bitmap(256, 256, PixelFormat.Format32bppArgb);
            Brush brush = new SolidBrush(Color.FromArgb(255, 255, 255, 255));
            System.Drawing.Graphics graph = System.Drawing.Graphics.FromImage(bmp);
            int margin = 0;
            int c;

            graph.Clear(Color.FromArgb(0, 255, 255, 255));
            for (c = 0; c <= 255; ++c) {
                int x = (c % 16) * 16;
                int y = (c / 16) * 16;
                RectangleF rect = new RectangleF(x + margin, y + margin, x + 16 - margin, y + 16 - margin);
                graph.DrawString(((char)(c + 32)).ToString(), font, brush, rect);
            }

            ImageGridFontData res = new ImageGridFontData(new Texture(bmp));
            res.Margin = margin / 16;
            return res;
        }
    }
}

