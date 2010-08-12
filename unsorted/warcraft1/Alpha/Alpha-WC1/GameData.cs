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
using System.IO;
using System.Drawing;
using System.Collections;

using Alpha.Util;
using Alpha.Graphics;
using Alpha.UI;
using Alpha.Game;
using Alpha.Scene;

// The game data.
public class WarGameData: GameData
{
    // How many units are soldiers (moving units).
    public const int SoldierCount = 32;

    // How many entities are buildings (stationary units).
    public const int BuildingCount = 19;

    // Hoe many entities are units.
    public const int UnitCount = SoldierCount + BuildingCount;

    // How many entities are effects (projectiles/clouds).
    // HACK: Includes second corpse.
    public const int EffectCount = 13;

    // How many Entities there are in total.
    public const int EntityCount = UnitCount + EffectCount;

    // Size of a tile.
    public const int TileSize = 400;

    // The game's tilesets.
    public SimpleTileMapTileset GrassTileset;
    public SimpleTileMapTileset SwampTileset;
    public SimpleTileMapTileset DungeonTileset;

    // Array of unit actors, index by UnitEnum.
    public AnimatedSpriteActorType[] UnitActors;

    // Array of effect actor, index by EffectId.
    public AnimatedSpriteActorType[] EffectActors;

    // Array of unit types, indexed just like in WC1.
    // You can index by UnitEnum.
    public UnitType[] Units;

    // Used by the firedrop spell.
    public MissileType FireDropMissile;

    // Various clouds/effects
    public CloudType SparksCloud;
    public CloudType ExplosionCloud;
    public CloudType DemolitionCloud;
    public CloudType PoisonCloud;

    // Upgrade activityes, indexed by ActivityId.
    // FIXME: confusing.
    public UpgradeActivity[] Upgrades;

    // Spells/Abilities.
    public AttackAbility AttackAbility;
    public SummonSpell PoisonCloudSpell;
    public SummonSpell FireRainSpell;
    public SummonSpell SummonSpidersSpell;
    public SummonSpell SummonScorpionsSpell;
    public SummonSpell SummonElementalSpell;
    public SummonSpell SummonDaemonSpell;
    public HealSpell HealSpell;
    public RaiseDeadSpell RaiseDeadSpell;
    public InvisibilitySpell InvisibilitySpell;
    public UnholyArmorSpell UnholyArmorSpell;

    // Orders, misc.
    public Order GuardOrder;
    public Order IdleOrder;
    public ITargetFinder TargetFinder;

    // Load the gamedata. 
    public WarGameData()
    {
        // Basic stuff.
        CacheTileSize = TileSize / 2;

        TilePathfinder pf;
        Pathfinder = pf = new TilePathfinder(this);
        pf.TileSize = TileSize / 2;
        pf.SquareHack = false;
        pf.MediumCount = 1;
        pf.LargestEntity = (TileSize + pf.TileSize - 1) / pf.TileSize;
//        pf.LargestEntity = 10;
        pf.Smooth = true;
        pf.SqueezeMode = PathfinderSqueeze.Full;

        // Performance tuning.
        pf.MaxSearchNodes = 5000;
        pf.MaxNonImprovingSearchNodes = 500;
        pf.MinRecheckTime = 50;
        pf.MaxRecheckTime = 100;
        pf.MinFailWaitTime = 20;
        pf.MaxFailWaitTime = 30;
        pf.MinStallWaitTime = 5;
        pf.MaxStallWaitTime = 10;
        pf.MaxAllowedWaitTime = 50;
        pf.MaxAllowedWaitTime = 100;

        pf.MaxSearchNodesPerCycle = 10000;
        pf.HeuristicDistance = 4 * pf.LargestEntity;
        pf.PermBlockDistance = 1; //1 * pf.LargestEntity;

        // Loading various objects is split into multiple functions
        // Loading order is important.
        LoadTilesets();
        LoadActors();
        LoadActions();
        LoadUnits();
        LoadUpgrades();
        LoadSpells();

        //PrintAnimations("anims.xhtml");
    }

/*    // PrettyPrint animations to a file.
    private void PrintAnimations(string filename)
    {
        ScriptPrettyPrinter printer = new ScriptPrettyPrinter();
        ScriptNode[][] eps = WarArchive.GetAnimations(Global.Archive.GetChunk(212));
        
        printer.EntryPoints = eps;
        printer.Title = "WC1 animations";
        printer.GenerateNames();
        string[] animNames;

        int i;

        // Missiles
        animNames = new string[] {
            "{0}-0", "{0}-1", "{0}-hit",
        };
        for (i = WarArchive.MissileAnimCount - 1; i > 0; --i) {
            printer.AnimationNames[WarArchive.UnitAnimCount + i] = "missile-" + i;
            printer.SetEntryPointNames(WarArchive.UnitAnimCount + i, animNames);
        }

        // Units
        animNames = new string[] {
            "{0}-unknown", "{0}-death", "{0}-idle", "{0}-move", "{0}-action", "{0}-spawn",
        };
        for (i = WarArchive.UnitAnimCount - 1; i >= 0; --i) {
            printer.AnimationNames[i] = "unit-" + ((UnitEnum)i).ToString();
            printer.SetEntryPointNames(i, animNames);
        }

        // Special
        printer.AnimationNames[WarArchive.UnitAnimCount] = "corpse";
        printer.NamedNodes[eps[50][5]] = "short-empty-loop";
        printer.NamedNodes[eps[51][0]] = "long-mistery-loop";
        printer.NamedNodes[eps[51][1]] = "human-corpse";
        printer.NamedNodes[eps[51][2]] = "orc-corpse";
        printer.Print(filename);
    }*/

