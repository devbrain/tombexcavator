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
using System.IO;
using System.Drawing;
using System.Collections;

using Alpha.Util;
using Alpha.Graphics;
using Alpha.UI;
using Alpha.Game;
using Alpha.Scene;

public class GameGraphicFactory: IGameGraphicFactory
{
    // Standard orientation infos.
    private SpriteOrienter Snap5Orienter;
    private SpriteOrienter Smooth5Orienter;
    private SpriteOrienter NullOrienter;

    // Map drawers for every tileset.
    private TypedTileMapDrawer GrassTilesetDrawer;
    private TypedTileMapDrawer SwampTilesetDrawer;
    private TypedTileMapDrawer DungeonTilesetDrawer;

    // Entity drawers, index by DrawTag.
    private EntityDrawer[] EntityDrawers;

    // Creates graphic for a certain entity.
    public IGraphicObject CreateEntityGraphic(Entity entity, Player player)
    {
        int tag;
        if (entity.Actor == null) {
            return null;
        }
        tag = entity.Actor.DrawTag;

        //Console.WriteLine("Create graphic tag {0}", tag);
        if (EntityDrawers == null || tag < 0 || tag >= EntityDrawers.Length || EntityDrawers[tag] == null) {
            return null;
        } else {
            return new EntityDrawerGraphic(entity, player, EntityDrawers[tag]);
        }
    }

    // Creates a graphic for the map.
    public IGraphicObject CreateMapGraphic(GameMap map, Player player)
    {
        if (map is SimpleTileMap) {
            SimpleTileMap smap = (SimpleTileMap)map;
            if (smap.Tileset == Global.GameData.GrassTileset) {
                return new MapDrawerGraphic(smap, player, GrassTilesetDrawer);
            }
            if (smap.Tileset == Global.GameData.SwampTileset) {
                return new MapDrawerGraphic(smap, player, SwampTilesetDrawer);
            }
            if (smap.Tileset == Global.GameData.DungeonTileset) {
                return new MapDrawerGraphic(smap, player, DungeonTilesetDrawer);
            }
        }
        return null;
    }

    public IMapSceneBuilder CreateSceneBuilder(GameMap map, Player player)
    {
        return new MapSceneBuilder(this, map, player);
    }

    // Constructor. Does a lot of initialization work.
    public GameGraphicFactory()
    {
        CreateOrienters();
        CreateEntityDrawers();
        CreateMapDrawers();
    }

    // Creates sprite orienters.
    private void CreateOrienters()
    {
        Snap5Orienter = new SpriteOrienter();
        Snap5Orienter.OrientationFrames = 5;
        Snap5Orienter.UpDirection = 90;
        Snap5Orienter.ClockwiseFrames = true;
        Snap5Orienter.Mirrored = true;
        Snap5Orienter.RotationAdjustment = false;

        Smooth5Orienter = new SpriteOrienter();
        Smooth5Orienter.OrientationFrames = 5;
        Smooth5Orienter.UpDirection = 90;
        Smooth5Orienter.ClockwiseFrames = true;
        Smooth5Orienter.Mirrored = true;
        Smooth5Orienter.RotationAdjustment = true;

        NullOrienter = new SpriteOrienter();
        NullOrienter.OrientationFrames = 1;
        NullOrienter.RotationAdjustment = false;
    }

    // Creates a single EntityDrawer. Used in initialization code.
    private static EntityDrawer CreateDrawer(int sprite,
            SpriteOrienter orienter, int layer)
    {
        EntityDrawer drawer;
        int i;

        drawer = new EntityDrawer();
        drawer.SpriteOrienter = orienter;
        drawer.DrawLayer = layer;

        ILazyLink res = Global.Resources.GetLink("anim-" + sprite);
        drawer.Sprites = new ILazyLink[(int)Tileset.Max];
        for (i = 0; i < (int)Tileset.Max; ++i) {
            drawer.Sprites[i] = Global.Resources.Remap(res, (Tileset)i);
        }

        return drawer;
    }

