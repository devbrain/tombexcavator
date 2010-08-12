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
using Alpha.Game;
using Alpha.GameUI;

// Base class for unit buttons.
public abstract class UnitButton: ICloneable
{
    // If != UpgradeId.Max then the button will only be visibile
    // if the upgrade is at UpgradeLevelRequired.
    public UpgradeId UpgradeRequired = UpgradeId.Max;

    public int UpgradeLevelRequired;

    // Implement UpgradeRequired stuff.
    public virtual bool GetVisible(IGameScreen screen)
    {
        WarPlayer player = (WarPlayer)screen.Player;
        if (UpgradeRequired != UpgradeId.Max &&
                player.UpgradeLevels[(int)UpgradeRequired] != UpgradeLevelRequired) {
            return false;
        }
        return true;
    }

    // Function called when the button is pressed.
    public abstract void Execute(IGameScreen screen);

    // Icon to display for a certain button.
    public ILazyLink Icon;

    // Position hint.
    public int PositionHint;

    // Convenience funtion, changes visuals (icon/pos).
    public UnitButton Clone()
    {
        return (UnitButton)this.MemberwiseClone();
    }

    // Implement ICloneable.
    object ICloneable.Clone()
    {
        return this.Clone();
    }
}

// Button which cancels targetting mode.
public class CancelTargettingButton: UnitButton
{
    public override bool GetVisible(IGameScreen screen)
    {
        return base.GetVisible(screen) && screen.Targetting;
    }
    
    public override void Execute(IGameScreen screen)
    {
        screen.StopTargetting();
    }
}

// Generic order button.
// FIXME: tends to suck.
public class OrderButton: UnitButton
{
    public Order Order;

    public override void Execute(IGameScreen screen)
    {
        bool queue = screen.QueueOrders;

        foreach (Unit unit in screen.SelectedEntities) {
            ValidationError err;
            if ((err = Order.Validate(unit)) != null) {
                Console.WriteLine(err);
                return;
            }
        }

        screen.Game.SendCommand(new OrderCommand(screen.SelectedEntities, Order, queue));
    }
}

// UnitButton using an ITargeter
public class TargeterButton: UnitButton
{
    public ITargeter Targeter;

    public override void Execute(IGameScreen screen)
    {
        screen.StartTargetting(Targeter);
    }
}

// UnitButton for casting a spell.
public class SpellButton: UnitButton
{
    public Spell Spell;

    // Checks upgrades.
    public override bool GetVisible(IGameScreen screen)
    {
        WarPlayer player = (WarPlayer)screen.Player;
        if (Spell.UpgradeRequired != UpgradeId.Max &&
                player.UpgradeLevels[(int)(Spell.UpgradeRequired)] == 0) {
            return false;
        }

        return base.GetVisible(screen);
    }

    public override void Execute(IGameScreen screen)
    {
        ValidationError err;

        // Validate caster first.
        foreach (Unit unit in screen.SelectedEntities) {
            if ((err = Spell.ValidateCaster(unit)) != null) {
                Console.WriteLine(err);
                return;
            }
        }

        if (Spell.NeedsTarget == false) {
            screen.Game.SendCommand(new OrderCommand(screen.SelectedEntities,
                    new SelfAbilityOrder(Spell), screen.QueueOrders));
        } else {
            ITargeter targeter;
            if (Spell.NeedsUnitTarget) {
                targeter = new CommonTargeter(null, new AbilityOrder(Spell, TargetInfo.Empty));
            } else {
                targeter = new CommonTargeter(new AbilityOrder(Spell, TargetInfo.Empty), null);
            }
            screen.StartTargetting(targeter);
        }
    }
}

// Activity button.
// Works for both training and upgrades.
public class ActivityButton: UnitButton
{
    // Activity to start for this button.
    public ActivityBase Activity;

    // Check visibility.
    public override bool GetVisible(IGameScreen screen)
    {
        Unit unit = (Unit)screen.SelectedEntities[0];
        WarGameState state = (WarGameState)screen.Game.State;
        WarPlayer player = (WarPlayer)screen.Player;

        if (!base.GetVisible(screen)) {
            return false;
        }
        if (unit.Type != Activity.BuilderUnitType) {
            return false;
        }
        if (!(unit.OrderQueue.Action is ActionIdle)) {
            return false;
        }
        // If Allowed fails then hide button.
        if ((int)Activity.AllowTest != -1 && !state.IsAllowed(Activity.AllowTest)) {
            return false;
        }

        // HACK: Upgrade visibility.
        UpgradeActivity uact;
        if ((uact = Activity as UpgradeActivity) != null &&
                (player.UpgradeLevels[(int)uact.UpgradeIdent] != uact.UpgradeLevel - 1 ||
                player.UpgradeResearching[(int)uact.UpgradeIdent])) {
            return false;
        }

        return true;
    }

    // Execute.
    // FIXME: messages in GameScreen.
    public override void Execute(IGameScreen screen)
    {
        ValidationError err;
        Unit unit = (Unit)screen.SelectedEntities[0];
        if ((err = Activity.Validate(screen.SelectedEntities[0])) != null) {
            Console.WriteLine(err);
            return;
        }
        if (!(unit.OrderQueue.Action is ActionIdle)) {
            Console.WriteLine("Unit is busy");
            return;
        }
        screen.Game.SendCommand(new OrderCommand(unit,
                new ActivityOrder(Activity), false));
    }
}

// Cancel buttons for buildings. Invisibile if idle.
public class ActivityStopButton: UnitButton
{
    // Invisible for idle units.
    public override bool GetVisible(IGameScreen screen)
    {
        Unit unit = (Unit)screen.SelectedEntities[0];

        return base.GetVisible(screen) && !(unit.OrderQueue.Action is ActionIdle);
    }

    // Send OrderIdle.
    public override void Execute(IGameScreen screen)
    {
        screen.Game.SendCommand(new OrderCommand(
                    screen.SelectedEntities, Global.GameData.IdleOrder, false));
    }
}
