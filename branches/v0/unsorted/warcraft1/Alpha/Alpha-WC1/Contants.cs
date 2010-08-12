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

// Races.
public enum Race
{
    Human,
    Orc,
    Max
};

// Tilesets.
public enum Tileset
{
    Grass,
    Swamp,
    Dungeon,
    Max
}

// Unit enumeration.
// The values are the same used in WC1.
public enum UnitTypeId
{
    // Simple Units, soldiers/troops/creatures.
    Footman                     = 0,
    Grunt                       = 1,
    Peasant                     = 2,
    Peon                        = 3,
    HumanCatapult               = 4,
    OrcCatapult                 = 5,
    Knight                      = 6,
    Raider                      = 7,
    Archer                      = 8,
    Spearman                    = 9,
    Conjurer                    = 10,
    Warlock                     = 11,
    Cleric                      = 12,
    Necrolyte                   = 13,

    // Hero units.
    Medivh                      = 14,
    Lothar                      = 15,
    Wounded                     = 16,
    Griselda                    = 17,
    Garona                      = 18,

    // Dungeon units.
    Ogre                        = 19,
    Slime                       = 22,
    Brigand                     = 25,
    ScenarioSkeleton            = 27,
    FireElemental               = 23,

    // Summoned units
    Spider                      = 21,
    Scorpion                    = 24,
    Skeleton                    = 26,
    Daemon                      = 28,
    WaterElemental              = 31,

    // Unused units.
    Unused20                    = 20,
    Unused29                    = 29,
    Unused30                    = 30,

    // Buildings.
    HumanFarm                   = 32,
    OrcFarm                     = 33,
    HumanBarracks               = 34,
    OrcBarracks                 = 35,
    Church                      = 36,
    Temple                      = 37,
    HumanTower                  = 38,
    OrcTower                    = 39,
    HumanTownHall               = 40,
    OrcTownHall                 = 41,
    HumanMill                   = 42,
    OrcMill                     = 43,
    Stables                     = 44,
    Kennel                      = 45,
    HumanSmith                  = 46,
    OrcSmith                    = 47,
    Stormwind                   = 48,
    BlackRock                   = 49,
    GoldMine                    = 50,
}

// Missiles/clouds/whatever.
public enum EffectId
{
    SecondCorpse                = 0,
    EnergyBall                  = 1,
    Unused2                     = 2,
    Arrow                       = 3,
    PoisonCloud                 = 4,
    FireDrop                    = 5,
    SmallFire                   = 6,
    BigFire                     = 7,
    Explosion                   = 8,
    Sparks                      = 9,
    Demolition                  = 10,
    BlueFireball                = 11,
    Fireball                    = 12,
}

// This which can be allowed/disallowed in maps.
// A bitmask of 1 << these things can be found in MapFlags.
// A lot of human/orc pairs are equal.
public enum AllowFeature
{
    // Training units.
    TrainFootman                = 0,
    TrainGrunt                  = 0,

    TrainPeasant                = 1,
    TrainPeon                   = 1,

    TrainHumanCatapult          = 2,
    TrainOrcCatapult            = 2,

    TrainKnight                 = 3,
    TrainRaider                 = 3,

    TrainArcher                 = 4,
    TrainSpearman               = 4,

    TrainConjurer               = 5,
    TrainWarlock                = 5,

    TrainCleric                 = 6,
    TrainNecrolyte              = 6,

    // Constructing buildings.
    BuildHumanFarm              = 7,
    BuildOrcFarm                = 7,

    BuildHumanBarracks          = 8,
    BuildOrcBarracks            = 8,

    BuildChurch                 = 9,
    BuildTemple                 = 9,

    BuildHumanTower             = 10,
    BuildOrcTower               = 10,

    BuildHumanTownHall          = 11,
    BuildOrcTownHall            = 11,

    BuildHumanMill              = 12,
    BuildOrcMill                = 12,

    BuildStable                 = 13,
    BuildKennel                 = 13,

    BuildHumanSmith             = 14,
    BuildOrcSmith               = 14,

    // Cleric/Necrolyte spells.
    Heal                        = 15,
    RaiseDead                   = 15,
    HolyVision                  = 16,
    DarkVision                  = 16,
    Invisibility                = 17,
    UnholyArmor                 = 17,

    // Conjurer/Warlock spells.
    SummonScorpions             = 18,
    SummonSpiders               = 18,
    FireRain                    = 19,
    PoisonCloud                 = 19,
    SummonElemental             = 20,
    SummonDaemon                = 20,

    // Roads and walls.
    BuildRoad                   = 21,
    BuildWall                   = 22,

    // This is valid, and always allowed.
    Max,
    None = Max,
}

// Upgrade identifiers. Same as in WC1.
public enum UpgradeId
{
    // Human arrows/Orc spears.
    Ranged                      = 0,
    // Human swords/Orc axes.
    Meelee                      = 1,
    // Human horses/Orc wolves.
    Speed                       = 2,

    // Conjurer/Warlock spells.
    SummonScorpions             = 3,
    SummonSpiders               = 3,
    FireRain                    = 4,
    PoisonCloud                 = 4,
    SummonElemental             = 5,
    SummonDaemon                = 5,

    // Cleric/Necrolyte spells.
    Heal                        = 6,
    RaiseDead                   = 6,
    HolyVision                  = 7,
    DarkVision                  = 7,
    Invisibility                = 8,
    UnholyArmor                 = 8,

    // Human/Orc shields.
    Shields                     = 9,

    Max,
}

// Activity idents. Alpha-WC1 specific.
// Only for upgrades. BAH.
public enum ActivityId
{
    // Human fighters.
    HumanSwords1,
    HumanSwords2,
    HumanShields1,
    HumanShields2,
    HumanHorses1,
    HumanHorses2,
    HumanArrows1,
    HumanArrows2,

    // Orc fighters
    OrcAxes1,
    OrcAxes2,
    OrcShields1,
    OrcShields2,
    OrcWolves1,
    OrcWolves2,
    OrcSpears1,
    OrcSpears2,

    // Cleric
    Heal,
    HolyVision,
    Invisibility,

    // Necrolyte
    RaiseDead,
    DarkVision,
    UnholyArmor,

    // Conjurer
    SummonScorpions,
    FireRain,
    SummonElemental,

    // Warlock
    SummonSpiders,
    PoisonCloud,
    SummonDaemon,

    Max,
}
