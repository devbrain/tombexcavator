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
using System.Drawing;

using Alpha.Util;

namespace Alpha.Game
{
    // Enumeration of standard actor animations.
    // Actors need not support all of the animations or they can support
    // more animations.
    public enum ActorAnimation {
        Idle,
        Move,
        Action,
        Birth,
        Death,
        Max
    }

    // Actor interface.
    // The actor is responsinble for unit animation.
    public interface IActor: IEntityComponent
    {
        // This returns true if the animation can be cleanly broken
        // at this point.
        bool CanBreak();

        // Gets the current animation.
        ActorAnimation CurrentAnimation { get; }

        // Check if a certain animation is valid.
        bool IsValidAnimation(ActorAnimation anim);

        void StartAnimation(ActorAnimation anim);

        void SetAnimation(ActorAnimation anim);

        // Called each cycle to update the actor.
        void NextCycle();

        // This "tag" is a label attached to an actor that serves as a hint to the drawing system.
        int DrawTag { get; }
    }

    // Simple interface that creates an actor.
    public interface IActorFactory
    {
        IActor CreateActor(Entity entity);
    }

    // An actor animation set maps ActorAnimations to integers.
    // To be when animation is based on a number of entry-points.
    public interface IActorAnimationMapper
    {
        int MapAnimation(ActorAnimation anim);
    }

    // Trivial IActorAnimationSet implementation based on an array.
    public class ArrayAnimationMapper: IActorAnimationMapper
    {
        // Backing store.
        private int[] Animations;

        // Constructor.
        public ArrayAnimationMapper()
        {
            Animations = new int[(int)ActorAnimation.Max];
        }

        // Accessor.
        public int this[ActorAnimation anim] {
            get {
                return Animations[(int)anim];
            }
            set {
                Animations[(int)anim] = value;
            }
        }

        // Implement IActorAnimationMapper
        public int MapAnimation(ActorAnimation anim)
        {
            return this[anim];
        }
    }

    // Base class for actors. Implements some useful behaviour.
    public abstract class ActorBase: EntityComponentBase, IActor
    {
        // This default implementation always returns true.
        // Override for special behaviour.
        public virtual bool CanBreak()
        {
            return true;
        }

        // If an animation is valid.
        public abstract bool IsValidAnimation(ActorAnimation anim);

        // Current animation (that which is executing RIGHT NOW).
        // The -1 ensures the first SetAnimation will to its job.
        private ActorAnimation currentAnimation = (ActorAnimation)(-1);
        public ActorAnimation CurrentAnimation {
            get {
                return currentAnimation;
            }
        }

        // Force break the animation and restart.
        // Override should call this to change CurrentAnimation.
        // This will throw an exception if Type.IsValidAnimation returns false.
        public virtual void StartAnimation(ActorAnimation anim)
        {
            if (!IsValidAnimation(anim)) {
                throw new ArgumentException("Invalid animation", "anim");
            }
            currentAnimation = anim;
        }

        // Set the actor animation.
        // If it's the same as CurrentAnimation, nothing happens.
        public void SetAnimation(ActorAnimation anim)
        {
            if (anim == ActorAnimation.Idle && CurrentAnimation == ActorAnimation.Birth) {
                return;
            }
            if (anim != CurrentAnimation) {
                StartAnimation(anim);
            }
        }

        // Calls Entity.Locator.Advance
        protected void AdvanceLocator(int dist)
        {
            if (Entity.CanMove()) {
                Entity.Locator.Advance(dist * Entity.GetSpeed());
            }
        }

        // It takes up 4 bytes for each actor. Who cares?
        // Factories should probably set this.
        private int drawTag;
        public int DrawTag {
            get {
                return drawTag;
            }
            set {
                drawTag = value;
            }
        }

        // Start idling.
        public override void Attach(Entity entity)
        {
            base.Attach(entity);
            StartAnimation(ActorAnimation.Idle);
        }
    }

    // Trivial actor type, moves at constant speed, doesn't animate, and draw
    // itself as a simple square.
    // All the animation stuff has no effect on this actor.
    public class SimpleActorType: GameDataObject, IActorFactory
    {
        // Drawing tag.
        public int DrawTag;

        // Create a new Actor.
        public IActor CreateActor(Entity entity)
        {
            SimpleActor actor;
            
            actor = new SimpleActor();
            actor.DrawTag = DrawTag;

            return actor;
        }

        public SimpleActorType(GameData data): base(data)
        {
        }

        public SimpleActorType(GameState state): base(state)
        {
        }
    }

    // Doesn't do much, I'll grant you that.
    public class SimpleActor: ActorBase
    {
        public override bool IsValidAnimation(ActorAnimation anim)
        {
            return true;
        }

        // Not much to do here.
        public override void NextCycle()
        {
            AdvanceLocator(1);
        }
    }
}

