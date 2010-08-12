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
using System.Collections;
using System.Drawing;

using Alpha.UI;
using Alpha.Util;
using Alpha.Graphics;
using Alpha.Game;
using Alpha.Scene;

namespace Alpha.GameUI
{
    // Parameters for a selection event.
    public class SelectEventArgs: EventArgs
    {
        // The list of entities caught in the selection rectangle.
        public readonly IList EntityList;

        // Create a new SelectEventArgs.
        public SelectEventArgs(IList EntityList)
        {
            this.EntityList = EntityList;
        }
    }

    // Event handler for a Selection Event.
    public delegate void SelectEventHandler(object sender, SelectEventArgs args);

    // This class will draw a map in a widget.
    // It adds mouse event in map coordinates.
    public abstract class MapView: Widget
    {
        // The GameMap to observe.
        // can be null, and nothing bad will happen.
        public abstract GameMap Map {
            get;
        }

        public MapView()
        {
            Selection = new SelectionRectGraphic();
        }

        // Selection event. Selection works by firing a SelectEventHandler,
        // taking as a parameter all of the entities in that square.
        public event SelectEventHandler Select;

        // If selection is enabled. If this is false then you get no selection square drawn and no
        // Select event. Note that if this is true you will get a selection square, even if Select
        // is null.
        private bool selectionEnabled = true;
        public bool SelectionEnabled {
            get {
                return selectionEnabled;
            }
            set {
                selectionEnabled = value;
                Selection.Visible &= value;
            }
        }

        // This thing draws a selection rectangle on the screen.
        protected SelectionRectGraphic Selection;

        // If we are selecting.
        public bool SelectionUnderway {
            get {
                return Selection.Visible;
            }
        }

        // Convert screen(widget) coordinates to U-coords
        // You should override this in children to make calculations for your view.
        // They are U-coords, meaning entity positioning.
        abstract public PointD ScreenToMap(PointD screenLocation);

        // Mouse events on the map.
        public event MouseEventHandler MapMouseEvent;

        // Called to fire MapMouseMove.
        // Also handles selection.
        protected void OnMapMouseEvent(MouseEventArgs e)
        {
            if (SelectionEnabled) {
                if (e.EventType == MouseEventType.Movement && SelectionUnderway) {
                    Selection.X2 = e.Location.X;
                    Selection.Y2 = e.Location.Y;
                } else if (e.EventType == MouseEventType.ButtonDown && e.Button == MouseButton.Left) {
                    Selection.Visible = true;
                    Selection.X1 = Selection.X2 = e.Location.X;
                    Selection.Y1 = Selection.Y2 = e.Location.Y;
                } else if (e.EventType == MouseEventType.ButtonUp && e.Button == MouseButton.Left) {
                    Selection.X2 = e.Location.X;
                    Selection.Y2 = e.Location.Y;
                    if (Select != null && Map != null) {
                        Alpha.Util.Rectangle r = Selection.Rect.OuterRound();
                        Select(this, new SelectEventArgs(Map.Cache.SelectList(r.X1, r.Y1, r.X2, r.Y2)));
                    }
                    Selection.Visible = false;
                }
            }
            if (MapMouseEvent != null) {
                MapMouseEvent(this, e);
            }
        }

        // Override to generate map mouse events.
        public override void OnMouseEvent(MouseEventArgs e)
        {
            OnMapMouseEvent(new MouseEventArgs(e.EventType,
                        ScreenToMap(e.Location), e.ButtonState, e.Button));
            base.OnMouseEvent(e);
        }

        public override void OnLoseMouseFocus()
        {
            Selection.Visible = false;
            base.OnLoseMouseFocus();
        }
    }

    // MapView using an OrthoSceneView
    public class OrthoMapView: MapView
    {
        // OrthoSceneView used to draw the map.
        public readonly OrthoSceneView View;

        private IMapSceneBuilder mapSceneBuilder;
        public IMapSceneBuilder MapSceneBuilder {
            get {
                return mapSceneBuilder;
            }
            set {
                if (MapSceneBuilder != null) {
                    //MapSceneBuilder.Dispose();
                }
                mapSceneBuilder = value;
                if (Scene != null) {
                    View.Scene = Scene;
                    View.Scene.DrawOrderComparer = new DrawOrderComparer();
                    View.Scene.AddObject(Selection);
                }
                SetView(CenterX, CenterY, Zoom);
            }
        }

        // Get the scene.
        public Alpha.Scene.Scene Scene {
            get {
                if (MapSceneBuilder == null) {
                    return null;
                }
                return MapSceneBuilder.Scene;
            }
        }

        // The map drawn by this view.
        public override GameMap Map {
            get {
                if (MapSceneBuilder == null) {
                    return null;
                }
                return MapSceneBuilder.Map;
            }
        }

        // X center of the view.
        private double centerX;
        public double CenterX {
            get {
                return centerX;
            }
            set {
                SetView(value, CenterY, Zoom);
            }
        }

