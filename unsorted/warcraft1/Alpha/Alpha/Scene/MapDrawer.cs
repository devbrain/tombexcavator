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
using Color = System.Drawing.Color;

using Alpha.Graphics;
using Alpha.Game;
using Alpha.Util;

namespace Alpha.Scene
{
    // Interface for an object which can draw a map.
    public interface IMapDrawer
    {
        void DrawOrtho(GameMap map, Player player, BoundingBox box);

        BoundingBox GetBounds(GameMap map, Player player);
    }

    // Interface for an object which can draw a portion of a ITileMap.
    public interface ITileMapDrawer
    {
        void DrawTileRect(ITileMap map, Player player, Rectangle rect);
    }

    // Adapts ITileMapDrawer to IMapDrawer.
    public class MapDrawerAdaptor: IMapDrawer
    {
        // Inner TileMapDrawer;
        private readonly ITileMapDrawer tileMapDrawer;
        public ITileMapDrawer TileMapDrawer {
            get {
                return tileMapDrawer;
            }
        }

        public MapDrawerAdaptor(ITileMapDrawer tileMapDrawer)
        {
            if (tileMapDrawer == null) {
                throw new ArgumentNullException("tileMapDrawer");
            }
            this.tileMapDrawer = tileMapDrawer;
        }

        // Uses TileMapDrawer.
        // Only draws what's inside the BoundingBox
        public void DrawOrtho(GameMap the_map, Player player, BoundingBox box)
        {
            int x1, y1, x2, y2;
            ITileMap map = (ITileMap)the_map;
            box.Normalize();
            
            x1 = (int)Math.Floor(box.A.X / map.TileSize);
            y1 = (int)Math.Floor(box.A.Y / map.TileSize);
            x2 = (int)Math.Ceiling(box.B.X / map.TileSize);
            y2 = (int)Math.Ceiling(box.B.Y / map.TileSize);

            x1 = Math.Min(Math.Max(x1, 0), map.TileWidth);
            x2 = Math.Min(Math.Max(x2, 0), map.TileWidth);
            y1 = Math.Min(Math.Max(y1, 0), map.TileHeight);
            y2 = Math.Min(Math.Max(y2, 0), map.TileHeight);

            TileMapDrawer.DrawTileRect(map, player, new Rectangle(x1, y1, x2, y2));
        }

        public BoundingBox GetBounds(GameMap the_map, Player player)
        {
            ITileMap map = (ITileMap)the_map;
            BoundingBox b;

            b.A = new Vector3D(0, 0, 0);
            b.B = new Vector3D(map.TileWidth * map.TileSize, map.TileHeight * map.TileSize, 0);

            return b;
        }
    }

    // HeightMap drawer based on blending two textures.
    public class BlendHeightMapDrawer: ITileMapDrawer
    {
        // Texture used for low terrain.
        public Texture LowTexture;
           
        // Texture used for height terrain.
        public Texture HighTexture;

        // The low and high blending points.
        // At LowPoint and below it uses LowTexture, at HighPoint and above it uses
        // HighTexture, between them it smoothly blends the texture.
        public int LowPoint = 0;
            
        public int HighPoint = 1;

        // Get the Weight of the high texture at x, y in M-coords.
        private byte GetHighWeight(IHeightMap map, int x, int y)
        {
            int z = (255 * (map.GetHeight(x, y) - LowPoint)) / (HighPoint - LowPoint);
            return (byte)(z < 0 ? 0 : (z > 255 ? 255 : z));
        }

