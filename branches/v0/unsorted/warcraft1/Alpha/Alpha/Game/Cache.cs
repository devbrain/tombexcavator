// Copyright (C) 2005 Crestez Leonard    cleonard@go.ro
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
    // This class is a data-structure that can be used to query all the entities
    // inside a certain rectangle.
    //
    // For a query of (x1, y1, x2, y2) the find entity functions will return
    // all entities for which:
    //     Entity.UX1 < x2 && x1 < Entity.UX2 && Entity.UY1 < y2 && y1 < Entity.UY2
    // This means that if an entity only *touches* the query area, it won't get it.
    // If you don't like that the feel free to use x1 + 1 and the like.
    //
    // TODO: This must be FAST.
    public class EntityCache: GameObject
    {
        // Lists of entities on every cache tile.
        private ArrayList[,] cache;

        // Get the width of the cache, in cache tiles.
        public int Width {
            get {
                return cache.GetLength(0);
            }
        }

        // Get the height of the cache, in cache tiles.
        public int Height {
            get {
                return cache.GetLength(1);
            }
        }

        // Create a new EntityCache
        // There are width * height cache tiles.
        // The EntityCache should still work for entities outside of those
        // width * height cache tiles, but it will be painfully slow
        public EntityCache(GameState state, int width, int height): base(state)
        {
            int x, y;
            cache = new ArrayList[width, height];
            for (x = 0; x < Width; ++x) {
                for (y = 0; y < Height; ++y) {
                    cache[x, y] = new ArrayList(4);
                }
            }
        }

        // Returns the bounds of an entity in the cache.
        private Rectangle EntityCacheBounds(EntityCacher cacher, Point pos)
        {
            Rectangle output = new Rectangle();

            output.X1 = pos.X - cacher.Size / 2;
            output.Y1 = pos.Y - cacher.Size / 2;
            output.X2 = pos.X + (cacher.Size + 1) / 2;
            output.Y2 = pos.Y + (cacher.Size + 1) / 2;

            return output;
        }

        // This convert a rectangle from Entity coords to cache tile coords
        // Amazingly, it works for both entities and queries.
        private Rectangle EntityToCacheCoords(Rectangle input)
        {
            Rectangle output = new Rectangle();

            output.X1 = input.X1 / Data.CacheTileSize;
            output.X1 = Math.Min(Width - 1, Math.Max(0, output.X1));
            output.Y1 = input.Y1 / Data.CacheTileSize;
            output.Y1 = Math.Min(Height - 1, Math.Max(0, output.Y1));
            output.X2 = (input.X2 - 1) / Data.CacheTileSize + 1;
            output.X2 = Math.Min(Width, Math.Max(output.X1 + 1, output.X2));
            output.Y2 = (input.Y2 - 1) / Data.CacheTileSize + 1;
            output.Y2 = Math.Min(Height, Math.Max(output.Y1 + 1, output.Y2));

            //Console.WriteLine("Entity to Cache: {0} to {1}", input, output);
            return output;
        }

        // Add an entity to the cache
        // Makes a boo boo if entity already in cache.
        // To be called when the entity moves or when the entity is placed on the map.
        public void Add(EntityCacher cacher, Point newpos)
        {
            Rectangle carea;
            int x, y;

            if (cacher.Size < 0) {
                return;
            }

            carea = EntityToCacheCoords(EntityCacheBounds(cacher, newpos));

            //Console.WriteLine("Add entity in cache at {0}", carea);
            for (x = carea.X1; x < carea.X2; ++x) {
                for (y = carea.Y1; y < carea.Y2; ++y) {
                    cache[x, y].Add(cacher);
                }
            }
        }

        // Remove an entity from the cache
        // Makes a boo boo if entity not in cache
        // To be called when the entity moves or when is is removed from the map.
        public void Remove(EntityCacher cacher, Point oldpos)
        {
            Rectangle carea;
            int x, y;

            if (cacher.Size < 0) {
                return;
            }

            carea = EntityToCacheCoords(EntityCacheBounds(cacher, oldpos));

            //Console.WriteLine("Remove entity from cache at {0}", carea);
            for (x = carea.X1; x < carea.X2; ++x) {
                for (y = carea.Y1; y < carea.Y2; ++y) {
                    cache[x, y].Remove(cacher);
                }
            }
        }

        // Returns entities in x1 y2 x2 y2
        // The result contains every entity for which
        //      Entity.UX1 < x2 && x1 < Entity.UX2 && Entity.UY1 < y2 && y1 < Entity.UY2
        // This will return an IList with the entities.
        public IList SelectList(Rectangle area)
        {
            Rectangle carea;
            IList list;
            int x, y;

            if (area.Corner1 == area.Corner2) {
                return SelectList(area.Corner1);
            }

            area = area.Normalize();
            carea = EntityToCacheCoords(area);

            list = new ArrayList();

            for (x = carea.X1; x < carea.X2; ++x) {
                for (y = carea.Y1; y < carea.Y2; ++y) {
                    foreach (EntityCacher c in cache[x, y]) {
                        if (c.Lock == false && area.StrictlyIntersects(EntityCacheBounds(c, c.Entity.Pos))) {
                            c.Lock = true;
                            list.Add(c.Entity);
                        }
                    }
                }
            }

            foreach (Entity ent in list) {
                ent.Cacher.Lock = false;
            }

            return list;
        }

        // Convenience function
        public IList SelectList(int x1, int y1, int x2, int y2)
        {
            return SelectList(new Rectangle(x1, y1, x2, y2));
        }

        // Returns entities at x y
        // The result contains every entity for which
        //      Entity.UX1 < x && x < Entity.UX2 && Entity.UY1 < y && y < Entity.UY2
        // This will return an IList with the entities.
        public IList SelectList(Point p)
        {
            int cx, cy;
            IList list;

            list = new ArrayList();

            cx = Math.Min(Width - 1, Math.Max(0, p.X / Data.CacheTileSize));
            cy = Math.Min(Height - 1, Math.Max(0, p.Y / Data.CacheTileSize));

            foreach (EntityCacher c in cache[cx, cy]) {
                if (EntityCacheBounds(c, c.Entity.Pos).StrictlyIncludes(p)) {
                    list.Add(c.Entity);
                }
            }

            return list;
        }

        // Convenience function
        public IList SelectList(int x, int y)
        {
            return SelectList(new Point(x, y));
        }
    }

    // This class is reponsible for keeping an entity in the cache.
    public class EntityCacher: EntityComponentBase
    {
        // Size in the cache.
        // If this is < 0 (default) then the unit is not placed in the
        // cache.
        private int size = -1;
        public int Size {
            get {
                return size;
            }
            set {
                if (value == size) {
                    return;
                }
                if (Entity.OffMap) {
                    size = value;
                } else {
                    Entity.Map.Cache.Remove(this, Entity.Pos);
                    size = value;
                    Entity.Map.Cache.Add(this, Entity.Pos);
                }
            }
        }

        // Handle add to map.
        public override void EntityPlacedOnMap(GameMap map, Point pos)
        {
            map.Cache.Add(this, pos);
        }

        // Handle moving.
        // FIXME: don't always add/remove; potential optimization.
        public override void EntityMoved(Point oldpos, Point newpos)
        {
            Entity.Map.Cache.Remove(this, oldpos);
            Entity.Map.Cache.Add(this, newpos);
        }

        // Handle remove from map.
        public override void EntityRemovedFromMap(GameMap map, Point pos)
        {
            map.Cache.Remove(this, pos);
        }

        // Used to avoid returning an unit multiple times.
        public bool Lock = false;
    }
}

