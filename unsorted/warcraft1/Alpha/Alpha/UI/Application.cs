// Copyright (C) 2004-2005 Crestez Leonard    cleonard@go.ro
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
using Alpha.UI;

using Tao.Sdl;
using Alpha.Graphics;

namespace Alpha.UI
{
    // The application class, containing the main loop.
    public sealed class Application
    {
        // Get the "Clock" in miliseconds at the start of the current frame.
        private static int thisClock;
        public static int ThisClock {
            get {
                return thisClock;
            }
        }

        // Get the Clock in miliseconds at the start of the last frame.
        private static int lastClock;
        public static int LastClock {
            get {
                return lastClock;
            }
        }

        private static int startClock;
        public static int StartClock {
            get {
                return startClock;
            }
        }

        // Framerate, the number of frames drwain in one second.
        private static double fps = 0;
        public static double Fps {
            get {
                return fps;
            }
        }
         
        // Event for a new frame.
        public static event EventHandler NewFrame;

        // Initialize the application.
        public static void Init()
        {
            startClock = Environment.TickCount;
        }

        // Returns the length of the last frame in seconds.
        public static double LastFrameLength {
            get {
                return (ThisClock - LastClock) / 1000.0;
            }
        }

        // If the application is running.
        private static bool running;

        // This is the function which contains the main loop for every App
        public static void Run()
        {
            const int FpsCheckCycles = 100;
            const int FpsCheckClocks = 1000;
            int CyclesSinceFpsCheck, ClocksSinceFpsCheck;
            
            CyclesSinceFpsCheck = 0;
            ClocksSinceFpsCheck = 0;
            running = true;

            // And here is the main loop
            while (running) {
                lastClock = thisClock;
                thisClock = Environment.TickCount;

                GL.CheckErrors();
                if (NewFrame != null) {
                    NewFrame(null, new EventArgs());
                }
                
                ClocksSinceFpsCheck += ThisClock - LastClock;
                CyclesSinceFpsCheck++;
                if (CyclesSinceFpsCheck > FpsCheckCycles ||
                        ClocksSinceFpsCheck > FpsCheckClocks) {
                    fps = (1000.0 * CyclesSinceFpsCheck) / ClocksSinceFpsCheck;
                    //Console.WriteLine("{0} frames in {1} miliseconds: {2} fps", CyclesSinceFpsCheck, ClocksSinceFpsCheck, fps);
                    ClocksSinceFpsCheck = 0;
                    CyclesSinceFpsCheck = 0;
                }
            }
        }
       
        // Stop the application, when back in the main loop.
        public static void Quit()
        {
            running = false;
        }
    }
}