    // Create the EntityDrawers array.
    private void CreateEntityDrawers()
    {
        EntityDrawers = new EntityDrawer[64];

        EntityDrawers[0] = CreateDrawer(279, Snap5Orienter, 0);
        EntityDrawers[1] = CreateDrawer(280, Snap5Orienter, 0);
        EntityDrawers[2] = CreateDrawer(281, Snap5Orienter, 0);
        EntityDrawers[3] = CreateDrawer(282, Snap5Orienter, 0);
        EntityDrawers[4] = CreateDrawer(283, Smooth5Orienter, 0);
        EntityDrawers[5] = CreateDrawer(284, Smooth5Orienter, 0);
        EntityDrawers[6] = CreateDrawer(285, Snap5Orienter, 0);
        EntityDrawers[7] = CreateDrawer(286, Snap5Orienter, 0);
        EntityDrawers[8] = CreateDrawer(287, Snap5Orienter, 0);
        EntityDrawers[9] = CreateDrawer(288, Snap5Orienter, 0);
        EntityDrawers[10] = CreateDrawer(289, Snap5Orienter, 0);
        EntityDrawers[11] = CreateDrawer(290, Snap5Orienter, 0);
        EntityDrawers[12] = CreateDrawer(291, Snap5Orienter, 0);
        EntityDrawers[13] = CreateDrawer(292, Snap5Orienter, 0);
        EntityDrawers[14] = CreateDrawer(293, Snap5Orienter, 0);
        EntityDrawers[15] = CreateDrawer(294, Snap5Orienter, 0);
        EntityDrawers[16] = CreateDrawer(295, Snap5Orienter, 0);
        EntityDrawers[17] = CreateDrawer(296, Snap5Orienter, 0);
        EntityDrawers[18] = CreateDrawer(296, Snap5Orienter, 0);
        EntityDrawers[19] = CreateDrawer(297, Snap5Orienter, 0);
        EntityDrawers[20] = CreateDrawer(297, Snap5Orienter, 0);
        EntityDrawers[21] = CreateDrawer(298, Snap5Orienter, 0);
        EntityDrawers[22] = CreateDrawer(299, Snap5Orienter, 0);
        EntityDrawers[23] = CreateDrawer(300, Snap5Orienter, 0);
        EntityDrawers[24] = CreateDrawer(301, Snap5Orienter, 0);
        EntityDrawers[25] = CreateDrawer(302, Snap5Orienter, 0);
        EntityDrawers[26] = CreateDrawer(303, Snap5Orienter, 0);
        EntityDrawers[27] = CreateDrawer(304, Snap5Orienter, 0);
        EntityDrawers[28] = CreateDrawer(305, Snap5Orienter, 0);
        EntityDrawers[29] = CreateDrawer(305, Snap5Orienter, 0);
        EntityDrawers[30] = CreateDrawer(305, Snap5Orienter, 0);
        EntityDrawers[31] = CreateDrawer(306, Snap5Orienter, 0);

        // Buildings
        EntityDrawers[32] = CreateDrawer(307, NullOrienter, 0);
        EntityDrawers[33] = CreateDrawer(308, NullOrienter, 0);
        EntityDrawers[34] = CreateDrawer(309, NullOrienter, 0);
        EntityDrawers[35] = CreateDrawer(310, NullOrienter, 0);
        EntityDrawers[36] = CreateDrawer(311, NullOrienter, 0);
        EntityDrawers[37] = CreateDrawer(312, NullOrienter, 0);
        EntityDrawers[38] = CreateDrawer(313, NullOrienter, 0);
        EntityDrawers[39] = CreateDrawer(314, NullOrienter, 0);
        EntityDrawers[40] = CreateDrawer(315, NullOrienter, 0);
        EntityDrawers[41] = CreateDrawer(316, NullOrienter, 0);
        EntityDrawers[42] = CreateDrawer(317, NullOrienter, 0);
        EntityDrawers[43] = CreateDrawer(318, NullOrienter, 0);
        EntityDrawers[44] = CreateDrawer(319, NullOrienter, 0);
        EntityDrawers[45] = CreateDrawer(320, NullOrienter, 0);
        EntityDrawers[46] = CreateDrawer(321, NullOrienter, 0);
        EntityDrawers[47] = CreateDrawer(322, NullOrienter, 0);
        EntityDrawers[48] = CreateDrawer(323, NullOrienter, 0);
        EntityDrawers[49] = CreateDrawer(324, NullOrienter, 0);
        EntityDrawers[50] = CreateDrawer(325, NullOrienter, 0);

        // Special effect entities.
        EntityDrawers[51] = CreateDrawer(326, Snap5Orienter, 0);
        EntityDrawers[52] = CreateDrawer(347, Smooth5Orienter, 50);
        EntityDrawers[53] = CreateDrawer(348, Smooth5Orienter, 50);
        EntityDrawers[54] = CreateDrawer(349, Smooth5Orienter, 50);
        EntityDrawers[55] = CreateDrawer(350, NullOrienter, 60);
        EntityDrawers[56] = CreateDrawer(351, NullOrienter, 70);
        EntityDrawers[57] = CreateDrawer(352, NullOrienter, 50);
        EntityDrawers[58] = CreateDrawer(353, NullOrienter, 50);
        EntityDrawers[59] = CreateDrawer(354, NullOrienter, 50);
        EntityDrawers[60] = CreateDrawer(355, NullOrienter, 50);
        EntityDrawers[61] = CreateDrawer(356, NullOrienter, 50);
        EntityDrawers[62] = CreateDrawer(357, Smooth5Orienter, 50);
        EntityDrawers[63] = CreateDrawer(358, Smooth5Orienter, 50);

        // Hack: Peasant/Peon with wood.
        EntityDrawers[(int)UnitTypeId.Peon].GoldCarrySprite = Global.Resources.GetLink("anim-330");
        EntityDrawers[(int)UnitTypeId.Peon].WoodCarrySprite = Global.Resources.GetLink("anim-328");
        EntityDrawers[(int)UnitTypeId.Peasant].GoldCarrySprite = Global.Resources.GetLink("anim-329");
        EntityDrawers[(int)UnitTypeId.Peasant].WoodCarrySprite = Global.Resources.GetLink("anim-327");
    }