    // Load actions/abilities/clouds/etc.
    private void LoadActions()
    {
        // Missiles/Clouds/etc.
        FireDropMissile = CreateMissile(EffectId.FireDrop);
        SparksCloud = new CloudType(this, EffectActors[(int)EffectId.Sparks], true, -1);
        ExplosionCloud = new CloudType(this, EffectActors[(int)EffectId.Explosion], true, -1);
        DemolitionCloud = new CloudType(this, EffectActors[(int)EffectId.Demolition], true, -1);
        PoisonCloud = new PoisonCloudType(this, EffectActors[(int)EffectId.PoisonCloud]);

        // Attacking.
        AttackAbility = new AttackAbility(this);
        TargetFinder = new TargetFinder(this);

        // Orders ick.
        GuardOrder = new HoldPositionOrder();
        IdleOrder = new IdleOrder();
    }

    // Loads spells. They needs units to be loaded.
    private void LoadSpells()
    {
        // Cleric/Necrolyte spells fill themselves up in their constructors.
        HealSpell = new HealSpell(this);
        HealSpell.UpgradeRequired = UpgradeId.Heal;
        RaiseDeadSpell = new RaiseDeadSpell(this);
        RaiseDeadSpell.UpgradeRequired = UpgradeId.RaiseDead;
        UnholyArmorSpell = new UnholyArmorSpell(this);
        UnholyArmorSpell.UpgradeRequired = UpgradeId.UnholyArmor;
        InvisibilitySpell = new InvisibilitySpell(this);
        InvisibilitySpell.UpgradeRequired = UpgradeId.Invisibility;

        // Conjurer spells.
        FireRainSpell = new SummonSpell(this);
        FireRainSpell.Range = 9 * TileSize;
        FireRainSpell.TargetSpawn = null;
        FireRainSpell.MultiSummon = false;
        FireRainSpell.RepeatCast = true;
        FireRainSpell.EntityFactory = new FireRainFactory();
        FireRainSpell.SpellPointCost = 50;
        FireRainSpell.SetNeedsTarget(true);
        FireRainSpell.CasterType = Units[(int)UnitTypeId.Conjurer];
        FireRainSpell.UpgradeRequired = UpgradeId.FireRain;

        SummonScorpionsSpell = new SummonSpell(this);
        SummonScorpionsSpell.EntityFactory = Units[(int)UnitTypeId.Scorpion];
        SummonScorpionsSpell.SpellPointCost = 60;
        SummonScorpionsSpell.CasterType = Units[(int)UnitTypeId.Conjurer];
        SummonScorpionsSpell.UpgradeRequired = UpgradeId.SummonScorpions;
        SummonScorpionsSpell.UpgradeRequired = UpgradeId.SummonScorpions;

        SummonElementalSpell = new SummonSpell(this);
        SummonElementalSpell.EntityFactory = Units[(int)UnitTypeId.WaterElemental];
        SummonElementalSpell.SpellPointCost = 255;
        SummonElementalSpell.CasterType = Units[(int)UnitTypeId.Conjurer];
        SummonElementalSpell.UpgradeRequired = UpgradeId.SummonElemental;

        // Warlock spells.
        PoisonCloudSpell = new SummonSpell(this);
        PoisonCloudSpell.Range = 5 * TileSize;
        PoisonCloudSpell.TargetSpawn = null;
        PoisonCloudSpell.MultiSummon = false;
        PoisonCloudSpell.EntityFactory = PoisonCloud;
        PoisonCloudSpell.SpellPointCost = 50;
        PoisonCloudSpell.SetNeedsTarget(true);
        PoisonCloudSpell.CasterType = Units[(int)UnitTypeId.Warlock];
        PoisonCloudSpell.UpgradeRequired = UpgradeId.PoisonCloud;

        SummonDaemonSpell = new SummonSpell(this);
        SummonDaemonSpell.EntityFactory = Units[(int)UnitTypeId.Daemon];
        SummonDaemonSpell.SpellPointCost = 255;
        SummonDaemonSpell.CasterType = Units[(int)UnitTypeId.Warlock];
        SummonDaemonSpell.UpgradeRequired = UpgradeId.SummonDaemon;

        SummonSpidersSpell = new SummonSpell(this);
        SummonSpidersSpell.EntityFactory = Units[(int)UnitTypeId.Spider];
        SummonSpidersSpell.SpellPointCost = 60;
        SummonSpidersSpell.CasterType = Units[(int)UnitTypeId.Warlock];
        SummonSpidersSpell.UpgradeRequired = UpgradeId.SummonSpiders;
    }

