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
using System.Collections;
using SD = System.Drawing;

using Alpha.Graphics;
using Alpha.Util;

namespace Alpha.UI
{
    // Widget, the basic building block of the user interface.
    //
    // Mouse focus is passed down the container chain.
    // 
    // Keyboard focus can only be changed from code, like a widget
    // grabbing focus when clicked.
    //
    // All widgets support children, but you shouldn't abuse this.
    // Many things are easier if container functionality is exposed
    // in Widget, but most of the time the container functionality is
    // for the widget to compose itself.
    //
    // Please see documentation on IWidget as well.
    public abstract class Widget: IWidget
    {
        // Simple class, compares Widgets depending on ZOrder
        private class WidgetZOrderComparer: IComparer
        {
            public int Compare(object a, object b)
            {
                IWidget wa = a as IWidget;
                IWidget wb = b as IWidget;
                if (a == null || b == null) {
                    throw new ArgumentException();
                }
                return wa.ZOrder - wb.ZOrder;
            }
        }

        // Get the widget's draw flags.
        public WidgetDrawFlags GetDrawFlags()
        {
            return (Pressed ? WidgetDrawFlags.Pressed : 0) |
                    (MouseFocus ? WidgetDrawFlags.MouseFocus : 0) |
                    (KeyFocus ? WidgetDrawFlags.KeyFocus : 0) |
                    (Disabled ? WidgetDrawFlags.Disabled : 0);
        }

        // If the widget has mouse focus.
        private bool mouseFocus;
        public bool MouseFocus {
            get {
                return mouseFocus;
            }
        }

        // If the widget has key focus.
        private bool keyFocus;
        public bool KeyFocus {
            get {
                return keyFocus;
            }
        }

        // If the mouse button is pressed over the widget.
        private bool pressed;
        public bool Pressed {
            get {
                return pressed;
            }
        }

        // If the widget is hidden from view/ mouse focus/ everything
        private bool hidden = false;
        public bool Hidden {
            get {
                return hidden;
            }
            set {
                hidden = value;
            }
        }

        private bool disabled = false;
        public bool Disabled {
            get {
                return disabled;
            }
            set {
                disabled = value;
            }
        }

        // Public accessor for ZOrder. Changing this causes the widget to
        // remove/add itself in the container.
        // ZOrder. Widgets are drawing in increasing values of this property.
        private int zorder;
        public int ZOrder {
            get {
                return zorder;
            }
            set {
                if (Parent != null) {
                    IWidget p = Parent;
                    p.Remove(this);
                    this.zorder = value;
                    p.Add(this);
                } else {
                    this.zorder = value;
                }
            }
        }

        // The parent of the widget. Can be null.
        private IWidget parent;
        public IWidget Parent {
            get {
                return parent;
            }
            set {
                if (parent != null) {
                    parent.Remove(this);
                }
                if (value != null) {
                    value.Add(this);
                }
                parent = value;
            }
        }

        // Transform a Point from widget coords to child coords.
        // All children have the same coordinate space.
        //
        // NOTE: When overriding you should always transform base.Transform(point).
        protected virtual PointD Transform(PointD point)
        {
            return point - GetInnerBounds().Corner1;
        }

        // Transform a rectangle (by transforming each point in turn)
        // This means that only Translations and scalings are supported.
        // Ooops.
        protected RectangleD Transform(RectangleD rect)
        {
            return new RectangleD(Transform(rect.Corner1), Transform(rect.Corner2));
        }

        // Do the transformation in reverse on the OpenGL matrix.
        // NOTE: You should always call base.GLTransform FIRST
        //
        // Don't worry about push/pop.
        protected virtual void GLTransform()
        {
            GL.Translate(GetInnerBounds().Corner1.X, GetInnerBounds().Corner1.Y, 0);
        }

        // Get the inner bounds of the widget.
        // Every widget is a container, and the contained items are inside
        // these bounds.
        // Their coords are relative to these bounds as well.
        //
        // Default implementation returns Bounds. When overriding
        // you should return some smaller bounds.
        protected virtual RectangleD GetInnerBounds()
        {
            return new RectangleD(0, 0, Width, Height);
        }

        // Distance from the top edge of the widget to the top edge of the container.
        private double posX;
        public double PosX {
            get {
                return posX;
            }
            set {
                SetSize(value, PosY, Width, Height);
            }
        }
        
        // Distance from the bottom edge of the widget to the top edge of the container. 
        private double posY;
        public double PosY {
            get {
                return posY;
            }
            set {
                SetSize(PosX, value, Width, Height);
            }
        }

