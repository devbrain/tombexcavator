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
    // Simple Widget that draws a box with an image.
    // You can set different images for when it has mouse focus
    // and when a mouse button is pressed.
    // If one of those are null, they degrade:
    // PressedTexture to MouseFocusTexture to NormalTexture.
    //
    // If you just set NormalTexture then it will have the same look
    // at all times.
    public class ImageBox: Widget
    {
        // Texture under normal conditions.
        public Texture Texture;

        // Texture when under mouse focus.
        public Texture MouseFocusTexture;

        // Texture when a mouse button was pressed.
        public Texture PressedTexture;

        // Get the texture to draw with.
        private Texture GetTexture()
        {
            if (Pressed & PressedTexture != null) {
                return PressedTexture;
            }
            if (MouseFocus && MouseFocusTexture != null) {
                return MouseFocusTexture;
            }
            return Texture;
        }

        public override void Draw(RectangleD ClipRect, RectangleD WindowClipRect)
        {           
            Texture tex = GetTexture();

            if (tex == null) {
                return;
            }

            GL.Color(Color.White);
            tex.Bind();
            GL.Begin(DrawMode.Quads);

            GL.TexCoord(0, 0);
            GL.Vertex(0, 0);
            GL.TexCoord(1, 0);
            GL.Vertex(Width, 0);
            GL.TexCoord(1, 1);
            GL.Vertex(Width, Height);
            GL.TexCoord(0, 1);
            GL.Vertex(0, Height);

            GL.End();
            tex.Unbind();
            base.Draw(ClipRect, WindowClipRect);
        }
    }
}