    // Load the tilesets.
    private void LoadTilesets()
    {
        int i;
        SimpleTileMapTileset tileset;
        
        GrassTileset = tileset = new SimpleTileMapTileset(this);
        tileset.TileSize = TileSize;
        tileset.Tiles.Capacity = 311;
        for (i = 0; i < 311; ++i) {
            tileset.Tiles.Add(new SimpleTileData());
        }

        SwampTileset = tileset = new SimpleTileMapTileset(this);
        tileset.TileSize = TileSize;
        tileset.Tiles.Capacity = 283;
        for (i = 0; i < 283; ++i) {
            tileset.Tiles.Add(new SimpleTileData());
        }

        DungeonTileset = tileset = new SimpleTileMapTileset(this);
        tileset.TileSize = TileSize;
        tileset.Tiles.Capacity = 338;
        for (i = 0; i < 338; ++i) {
            tileset.Tiles.Add(new SimpleTileData());
        }
    }

    private UnitType NewUnit(string name, int id, int icon, int hp,
            int defense, int range, int damage, int pierce, bool race, bool magic)
    {
        UnitType type;

        Units[id] = type = new UnitType(this);
        type.Size = 1;
        type.Ident = (UnitTypeId)id;
        type.Name = name;
        type.Icon = Global.Resources.GetLink(String.Format("icon-{0}", icon));
        type.Speed = TileSize / 16;
        type.MaxHitPoints = hp;
        type.Defense = defense;
        type.Building = false;
        type.NormalDamage = damage;
        type.PierceDamage = pierce;
        type.CanAttack = damage != 0 || pierce != 0;
        type.SpellCaster = magic;
        type.Race = race ? Race.Human : Race.Orc;
        type.AttackRange = (range - 1) * TileSize;
        type.ActorFactory = UnitActors[id];
        type.SecondaryCorpse = true;

        return type;
    }

    private void SetUnitTrain(UnitTypeId id, UnitTypeId builder, AllowFeature allow)
    {
        UnitType type = Units[(int)id];
        type.TrainActivity = new TrainActivity(this);
        type.TrainActivity.TrainedUnitType = type;
        type.TrainActivity.BuilderUnitType = Units[(int)builder];
        type.TrainActivity.AllowTest = allow;
    }

    private void SetUnitCost(UnitTypeId id, int gold, int wood, int time)
    {
        UnitType type = Units[(int)id];
        type.TrainActivity.GoldCost = gold;
        type.TrainActivity.WoodCost = wood;
        type.TrainActivity.Duration = time;
    }

    // Create a new building (and register it).
    private UnitType NewBuilding(UnitTypeId id, string name, int icon, int hp, int size)
    {
        UnitType type;

        Units[(int)id] = type = new UnitType(this);
        type.Size = size;
        type.Ident = id;
        type.Name = name;
        type.Icon = Global.Resources.GetLink(String.Format("icon-{0}", icon));
        type.Speed = 0;
        type.MaxHitPoints = hp;
        type.Building = true;
        type.CanRaiseDead = false;
        type.DeathSpawn = DemolitionCloud;
        type.CanAttack = false;
        type.ActorFactory = UnitActors[(int)id];

        return type;
    }

