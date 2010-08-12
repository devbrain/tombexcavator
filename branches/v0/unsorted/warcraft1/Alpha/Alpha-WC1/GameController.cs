// Copyright (C) 2005-2006 Crestez Leonard    cleonard@go.ro
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

using Alpha.GameUI;
using Alpha.Game;
using Alpha.Util;

// This is a class that manages button sets.
// You can ask this class what button set to start with for a certain selection.
public class GameController
{
    // Some standard button sets.
    public UnitButton[] EmptySet;
    public UnitButton[] CancelSet;
    public UnitButton[] HumanStopMoveSet;
    public UnitButton[] HumanStopMoveAttackSet;
    public UnitButton[] OrcStopMoveSet;
    public UnitButton[] OrcStopMoveAttackSet;

    // This maps UnitTypeId to UnitButton[]
    // When all selected entities have the same type, this set is used.
    public Hashtable UnitButtonSet;

    // Clones an icon variant for an UnitButton.
    // Result identical except for the icon.
    private UnitButton CloneIconVariant(UnitButton button, string icon)
    {
        UnitButton copy = button.Clone();
        copy.Icon = Global.Resources.GetLink(icon);
        return copy;
    }

    // Clones an upgrade variant for an UnitButton.
    // Result will only be visible in a certain upgrade state.
    private UnitButton CloneUpgradeVariant(UnitButton button, string icon, UpgradeId upg, int level)
    {
        UnitButton copy = button.Clone();
        copy.Icon = Global.Resources.GetLink(icon);
        copy.UpgradeRequired = upg;
        copy.UpgradeLevelRequired = level;
        return copy;
    }

    private TargeterButton CreateTargeterButton(TargetOrder groundOrder, TargetOrder entityOrder,
            int position, string icon)
    {
        TargeterButton button;
        button = new TargeterButton();
        button.Targeter = new CommonTargeter(groundOrder, entityOrder);
        button.PositionHint = position;
        button.Icon = Global.Resources.GetLink(icon);
        return button;
    }

    private SpellButton CreateSpellButton(Spell spell, int position, string icon)
    {
        SpellButton button;
        button = new SpellButton();
        button.Spell = spell;
        button.PositionHint = position;
        button.Icon = Global.Resources.GetLink(icon);
        return button;
    }

    // Create an UnitButton for training.
    private ActivityButton CreateTrainButton(UnitTypeId unit, int position)
    {
        UnitType utype = Global.GameData.Units[(int)unit];

        ActivityButton button;
        button = new ActivityButton();
        button.Activity = utype.TrainActivity;
        button.PositionHint = position;
        button.Icon = utype.Icon;
        return button;
    }

    // Create an UnitButton for upgrading.
    public ActivityButton CreateUpgradeButton(ActivityId id, int position, string icon)
    {
        ActivityButton button;
        button = new ActivityButton();
        button.Activity = Global.GameData.Upgrades[(int)id];
        button.PositionHint = position;
        button.Icon = Global.Resources.GetLink(icon);
        return button;
    }

