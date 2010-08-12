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

namespace Alpha.Game
{
    // A heightmap is just a tilemap with height-information at every grid point.
    public interface IHeightMap: ITileMap
    {
        int GetHeight(int x, int y);
    }

    // Doesn't hold much.
    public class HeightMapTileset: GameDataObject, IMapFactory
    {
        // Create a new map with the specific size. Used from GameState:.ctor
        public GameMap CreateMap(GameState state, int width, int height)
        {
            return new HeightMap(state, this, width, height);
        }

        // Size of a tile.
        private int tileSize;
        public int TileSize {
            get {
                return tileSize;
            }
            set {
                tileSize = value;
            }
        }

        // Constructors.
        public HeightMapTileset(GameData data): base(data)
        {
        }

        public HeightMapTileset(GameState state): base(state)
        {
        }
    }

    // Trivial heightmap.
    public class HeightMap: TileMapBase, IHeightMap
    {
        // The tileset.
        public readonly HeightMapTileset Tileset;

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

        // The height map, heights in every tile point.
        private int[,] hmap;

        // Set the map height at a point, in M-coords.
        public void SetHeight(int x, int y, int value)
        {
            hmap[x, y] = value;
        }

        // Get the map height at a point, in M-coords.
        public int GetHeight(int x, int y)
        {
            return hmap[x, y];
        }

        public void RandomizeHeights(int min, int max)
        {
            int x, y;
            for (x = 0; x < TileWidth; ++x) {
                for (y = 0; y < TileHeight; ++y) {
                    hmap[x, y] = State.SyncRand(max - min + 1) + min;
                }
            }
        }

        public HeightMap(GameState state, HeightMapTileset tset, int width, int height): base(state)
        {
            this.Tileset = tset;
            this.twidth = width;
            this.theight = height;
            hmap = new int[TileWidth + 1, TileHeight + 1];
            Init();
        }
    }
}
