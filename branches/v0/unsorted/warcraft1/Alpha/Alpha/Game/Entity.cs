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
using System.Collections;

using Alpha.Util;

namespace Alpha.Game
{
    // Interface for entity components.
    // Entity component are objects which can be attached to and detached
    // from an entity.
    //
    // Objects should start detaced (Entity == null).
    // NOTE: There's no need to support reattaching.
    public interface IEntityComponent
    {
        // The entity this is attached to.
        // null means detached.
        Entity Entity { get; }

        // Attach to an entity. You should generally throw an exception
        // if already attached.
        void Attach(Entity entity);

        // Detach the component. Object may becomes unusable.
        void Detach();
    }

    // Base class for entity components (Actor, OrderQueue etc.)
    public abstract class EntityComponentBase: IEntityComponent, IEntityEventListener
    {
        // The entity this is part of.
        private Entity entity;
        public Entity Entity {
            get {
                return entity;
            }
        }

        // Shortcut for Entity.State
        public GameState State {
            get {
                return Entity.State;
            }
        }

        // Shortcut for Entity.State.Data
        public GameData Data {
            get {
                return Entity.State.Data;
            }
        }

        // Check if it's attached to an entity, and throw an Exception if it's not.
        // You don't really have to call this all the time, it's only there to make
        // bugs more obvious.
        protected void CheckAttached()
        {
            if (Entity == null) {
                throw new InvalidOperationException("Not attached to an entity");
            }
        }

        // Attach object to an entity.
        public virtual void Attach(Entity entity)
        {
            if (entity == null) {
                throw new ArgumentNullException("entity");
            }
            this.entity = entity;
            if (SelfRegister) {
                Entity.RegisterListener(this);
            }
        }

        // Detach entity. Returns null.
        public virtual void Detach()
        {
            if (SelfRegister) {
                Entity.UnregisterListener(this);
            }
            entity = null;
        }

        // If true the component will register as a listener.
        // Defaults to true.
        protected virtual bool SelfRegister {
            get {
                return true;
            }
        }

        // Called by the entity every frame.
        public virtual void NextCycle()
        {
            CheckAttached();
        }

        // Entity event.
        public virtual void EntityPlacedOnMap(GameMap map, Point newpos)
        {
            CheckAttached();
        }

        // Entity event.
        public virtual void EntityMoved(Point oldpos, Point newpos)
        {
            CheckAttached();
        }

        // Entity event.
        public virtual void EntityRemovedFromMap(GameMap map, Point oldpos)
        {
            CheckAttached();
        }

        // Entity event.
        public virtual void EntityRemovedFromGame()
        {
            CheckAttached();
        }
    }

    // Trivial interface for entity creators.
    public interface IEntityFactory
    {
        // Create a new entity that belongs to a certain player.
        Entity CreateEntity(GameState state, Player player);
    }

    // An entity represent an object placed on the map. It's used for soldiers,
    // buildings, but can also be used for trees and the like.
    // This class provides entity placement and an order queue.
    public abstract class Entity: UpdatableGameObject
    {
        // Creates a new Entity.
        //
        // The entity starts outside of any map.
        // NOTE: The unit starts without an actor, locator or order queue.
        // FIXME: unit starts with a cacher however. LOL.
        public Entity(GameState state, Player player): base(state)
        { 
            if (player == null) {
                throw new ArgumentException("Entity needs a valid Player");
            }
            state.Entities.Add(this);
            Player = player;

            // Build sub-componenets.
            cacher = new EntityCacher();
            cacher.Attach(this);
        }

        // Entity actor, handles animation and drawing.
        // NOTE: This can be null.
        // FIXME: public set accessor?
        private IActor actor;
        public IActor Actor {
            get {
                return actor;
            }
            set {
                if (Actor != value) {
                    GameMap oldmap = Map;
                    if (oldmap != null) {
                        RemoveFromMap();
                    }

                    if (Actor != null) {
                        Actor.Detach();
                    }
                    actor = value;
                    if (Actor != null) {
                        Actor.Attach(this);
                    }

                    if (oldmap != null) {
                        PlaceOnMap(oldmap, X, Y);
                    }
                }
            }
        }

        // Order queue, implements order/action system.
        // NOTE: This can be null.
        // FIXME: public set accessor?
        private OrderQueue orderQueue;
        public OrderQueue OrderQueue {
            get {
                return orderQueue;
            }
            set {
                if (OrderQueue != value) {
                    GameMap oldmap = Map;
                    if (oldmap != null) {
                        RemoveFromMap();
                    }

                    if (OrderQueue != null) {
                        OrderQueue.Detach();
                    }
                    orderQueue = value;
                    if (OrderQueue != null) {
                        OrderQueue.Attach(this);
                    }

                    if (oldmap != null) {
                        PlaceOnMap(oldmap, X, Y);
                    }
                }
            }
        }

        // Create order queue with default order.
        // FIXME: convenience function, move it.
        public void CreateOrderQueue(Order defaultOrder)
        {
            OrderQueue = new OrderQueue(defaultOrder);
        }

