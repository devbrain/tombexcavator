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
    // Single-line text edit widget.
    public class EditBox: Widget, IEditBox
    {
        // Style information.
        private IEditBoxStyle style;
        public IEditBoxStyle Style {
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

        // TextLayout object to draw the actual text.
        private readonly TextLayout Layout;

        //The text to be shown/edited in the EditBox
        public string Text {
            get {
                return Layout.Text;
            }
            set {
                bool chg = value != Layout.Text;
                Layout.Text = value;
                if (chg && TextChanged != null) {
                    TextChanged(this, new EventArgs());
                }
            }
        }

        // The position of the carret in the text.
        public int EditPos {
            get {
                return Layout.CarretPosition;
            }
            set {
                Layout.CarretPosition = value;
            }
        }

        // If the edit box is editable.
        private bool editable;
        public bool Editable {
            get {
                return editable;
            }
            set {
                editable = value;
                Layout.ShowCarret = Editable;
            }
        }

        // Set the content's horizontal alignment.
        public HorizontalTextAlignment Alignment {
            get {
                return Layout.HorizontalAlignment;
            }
            set {
                Layout.HorizontalAlignment = value;
            }
        }

        // Maximum length of the field. Unlimited if set to 0.
        public int MaxLength = 0;
        
        // Event generated when enter key is pressed
        public event EventHandler ReturnPressed;

        // Event generated when the text changes
        public event EventHandler TextChanged;
       
        // Event generated when editing is complete.
        public event EventHandler EditDone;
       
        // Create a new EditBox.
        public EditBox(IEditBoxStyle s)
        {
            Style = s;
            this.Add(Layout = new TextLayout(Style.Font));
            Editable = true;
            Layout.CarretDrawer = Style;
            Layout.ShowCarret = false;
            Layout.VerticalAlignment = VerticalTextAlignment.Center;
        }

        // Get the inner bounds, depending on style.
        protected override RectangleD GetInnerBounds()
        {
            return Style.GetEditBoxInnerBounds(new RectangleD(0, 0, Width, Height), GetDrawFlags());
        }

        // Resize the edit box.
        protected override void CalculateChildSizes()
        {
            base.CalculateChildSizes();
            Layout.SetSize(0, 0, GetInnerBounds().Width, GetInnerBounds().Height);
        }

        // Get desired width.
        public override double GetDesiredWidth()
        {
            return Bounds.Width - GetInnerBounds().Width + Layout.GetDesiredWidth();
        }

        // Get desired height.
        public override double GetDesiredHeight()
        {
           return Bounds.Height - GetInnerBounds().Height + Layout.GetDesiredHeight();
        }
       
        // Draw the edit box.
        public override void Draw(RectangleD ClipRect, RectangleD WindowClipRect)
        {
            Style.DrawEditBox(new RectangleD(0, 0, Width, Height), GetDrawFlags());
            base.Draw(ClipRect, WindowClipRect);
        }

        // Handle key presses.
        public override void OnKeyDown(KeyCode key)
        {
            if (Editable) {
                if (key == KeyCode.Left) {
                    --EditPos;
                } else if (key == KeyCode.Right) {
                    ++EditPos;
                } else if (key == KeyCode.Home) {
                    EditPos = 0;
                } else if (key == KeyCode.End) {
                    EditPos = Text.Length;
                } else if (key == KeyCode.Backspace) {
                    if (EditPos > 0) {
                        --EditPos;
                        Text = Text.Remove(EditPos, 1);
                    }
                } else if (key == KeyCode.Delete) {
                    if (EditPos < Text.Length) {
                        Text = Text.Remove(EditPos, 1);
                    }
                }
            }
            if (key == KeyCode.Return) {
                OnReturnPressed();
            }
            base.OnKeyDown(key);
        }
        
        // Handle character typing. Only visible characters allowed.
        // \n and \r are especially forbidden.
        public override void OnCharType(char c)
        {
            if (!Editable) {
                return;
            }
            if ((Text.Length < MaxLength || MaxLength == 0) && c < 127 && c > 31) {
                Text = Text.Insert(EditPos, c.ToString());
                ++EditPos;
            }
            base.OnCharType(c);
        }

        public override void OnGainKeyFocus()
        {
            if (Editable) {
                Layout.ShowCarret = true;
            }
        }

        public override void OnLoseKeyFocus()
        {
            if (Editable) {
                Layout.ShowCarret = false;
                OnEditDone();
            }
        }

        // We override this in order to set input focus
        public override void OnMouseEvent(MouseEventArgs e)
        {
            if (e.EventType == MouseEventType.ButtonDown && e.Button == MouseButton.Left) {
                ParentWindow.KeyFocusWidget = this;
                EditPos = Layout.PointToCarret(Transform(e.Location) - Layout.Bounds.Corner1);
            }
        }
        
        // We generate an event if Return is pressed.
        // Useful for chat. maybe we should empty Text after calling
        // the delegates
        protected virtual void OnReturnPressed()
        {
            if (ReturnPressed != null) {
                ReturnPressed(this, new EventArgs());
            }
            OnEditDone();
        }

        // Fire EditDone.
        protected virtual void OnEditDone()
        {
            if (EditDone != null) {
                EditDone(this, new EventArgs());
            }
        }
    }
}