        // Draw a rect of tiles.
        public void DrawTileRect(ITileMap the_map, Player player, Rectangle rect)
        {
            int x, y;

            IHeightMap map = (IHeightMap)the_map;

            GL.MatrixMode(MatrixMode.ModelView);
            GL.PushMatrix();
            GL.Scale(map.TileSize, map.TileSize, 1);
            GL.Enable(Feature.Texture);
           
            // Draw the LowTexture.
            LowTexture.Bind();
            GL.Color(255, 255, 255, 255);
            GL.Begin(DrawMode.Quads);
            for (x = rect.X1; x < rect.X2; ++x) {
                for (y = rect.Y1; y < rect.Y2; ++y) {
                    GL.TexCoord(x, y);
                    GL.Vertex(x, y, map.GetHeight(x, y));
                    GL.TexCoord(x + 1, y);
                    GL.Vertex(x + 1, y, map.GetHeight(x + 1, y));
                    GL.TexCoord(x + 1, y + 1);
                    GL.Vertex(x + 1, y + 1, map.GetHeight(x + 1, y + 1));
                    GL.TexCoord(x, y + 1);
                    GL.Vertex(x, y + 1, map.GetHeight(x, y + 1));
                }
            }
            GL.End();
            LowTexture.Unbind();

            // Draw the HighTexture.
            HighTexture.Bind();
            GL.Begin(DrawMode.Quads);
            for (x = rect.X1; x < rect.X2; ++x) {
                for (y = rect.Y1; y < rect.Y2; ++y) {
                    GL.TexCoord(x, y);
                    GL.Color(255, 255, 255, GetHighWeight(map, x, y));
                    GL.Vertex(x, y, map.GetHeight(x, y));
                    GL.TexCoord(x + 1, y);
                    GL.Color(255, 255, 255, GetHighWeight(map, x + 1, y));
                    GL.Vertex(x + 1, y, map.GetHeight(x + 1, y));
                    GL.TexCoord(x + 1, y + 1);
                    GL.Color(255, 255, 255, GetHighWeight(map, x + 1, y + 1));
                    GL.Vertex(x + 1, y + 1, map.GetHeight(x + 1, y + 1));
                    GL.TexCoord(x, y + 1);
                    GL.Color(255, 255, 255, GetHighWeight(map, x, y + 1));
                    GL.Vertex(x, y + 1, map.GetHeight(x, y + 1));
                }
            }
            GL.End();
            HighTexture.Unbind();

            GL.MatrixMode(MatrixMode.ModelView);
            GL.PopMatrix();
        }
    }

    // Draws an ITypedTileMap based on a bunch of Textures.
    // FIXME: smarter mapping.
    public class TypedTileMapDrawer: ITileMapDrawer
    {
        // The texture for every tile type.
        // Links to those textures actually.
        public ILazyLink[] Textures;

        // Draw a rect of tiles.
        public void DrawTileRect(ITileMap the_map, Player p, Rectangle rect)
        {
            int x, y;
            ITypedTileMap map = (ITypedTileMap)the_map;

            GL.PushMatrix();
            GL.Scale(map.TileSize, map.TileSize, 1);
            GL.Color(255, 255, 255, 255);
            GL.Enable(Feature.Texture);
            for (x = rect.X1; x < rect.X2; ++x) {
                for (y = rect.Y1; y < rect.Y2; ++y) {
                    Texture tex = (Texture)(Textures[map.GetTileType(x, y)].Get());

                    tex.Bind();
                    GL.Begin(DrawMode.Quads);
                    GL.TexCoord(0, 0);
                    GL.Vertex(x, y);
                    GL.TexCoord(1, 0);
                    GL.Vertex(x + 1, y);
                    GL.TexCoord(1, 1);
                    GL.Vertex(x + 1, y + 1);
                    GL.TexCoord(0, 1);
                    GL.Vertex(x, y + 1);
                    GL.End();
                    tex.Unbind();
                }
            }
            GL.PopMatrix();
        }
    }

    // Draws a pathfinder blocktable. Useful in debugging.
    public class PathfinderDrawer: IMapDrawer
    {
        // If true it will draw the block table.
        public bool DrawBlockTable = true;

        // If true it will draw PermBlock instead of Block, otherwise it will draw PermBlock.
        public bool DrawPermBlock = true;

        // If true it will draw the last pathfinder search.
        public bool DrawLastSearch = true;

