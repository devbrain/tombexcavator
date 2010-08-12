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
using System.Drawing;
using System.Collections;

using Alpha.Util;
using Alpha.Graphics;
using Alpha.Game;
using Alpha.Scene;

namespace Alpha.GameUI
{
    // Graphic object which draws the current selection.
    //
    // It uses a bunch of IEntityDrawers for the actual drawing.
    public class SelectionGraphic: GraphicObjectBase, IDrawOrderable
    {
        // POV player
        public Player Player;

        // List of selected entities.
        public IList SelectedEntities;

        // List of IEntityDrawers.
        public IList Drawers;

        // Draw selection stuff.
        public override void DrawOrtho(BoundingBox box)
        {
            if (SelectedEntities == null || Drawers == null) {
                return;
            }

            foreach (Entity entity in SelectedEntities) {
                foreach (IEntityDrawer drawer in Drawers) {
                    drawer.DrawOrtho(entity, Player, box);
                }
            }
        }

        // HACK: On top of everything.
        public int GetDrawLayer()
        {
            return 10000;
        }

        public double GetDrawLevel()
        {
            return 0;
        }

        // Default constructor.
        public SelectionGraphic()
        {
            Drawers = new ArrayList();
        }

        // Creates an instance with some default drawers.
        public static SelectionGraphic CreateDefault()
        {
            SelectionGraphic graphic = new SelectionGraphic();

            graphic.Drawers = new ArrayList();
            graphic.Drawers.Add(new EntityHighlightDrawer());
            graphic.Drawers.Add(new EntityPathDrawer());
            graphic.Drawers.Add(OrderQueueDrawer.CreateDefault());

            return graphic;
        }
    }

    // Highlights an entity. Color depends on Player relation.
    // FIXME: hacked to use TileLocator size.
    public class EntityHighlightDrawer: EntityDrawerBase
    {
        // The color of the selection square for owned entities.
        public Color OwnedColor = Color.Green;

        // The color of the selection square for friendly entities.
        public Color FriendlyColor = Color.Yellow;

        // The color of the selection square for enemy entities.
        public Color EnemyColor = Color.Red;

        // Draw the path for an entity
        public override void DrawOrtho(Entity entity, Player player, BoundingBox box)
        {
            // Draw selection square.
            if (entity.Locator as TileLocator == null) {
                return;
            }

            // FIXME FIXME FIXME: This depends on the tile pathfinder.
            int rad = ((TileLocator)entity.Locator).Size / 2;

            // Get color.
            Color col;

            if (player == null) {
                col = OwnedColor;
            } else {
                if (player == entity.Player) {
                    col = OwnedColor;
                } else if (player.ShouldAttack(entity)) {
                    col = EnemyColor;
                } else {
                    col = FriendlyColor;
                }
            }

            GL.Disable(Feature.Texture);
            GL.Color(col);
            GL.Begin(DrawMode.LineLoop);
            GL.Vertex(entity.X - rad, entity.Y - rad);
            GL.Vertex(entity.X - rad, entity.Y + rad);
            GL.Vertex(entity.X + rad, entity.Y + rad);
            GL.Vertex(entity.X + rad, entity.Y - rad);
            GL.End();
        }
    }

    // Draws an entity's path.
    public class EntityPathDrawer: EntityDrawerBase
    {
        // The color of the movement path
        public Color Color = Color.Gray;

        // Draw the path for an entity
        public override void DrawOrtho(Entity entity, Player player, BoundingBox box)
        {
            int i;

            if (entity.Locator == null) {
                return;
            }
            IList path = entity.Locator.GetPath();
            if (path == null) {
                return;
            }

            GL.Disable(Feature.Texture);
            GL.Color(Color);
            GL.Begin(DrawMode.LineStrip);
            GL.Vertex(entity.X, entity.Y, 0);
            for (i = 1; i < path.Count; ++i) {
                GL.Vertex(((Alpha.Util.Point)path[i]).X, ((Alpha.Util.Point)path[i]).Y, 0);
            }
            GL.End();
        }
    }

    // Draws an unit's order queue.
    //
    // NOTE: By default this doesn't draw anything, you have to add
    // your own drawers. You can use CreateDefault.
    public class OrderQueueDrawer: EntityDrawerBase
    {
        // If true it will draw entity actions (using ActionDrawers).
        public bool DrawActions = true;

        // If true it will draw entity actions (using OrderDrawers).
        // If you set this to true you probably also want to set DrawActions to true.
        public bool DrawOrders = true;

        // List of IActionDrawer. The first drawer that matched (CanDrawAction is true)
        // will be used.
        public IList ActionDrawers;

        // List of IOrderDrawer. The first drawer that matched (CanDrawOrder is true)
        // will be used.
        public IList OrderDrawers;

        // Creates an instance with some default drawers.
        public static OrderQueueDrawer CreateDefault()
        {
            OrderQueueDrawer drawer = new OrderQueueDrawer();

            drawer.DrawActions = true;
            drawer.DrawOrders = true;
            drawer.ActionDrawers = new ArrayList();
            drawer.ActionDrawers.Add(new GuardActionDrawer());
            drawer.ActionDrawers.Add(new HuntActionDrawer());
            drawer.ActionDrawers.Add(new TargetActionDrawer());
            drawer.OrderDrawers = new ArrayList();
            drawer.OrderDrawers.Add(new TargetOrderDrawer());

            return drawer;
        }

