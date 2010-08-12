// Copyright (C) 2004-2005 Crestez Leonard    cleonard@go.ro
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

using Alpha.Graphics;

namespace Alpha.UI
{
    // Simple cursor class.
    // It only contains a Draw function.
    //
    // A cursor is drawn in widget space, coordinates and sizes mean the
    // same thing they mean for widgets.
    //
    // From the UI's point of view, the Cursor is stateless.
    // Feel free to change UI.Cursor at will.
    public abstract class Cursor
    {
        // Draw the cursor with the hotspot at x, y
        public abstract void Draw(double x, double y);
    }

    // Simple implementation of Cursor.
    // This function draws a Texture as a cursor.
    //
    // There is support for setting a hotspot, the point in the cursor where clicking is done.
    // Information like Width/Height is in widget space.
    public class ImageCursor: Cursor
    {
        // The image of the cursor.
        public Texture Image;

        // The width of the cursor on the screen.
        public double Width;

        // The height of the cursor on the screen.
        public double Height;

        // The hotstop X location in the cursor.
        // The hotstop is the place in the cursor that "clicks".
        // This will in fact translate the cursor image by -HotX, -HotY
        public double HotX;

        // The hotstop Y location in the cursor.
        // The hotstop is the place in the cursor that "clicks".
        // This will in fact translate the cursor image by -HotX, -HotY
        public double HotY;

        // The width of the cursor
        public override void Draw(double X, double Y)
        {
            if (Image == null) {
                throw new InvalidOperationException("ImageCursor needs Image, otherwise it can't draw.");
            }
            Image.Bind();

            GL.Begin(DrawMode.Quads);
            GL.Color(255, 255, 255);
            GL.TexCoord(0, 0);
            GL.Vertex(X - HotX, Y - HotY);
            GL.TexCoord(1, 0);
            GL.Vertex(X + Width - HotX, Y - HotY);
            GL.TexCoord(1, 1);
            GL.Vertex(X + Width - HotX, Y + Height - HotY);
            GL.TexCoord(0, 1);
            GL.Vertex(X - HotX, Y + Height - HotY);
            GL.End();

            Image.Unbind();
        }
    }
}
