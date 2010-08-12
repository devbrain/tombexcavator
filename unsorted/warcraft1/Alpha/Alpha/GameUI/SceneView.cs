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
using SD = System.Drawing;

using Alpha.UI;
using Alpha.Util;
using Alpha.Graphics;

namespace Alpha.GameUI
{
    // Widget that can draw a scene.
    // This is just a thin abstract class.
    public abstract class SceneView: Widget
    {
        // The scene to draw.
        // Can be null.
        private Alpha.Scene.Scene scene;
        public Alpha.Scene.Scene Scene {
            get {
                return scene;
            }
            set {
                scene = value;
            }
        }

        // Override this in children to do the actual drawing.
        //
        // SceneView.Draw does a lot of setup for you
        // When in DoDraw, you should draw just like you would for
        // a screen sized this.Width * this.Height.
        // Draw takes care of scissor and viewport.
        public abstract void DoDraw();

        // Draw the scene.
        // This function does a lot of setup to prevent spilling
        // over and the like. Actual drawing is done by children in DoDraw()
        public override void Draw(RectangleD ClipRect, RectangleD WindowClipRect)
        {
            PushSelfScissor(ClipRect, WindowClipRect);
            GL.Viewport(GetBoundsInWindow(ClipRect, WindowClipRect).OuterRound().Normalize());
            DoDraw();
            GL.Viewport(0, 0, (int)ParentWindow.Width, (int)ParentWindow.Height);
            GL.PopScissor();
            base.Draw(ClipRect, WindowClipRect);
        }
    }

    // This is a simple SceneView implementation that does orthographic projection.
    public class OrthoSceneView: SceneView
    {
        // View volume.
        // This box is the part of the screen that is drawn.
        private BoundingBox viewVolume;
        public BoundingBox ViewVolume {
            get {
                 return viewVolume;
            }
            set {
                viewVolume = value;
            }
        }

        // Background color;
        private SD.Color backColor = SD.Color.Black;
        public SD.Color BackgroundColor {
            get {
                return backColor;
            }
            set {
                backColor = value;
            }
        }

        // This is the amount of the widget that is actually used to draw.
        // If this is less then one then space will be left black on the sides,
        // but drawing will not be scissored.
        // It's usefull for debugging.
        //
        // A Margin of 1 is the default, and it works as expected.
        // A Margin of 0 doesn't show anything.
        private double margin = 1;
        public double Margin {
            get {
                return margin;
            }
            set {
                margin = value;
            }
        }

        // Draw the scene.
        public override void DoDraw()
        {
            GL.ClearColor(BackgroundColor.R, BackgroundColor.G, BackgroundColor.B, BackgroundColor.A);
            GL.ClearDepth();

            if (Scene == null) {
                return;
            }

            GL.MatrixMode(MatrixMode.Projection);
            GL.PushMatrix();
            GL.LoadIdentity();
            GL.Ortho(-1, 1, 1, -1, -1, 1);

            GL.MatrixMode(MatrixMode.ModelView);
            GL.PushMatrix();
            GL.LoadIdentity();

            GL.Scale(Margin * 2 / (ViewVolume.B - ViewVolume.A));
            GL.Translate(- (ViewVolume.A + ViewVolume.B) / 2);

            Scene.DrawOrtho(ViewVolume);

            GL.MatrixMode(MatrixMode.Projection);
            GL.PopMatrix();
            GL.MatrixMode(MatrixMode.ModelView);
            GL.PopMatrix();
        }
    }
}

