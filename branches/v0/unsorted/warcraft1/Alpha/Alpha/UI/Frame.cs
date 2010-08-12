// Copyright (C) 2006 Crestez Leonard    cleonard@go.ro
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

namespace Alpha.UI
{
    // Draws a border around the child widget.
    //
    // This is a classic exemple of the decorator parent, where an object
    // wraps around another object and offers additional behaviour.
    //
    // Frame is a widget so you can add how many children you want, but only
    // one of those is bordered (this.Child). If you want to make a panel with
    // a border, don't add widget to this. Add widgets to a panel (like an empty
    // widget) and make the panel a child of a border.
    public class Frame: BaseDecorator, IDecorator
    {
        // Style information.
        private IFrameStyle style;
        public IFrameStyle Style {
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

        // The kind of frame to draw.
        private FrameType ft = FrameType.Content;
        public FrameType FrameType {
            get {
                return ft;
            }
            set {
                ft = value;
            }
        }

        // If this is set to true, ignore mouse focus when drawing.
        // True by default.
        private bool ignoreMouseFocus = true;
        public bool IgnoreMouseFocus {
            get {
                return ignoreMouseFocus;
            }
            set {
                ignoreMouseFocus = value;
            }
        }

        public override void Draw(RectangleD ClipRect, RectangleD WindowClipRect)
        {
            Style.DrawFrame(new RectangleD(0, 0, Width, Height), GetDrawFlags(), FrameType);
            base.Draw(ClipRect, WindowClipRect);
        }

        // Get the inner bounds of the frame.
        protected override RectangleD GetInnerBounds()
        {
            return Style.GetFrameInnerBounds(new RectangleD(0, 0, Width, Height), GetDrawFlags(), FrameType);
        }

        // Default constructor.
        public Frame(IFrameStyle s)
        {
            Style = s;
        }

        // Create and assign a child.
        public Frame(IFrameStyle s, IWidget child): this(s)
        {
            Child = child;
        }
    }
}