        // Distance from the left edge of the widget to the left edge of the container.
        private double width;
        public double Width {
            get {
                return width;
            }
            set {
                SetSize(PosX, PosY, value, Height);
            }
        }

        // Distance from the right edge of the widget to the left edge of the container.
        private double height;
        public double Height {
            get {
                return height;
            }
            set {
                SetSize(PosX, PosY, width, value);
            }
        }

        // The bounds of the widgets as a RectangleF
        public RectangleD Bounds {
            get {
                return RectangleD.FromXYWidthHeight(PosX, PosY, Width, Height);
            }
            set {
                value.Normalize();
                SetSize(value.X1, value.Y1, value.Width, value.Height);
            }
        }

        // This is called when a recalculation of child sizes is required.
        // It will ask the parent for a size recalculation.
        //
        // This first calls Parent.RecalculateSizes and, if that doesn't
        // cause the widget to CalculateChildSizes (if the parent doesn't resize it),
        // it will call CalculateChildSizes. This prevents useless CalculateChildSizes.
        //
        // Since there's no reason for SetSize to call RecalculateChildSizes
        // (the default implementation calls CalculateSizes) there's no
        // posibility of an infinite loop.
        public void RecalculateChildSizes()
        {
            sizecalc = false;
            if (Parent != null) {
                Parent.RecalculateChildSizes();
            }
            if (!sizecalc) {
                CalculateChildSizes();
            }
        }

        // CalculateChildSizes sets this to true.
        // Used by RecalculateChildSizes to avoid useless recalculations.
        private bool sizecalc;

        // Override this to calculate your children sizes, and don't forget
        // to call the base implementation.
        // Call from SetSize by default.
        //
        // DO NOT do this by overriding SetSize.
        // It might work, but it might not.
        // DO NOT do call Parent.RecalculateSizes here, there's no logical
        // reason for you to do that.
        //
        // Size recalculation is rather complex, read the comments on
        // RecalculateSizes to see how it's done.
        protected virtual void CalculateChildSizes()
        {
            sizecalc = true;
        }

        // When the widget is resized.
        public event ResizeEventHandler Resize;

        // When the widget gains key focus. 
        public event EventHandler GainKeyFocus;

        // When a key is pressed and the widget has focus. 
        public event KeyEventHandler KeyDown;
        
        // When a key is released and the widget has focus. 
        public event KeyEventHandler KeyUp;

        // When a character is typed and the widget has focus. This is different from a
        // KeyDown event, as it's from characters, not keys. It has full translation for
        // stuff like Caps-Lock and Shift.
        public event CharEventHandler CharType;

        // When the widget loses key focus. 
        public event EventHandler LoseKeyFocus;

        // When the mouse enters the widget. 
        public event EventHandler GainMouseFocus;

        // When the mouse does something over the widget. 
        public event MouseEventHandler MouseEvent;
        
        // When the mouse exits the widget. 
        public event EventHandler LoseMouseFocus;

        // Constructor.
        public Widget()
        {
            children = new ArrayList();
        }

        // When the widget gains key focus.
        public virtual void OnGainKeyFocus()
        {
            if (!KeyFocus) {
                keyFocus = true;
                if (GainKeyFocus != null) {
                    GainKeyFocus(this, new EventArgs());
                }
            }
        }

        // When a key is pressed down.
        public virtual void OnKeyDown(KeyCode key)
        {
            if (KeyDown != null) {
                KeyDown(this, new KeyEventArgs(key));
            }
        }

        // When a key is released.
        public virtual void OnKeyUp(KeyCode key)
        {
            if (KeyUp != null) {
                KeyUp(this, new KeyEventArgs(key));
            }
        }

        // When a character is typed. This is very different from a keycode,
        // as it already includes translation for shift, CAPSLOCK and the like.
        public virtual void OnCharType(char c)
        {
            if (CharType != null) {
                CharType(this, new CharEventArgs(c));
            }
        }

        // When the widget loses key focus.
        public virtual void OnLoseKeyFocus()
        {
            if (KeyFocus) {
                keyFocus = false;
                if (LoseKeyFocus != null) {
                    LoseKeyFocus(this, new EventArgs());
                }
            }
        }

        // When the widget gains mouse focus.
        public virtual void OnGainMouseFocus()
        {
            mouseFocus = true;
            if (GainMouseFocus != null) {
                GainMouseFocus(this, new EventArgs());
            }
        }