        // Locator: handles blocking, positioning and pathfinding.
        // NOTE: can be null, this is not an error.
        // If null the unit can't use the pathfinder.
        private ILocator locator;
        public ILocator Locator {
            get {
                return locator;
            }
            set {
                if (Locator != value) {
                    GameMap oldmap = Map;
                    if (oldmap != null) {
                        RemoveFromMap();
                    }

                    if (Locator != null) {
                        Locator.Detach();
                    }
                    locator = value;
                    if (Locator != null) {
                        Locator.Attach(this);
                    }

                    if (oldmap != null) {
                        PlaceOnMap(oldmap, X, Y);
                    }
                }
            }
        }

        // Create a locator.
        // You must call this from the constructor if you want to be able to use
        // the pathfinder.
        // NOTE: This is useful for buildings too, because they block on the pathfinder.
        protected void CreateLocator()
        {
            Locator = Data.Pathfinder.CreateLocator(this);
        }

        // Entity 'cacher', keeps the entity in the cache.
        // FIXME: make it completely independent.
        private EntityCacher cacher;
        public EntityCacher Cacher {
            get {
                return cacher;
            }
        }

        // This little thing is used by certain actions to search for suitable targets.
        // See ITargetFinder.
        public ITargetFinder TargetFinder;

        // The player who owns this entity.
        // FIXME: could be changed?
        // FIXME: remove? Only add in children, if relevant?
        public readonly Player Player;

        // The map the entity is placed on. You can only change this by RemoveFromMap/PlaceOnMap.
        private GameMap map;
        public GameMap Map {
            get {
                return map;
            }
        }

        // If the entity is on the map or not.
        // You can only change this through PlaceOnMap/RemoveFromMap.
        public bool OffMap {
            get {
                return map == null;
            }
        }

        // X coordinate on map (or last pos on map).
        // NOTE: This is valid and meaninful even if OffMap or Removed.
        private int x;
        public int X {
            get {
                return x;
            }
        }

        // Y coordinate on map (or last pos on map).
        // NOTE: This is valid and meaninful even if OffMap or Removed.
        private int y;
        public int Y {
            get {
                return y;
            }
        }

        // Entity position as a point.
        public Point Pos {
            get {
                return new Point(x, y);
            }
        }

        // Orientation of the entity, from 0 to 255.
        // An orientation of 0 means looking towards y-, and it goes clockwise from there.
        // It's just like a clock.
        // FIXME: retarded convention.
        // There are some very good functions in Utilities to deal with this.
        private int orientation;
        public int Orientation {
            get {
                return orientation;
            }
            set {
                // Yes, this works correctly.
                orientation = value & 0xFF;
            }
        }

        // Place an entity on the map at ux, uy.
        // If the pathfinder is not free at that location the entity will still be placed,
        // but you won't be able to move it.
        //
        // If the entity is already on the map it will first remove it.
        //
        // NOTE: You might one to use one of the functions from AG.Utilities
        public void PlaceOnMap(GameMap map, int x, int y)
        {
            if (!OffMap) {
                RemoveFromMap();
            }

            this.map = map;
            this.x = x;
            this.y = y;

            OnPlacedOnMap(map, new Point(x, y));
        }

        // Move the entity from X Y to newx newy.
        // If entity is offmap you get an exception.
        public void MoveOnMap(int newx, int newy)
        {
            // Nothing to do
            if (newx == X && newy == Y) {
                return;
            }

            // Doesn't make sense while OffMap.
            if (OffMap) {
                throw new InvalidOperationException("Can't move the entity while OffMap");
            }

            // Keep old position.
            Point oldpos = new Point(X, Y);

            // Update position
            this.x = newx;
            this.y = newy;

            // Launch events.
            OnMoved(oldpos, new Point(X, Y));
        }

        // Remove an entity from the map. Nothing happens if it's already OffMap.
        public void RemoveFromMap()
        {
            if (OffMap) {
                return;
            }

            Point oldpos = new Point(X, Y);
            GameMap oldmap = Map;
            map = null;
            OnRemovedFromMap(oldmap, oldpos);
        }

        // Remove an entity from the game, forever.
        // DO NOT CONFUSE THIS WITH RemoveFromMap
        // RemoveFromMap removes the entity from the map, but it can be added back.
        // This is forever.
        public override void Remove()
        {
            //Console.WriteLine("Entity removed from game.");
            
            this.RemoveFromMap();
            OnRemovedFromGame();

            // Destroy components.
            Listeners = null;
            locator = null;
            actor = null;
            orderQueue = null;
            cacher = null;

            State.Entities.Remove(this);
            base.Remove();
            if (!Removed) {
                throw new Exception(".Remove() broke");
            }
        }

        // This determines movement speed.
        // See IActor for more details.
        // return <= 0 if the entity can't move.
        public virtual int GetSpeed()
        {
            return 1;
        }

        // Check if the unit can move.
        public bool CanMove()
        {
            return GetSpeed() > 0 && Locator != null;
        }

