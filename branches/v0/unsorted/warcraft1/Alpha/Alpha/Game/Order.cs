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

using Alpha.Util;

namespace Alpha.Game
{
    // This struct contains information on a target.
    //
    // It an be a point on a map or an entity.
    //
    // .Map .X .Y will still work for entity targets, they will
    // just return the entity's map/location.
    [Serializable]
    public struct TargetInfo
    {
        // Y coordinate of target 
        private int x;

        // X coordinate of target (only when entity is null).
        private int y;

        // This can be either a GameMap or an Entity.
        private Object obj;

        // Return true if this Target has an entity or not.
        public bool HasEntity {
            get {
                return obj is Entity;
            }
        }

        // Gets the entity target. Can be null.
        public Entity Entity {
            get {
                return obj as Entity;
            }
        }

        // Gets the map the target is on. Can be null.
        public GameMap Map {
            get {
                if (obj is Entity) {
                    return (obj as Entity).Map;
                } else {
                    return obj as GameMap;
                }
            }
        }

        // Represent the empty target. It happens to be unique.
        public static readonly TargetInfo Empty = new TargetInfo(null, 0, 0);

        // Checks if the target is empty.
        public bool IsEmpty {
            get {
                return obj == null;
            }
        }

        // Gets the x target coord. If Entity is non-null this returns Entity.UX
        public int X {
            get {
                if (obj is Entity) {
                    return (obj as Entity).X;
                } else {
                    return x;
                }
            }
        }

        // Gets the y target coord. If Entity is non-null this returns Entity.UY
        public int Y {
            get {
                if (obj is Entity) {
                    return (obj as Entity).Y;
                } else {
                    return y;
                }
            }
        }

        // Position as a point. Convenience property.
        public Point Pos {
            get {
                if (obj is Entity) {
                    return (obj as Entity).Pos;
                } else {
                    return new Point(x, y);
                }
            }
        }

        // Constructor for location target.
        public TargetInfo(GameMap map, int x, int y)
        {
            this.x = x;
            this.y = y;
            this.obj = map;
        }

        // Constructor for entity target.
        public TargetInfo(Entity entity)
        {
            if (entity == null) {
                throw new ArgumentNullException("entity");
            }
            this.x = 0;
            this.y = 0;
            this.obj = entity;
        }
        
        // Compare for equality
        // two TargetInfo are equal if they both reference the same entity
        // or they both have the same coords (and no entities). 
        public bool Equals(TargetInfo that)
        {
            if (obj is Entity) {
                return this.Entity == that.Entity;
            } else {
                return this.Map == that.Map && this.X == that.X && this.Y == that.Y;
            }
        }
        
        // Implement Object.Equals.
        public override bool Equals(object that)
        {
            return Equals((TargetInfo)that);
        }
        
        // Implement Object.GetHashCode to get rid of warnings.
        // This should work but it's not tested and might be slow for
        // location targets.
        public override int GetHashCode()
        {
            if (Entity != null) {
                return Entity.GetHashCode();
            } else {
                return new Point(X, Y).GetHashCode();
            }
        }
        
        // Override ==, using Equals.
        public static bool operator ==(TargetInfo a, TargetInfo b)
        {
            return a.Equals(b);
        }
        
        // Override !=, using Equals.
        public static bool operator !=(TargetInfo a, TargetInfo b)
        {
            return !a.Equals(b);
        }
        
        // Convert to string, used when printing.
        public override string ToString()
        {
            if (HasEntity) {
                return String.Format("Target(Entity={0}, Map={1} X={2}, Y={3})", Entity, Map, X, Y);
            } else {
                return String.Format("Target(Map={0}, X={1}, Y={2})", Map, X, Y);
            }
        }
    }

    // An order is an abstract class for representing commands given to entities.
    // This is not sent directly over the network, it's encapsulated, since
    // network commands can contain more stuff, like chat and diplomacy.
    //
    // This is an abstract class, it doesn't really do anything.
    // You probably want to send order derivates.
    [Serializable]
    public abstract class Order: ICloneable
    {
        // Validate if the order is valid for the entity.
        //
        // Returns null if invalid, or a ValidationError.
        // If this is non-null, the entity will refuse to queue or execute the order.
        //
        // Defaults to null.
        public virtual ValidationError Validate(Entity entity)
        {
            return null;
        }

