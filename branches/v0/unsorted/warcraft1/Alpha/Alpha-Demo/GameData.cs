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

using System.Drawing;
using System;

using Alpha.Game;
using Alpha.Graphics;

// Stats for a type.
//[Serializable]
public class UnitType: GameDataObject, IEntityFactory
{
    // Size of the entity.
    public int Size;

    // Speed of the entity.
    public int Speed;

    // Actor factory
    public IActorFactory ActorFactory;

    public Entity CreateEntity(GameState state, Player player)
    {
        Unit unit;

        unit = new Unit(state, player, this);
        unit.Cacher.Size = Size;
        ((TileLocator)unit.Locator).Size = Size;

        return unit;
    }

    public UnitType(Alpha.Game.GameData data): base(data)
    {
    }
}

// Unit stuff
public class Unit: Alpha.Game.Entity
{
    // Type information
    public readonly UnitType Type;

    // Get the entity's speed
    public override int GetSpeed()
    {
        return Type.Speed;
    }

    // Constructor.
    public Unit(GameState state, Player p, UnitType type): base(state, p)
    {
        this.Type = type;
        CreateLocator();
        Locator.Transparent = false;
        ((TileLocator)Locator).Size = Type.Size;
        Actor = Type.ActorFactory.CreateActor(this);
        CreateOrderQueue(new IdleOrder());
    }
}

// This class contains everything related to gameplay.
public class GameData: Alpha.Game.GameData
{
    // The tileset
    public HeightMapTileset Tileset;

    // The actual unit types.
    public UnitType SmallUnit;
    public UnitType BigUnit;

    // This constructor does a lot of initialization.
    public GameData()
    {
        TilePathfinder pf;
        
        Pathfinder = pf = new TilePathfinder(this);
        pf.Smooth = true;
        pf.MediumCount = 1;
        pf.LargestEntity = 5;

        Tileset = new HeightMapTileset(this);
        Tileset.TileSize = 1000;

        CreateUnitTypes();
    }

    private void CreateUnitTypes()
    {
        SmallUnit = new UnitType(this);
        SmallUnit.Size = 400;
        SmallUnit.Speed = 250;
        SmallUnit.ActorFactory = new SimpleActorType(this);
        ((SimpleActorType)SmallUnit.ActorFactory).DrawTag = 0;
        
        BigUnit = new UnitType(this);
        BigUnit.Size = 800;
        BigUnit.Speed = 50;
        BigUnit.ActorFactory = new SimpleActorType(this);
        ((SimpleActorType)BigUnit.ActorFactory).DrawTag = 1;
    }
}
