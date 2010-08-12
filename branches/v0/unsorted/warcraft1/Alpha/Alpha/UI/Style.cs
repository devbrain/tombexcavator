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

using Alpha.Graphics;
using Alpha.Util;

namespace Alpha.UI
{
    /// Enumeration for common widget (or widget component) drawing flags.
    /// This is here to avoid passing a ton of bool parameters when
    /// drawing widget components.
    [Flags]
    public enum WidgetDrawFlags
    {
        // No special flags. Writing 0 is sort of ugly.
        None = 0,

        // If the widget has mouse focus.
        MouseFocus = 1,
                   
        // If the widget has keyboard focus.
        KeyFocus = 2,
        
        // If a mouse button is pressed.
        // Shouldn't be set if MouseFocus isn't set.
        Pressed = 4,
        
        // If the widget is disabled.
        Disabled = 8,
        
        // If the widget should be hollow.
        // If this is set only the border will be drawn.
        // This is useful if you draw the contents yourself
        // and want to avoid useless redrawing.
        Hollow = 16,
    }

    // Style class...should contain the LookNFeel for the widgets
    public class Style: IStyle
    {
        // Default font for the UI.
        private IFont font;
        public IFont Font {
            get {
                return font;
            }
            set {
                if (value == null) {
                    throw new ArgumentNullException("value");
                }
                font = value;
            }
        }

        public FontData FontData {
            get {
                return ((SimpleFont)font).Data;
            }
        }

        // Normal background for widgets.
        public Color NormalBackColor;

        // Highlighted background (like button mouse over)
        public Color HighlightBackColor;

        // Disabled widget background.
        public Color DisabledBackColor;

        // Normal margin for widgets.
        public Color NormalMarginColor;

        // Highlighted margin (like button mouse over)
        public Color HighlightMarginColor;

        // Disabled widget margin.
        public Color DisabledMarginColor;

        // This is the background color for edit boxes, list boxes, etc.
        public Color EmptyBackColor;

        // This is the disable background color for edit boxes, list boxes, etc.
        public Color DisabledEmptyBackColor;

        // The color for selections.
        private Color selectionColor;
        public Color SelectionColor {
            get {
                return selectionColor;
            }
            set {
                selectionColor = value;
            }
        }

        // Width for margins.
        public double Margin;

        // Default width for scrollbars.
        private double scrollbarLateralSize;
        public double ScrollbarLateralSize {
            get {
                return scrollbarLateralSize;
            }
            set {
                scrollbarLateralSize = value;
            }
        }

        // The size of a scrollbar button.
        private double scrollbarButtonSize;
        public double ScrollbarButtonSize {
            get {
                return scrollbarButtonSize;
            }
            set {
                scrollbarButtonSize = value;
            }
        }

        // Minimum size of a scrollbar knob.
        private double scrollbarMinKnobSize;
        public double ScrollbarMinKnobSize {
            get {
                return scrollbarMinKnobSize;
            }
            set {
                scrollbarMinKnobSize = value;
            }
        }

        public Style(IFont f)
        {
            Font = f;

            DisabledBackColor       = Color.FromArgb(100,  25,  50,  75);
            DisabledMarginColor     = Color.FromArgb(150,  75, 100, 125);

            NormalBackColor         = Color.FromArgb(150,  75, 100, 125);
            NormalMarginColor       = Color.FromArgb(200, 125, 150, 175);

            HighlightBackColor      = Color.FromArgb(200, 125, 150, 175);
            HighlightMarginColor    = Color.FromArgb(250, 175, 200, 225);

            EmptyBackColor          = Color.FromArgb(200,  25,  50,  75);
            DisabledEmptyBackColor  = Color.FromArgb(150,   0,  25,  25);

            SelectionColor          = Color.FromArgb(150, 250, 200, 150);

            ScrollbarLateralSize = 0.03;
            ScrollbarButtonSize = 0.03;
            ScrollbarMinKnobSize = 0.06;
            Margin = 0.003;
        }

        // Draw a rectangle.
        public void DrawRect(RectangleD r)
        {
            GL.Begin(DrawMode.Quads);
            GL.Vertex(r.X1, r.Y1);
            GL.Vertex(r.X2, r.Y1);
            GL.Vertex(r.X2, r.Y2);
            GL.Vertex(r.X1, r.Y2);
            GL.End();
        }

        // Draw the margins of a rectangle, according to MarginWidth.
        public void DrawMargin(RectangleD r)
        {
            GL.Begin(DrawMode.QuadStrip);
            GL.Vertex(r.X1         , r.Y1         );
            GL.Vertex(r.X1 + Margin, r.Y1 + Margin);
            GL.Vertex(r.X2         , r.Y1         );
            GL.Vertex(r.X2 - Margin, r.Y1 + Margin);
            GL.Vertex(r.X2         , r.Y2         );
            GL.Vertex(r.X2 - Margin, r.Y2 - Margin);
            GL.Vertex(r.X1         , r.Y2         );
            GL.Vertex(r.X1 + Margin, r.Y2 - Margin);
            GL.Vertex(r.X1         , r.Y1         );
            GL.Vertex(r.X1 + Margin, r.Y1 + Margin);
            GL.End();
        }

        // Draw a rect with a margin.
        // x, y, w, h are dimension of the rectangle.
        // back is the background color
        // margin is the margin color
        // 
        // This uses this.MarginWidth
        public void DrawMarginRect(RectangleD rect, Color back, Color margin, bool hollow)
        {
            GL.Disable(Feature.Texture);
            if (!hollow) {
                GL.Color(back);
                DrawRect(rect);
            }
            GL.Color(margin);
            DrawMargin(rect);
        }