        // Internal function to update the Focused widget on any mouse action.
        // This will set MouseFocusWidget to the highest widget(by ZOrder)
        // that contains x, y.
        private void UpdateMouseFocus(PointD loc)
        {
            Widget w;
            int i;
            for (i = children.Count - 1; i >= 0; --i) {
                w = (Widget)children[i];
                // Ignore hidden Widgets.
                if (w.Hidden) {
                    continue;
                }
                if (w.PosX <= loc.X && loc.X <= w.PosX + w.Width && w.PosY <= loc.Y && loc.Y <= w.PosY + w.Height) {
                    /* we found the Widget with focus. */
                    if (w != MouseFocusWidget) {
                        if (MouseFocusWidget != null) {
                            MouseFocusWidget.OnLoseMouseFocus();
                        }
                        w.OnGainMouseFocus();
                        mouseFocusWidget = w;
                    }
                    return;
                }
            }
            /* no mouse focus. */
            if (MouseFocusWidget != null) {
                MouseFocusWidget.OnLoseMouseFocus();
                mouseFocusWidget = null;
            }
        }

        // When a mouse button is pressed.
        public virtual void OnMouseEvent(MouseEventArgs e)
        {
            PointD loc = Transform(e.Location);
            UpdateMouseFocus(loc);
            if (MouseFocusWidget != null) {
                PointD childLoc = loc - MouseFocusWidget.Bounds.Corner1;
                MouseFocusWidget.OnMouseEvent(e.Relocate(childLoc));
            }
            if (e.EventType == MouseEventType.ButtonUp && Pressed) {
                pressed = false;
            }
            if (e.EventType == MouseEventType.ButtonDown) {
                pressed = true;
            }
            if (MouseEvent != null) {
                MouseEvent(this, e);
            }
        }

        // When the widget loses mouse focus.
        public virtual void OnLoseMouseFocus()
        { 
            if (MouseFocusWidget != null) {
                MouseFocusWidget.OnLoseMouseFocus();
                mouseFocusWidget = null;
            }
            mouseFocus = false;
            pressed = false;
            if (LoseMouseFocus != null) {
                LoseMouseFocus(this, new EventArgs());
            }
        }

        // This is the actual function called by the PosX/PosY/Width/Height accessors,
        // and is supposed to resize the widget. It's not really an event, except SdlWindow
        // receives it from the window interface.
        public virtual void SetSize(double posX, double posY, double width, double height)
        {
            this.posX = posX;
            this.posY = posY;
            this.width = width;
            this.height = height;
            CalculateChildSizes();
            if (Resize != null) {
                Resize(this, new ResizeEventArgs(PosX, PosY, Width, Height));
            }
        }

        // Draw a bunch of stuff to debug clipping calculations.
        protected void DebugClipping(RectangleD ClipRect)
        {
            //Console.WriteLine("Debug Clipping for {0}", this);

            GL.Disable(Feature.Texture);

            // Outer ClipRect, in red.
            GL.Color(SD.Color.Red);
            GL.Begin(DrawMode.LineLoop);
            GL.Vertex(ClipRect.X1, ClipRect.Y1);
            GL.Vertex(ClipRect.X2, ClipRect.Y1);
            GL.Vertex(ClipRect.X2, ClipRect.Y2);
            GL.Vertex(ClipRect.X1, ClipRect.Y2);
            GL.End();

            // Widget bounds, in green.
            GL.Color(SD.Color.Green);
            GL.Begin(DrawMode.LineLoop);
            GL.Vertex(0, 0);
            GL.Vertex(Width, 0);
            GL.Vertex(Width, Height);
            GL.Vertex(0, Height);
            GL.End();

            // Linking the two, from green to blue
            GL.Begin(DrawMode.Lines);

            GL.Color(SD.Color.Green);
            GL.Vertex(0, 0);
            GL.Color(SD.Color.Red);
            GL.Vertex(ClipRect.X1, ClipRect.Y1);
            
            GL.Color(SD.Color.Green);
            GL.Vertex(Width, 0);
            GL.Color(SD.Color.Red);
            GL.Vertex(ClipRect.X2, ClipRect.Y1);
            
            GL.Color(SD.Color.Green);
            GL.Vertex(Width, Height);
            GL.Color(SD.Color.Red);
            GL.Vertex(ClipRect.X2, ClipRect.Y2);

            GL.Color(SD.Color.Green);
            GL.Vertex(0, Height);
            GL.Color(SD.Color.Red);
            GL.Vertex(ClipRect.X1, ClipRect.Y2);
            //Console.WriteLine("Debug Clipping for {0}", this);

            GL.End();
        }

