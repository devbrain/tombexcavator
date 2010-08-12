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
using Tao.Sdl;
using Alpha.Util;

namespace Alpha.UI
{
    // This is a SDL( www.libsdl.org ) implementation of a window.
    public class SdlWindow: Window
    {
        // If an SDL window is currently opened. SDL doesn't support
        // more that one window at the same time. It's useless anyway.
        private static bool WindowOpen = false;
       
        // Open a width * height window, initializing sdl.
        private static void OpenVideo()
        {
            if (WindowOpen) {
                throw new Exception("Sdl doesn't support multiple windows for the same program.");
            } else {
                WindowOpen = true;
            }

            if (Sdl.SDL_Init(Sdl.SDL_INIT_VIDEO) < 0) {
                throw new Exception("Video initialization failed");
            }
            
            Sdl.SDL_EnableUNICODE(1);

            Sdl.SDL_EnableKeyRepeat(Sdl.SDL_DEFAULT_REPEAT_DELAY, Sdl.SDL_DEFAULT_REPEAT_INTERVAL);

            Sdl.SDL_GL_SetAttribute(Sdl.SDL_GL_DOUBLEBUFFER, 1);
            
        }

        // Close the sdl window.
        private static void CloseVideo()
        {
            Sdl.SDL_Quit();
            WindowOpen = false;
        }

        // Fullscreen mode.
        private bool fullscreen;
        public bool Fullscreen {
            get {
                return fullscreen;
            }
            set {
                SetVideoMode((int)Width, (int)Height, value, Resizable);
            }
        }

        private bool resizable;
        public bool Resizable {
            get {
                return resizable;
            }
            set {
                SetVideoMode((int)Width, (int)Height, Fullscreen, value);
            }
        }

        // Constructor, create with a set width and height.
        // Not fullscreen, not resizable
        public SdlWindow(int width, int height)
        {
            OpenVideo();
            SetVideoMode(width, height, false, false);
        }

        // Constructor, create with a set width and height.
        // Not resizable
        public SdlWindow(int width, int height, bool fullscreen)
        {
            OpenVideo();
            SetVideoMode(width, height, fullscreen, false);
        }

        // Constructor, create with a set width and height.
        // Can set all paramenters.
        public SdlWindow(int width, int height, bool fullscreen, bool resizable)
        {
            OpenVideo();
            SetVideoMode(width, height, fullscreen, resizable);
        }

        // Quit sdl.
        public override void Dispose()
        {
            base.Dispose();
            CloseVideo();
        }

        // Show the system cursor.
        protected override void ShowSystemCursor()
        {
            Sdl.SDL_ShowCursor(1);
        }

        // Hide the system cursor.
        protected override void HideSystemCursor()
        {
            Sdl.SDL_ShowCursor(0);
        }

        private string title = "SDL Window";
        public override string Title {
            get {
                return title;
            }
            set {
                if (value == null) {
                    throw new ArgumentNullException("value");
                }
                title = value;
                Sdl.SDL_WM_SetCaption(title, null);
            }
        }

        // Set video mode.
        // Throws an exception if it fails.
        public void SetVideoMode(int width, int height, bool fullscreen, bool resizable)
        {
            int flags;

            this.fullscreen = fullscreen;
            this.resizable = resizable;
            flags = Sdl.SDL_OPENGL;
            if (resizable) {
                flags |= Sdl.SDL_RESIZABLE;
            }
            if (fullscreen) {
                flags |= Sdl.SDL_FULLSCREEN;
            }

            if (Sdl.SDL_SetVideoMode(width, height, 0, flags) == IntPtr.Zero) {
                throw new Exception(Sdl.SDL_GetError());
            }
            Sdl.SDL_WM_SetCaption(Title, null);

            base.SetSize(0, 0, width, height);

            // Changing size can screw up focus.
            int state = Sdl.SDL_GetAppState();

            if ((state & Sdl.SDL_APPMOUSEFOCUS) != 0) {
                OnGainMouseFocus();

                int x, y;
                MouseButton btns = DecodeSdlButtonState(Sdl.SDL_GetMouseState(out x, out y));
                OnMouseEvent(new MouseEventArgs(MouseEventType.Movement, new PointD(x, y),
                            btns, MouseButton.None));
            } else {
                OnLoseMouseFocus();
            }
        }