        // The layer to draw. See TilePathLayer and TilePathMap.
        public int Layer = 0;

        // The colors to use for the tiles.
        public Color FreeColor = Color.FromArgb(128, 0, 255, 0);
        public Color BlockedColor = Color.FromArgb(128, 255, 0, 0);

        // The colors to use for the last search
        public Color FromColor = Color.FromArgb(0, 255, 0, 0);
        public Color ToColor = Color.FromArgb(255, 0, 0, 255);

        // Draw the BlockTable.
        public void DoDrawBlockTable(TilePathLayer layer, Rectangle rect)
        {
            int x, y;
            BlockTable block;

            block = DrawPermBlock ? layer.PermBlock : layer.Block;

            GL.PushMatrix();
            GL.Scale(layer.Pathfinder.TileSize, layer.Pathfinder.TileSize, 1);
            GL.Disable(Feature.Texture);
            GL.Begin(DrawMode.Quads);

            int lsq = block.LargestSquare;
            for (x = rect.X1; x < rect.X2; ++x) {
                for (y = rect.Y1; y < rect.Y2; ++y) {
                    int q = Math.Max(0, (int)block[x, y]);
                    GL.Color((byte)((q * FreeColor.R + (lsq - q) * BlockedColor.R) / lsq),
                             (byte)((q * FreeColor.G + (lsq - q) * BlockedColor.G) / lsq),
                             (byte)((q * FreeColor.B + (lsq - q) * BlockedColor.B) / lsq),
                             (byte)((q * FreeColor.A + (lsq - q) * BlockedColor.A) / lsq));
                    GL.Vertex(x, y);
                    GL.Vertex(x + 1, y);
                    GL.Vertex(x + 1, y + 1);
                    GL.Vertex(x, y + 1);
                }
            }
            GL.End();
            GL.PopMatrix();
        }

        // Draw the last FindPath
        public void DoDrawLastSearch(TilePathLayer layer, Rectangle rect)
        {
            int x, y;
            GL.Disable(Feature.Texture);
            GL.Begin(DrawMode.Lines);

            for (x = rect.X1; x < rect.X2; ++x) {
                for (y = rect.Y1; y < rect.Y2; ++y) {
                    int cnode = layer.Matrix[x, y] - layer.FirstNodeNumber;
                    if (cnode < 0) {
                        continue;
                    }
                    int pnode = layer.Nodes[cnode].Prev;
                    if (pnode < 0) {
                        continue;
                    }
                    GL.Color(FromColor);
                    GL.Vertex(layer.Nodes[pnode].EX, layer.Nodes[pnode].EY);
                    GL.Color(ToColor);
                    GL.Vertex(layer.Nodes[cnode].EX, layer.Nodes[cnode].EY);
                }
            }
            GL.End();
        }

        // Draw the monkey
        public void DrawOrtho(GameMap map, Player player, BoundingBox box)
        {

            if (!(map.PathMap is TilePathMap)) {
                throw new ArgumentException("Map doesn't use a tile-based pathfinder", "map");
            }
            TilePathMap pm = (TilePathMap)map.PathMap;
            if (Layer < 0 | Layer >= pm.LayerCount) {
                throw new InvalidOperationException("Layer is out of range");
            }

            int ts = pm.Pathfinder.TileSize;

            box.Normalize();
            Rectangle rect = new Rectangle(
                    (int)Math.Floor(box.A.X / ts),
                    (int)Math.Floor(box.A.Y / ts),
                    (int)Math.Ceiling(box.B.X / ts),
                    (int)Math.Ceiling(box.B.Y / ts));

            if (DrawBlockTable) {
                DoDrawBlockTable(pm[Layer], rect);
            }
            if (DrawLastSearch) {
                DoDrawLastSearch(pm[Layer], rect);
            }
        }

        public BoundingBox GetBounds(GameMap map, Player player)
        {
            return BoundingBox.Infinite;
        }
    }
}
