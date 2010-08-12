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

using Tao.OpenGl;
using Alpha.Graphics;
using Alpha.Util;

namespace Alpha.UI
{
    // The window should contain the OpenGL rendering context.
    // It also implements scaling for the children.
    // TODO: fullscreen, set resizeable
    // TODO: window should keep track of mouse position.
    public abstract class Window: BaseDecorator, IDisposable
    {
        // Scale factors for the children. Coordinates of children are multiplied by it.
        // It should generally be avoided.
        public double XScale = 1;
        public double YScale = 1;

        // Translation for the children. They are add to children coordinates.
        // Translation is done AFTER scaling.
        // Intuitively, a point (x, y) in window pixel coordinates corresponds to a point
        // (x * XScale + XTrans, y * YScale + YTrans) in widget coordinates.
        // Should be avoided.
        public double XTrans = 0;
        public double YTrans = 0;

        // Override transformation.
        protected override PointD Transform(PointD point)
        {
            point = base.Transform(point);
            return new PointD(point.X * XScale + XTrans, point.Y * YScale + YTrans);
        }

        protected override void GLTransform()
        {
            base.GLTransform();
            GL.Scale(1 / XScale, 1 / YScale, 1);
            GL.Translate(-XTrans, -YTrans, 1);
        }

        // Mouse location. Only meaningfull if the mouse is on the window.
        private PointD mouseLoc = new PointD(0, 0);
        public PointD MouseLoc {
            get {
                return mouseLoc;
            }
        }

        // Implementations should override this to process events.
        public abstract void ProcessEvents();

        //The widget which has Key Focus;
        private IWidget keyFocusWidget = null;
        public IWidget KeyFocusWidget {
            get {
                return keyFocusWidget;
            }
            set {
                if (KeyFocusWidget == value) {
                    return;
                }
                if (KeyFocusWidget != null) {
                    KeyFocusWidget.OnLoseKeyFocus();
                }
                keyFocusWidget = value;
                if (KeyFocusWidget != null) {
                    KeyFocusWidget.OnGainKeyFocus();
                }
            }
        }

        // The cursor shown in the window.
        // Can be null, if so the system cursor will be visible.
        // Can be override by FrameCursor, but only for one frame.
        private Cursor defaultCursor;
        public Cursor DefaultCursor {
            get {
                return defaultCursor;
            }
            set {
                defaultCursor = value;
            }
        }

        // The cursor to draw this frame.
        // This is always initialized to 0 at the start.
        // Whatever is here at the end of Window.Draw is drawn
        // as the cursor.
        // If null, it fall back to DefaultCursor. If that is null too,
        // the system cursor will be visible.
        //
        // Therefore, in order to change the visible cursor in a widget,
        // override MouseEvent and set ParentWindow.FrameCursor.
        // Someone else (maybe a parent) can set FrameCursor too after you
        // do, if that is the case their cursor will be shown.
        //
        // If containers want a special cursor but still want to let their
        // children override it they should set FrameCursor BEFORE calling
        // base.MouseEvent.
        private Cursor frameCursor;
        public Cursor FrameCursor {
            get {
                return frameCursor;
            }
            set {
                frameCursor = value;
            }
        }

        // Draw the cursor.
        // It draws Cursor if non-null, otherwise it will show the system cursor.
        private void DrawCursor()
        {
            if (!MouseFocus) {
                return;
            }
            if (FrameCursor != null) {
                HideSystemCursor();
                FrameCursor.Draw(MouseLoc.X, MouseLoc.Y);
            } else if (DefaultCursor != null) {
                HideSystemCursor();
                DefaultCursor.Draw(MouseLoc.X, MouseLoc.Y);
            } else {
                ShowSystemCursor();
            }
            FrameCursor = null;
        }

        // Draw the Window.
        public void Draw()
        {
            // Calculate clipping.
            RectangleD ClipRect;
            RectangleD WinClipRect;

            WinClipRect = RectangleD.FromXYWidthHeight(0, Height, Width, -Height);
            ClipRect = Transform(GetInnerBounds());

            // Setup OpenGL rendering.
            GL.Viewport(0, 0, (int)Width, (int)Height);
            GL.ClearColor(System.Drawing.Color.Black);
            GL.ClearDepth();
            GL.Enable(Feature.Blend);
            Gl.glBlendFunc(Gl.GL_SRC_ALPHA, Gl.GL_ONE_MINUS_SRC_ALPHA);

            GL.MatrixMode(MatrixMode.Projection);
            GL.LoadIdentity();
            GL.Ortho(0, Width, Height, 0, -1, 1);
            GL.MatrixMode(MatrixMode.ModelView);
            GL.LoadIdentity();

            //if (Child != null) Child.Draw(ClipRect, WinClipRect);
            DrawChildren(ClipRect, WinClipRect);

            GLTransform();
            DrawCursor();
        }

        public override void Draw(RectangleD ClipRect, RectangleD WindowClipRect)
        {
            throw new InvalidOperationException("Use the parameterless Draw for windows.");
        }

        public Window()
        {
            Application.NewFrame += new EventHandler(App_NewFrame);
        }

        public abstract string Title {
            get;
            set;
        }

        // Event called when the user tries to close the window. Can be ignored.
        public event EventHandler Close;

        // Used to trigger the Close event.
        protected void OnClose()
        {
            if (Close != null) {
                Close(this, new EventArgs());
            }
        }

        // A buffer to keep up/down states for keys.
        private bool[] KeyBuffer = new bool[(int)KeyCode.Max];

        // Returns if a certain key is down or not.
        public bool IsKeyDown(KeyCode k)
        {
            return KeyBuffer[(int)k];
        }

        // Override KeyDown to maintain the state in KeyBuffer.
        public override void OnKeyDown(KeyCode key)
        {
            KeyBuffer[(int)key] = true;
            if (KeyFocusWidget != null) {
                KeyFocusWidget.OnKeyDown(key);
            }
            base.OnKeyDown(key);
        }

        // Override KeyUp to maintain the state in KeyBuffer.
        public override void OnKeyUp(KeyCode key)
        {
            KeyBuffer[(int)key] = false;
            if (KeyFocusWidget != null) {
                KeyFocusWidget.OnKeyUp(key);
            }
            base.OnKeyUp(key);
        }

        // Override CharType to transmit to KeyFocusWidget.
        public override void OnCharType(char c)
        {
            if (KeyFocusWidget != null) {
                KeyFocusWidget.OnCharType(c);
            }
            base.OnCharType(c);
        }

        // Implementations should implement this and make it show the system cursor over the window
        // The constructor should also do that.
        protected abstract void ShowSystemCursor();

        // Implementations should implement this and make it hide the system cursor over the window
        protected abstract void HideSystemCursor();

        // NOTE: mouse movement events are fired every single frame.
        // Override mouse events to save cursor position.
        public override void OnMouseEvent(MouseEventArgs e)
        {
            mouseLoc = Transform(e.Location);
            // Widget.OnMouseEvents transforms event locations by itself.
            base.OnMouseEvent(e);
        }

        // Override this for your own clean-up routines.
        public virtual void Dispose()
        {
            Application.NewFrame -= new EventHandler(App_NewFrame);
        }

        private void App_NewFrame(object sender, EventArgs e)
        {
            ProcessEvents();
            Draw();
        }

        // Make a screenshot, returns a System.Drawing.Bitmap with the contents of the window.
        public System.Drawing.Bitmap ScreenShot()
        {
            return GL.ReadPixels(0, 0, (int)Width, (int)Height);
        }
    }
}
