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
using System.Drawing;

using Alpha.Util;
using Alpha.Graphics;
using Alpha.Game;

namespace Alpha.Scene
{
    // Draws an entity, or an aspect of it.
    public interface IEntityDrawer
    {
        void DrawOrtho(Entity entity, Player p, BoundingBox box);

        BoundingBox GetBounds(Entity entity, Player p);

        int GetDrawLayer(Entity entity, Player p);

        double GetDrawLevel(Entity entity, Player p);
    }

    // Base class for entity drawers.
    //
    // Infinite bounds, 0 layer/level.
    public abstract class EntityDrawerBase: IEntityDrawer
    {
        public abstract void DrawOrtho(Entity entity, Player player, BoundingBox box);

        public virtual BoundingBox GetBounds(Entity entity, Player p)
        {
            return BoundingBox.Infinite;
        }

        public virtual int GetDrawLayer(Entity entity, Player p)
        {
            return 0;
        }

        public virtual double GetDrawLevel(Entity entity, Player p)
        {
            return 0;
        }
    }

    // Draws an entity as a cube.
    public class CubeEntityDrawer: EntityDrawerBase
    {
        // Color for the center of the square.
        public Color InsideColor = Color.Red;

        // Color for the outside of the square.
        public Color OutsideColor = Color.Blue;

        // Dimension of one of the cube's sides.
        public double Size;

        // Draws a cube.
        public override void DrawOrtho(Entity entity, Player player, BoundingBox view)
        {
            double hs = Size / 2;

            GL.PushMatrix();
            GL.Translate(entity.X, entity.Y, 0);
            GL.Enable(Feature.Blend);
            GL.Disable(Feature.Texture);

            GL.Begin(DrawMode.TriangleFan);
            GL.Color(InsideColor);
            GL.Vertex(0, 0, 0);
            GL.Color(OutsideColor);
            GL.Vertex(- hs, - hs, 0);
            GL.Vertex(- hs, + hs, 0);
            GL.Vertex(+ hs, + hs, 0);
            GL.Vertex(+ hs, - hs, 0);
            GL.Vertex(- hs, - hs, 0);
            GL.End();

            GL.PopMatrix();
        }

        // Get the bounds of this object.
        public override BoundingBox GetBounds(Entity entity, Player player)
        {
            Vector3D v = Vector3D.Unit * Size * Math.Sqrt(2) / 2;
            Vector3D c = new Vector3D(entity.X, entity.Y, 0);
            return new BoundingBox(c - v, c + v);
        }
    }
}
