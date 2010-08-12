// Copyright (C) 2005 Crestez Leonard    cleonard@go.ro
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

namespace Alpha.GameUI
{
    // This is the basic outline for a GameScreen. You should implement
    // this if you plan to use the Alpha entity buttons system.
    //
    // A GameScreen should probably be a full-screen widget, but it
    // doesn't have to be. It will also probably control a GameView(or more)
    // inside it.
    //
    // Targetting mode is used when you want to give a command that needs
    // a target, for instance when attacking or casting a spell or gathering
    // lumber. You should call StartTargetting, give some information about
    // the kind of targeting you want, and when done call StopTargetting.
    //
    // Right now targeting works for coordinates, and you have to select an entity
    // at that location yourself. I think this is better, for instance you
    // might only want air entities, and the targeter shold decide what entity
    // at that location to use.
    //
    // This might be a problem since you don't target the entity, you target the
    // ground under the entity.
    public interface IGameScreen
    {
        // This returns the Game this GameScreen is interfacing to.
        // Please see the Game class.
        Alpha.Game.Game Game {
            get;
        }

        // Current Player
        Alpha.Game.Player Player {
            get;
        }

        // Current Map. FIXME: hack?
        Alpha.Game.GameMap Map {
            get;
        }

        // The selected entities.
        IList SelectedEntities {
            get;
            set;
        }

        // This should be fired after SelectedEntities is changed.
        // FIXME: remove?
        event EventHandler SelectedEntitiesChanged;

        // If we are in targetting mode. Should start as false.
        // Use StartTargetting/StopTargetting to change this value.
        // Please see the class description for what targetting mode means.
        bool Targetting {
            get;
        }
       
        // Start targetting mode, See ITargeter
        //
        // If already in targetting mode it's just like calling
        // StopTargetting right before this.
        void StartTargetting(ITargeter Targeter);

        // This cancel Targetting mode.
        // SelectTargetCallback is NOT called, this cancels the whole targetting thing.
        // Nothing happens in not in targetting mode.
        void StopTargetting();

        // This returns true of false if orders should be queued or not.
        // You probably just want to check if shift is pressed.
        //
        // This sort of a HACK... but then again it's not that ugly
        // Another possible solution would be to funnel all orders through
        // IGameScreen, but this solution seems nicer somehow.
        bool QueueOrders {
            get;
        }
    }

    // Interface for an unit targetting controller.
    // When in targetting mode IGameScreen delegates actual targetting
    // calls to a targetting controllers.
    //
    // map, x, y is sent instead of a TargetInfo; the controller
    // should select a proper Entity target if required.
    //
    // UnitCommandButtons generally implement TargettingControllers.
    //
    // The IGameScreen is sent as a parameter to enable stateless
    // targetting controllers.
    //
    // FIXME: MouseMove, MouseEnterMap, MouseLeaveMap ?
    public interface ITargeter
    {
        // Called when a target is selected.
        void Select(IGameScreen screen, GameMap map, int x, int y);

        // Called when targetting is stopped by the user.
        void Cancel(IGameScreen screen);
    }

    // This is most likely what you want from a targetting controller.
    //
    // It supports giving arbritary TargetOrders. They can be different
    // for ground and entity targets.
    //
    // This should work in 90% of all target button cases.
    public class CommonTargeter: ITargeter
    {
        // The Order when targetting an Entity.
        public TargetOrder EntityTargetOrder;

        // The Order when targetting the ground.
        // If EntityTargetOrder is non-null then when an Entity is found under
        // the cursor (GetEntity returns non-null) EntityAction is used.
        // If set to null(default) then an entity target is required.
        //
        // If both GroundTargetOrder and EntityTargetOrder are null
        // CheckTargeted will always fail.
        public TargetOrder GroundTargetOrder;

        // If true it will call StopTargetting in Select
        // Defaults to true.
        bool AutoStop = true;

        // Returns an Entity at x, y.
        // Right now it just returns the first entity.
        //
        // null means nothing decent was found.
        //
        // Feel free to override.
        protected virtual Entity GetEntity(GameMap map, int x, int y)
        {
            IList entities;
            entities = map.Cache.SelectList(x, y);
            if (entities.Count == 0) {
                return null;
            } else {
                return (Entity)entities[0];
            }
        }

        // This function returns the order to be given to the selected entities.
        // Return null if you don't like the target.
        // Feel free to override, but you don't really need to.
        protected virtual Order GetOrder(IGameScreen screen, GameMap map, int x, int y)
        {
            if (EntityTargetOrder != null) {
                Entity entity = GetEntity(map, x, y);
                if (entity != null) {
                    return EntityTargetOrder.CloneWithNewTarget(new TargetInfo(entity));
                }
                // If no entity found, fallback to ground target.
            }
            // Check for ground target.
            if (GroundTargetOrder != null) {
                return GroundTargetOrder.CloneWithNewTarget(new TargetInfo(map, x, y));
            }
            // Give up.
            return null;
        }

        // If CheckTarget returns true then this will send the order, yay.
        // This will StopTargetting() even if the target is invalid.
        public void Select(IGameScreen screen, GameMap map, int x, int y)
        {
            Order order = GetOrder(screen, map, x, y);
            if (order == null) {
                Console.WriteLine("Invalid target");
                return;
            }

            bool valid = true;
            foreach (Entity entity in screen.SelectedEntities) {
                ValidationError err;
                if ((err = order.Validate(entity)) != null) {
                    Console.WriteLine(err);
                    valid = false;
                    break;
                }
            }
            if (valid) {
                screen.Game.SendCommand(new OrderCommand(
                            screen.SelectedEntities, order, screen.QueueOrders));
            }
            if (AutoStop) {
                screen.StopTargetting();
            }
        }

        // Nothing here, feel free to override.
        public virtual void Cancel(IGameScreen screen)
        {
        }

        public CommonTargeter()
        {
        }

        // Convenience constructor.
        public CommonTargeter(TargetOrder groundOrder, TargetOrder entityOrder)
        {
            this.GroundTargetOrder = groundOrder;
            this.EntityTargetOrder = entityOrder;
        }
    }
}