        // Move the window to posX, posY and resize to width*height.
        // Window moving is not supported with SDL, it just has no effect.
        // It's remarcably pointless anyway.
        public override void SetSize(double posX, double posY, double width, double height)
        {
            SetVideoMode((int)width, (int)height, Fullscreen, Resizable);
        }

        // Decode a SDL button state flags combination, from either
        // SDL_MouseMotionEvent or SDL_GetMouseState.
        private MouseButton DecodeSdlButtonState(int sdl_btn_state)
        {
            MouseButton btn;
            btn = MouseButton.None;
            if ((sdl_btn_state & Sdl.SDL_BUTTON_LMASK) != 0) {
                btn |= MouseButton.Left;
            }
            if ((sdl_btn_state & Sdl.SDL_BUTTON_MMASK) != 0) {
                btn |= MouseButton.Middle;
            }
            if ((sdl_btn_state & Sdl.SDL_BUTTON_RMASK) != 0) {
                btn |= MouseButton.Right;
            }
            return btn;
        }

        // A hashtable of sdl key sims -> KeyCode
        private static KeyCode[] KeyTable;
        static SdlWindow()
        {
            KeyTable = new KeyCode[(int)Sdl.SDLK_LAST];
            
            KeyTable[(int)Sdl.SDLK_BACKSPACE] = KeyCode.Backspace;
            KeyTable[(int)Sdl.SDLK_TAB] = KeyCode.Tab;
            KeyTable[(int)Sdl.SDLK_RETURN] = KeyCode.Return;
            KeyTable[(int)Sdl.SDLK_PAUSE] = KeyCode.Pause;
            KeyTable[(int)Sdl.SDLK_ESCAPE] = KeyCode.Escape;
            KeyTable[(int)Sdl.SDLK_SPACE] = KeyCode.Space;
            KeyTable[(int)Sdl.SDLK_EXCLAIM] = KeyCode.Exclaim;
            KeyTable[(int)Sdl.SDLK_QUOTEDBL] = KeyCode.DoubleQuote;
            KeyTable[(int)Sdl.SDLK_HASH] = KeyCode.Hash;
            KeyTable[(int)Sdl.SDLK_DOLLAR] = KeyCode.Dollar;
            KeyTable[(int)Sdl.SDLK_AMPERSAND] = KeyCode.Ampersand;
            KeyTable[(int)Sdl.SDLK_QUOTE] = KeyCode.Quote;
            KeyTable[(int)Sdl.SDLK_LEFTPAREN] = KeyCode.LeftParen;
            KeyTable[(int)Sdl.SDLK_RIGHTPAREN] = KeyCode.RightParen;
            KeyTable[(int)Sdl.SDLK_ASTERISK] = KeyCode.Asterisk;
            KeyTable[(int)Sdl.SDLK_PLUS] = KeyCode.Plus;
            KeyTable[(int)Sdl.SDLK_COMMA] = KeyCode.Comma;
            KeyTable[(int)Sdl.SDLK_MINUS] = KeyCode.Minus;
            KeyTable[(int)Sdl.SDLK_PERIOD] = KeyCode.Period;
            KeyTable[(int)Sdl.SDLK_SLASH] = KeyCode.Slash;
            KeyTable[(int)Sdl.SDLK_0] = KeyCode.Digit0;
            KeyTable[(int)Sdl.SDLK_1] = KeyCode.Digit1;
            KeyTable[(int)Sdl.SDLK_2] = KeyCode.Digit2;
            KeyTable[(int)Sdl.SDLK_3] = KeyCode.Digit3;
            KeyTable[(int)Sdl.SDLK_4] = KeyCode.Digit4;
            KeyTable[(int)Sdl.SDLK_5] = KeyCode.Digit5;
            KeyTable[(int)Sdl.SDLK_6] = KeyCode.Digit6;
            KeyTable[(int)Sdl.SDLK_7] = KeyCode.Digit7;
            KeyTable[(int)Sdl.SDLK_8] = KeyCode.Digit8;
            KeyTable[(int)Sdl.SDLK_9] = KeyCode.Digit9;
            KeyTable[(int)Sdl.SDLK_COLON] = KeyCode.Colon;
            KeyTable[(int)Sdl.SDLK_SEMICOLON] = KeyCode.SemiColon;
            KeyTable[(int)Sdl.SDLK_LESS] = KeyCode.Less;
            KeyTable[(int)Sdl.SDLK_EQUALS] = KeyCode.Equals;
            KeyTable[(int)Sdl.SDLK_GREATER] = KeyCode.Greater;
            KeyTable[(int)Sdl.SDLK_QUESTION] = KeyCode.Question;
            KeyTable[(int)Sdl.SDLK_AT] = KeyCode.At;
            KeyTable[(int)Sdl.SDLK_LEFTBRACKET] = KeyCode.LeftBracket;
            KeyTable[(int)Sdl.SDLK_BACKSLASH] = KeyCode.BackSlash;
            KeyTable[(int)Sdl.SDLK_RIGHTBRACKET] = KeyCode.RightBracket;
            KeyTable[(int)Sdl.SDLK_CARET] = KeyCode.Caret;
            KeyTable[(int)Sdl.SDLK_UNDERSCORE] = KeyCode.Underscore;
            KeyTable[(int)Sdl.SDLK_BACKQUOTE] = KeyCode.BackQuote;
            KeyTable[(int)Sdl.SDLK_a] = KeyCode.A;
            KeyTable[(int)Sdl.SDLK_b] = KeyCode.B;
            KeyTable[(int)Sdl.SDLK_c] = KeyCode.C;
            KeyTable[(int)Sdl.SDLK_d] = KeyCode.D;
            KeyTable[(int)Sdl.SDLK_e] = KeyCode.E;
            KeyTable[(int)Sdl.SDLK_f] = KeyCode.F;
            KeyTable[(int)Sdl.SDLK_g] = KeyCode.G;
            KeyTable[(int)Sdl.SDLK_h] = KeyCode.H;
            KeyTable[(int)Sdl.SDLK_i] = KeyCode.I;
            KeyTable[(int)Sdl.SDLK_j] = KeyCode.J;
            KeyTable[(int)Sdl.SDLK_k] = KeyCode.K;
            KeyTable[(int)Sdl.SDLK_l] = KeyCode.L;
            KeyTable[(int)Sdl.SDLK_m] = KeyCode.M;
            KeyTable[(int)Sdl.SDLK_n] = KeyCode.N;
            KeyTable[(int)Sdl.SDLK_o] = KeyCode.O;
            KeyTable[(int)Sdl.SDLK_p] = KeyCode.P;
            KeyTable[(int)Sdl.SDLK_q] = KeyCode.Q;
            KeyTable[(int)Sdl.SDLK_r] = KeyCode.R;
            KeyTable[(int)Sdl.SDLK_s] = KeyCode.S;
            KeyTable[(int)Sdl.SDLK_t] = KeyCode.T;
            KeyTable[(int)Sdl.SDLK_u] = KeyCode.U;
            KeyTable[(int)Sdl.SDLK_v] = KeyCode.V;
            KeyTable[(int)Sdl.SDLK_w] = KeyCode.W;
            KeyTable[(int)Sdl.SDLK_x] = KeyCode.X;
            KeyTable[(int)Sdl.SDLK_y] = KeyCode.Y;
            KeyTable[(int)Sdl.SDLK_z] = KeyCode.Z;
            KeyTable[(int)Sdl.SDLK_DELETE] = KeyCode.Delete;
            KeyTable[(int)Sdl.SDLK_KP0] = KeyCode.KeyPad0;
            KeyTable[(int)Sdl.SDLK_KP1] = KeyCode.KeyPad1;
            KeyTable[(int)Sdl.SDLK_KP2] = KeyCode.KeyPad2;
            KeyTable[(int)Sdl.SDLK_KP3] = KeyCode.KeyPad3;
            KeyTable[(int)Sdl.SDLK_KP4] = KeyCode.KeyPad4;
            KeyTable[(int)Sdl.SDLK_KP5] = KeyCode.KeyPad5;
            KeyTable[(int)Sdl.SDLK_KP6] = KeyCode.KeyPad6;
            KeyTable[(int)Sdl.SDLK_KP7] = KeyCode.KeyPad7;
            KeyTable[(int)Sdl.SDLK_KP8] = KeyCode.KeyPad8;
            KeyTable[(int)Sdl.SDLK_KP9] = KeyCode.KeyPad9;
            KeyTable[(int)Sdl.SDLK_KP_PERIOD] = KeyCode.KeyPadPeriod;
            KeyTable[(int)Sdl.SDLK_KP_DIVIDE] = KeyCode.KeyPadDivide;
            KeyTable[(int)Sdl.SDLK_KP_MULTIPLY] = KeyCode.KeyPadMultiply;
            KeyTable[(int)Sdl.SDLK_KP_MINUS] = KeyCode.KeyPadMinus;
            KeyTable[(int)Sdl.SDLK_KP_PLUS] = KeyCode.KeyPadPlus;
            KeyTable[(int)Sdl.SDLK_KP_ENTER] = KeyCode.KeyPadEnter;
            KeyTable[(int)Sdl.SDLK_KP_EQUALS] = KeyCode.KeyPadEquals;
            KeyTable[(int)Sdl.SDLK_UP] = KeyCode.Up;
            KeyTable[(int)Sdl.SDLK_DOWN] = KeyCode.Down;
            KeyTable[(int)Sdl.SDLK_RIGHT] = KeyCode.Right;
            KeyTable[(int)Sdl.SDLK_LEFT] = KeyCode.Left;
            KeyTable[(int)Sdl.SDLK_INSERT] = KeyCode.Insert;
            KeyTable[(int)Sdl.SDLK_HOME] = KeyCode.Home;
            KeyTable[(int)Sdl.SDLK_END] = KeyCode.End;
            KeyTable[(int)Sdl.SDLK_PAGEUP] = KeyCode.PageUp;
            KeyTable[(int)Sdl.SDLK_PAGEDOWN] = KeyCode.PageDown;
            KeyTable[(int)Sdl.SDLK_F1] = KeyCode.F1;
            KeyTable[(int)Sdl.SDLK_F2] = KeyCode.F2;
            KeyTable[(int)Sdl.SDLK_F3] = KeyCode.F3;
            KeyTable[(int)Sdl.SDLK_F4] = KeyCode.F4;
            KeyTable[(int)Sdl.SDLK_F5] = KeyCode.F5;
            KeyTable[(int)Sdl.SDLK_F6] = KeyCode.F6;
            KeyTable[(int)Sdl.SDLK_F7] = KeyCode.F7;
            KeyTable[(int)Sdl.SDLK_F8] = KeyCode.F8;
            KeyTable[(int)Sdl.SDLK_F9] = KeyCode.F9;
            KeyTable[(int)Sdl.SDLK_F10] = KeyCode.F10;
            KeyTable[(int)Sdl.SDLK_F11] = KeyCode.F11;
            KeyTable[(int)Sdl.SDLK_F12] = KeyCode.F12;
            KeyTable[(int)Sdl.SDLK_NUMLOCK] = KeyCode.NumLock;
            KeyTable[(int)Sdl.SDLK_CAPSLOCK] = KeyCode.CapsLock;
            KeyTable[(int)Sdl.SDLK_SCROLLOCK] = KeyCode.ScrollLock;
            KeyTable[(int)Sdl.SDLK_RSHIFT] = KeyCode.RightShift;
            KeyTable[(int)Sdl.SDLK_LSHIFT] = KeyCode.LeftShift;
            KeyTable[(int)Sdl.SDLK_RCTRL] = KeyCode.RightControl;
            KeyTable[(int)Sdl.SDLK_LCTRL] = KeyCode.LeftControl;
            KeyTable[(int)Sdl.SDLK_RALT] = KeyCode.RightAlt;
            KeyTable[(int)Sdl.SDLK_LALT] = KeyCode.LeftAlt;
            KeyTable[(int)Sdl.SDLK_PRINT] = KeyCode.PrintScreen;
            KeyTable[(int)Sdl.SDLK_BREAK] = KeyCode.Break;
            KeyTable[(int)Sdl.SDLK_EURO] = KeyCode.Euro;
        }

