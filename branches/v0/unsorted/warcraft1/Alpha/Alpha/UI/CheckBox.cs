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
    // CheckBox widget.
    public class CheckBox: Widget, ICheckBox
    {
        // Style information.
        private ICheckBoxStyle style;
        public ICheckBoxStyle Style {
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

        // If this is checked or not.
        private bool isChecked = false;
        public bool IsChecked {
            get {
                return isChecked;
            }
            set {
                isChecked = value;
                if (isChecked != value) {
                    OnToggle();
                }
            }
        }

        // Event fired when IsChecked changes.
        public event EventHandler Toggle;

        // Fire the Toggle event.
        protected void OnToggle()
        {
            if (Toggle != null) {
                Toggle(this, new EventArgs());
            }
        }

        // Get the actual rectangle with the checkbox.
        private RectangleD GetRect()
        {
            double w, h;
            w = Math.Min(GetDesiredWidth(), Width);
            h = Math.Min(GetDesiredHeight(), Height);
            return new RectangleD((Width - w) / 2, (Height - h) / 2, (Width + w) / 2, (Height + h) / 2);
        }

        // Draw the widget
        // This draws a box sized Height * 2 / 3 on the left and then the text.
        public override void Draw(RectangleD ClipRect, RectangleD WindowClipRect)
        {
            Style.DrawCheckBox(GetRect(), GetDrawFlags(), IsChecked);
            base.Draw(ClipRect, WindowClipRect);
        }

        // Check or uncheck the box when it's pressed
        public override void OnMouseEvent(MouseEventArgs e)
        {
            if (e.EventType == MouseEventType.ButtonUp && e.Button == MouseButton.Left && Pressed) {
                IsChecked = !IsChecked;
            }
            base.OnMouseEvent(e);
        }

        public override double GetDesiredWidth()
        {
            return Style.GetCheckBoxWidth(GetDrawFlags(), IsChecked);
        }

        public override double GetDesiredHeight()
        {
            return Style.GetCheckBoxWidth(GetDrawFlags(), IsChecked);
        }

        public CheckBox(ICheckBoxStyle s)
        {
            Style = s;
        }
       
        // Constructor that also sets the initial toggle state.
        public CheckBox(bool check, IStyle s): this(s)
        {
            isChecked = check;
        }
    }
}

