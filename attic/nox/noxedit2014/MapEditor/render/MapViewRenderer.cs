/*
 * MapEditor
 * 
 * Пользователь: AngryKirC
 * Copyleft - PUBLIC DOMAIN
 * Дата: 09.10.2014
 */
using System;
using System.Drawing;
using System.Drawing.Drawing2D;
using System.Windows.Forms;
using System.Collections.Generic;
using NoxShared;
using MapEditor.videobag;
using MapEditor.MapInt;
using WallFacing = NoxShared.Map.Wall.WallFacing;

namespace MapEditor.render
{
    /// <summary>
    /// Class responsible for drawing the Nox map.
    /// </summary>
    public class MapViewRenderer
    {
    	const int squareSize = MapView.squareSize;
        private readonly MapView mapView;
        internal newgui.MapObjectCollection SelectedObjects
        {
        	get
        	{
        		return mapView.SelectedObjects;
        	}
        }
        internal Map Map
        {
            get
            {
            	return MapInterface.TheMap;
            }
        }
        private readonly Font drawFont;
        private VideoBagCachedProvider videoBagProvider = null;
        public VideoBagCachedProvider VideoBag
        {
        	get
        	{
        		if (videoBagProvider == null)
                	videoBagProvider = new VideoBagCachedProvider();
            	return videoBagProvider;
        	}
        }
        private readonly ObjectRenderer objectRenderer;
        private readonly TileRenderer floorRenderer;

        public MapViewRenderer(MapView mapView)
        {
            this.mapView = mapView;
            this.drawFont = new Font("Arial", 9.4F);
            this.objectRenderer = new ObjectRenderer(this);
            this.floorRenderer = new TileRenderer(this);
        }

        public ColorLay ColorLayout = new ColorLay();
        public class ColorLay
        {
            public Pen Tiles;
            public Pen Tiles2;
            public Brush Walls;
            public Color Background;
            public Pen Objects;
            public Color WallsBreakable;
            public Color WallsWindowed;
            public Color WallsSecret;
            public Color Selection;
            public Color Removing;
            public Pen WaypointNorm;
            public Pen WaypointSel;
            public Pen WaypointTwoPath;

            public ColorLay()
            {
                ResetColors();
            }
            
            public void InvertColors()
            {
                Tiles2 = Pens.Blue;
                Tiles = Pens.Green;
                Walls = Brushes.Black;
                Background = Color.White;
                Objects = Pens.Red;
            }
            
            public void ResetColors()
            {
                Tiles2 = Pens.Yellow;
                Tiles = Pens.Gray;
                Walls = Brushes.White;
                Background = Color.Black;
                Objects = Pens.Blue;
                WallsBreakable = Color.Red;
                WallsWindowed = Color.Orange;
                WallsSecret = Color.Green;
                Selection = Color.Green;
                Removing = Color.Red;
                WaypointNorm = Pens.Olive;
                WaypointSel = Pens.Aqua;
                WaypointTwoPath = Pens.Orange;
            }
        }
        
