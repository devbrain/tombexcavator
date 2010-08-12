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

using Alpha.Game;
using Alpha.Graphics;
using Alpha.Util;

namespace Alpha.Scene
{
    // Abstract base class for sprite graphics.
    // 
    // This is an IEntityDrawer that draws the entity as a rectangular sprite.
    //
    // A lot of stuff is abstracted, you can probably use this for most
    // frame-based sprites. There's a bunch of stuff for you to override.
    public abstract class SpriteEntityDrawerBase: EntityDrawerBase
    {
        // Get the sprite.
        public abstract Sprite GetSprite(Entity entity, Player player);

        // The frame of animation to draw.
        // HACK: Base implementation only works for ScriptActors.
        // It returns 0 otherwise.
        public virtual int GetFrame(Entity entity, Player player)
        {
            if (entity.Actor is ScriptActor) {
                return ((ScriptActor)entity.Actor).Frame;
            }
            return 0;
        }

        // Return sprite orientation information.
        // Returns null by default which disables the whole mess, so it
        // can be safely ignored if you don't care about SpriteOrienters.
        public virtual SpriteOrienter GetSpriteOrienter(Entity entity, Player player)
        {
            return null;
        }

        // The color to draw the sprite in. Defaults to white, which has no effect.
        public virtual Color GetColor(Entity entity, Player player)
        {
            return Color.White;
        }

        // Override to scale the sprite. This defaults to 1, but this
        // is most likely NOT what you want.
        public virtual double GetScale(Entity entity, Player player)
        {
            return 1;
        }

        // Draw the Entity.
        public override void DrawOrtho(Entity entity, Player player, BoundingBox box)
        {
            if (GetSprite(entity, player) == null) {
                return;
            }
            GL.PushMatrix();
            GL.Translate(entity.X, entity.Y, 0);
            GL.Scale(GetScale(entity, player));
            SpriteOrienter.Draw(
                    GetSprite(entity, player),
                    GetFrame(entity, player),
                    Utilities.EntityOrientationDegrees(entity),
                    GetSpriteOrienter(entity, player));
            GL.PopMatrix();
        }

        // Return the bounding box.
        // By default this is the size of the current frame.
        public override BoundingBox GetBounds(Entity entity, Player player)
        {
            Sprite sprite = GetSprite(entity, player);
            double scale = GetScale(entity, player);

            if (sprite == null) {
                return BoundingBox.Null;
            }

            BoundingBox aabb;

            Vector3D v = new Vector3D(sprite.Width * scale, sprite.Height * scale, 0);
            Vector3D c = new Vector3D(entity.X, entity.Y, 0);
            aabb.A = c - v / 2;
            aabb.B = c + v / 2;

            return aabb;
        }
    }

    // Trivial implementation of SpriteGraphicBase which forwards all
    // meaningfull behaviour to an ISpriteGraphicType, which can double
    // as a factory.
    //
    // You can use this class if you don't need internal
    // state, and can do all the work in an ISpriteGraphicType.
    public class SpriteEntityDrawer: SpriteEntityDrawerBase
    {
        // Link to the sprites used to draw the entity.
        private ILazyLink spritesLink;
        public ILazyLink SpritesLink {
            get {
                return spritesLink;
            }
            set {
                spritesLink = value;
            }
        }

        // Orientation information. Can be null.
        private SpriteOrienter spriteOrienter = null;
        public SpriteOrienter SpriteOrienter {
            get {
                return spriteOrienter;
            }
            set {
                spriteOrienter = value;
            }
        }

        // Scaling factor.
        private double scale = 1;
        public double Scale {
            get {
                return scale;
            }
            set {
                scale = value;
            }
        }

        // The sprite used to draw the entity.
        public override Sprite GetSprite(Entity entity, Player player)
        {
            return (Sprite)SpritesLink.Get();
        }

        // Get orientation information.
        public override SpriteOrienter GetSpriteOrienter(Entity entity, Player player)
        {
            return SpriteOrienter;
        }

        // Get scaling.
        public override double GetScale(Entity entity, Player player)
        {
            return Scale;
        }
    }
}
