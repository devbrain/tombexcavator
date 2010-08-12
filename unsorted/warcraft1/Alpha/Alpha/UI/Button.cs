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
    // Simple button widget.
    public class Button: BaseDecorator, IButton
    {
        // Style information.
        private IButtonStyle style;
        public IButtonStyle Style {
            get {
                return style;
            }
            set {
                if (value == null) {
                    throw new ArgumentNullException("value");
                }
                style = value;
            }
        }

        // When the user click the widget. A click is defined as a released left mouse button.
        public event EventHandler Click;

        // Default constructor.
        public Button(IButtonStyle s)
        {
            Style = s;
        }

        // Create a button with a label.
        public Button(IButtonStyle style, IFont font, string text): this(style)
        {
            TextLayout layout;
            layout = new TextLayout(font);
            layout.HorizontalAlignment = HorizontalTextAlignment.Center;
            layout.VerticalAlignment = VerticalTextAlignment.Center;
            layout.Text = text;
            Child = layout;
        }

        // Create a button with a label.
        public Button(IStyle style, string text): this(style, style.Font, text)
        {
        }

        // Get the inner bounds of the button.
        protected override RectangleD GetInnerBounds()
        {
            return Style.GetButtonInnerBounds(new RectangleD(0, 0, Width, Height), GetDrawFlags());
        }

        // Draw the button.
        public override void Draw(RectangleD ClipRect, RectangleD WindowClipRect)
        {
            Style.DrawButton(new RectangleD(0, 0, Width, Height), GetDrawFlags());
            base.Draw(ClipRect, WindowClipRect);
        }

        // Override OnMouseEvent to generate click events.
        public override void OnMouseEvent(MouseEventArgs e)
        {
            if (e.EventType == MouseEventType.ButtonUp && e.Button == MouseButton.Left && Pressed) {
                if (Click != null) {
                    Click(this, new EventArgs());
                }
            }
            base.OnMouseEvent(e);
        }
    }

    // Button style done with a bunch of images.
    public class ImageButtonStyle: IButtonStyle
    {
        // Normal background of the button.
        public Texture NormalTexture;

        // Background for when it's under the mouse. If null uses NormalTexture.
        public Texture HighlightTexture;

        // Background for when it's Pressed. If null uses NormalTexture.
        public Texture PressedTexture;

        // Draw the button.
        public void DrawButton(RectangleD rect, WidgetDrawFlags flags)
        {
            Texture tex;
            if ((flags & WidgetDrawFlags.Pressed) != 0 && PressedTexture != null) {
                tex = PressedTexture;
            } else if ((flags & WidgetDrawFlags.MouseFocus) != 0 && HighlightTexture != null) {
                tex = HighlightTexture;
            } else {
                tex = NormalTexture;
            }

            tex.Bind();
            GL.Begin(DrawMode.Quads);
            GL.Color(255, 255, 255);
            GL.TexCoord(0, 0);
            GL.Vertex(rect.X1, rect.Y1);
            GL.TexCoord(1, 0);
            GL.Vertex(rect.X2, rect.Y1);
            GL.TexCoord(1, 1);
            GL.Vertex(rect.X2, rect.Y2);
            GL.TexCoord(0, 1);
            GL.Vertex(rect.X1, rect.Y2);
            GL.End();
            tex.Unbind();
        }

        // Doesn't shrink the button.
        public RectangleD GetButtonInnerBounds(RectangleD rect, WidgetDrawFlags flags)
        {
            return new RectangleD(0, 0, rect.Width, rect.Height);
        }
    }
}

