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
    // An interface that receives entity events.
    public interface IEntityEventListener
    {
        // Called AFTER the entity is placed on a map.
        void EntityPlacedOnMap(GameMap map, Point newpos);

        // Called AFTER the entity is removed from the map.
        void EntityRemovedFromMap(GameMap map, Point oldpos);

        // Called when the entity is moved on the map.
        void EntityMoved(Point oldpos, Point newpos);

        // Called when the entity is about to be removed from the game.
        // Entity.State is still valid at this time.
        //
        // If the entity is on a map when removed from the game
        // EntityRemovedFromMap is received first.
        void EntityRemovedFromGame();
    }

    // Event args that contains an Entity reference.
    public class EntityEventArgs: EventArgs
    {
        // The entity the event refers to.
        public readonly Entity Entity;

        public EntityEventArgs(Entity e)
        {
            Entity = e;
        }
    }

    // Event args that contains both an Entity and a GameMap reference.
    // Used for PlacedOnMap/RemovedFromMap
    public class EntityMapEventArgs: EntityEventArgs
    {
        // The map.
        public readonly GameMap Map;

        // Position on the map.
        public readonly Point Position;

        public EntityMapEventArgs(Entity e, GameMap m, Point pos): base(e)
        {
            Map = m;
            Position = pos;
        }
    }

    // Event args for the entity moved event.
    public class EntityMovedEventArgs: EntityEventArgs
    {
        // Old location of the entity.
        public readonly Point OldPosition;

        // New location of the entity.
        public readonly Point NewPosition;

        public EntityMovedEventArgs(Entity e, Point oldpos, Point newpos): base(e)
        {
            OldPosition = oldpos;
            NewPosition = newpos;
        }
    }

    public delegate void EntityEventHandler(object sender, EntityEventArgs args);

    public delegate void EntityMapEventHandler(object sender, EntityMapEventArgs args);

    public delegate void EntityMovedEventHandler(object sender, EntityMovedEventArgs args);
}