        // Create the action.
        // Action will control the entity while this order is executing.
        public abstract IAction CreateAction(Entity u);

        // This clones the Order using MemberwiseClone.
        // Most likely you won't need to override.
        public virtual object Clone()
        {
            return this.MemberwiseClone();
        }
    }

    // This is an order that takes a target parameter.
    [Serializable]
    public abstract class TargetOrder: Order
    {
        // The target in question.
        private TargetInfo target;
        public TargetInfo Target {
            get {
                return target;
            }
        }

        // Checks Entity.ValidateTarget.
        public override ValidationError Validate(Entity entity)
        {
            return entity.ValidateTarget(target);
        }

        // Constructor.
        public TargetOrder(TargetInfo target)
        {
            this.target = target;
        }

        // This will create a clone of the current order and modify the target
        // on the new order.
        public TargetOrder CloneWithNewTarget(TargetInfo target)
        {
            TargetOrder newOrder = (TargetOrder)this.Clone();
            newOrder.target = target;
            return newOrder;
        }

        // Override so that printing provides usefull information.
        // It will still be usefull even if you don't override for every
        // order.
        public override string ToString()
        {
            return String.Format("{0}({1})", GetType().Name, Target);
        }
    }

    // Gives an Entity ActionIdle
    [Serializable]
    public class IdleOrder: Order
    {
        public override IAction CreateAction(Entity u)
        {
            return new ActionIdle();
        }
    }

    // Gives an Entity MoveAction
    [Serializable]
    public class MoveOrder: TargetOrder
    {
        // Validate if the entity can move.
        public override ValidationError Validate(Entity ent)
        {
            ValidationError err;
            if ((err = base.Validate(ent)) != null) {
                return err;
            }
            if (!ent.CanMove()) {
                return new ValidationError("Entity can't move.");
            }
            return null;
        }

        // Create the action.
        public override IAction CreateAction(Entity u)
        {
            return new MoveAction(Target);
        }

        // Constructor.
        public MoveOrder(TargetInfo target): base(target)
        {
        }
    }

    // Gives an Entity ActionAbility
    [Serializable]
    public class AbilityOrder: TargetOrder
    {
        // The ability to use.
        public readonly IAbility Ability;

        // Calls Ability.Validate
        public override ValidationError Validate(Entity entity)
        {
            // Does not inherit, prevents duplicate checks.
            return Ability.Validate(entity, Target);
        }

        // Create the action.
        public override IAction CreateAction(Entity entity)
        {
            return new AbilityAction(Target, Ability);
        }

        // Constructor.
        public AbilityOrder(IAbility ability, TargetInfo target): base(target)
        {
            this.Ability = ability;
        }
    }

    // ActionAbility with target set to self.
    [Serializable]
    public class SelfAbilityOrder: Order
    {
        // The ability to use.
        public readonly IAbility Ability;

        // Calls Ability.Validate
        public override ValidationError Validate(Entity entity)
        {
            return Ability.Validate(entity, new TargetInfo(entity));
        }

        // Create the action.
        public override IAction CreateAction(Entity entity)
        {
            return new AbilityAction(new TargetInfo(entity), Ability);
        }

        // Constructor.
        public SelfAbilityOrder(IAbility ability)
        {
            this.Ability = ability;
        }
    }

    // Gives an Entity ActionActivity
    [Serializable]
    public class ActivityOrder: Order
    {
        // The activity to use.
        public readonly IActivity Activity;

        public override ValidationError Validate(Entity entity)
        {
            return Activity.Validate(entity);
        }

        // Create the action.
        public override IAction CreateAction(Entity entity)
        {
            return new ActivityAction(Activity);
        }

        // Constructor.
        public ActivityOrder(IActivity activity)
        {
            this.Activity = activity;
        }
    }

    // Gives an Entity ActionMoveFindTarget
    [Serializable]
    public class HuntOrder: TargetOrder
    {
        // Validate if the entity can move.
        public override ValidationError Validate(Entity ent)
        {
            ValidationError err;
            if ((err = base.Validate(ent)) != null) {
                return err;
            }
            if (!ent.CanMove()) {
                return new ValidationError("Entity can't move");
            }
            return null;
        }