        // Draw the monkey.
        public override void DrawOrtho(Entity entity, Player p, BoundingBox box)
        {
            if (entity.OrderQueue == null) {
                return;
            }

            TargetInfo target = new TargetInfo(entity);
            if (DrawActions) {
                IAction act = entity.OrderQueue.Action;
                foreach (IActionDrawer drawer in ActionDrawers) {
                    if (drawer.CanDrawAction(act)) {
                        target = drawer.DrawAction(act);
                        break;
                    }
                }
            }
            if (DrawOrders) {
                IList orders = entity.OrderQueue.GetOrders();
                foreach (Order o in orders) {
                    foreach (IOrderDrawer drawer in OrderDrawers) {
                        if (drawer.CanDrawOrder(entity, o)) {
                            target = drawer.DrawOrder(entity, o, target);
                            break;
                        }
                    }
                }
            }
        }

        // Draws a line of a certain color between two TargetInfos
        public static void DrawLine(TargetInfo start, TargetInfo end, Color color)
        {
            GL.Disable(Feature.Texture);
            GL.Color(color);
            GL.Begin(DrawMode.Lines);
            GL.Vertex(start.X, start.Y, 0);
            GL.Vertex(end.X, end.Y, 0);
            GL.End();
        }
    }

    // Interface for helper classes that can draw an entity's current action.
    public interface IActionDrawer
    {
        // Returns true or false if this object can draw a certain action.
        bool CanDrawAction(IAction act);

        // Draw a certain action.
        //
        // Feel free to throw up if CanDrawAction is false.
        //
        // Returns the "expected entity position on termination". This is
        // used further on to draw orders.
        // You can return the entity itself if you don't care about that.
        TargetInfo DrawAction(IAction act);
    }

    // Interface for helper classes that can draw an order
    public interface IOrderDrawer
    {
        // Returns true or false if this object can draw a certain order.
        bool CanDrawOrder(Entity u, Order o);

        // Draw a certain order.
        //
        // Feel free to throw up if CanDrawOrder is false.
        //
        // start is the "target" of the previous order. In case of a move
        // order that would be the move target.
        //
        // This should return the "target" as well, the start parameter
        // to be given when drawing the next order. You can return the entity itself
        // as a target if you don't care.
        TargetInfo DrawOrder(Entity u, Order o, TargetInfo start);
    }

    // Draws an unidentified ITargetAction. You are advised to make better
    // drawers for your actions.
    //
    // This will the a simple line from the previous target to this target.
    public class TargetActionDrawer: IActionDrawer
    {
        // Line color, defaults to green.
        public Color LineColor = Color.Green;

        public bool CanDrawAction(IAction act)
        {
            return act is ITargetAction;
        }

        public TargetInfo DrawAction(IAction act)
        {
            TargetInfo tgt = ((ITargetAction)act).Target;
            OrderQueueDrawer.DrawLine(new TargetInfo(act.Entity), tgt, LineColor);
            return tgt;
        }
    }

    // Draws an unidentified TargetOrder. You are advised to make better
    // drawers for your actions.
    //
    // This will the a simple line from the previous target to this target.
    public class TargetOrderDrawer: IOrderDrawer
    {
        // Line color, defaults to green.
        public Color LineColor = Color.Green;

        public bool CanDrawOrder(Entity u, Order order)
        {
            return order is TargetOrder;
        }

        public TargetInfo DrawOrder(Entity u, Order order, TargetInfo start)
        {
            TargetInfo tgt = ((TargetOrder)order).Target;
            OrderQueueDrawer.DrawLine(start, tgt, LineColor);
            return tgt;
        }
    }

    // Draw ActionMoveFindTarget
    public class HuntActionDrawer: IActionDrawer
    {
        // Color to draw movement
        public Color MovementColor = Color.Green;

        // Color to draw aquired target
        public Color FoundTargetColor = Color.Red;

        // If this is set to true, the movement line is drawn from
        // the found target to the movement target.
        // Otherwise you get the default behaviour of drawing both starting
        // from the current entity position;
        public bool MovementFromTarget = false;

        public bool CanDrawAction(IAction act)
        {
            return act is HuntAction;
        }

        public TargetInfo DrawAction(IAction a)
        {
            HuntAction act = (HuntAction)a;
            TargetInfo tgt = new TargetInfo(act.Entity);
            if (act.HasFoundTarget) {
                OrderQueueDrawer.DrawLine(tgt, act.FoundTarget, FoundTargetColor);
                if (MovementFromTarget) {
                    tgt = act.FoundTarget;
                }
            }
            OrderQueueDrawer.DrawLine(tgt, act.Target, MovementColor);
            return act.Target;
        }
    }

    // Draw GaurdAction
    public class GuardActionDrawer: IActionDrawer
    {
        // Color to draw the line to the guard target.
        public Color GuardTargetColor = Color.Green;

        // Color to draw aquired target
        public Color FoundTargetColor = Color.Red;

        // If this is set to true, the movement line is drawn from
        // the found target to the guard target.
        public bool GuardFromTarget = true;

        // If this is set to true, the ranges are filled, otherwise only the contour is shown.
        public bool FilledRanges = true;

        public bool CanDrawAction(IAction act)
        {
            return act is GuardAction;
        }

        public TargetInfo DrawAction(IAction a)
        {
            GuardAction act = (GuardAction)a;
            TargetInfo tgt = new TargetInfo(act.Entity);
            if (act.HasFoundTarget) {
                OrderQueueDrawer.DrawLine(tgt, act.FoundTarget, FoundTargetColor);
                if (GuardFromTarget) {
                    tgt = act.FoundTarget;
                }
            }
            OrderQueueDrawer.DrawLine(tgt, act.GuardTarget, GuardTargetColor);
            tgt = act.GuardTarget;

            return tgt;
        }
    }
}