        // Sets hollow to false.
        public void DrawMarginRect(RectangleD rect, Color back, Color margin)
        {
            DrawMarginRect(rect, back, margin, false);
        }

        // Draw a simple button widget.
        public void DrawButton(RectangleD rect, WidgetDrawFlags flags)
        {
            Color back, margin;

            if ((flags & WidgetDrawFlags.MouseFocus) != 0) {
                if ((flags & WidgetDrawFlags.Pressed) != 0) {
                    back = HighlightMarginColor;
                    margin = HighlightMarginColor;
                } else {
                    back = HighlightBackColor;
                    margin = HighlightMarginColor;
                }
            } else {
                back = NormalBackColor;
                margin = NormalMarginColor;
            }
            DrawMarginRect(rect, back, margin, (flags & WidgetDrawFlags.Hollow) != 0);
        }

        // Get the inner bounds of a button. This is the actual content of the button,
        // asside from the frame.
        public RectangleD GetButtonInnerBounds(RectangleD rect, WidgetDrawFlags flags)
        {
            return rect.Shrink(2 * Margin);
        }

        // Draw a simple editbox widget.
        public void DrawEditBox(RectangleD rect, WidgetDrawFlags flags)
        {
            Color back, margin;

            if ((flags & WidgetDrawFlags.KeyFocus) != 0) {
                back = HighlightBackColor;
                margin = HighlightMarginColor;
            } else {
                back = NormalBackColor;
                margin = NormalMarginColor;
            }
            DrawMarginRect(rect, back, margin, (flags & WidgetDrawFlags.Hollow) != 0);
        }

        // Get the inner bounds of an editbox.
        public RectangleD GetEditBoxInnerBounds(RectangleD rect, WidgetDrawFlags flags)
        {
            return rect.Shrink(2 * Margin);
        }

        // Draw a rectangular frame.
        public void DrawFrame(RectangleD rect, WidgetDrawFlags flags, FrameType ft)
        {
            Color back, margin;

            if (ft == FrameType.Raised || ft == FrameType.Sunken || ft == FrameType.Outline) {
                back = NormalBackColor;
                margin = NormalMarginColor;
            } else if (ft == FrameType.Content) {
                back = EmptyBackColor;
                margin = NormalMarginColor;
            } else {
                throw new ArgumentException("ft");
            }

            DrawMarginRect(rect, back, margin, (flags & WidgetDrawFlags.Hollow) != 0);
        }

        // Get the inner bounds of a certain frame.
        public RectangleD GetFrameInnerBounds(RectangleD rect, WidgetDrawFlags flags, FrameType ft)
        {
            return rect.Shrink(2 * Margin);
        }

        // Draw a scrollbar button, one of those little things with arrows on them.
        public void DrawScrollbarButton(RectangleD rect, WidgetDrawFlags flags, bool xaxis, bool increase)
        {
            Color back, margin;
            if ((flags & WidgetDrawFlags.MouseFocus) != 0) {
                back = HighlightBackColor;
                margin = HighlightMarginColor;
            } else {
                back = NormalBackColor;
                margin = NormalMarginColor;
            }
            DrawMarginRect(rect, back, margin);

            // Draw an arrow.
            GL.Begin(DrawMode.Triangles);
            GL.Color(margin);
            double x = rect.X1;
            double y = rect.Y1;
            double w = rect.Width;
            double h = rect.Height;
            if (xaxis) {
                if (increase) {
                    GL.Vertex(x + 2 * w / 3, y + h / 2);
                    GL.Vertex(x + w / 3, y + h / 3);
                    GL.Vertex(x + w / 3, y + 2 * h / 3);
                } else {
                    GL.Vertex(x + w / 3, y + h / 2);
                    GL.Vertex(x + 2 * w / 3, y + h / 3);
                    GL.Vertex(x + 2 * w / 3, y + 2 * h / 3);
                }
            } else {
                if (increase) {
                    GL.Vertex(x + w / 2, y + h / 3);
                    GL.Vertex(x + w / 3, y + 2 * h / 3);
                    GL.Vertex(x + 2 * w / 3, y + 2 * h / 3);
                } else {
                    GL.Vertex(x + w / 2, y + 2 * h / 3);
                    GL.Vertex(x + w / 3, y + h / 3);
                    GL.Vertex(x + 2 * w / 3, y + h / 3);
                }
            }
            GL.End();
        }

        // Draw a scrollbar knob.
        // The knob is the big thing that moves around between the buttons.
        public void DrawScrollbarKnob(RectangleD rect, WidgetDrawFlags flags, bool xaxis)
        {
            DrawMarginRect(rect, NormalBackColor, NormalMarginColor);
        }

        // Draw a tiny carret at x, y, in a text with Font f
        public void DrawCarret(IFont font, double x, double y)
        {
            double h = font.GetHeight();
            GL.Color(HighlightMarginColor);
            DrawRect(RectangleD.FromXYWidthHeight(x, y - h / 8, h / 6, h + h / 4));
        }

        // Draw a checkbox.
        public void DrawCheckBox(RectangleD rect, WidgetDrawFlags flags, bool check)
        {
            DrawButton(rect, flags | (!check ? WidgetDrawFlags.Hollow: 0));
        }

        // Get the desired width of a checkbox.
        public double GetCheckBoxWidth(WidgetDrawFlags flags, bool check)
        {
            return 0.03;
        }

        // Get the desired height of a checkbox.
        public double GetCheckBoxHeight(WidgetDrawFlags flags, bool check)
        {
            return 0.03;
        }
    }
}
