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
    // Represents an object that can be shared between multiple GameStates.
    // It can be part of the GameState or of the GameData.
    //
    // Even if you make it part of the GameState, you still can't access the
    // GameState from the object. Adding a GameDataObject a GameState is simply
    // a method for making scenario-specific data changes. 
    //
    // There are two constructors: one which adds to a GameStats and another
    // to a GameData. If you don't want to be added to a GameState for some
    // reason you can simply refuse to implement the constructor.
    //
    // You can remove it, but when you remove it you remove it forever.
    // In theory you should call CheckRemoved from every function, but it's
    // not really needed.
    //
    // If you choose to implement IDisposable in descendants, you should call
    // Remove from Dispose, and use Removed as a disposed marker (If you don't
    // know what I'm talking about read about the standard IDisposable pattern).
    public abstract class GameDataObject
    {
        // This can be either a GameState or a GameData
        private object parent;

        // The GameData this is part of.
        // If it's placed in a GameState, this is State.Data.
        public GameData Data {
            get {
                if (parent is GameData) {
                    return (GameData)parent;
                } else {
                    return ((GameState)parent).Data;
                }
            }
        }

        // Return is the object was removed.
        public bool Removed {
            get {
                return parent == null;
            }
        }

        // If this returns true then the constructor will automatically register the
        // object and Remove will Unregister.
        //
        // Defaults to true.
        protected bool AutoRegister {
            get {
                return true;
            }
        }

        // Create and place in a GameData
        public GameDataObject(GameData data)
        {
            if (data == null) {
                throw new ArgumentNullException("data");
            }
            parent = data;
            if (AutoRegister) {
                Register();
            }
        }

        // Create and place in a GameState
        public GameDataObject(GameState state)
        {
            if (state == null) {
                throw new ArgumentNullException("state");
            }
            parent = state;
            if (AutoRegister) {
                Register();
            }
        }

        // Register in the IdManager.
        protected void Register()
        {
            if (parent is GameData) {
                ((GameData)parent).IdManager.AddObject(this);
            } else {
                ((GameState)parent).Data.IdManager.AddObject(this);
            }
        }

        // Unregister from the IdManager.
        protected void Unregister()
        {
            if (parent is GameData) {
                ((GameData)parent).IdManager.RemObject(this);
            } else {
                ((GameState)parent).Data.IdManager.RemObject(this);
            }
        }

        // Remove the object from the gamestate.
        public virtual void Remove()
        {
            if (AutoRegister) {
                Unregister();
            }
            parent = null;
        }

        // Check if the object is removed, and if so throw an exception.
        public void CheckRemoved()
        {
            if (Removed) {
                throw new InvalidOperationException("You can't do that, the object was removed");
            }
        }
    }

    // This class contains the description of the actual game. It includes
    // Entity types and other configuration data that can't be changed during
    // the game.
    public class GameData
    {
        // This is the size in U-coords of a Cache tile.
        // This should have no influence on gameplay
        // Try to make it divisible with/multiple of fog and path tile sizes
        private int cacheTileSize = 300;
        public int CacheTileSize {
            get {
                return cacheTileSize;
            }
            set {
                if (cacheTileSize < 1) {
                    throw new ArgumentOutOfRangeException("value");
                }
                cacheTileSize = value;
            }
        }

        // The master pathfinder class.
        public IPathfinder Pathfinder;

        // Objects that need IDs should add themselves here.
        public readonly ObjectIdManager IdManager;

        // Create a new game description.
        public GameData()
        {
            IdManager = new ObjectIdManager();
            IdManager.SetIdRange(1000000000, 1999999999);
        }

        // Creates a new GameState. You should override in games.
        public virtual GameState CreateGameState()
        {
            return new GameState(this);
        }
    }
}