    // Create a trivial missile with a certain actor.
    private MissileType CreateMissile(EffectId missile)
    {
        return new MissileType(this, EffectActors[(int)missile], 4 * TileSize / 16);
    }

    // Load the entities.
    private void LoadUnits()
    {
        UnitType u;

        Units = new UnitType[51];

        // Here are buildings. See NewBuilding
        //          Name            Id                          Icon    HP      Size
        NewBuilding(UnitTypeId.HumanFarm,       "Farm",         14,     400,    2);
        NewBuilding(UnitTypeId.OrcFarm,         "Farm",         15,     400,    2);
        NewBuilding(UnitTypeId.HumanBarracks,   "Barracks",     16,     800,    3);
        NewBuilding(UnitTypeId.OrcBarracks,     "Barracks",     17,     800,    3);
        NewBuilding(UnitTypeId.Church,          "Church",       28,     700,    3);
        NewBuilding(UnitTypeId.Temple,          "Temple",       29,     700,    3);
        NewBuilding(UnitTypeId.HumanTower,      "Tower",        18,     900,    2);
        NewBuilding(UnitTypeId.OrcTower,        "Tower",        19,     900,    2);
        NewBuilding(UnitTypeId.HumanTownHall,   "Town Hall",    20,     2500,   3);
        NewBuilding(UnitTypeId.OrcTownHall,     "Town Hall",    21,     2500,   3);
        NewBuilding(UnitTypeId.HumanMill,       "Mill",         22,     600,    3);
        NewBuilding(UnitTypeId.OrcMill,         "Mill",         23,     600,    3);
        NewBuilding(UnitTypeId.Stables,         "Stables",      24,     500,    3);
        // This is actuall 2 * 3, but it's not supported.
        NewBuilding(UnitTypeId.Kennel,          "Kennel",       25,     500,    3);
        NewBuilding(UnitTypeId.HumanSmith,      "Blacksmith",   26,     800,    2);
        NewBuilding(UnitTypeId.OrcSmith,        "Blacksmith",   27,     800,    2);
        NewBuilding(UnitTypeId.Stormwind,       "Stormwind",    31,     5000,   4);
        NewBuilding(UnitTypeId.BlackRock,       "Black Rock",   32,     5000,   4);
        NewBuilding(UnitTypeId.GoldMine,        "Gold Mine",    30,     25500,  3);

        // Here are the units. See NewUnit
        // Entities called Dragon are not really present in the game
        // Also, Griselda and Garona share a sprite.
        // Name, Id, Icon, HP, Defense, Range, Damage, Pierce, Human, Magic
        u = NewUnit("Footman",   0, 0,  60,  2,  1, 9,  1,  true,  false);
        u = NewUnit("Grunt",     1, 1,  60,  2,  1, 9,  1,  false, false);
        u = NewUnit("Peasant",   2, 4,  40,  0,  1, 0,  0,  true,  false);
        u = NewUnit("Peon",      3, 5,  40,  0,  1, 0,  0,  false, false);

        u = NewUnit("Catapult",  4, 6,  120, 0,  8, 255, 0,  true,  false);
        u.AttackMissile = CreateMissile(EffectId.Fireball);
        u.AttackHitSpawn = ExplosionCloud;
        u = NewUnit("Catapult",  5, 7,  120, 0,  8, 255, 0,  false, false);
        u.AttackMissile = CreateMissile(EffectId.Fireball);
        u.AttackHitSpawn = ExplosionCloud;

        u = NewUnit("Knight",    6, 8,  90,  5,  1, 13, 1,  true,  false);
        u = NewUnit("Raider",    7, 9,  90,  5,  1, 13, 1,  false, false);

        u = NewUnit("Archer",    8, 10, 60,  1,  5, 0,  4,  true,  false);
        u.AttackMissile = CreateMissile(EffectId.Arrow);
        u = NewUnit("Spearman",  9, 11, 60,  1,  4, 0,  5,  false, false);
        u.AttackMissile = CreateMissile(EffectId.Arrow);

        u = NewUnit("Conjurer",  10, 2,  40,  0,  3, 0,  6,  true,  true);
        u.AttackMissile = CreateMissile(EffectId.EnergyBall);
        u = NewUnit("Warlock",   11, 3,  40,  0,  2, 0,  6,  false, true);
        u.AttackMissile = CreateMissile(EffectId.EnergyBall);
        u = NewUnit("Cleric",    12, 12, 40,  0,  1, 0,  6,  true,  true);
        u.AttackMissile = CreateMissile(EffectId.EnergyBall);
        u = NewUnit("Necrolyte", 13, 13, 40,  0,  2, 0,  6,  false, true);
        u.AttackMissile = CreateMissile(EffectId.EnergyBall);
        u = NewUnit("Medivh",    14, 54, 150, 0,  5, 0,  10, false, true);
        u.AttackMissile = CreateMissile(EffectId.EnergyBall);

        u = NewUnit("Lothar",    15, 53, 50,  5,  1, 15, 1,  true,  false);
        u = NewUnit("Wounded",   16, 10, 40,  0,  1, 0,  0,  true,  false);
        u = NewUnit("Griselda",  17, 56, 30,  0,  1, 0,  0,  false, false);
        u = NewUnit("Garona",    18, 55, 30,  0,  1, 0,  0,  false, false);
        u = NewUnit("Ogre",      19, 44, 60,  3,  1, 12, 1,  false, false);
        u = NewUnit("Spider",    21, 45, 30,  0,  1, 3,  1,  false, false);
        u = NewUnit("Slime",     22, 46, 150, 10, 1, 0,  1,  false, false);
        u = NewUnit("Elemental", 23, 47, 200, 0,  1, 40, 0,  false, false);
        u.DeathSpawn = ExplosionCloud;
        u.AttackHitSpawn = ExplosionCloud;
        u = NewUnit("Scorpion",  24, 48, 30,  0,  1, 0,  3,  false, false);
        u = NewUnit("Brigand",   25, 58, 40,  4,  1, 9,  1,  true,  false);
        u = NewUnit("Skeleton",  26, 49, 40,  1,  1, 4,  1,  false, false);
        u = NewUnit("Skeleton",  27, 50, 30,  2,  1, 9,  1,  false, false);
        u = NewUnit("Daemon",    28, 51, 300, 0,  1, 65, 0,  false, false);
        u = NewUnit("Elemental", 31, 52, 250, 0,  3, 0,  40, false, false);
        u.AttackMissile = CreateMissile(EffectId.BlueFireball);

        // Small adjustments
        Units[(int)UnitTypeId.Wounded].Speed = 0;

        // Handle TTL units.
        Units[(int)UnitTypeId.Skeleton].LimitedTimeToLive = true;
        Units[(int)UnitTypeId.Scorpion].LimitedTimeToLive = true;
        Units[(int)UnitTypeId.Spider].LimitedTimeToLive = true;
        Units[(int)UnitTypeId.WaterElemental].LimitedTimeToLive = true;
        Units[(int)UnitTypeId.Daemon].LimitedTimeToLive = true;

        // Disable Secondary corpses.
        Units[(int)UnitTypeId.HumanCatapult].SecondaryCorpse = false;
        Units[(int)UnitTypeId.OrcCatapult].SecondaryCorpse = false;
        Units[(int)UnitTypeId.Scorpion].SecondaryCorpse = false;
        Units[(int)UnitTypeId.Spider].SecondaryCorpse = false;
        Units[(int)UnitTypeId.Slime].SecondaryCorpse = false;
        Units[(int)UnitTypeId.Skeleton].SecondaryCorpse = false;
        Units[(int)UnitTypeId.ScenarioSkeleton].SecondaryCorpse = false;
        Units[(int)UnitTypeId.WaterElemental].SecondaryCorpse = false;
        Units[(int)UnitTypeId.FireElemental].SecondaryCorpse = false;
        Units[(int)UnitTypeId.Daemon].SecondaryCorpse = false;

        // Set unit trainer and allow feature.
        SetUnitTrain(UnitTypeId.Footman,        UnitTypeId.HumanBarracks,   AllowFeature.TrainFootman);
        SetUnitTrain(UnitTypeId.Grunt,          UnitTypeId.OrcBarracks,     AllowFeature.TrainGrunt);
        SetUnitTrain(UnitTypeId.Peasant,        UnitTypeId.HumanTownHall,   AllowFeature.TrainPeasant);
        SetUnitTrain(UnitTypeId.Peon,           UnitTypeId.OrcTownHall,     AllowFeature.TrainPeon);
        SetUnitTrain(UnitTypeId.HumanCatapult,  UnitTypeId.HumanBarracks,   AllowFeature.TrainHumanCatapult);
        SetUnitTrain(UnitTypeId.OrcCatapult,    UnitTypeId.OrcBarracks,     AllowFeature.TrainOrcCatapult);
        SetUnitTrain(UnitTypeId.Knight,         UnitTypeId.HumanBarracks,   AllowFeature.TrainKnight);
        SetUnitTrain(UnitTypeId.Raider,         UnitTypeId.OrcBarracks,     AllowFeature.TrainRaider);
        SetUnitTrain(UnitTypeId.Archer,         UnitTypeId.HumanBarracks,   AllowFeature.TrainArcher);
        SetUnitTrain(UnitTypeId.Spearman,       UnitTypeId.OrcBarracks,     AllowFeature.TrainSpearman);
        SetUnitTrain(UnitTypeId.Conjurer,       UnitTypeId.HumanTower,      AllowFeature.TrainConjurer);
        SetUnitTrain(UnitTypeId.Warlock,        UnitTypeId.OrcTower,        AllowFeature.TrainWarlock);
        SetUnitTrain(UnitTypeId.Cleric,         UnitTypeId.Church,          AllowFeature.TrainCleric);
        SetUnitTrain(UnitTypeId.Necrolyte,      UnitTypeId.Temple,          AllowFeature.TrainNecrolyte);

        // Set unit train costs.                Gold    Wood    Time
        SetUnitCost(UnitTypeId.Footman,         400,    0,      600);
        SetUnitCost(UnitTypeId.Grunt,           400,    0,      600);
        SetUnitCost(UnitTypeId.Peasant,         400,    0,      750);
        SetUnitCost(UnitTypeId.Peon,            400,    0,      750);
        SetUnitCost(UnitTypeId.HumanCatapult,   900,    200,    1000);
        SetUnitCost(UnitTypeId.OrcCatapult,     900,    200,    1000);
        SetUnitCost(UnitTypeId.Knight,          850,    0,      800);
        SetUnitCost(UnitTypeId.Raider,          850,    0,      800);
        SetUnitCost(UnitTypeId.Archer,          450,    50,     700);
        SetUnitCost(UnitTypeId.Spearman,        450,    50,     600);
        SetUnitCost(UnitTypeId.Conjurer,        900,    0,      900);
        SetUnitCost(UnitTypeId.Warlock,         900,    0,      900);
        SetUnitCost(UnitTypeId.Cleric,          700,    0,      800);
        SetUnitCost(UnitTypeId.Necrolyte,       700,    0,      800);

        // Valid storage depots.
        Units[(int)UnitTypeId.HumanTownHall].IsStorage = true;
        Units[(int)UnitTypeId.OrcTownHall].IsStorage = true;
        Units[(int)UnitTypeId.BlackRock].IsStorage = true;
        Units[(int)UnitTypeId.Stormwind].IsStorage = true;
    }

