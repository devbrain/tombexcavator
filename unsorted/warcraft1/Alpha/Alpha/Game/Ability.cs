// Copyright (C) 2005-2006 Crestez Leonard    cleonard@go.ro
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

namespace Alpha.Game
{
    // An ability is something that an entity can do. Example abilities
    // are attacking, healing, summoning, etc.
    //
    // You don't really have to use abilities, everything an ability
    // does can by done by the action.
    public interface IAbility
    {
        // Returns the animation to use for this ability.
        ActorAnimation GetAnimation(Entity caster);

        // Get the range of this ability for a certain caster.
        int GetRange(Entity caster);

        // Validates if the target if valid for this caster.
        ValidationError Validate(Entity caster, TargetInfo target);

        // If this returns true then the ability will be cast again.
        // This can be implemented by forwarding to IsValidTarget, but
        // you might want to disable repeat casting.
        //
        // Don't confuse this with multiple-casting from AbilityBase.
        bool GetRepeatCast(Entity caster, TargetInfo target);

        // Executes the ability. TargetInfo is the target.
        // You shouldn't relie on IsValidTarget being true.
        void Execute(Entity caster, TargetInfo target);
    }
}