        /// <summary>
        /// Wall texture rendering routine
        /// </summary>
        public void DrawTexturedWall(Graphics g, Map.Wall wall, bool transparent, bool beingSelected)
        {
            ThingDb.Wall tt = ThingDb.Walls[wall.matId];
            try
            {
            	int actualVari = (int) wall.Variation * 2;
                // для обычных стен все данные берутся отсюда
                ThingDb.Wall.WallRenderInfo wri = tt.RenderNormal[(int)wall.Facing][actualVari];
                // если стену можно сломать
                if (wall.Destructable)
                    wri = tt.RenderBreakable[(int)wall.Facing][actualVari];
                // если стена содержит окошко
                if (wall.Window)
                {
                    switch (wall.Facing)
                    {
                        case WallFacing.NORTH:
                            wri = tt.RenderNormal[11][actualVari];
                            break;
                        case WallFacing.WEST:
                            wri = tt.RenderNormal[12][actualVari];
                            break;
                    }
                }

                // достаем картинку
                Bitmap bitmap = VideoBag.GetBitmap(wri.SpriteIndex);
                BitmapShader shader = null;
                // тонируем если стена необычная
                if (wall.Destructable || wall.Secret || transparent || beingSelected)
                {
                    shader = new BitmapShader(bitmap);
                    shader.LockBitmap();
                    if (wall.Destructable)
                        shader.ColorShade(ColorLayout.WallsBreakable, 0.15F);
                    if (wall.Secret)
                        shader.ColorShade(ColorLayout.WallsSecret, 0.15F);
                    if (beingSelected)
                    {
                    	Color selCol = Color.Purple;
                    	if (MapInterface.CurrentMode == EditMode.WALL_PLACE)
                    		selCol = ColorLayout.Removing;
                    	shader.ColorGradWaves(selCol, 4F, Environment.TickCount);
                    	
                    }
                    if (transparent)
                        shader.MakeSemitransparent();
                    bitmap = shader.UnlockBitmap();
                }
                // допускается что стена одновременно и секретная, и разрушаемая, и с окном
                int x, y;
                if (bitmap != null)
                {
                    x = 23 * wall.Location.X;
                    y = 23 * wall.Location.Y;
                    // коррекция координат
                    int offX = (0 - wri.unknown1) - videoBagProvider.DrawOffsets[wri.SpriteIndex][0];
                    int offY = wri.unknown2 - videoBagProvider.DrawOffsets[wri.SpriteIndex][1];
                    x -= offX + 50;
                    y -= offY + 72;
                    // собственно рисуем
                    g.DrawImage(bitmap, x, y, bitmap.Width, bitmap.Height);
                    // сразу чистим память если картинка не кэшируется
                    if (shader != null) bitmap.Dispose();
                }
            }
            catch (Exception) { }
        }

        private void RenderPostWalls(Graphics g)
        {
        	if (MapInterface.CurrentMode == EditMode.WALL_PLACE && EditorSettings.Default.Edit_PreviewMode)
            {
        		Point pt = MapView.GetNearestWallPoint(mapView.mouseLocation);
                // Render the wall being created (if there is no other in place)
                if (mapView.WallMakeNewCtrl != null && !Map.Walls.ContainsKey(pt))
                {
                    Map.Wall fakeWall = mapView.WallMakeNewCtrl.NewWall(pt, true);
                    DrawTexturedWall(g, fakeWall, true, false);
                }
            }
        }
       
        /*
        private void RenderPostTiles(Graphics g)
        {
        	if (MapInterface.CurrentMode == EditMode.FLOOR_PLACE && MapEditorSettings.DrawTextured)
        	{
        		Point point = MapView.GetNearestTilePoint(mapView.mouseLocation);
        		// Render the tile being created
        		if (!Map.Tiles.ContainsKey(point))
        		{
	                Map.Tile tile = mapView.TileMakeNewCtrl.GetTile(point,true);
	                DrawTexturedTile(g, tile, true);
        		}
        	}
        }
        */

        bool updCanvasObjects = true;
        bool updCanvasTiles = true;
        
        public void UpdateCanvas(bool objects, bool tiles)
        {
        	// This is correct logic because we won't override canvas status in the same frame
        	if (objects)
        		updCanvasObjects = true;
        	if (tiles)
        		updCanvasTiles = true;
        }
        
