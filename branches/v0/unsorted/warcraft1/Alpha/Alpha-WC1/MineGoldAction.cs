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

using Alpha.Game;
using Alpha.Util;

// Gives MineGoldAction.
public class MineGoldOrder: TargetOrder
{
    // Must target non-empty gold mine.
    public override ValidationError Validate(Entity entity)
    {
        ValidationError err;
        if ((err = base.Validate(entity)) != null) {
            return err;
        }

        // Check miner.
        Unit unit = entity as Unit;
        if (unit == null || (unit.Type != Global.GameData.Units[(int)UnitTypeId.Peon] &&
                unit.Type != Global.GameData.Units[(int)UnitTypeId.Peasant])) {
            return new ValidationError("Unit can't mine.");
        }

        // Validate target.
        Unit tunit = Target.Entity as Unit;
        if (tunit == null) {
            return new ValidationError("Unit target required.");
        }
        if (tunit.Type != Global.GameData.Units[(int)UnitTypeId.GoldMine]) {
            return new ValidationError("Gold mine required.");
        }
        if (tunit.GoldAmount <= 0) {
            return new ValidationError("Gold mine is exhausted.");
        }
        return null;
    }

    public override IAction CreateAction(Entity entity)
    {
        MineGoldAction act = new MineGoldAction();
        act.Mine = Target.Entity as Unit;
        return act;
    }

    public MineGoldOrder(TargetInfo target): base(target)
    {
    }
}

// Action for mining gold.
// Unit will go back and forth carrying gold.
public class MineGoldAction: ActionBase
{
    // How long to wait inside the mine.
    public const int MineWaitTime = 100;

    // How long to wait inside storage.
    public const int StorageWaitTime = 100;

    // How much gold to mine at once.
    public int ResourceAmount = 100;

    // Shortcut to avoid casting all the time.
    public Unit Unit {
        get {
            return (Unit)Entity;
        }
    }

    // Wait cycles. If this is > 1 NextCycle decrements and returns.
    private int Wait;

    // Containing unit.
    private Unit Container;

    // Mine to mine from.
    public Unit Mine;

    // Storage to store in.
    public Unit Storage;

    // Updates Mine to something which IsValidMine or null.
    // FIXME: Painfully slow.
    private void FindMine()
    {
        Rectangle area = new Rectangle(Int32.MinValue, Int32.MinValue, Int32.MaxValue, Int32.MaxValue);
        int newDist, bestDist;
        bestDist = Int32.MaxValue;
        
        foreach (Entity mine in Unit.Map.Cache.SelectList(area)) {
            if (!IsValidMine(mine)) {
                continue;
            }
            newDist = IntMath.Distance(Entity.Pos, mine.Pos);
            if (newDist < bestDist) {
                bestDist = newDist;
                Mine = (Unit)mine;
            }
        }
    }

    // Updates Storage to something which IsValidStorage or null.
    // FIXME: Painfully slow.
    private void FindStorage()
    {
        Rectangle area = new Rectangle(Int32.MinValue, Int32.MinValue, Int32.MaxValue, Int32.MaxValue);
        int newDist, bestDist;
        bestDist = Int32.MaxValue;
        
        foreach (Entity storage in Unit.Map.Cache.SelectList(area)) {
            if (!IsValidStorage(storage)) {
                continue;
            }
            newDist = IntMath.Distance(Entity.Pos, storage.Pos);
            if (newDist < bestDist) {
                bestDist = newDist;
                Storage = (Unit)storage;
            }
        }
    }

    // Returns if mine is valid.
    private bool IsValidMine(Entity mine)
    {
        Unit umine = mine as Unit;

        if (umine == null) {
            return false;
        }
        if (Entity.ValidateTarget(new TargetInfo(mine)) != null) {
            return false;
        }
        if (umine.GoldAmount <= 0) {
            return false;
        }
        if (umine.Type != Global.GameData.Units[(int)UnitTypeId.GoldMine]) {
            return false;
        }

        return true;
    }

    // Returns if storage is valid
    private bool IsValidStorage(Entity storage)
    {
        Unit ustorage = storage as Unit;

        if (ustorage == null) {
            return false;
        }
        if (Entity.ValidateTarget(new TargetInfo(storage)) != null) {
            return false;
        }
        if (ustorage.Player != Entity.Player) {
            return false;
        }
        if (!ustorage.Type.IsStorage) {
            return false;
        }

        return true;
    }

    // Called when the mine is reached.
    private void OnMineReached()
    {
        if (!IsValidMine(Mine)) {
            Stop(false);
            return;
        }
        Unit.GoldAmount = Math.Min(Mine.GoldAmount, ResourceAmount);
        Mine.GoldAmount -= Unit.GoldAmount;
        if (Mine.GoldAmount <= 0) {
            Mine.Kill();
        }

        Entity.RemoveFromMap();
        Wait = MineWaitTime;
        Container = Mine;
    }

    // Called when storage is reached.
    private void OnStorageReached()
    {
        if (!IsValidStorage(Storage)) {
            Stop(false);
            return;
        }
        Unit.Player.GoldAmount += Unit.GoldAmount;
        Unit.GoldAmount = 0;

        Entity.RemoveFromMap();
        Wait = StorageWaitTime;
        Container = Storage;
    }

    public override void NextCycle()
    {
        base.NextCycle();

        if (Container != null && Container.Dead) {
            Unit.Kill();
        }

        // Stopping...
        if (ActionState >= ActionState.Stopping) {
            return;
        }

        if (Wait > 1) {
            --Wait;
            return;
        } else if (Wait == 1) {
            Wait = 0;
            if (!Utilities.PlaceEntityRange(Unit, Container.Map, Container.X, Container.Y, 3 * WarGameData.TileSize)) {
                //Console.WriteLine("{0}: Can't place unit on the map, killing it");
                Unit.Remove();
                return;
            }
        }

        // Pathfinder gave up, damn.
        if (PathStatus == PathStatus.Unreachable) {
            Stop(false);
            return;
        }

        if (Unit.GoldAmount == 0) {
            if (!HasPathGoal) {
                //FindMine();
                if (!IsValidMine(Mine)) {
                    //Console.WriteLine("Can't find mine, give up.");
                    Stop(false);
                    return;
                }

                PathGoal goal = new PathGoal(Mine, 0);
                if (EntityInGoal(goal)) {
                    OnMineReached();
                } else {
                    SetPathGoal(goal);
                }
            } else if (PathStatus == PathStatus.Reached) {
                OnMineReached();
            }
        } else {
            if (!HasPathGoal) {
                FindStorage();
                if (!IsValidStorage(Storage)) {
                    //Console.WriteLine("Can't find storage silo, giving up.");
                    Stop(false);
                    return;
                }

                PathGoal goal = new PathGoal(Storage, 0);
                if (EntityInGoal(goal)) {
                    OnStorageReached();
                } else {
                    SetPathGoal(goal);
                }
            } else if (PathStatus == PathStatus.Reached) {
                OnStorageReached();
            }
        }
    }

    // Constructor.
    public MineGoldAction(): base()
    {
    }

    // Constructor, sets mine and storage.
    // Either can be null or otherwise invalid.
    public MineGoldAction(Unit mine, Unit storage): base()
    {
        if (IsValidMine(mine)) {
            this.Mine = mine;
        } else {
            FindMine();
        }
        if (IsValidStorage(storage)) {
            this.Storage = storage;
        } else {
            FindStorage();
        }
    }
}