    // Initialize an upgrade.
    private void NewUpgrade(ActivityId activityId, UpgradeId upgradeId, int level, AllowFeature feat)
    {
        Upgrades[(int)activityId] = new UpgradeActivity(this);
        Upgrades[(int)activityId].UpgradeIdent = upgradeId;
        Upgrades[(int)activityId].UpgradeLevel = level;
        Upgrades[(int)activityId].AllowTest = feat;
    }

    // Set upgrade building/cost.
    private void SetUpgCost(ActivityId activityId, UnitTypeId buildingId, int gold, int time)
    {
        Upgrades[(int)activityId].BuilderUnitType = Units[(int)buildingId];
        Upgrades[(int)activityId].GoldCost = gold;
        Upgrades[(int)activityId].Duration = time;
    }

    private void LoadUpgrades()
    {
        Upgrades = new UpgradeActivity[(int)ActivityId.Max];

        // Create upgrades.
        
        // Human fighters
        NewUpgrade(ActivityId.HumanSwords1,     UpgradeId.Meelee,           1,  AllowFeature.None);
        NewUpgrade(ActivityId.HumanSwords2,     UpgradeId.Meelee,           2,  AllowFeature.None);
        NewUpgrade(ActivityId.HumanShields1,    UpgradeId.Shields,          1,  AllowFeature.None);
        NewUpgrade(ActivityId.HumanShields2,    UpgradeId.Shields,          2,  AllowFeature.None);
        NewUpgrade(ActivityId.HumanArrows1,     UpgradeId.Ranged,           1,  AllowFeature.None);
        NewUpgrade(ActivityId.HumanArrows2,     UpgradeId.Ranged,           2,  AllowFeature.None);
        NewUpgrade(ActivityId.HumanHorses1,     UpgradeId.Speed,            1,  AllowFeature.None);
        NewUpgrade(ActivityId.HumanHorses2,     UpgradeId.Speed,            2,  AllowFeature.None);

        // Orc fighters
        NewUpgrade(ActivityId.OrcAxes1,         UpgradeId.Meelee,           1,  AllowFeature.None);
        NewUpgrade(ActivityId.OrcAxes2  ,       UpgradeId.Meelee,           2,  AllowFeature.None);
        NewUpgrade(ActivityId.OrcShields1,      UpgradeId.Shields,          1,  AllowFeature.None);
        NewUpgrade(ActivityId.OrcShields2,      UpgradeId.Shields,          2,  AllowFeature.None);
        NewUpgrade(ActivityId.OrcSpears1,       UpgradeId.Ranged,           1,  AllowFeature.None);
        NewUpgrade(ActivityId.OrcSpears2,       UpgradeId.Ranged,           2,  AllowFeature.None);
        NewUpgrade(ActivityId.OrcWolves1,       UpgradeId.Speed,            1,  AllowFeature.None);
        NewUpgrade(ActivityId.OrcWolves2,       UpgradeId.Speed,            2,  AllowFeature.None);

        // Human spells
        NewUpgrade(ActivityId.Heal,             UpgradeId.Heal,             1,  AllowFeature.Heal);
        NewUpgrade(ActivityId.HolyVision,       UpgradeId.HolyVision,       1,  AllowFeature.HolyVision);
        NewUpgrade(ActivityId.Invisibility,     UpgradeId.Invisibility,     1,  AllowFeature.Invisibility);
        NewUpgrade(ActivityId.SummonScorpions,  UpgradeId.SummonScorpions,  1,  AllowFeature.SummonScorpions);
        NewUpgrade(ActivityId.FireRain,         UpgradeId.FireRain,         1,  AllowFeature.FireRain);
        NewUpgrade(ActivityId.SummonElemental,  UpgradeId.SummonElemental,  1,  AllowFeature.SummonElemental);

        // Orc spells
        NewUpgrade(ActivityId.RaiseDead,        UpgradeId.RaiseDead,        1,  AllowFeature.RaiseDead);
        NewUpgrade(ActivityId.DarkVision,       UpgradeId.DarkVision,       1,  AllowFeature.DarkVision);
        NewUpgrade(ActivityId.UnholyArmor,      UpgradeId.UnholyArmor,      1,  AllowFeature.UnholyArmor);
        NewUpgrade(ActivityId.SummonSpiders,    UpgradeId.SummonSpiders,    1,  AllowFeature.SummonSpiders);
        NewUpgrade(ActivityId.PoisonCloud,      UpgradeId.PoisonCloud,      1,  AllowFeature.PoisonCloud);
        NewUpgrade(ActivityId.SummonDaemon,     UpgradeId.SummonDaemon,     1,  AllowFeature.SummonDaemon);

        // Set upgrade costs/building.
 
        // Human fighters
        SetUpgCost(ActivityId.HumanSwords1,     UnitTypeId.HumanSmith,      750,    1200);
        SetUpgCost(ActivityId.HumanSwords2,     UnitTypeId.HumanSmith,      1500,   1200);
        SetUpgCost(ActivityId.HumanShields1,    UnitTypeId.HumanSmith,      750,    1200);
        SetUpgCost(ActivityId.HumanShields2,    UnitTypeId.HumanSmith,      1500,   1200);
        SetUpgCost(ActivityId.HumanArrows1,     UnitTypeId.HumanMill,       750,    1400);
        SetUpgCost(ActivityId.HumanArrows2,     UnitTypeId.HumanMill,       1500,   1400);
        SetUpgCost(ActivityId.HumanHorses1,     UnitTypeId.Stables,         750,    1400);
        SetUpgCost(ActivityId.HumanHorses2,     UnitTypeId.Stables,         1500,   1400);

        // Orc fighters
        SetUpgCost(ActivityId.OrcAxes1,         UnitTypeId.OrcSmith,        750,    1200);
        SetUpgCost(ActivityId.OrcAxes2,         UnitTypeId.OrcSmith,        1500,   1200);
        SetUpgCost(ActivityId.OrcShields1,      UnitTypeId.OrcSmith,        750,    1200);
        SetUpgCost(ActivityId.OrcShields2,      UnitTypeId.OrcSmith,        1500,   1200);
        SetUpgCost(ActivityId.OrcSpears1,       UnitTypeId.OrcMill,         750,    1400);
        SetUpgCost(ActivityId.OrcSpears2,       UnitTypeId.OrcMill,         1500,   1400);
        SetUpgCost(ActivityId.OrcWolves1,       UnitTypeId.Kennel,          750,    1400);
        SetUpgCost(ActivityId.OrcWolves2,       UnitTypeId.Kennel,          1500,   1400);

        // Human spells.
        SetUpgCost(ActivityId.Heal,             UnitTypeId.Church,          750,    1200);
        SetUpgCost(ActivityId.HolyVision,       UnitTypeId.Church,          1500,   1200);
        SetUpgCost(ActivityId.Invisibility,     UnitTypeId.Church,          3000,   1200);
        SetUpgCost(ActivityId.SummonScorpions,  UnitTypeId.HumanTower,      750,    1400);
        SetUpgCost(ActivityId.FireRain,         UnitTypeId.HumanTower,      1500,   1400);
        SetUpgCost(ActivityId.SummonElemental,  UnitTypeId.HumanTower,      3000,   1400);

        // Orc spells.
        SetUpgCost(ActivityId.RaiseDead,        UnitTypeId.Temple,          750,    1200);
        SetUpgCost(ActivityId.DarkVision,       UnitTypeId.Temple,          1500,   1200);
        SetUpgCost(ActivityId.UnholyArmor,      UnitTypeId.Temple,          3000,   1200);
        SetUpgCost(ActivityId.SummonSpiders,    UnitTypeId.OrcTower,        750,    1400);
        SetUpgCost(ActivityId.PoisonCloud,      UnitTypeId.OrcTower,        1500,   1400);
        SetUpgCost(ActivityId.SummonDaemon,     UnitTypeId.OrcTower,        3000,   1400);
    }