    // This creates a map drawer for every tileset.
    public void CreateMapDrawers()
    {
        TypedTileMapDrawer drawer;
        ILazyLink tileimages;
        int i;

        GrassTilesetDrawer = drawer = new TypedTileMapDrawer();
        drawer.Textures = new ILazyLink[Global.GameData.GrassTileset.Tiles.Count];
        tileimages = new TilesetLink(false, Global.Archive, 190, Global.Resources.GetLink("grass-main-pal"));
        for (i = 0; i < drawer.Textures.Length; ++i) {
            drawer.Textures[i] = new ArrayElementLink(false, tileimages, i);
        }

        SwampTilesetDrawer = drawer = new TypedTileMapDrawer();
        drawer.Textures = new ILazyLink[Global.GameData.SwampTileset.Tiles.Count];
        tileimages = new TilesetLink(false, Global.Archive, 193, Global.Resources.GetLink("swamp-main-pal"));
        for (i = 0; i < drawer.Textures.Length; ++i) {
            drawer.Textures[i] = new ArrayElementLink(false, tileimages, i);
        }

        DungeonTilesetDrawer = drawer = new TypedTileMapDrawer();
        drawer.Textures = new ILazyLink[Global.GameData.DungeonTileset.Tiles.Count];
        tileimages = new TilesetLink(false, Global.Archive, 196, Global.Resources.GetLink("dungeon-main-pal"));
        for (i = 0; i < drawer.Textures.Length; ++i) {
            drawer.Textures[i] = new ArrayElementLink(false, tileimages, i);
        }

    }
}

// WC1 entity drawer.
public class EntityDrawer: SpriteEntityDrawerBase
{
    // Sprites, indexed by tileset.
    private ILazyLink[] sprites;
    public ILazyLink[] Sprites {
        get {
            return sprites;
        }
        set {
            sprites = value;
        }
    }

    // HACK: Sprites used when carrying wood/gold.
    public ILazyLink GoldCarrySprite;
    public ILazyLink WoodCarrySprite;

    // Sprite orientation info.
    private SpriteOrienter spriteOrienter;
    public SpriteOrienter SpriteOrienter {
        get {
            return spriteOrienter;
        }
        set {
            spriteOrienter = value;
        }
    }

    // For GetDrawLayer().
    private int drawLayer;
    public int DrawLayer {
        get {
            return drawLayer;
        }
        set {
            drawLayer = value;
        }
    }

    // Get the sprite for a certain frame.
    // FIXME: SUX.
    public override Sprite GetSprite(Entity entity, Player player)
    {
        if (entity.OffMap) {
            throw new InvalidOperationException("Entity is OffMap");
        }

        // Hack for peasant/peons with resources.
        if (entity is Unit) {
            if (GoldCarrySprite != null && ((Unit)entity).GoldAmount > 0) {
                return (Sprite)GoldCarrySprite.Get();
            }
            if (WoodCarrySprite != null && ((Unit)entity).WoodAmount > 0) {
                return (Sprite)WoodCarrySprite.Get();
            }
        }

        Tileset tset = ((WarGameData)entity.Map.Data).GetMapTilesetEnum(entity.Map);
        return (Sprite)Sprites[(int)tset].Get();
    }

    // Shared, forward to EntityGraphicType.
    public override SpriteOrienter GetSpriteOrienter(Entity entity, Player player)
    {
        return SpriteOrienter;
    }

    // Lots of tiny hacks here, RTFC
    public override Color GetColor(Entity entity, Player player)
    {
        Color col = Color.White;
        Unit unit = entity as Unit;

        // Invulnerability is red.
        if (unit != null && unit.InvulnerabilityTicks > 0) {
            col = Color.FromArgb(col.A, col.R, 0, 0);
        }
        // Invisibility is alpha.
        if (unit != null && unit.InvisibilityTicks > 0) {
            col = Color.FromArgb(127, col.R, col.G, col.B);
        }
        // Poison cloud is alpha.
        if (entity.Actor.DrawTag == (int)(WarGameData.UnitCount + EffectId.PoisonCloud)) {
            col = Color.FromArgb(127, col.R, col.G, col.B);
        }

        return col;
    }

    // Fixed scaling
    public override double GetScale(Entity entity, Player player)
    {
        return WarGameData.TileSize / 16.0;
    }

    // Get Draw layer.
    public override int GetDrawLayer(Entity entity, Player player)
    {
        // Hack for corpses.
        if (entity is Unit && ((Unit)entity).Dead) {
            return -10;
        } else {
            return DrawLayer;
        }
    }
}
