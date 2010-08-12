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
using System.Collections;

using Alpha.Util;

namespace Alpha.Game
{
    // Trivial interface for a target finder.
    // Little more than a gloryfied delegate. There is a single method,
    // it receives the current ability/target and it can modify it.
    //
    // Use abil = null and target = TargetInfo.Empty to give up.
    public interface ITargetFinder
    {
        void FindTarget(Entity entity, ref IAbility abil, ref TargetInfo target);
    }

    // Base class for finding an entity target.
    // Override this to implement target finders which search for unit targets.
    public abstract class TargetFinderBase: GameDataObject, ITargetFinder
    {
        // Get the acquire radius for this entity. New targets will only be aquired in that radius.
        // KeepRadius can be larger however.
        public abstract int GetAcquireRadius(Entity entity);

        // Get the keep radius for this entity. Entity will give up on targets outside this radius.
        public abstract int GetKeepRadius(Entity entity);

        // Evaluate an unit target. Returns the ability and the score for the specified target.
        // Return false to reject the target.
        public abstract bool EvaluateTarget(Entity caster, Entity target, out IAbility abil, out int score);

        // Find the target. Calls EvaluateTarget for all entities in GetSearchRadius().
        public void FindTarget(Entity entity, ref IAbility ability, ref TargetInfo tinfo)
        {
            // Entities to evaluate.
            int bestScore, newScore;
            IAbility newAbility;
            IList entities;

            int rad = GetAcquireRadius(entity);
            entities = entity.Map.Cache.SelectList(entity.X - rad, entity.Y - rad, entity.X + rad, entity.Y + rad);

            bestScore = 0;
            if (IntMath.Distance(entity.Pos, tinfo.Pos) <= GetKeepRadius(entity)) {
                if (tinfo.HasEntity) {
                    if (EvaluateTarget(entity, tinfo.Entity, out newAbility, out newScore)) {
                        if (newAbility == null) {
                            throw new InvalidOperationException("EvaluateTarget returned null ability");
                        }
                        ability = newAbility;
                        bestScore = newScore;
                    } else {
                        ability = null;
                        tinfo = TargetInfo.Empty;
                    }
                }
            } else {
                ability = null;
                tinfo = TargetInfo.Empty;
            }

            foreach (Entity target in entities) {
                if (EvaluateTarget(entity, target, out newAbility, out newScore)) {
                    if (newScore > bestScore) {
                        if (newAbility == null) {
                            throw new InvalidOperationException("EvaluateTarget returned null ability");
                        }
                        bestScore = newScore;
                        ability = newAbility;
                        tinfo = new TargetInfo(target);
                    }
                }
            }
        }

        public TargetFinderBase(GameData data): base(data)
        {
        }

        public TargetFinderBase(GameState state): base(state)
        {
        }
    }

    // Trivial target finder, generally used for attack abilities.
    public class OffensiveTargetFinder: TargetFinderBase
    {
        // Constant AcquireRadius.
        private int acquireRadius;
        public int AcquireRadius {
            get {
                return acquireRadius;
            }
            set {
                acquireRadius = value;
            }
        }

        // Returns AcquireRadius.
        public override int GetAcquireRadius(Entity entity)
        {
            return acquireRadius;
        }

        // Constant KeepRadius.
        private int keepRadius;
        public int KeepRadius {
            get {
                return keepRadius;
            }
            set {
                keepRadius = value;
            }
        }

        // Returns KeepRadius.
        public override int GetKeepRadius(Entity entity)
        {
            return keepRadius;
        }

        // Constant ability.
        private IAbility ability;
        public IAbility Ability {
            get {
                return ability;
            }
            set {
                ability = value;
            }
        }

        // Attacks the closes target.
        public override bool EvaluateTarget(Entity entity, Entity target, out IAbility abil, out int score)
        {
            if (Ability.Validate(entity, new TargetInfo(target)) != null ||
                    entity.Player.ShouldAttack(target) == false) {
                abil = null;
                score = 0;
                return false;
            }
            int dist = IntMath.Distance(entity.X, entity.Y, target.X, target.Y);
            abil = Ability;
            score = 2 * GetAcquireRadius(entity) - (dist - Ability.GetRange(entity));

            return true;
        }

        public OffensiveTargetFinder(GameState state): base(state)
        {
        }

        public OffensiveTargetFinder(GameData data): base(data)
        {
        }

        // Convenience constructor.
        public OffensiveTargetFinder(GameData data, IAbility ability, int acquireRadius, int keepRadius): base(data)
        {
            this.Ability = ability;
            this.AcquireRadius = acquireRadius;
            this.KeepRadius = keepRadius;
        }
    }
}