    // Load actors.
    private void LoadActors()
    {
        int i;

        // Load animations.
        ScriptNode[][] EntryPoints;
        EntryPoints = WarArchive.GetAnimations(Global.Archive.GetChunk(212));
        
        // Create unit animation mapper.
        ArrayAnimationMapper unitAnimationMapper = new ArrayAnimationMapper();
        unitAnimationMapper[ActorAnimation.Death] = 1;
        unitAnimationMapper[ActorAnimation.Idle] = 2;
        unitAnimationMapper[ActorAnimation.Move] = 3;
        unitAnimationMapper[ActorAnimation.Action] = 4;
        unitAnimationMapper[ActorAnimation.Birth] = 5;

        // Load unit actors.
        UnitActors = new AnimatedSpriteActorType[WarGameData.UnitCount];
        for (i = 0; i < WarGameData.UnitCount; ++i) {
            UnitActors[i] = new AnimatedSpriteActorType(this);
            UnitActors[i].ControlsMovement = true;
            UnitActors[i].DrawTag = i;
            UnitActors[i].EntryPoints = EntryPoints[i];
            UnitActors[i].AnimMap = unitAnimationMapper;
        }

        // Create missile animation mapper.
        ArrayAnimationMapper missileAnimationMapper = new ArrayAnimationMapper();
        missileAnimationMapper[ActorAnimation.Death] = 0;
        missileAnimationMapper[ActorAnimation.Idle] = 1;
        missileAnimationMapper[ActorAnimation.Move] = 2;
        missileAnimationMapper[ActorAnimation.Action] = 1;
        missileAnimationMapper[ActorAnimation.Birth] = 1;

        // Load unit actors.
        EffectActors = new AnimatedSpriteActorType[WarGameData.EffectCount];
        for (i = 0; i < WarGameData.EffectCount; ++i) {
            EffectActors[i] = new AnimatedSpriteActorType(this);
            EffectActors[i].ControlsMovement = false;
            EffectActors[i].DrawTag = i + WarGameData.UnitCount;
            EffectActors[i].EntryPoints = EntryPoints[i + WarGameData.UnitCount];
            EffectActors[i].AnimMap = missileAnimationMapper;
        }
    }

    // Returns map's tileset as one of the tileset enums.
    public Tileset GetMapTilesetEnum(GameMap map)
    {
        if (map is SimpleTileMap) {
            if (((SimpleTileMap)map).Tileset == GrassTileset) {
                return Tileset.Grass;
            }
            if (((SimpleTileMap)map).Tileset == SwampTileset) {
                return Tileset.Swamp;
            }
            if (((SimpleTileMap)map).Tileset == DungeonTileset) {
                return Tileset.Dungeon;
            }
        }
        return Tileset.Max;
    }

    // Override to return a WarGameState
    public override GameState CreateGameState()
    {
        return new WarGameState(this);
    }
}