        // This function is called by the Actor when the animation reaches the
        // so-called hot-spot.
        //
        // Default implementation forwards To OrderQueue.Action.AnimationHotspot
        // (if the OrderQueue is enabled).
        public virtual void AnimationHotspot()
        {
            if (OrderQueue != null) {
                OrderQueue.Action.AnimationHotspot();
            }
        }

        // Check if this entity can target something.
        // This is the most basic validator:
        //   * target.IsEmpty
        //   * unit or target removed.
        //   * offmap or placed on different maps
        //
        // FIXME: Some of these validation errors should really be exceptions.
        public virtual ValidationError ValidateTarget(TargetInfo target)
        {
            if (target.IsEmpty) {
                return new ValidationError("Target is empty.");
            }
            if (this.Removed) {
                return new ValidationError("Entity was removed from the game.");
            }
            if (this.Map == null) {
                return new ValidationError("Entity is not on a map.");
            }
            if (target.HasEntity && target.Entity.Removed) {
                return new ValidationError("Target entity was removed from the game.");
            }
            if (target.Map.Removed) {
                return new ValidationError("Target map was removed from the game.");
            }
            if (this.Map != target.Map) {
                return new ValidationError("Target is on a different map.");
            }
            return null;
        }

        // Update this entity, bring to next cycle.
        public override void NextCycle()
        {
            // Entity might have been killed this exact cycle.
            if (Removed) {
                return;
            }

            // Update Cacher. Not really required but wtf.
            Cacher.NextCycle();
            if (Removed) {
                return;
            }

            // Feed some stuff to the SyncSeed
            if (!OffMap) {
                State.SyncRand(X);
                State.SyncRand(Y);
                State.SyncRand(Orientation);
            }

            // Update actor.
            if (Actor != null) {
                Actor.NextCycle();
                if (Removed) {
                    return;
                }
            } else {
                if (Locator != null) {
                    Locator.Advance(GetSpeed());
                }
            }

            // Update locator.
            if (Locator != null) {
                Locator.NextCycle();
                if (Removed) {
                    return;
                }
            }

            // Update order queue.
            if (OrderQueue != null) {
                if (Actor == null) {
                    throw new InvalidOperationException("OrderQueue requires Actor");
                }
                if (Locator == null) {
                    throw new InvalidOperationException("OrderQueue requires Locator");
                }
                OrderQueue.NextCycle();
                if (Removed) {
                    return;
                }
            }
        }

        // Events. Mirrors IEntityEventListener.
        public event EntityMapEventHandler PlacedOnMap;
        public event EntityMapEventHandler RemovedFromMap;
        public event EntityMovedEventHandler Moved;
        public event EntityEventHandler RemovedFromGame;

        // Array of IEntityEventListeners.
        private ArrayList Listeners = null;

        // Registers a random event listener.
        public void RegisterListener(IEntityEventListener listener)
        {
            if (listener == null) {
                throw new ArgumentNullException("listener");
            }
            if (Listeners == null) {
                Listeners = new ArrayList(1);
            }
            if (Listeners.Contains(listener)) {
                return;
            }
            Listeners.Add(listener);
        }

        // Unregister an event listener.
        // If the listener is not registered nothing bad happens.
        public void UnregisterListener(IEntityEventListener listener)
        {
            Listeners.Remove(listener);
            if (Listeners.Count == 0) {
                Listeners = null;
            }
        }

        // Trigger EntityMoved events.
        protected virtual void OnMoved(Point oldpos, Point newpos)
        {
            if (Moved != null) {
                Moved(this, new EntityMovedEventArgs(this, oldpos, newpos));
            }
            if (Listeners != null) {
                foreach (IEntityEventListener listener in Listeners.ToArray()) {
                    listener.EntityMoved(oldpos, newpos);
                }
            }
        }

        // Trigger PlacedOnMap events.
        protected virtual void OnPlacedOnMap(GameMap map, Point newpos)
        {
            Map.OnEntityPlacedOnMap(this, newpos);
            if (PlacedOnMap != null) {
                PlacedOnMap(this, new EntityMapEventArgs(this, map, newpos));
            }
            if (Listeners != null) {
                foreach (IEntityEventListener listener in Listeners.ToArray()) {
                    listener.EntityPlacedOnMap(map, newpos);
                }
            }
        }

        // Trigger RemovedFromMap events.
        protected virtual void OnRemovedFromMap(GameMap map, Point oldpos)
        {
            map.OnEntityRemovedFromMap(this, oldpos);
            if (RemovedFromMap != null) {
                RemovedFromMap(this, new EntityMapEventArgs(this, map, oldpos));
            }
            if (Listeners != null) {
                foreach (IEntityEventListener listener in Listeners.ToArray()) {
                    listener.EntityRemovedFromMap(map, oldpos);
                }
            }
        }

        // Trigger RemovedFromGame events.
        protected virtual void OnRemovedFromGame()
        {
            if (RemovedFromGame != null) {
                RemovedFromGame(this, new EntityEventArgs(this));
            }
            if (Listeners != null) {
                foreach (IEntityEventListener listener in Listeners.ToArray()) {
                    listener.EntityRemovedFromGame();
                }
            }
        }
    }
}
