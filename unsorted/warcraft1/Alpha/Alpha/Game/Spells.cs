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
using Alpha.Util;

namespace Alpha.Game
{
    // Very simple interface for a spell effect.
    // A spell effect is something which is done to a target.
    //
    // This interface has only one method: Execute. The method
    // should fail silently instead of throwing exceptions.
    //
    // This could be a delegate but C# 1.0 has no closures and
    // the code would end up sucking. (A lot).
    public interface ISpellEffect
    {
        void Execute(TargetInfo target);
    }

    // Spell effect with multiple smaller effects.
    //
    // FIXME: delegate-style operator overloads.
    // FIXME: This class is retarded.
    public class MultiEffect: ISpellEffect
    {
        // List of effects executed by .Execute.
        public readonly ISpellEffect[] Effects;

        public void Execute(TargetInfo target)
        {
            foreach (ISpellEffect effect in Effects) {
                effect.Execute(target);
            }
        }

        // Create from a list of effects.
        public MultiEffect(params ISpellEffect[] effects)
        {
            this.Effects = effects;
        }

        // Combine two spell effects.
        // a is executed before b.
        public static MultiEffect Combine(ISpellEffect a, ISpellEffect b)
        {
            // Calculate the length of the new effects array.
            ArrayList list = new ArrayList();

            if (a is MultiEffect) {
                foreach (ISpellEffect e in ((MultiEffect)a).Effects) {
                    list.Add(e);
                }
            } else {
                list.Add(a);
            }

            if (b is MultiEffect) {
                foreach (ISpellEffect e in ((MultiEffect)a).Effects) {
                    if (!list.Contains(e)) {
                        list.Add(e);
                    }
                }
            } else {
                if (!list.Contains(b)) {
                    list.Add(b);
                }
            }

            return new MultiEffect((ISpellEffect[])list.ToArray(typeof(ISpellEffect)));
        }
    }

    // ISpellEffect using an ability.
    // You shouldn't really use this, it's better to make abilities use
    // effects than the other way around.
    //
    // This is because IAbility does a lot more than ISpellEffect.
    public class AbilityEffect: ISpellEffect
    {
        // Ability to use.
        public readonly IAbility Ability;

        // Spell caster.
        public readonly Entity Caster;

        // Straight forward, calls Ability.Execute
        public void Execute(TargetInfo target)
        {
            Ability.Execute(Caster, target);
        }

        // Constructor.
        public AbilityEffect(IAbility ability, Entity caster)
        {
            if (ability == null) {
                throw new ArgumentNullException("ability");
            }
            if (caster == null) {
                throw new ArgumentNullException("caster");
            }
            this.Ability = ability;
            this.Caster = caster;
        }
    }
}