        // Y center of the view.
        private double centerY;
        public double CenterY {
            get {
                return centerY;
            }
            set {
                SetView(CenterX, value, Zoom);
            }
        }

        // Zoom of the view.
        // This is the size of an entity coord in widget space.
        // You probably want small values.
        private double zoom;
        public double Zoom {
            get {
                return zoom;
            }
            set {
                SetView(CenterX, CenterY, value);
            }
        }

        // If this is true then the view is clamped to the map.
        private bool clampView = false;
        public bool ClampView {
            get {
                return clampView;
            }
            set {
                clampView = value;
                SetView(CenterX, CenterY, Zoom);
            }
        }

        // Called when view options are changed.
        // This is responsible for clamping.
        private void SetView(double cx, double cy, double z)
        {
            BoundingBox box;

            if (ClampView && Map != null) {
                double mw = Map.Width;
                double mh = Map.Height;
                
                if (Width / Zoom > mw) {
                    cx = mw / 2;
                } else {
                    cx = Math.Max(cx, Width / (2 * Zoom));
                    cx = Math.Min(cx, mw - Width / (2 * Zoom));
                }

                if (Height / Zoom > mh) {
                    cy = mh / 2;
                } else {
                    cy = Math.Max(cy, Height / (2 * Zoom));
                    cy = Math.Min(cy, mh - Height / (2 * Zoom));
                }
            }

            this.centerX = cx;
            this.centerY = cy;
            this.zoom = z;

            // Calculate the bounding box we draw.
            box.A.X = ((- Width / 2) / Zoom + CenterX);
            box.A.Y = ((- Height / 2) / Zoom + CenterY);
            box.A.Z = -1000;
            box.B.X = ((+ Width / 2) / Zoom + CenterX);
            box.B.Y = ((+ Height / 2) / Zoom + CenterY);
            box.B.Z = 1000;

            View.ViewVolume = box;
        }

        // Map screen to map coordinates.
        public override PointD ScreenToMap(PointD screenLocation)
        {
            return new PointD(
                    (screenLocation.X - Width / 2) / Zoom + CenterX,
                    (screenLocation.Y - Height / 2) / Zoom + CenterY);
        }

        // Constructor.
        public OrthoMapView()
        {
            View = new OrthoSceneView();
            this.Add(View);
            SetView(0, 0, 1);
            View.SetSize(0, 0, Width, Height);
        }

        // Override SetSize to resize the scene view.
        public override void SetSize(double x, double y, double width, double height)
        {
            base.SetSize(x, y, width, height);
            View.SetSize(0, 0, width, height);
            SetView(CenterX, CenterY, Zoom);
        }
    }

    // Graphic object representing a selection rectangle.
    public class SelectionRectGraphic: GraphicObjectBase, IDrawOrderable
    {
        // Color for the selection margin.
        public Color SelectionMarginColor = Color.FromArgb(255, 32, 32, 128);

        // Color for the selection interior.
        // Transparent color stronly recommended.
        public Color SelectionInteriorColor = Color.FromArgb(64, 32, 32, 128);

        // If the rectangle is visible
        public bool Visible = false;

        // Selection rectangle
        private RectangleD rect;
        public RectangleD Rect {
            get {
                return rect;
            }
            set {
                rect = value;
            }
        }

        public double X1 {
            get {
                return Rect.X1;
            }
            set {
                rect.X1 = value;
            }
        }

        public double Y1 {
            get {
                return Rect.Y1;
            }
            set {
                rect.Y1 = value;
            }
        }

        public double X2 {
            get {
                return Rect.X2;
            }
            set {
                rect.X2 = value;
            }
        }

        public double Y2 {
            get {
                return Rect.Y2;
            }
            set {
                rect.Y2 = value;
            }
        }

        // IGraphicObject.GetBounds()
        public override BoundingBox GetBounds()
        {
            BoundingBox res;

            res.A = new Vector3D(X1, Y1, 0);
            res.B = new Vector3D(X2, Y2, 0);

            return res;
        }

        // IGraphicObject.DrawOrtho()
        public override void DrawOrtho(BoundingBox viewVolume)
        {
            if (!Visible) {
                return;
            }

            GL.Disable(Feature.Texture);
            GL.Enable(Feature.Blend);
            GL.Begin(DrawMode.Quads);
            GL.Color(SelectionInteriorColor);
            GL.Vertex(X1, Y1);
            GL.Vertex(X2, Y1);
            GL.Vertex(X2, Y2);
            GL.Vertex(X1, Y2);
            GL.End();

            GL.Begin(DrawMode.LineLoop);
            GL.Color(SelectionMarginColor);
            GL.Vertex(X1, Y1);
            GL.Vertex(X2, Y1);
            GL.Vertex(X2, Y2);
            GL.Vertex(X1, Y2);
            GL.End();
        }

        public int GetDrawLayer()
        {
            return 10000;
        }

        public double GetDrawLevel()
        {
            return 0;
        }
    }
}