        public override IAction CreateAction(Entity ent)
        {
            return new HuntAction(Target);
        }

        // Default constructor.
        public HuntOrder(TargetInfo target): base(target)
        {
        }
    }

    // Creates an ActionGuard.
    [Serializable]
    public class GuardTargetOrder: TargetOrder
    {
        public override IAction CreateAction(Entity u)
        {
            return new GuardAction(Target);
        }

        // Constructor.
        public GuardTargetOrder(TargetInfo target): base(target)
        {
        }
    }

    // This creates an ActionGuard with Target set to the entity itself.
    // This will make the entity Hold it's position
    [Serializable]
    public class HoldPositionOrder: Order
    {
        // Create the action.
        public override IAction CreateAction(Entity entity)
        {
            return new GuardAction(new TargetInfo(entity));
        }
    }

    // A command is an abstract class to represent a command given by a player.
    // It includes any sort of command that has an effect on the GameState.
    // The command class should implement it's own serialization, for efficiency.
    //
    // FIXME: Validation.
    [Serializable]
    public abstract class Command
    {
        // Execute the command.
        public abstract void Execute(GameState state);
    }

    // This sets the random number generator seed.
    [Serializable]
    public class SetRandomSeedCommand: Command
    {
        // New GameState.SyncRandSeed.
        public readonly uint NewSeed;

        // Constructor, sets NewSeed.
        public SetRandomSeedCommand(uint seed)
        {
            NewSeed = seed;
        }

        // Execute the command, set GameState.SyncRand
        public override void Execute(GameState state)
        {
            state.SyncSeed = NewSeed;
        }
    }

    // A command given to an entity. This is a wrapper for an order.
    // This command gives an entity an order to execute.
    //
    // FIXME: send to multiple entities, to save transfer.
    // FIXME: perhaps that should be implemented as another Command.
    [Serializable]
    public class OrderCommand: Command
    {
        // The entity this order is given to.
        public readonly IList Entities;

        // The actual order.
        public readonly Order Order;

        // If this is true then this order is queued after the current order.
        public readonly bool Queue;

        // Create a new Order Command.
        public OrderCommand(Entity u, Order o, bool queue)
        {
            Entities = new Entity[] {u};
            Order = o;
            Queue = queue;
        }

        // Create a new Order Command.
        public OrderCommand(IList entities, Order o, bool queue)
        {
            Entities = entities;
            Order = o;
            Queue = queue;
        }

        // Execute the order, duh.
        public override void Execute(GameState state)
        {
            foreach (Entity entity in Entities) {
                ValidationError err;
                if ((err = Order.Validate(entity)) != null) {
                    Console.WriteLine("Invalid order, ignoring: {0}", err);
                    continue;
                }
                if (entity.OrderQueue == null) {
                    Console.WriteLine("Entity doesn't support orders");
                    continue;
                }
                if (Queue) {
                    entity.OrderQueue.QueueOrder(Order);
                } else {
                    entity.OrderQueue.SetOrder(Order);
                }
            }
        }
    }

    [Serializable]
    public class SpawnEntityCommand: Command
    {
        // Factory to create the new entity.
        public readonly IEntityFactory Factory;

        // The location to spawn the entity at, in U-coords.
        public readonly int X, Y;

        // The map to spawn the entity on.
        public readonly GameMap Map;

        // The player the new entity belongs to.
        public readonly Player Player;

        // Create the command.
        public SpawnEntityCommand(IEntityFactory factory, GameMap map, Player player, int x, int y)
        {
            if (factory == null) {
                throw new ArgumentNullException("factory");
            }
            if (map == null) {
                throw new ArgumentNullException("map");
            }
            if (player == null) {
                throw new ArgumentNullException("player");
            }
            Map = map;
            Factory = factory;
            Player = player;
            X = x;
            Y = y;
        }

        // Execute the command and spawn the entity.
        public override void Execute(GameState state)
        {
            if (Utilities.TryCreatePlaceEntity(Factory, Player, Map, X, Y, Int32.MaxValue) == null) {
                Console.WriteLine("Failed placing entity");
            }
        }
    }
}
