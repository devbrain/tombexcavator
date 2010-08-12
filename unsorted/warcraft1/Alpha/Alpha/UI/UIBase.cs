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
using Alpha.Util;

namespace Alpha.UI
{
    // MouseButton enumeration. It's marked as flags, since sometimes it's used as a
    // description of mouse button state.
    // TODO: scroll buttons are buttons too.
    [Flags]
    public enum MouseButton {
        None = 0,
        Left = 1,
        Middle = 2,
        Right = 4,
    }

    // Types of mouse events
    public enum MouseEventType {
        Movement,
        ButtonDown,
        ButtonUp,
        Click,
        DoubleClick,
    }

    // Event args for a MouseEvent.
    public class MouseEventArgs: EventArgs
    {
        // The kind of a mouse event this is. All mouse events share
        // the same args, for simplicity.
        private readonly MouseEventType eventType;
        public MouseEventType EventType {
            get {
                return eventType;
            }
        }

        // Location of the mouse event.
        private readonly PointD location;
        public PointD Location {
            get {
                return location;
            }
        }

        // X coordinate of the mouse event.
        public double X {
            get {
                return Location.X;
            }
        }

        // Y coordinate of the mouse event.
        public double Y {
            get {
                return Location.Y;
            }
        }

        // The state of the mouse buttons. This is a flag combination.
        private readonly MouseButton buttonState;
        public MouseButton ButtonState {
            get {
                return buttonState;
            }
        }

        // The specific button this refers to.
        // Only meaninfull in case of button up/down events.
        private readonly MouseButton button;
        public MouseButton Button {
            get {
                return button;
            }
        }
       
        // Returns a clone with a different location
        public MouseEventArgs Relocate(PointD loc)
        {
            return new MouseEventArgs(EventType, loc, ButtonState, Button);
        }

        // Constructor.
        public MouseEventArgs(MouseEventType type, PointD loc,
                MouseButton btns, MouseButton btn)
        {
            if (type == MouseEventType.Movement && btn != MouseButton.None) {
                throw new ArgumentException("btn");
            }
            eventType = type;
            location = loc;
            buttonState = btns;
            button = btn;
        }
    }

    // Event handler for a Mouse Event.
    public delegate void MouseEventHandler(object sender, MouseEventArgs args);
    
    // KeyCode enumeration.
    // TODO: Add values. Like A = 'A' and stuff. Maybe.
    // TODO: organize.
    public enum KeyCode {
        Backspace, Tab, Return, Pause, Escape, Space, Exclaim, DoubleQuote, Hash,
        Dollar, Ampersand, Quote, LeftParen, RightParen, Asterisk, Plus, Comma, Minus, Period, Slash,
        Digit0, Digit1, Digit2, Digit3, Digit4, Digit5, Digit6, Digit7, Digit8, Digit9,
        Colon, SemiColon, Less, Equals, Greater, Question, At, LeftBracket, BackSlash, RightBracket,
        Caret, Underscore, BackQuote,
        A, B, C, D, E, F, G, H, I, J, K, L, M, N, O, P, Q, R, S, T, U, V, W, X, Y, Z,
        Delete, KeyPad0, KeyPad1, KeyPad2, KeyPad3, KeyPad4, KeyPad5, KeyPad6, KeyPad7, KeyPad8, KeyPad9,
        KeyPadPeriod, KeyPadDivide, KeyPadMultiply, KeyPadMinus, KeyPadPlus, KeyPadEnter, KeyPadEquals,
        Up, Down, Right, Left, Insert, Home, End, PageUp, PageDown,
        F1, F2, F3, F4, F5, F6, F7, F8, F9, F10, F11, F12,
        NumLock, CapsLock, ScrollLock, PrintScreen, Break, Euro,
        RightShift, LeftShift, RightControl, LeftControl, RightAlt, LeftAlt, 
        Max // The maximum value for a KeyCode.
    }

    // EventArgs for a Key event.
    public class KeyEventArgs: EventArgs
    {
        public readonly KeyCode Key;

        public KeyEventArgs(KeyCode key)
        {
            Key = key;
        }
    }

    // Event handler for a Key Event.
    public delegate void KeyEventHandler(object sender, KeyEventArgs args);

    // EventArgs for a char event.
    public class CharEventArgs: EventArgs
    {
        public readonly char Char;

        public CharEventArgs(char c)
        {
            Char = c;
        }
    }

    // Event handler for a Char Event.
    public delegate void CharEventHandler(object sender, CharEventArgs args);

    // Args for a resize Event.
    public class ResizeEventArgs: EventArgs
    {
        // New X position.
        public readonly double PosX;

        // New Y position.
        public readonly double PosY;

        // New Width
        public readonly double Width;

        // New Height
        public readonly double Height;

        public ResizeEventArgs(double posX, double posY, double width, double height)
        {
            PosX = posX;
            PosY = posY;
            Width = width;
            Height = height;
        }
    }

    // Event handler for a Resize Event.
    public delegate void ResizeEventHandler(object sender, ResizeEventArgs args);
}
 