    // Load everything.
    public GameController()
    {
        // Basics
        TargetInfo tinfo = new TargetInfo(null, 0, 0);
        UnitButtonSet = new Hashtable();
        
        EmptySet = new UnitButton[] {};

        // Cancel targetting.
        CancelTargettingButton CancelButton = new CancelTargettingButton();
        CancelButton.PositionHint = 5;
        CancelButton.Icon = Global.Resources.GetLink("icon-cancel");
        CancelSet = new UnitButton[] { CancelButton };

        ActivityStopButton BuildingStopButton;
        BuildingStopButton = new ActivityStopButton();
        BuildingStopButton.PositionHint = 5;
        BuildingStopButton.Icon = Global.Resources.GetLink("icon-cancel");

        // Stop button
        OrderButton StopButton;
        StopButton = new OrderButton();
        StopButton.Order = new IdleOrder();
        StopButton.PositionHint = 1;
        
        // Move Item
        TargeterButton MoveButton = CreateTargeterButton(
                new MoveOrder(tinfo), new MoveOrder(tinfo),
                0, "icon-human-walk");
 
        // Attack Button
        TargeterButton AttackButton = CreateTargeterButton(
                new HuntOrder(tinfo),
                new AbilityOrder(Global.GameData.AttackAbility, tinfo),
                2, "icon-human-walk");

        // Race-based button clones.
        // TODO: upgrade based.
        UnitButton HumanStop = CloneIconVariant(StopButton, "icon-human-shield");
        UnitButton HumanStop1 = CloneUpgradeVariant(StopButton, "icon-human-shield-1", UpgradeId.Meelee, 0);
        UnitButton HumanStop2 = CloneUpgradeVariant(StopButton, "icon-human-shield-2", UpgradeId.Meelee, 1);
        UnitButton HumanStop3 = CloneUpgradeVariant(StopButton, "icon-human-shield-3", UpgradeId.Meelee, 2);
        UnitButton HumanMove = CloneIconVariant(MoveButton, "icon-human-walk");

        UnitButton OrcStop = CloneIconVariant(StopButton, "icon-orc-shield");
        UnitButton OrcStop1 = CloneUpgradeVariant(StopButton, "icon-orc-shield-1", UpgradeId.Meelee, 0);
        UnitButton OrcStop2 = CloneUpgradeVariant(StopButton, "icon-orc-shield-2", UpgradeId.Meelee, 1);
        UnitButton OrcStop3 = CloneUpgradeVariant(StopButton, "icon-orc-shield-3", UpgradeId.Meelee, 2);
        UnitButton OrcMove = CloneIconVariant(MoveButton, "icon-orc-walk");

        // Standard move and attack sets.
        HumanStopMoveSet = new UnitButton[] {
                HumanMove, HumanStop,
        };
        OrcStopMoveSet = new UnitButton[] {
                OrcMove, OrcStop,
        };
        HumanStopMoveAttackSet = new UnitButton[] {
                HumanMove, HumanStop1, HumanStop2, HumanStop3,
                CloneUpgradeVariant(AttackButton, "icon-human-sword-1", UpgradeId.Meelee, 0),
                CloneUpgradeVariant(AttackButton, "icon-human-sword-2", UpgradeId.Meelee, 1),
                CloneUpgradeVariant(AttackButton, "icon-human-sword-3", UpgradeId.Meelee, 2),
        };
        OrcStopMoveAttackSet = new UnitButton[] {
                OrcMove, OrcStop1, OrcStop2, OrcStop3,
                CloneUpgradeVariant(AttackButton, "icon-orc-axe-1", UpgradeId.Meelee, 0),
                CloneUpgradeVariant(AttackButton, "icon-orc-axe-2", UpgradeId.Meelee, 1),
                CloneUpgradeVariant(AttackButton, "icon-orc-axe-3", UpgradeId.Meelee, 2),
        };

        // Archer/Spearman
        UnitButtonSet[UnitTypeId.Archer] = new UnitButton[] {
                HumanMove, HumanStop1, HumanStop2, HumanStop3,
                CloneUpgradeVariant(AttackButton, "icon-human-bow-1", UpgradeId.Ranged, 0),
                CloneUpgradeVariant(AttackButton, "icon-human-bow-2", UpgradeId.Ranged, 1),
                CloneUpgradeVariant(AttackButton, "icon-human-bow-3", UpgradeId.Ranged, 2),
        };
        UnitButtonSet[UnitTypeId.Spearman] = new UnitButton[] {
                OrcMove, OrcStop1, OrcStop2, OrcStop3,
                CloneUpgradeVariant(AttackButton, "icon-orc-spear-1", UpgradeId.Ranged, 0),
                CloneUpgradeVariant(AttackButton, "icon-orc-spear-2", UpgradeId.Ranged, 1),
                CloneUpgradeVariant(AttackButton, "icon-orc-spear-3", UpgradeId.Ranged, 2),
        };

        // Cleric/Necrolyte
        UnitButtonSet[UnitTypeId.Cleric] = new UnitButton[] {
                HumanStop, HumanMove, CloneIconVariant(AttackButton, "icon-cleric-attack"),
                CreateSpellButton(Global.GameData.HealSpell, 3, "icon-heal"),
                // NI: CreateSpellButton(Global.GameData.RevealMapSpell, 4, "icon-holy-vision"),
                CreateSpellButton(Global.GameData.InvisibilitySpell, 5, "icon-invisibility"),
        };
        UnitButtonSet[UnitTypeId.Necrolyte] = new UnitButton[] {
                OrcStop, OrcMove, CloneIconVariant(AttackButton, "icon-necrolyte-attack"),
                CreateSpellButton(Global.GameData.RaiseDeadSpell, 3, "icon-raise-dead"),
                // NI: CreateSpellButton(Global.GameData.RevealMapAbility, 4, "icon-unholy-vision"),
                CreateSpellButton(Global.GameData.UnholyArmorSpell, 5, "icon-unholy-armor"),
        };

        // Conjurer/Warlock
        UnitButtonSet[UnitTypeId.Conjurer] = new UnitButton[] {
                HumanStop, HumanMove, CloneIconVariant(AttackButton, "icon-conjurer-attack"),
                CreateSpellButton(Global.GameData.SummonScorpionsSpell, 3, "icon-48"),
                CreateSpellButton(Global.GameData.FireRainSpell, 4, "icon-fire-rain"),
                CreateSpellButton(Global.GameData.SummonElementalSpell, 5, "icon-52"),
        };
        UnitButtonSet[UnitTypeId.Warlock] = new UnitButton[] {
                OrcStop, OrcMove, CloneIconVariant(AttackButton, "icon-warlock-attack"),
                CreateSpellButton(Global.GameData.SummonSpidersSpell, 3, "icon-45"),
                CreateSpellButton(Global.GameData.PoisonCloudSpell, 4, "icon-poison-cloud"),
                CreateSpellButton(Global.GameData.SummonDaemonSpell, 5, "icon-51"),
        };

        // Peasant/Peon
        UnitButtonSet[UnitTypeId.Peasant] = new UnitButton[] {
                HumanStop, HumanMove,
                CreateTargeterButton(null, new MineGoldOrder(TargetInfo.Empty), 2, "icon-harvest"),
        };
        UnitButtonSet[UnitTypeId.Peon] = new UnitButton[] {
                OrcStop, OrcMove,
                CreateTargeterButton(null, new MineGoldOrder(TargetInfo.Empty), 2, "icon-harvest"),
        };

        // Barracks
        UnitButtonSet[UnitTypeId.HumanBarracks] = new UnitButton[] {
                CreateTrainButton(UnitTypeId.Footman, 0),
                CreateTrainButton(UnitTypeId.Archer, 1),
                CreateTrainButton(UnitTypeId.Knight, 2),
                CreateTrainButton(UnitTypeId.HumanCatapult, 3),
                BuildingStopButton,
        };
        UnitButtonSet[UnitTypeId.OrcBarracks] = new UnitButton[] {
                CreateTrainButton(UnitTypeId.Grunt, 0),
                CreateTrainButton(UnitTypeId.Spearman, 1),
                CreateTrainButton(UnitTypeId.Raider, 2),
                CreateTrainButton(UnitTypeId.OrcCatapult, 3),
                BuildingStopButton,
        };

        // Smith
        UnitButtonSet[UnitTypeId.HumanSmith] = new UnitButton[] {
                CreateUpgradeButton(ActivityId.HumanSwords1,    0,  "icon-human-sword-2"),
                CreateUpgradeButton(ActivityId.HumanSwords2,    0,  "icon-human-sword-3"),
                CreateUpgradeButton(ActivityId.HumanShields1,   1,  "icon-human-shield-2"),
                CreateUpgradeButton(ActivityId.HumanShields2,   1,  "icon-human-shield-3"),
                BuildingStopButton,
        };
        UnitButtonSet[UnitTypeId.OrcSmith] = new UnitButton[] {
                CreateUpgradeButton(ActivityId.OrcAxes1,        0,  "icon-orc-axe-2"),
                CreateUpgradeButton(ActivityId.OrcAxes2,        0,  "icon-orc-axe-3"),
                CreateUpgradeButton(ActivityId.OrcShields1,     1,  "icon-orc-shield-2"),
                CreateUpgradeButton(ActivityId.OrcShields2,     1,  "icon-orc-shield-3"),
                BuildingStopButton,
        };
        
        // Mill
        UnitButtonSet[UnitTypeId.HumanMill] = new UnitButton[] {
                CreateUpgradeButton(ActivityId.HumanArrows1,    0,  "icon-human-bow-2"),
                CreateUpgradeButton(ActivityId.HumanArrows2,    0,  "icon-human-bow-3"),
                BuildingStopButton,
        };
        UnitButtonSet[UnitTypeId.OrcMill] = new UnitButton[] {
                CreateUpgradeButton(ActivityId.OrcSpears1,      0,  "icon-orc-spear-2"),
                CreateUpgradeButton(ActivityId.OrcSpears2,      0,  "icon-orc-spear-3"),
                BuildingStopButton,
        };

        // Stables/Kennel
        UnitButtonSet[UnitTypeId.Stables] = new UnitButton[] {
                CreateUpgradeButton(ActivityId.HumanHorses1,    0,  "icon-human-horse-1"),
                CreateUpgradeButton(ActivityId.HumanHorses2,    0,  "icon-human-horse-2"),
                BuildingStopButton,
        };
        UnitButtonSet[UnitTypeId.Kennel] = new UnitButton[] {
                CreateUpgradeButton(ActivityId.OrcWolves1,      0,  "icon-orc-wolf-1"),
                CreateUpgradeButton(ActivityId.OrcWolves2,      0,  "icon-orc-wolf-2"),
                BuildingStopButton,
        };

        // Town halls
        UnitButtonSet[UnitTypeId.HumanTownHall] = new UnitButton[] {
                CreateTrainButton(UnitTypeId.Peasant, 0),
                BuildingStopButton,
        };
        UnitButtonSet[UnitTypeId.OrcTownHall] = new UnitButton[] {
                CreateTrainButton(UnitTypeId.Peon, 0),
                BuildingStopButton,
        };

        // Church/Temple
        UnitButtonSet[UnitTypeId.Church] = new UnitButton[] {
                CreateTrainButton(UnitTypeId.Cleric, 0),
                CreateUpgradeButton(ActivityId.Heal,            1,  "icon-heal"),
                CreateUpgradeButton(ActivityId.HolyVision,      2,  "icon-holy-vision"),
                CreateUpgradeButton(ActivityId.Invisibility,    3,  "icon-invisibility"),
                BuildingStopButton,
        };
        UnitButtonSet[UnitTypeId.Temple] = new UnitButton[] {
                CreateTrainButton(UnitTypeId.Necrolyte, 0),
                CreateUpgradeButton(ActivityId.RaiseDead,       1,  "icon-raise-dead"),
                CreateUpgradeButton(ActivityId.DarkVision,      2,  "icon-dark-vision"),
                CreateUpgradeButton(ActivityId.UnholyArmor,     3,  "icon-unholy-armor"),
                BuildingStopButton,
        };

        // Tower
        UnitButtonSet[UnitTypeId.HumanTower] = new UnitButton[] {
                CreateTrainButton(UnitTypeId.Conjurer, 0),
                CreateUpgradeButton(ActivityId.SummonScorpions, 1,  "icon-48"),
                CreateUpgradeButton(ActivityId.FireRain,        2,  "icon-fire-rain"),
                CreateUpgradeButton(ActivityId.SummonElemental, 3,  "icon-52"),
                BuildingStopButton,
        };
        UnitButtonSet[UnitTypeId.OrcTower] = new UnitButton[] {
                CreateTrainButton(UnitTypeId.Warlock, 0),
                CreateUpgradeButton(ActivityId.SummonSpiders,   1,  "icon-45"),
                CreateUpgradeButton(ActivityId.PoisonCloud,     2,  "icon-poison-cloud"),
                CreateUpgradeButton(ActivityId.SummonDaemon,    3,  "icon-51"),
                BuildingStopButton,
        };
    }

