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
    // Very simple Entity implementation, has no state except
    // for the creation cycle.
    public class Cloud: Entity
    {
        // Shared stuff about this cloud.
        private CloudTypeBase type;
        public CloudTypeBase Type {
            get {
                return type;
            }
            set {
                type = value;
            }
        }

        // State.Cycle when this cloud was created.
        private int creationCycle;
        public int CreationCycle {
            get {
                return creationCycle;
            }
        }

        // Get the age of the cloud.
        public int Age {
            get {
                return State.Cycle - CreationCycle;
            }
        }

        // Constructor.
        public Cloud(GameState state, Player p, CloudTypeBase type): base(state, p)
        {
            if (type == null) {
                throw new ArgumentNullException("type");
            }
            this.type = type;
            this.creationCycle = State.Cycle;
            Actor = Type.CreateActor(this);
        }

        // Update entity.
        public override void NextCycle()
        {
            base.NextCycle();

            if (!Removed) {
                Type.CloudNextCycle(this);
            }
        }
    }

    // Common information for clouds. You can use this class
    // for various visual effects or even things like storms.
    public class CloudTypeBase: GameDataObject, IEntityFactory
    {
        // Override to create an actor for the cloud.
        // null by default.
        public virtual IActor CreateActor(Cloud c)
        {
            return null;
        }

        // Override to do something every cycle for the cloud.
        // Empty by default.
        public virtual void CloudNextCycle(Cloud c)
        {
        }

        // Create cloud.
        public Entity CreateEntity(GameState state, Player p)
        {
            return new Cloud(state, p, this);
        }

        public CloudTypeBase(GameData data): base(data)
        {
        }

        public CloudTypeBase(GameState state): base(state)
        {
        }
    }

    // CloudType object useful for many common tasks.
    public class CloudType: CloudTypeBase
    {
        // If true clouds get removed on Actor.CanBreak().
        // Default to false.
        public bool RemoveOnBreak = false;

        // If >= 0 clouds get removed when they pass that age.
        // Defaults to -1, disabled.
        public int MaxAge = -1;

        // Used to create the actor. Can be null.
        public IActorFactory ActorFactory;

        // Uses ActorFactory.
        public override IActor CreateActor(Cloud cloud)
        {
            if (ActorFactory != null) {
                return ActorFactory.CreateActor(cloud);
            } else {
                return null;
            }
        }

        // Override to do something every cycle for the cloud.
        // Empty by default.
        public override void CloudNextCycle(Cloud cloud)
        {
            // RemoveOnBreak
            if (cloud.Actor != null && cloud.Actor.CanBreak() && RemoveOnBreak) {
                cloud.Remove();
                return;
            }

            // MaxAge
            if (cloud.Age >= MaxAge && MaxAge >= 0) {
                cloud.Remove();
            }
        }

        public CloudType(GameData data): base(data)
        {
        }

        public CloudType(GameState state): base(state)
        {
        }

        // Initializing constructor.
        public CloudType(GameData data, IActorFactory factory, bool rem, int maxage): base(data)
        {
            this.ActorFactory = factory;
            this.RemoveOnBreak = rem;
            this.MaxAge = maxage;
        }
    }
}
