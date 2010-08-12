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

using Alpha.Graphics;
using Alpha.Util;

namespace Alpha.UI
{
    public class Scroller: Widget, IDecorator
    {
        // This is a small widget that translates its children.
        private class TranslatingContainer: BaseDecorator, IDecorator
        {
            // X scrolling for the children.
            public double ScrollX;

            // Y scrolling for the children.
            public double ScrollY;

            // Transform to child coordinates.
            protected override PointD Transform(PointD p)
            {
                p = base.Transform(p);
                return new PointD(p.X + ScrollX, p.Y + ScrollY);
            }

            protected override void GLTransform()
            {
                base.GLTransform();
                GL.Translate(-ScrollX, -ScrollY, 0);
            }
        }

        // The widget to scroll.
        // It's not really a child of the scroller, but a child of the
        // translator.
        public IWidget Child {
            get {
                return translator.Child;
            }
            set {
                translator.Child = value;
            }
        }

        // This is the widget that contains the child, and does translation.
        private readonly TranslatingContainer translator;

        // Horizontal scrollbar. Set to hidden when not used.
        private readonly XScrollbar xscrollbar;

        // Vertical scrollbar. Set to hidden when not used.
        private readonly YScrollbar yscrollbar;

        // Calculates if to show the scrollbars or not.
        // Rather complex.
        protected override void CalculateChildSizes()
        {
            base.CalculateChildSizes();

            // A bunch of aliases for long and expensive calls.
            double cw, ch, sw, sh, w, h;
            w = Width;
            h = Height;
            if (Child == null) {
                cw = ch = 0;
            } else {
                cw = Child.GetDesiredWidth();
                ch = Child.GetDesiredHeight();
            }
            sw = yscrollbar.GetDesiredWidth();
            sh = xscrollbar.GetDesiredHeight();

            // Determine which scrollbars to show.
            if (Child == null || w < sw || h < sh || (cw < w && ch < h)) {
                xscrollbar.Hidden = true;
                yscrollbar.Hidden = true;
            } else if (ch < h - sh) {
                xscrollbar.Hidden = false;
                yscrollbar.Hidden = true;
            } else if (cw < w - sw) {
                xscrollbar.Hidden = true;
                yscrollbar.Hidden = false;
            } else {
                xscrollbar.Hidden = false;
                yscrollbar.Hidden = false;
            }
           
            // Setup x scrollbar.
            if (xscrollbar.Hidden) {
                xscrollbar.Value = 0;
            } else {
                xscrollbar.SetSize(0, h - sh, w - (yscrollbar.Hidden ? 0 : sw), sh);
                xscrollbar.SetRange(0, cw - xscrollbar.Width);
                xscrollbar.Cover = xscrollbar.Width;
                xscrollbar.ButtonJumpAmount = xscrollbar.Width / 5;
            }

            // Setup y scrollbar.
            if (yscrollbar.Hidden) {
                yscrollbar.Value = 0;
            } else {
                yscrollbar.SetSize(w - sw, 0, sw, h - (xscrollbar.Hidden ? 0 : sh));
                yscrollbar.SetRange(0, ch - yscrollbar.Height);
                yscrollbar.Cover = yscrollbar.Height;
                yscrollbar.ButtonJumpAmount = yscrollbar.Height / 5;
            }

            // Finalize.
            translator.SetSize(0, 0, w - (yscrollbar.Hidden ? 0 : sw), h - (xscrollbar.Hidden ? 0 : sh));
            if (Child != null) {
                // Fill the scroller even if you don't need all the space.
                Child.SetSize(0, 0, Math.Max(translator.Width, cw), Math.Max(translator.Height, ch));
            }
        }

        // Constructor.
        public Scroller(IStyle s)
        {
            translator = new TranslatingContainer();
            xscrollbar = new XScrollbar(s);
            xscrollbar.Scrolled += new EventHandler(Scrollbar_Scrolled);
            yscrollbar = new YScrollbar(s);
            yscrollbar.Scrolled += new EventHandler(Scrollbar_Scrolled);
            this.Add(translator);
            this.AddOnTop(xscrollbar);
            this.AddOnTop(yscrollbar);
        }

        // When the scrollbar scrolls.
        private void Scrollbar_Scrolled(object sender, EventArgs args)
        {
            translator.ScrollY = yscrollbar.Value;
            translator.ScrollX = xscrollbar.Value;
        }
    }
}