    // Get the buttonset for a certain UnitType.
    private UnitButton[] GetButtonSet(UnitType t)
    {
        return (UnitButton[])UnitButtonSet[t.Ident];
    }

    // This function will filter a list of entities for selecting.
    // Read the code for filterings done.
    // Returns filtered list.
    public IList FilterSelection(IGameScreen screen, IList entities)
    {
        int i;
        bool owned = false;
        bool nonBuildings = false;
        ArrayList result = new ArrayList();
        Unit unit;
        Player player = (Player)screen.Player;

        if (entities == null) {
            throw new ArgumentNullException("entities");
        }

        // Hack to support null player.
        if (player == null) {
            owned = true;
        }

        // Remove missiles, dead units, offmap, etc.
        // Check for owned units.
        for (i = 0; i < entities.Count; ++i) {
            unit = entities[i] as Unit;
            if (unit != null && unit.Dead == false && unit.OffMap == false) {
                result.Add(unit);
                owned |= (unit.Player == player);
            }
        }
      
        // If some of the entities belong to the current player remove all others.
        if (owned && player != null) {
            for (i = 0; i < result.Count; ++i) {
                if (((Entity)result[i]).Player != player) {
                    result.RemoveAt(i--);
                }
            }
        }

        // Check for non-buildings.
        for (i = 0; i < result.Count; ++i) {
            unit = (Unit)result[i];
            nonBuildings |= (unit.Type.Building == false);
        }

        // Remove buildings if there are some normal entities too.
        if (nonBuildings) {
            for (i = 0; i < result.Count; ++i) {
                unit = (Unit)result[i];
                if (unit.Type.Building) {
                    result.RemoveAt(i--);
                }
            }
        }

        // We only select multiple entities if owned by the current
        // player and they're not buildings.
        if (owned == false || nonBuildings == false) {
            while (result.Count > 1) {
                result.RemoveAt(result.Count - 1);
            }
        }

        return result;
    }