        // This will be called to draw the widget.
        //
        // ClipRect is the rectangle to clip the widget to.
        // WindowClipRect is the same rectangle, in window coords, suitable
        // for glViewport and glScissor and similar.
        // WindowClipRect is a horrible horrible horrible horrible hack.
        //
        // NOTE: You have to obbey those clip rectangles so you won't draw
        // outside the container. ClipRect is distinct from the actual widget
        // limits. Use ScissorSelf to PushScissor a rectangle that encloses
        // the current widget.
        //
        // When entering this function, opengl coords are relative to the widget.
        //
        // It's recommended that you call base.Draw first this when you override
        // Widget.Draw(), otherwise the base widget won't draw.
        public virtual void Draw(RectangleD ClipRect, RectangleD WindowClipRect)
        {
            double winXratio = WindowClipRect.Width / ClipRect.Width;
            double winYratio = WindowClipRect.Height / ClipRect.Height;

            RectangleD TransClipRect = Transform(ClipRect);
            RectangleD ChildClipRect = RectangleD.Intersection(TransClipRect, Transform(GetInnerBounds()));
            RectangleD ChildWinClipRect = new RectangleD(
                    WindowClipRect.X1 + (ChildClipRect.X1 - TransClipRect.X1) * winXratio,
                    WindowClipRect.Y1 + (ChildClipRect.Y1 - TransClipRect.Y1) * winYratio,
                    WindowClipRect.X1 + (ChildClipRect.X2 - TransClipRect.X1) * winXratio,
                    WindowClipRect.Y1 + (ChildClipRect.Y2 - TransClipRect.Y1) * winYratio);
            //Console.WriteLine("{0}: {1} trans {2} child {3}", this, ClipRect, TransClipRect, ChildClipRect);

            Refresh();
            DrawChildren(ChildClipRect, ChildWinClipRect);

            //DebugClipping(ClipRect);
        }

        // Function called before actual drawing.
        // Override to do all sorts of refreshing tasks.
        protected virtual void Refresh()
        {
        }

        // Draw all the children.
        // ChildClipRect and ChildWindowClipRect are the clipping parameters
        // sent to children.
        //
        // The opengl coordinate transform should already set for drawing the children.
        // This is obviousely an internal function.
        protected void DrawChildren(RectangleD ChildClipRect, RectangleD ChildWindowClipRect)
        {
            int i;
            IWidget child;
            GL.PushMatrix();
            GLTransform();
            for (i = 0; i < children.Count; ++i) {
                child = children[i] as IWidget;
                // Ignore hidden widgets.
                if (child.Hidden) {
                    continue;
                }
                GL.PushMatrix();
                GL.Translate(child.PosX, child.PosY, 0);
                child.Draw(new RectangleD(ChildClipRect.X1 - child.PosX, ChildClipRect.Y1 - child.PosY,
                        ChildClipRect.X2 - child.PosX, ChildClipRect.Y2 - child.PosY),
                        ChildWindowClipRect);
                GL.PopMatrix();
            }
            GL.PopMatrix();
        }

        // Get the widgets bound in window coordinates.
        protected RectangleD GetBoundsInWindow(RectangleD ClipRect, RectangleD WindowClipRect)
        {
            double winXratio = WindowClipRect.Width / ClipRect.Width;
            double winYratio = WindowClipRect.Height / ClipRect.Height;
            RectangleD WidgetBounds = RectangleD.FromXYWidthHeight(
                    WindowClipRect.X1 - ClipRect.X1 * winXratio,
                    WindowClipRect.Y1 - ClipRect.Y1 * winYratio,
                    Width * winXratio, Height * winYratio);

            //Console.WriteLine("ClipRect {0} WindowClipRect {1} Bounds {2} BoundsInWindow {3}",
            //        ClipRect, WindowClipRect, Bounds, WidgetBounds);
            return WidgetBounds;
        }

        // This is what you probably want to enforce clipping.
        // It pushes the intersection between your own bounds and WindowClipRect.
        //
        // The parameters are the same you send to Draw.
        protected void PushSelfScissor(RectangleD ClipRect, RectangleD WindowClipRect)
        {
            RectangleD WindowBounds = GetBoundsInWindow(ClipRect, WindowClipRect);
            GL.PushScissor(RectangleD.Intersection(WindowBounds, WindowClipRect).OuterRound().Normalize());
        }

