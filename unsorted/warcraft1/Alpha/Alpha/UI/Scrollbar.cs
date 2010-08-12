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
    // The scrollbar widget
    public abstract class BaseScrollbar: Widget, IScrollbar
    {
        // Style information.
        private IScrollbarStyle style;
        public IScrollbarStyle Style {
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
 
        // This is the current value of this scrollbar.
        // It can range from MinValue to MaxValue (use SetRange to change those).
        // By default it ranges from 0 to 100.
        //
        // Setting this to a value outside for the MinValue, MaxValue range
        // will result in the value getting silently clamped.
        private double theValue;
        public double Value {
            get {
                return theValue;
            }
            set {
                if (value > MaxValue) {
                    value = MaxValue;
                }
                if (value < MinValue) {
                    value = MinValue;
                }
                bool fireEvent = Math.Abs(value - Value) > 0.0001;
                theValue = value;
                if (fireEvent) {
                    OnScroll();
                }
            }
        }

        // Minimum value (inclusive!) for this scrollbar.
        // The range defaults to 0, 100
        // Use SetRange to change it.
        private double minValue;
        public double MinValue {
            get {
                return minValue;
            }
        }

        // Maximum value (inclusive!) for this scrollbar.
        // The range defaults to 0, 100
        // Use SetRange to change it.
        private double maxValue;
        public double MaxValue {
            get {
                return maxValue;
            }
        }

        // Set the range for this scrollbar.
        // If min < max it will swap them.
        // Default range is from 0 to 100.
        public void SetRange(double min, double max)
        {
            minValue = Math.Min(min, max);
            maxValue = Math.Max(min, max);
            // Force clamping.
            Value = Value;
        }

        // Defaults to 0. Ignored if it causes the scrollbar knob to
        // be smaller than Style.MinScrollbarKnobSize.
        // Clamped to positive values.
        private double cover;
        public double Cover {
            get {
                return cover;
            }
            set {
                cover = Math.Max(0, value);
            }
        }

        // This is how much the value increases with when clicking one
        // of the buttons.
        private double buttonJumpAmount;
        public double ButtonJumpAmount {
            get {
                return buttonJumpAmount;
            }
            set {
                buttonJumpAmount = Math.Max(0, value);
            }
        }

        // buttoninc and buttondec are the two buttons on the scrollbar
        // this boolean values are true if the buttons are clicked
        // useful to fix a "bug" in event generation
        private bool buttonIncClicked;
        private bool buttonDecClicked;
        
        // if user has cliked the scroll buton and it is holding it pressed
        // useful to turn move events into value changes
        private bool scrolling;

        // We use this to store the last real position of the mouse pointer
        // in order to calculate the value difference and then add it to Value.
        // This way we can drag from every point of the knob.
        private double lastPosition;

        // This event is called when val is changed
        public event EventHandler Scrolled;

        // Create a new Scrollbar, with reasonable default values.
        public BaseScrollbar(IScrollbarStyle s)
        {
            Style = s;
            SetRange(0, 100);
            ButtonJumpAmount = 10;
        }

        // Get the size of a button.
        protected double GetButtonSize()
        {
            return Math.Min(Style.ScrollbarButtonSize, GetScrollAxisSize() / 2);
        }

        // Get the actual size of the scrollbar knob.
        protected double GetKnobSize()
        {
            double size = Cover * (GetScrollAxisSize() - 2 * GetButtonSize()) / (MaxValue - MinValue + Cover);
            return Math.Max(size, Style.ScrollbarMinKnobSize);
        }

        // Gets the position of the knob
        protected double GetKnobPosition()
        {
            double f = (GetScrollAxisSize() - 2 * GetButtonSize() - GetKnobSize()) / (MaxValue - MinValue);
            return GetButtonSize() + Value * f;
        }

        // Gets the size of the scrollbar in the relevant dimension.
        protected abstract double GetScrollAxisSize();

        // Get the WidgetDrawFlags for the knob
        protected WidgetDrawFlags GetKnobFlags()
        {
            return scrolling ? WidgetDrawFlags.MouseFocus : 0;
        }

        // Get the WidgetDrawFlags for the inc button
        protected WidgetDrawFlags GetIncButtonFlags()
        {
            return buttonIncClicked ? WidgetDrawFlags.MouseFocus : 0;
        }

        // Get the WidgetDrawFlags for the dec button
        protected WidgetDrawFlags GetDecButtonFlags()
        {
            return buttonDecClicked ? WidgetDrawFlags.MouseFocus : 0;
        }

        // Handle a mouse button press. loc is the location of the click in the
        // scrollingDimension (see GetScrollAxisSize)
        protected void HandleMouseDown(double loc, MouseButton btns, MouseButton btn)
        {
            double bsize = GetButtonSize();
            if (loc < bsize) {
                // Inc button pressed.
                buttonIncClicked = true;
            } else if (loc > GetScrollAxisSize() - bsize) {
                // Dec button pressed.
                buttonDecClicked = true;
            } else if ((loc > GetKnobPosition()) && 
                       (loc < GetKnobPosition() + GetKnobSize())) {
                lastPosition = loc;
                scrolling = true;
            }
        }

        // Handle mouse up events along the scrolling axis.
        protected void HandleMouseUp(double loc, MouseButton btns, MouseButton btn)
        {
            buttonIncClicked = buttonDecClicked = scrolling = false;
            if (loc < GetButtonSize()) {
                Value -= ButtonJumpAmount;
            } else if (loc > GetScrollAxisSize() - GetButtonSize()) {
                Value += ButtonJumpAmount;
            }
        }

        // Handle mouse movement long the scrolling axis.
        protected void HandleMouseMove(double loc, MouseButton btns)
        {
            double bsize = GetButtonSize();
            // We only consider movement if made over the scroll button
            if (scrolling && loc > bsize && loc < GetScrollAxisSize() - bsize) {
                // Here we calculate the new value using the last position of the pointer and the size
                // of the bar(scroll button)
                Value += (loc - lastPosition) * (MaxValue - MinValue) / (GetScrollAxisSize() - 2 * bsize - GetKnobSize());
                // We store the new position of the pointer for later use
                lastPosition = loc;
            }
        }

        public override void OnLoseMouseFocus()
        {
            buttonIncClicked = buttonDecClicked = scrolling = false;
            base.OnLoseMouseFocus();
        }

        // Generate the Scrolled event.
        protected void OnScroll()
        {
            if (Scrolled != null) {
                Scrolled(this, new EventArgs());
            }
        }
    }

    // Scrollbar along the X axis.
    // This is a very thin class, most of the work in XScrollbar and YScrollbar
    // is shared in BaseScrollbar.
    public class XScrollbar: BaseScrollbar
    {
        // Returns the widget's Width.
        protected override double GetScrollAxisSize()
        {
            return Width;
        }

        // Here we draw it
        public override void Draw(RectangleD ClipRect, RectangleD WindowClipRect)
        {
            Style.DrawScrollbarKnob(RectangleD.FromXYWidthHeight(GetKnobPosition(), 0, GetKnobSize(), Height),
                    GetKnobFlags(), true);
            Style.DrawScrollbarButton(RectangleD.FromXYWidthHeight(0, 0, GetButtonSize(), Height),
                    GetIncButtonFlags(), true, false);
            Style.DrawScrollbarButton(RectangleD.FromXYWidthHeight(Width - GetButtonSize(), 0, GetButtonSize(), Height),
                    GetDecButtonFlags(), true, true);
            base.Draw(ClipRect, WindowClipRect);
        }

        // Forward to HandleMouseDown/Up/Move
        public override void OnMouseEvent(MouseEventArgs e)
        {
            if (e.EventType == MouseEventType.ButtonDown) {
                HandleMouseDown(e.Location.X, e.ButtonState, e.Button);
            } else if (e.EventType == MouseEventType.Movement) {
                HandleMouseMove(e.Location.X, e.ButtonState);
            } else if (e.EventType == MouseEventType.ButtonUp) {
                HandleMouseUp(e.Location.X, e.ButtonState, e.Button);
            }
            base.OnMouseEvent(e);
        }

        // Return the desired width.
        public override double GetDesiredWidth()
        {
            return 2 * Style.ScrollbarButtonSize + Style.ScrollbarMinKnobSize;
        }

        // Return the desired height.
        public override double GetDesiredHeight()
        {
            return Style.ScrollbarLateralSize;
        }

        public XScrollbar(IStyle s): base(s)
        {
        }
    }

    // Scrollbar along the Y axis.
    // This is a very thin class, most of the work in XScrollbar and YScrollbar
    // is shared in BaseScrollbar.
    public class YScrollbar: BaseScrollbar
    {
        // Returns the widget's Height.
        protected override double GetScrollAxisSize()
        {
            return Height;
        }

        // Here we draw it
        public override void Draw(RectangleD ClipRect, RectangleD WindowClipRect)
        {
            Style.DrawScrollbarKnob(RectangleD.FromXYWidthHeight(0, GetKnobPosition(), Width, GetKnobSize()),
                    GetKnobFlags(), false);
            Style.DrawScrollbarButton(RectangleD.FromXYWidthHeight(0, 0, Width, GetButtonSize()),
                    GetIncButtonFlags(), false, true);
            Style.DrawScrollbarButton(RectangleD.FromXYWidthHeight(0, Height - GetButtonSize(), Width, GetButtonSize()),
                    GetDecButtonFlags(), false, false);
            base.Draw(ClipRect, WindowClipRect);
        }

        // Forward to HandleMouseDown/Up/Move
        public override void OnMouseEvent(MouseEventArgs e)
        {
            if (e.EventType == MouseEventType.ButtonDown) {
                HandleMouseDown(e.Location.Y, e.ButtonState, e.Button);
            } else if (e.EventType == MouseEventType.Movement) {
                HandleMouseMove(e.Location.Y, e.ButtonState);
            } else if (e.EventType == MouseEventType.ButtonUp) {
                HandleMouseUp(e.Location.Y, e.ButtonState, e.Button);
            }
            base.OnMouseEvent(e);
        }
       
        // Return the desired width.
        public override double GetDesiredWidth()
        {
            return Style.ScrollbarLateralSize;
        }

        // Return the desired height.
        public override double GetDesiredHeight()
        {
            return 2 * Style.ScrollbarButtonSize + Style.ScrollbarMinKnobSize;
        }

        public YScrollbar(IStyle s): base(s)
        {
        }
    }
}