    // Handles right-clicking.
    public void DefaultTargetHandler(IGameScreen screen, int x, int y)
    {
        Order order;
        IList list;
        int i;

        if (screen.SelectedEntities.Count == 0 ||
                ((Entity)screen.SelectedEntities[0]).Player != screen.Player) {
            return;
        }

        // Select targetted units.
        list = screen.Map.Cache.SelectList(x, y);
        for (i = 0; i < list.Count; ++i) {
            if (list[i] is Unit == false) {
                list.RemoveAt(i--);
            }
        }

        // Trivial logic.
        if (list.Count > 0) {
            Entity entity = list[0] as Entity;
            if ((screen.SelectedEntities[0] as Entity).Player.ShouldAttack(entity)) {
                order = new AbilityOrder(Global.GameData.AttackAbility, new TargetInfo(entity));
            } else {
                order = new MoveOrder(new TargetInfo(entity));
            }
        } else {
            order = new MoveOrder(new TargetInfo(screen.Game.State.Map, x, y));
        }

        // Send the order.
        foreach (Entity entity in screen.SelectedEntities) {
            screen.Game.SendCommand(new OrderCommand(entity, order, screen.QueueOrders));
        }
    }

    // What ButtonSet to start with for this selection?
    public UnitButton[] GetSelectionButtons(IGameScreen screen, IList units)
    {
        if (units.Count == 0 || ((Entity)units[0]).Player != screen.Player) {
            return EmptySet;
        }

        if (screen.Targetting) {
            return CancelSet;
        }

        // Check if all entities have the same type.
        UnitType t = (units[0] as Unit).Type;
        foreach (Unit unit in units) {
            if (unit.Type != t) {
                t = null;
                break;
            }
        }

        // They do have the same type.
        if (t != null && UnitButtonSet.Contains(t.Ident)) {
            if (!t.Building) {
                return GetButtonSet(t);
            } else {
                // Single building with fancy buttons:
                if (units.Count != 1) {
                    throw new InvalidOperationException("Can't show multiple buildings at once.");
                }
                return GetButtonSet(t);
            }
        }

        if (units.Count == 1) {
            Unit unit = units[0] as Unit;
            if (!unit.CanMove()) {
                return EmptySet;
            }
            if (unit.Type.CanAttack) {
                if (unit.Type.Race == Race.Human) {
                    return HumanStopMoveAttackSet;
                } else {
                    return OrcStopMoveAttackSet;
                }
            } else {
                if (unit.Type.Race == Race.Human) {
                    return HumanStopMoveSet;
                } else {
                    return OrcStopMoveSet;
                }
            }
        } else {
            bool CanAttack = true;
            Race race = Race.Human;
            foreach (Unit unit in units) {
                if (!unit.Type.CanAttack) {
                    CanAttack = false;
                    break;
                }
                if (unit.Type.Race == Race.Orc) {
                    race = Race.Orc;
                }
            }
            if (CanAttack) {
                if (race == Race.Human) {
                    return HumanStopMoveAttackSet;
                } else {
                    return OrcStopMoveAttackSet;
                }
            } else {
                if (race == Race.Human) {
                    return HumanStopMoveSet;
                } else {
                    return OrcStopMoveSet;
                }
            }
        }
    }
}

