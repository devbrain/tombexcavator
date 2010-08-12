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
    // Various useful functions.
    // Most of these are convenience function moved here to avoid cluttering in
    // the actual classes.
    public sealed class Utilities
    {
        // Changes Entity.Orientation along a vector.
        public static void OrientEntityAlong(Entity ent, int x, int y)
        {
            if (x != 0 || y != 0) {
                ent.Orientation = IntMath.GetOrientation(x, y) + 64;
            }
        }

        // Changes Entity.Orientation along a vector.
        public static void OrientEntityAlong(Entity ent, Point p)
        {
            OrientEntityAlong(ent, p.X, p.Y);
        }

        // Changes Entity.Orientation towards some point on the map.
        public static void OrientEntity(Entity ent, int x, int y)
        {
            OrientEntityAlong(ent, x - ent.X, y - ent.Y);
        }

        public static void OrientEntity(Entity ent, Point p)
        {
            OrientEntity(ent, p.X, p.Y);
        }

        public static void OrientEntity(Entity ent, TargetInfo target)
        {
            OrientEntity(ent, target.X, target.Y);
        }

        // Get an entity's orientation in radians
        // NOTE: For display only, it uses floating point
        public static double EntityOrientationRadians(Entity ent)
        {
            return ((ent.Orientation - 64) * Math.PI) / 128;
        }

        // Get an entity's orientation in degrees
        // NOTE: For display only, it uses floating point
        public static double EntityOrientationDegrees(Entity ent)
        {
            return ((ent.Orientation - 64) * 180.0) / 128;
        }

        // This uses Locator.PlaceOnMap to place the entity on the map in a certain range.
        //
        // It returns false on failure and true on succes. If it fails the unit is not
        // placed on the map (should be obvious).
        public static bool PlaceEntityRange(Entity entity, GameMap map, int x, int y, int range)
        {
            if (entity == null) {
                throw new ArgumentNullException("entity");
            }
            if (map == null) {
                throw new ArgumentNullException("map");
            }

            // Trivial if there's no locator, can't fail.
            if (entity.Locator == null) {
                entity.PlaceOnMap(map, x, y);
                return true;
            }

            // Find a free spot or fail.
            int ox, oy;
            if (entity.Locator.FindFreeSpot(map, x, y, range, out ox, out oy)) {
                entity.PlaceOnMap(map, ox, oy);
                return true;
            } else {
                return false;
            }
        }

        // Convenience function.
        public static bool PlaceEntityRange(Entity ent, TargetInfo target, int range)
        {
            return PlaceEntityRange(ent, target.Map, target.X, target.Y, range);
        }

        // Tries to create and place an Entity on the map.
        //
        // If it fails in placing the entity on the map it will Remove it
        // from the game and return null. What do you need a removed entity for?
        //
        // This the function you probably want to call from summoning code.
        public static Entity TryCreatePlaceEntity(IEntityFactory factory,
                Player p, GameMap map, int x, int y, int range)
        {
            // Check parameters.
            if (factory == null) {
                throw new ArgumentNullException("factory");
            }
            if (map == null) {
                throw new ArgumentNullException("map");
            }

            Entity entity = factory.CreateEntity(map.State, p);
            if (!PlaceEntityRange(entity, map, x, y, range)) {
                entity.Remove();
                return null;
            } else {
                return entity;
            }
        }

        // Convenience function.
        public static Entity TryCreatePlaceEntity(IEntityFactory factory,
                Player p, TargetInfo target, int range)
        {
            return TryCreatePlaceEntity(factory, p, target.Map, target.X, target.Y, range);
        }
    }
}
