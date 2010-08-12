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
using Alpha.Util;

namespace Alpha.Game
{
    // Missile class
    //
    // A missile is a simple project that moves to the target in a straight line.
    // When the target is reached the effect is executed and the missile is removed.
    //
    // Actor and speed information is in MissileType.
    //
    // FIXME: Split off abstract MissileBase?
    // FIXME: Trajectories?
    public class Missile: Entity
    {
        // Type information.
        public readonly MissileTypeBase Type;

        // Hit effect, executed when the target is reached.
        private ISpellEffect effect;
        public ISpellEffect Effect {
            get {
                return effect;
            }
            set {
                effect = value;
            }
        }

        // Target for the missile.
        // only MissileAction should touch the target.
        private TargetInfo target;
        public TargetInfo Target {
            get {
                return target;
            }
            set {
                if (Removed) {
                    throw new InvalidOperationException("Target already reached, can't change");
                }

                target = value;
                Locator.PathGoal = new PathGoal(Target);
            }
        }

        // Type contains speed.
        public override int GetSpeed()
        {
            return Type.GetSpeed();
        }

        // Update.
        public override void NextCycle()
        {
            base.NextCycle();

            if (Removed) {
                return;
            }

            // If target is not valid anymore then give up.
            if (ValidateTarget(Target) != null) {
                Remove();
                return;
            }

            if (Locator.PathStatus == PathStatus.Reached) {
                if (Effect != null) {
                    Effect.Execute(Target);
                }
                Remove();
            }
        }

        // Constructor.
        public Missile(GameState state, Player p, MissileTypeBase type): base(state, p)
        {
            this.Type = type;
            CreateLocator();
            Actor = type.GetActorFactory().CreateActor(this);
        }
    }

    // Abstract base class for missile types. You can cover most
    // missile use cases by deriving this class.
    public abstract class MissileTypeBase: GameDataObject
    {
        // Get the actor for the missile. Must override.
        public abstract IActorFactory GetActorFactory();

        // The speed of the missile.
        // This is the value returned by Missile.GetSpeed.
        public abstract int GetSpeed();

        // Launch a missile. This is the complete form.
        public Entity Launch(Player player, GameMap map, Point start, TargetInfo target, ISpellEffect effect)
        {
            Missile mis;

            if (player == null) {
                throw new ArgumentNullException("player");
            }
            if (map == null) {
                throw new ArgumentNullException("map");
            }
            if (target.HasEntity) {
                if (target.Entity.OffMap) {
                    throw new ArgumentException("target", "Target entity is not on a map");
                }
                if (target.Entity.Map != map) {
                    throw new ArgumentException("target", "Target entity is on another map");
                }
            }

            mis = new Missile(player.State, player, this);
            mis.PlaceOnMap(map, start.X, start.Y);
            mis.Target = target;
            mis.Effect = effect;

            return mis;
        }

        // Simpler version of launch, with fewer parameters.
        // Calls the complete form of Launch taking player, map and starting point
        // from the caster entity.
        public Entity Launch(Entity caster, TargetInfo target, ISpellEffect effect)
        {
            return Launch(caster.Player, caster.Map, caster.Pos, target, effect);
        }

        public MissileTypeBase(GameData data): base(data)
        {
        }

        public MissileTypeBase(GameState state): base(state)
        {
        }
    }

    // Concrete implementation of MissileTypeBase.
    public class MissileType: MissileTypeBase
    {
        // Produces actors for the missile.
        public IActorFactory ActorFactory;

        public override IActorFactory GetActorFactory()
        {
            return ActorFactory;
        }

        // The speed of the missile.
        public int Speed;

        public override int GetSpeed()
        {
            return Speed;
        }

        public MissileType(GameData data): base(data)
        {
        }

        public MissileType(GameState state): base(state)
        {
        }

        // Initializing constructor.
        public MissileType(GameData data, IActorFactory factory, int speed): base(data)
        {
            this.Speed = speed;
            this.ActorFactory = factory;
        }
    }
}