        // Get the minimum desired width for this widget
        // By default it returns 0, override to provide a meaningfull value.
        public virtual double GetDesiredWidth()
        {
            return 0;
        }

        // Get the minimum desired width for this widget
        // By default it returns 0, override to provide a meaningfull value.
        public virtual double GetDesiredHeight()
        {
            return 0;
        }

        // List of children.
        private readonly ArrayList children;

        // Add a widget to the container.
        public void Add(IWidget widget)
        {
            if (children.Contains(widget)) {
                return;
            }
            children.Add(widget);
            children.Sort(new WidgetZOrderComparer());
            widget.Parent = this;
        }

        // Remove a widget from the container.
        public void Remove(IWidget widget)
        {
            if (!children.Contains(widget)) {
                return;
            }
            children.Remove(widget);
            children.Sort(new WidgetZOrderComparer());
            widget.Parent = null;
        }

        // Add a widget on top of all others already in the container.
        public void AddOnTop(IWidget widget)
        {
            if (children.Count == 0) {
                widget.ZOrder = 0;
            } else {
                widget.ZOrder = (children[children.Count - 1] as IWidget).ZOrder + 100;
            }
            Add(widget);
        }

        // Get the widget currently under the mouse, or null.
        protected IWidget mouseFocusWidget;
        public IWidget MouseFocusWidget {
            get {
                return mouseFocusWidget;
            }
        }

        // Get the highest parent on the container chain, which has to be a Window.
        public Window ParentWindow {
            get {
                IWidget x = Parent;
                while (x != null && !(x is Window)) {
                    x = x.Parent;
                }
                return x as Window;
            }
        }
    }

    // Just like ordinary widgets, but with style.
    public class StyledWidget: Widget
    {
        // Get the style of this particular Widget.
        // By default it will get the parents style.
        // If you want to set a style for this particular widget, you can set this.
        // Set this to null to revert to the parents style, or use SetParentStyle.
        //
        // Style can be null. In that case, you can throw an exception on drawing,
        // but the rest should mostly work.
        // Style is null before the widget is placed in a container (and maybe even after that.)
        private IStyle style;
        public IStyle Style {
            get {
                return style;
            }
            set {
                if (value == null) {
                    throw new ArgumentNullException("value");
                }
                this.style = value;
            }
        }

        // The font used for this widget. This can be ignored if you do your own
        // fancy stuff. By default this just gets Style.Font, but you can use set
        // this to change the font.
        // Set this to null to revert to the Style font.
        private IFont font;
        public IFont Font {
            get {
                if (font == null && Style != null) {
                    return Style.Font;
                } else {
                    return font;
                }
            }
            set {
                this.font = value;
            }
        }

        // Make the widget use the Style font (default).
        public void SetStyleFont()
        {
            Font = null;
        }

        // Constructor.
        // Styled widget always need a style.
        public StyledWidget(IStyle style): base()
        {
            Style = style;
        }
    }

    // Class with usefull behaviour for Decorators. You don't HAVE
    // to derive from this, but you probably will.
    //
    // It adds a child widget (with proper add/remove protocol).
    // CalculateChildren makes the Child fill GetInnerBounds().
    // GetDesiredWidth/Height asks for the space desired by the
    // Child plus the space taken by the InnerBounds
    // (widget size - inner bound size).
    public abstract class BaseDecorator: Widget, IDecorator
    {
        // Child widget.
        // A decorator can contain other widgets, but this one is special.
        private IWidget child;
        public IWidget Child {
            get {
                return child;
            }
            set {
                if (Child == value) {
                    return;
                }
                if (Child != null) {
                    Child.Parent = null;
                }
                if (value != null) {
                    AddOnTop(value);
                }
                child = value;
                RecalculateChildSizes();
            }
        }

        // Make the child fill the InnerBounds.
        protected override void CalculateChildSizes()
        {
            base.CalculateChildSizes();
            if (Child != null) {
                Child.Bounds = Transform(GetInnerBounds());
            }
        }

        // Adds the space between the widget bounds and the inner bounds.
        public override double GetDesiredWidth()
        {
            if (Child == null) {
                return Width - GetInnerBounds().Width;
            } else {
                return Child.GetDesiredWidth() + Width - GetInnerBounds().Width;
            }
        }

        // Adds the space between the widget bounds and the inner bounds.
        public override double GetDesiredHeight()
        {
            if (Child == null) {
                return Height - GetInnerBounds().Height;
            } else {
                return Child.GetDesiredHeight() + Height - GetInnerBounds().Height;
            }
        }
    }
}
