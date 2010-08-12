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
    // This class is an optional component of Entity which
    // implements and OrderQueue and executes actions. If this is
    // disabled then the whole Order/Action system won't be available
    // for the entity
    public class OrderQueue: IEntityComponent
    {
        // Entity this object is part of.
        private Entity entity;
        public Entity Entity {
            get {
                return entity;
            }
        }

        // Entity action, executes orders and controls the entity.
        private IAction action;
        public IAction Action {
            get {
                return action;
            }
        }

        // The default order. This will probably be some sort of idle/guard order.
        // This is the order the entity gets when the current order is finished.
        private Order defaultOrder;
        public Order DefaultOrder {
            get {
                return defaultOrder;
            }
            set {
                if (value == null) {
                    throw new ArgumentNullException("value");
                }
                ValidationError err;
                if ((err = value.Validate(Entity)) != null) {
                    throw new ValidationException("Invalid default order.", err);
                }
                defaultOrder = value;
            }
        }

        // The actual orders
        private Queue Orders;

        // Get the current order for this entity.
        private Order currentOrder;
        public Order CurrentOrder {
            get {
                return currentOrder;
            }
        }

        // Give this entity an immediate command.
        // This will flush all pending orders and execute the order as
        // soon as possible.
        public void SetOrder(Order o)
        {
            CheckAttached();

            ValidationError err;
            if ((err = o.Validate(Entity)) != null) {
                throw new ValidationException(err);
            }
            Orders.Clear();
            Action.Stop(false);
            Orders.Enqueue(o);
        }

        // Give this entity a command to do after currently pending orders.
        public void QueueOrder(Order o)
        {
            CheckAttached();

            ValidationError err;
            if ((err = o.Validate(Entity)) != null) {
                throw new ValidationException(err);
            }
            Action.Stop(false);
            Orders.Enqueue(o);
        }

        // Force changing the order right now. This kills the action
        // on the spot and starts a new one. Also clear the order queue.
        //
        // This is not for the player to use.
        //
        // FIXME: no immediate effect.
        public void ForceChangeOrder(Order o)
        {
            CheckAttached();

            ValidationError err;
            if ((err = o.Validate(Entity)) != null) {
                throw new ValidationException(err);
            }
            Action.Stop(true);
            //Console.WriteLine("{0}: entity {1} forced to change to {2}",
            //        State.Cycle, State.GetIdent(this), o);
            Orders.Clear();
            Orders.Enqueue(o);
        }

        // Change the order to something else.
        private void ChangeOrder(Order newOrder)
        {
            CheckAttached();

            //Console.WriteLine("{3}: Entity {0} changes order from {1} to {2}",
            //        Entity.State.GetIdent(Entity), currentOrder, newOrder, Entity.State.Cycle);

            // Stop old action.
            action.Detach();

            // Reset path goal and actor animation.
            Entity.Locator.PathGoal = null;
            Entity.Actor.SetAnimation(ActorAnimation.Idle);

            // Start new action.
            currentOrder = newOrder;
            action = currentOrder.CreateAction(Entity);
            action.Attach(Entity);
        }

        // Get the order queue as a list.
        // BEWARE: this makes a full copy of the list.
        public IList GetOrders()
        {
            CheckAttached();
            return Orders.ToArray();
        }

        // Update the order queue.
        // Magic :D
        public void NextCycle()
        {
            CheckAttached();
            if (Action.GetRollover()) {
                if (Orders.Count > 0) {
                    Action.Stop(true);
                    ChangeOrder(Orders.Dequeue() as Order);
                } else if (CurrentOrder != DefaultOrder) {
                    Action.Stop(true);
                    ChangeOrder(DefaultOrder);
                }
            }
            Action.NextCycle();
        }

        // Check if it's attached to an entity, and throw an Exception if it's not.
        private void CheckAttached()
        {
            if (Entity == null) {
                throw new InvalidOperationException("Not attached to an entity");
            }
        }

        // Default constructor, sets default order to IdleOrder.
        public OrderQueue()
        {
            defaultOrder = new IdleOrder();
        }

        // Constructor, initializes default order.
        public OrderQueue(Order o)
        {
            defaultOrder = o;
        }

        // Attach to an entity, start controlling.
        public void Attach(Entity entity)
        {
            if (entity == null) {
                throw new InvalidOperationException("entity");
            }
            this.entity = entity;
            this.currentOrder = this.defaultOrder;

            Orders = new Queue();

            // Create and attach the action.
            action = CurrentOrder.CreateAction(Entity);
            Action.Attach(Entity);
        }

        // Detach from the entity.
        public void Detach()
        {
            Action.Stop(true);
            Action.Detach();
            entity = null;
            action = null;
            Orders = null;
        }
    }
}

