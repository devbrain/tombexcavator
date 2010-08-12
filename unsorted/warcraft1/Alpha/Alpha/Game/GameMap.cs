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
    // GameMap factory
    public interface IMapFactory
    {
        // Create a new map with the specific size.
        GameMap CreateMap(GameState state, int width, int height);
    }

    // Map class. Multiple maps are supported.
    // Entities can be placed on maps or even moved from one to another.
    //
    // This class also keeps pathfinding information (a PathMap), an
    // entity cache and keeps track of graphic objects.
    //
    // Tracking graphic objects will be removed soon.
    public abstract class GameMap: UpdatableGameObject
    {
        // Width of the map in entity coords.
        public abstract int Width {
            get;
        }
        
        // Height of the map in entity coords.
        public abstract int Height {
            get;
        }

        // The pathfinder for this map.
        private IPathMap pathMap;
        public IPathMap PathMap {
            get {
                return pathMap;
            }
        }

        // Cache, contains entities on the map.
        private EntityCache cache;
        public EntityCache Cache {
            get {
                return cache;
            }
        }

        // Constructor. It creates the pathfinder and entity cache.
        public GameMap(GameState state): base(state)
        {
        }

        // You MUST call this function from child class constructors.
        protected void Init()
        {
            int cts = Data.CacheTileSize;
            cache = new EntityCache(State, (Width - 1) / cts + 1, (Height - 1) / cts + 1);
            pathMap = Data.Pathfinder.CreatePathMap(this);
            State.Maps.Add(this);
        }

        public override void Remove()
        {
            State.Maps.Remove(this);
        }

        // Event raised when an entity is placed on this map.
        public event EntityMapEventHandler EntityPlacedOnMap;

        // Event raised when an entity is removed from this map.
        public event EntityMapEventHandler EntityRemovedFromMap;

        // Should only be called from Entity.
        internal void OnEntityPlacedOnMap(Entity entity, Point pos)
        {
            if (EntityPlacedOnMap != null) {
                EntityPlacedOnMap(this, new EntityMapEventArgs(entity, this, pos));
            }
        }

        // Should only be called from Entity.
        internal void OnEntityRemovedFromMap(Entity entity, Point pos)
        {
            if (EntityRemovedFromMap != null) {
                EntityRemovedFromMap(this, new EntityMapEventArgs(entity, this, pos));
            }
        }

        // Bring to next cycle.
        public override void NextCycle()
        {
            PathMap.NextCycle();
        }
    }

    // A tilemap is a rectangular field of square tiles.
    // This is a basic interface implemented by tile-based maps.
    // Drawing sort of relies on it.
    public interface ITileMap
    {
        // The width of the map, in tiles.
        int TileWidth {
            get;
        }

        // The height of the map, in tiles.
        int TileHeight {
            get;
        }

        // The size of a single tile. Tiles are square.
        int TileSize {
            get;
        }
    }

    // Base class for tile-based maps.
    public abstract class TileMapBase: GameMap, ITileMap
    {
        // The tile width of the map, the number of tiles in the x direction.
        public abstract int TileWidth {
            get;
        }

        // The tile height of the map, the number of tiles in the y direction.
        public abstract int TileHeight {
            get;
        }

        // Returns the size of a tile.
        public abstract int TileSize {
            get;
        }

        // Width of the map in entity coords.
        public override int Width {
            get {
                return TileWidth * TileSize;
            }
        }
        
        // Height of the map in entity coords.
        public override int Height {
            get {
                return TileHeight * TileSize;
            }
        }

        public TileMapBase(GameState state): base(state)
        {
        }
    }
}