        private void FireMouseMove()
        {
            // Fire a mouse movement event.
            int x, y;
            MouseButton btns = DecodeSdlButtonState(Sdl.SDL_GetMouseState(out x, out y));
            OnMouseEvent(new MouseEventArgs(MouseEventType.Movement,
                    new PointD(x, y), btns, MouseButton.None));
        }

        // This is the event loop. This is where events are thrown from.
        // It's also where opengl buffers are swapped.
        public override void ProcessEvents()
        {
            Sdl.SDL_Event sdl_event;
            MouseButton btn;

            Sdl.SDL_GL_SwapBuffers();

            while (Sdl.SDL_PollEvent(out sdl_event) != 0) {
                switch (sdl_event.type) {
                    case Sdl.SDL_VIDEORESIZE:
                        SetSize(0, 0, sdl_event.resize.w, sdl_event.resize.h);
                        break;
                    case Sdl.SDL_ACTIVEEVENT:
                        if (sdl_event.active.state == Sdl.SDL_APPMOUSEFOCUS) {
                            if (sdl_event.active.gain != 0) {
                                OnGainMouseFocus();
                            } else {
                                OnLoseMouseFocus();
                            }
                        }
                        break;
                    case Sdl.SDL_MOUSEMOTION:
                        OnMouseEvent(new MouseEventArgs(MouseEventType.Movement,
                                new PointD(sdl_event.motion.x, sdl_event.motion.y),
                                DecodeSdlButtonState(sdl_event.motion.state), MouseButton.None));
                        break;
                    case Sdl.SDL_MOUSEBUTTONDOWN:
                    case Sdl.SDL_MOUSEBUTTONUP:
                        if (sdl_event.button.button == Sdl.SDL_BUTTON_LEFT) {
                            btn = MouseButton.Left;
                        } else if(sdl_event.button.button == Sdl.SDL_BUTTON_MIDDLE) {
                            btn = MouseButton.Middle;
                        } else if(sdl_event.button.button == Sdl.SDL_BUTTON_RIGHT) {
                            btn = MouseButton.Right;
                        } else {
                            Console.WriteLine("Unkown sdl mouse button.");
                            break;
                        }
                        // OnMouseDown/OnMouseUp sends both the button pressed/released and the button state
                        // We have to query the buttons state.
                        int x, y;
                        MouseButton btns = DecodeSdlButtonState(Sdl.SDL_GetMouseState(out x, out y));
                        MouseEventType et;
                        if (sdl_event.button.state == Sdl.SDL_PRESSED) {
                            et = MouseEventType.ButtonDown;
                        } else {
                            et = MouseEventType.ButtonUp;
                        }

                        OnMouseEvent(new MouseEventArgs(et, new PointD(x, y), btns, btn));
                        break;
                    case Sdl.SDL_KEYUP:
                        OnKeyUp(KeyTable[(int)(sdl_event.key.keysym.sym)]);
                        break;
                    case Sdl.SDL_KEYDOWN:
                        OnKeyDown(KeyTable[(int)(sdl_event.key.keysym.sym)]);
                        if (sdl_event.key.keysym.unicode != 0) {
                            OnCharType((char)sdl_event.key.keysym.unicode);
                        }
                        break;
                    case Sdl.SDL_QUIT:
                        OnClose();
                        break;
                }
            }
            if (MouseFocus) {
                FireMouseMove();
            }
        }
    }
}
