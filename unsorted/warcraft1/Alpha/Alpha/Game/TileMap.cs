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
using System.Drawing;

using Alpha.Util;

namespace Alpha.Game
{
    // Tile map that has type information for every tile.
    public interface ITypedTileMap: ITileMap
    {
        // Get the type of a certain tile.
        int GetTileType(int x, int y);

        // Set the type of a certain tile.
        void SetTileType(int x, int y, int type);
    }

    // Shared information about SimpleTileMap tiles.
    public struct SimpleTileData
    {
        // Flags. Meaning is implementation-dependant.
        private int flags;
        public int Flags {
            get {
                return flags;
            }
            set {
                flags = value;
            }
        }

        // A value associated with the tile. Meaning is implementation-dependant.
        private int value;
        public int Value {
            get {
                return this.value;
            }
            set {
                this.value = value;
            }
        }

        // Starting value for SimpleTile.Flags.
        // This is assigned in SimpleTileMap.InitTile
        private int startFlags;
        public int StartFlags {
            get {
                return startFlags;
            }
            set {
                startFlags = value;
            }
        }

        // Starting value for SimpleTile.Value.
        // This is assigned in SimpleTileMap.InitTile
        private int startValue;
        public int StartValue {
            get {
                return startValue;
            }
            set {
                startValue = value;
            }
        }
    }

    // A single SimpleTileMap tile.
    public struct SimpleTile
    {
        // This is an index into Tileset.Tiles. You can use SimpleTileMap.GetTileData
        // to avoid long statements.
        private int type;
        public int Type {
            get {
                return type;
            }
            set {
                type = value;
            }
        }

        // Flags. Meaning is implementation-dependant.
        private int flags;
        public int Flags {
            get {
                return flags;
            }
            set {
                flags = value;
            }
        }

        // A value associated with the tile. Meaning is implementation-dependant.
        private int value;
        public int Value {
            get {
                return this.value;
            }
            set {
                this.value = value;
            }
        }
    }

    // Shared data for a SimpleTileMap.
    // Little more than an array of SimpleTileData.
    public class SimpleTileMapTileset: GameDataObject, IMapFactory
    {
        // The list of all TileInfos.
        public readonly ArrayList Tiles;

        // Size of a single tile. Tiles are square.
        private int tileSize;
        public int TileSize {
            get {
                return tileSize;
            }
            set {
                tileSize = value;
            }
        }

        // Create a new map with the specific size.
        public GameMap CreateMap(GameState state, int width, int height)
        {
            return new SimpleTileMap(state, this, width, height);
        }

        public SimpleTileMapTileset(GameData data): base(data)
        {
            Tiles = new ArrayList();
        }

        public SimpleTileMapTileset(GameState state): base(state)
        {
            Tiles = new ArrayList();
        }

        // Index into the Tiles array.
        public SimpleTileData this[int index] {
            get {
                return (SimpleTileData)Tiles[index];
            }
            set {
                Tiles[index] = value;
            }
        }
    }

    // Simple implementation of TileMapBase.
    //
    // It's implemented as an array of SimpleTile. The dimension of a tile
    // in memory is a meager 12 bytes.
    //
    // This class can work for a ton of actual games because the meaning
    // of the various tile infos is not specified.
    public class SimpleTileMap: TileMapBase, ITypedTileMap
    {
        // The actual tileset, with information about the tiles and all.
        public readonly SimpleTileMapTileset Tileset;

        private int twidth;
        public override int TileWidth {
            get {
                return twidth;
            }
        }

        private int theight;
        public override int TileHeight {
            get {
                return theight;
            }
        }

        public override int TileSize {
            get {
                return Tileset.TileSize;
            }
        }

        // The big array of tiles.
        private SimpleTile[,] Tiles;

        // Get the tile at x, y.
        public SimpleTile GetTile(int x, int y)
        {
            return Tiles[x, y];
        }

        // Get the tile at x, y.
        public void SetTile(int x, int y, SimpleTile tile)
        {
            Tiles[x, y] = tile;
        }

        // Get tile data for the tile at x, y.
        public SimpleTileData GetTileData(int x, int y)
        {
            return Tileset[Tiles[x, y].Type];
        }

        // Get the type of the tile at x, y.
        public int GetTileType(int x, int y)
        {
            return Tiles[x, y].Type;
        }

        // Set the type of the tile at x, y.
        public void SetTileType(int x, int y, int type)
        {
            Tiles[x, y].Type = type;
        }

        public void InitTile(int x, int y)
        {
            Tiles[x, y].Flags = Tileset[Tiles[x, y].Type].StartFlags;
            Tiles[x, y].Value = Tileset[Tiles[x, y].Type].StartValue;
        }

        public SimpleTileMap(GameState state, SimpleTileMapTileset tset, int width, int height): base(state)
        {
            this.Tileset = tset;
            twidth = width;
            theight = height;
            Tiles = new SimpleTile[TileWidth + 1, TileHeight + 1];
            Init();
        }
    }
}

