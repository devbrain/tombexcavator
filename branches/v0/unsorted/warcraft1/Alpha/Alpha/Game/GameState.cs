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
    // Represents a GameObject, and object that is part of a certain gamestate.
    //
    // When you create it it's added to a GameState. You can remove it, but when
    // you remove it you remove it forever.
    //
    // In theory you should call CheckRemoved from every function.
    //
    // If you choose to implement IDisposable in descendants, you should call
    // Remove from Dispose, and use Removed as a disposed marker (If you don't
    // know what I'm talking about read about the standard IDisposable pattern).
    public abstract class GameObject
    {
        // The state this object is part of.
        private GameState state;
        public GameState State {
            get {
                return state;
            }
        }

        // Shortcut for State.Data
        public GameData Data {
            get {
                return State.Data;
            }
        }

        // Return is the object was removed.
        public bool Removed {
            get {
                return state == null;
            }
        }

        // If this is true then then constructor will automatically
        // register the object and Remove will unregister it.
        protected virtual bool AutoRegister {
            get {
                return true;
            }
        }

        // Register in the IdManager.
        protected void Register()
        {
            State.IdManager.AddObject(this);
        }

        // Unregister from the IdManager.
        protected void Unregister()
        {
            State.IdManager.RemObject(this);
        }

        // Trivial constructor.
        public GameObject(GameState state)
        {
            if (state == null) {
                throw new ArgumentNullException("Valid GameState required.", "state");
            }
            this.state = state;
            if (AutoRegister) {
                Register();
            }
        }

        // Remove the object from the gamestate.
        public virtual void Remove()
        {
            if (AutoRegister) {
                Unregister();
            }
            state = null;
        }

        // Check if the object is removed, and if so throw an exception.
        public void CheckRemoved()
        {
            if (Removed) {
                throw new InvalidOperationException("You can't do that, the object was removed");
            }
        }
    }

    // Interface for updatable objects
    public interface IUpdatable
    {
        void NextCycle();
    }

    // This is nothing more than a GameObject that also implements IUpdatable.
    // A lot of classes derive from this.
    //
    // It saves you from adding/removing from the update table by hand.
    public abstract class UpdatableGameObject: GameObject, IUpdatable
    {
        // Constructor, adds to update table.
        public UpdatableGameObject(GameState state): base(state)
        {
            State.UpdateTable.Add(this);
        }

        // IUpdatable.NextCycle()
        public abstract void NextCycle();

        // Remove the object.
        // Serves as a destructor.
        public override void Remove()
        {
            State.UpdateTable.Remove(this);
            base.Remove();
        }
    }

    // This is the big class containing everything about the state of the game.
    // Information which can be shared between multiple GameStates is in GameData.
    public class GameState: IObjectIdManager
    {
        // The information about the actual game.
        public readonly GameData Data;

        // The maximum number of players. Fixed to 32.
        public const int MaxPlayers = 32;

        // Player array.
        private Player[] players;

        // Get a player by id. Returns null if player doesn't exist.
        public Player GetPlayer(int id)
        {
            return players[id];
        }

        // Registers a certain player with the GameState.
        // Player's constructor call this.
        internal void RegisterPlayer(Player player)
        {
            if (player == null) {
                throw new ArgumentNullException("player");
            }
            if (player.State != this) {
                throw new InvalidOperationException("Player belongs to another state.");
            }
            if (players[player.Ident] == player) {
                throw new InvalidOperationException("Player already registered.");
            }
            if (players[player.Ident] != null) {
                throw new InvalidOperationException("Duplicate player id.");
            }
            players[player.Ident] = player;
        }

        // Override to create your kind of player.
        public virtual Player CreatePlayer(int id)
        {
            return new Player(this, id);
        }

        // Objects that need ids should add themselves here.
        public readonly ObjectIdManager IdManager;

        // Collection of maps.
        public readonly Collection Maps;

        // Collection of entities.
        public readonly Collection Entities;

        // Collection of updatable objects.
        // This is a SyncronizedOrderCollection.
        public readonly SyncronizedOrderCollection UpdateTable;

        // Get the map, if only one is available.
        // Throws InvalidOperationException if Maps.Count != 1
        // This is a helper for games that use only one map.
        //
        // There is rarely any reason to use this from Alpha.dll
        public GameMap Map {
            get {
                IEnumerator e;

                if (Maps.Count != 1) {
                    throw new InvalidOperationException("GameState doesn't have exactly one map, GameState.Map invalid");
                }

                // IEnumerator starts BEFORE the first element, so we have to MoveNext()
                e = Maps.GetEnumerator();
                e.Reset();
                e.MoveNext();
                return (GameMap)e.Current;
            }
        }

        // The current cycle.
        // Incremented by NextCycle, never goes backwards.
        private int cycle;
        public int Cycle {
            get {
                return cycle;
            }
        }

        // Advance the gamestate another cycle.
        public void NextCycle()
        {
            ++cycle;

            // Advance each entity.
            // Since the entity table can change, we must copy it.
            foreach (IUpdatable obj in UpdateTable.CloneToArray()) {
                obj.NextCycle();
            }
        }

        // Syncronised random number generator seed.
        public uint SyncSeed;

        // Pseudo random number generator.
        // Returns a random number in the 0..range-1 range. Returned values
        // are the same on all computers in a multiplayer game, provided that
        // the queries are the same.
        // A range of 0 will return 32 random bits.
        //
        // Implementation is a variation of a linear congruential generator.
        // The range is added to the seed after every query, so the generated
        // sequence actually depends on the range you ask.
        //
        // WARNING: Only call this from game-state code. The range parameter
        // modifies the seed, which is used for syncronisation checking.
        //
        // If you want to make sure some code is syncronised, you can call
        // SyncRange(some_value) and throw away the result. This makes
        // some_value affect the SyncSeed, and makes syncronisation errors
        // easier to spot.
        public uint SyncRand(uint range)
        {
            const uint LCG_A = 1664525;
            const uint LCG_B = 1013904223;

            // This will overflow. So what? :)
            SyncSeed = LCG_A * SyncSeed + LCG_B + range;
            if (range == 0) {
                return SyncSeed;
            } else {
                return SyncSeed % range;
            }
        }

        // Same as the other SyncRand, but with int not uint
        // returns values in range 0, range; inclusive.
        public int SyncRand(int range)
        {
            return (int)SyncRand((uint)range);
        }

        // Returns values in [min, max], inclusive.
        public int SyncRand(int min, int max)
        {
            if (min > max) {
                return SyncRand(min, max); 
            }
            if (min == max) {
                return min;
            }
            return min + (int)SyncRand((uint)(max - min + 1));
        }

        // Create a new GameState.
        public GameState(GameData data)
        {
            if (data == null) {
                throw new ArgumentException("GameState need a valid data set");
            }
            Data = data;
            IdManager = new ObjectIdManager();
            IdManager.SetIdRange(0, 999999999);
            Entities = new Collection();
            Maps = new Collection();
            players = new Player[MaxPlayers];
            UpdateTable = new SyncronizedOrderCollection();
            SyncSeed = 0xc0fee;
            cycle = 0;
        }

        // If id is a valid network id.
        public bool HasIdent(uint id)
        {
            return this.IdManager.HasIdent(id) || Data.IdManager.HasIdent(id);
        }

        // If object has an identifier
        public bool HasObject(object obj)
        {
            return this.IdManager.HasObject(obj) || Data.IdManager.HasObject(obj);
        }

        // Find a network id given an object.
        public uint GetIdent(object obj)
        {
            if (this.IdManager.HasObject(obj)) {
                return this.IdManager.GetIdent(obj);
            }
            if (Data.IdManager.HasObject(obj)) {
                return Data.IdManager.GetIdent(obj);
            }
            throw new Exception("Object not found");
        }

        // Find an object given a network id.
        public object GetObject(uint id)
        {
            if (this.IdManager.HasIdent(id)) {
                return this.IdManager.GetObject(id);
            }
            if (Data.IdManager.HasIdent(id)) {
                return Data.IdManager.GetObject(id);
            }
            throw new Exception("Ident not found");
        }
    }
}
