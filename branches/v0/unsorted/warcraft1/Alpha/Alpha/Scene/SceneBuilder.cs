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

using Alpha.Game;
using Alpha.Graphics;

namespace Alpha.Scene
{
    // Dumb interface for a scene builder.
    // A MapSceneBuilder builds a Scene that draws a map.
    // You CAN add your own objects in the scene.
    //
    // Used by MapView widgets or similar.
    public interface IMapSceneBuilder
    {
        // This is the map behind the scene.
        GameMap Map { get; }

        // The generated scene. You can add your own objects.
        Alpha.Scene.Scene Scene { get; }
    }

    // Creates graphics for maps and entities.
    //
    // Used by MapSceneBuilder
    public interface IGameGraphicFactory
    {
        IGraphicObject CreateEntityGraphic(Entity entity, Player player);

        IGraphicObject CreateMapGraphic(GameMap map, Player player);
    }

    // Basic implementation of IMapSceneBuilder.
    //
    // It responds to EntityPlacedOnMap event to add entity graphics.
    public abstract class MapSceneBuilderBase: IMapSceneBuilder
    {
        // The map.
        private GameMap map;
        public GameMap Map {
            get {
                return map;
            }
        }

        // The scene. You can add your own objects inside the scene.
        private Alpha.Scene.Scene scene;
        public Alpha.Scene.Scene Scene {
            get {
                return scene;
            }
        }

        // The player to draw for. Sort of.
        private Player player;
        public Player Player {
            get {
                return player;
            }
        }

        // Override to create a map drawer. Can be null.
        protected abstract IGraphicObject CreateMapGraphic(GameMap map, Player player);

        // Override to create an entity drawer. Can be null.
        protected abstract IGraphicObject CreateEntityGraphic(Entity entity, Player player);

        private void Map_EntityPlacedOnMap(object sent, EntityMapEventArgs args)
        {
            IGraphicObject obj;

            obj = CreateEntityGraphic(args.Entity, Player);
            if (obj != null) {
                Scene.AddObject(obj);
            }
        }

        // Constructor.
        public MapSceneBuilderBase(GameMap map, Player player)
        {
            this.map = map;
            this.player = player;
            this.scene = new TrivialScene();

            map.EntityPlacedOnMap += new EntityMapEventHandler(Map_EntityPlacedOnMap);
        }

        // You must call this function from the constructor.
        // It will create the initial graphical objects for the map and
        // the entities currently on it.
        protected void Initialize()
        {
            IGraphicObject obj;

            obj = CreateMapGraphic(Map, Player);
            if (obj != null) {
                Scene.AddObject(obj);
            }

            foreach (Entity e in map.State.Entities) {
                if (e.Map == map) {
                    obj = CreateEntityGraphic(e, Player);
                    if (obj != null) {
                        Scene.AddObject(obj);
                    }
                }
            }
        }
    }

    // Builds a scene using an IGameGraphicBuilder
    public class MapSceneBuilder: MapSceneBuilderBase
    {
        // Everything is forwarded here.
        private IGameGraphicFactory graphicFactory;
        public IGameGraphicFactory GraphicFactory {
            get {
                return graphicFactory;
            }
        }

        protected override IGraphicObject CreateEntityGraphic(Entity entity, Player player)
        {
            return GraphicFactory.CreateEntityGraphic(entity, player);
        }

        protected override IGraphicObject CreateMapGraphic(GameMap map, Player player)
        {
            return GraphicFactory.CreateMapGraphic(map, player);
        }

        public MapSceneBuilder(IGameGraphicFactory factory, GameMap map, Player player): base(map, player)
        {
            this.graphicFactory = factory;
            Initialize();
        }
    }

    // Map Graphic using a drawer.
    public class MapDrawerGraphic: GraphicObjectBase, IDrawOrderable
    {
        // Map to draw.
        private readonly GameMap map;
        public GameMap Map {
            get {
                return map;
            }
        }

        // Player to draw for.
        private readonly Player player;
        public Player Player {
            get {
                return player;
            }
        }

        // This is used to draw things.
        private readonly IMapDrawer drawer;
        public IMapDrawer Drawer {
            get {
                return drawer;
            }
        }

        // Constructor
        public MapDrawerGraphic(GameMap map, Player player, IMapDrawer drawer)
        {
            this.map = map;
            this.player = player;
            this.drawer = drawer;
        }

        // Creates an MapDrawerAdaptor
        public MapDrawerGraphic(GameMap map, Player player, ITileMapDrawer drawer)
        {
            this.map = map;
            this.player = player;
            this.drawer = new MapDrawerAdaptor(drawer);
        }

        // Uses TileMapDrawer.
        // Only draws what's inside the BoundingBox
        public override void DrawOrtho(BoundingBox box)
        {
            Drawer.DrawOrtho(Map, Player, box);
        }

        // Returns the map's bounds.
        public override BoundingBox GetBounds()
        {
            return Drawer.GetBounds(Map, Player);
        }

        // Explicit
        public int DrawLayer = -100;

        public int GetDrawLayer()
        {
            return DrawLayer;
        }

        // Explicit
        public double DrawLevel = 0;

        public double GetDrawLevel()
        {
            return DrawLevel;
        }
    }

    // Entity Graphic using a drawer.
    public class EntityDrawerGraphic: GraphicObjectBase, IDrawOrderable, IEntityEventListener
    {
        // Entity to draw.
        private readonly Entity entity;
        public Entity Entity {
            get {
                return entity;
            }
        }

        // Player to draw for.
        private readonly Player player;
        public Player Player {
            get {
                return player;
            }
        }

        // This is used to draw things.
        private readonly IEntityDrawer drawer;
        public IEntityDrawer Drawer {
            get {
                return drawer;
            }
        }

        // Constructor
        public EntityDrawerGraphic(Entity entity, Player player, IEntityDrawer drawer)
        {
            this.entity = entity;
            this.player = player;
            this.drawer = drawer;
            Entity.RegisterListener(this);
        }

        // Adds itself to the map.
        public void EntityPlacedOnMap(GameMap map, Alpha.Util.Point pos)
        {
        }

        // Removes itself from the map.
        public void EntityRemovedFromMap(GameMap map, Alpha.Util.Point pos)
        {
            if (Node != null) {
                Node.Remove();
            }
            Entity.UnregisterListener(this);
        }

        // Ignore.
        public void EntityMoved(Alpha.Util.Point oldloc, Alpha.Util.Point newloc)
        {
        }

        // Ignore.
        public void EntityRemovedFromGame()
        {
        }


        // Returns the entity's bounds.
        public override BoundingBox GetBounds()
        {
            return Drawer.GetBounds(Entity, Player);
        }

        // Forwarded.
        public override void DrawOrtho(BoundingBox box)
        {
            Drawer.DrawOrtho(Entity, Player, box);
        }

        // Forwarded.
        public int GetDrawLayer()
        {
            return Drawer.GetDrawLayer(Entity, Player);
        }

        // Forwarded.
        public double GetDrawLevel()
        {
            return Drawer.GetDrawLevel(Entity, Player);
        }
    }
}
