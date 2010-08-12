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
using System.Collections;

namespace Alpha.Game
{
    // An Activity is a long process. It's used to implement
    // things like training and upgrades.
    // 
    // There is no restriction on who can execute an ability,
    // but you are should place some with CanExecute.
    //
    // TODO: pausing
    // TODO: per-frame check, stalling
    public interface IActivity
    {
        // If a certain object can execute this activity.
        ValidationError Validate(Object etc);

        // How long it takes for this ability to be executed.
        // This should really really be constant.
        int GetDuration(Object obj);

        // Called when execution is starting.
        void Start(Object obj);

        // Called to stop the execution. This should never fail if
        // Start worked. It's needed for forced termination.
        void Abort(Object obj, int progress);

        // Finish execution. This is where you will actually do things
        // like spawn then trained unit, mark an upgrade as done, etc.
        void Finish(Object obj);
    }
}