        /// <summary>
        /// Renders the map using current settings and editing stuff. 
        /// </summary>
        public void RenderTo(Graphics g)
        {
            PointF nwCorner, neCorner, seCorner, swCorner, center;
            bool DrawTextured = EditorSettings.Default.Edit_PreviewMode;
            bool DrawExtents3D = EditorSettings.Default.Draw_Extents_3D;
            bool DrawText = EditorSettings.Default.Draw_AllText;
            // enable transparency only in textured preview mode
            if (DrawTextured) g.CompositingMode = CompositingMode.SourceOver;
            else g.CompositingMode = CompositingMode.SourceCopy;
            // optimizations
            g.CompositingQuality = CompositingQuality.HighSpeed;
            g.PixelOffsetMode = PixelOffsetMode.HighSpeed;
            g.InterpolationMode = InterpolationMode.Low;
            g.SmoothingMode = SmoothingMode.HighSpeed;

            // expand clip rectangle a bit
            const int sqSize2 = squareSize * 2;
            Rectangle clip = new Rectangle((int)g.ClipBounds.X - sqSize2, (int)g.ClipBounds.Y - sqSize2, (int)g.ClipBounds.Width + sqSize2, (int)g.ClipBounds.Height + sqSize2);
            if (updCanvasObjects) 
            {
            	objectRenderer.UpdateCanvas(clip);
            	updCanvasObjects = false;
            }
            if (updCanvasTiles)
            {
            	floorRenderer.UpdateCanvas(clip);
            	updCanvasTiles = false;
            }
            // Paint it black
            Size size = mapView.mapPanel.Size;
            g.Clear(ColorLayout.Background);
            Point mouseLocation = mapView.mouseLocation;
            Pen pen;

            // Draw tiles and edges
            if (MapInterface.CurrentMode == EditMode.FLOOR_PLACE || EditorSettings.Default.Draw_FloorTiles || MapInterface.CurrentMode == EditMode.EDGE_PLACE)
            	floorRenderer.Render(g);
            
            // Draw grid
            if (EditorSettings.Default.Draw_Grid)
            {
                using (pen = new Pen(Color.Gray, 1F))
                {
                    //draw the grid sloppily (an extra screen's worth of lines along either axis)
                    for (int x = -squareSize * (size.Width / squareSize) - 3 * squareSize / 2 % (2 * squareSize); x < 2 * size.Width; x += 2 * squareSize)
                    {
                        int y = -3 * squareSize / 2 % (2 * squareSize);
                        g.DrawLine(pen, new Point(x - 1, y), new Point(y, x - 1));
                        g.DrawLine(pen, new Point(x, y), new Point(size.Width + x, size.Width + y));
                    }
                }
            }
            
            if (MapInterface.CurrentMode >= EditMode.FLOOR_PLACE && MapInterface.CurrentMode <= EditMode.EDGE_PLACE)
            {
                // Draw the overlay to show tile location
           		Point pt = new Point(mouseLocation.X, mouseLocation.Y);
                PointF tilePt = (PointF) MapView.GetNearestTilePoint(pt);
                int squareSize2 = squareSize;

                int bs = (int)MainWindow.Instance.mapView.TileMakeNewCtrl.BrushSize.Value;

                if (MapInterface.CurrentMode == EditMode.FLOOR_BRUSH)
                {
                	squareSize2 *= bs;
                	if (bs > 1)
                	{
                        tilePt.X -= (float)(-0.5 + 0.5 * bs);
                        tilePt.Y -= (float)(1.5 + 0.5 * bs);
                	}
                }
             
                // Change overlay color depending on editor EditMode
                Color tileOverlayCol = Color.Yellow;
                if (MapInterface.CurrentMode == EditMode.EDGE_PLACE) tileOverlayCol = Color.Aqua;
                if (MapInterface.CurrentMode == EditMode.FLOOR_BRUSH) tileOverlayCol = Color.LawnGreen;
                    
              	tilePt.X *= squareSize;
               	tilePt.Y *= squareSize;

                center = new PointF(tilePt.X + squareSize / 2f, tilePt.Y + (3 / 2f) * squareSize);
                nwCorner = new PointF(tilePt.X - squareSize2 / 2f, tilePt.Y + (3 / 2f) * squareSize2);
                neCorner = new PointF(nwCorner.X + squareSize2, nwCorner.Y - squareSize2);
                swCorner = new PointF(nwCorner.X + squareSize2, nwCorner.Y + squareSize2);
                seCorner = new PointF(neCorner.X + squareSize2, neCorner.Y + squareSize2);
               
                g.DrawPolygon(new Pen(tileOverlayCol, 2), new PointF[] { nwCorner, neCorner, seCorner, swCorner });
            }
            
            Pen destructablePen = new Pen(ColorLayout.WallsBreakable, 2);
            Pen windowPen = new Pen(ColorLayout.WallsWindowed, 2);
            Pen secretPen = new Pen(ColorLayout.WallsSecret, 2);
            Pen invisiblePen = new Pen(Color.DarkGray, 2);
            Pen wallPen = new Pen(ColorLayout.Walls, 2);

            // Draw [BELOW] objects
            objectRenderer.RenderBelow(g);
            
            // Draw walls
            if (EditorSettings.Default.Draw_Walls)
            {
            	Map.Wall removing = mapView.GetWallUnderCursor();
	            foreach (Map.Wall wall in Map.Walls.Values)
	            {
	                Point pt = wall.Location;
	                int x = pt.X * squareSize, y = pt.Y * squareSize;
	                Point txtPoint = new Point(x, y);
	                if (clip.Contains(x, y))
	                {
	                    if (DrawTextured)
	                    {
	                        DrawTexturedWall(g, wall, false, removing == wall);
	                        continue;
	                    }
	
	                    //TODO: how to draw if a destructable window? is this even possible?
	                    if (wall.Destructable)
	                        pen = destructablePen;
	                    else if (wall.Window)
	                        pen = windowPen;
	                    else if (wall.Secret)
	                        pen = secretPen;
	                    else if (wall.Material.Contains("Invisible"))
	                        pen = invisiblePen;
	                    else
	                        pen = wallPen;
	
	                    center = new PointF(x + squareSize / 2f, y + squareSize / 2f);
	                    Point nCorner = new Point(x, y);
	                    Point sCorner = new Point(x + squareSize, y + squareSize);
	                    Point wCorner = new Point(x + squareSize, y);
	                    Point eCorner = new Point(x, y + squareSize);
	
	                    Point nCornerUp = new Point(x, y - 40);
	                    Point sCornerUp = new Point(x + squareSize, y + squareSize - 40);
	                    Point wCornerUp = new Point(x + squareSize, y - 40);
	                    Point eCornerUp = new Point(x, y + squareSize - 40);
	                    PointF centerUp = new PointF(x + squareSize / 2f, (y + squareSize / 2f) - 40);
	
	                    switch (wall.Facing)
	                    {
	                        case Map.Wall.WallFacing.NORTH:
	                            g.DrawLine(pen, wCorner, eCorner);
	                            if (!DrawExtents3D)
	                                break;
	                            g.DrawLine(pen, wCornerUp, eCornerUp);
	
	                            g.DrawLine(pen, wCornerUp, wCorner);
	                            g.DrawLine(pen, eCornerUp, eCorner);
	
	                            break;
	                        case Map.Wall.WallFacing.WEST:
	                            g.DrawLine(pen, nCorner, sCorner);
	                            if (!DrawExtents3D)
	                                break;
	                            g.DrawLine(pen, nCornerUp, sCornerUp);
	
	                            g.DrawLine(pen, nCorner, nCornerUp);
	                            g.DrawLine(pen, sCorner, sCornerUp);
	                            break;
	                        case Map.Wall.WallFacing.CROSS:
	                            g.DrawLine(pen, wCorner, eCorner);//north wall
	                            g.DrawLine(pen, nCorner, sCorner);//south wall
	                            if (!DrawExtents3D)
	                                break;
	                            g.DrawLine(pen, wCornerUp, eCornerUp);//north wall
	                            g.DrawLine(pen, nCornerUp, sCornerUp);//south wall
	
	                            g.DrawLine(pen, wCorner, wCornerUp);
	                            g.DrawLine(pen, nCorner, nCornerUp);
	                            g.DrawLine(pen, sCorner, sCornerUp);
	                            g.DrawLine(pen, eCorner, eCornerUp);
	
	                            break;
	                        case Map.Wall.WallFacing.NORTH_T:
	                            g.DrawLine(pen, wCorner, eCorner);//north wall
	                            g.DrawLine(pen, center, sCorner);//tail towards south
	                            if (!DrawExtents3D)
	                                break;
	                            g.DrawLine(pen, wCornerUp, eCornerUp);//north wall
	                            g.DrawLine(pen, centerUp, sCornerUp);//tail towards south
	
	                            g.DrawLine(pen, wCorner, wCornerUp);
	                            g.DrawLine(pen, eCorner, eCornerUp);
	                            g.DrawLine(pen, sCorner, sCornerUp);
	                            g.DrawLine(pen, center, centerUp);
	
	                            break;
	                        case Map.Wall.WallFacing.SOUTH_T:
	                            g.DrawLine(pen, wCorner, eCorner);//north wall
	                            g.DrawLine(pen, center, nCorner);//tail towards north
	                            if (!DrawExtents3D)
	                                break;
	                            g.DrawLine(pen, wCornerUp, eCornerUp);//north wall
	                            g.DrawLine(pen, centerUp, nCornerUp);//tail towards south
	
	                            g.DrawLine(pen, wCorner, wCornerUp);
	                            g.DrawLine(pen, eCorner, eCornerUp);
	                            g.DrawLine(pen, nCorner, nCornerUp);
	                            g.DrawLine(pen, center, centerUp);
	                            break;
	                        case Map.Wall.WallFacing.WEST_T:
	                            g.DrawLine(pen, nCorner, sCorner);//west wall
	                            g.DrawLine(pen, center, eCorner);//tail towards east
	                            if (!DrawExtents3D)
	                                break;
	                            g.DrawLine(pen, nCornerUp, sCornerUp);//north wall
	                            g.DrawLine(pen, centerUp, eCornerUp);//tail towards south
	
	                            g.DrawLine(pen, nCorner, nCornerUp);
	                            g.DrawLine(pen, sCorner, sCornerUp);
	                            g.DrawLine(pen, eCorner, eCornerUp);
	                            g.DrawLine(pen, center, centerUp);
	                            break;
	                        case Map.Wall.WallFacing.EAST_T:
	                            g.DrawLine(pen, nCorner, sCorner);//west wall
	                            g.DrawLine(pen, center, wCorner);//tail towards west
	                            if (!DrawExtents3D)
	                                break;
	                            g.DrawLine(pen, nCornerUp, sCornerUp);//north wall
	                            g.DrawLine(pen, centerUp, wCornerUp);//tail towards south
	
	                            g.DrawLine(pen, wCorner, wCornerUp);
	                            g.DrawLine(pen, nCorner, nCornerUp);
	                            g.DrawLine(pen, sCorner, sCornerUp);
	                            g.DrawLine(pen, center, centerUp);
	                            break;
	                        case Map.Wall.WallFacing.NE_CORNER:
	                            g.DrawLine(pen, center, eCorner);
	                            g.DrawLine(pen, center, sCorner);
	                            if (!DrawExtents3D)
	                                break;
	                            g.DrawLine(pen, centerUp, eCornerUp);
	                            g.DrawLine(pen, centerUp, sCornerUp);
	
	                            g.DrawLine(pen, centerUp, center);
	                            g.DrawLine(pen, eCornerUp, eCorner);
	                            g.DrawLine(pen, sCornerUp, sCorner);
	                            break;
	                        case Map.Wall.WallFacing.NW_CORNER:
	                            g.DrawLine(pen, center, wCorner);
	                            g.DrawLine(pen, center, sCorner);
	                            if (!DrawExtents3D)
	                                break;
	                            g.DrawLine(pen, centerUp, wCornerUp);
	                            g.DrawLine(pen, centerUp, sCornerUp);
	
	                            g.DrawLine(pen, centerUp, center);
	                            g.DrawLine(pen, wCornerUp, wCorner);
	                            g.DrawLine(pen, sCornerUp, sCorner);
	                            break;
	                        case Map.Wall.WallFacing.SW_CORNER:
	                            g.DrawLine(pen, center, nCorner);
	                            g.DrawLine(pen, center, wCorner);
	                            if (!DrawExtents3D)
	                                break;
	                            g.DrawLine(pen, centerUp, nCornerUp);
	                            g.DrawLine(pen, centerUp, wCornerUp);
	
	                            g.DrawLine(pen, centerUp, center);
	                            g.DrawLine(pen, wCornerUp, wCorner);
	                            g.DrawLine(pen, nCornerUp, nCorner);
	                            break;
	                        case Map.Wall.WallFacing.SE_CORNER:
	                            g.DrawLine(pen, center, nCorner);
	                            g.DrawLine(pen, center, eCorner);
	
	                            if (!DrawExtents3D)
	                                break;
	                            g.DrawLine(pen, centerUp, nCornerUp);
	                            g.DrawLine(pen, centerUp, eCornerUp);
	
	                            g.DrawLine(pen, centerUp, center);
	                            g.DrawLine(pen, eCornerUp, eCorner);
	                            g.DrawLine(pen, nCornerUp, nCorner);
	                            break;
	                        default:
	                            g.DrawRectangle(pen, x, y, squareSize, squareSize);
	                            if (DrawText) TextRenderer.DrawText(g, "?", drawFont, nCorner, Color.Red);
	                            break;
	                    }
	                    if (DrawText)
	                    	TextRenderer.DrawText(g, wall.Minimap.ToString(), drawFont, txtPoint, Color.Red);
	                }
	            }
            }

            RenderPostWalls(g);

            // Draw objects
            objectRenderer.RenderNormal(g);
            
            // Draw polygons
            if (EditorSettings.Default.Draw_Polygons)
            {
                foreach (Map.Polygon poly in Map.Polygons)
                {
                    pen = Pens.PaleGreen;
                    // Highlight the polygon being edited
                    if (MapInterface.CurrentMode == EditMode.POLYGON_RESHAPE)
                    {
                    	if (mapView.PolygonEditDlg.SelectedPolygon == poly)
                    	{
                    		pen = Pens.PaleVioletRed;
                    	}
                    }
                    
                    if (poly.Points.Count > 2)
                    {
                    	poly.Points.Add(poly.Points[0]);
                    	g.DrawLines(pen, poly.Points.ToArray());
                    	poly.Points.RemoveAt(poly.Points.Count - 1);
                    }
                }
            }
            
            // Draw waypoints
            if (EditorSettings.Default.Draw_Waypoints)
            {
                foreach (Map.Waypoint wp in Map.Waypoints)
                {
                	// highlight selected waypoint
                	pen = MapInterface.SelectedWaypoint == wp ? ColorLayout.WaypointSel : ColorLayout.WaypointNorm;
                	// draw waypoint and related pathes
                    center = new PointF(wp.Point.X - MapView.objectSelectionRadius, wp.Point.Y - MapView.objectSelectionRadius);
                    g.DrawEllipse(pen, new RectangleF(center, new Size(2 * MapView.objectSelectionRadius, 2 * MapView.objectSelectionRadius)));

                    // Draw paths (code/idea from UndeadZeus)
                    foreach (Map.Waypoint.WaypointConnection wpc in wp.connections)
                    {
                        g.DrawLine(pen, wp.Point.X, wp.Point.Y, wpc.wp.Point.X, wpc.wp.Point.Y);
                        foreach (Map.Waypoint.WaypointConnection wpwp in wpc.wp.connections)//Checks if the waypoint connection is connecting to wp
                        {
                            if (wpwp.wp.Equals(wp))
                            {
                            	// if there is two way connection
                            	if (MapInterface.SelectedWaypoint != wp) pen = ColorLayout.WaypointTwoPath;
                            	
                            	g.DrawLine(pen, wp.Point.X, wp.Point.Y, wpc.wp.Point.X, wpc.wp.Point.Y);
                                break;
                            }
                        }
                    }
                    
                    Point wayPoint = new Point(Convert.ToInt32(center.X) - MapView.objectSelectionRadius / 2, Convert.ToInt32(center.Y));
                    // text rendering is slow - as such don't label unseen waypoints
                    if (DrawText && clip.Contains(wayPoint))
                    {
                    	if (wp.Name.Length > 0)
                    		TextRenderer.DrawText(g, wp.Number + ":" + wp.ShortName, drawFont, wayPoint, Color.Purple);
                    	else
                    		TextRenderer.DrawText(g, wp.Number + "", drawFont, wayPoint, Color.Purple);
                    }
                }
            }
        }
    }
}
