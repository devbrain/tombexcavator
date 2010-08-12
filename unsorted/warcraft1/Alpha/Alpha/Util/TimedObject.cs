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
using System.Collections;

namespace Alpha.Util
{
    // Object with internal timing.
    public abstract class TimedObject
    {
        // Time at last update. Defaults to 0.
        // This can only be changed with SetTime
        // It's recommneded that you always call SetTime to initialize the timer.
        private int lastUpdate;
        public int LastUpdate {
            get {
                return lastUpdate;
            }
        }

        // How many time entities to wait until next update.
        private int wait;
        public int Wait {
            get {
                return wait;
            }
            set {
                if (value < 0) {
                    throw new ArgumentException("Wait must be a positive number");
                }
                wait = value;
            }
        }

        // This is called whenever the time changes.
        // It's either call this from the outside to reset the timer,
        // or from inside at the start of a new frame.
        //
        // This also sets Wait to 0.
        //
        // Override OnSetTime to do something when the time is set.
        public void SetTime(int time)
        {
            lastUpdate = time;
            Wait = 0;
            OnSetTime();
        }

        // Override this to do something when the time is set.
        protected virtual void OnSetTime()
        {
        }

        // Update the object. Time is the current time.
        // Update is only done if time >= LastUpdate + Wait.
        // You can use SetTime to initialize the time
        //
        // Override DoUpdate for custom update code.
        public void Update(int time)
        {
            if (time < LastUpdate + Wait) {
                return;
            }

            SetTime(time);
            OnUpdate();
        }

        // Override this in children with code to update the object.
        // NOTE: You should set Wait at the end.
        //
        // Timer stuff is handled in Update.
        //
        // If you don't care about all this waiting stuff just ignore everything
        // and DoUpdate will be called every time the timer advances.
        protected abstract void OnUpdate();
    }
}
