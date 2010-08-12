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

using System.Drawing;
using System;

using Alpha.Graphics;
using Alpha.Scene;
using Alpha.Game;

public class GameGraphicFactory: IGameGraphicFactory
{
    private IEntityDrawer[] EntityDrawers;

    private BlendHeightMapDrawer MapDrawer; 

    public IGraphicObject CreateEntityGraphic(Entity entity, Player player)
    {
        int tag;
        tag = entity.Actor.DrawTag;

        if (EntityDrawers == null || tag < 0 || tag >= EntityDrawers.Length || EntityDrawers[tag] == null) {
            return null;
        } else {
            return new EntityDrawerGraphic(entity, player, EntityDrawers[tag]);
        }
    }

    public IGraphicObject CreateMapGraphic(GameMap map, Player player)
    {
        return new MapDrawerGraphic(map, player, MapDrawer);
    }

    public IMapSceneBuilder CreateSceneBuilder(GameMap map, Player player)
    {
        return new MapSceneBuilder(this, map, player);
    }

    public GameGraphicFactory()
    {
        EntityDrawers = new IEntityDrawer[2];

        CubeEntityDrawer drawer;

        drawer = new CubeEntityDrawer();
        drawer.Size = 350;
        drawer.InsideColor = Color.FromArgb(255, 255, 32, 0);
        drawer.OutsideColor = Color.FromArgb(0, 0, 32, 255);
        EntityDrawers[0] = drawer;

        drawer = new CubeEntityDrawer();
        drawer.Size = 700;
        drawer.InsideColor = Color.FromArgb(255, 0, 32, 255);
        drawer.OutsideColor = Color.FromArgb(0, 255, 32, 0);
        EntityDrawers[1] = drawer;

        MapDrawer = new BlendHeightMapDrawer();
        MapDrawer.LowTexture = new Texture(new Bitmap("Data/Grass.jpg"));
        MapDrawer.HighTexture = new Texture(new Bitmap("Data/Dirt.jpg"));
        MapDrawer.LowTexture.XScale = MapDrawer.LowTexture.YScale = 0.1;
        MapDrawer.HighTexture.XScale = MapDrawer.HighTexture.YScale = 0.4;
        MapDrawer.LowPoint = 30;
        MapDrawer.HighPoint = 60;
    }
}
