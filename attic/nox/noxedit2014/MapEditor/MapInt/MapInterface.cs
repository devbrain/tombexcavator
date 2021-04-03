/*
 * MapEditor
 * Пользователь: AngryKirC
 * Дата: 06.07.2015
 */

using System;
using System.IO;
using System.Drawing;
using System.Collections;
using System.Reflection;

using MapEditor.XferGui;
using MapEditor.newgui;
using MapEditor.mapgen;
using NoxShared;

namespace MapEditor.MapInt
{
    /// <summary>
    /// Wrapper providing complex Map-related operations
    /// </summary>
    public class MapInterface
    {
    	// Singleton
    	private static MapInterface _instance = new MapInterface();
        /*
        const int HISTORY_LIMIT = 8;
        private Queue<Operation> history;
        */
        protected Map _Map;
        /// <summary>
        /// The map currently being edited.
        /// </summary>
        public static Map TheMap
        {
        	get
        	{
        		return _instance._Map;
        	}
        	set
        	{
        		_instance._Map = value;
        	}
        }
        protected MapHelper _mapHelper;

        /// <summary>
        /// Enables auto-removal of already existing tiles/walls in placement mode.
        /// </summary>
        public static bool AllowPlaceOverride 
        { 
        	get
        	{
        		return EditorSettings.Default.Edit_AllowOverride;
        	}
        }

        private EditMode _currentMode;
        
        /// <summary>
        /// Current editing mode/operation.
        /// </summary>
        public static EditMode CurrentMode
        {
            get
            {
                return _instance._currentMode;
            }
            set
            {
                _instance._currentMode = value;
                _instance._ModeIsUpdated = true;
            }
        }
        
        private bool _ModeIsUpdated = false;

        /// <summary>
        /// Forces MapView to update current mode in statusbar.
        /// </summary>
        public static bool ModeIsUpdated
        { 
        	get
        	{
        		return _instance._ModeIsUpdated;
        	}
        	set
        	{
        		_instance._ModeIsUpdated = value;
        	}
        }

        private KeyHelper _keyHelper = new KeyHelper();
        /// <summary>
        /// Instance of class that stores key states.
        /// </summary>
        public static KeyHelper KeyHelper
        {
            get
            {
                return _instance._keyHelper;
            }
        }
        
        private MapView _mapView = null;
        
        /// <summary>
        /// Reference to MapView class with editor tools. TODO: remove redundant calls to this
        /// </summary>
        private static MapView mapView
        {
        	get
        	{
        		if (_instance._mapView == null) 
        			_instance._mapView = MainWindow.Instance.mapView;
        		
        		return _instance._mapView;
        	}
        }
        
        const string BLANK_MAP_NAME = "blankmap.map";

        public MapInterface()
        {
            //CurrentMode = Mode.OBJECT_SELECT;
            //history = new Queue<Operation>();
        }
        
        /// <summary>
        /// Attempts to load a map by its filename. Passing null string will result in loading a blank map
        /// </summary>
        public static void SwitchMap(String fileName)
        {
        	Stream stream = null;
        	// Check if requested loading a blank map
        	if (fileName == null)
        	{
        		Assembly asm = Assembly.GetExecutingAssembly();
	        	// Find BlankMap resource name.
	        	string name = null;
	        	foreach (string file in asm.GetManifestResourceNames())
	        	{
	        		if (file.EndsWith(BLANK_MAP_NAME, StringComparison.InvariantCultureIgnoreCase))
	        		{
	        			name = file;
	        			break;
	        		}
	        	}
	        	// Open resource stream
	        	stream = asm.GetManifestResourceStream(name);
        	}
        	else
        	{
	        	// Open filestream from name
	        	stream = new FileStream(fileName, FileMode.Open);
	        	// attempt to decompress .NXZ (if specified nxz file)
	            if (Path.GetExtension(fileName).Equals(".nxz", StringComparison.InvariantCultureIgnoreCase))
	            {
	            	int length = 0; byte[] data = null;
	            	using (var br = new BinaryReader(stream))
	            	{
	            		length = br.ReadInt32();
	            		data = br.ReadBytes((int) (br.BaseStream.Length - br.BaseStream.Position));
	            	}
	            	byte[] mapData = new byte[length];
	            	NoxLzCompression.Decompress(data, mapData);
	            	stream = new MemoryStream(mapData);
	            }
        	}
            
            // load the map
            var map = new Map();
            map.FileName = fileName;
            using (var rdr = new NoxBinaryReader(stream, CryptApi.NoxCryptFormat.MAP)) 
            	map.Load(rdr);
            // create helper class
            _instance._mapHelper = new MapHelper(map);
            _instance._Map = map;
            // update mapinfo
            MainWindow.Instance.UpdateMapInfo();
        }

        #region Wall operations
        public static bool WallPlace(int x, int y)
        {
            return WallPlace(new Point(x, y));
        }

        public static bool WallPlace(Point pt)
        {
            if (pt.X <= 0 || pt.Y <= 0 || pt.X >= 255 || pt.Y >= 255) return false;

            if (TheMap.Walls.ContainsKey(pt))
            {
                if (!AllowPlaceOverride) return false;
                TheMap.Walls.Remove(pt);
            }

            TheMap.Walls.Add(pt, mapView.WallMakeNewCtrl.NewWall(pt));

            return true;
        }

        public static bool WallRemove(int x, int y)
        {
            return WallRemove(new Point(x, y));
        }

        public static bool WallRemove(Point pt)
        {
            if (TheMap.Walls.ContainsKey(pt))
            {
                TheMap.Walls.Remove(pt);
                return true;
            }
            return false;
        }

        public static bool WallChange(int x, int y)
        {
            return WallChange(new Point(x, y));
        }

        public static bool WallChange(Point pt)
        {
            if (TheMap.Walls.ContainsKey(pt))
            {
                WallProperties secprops = new WallProperties();
                secprops.SetWall(TheMap.Walls[pt]);
                secprops.ShowDialog();
                return true;
            }
            return false;
        }

        public static Map.Wall WallGet(Point pt)
        {
            if (!TheMap.Walls.ContainsKey(pt)) return null;
            return TheMap.Walls[pt];
        }

        public static bool WallAutoBrush(Point pt, bool recur)
        {
            if (recur) WallPlace(pt);
            var wall = WallGet(pt);

            if (wall == null) return false;

            var wmm = WallGet(new Point(pt.X - 1, pt.Y - 1));
            var wpp = WallGet(new Point(pt.X + 1, pt.Y + 1));
            var wpm = WallGet(new Point(pt.X + 1, pt.Y - 1));
            var wmp = WallGet(new Point(pt.X - 1, pt.Y + 1));
            if (recur)
            {
                WallAutoBrush(new Point(pt.X - 1, pt.Y - 1), false);
                WallAutoBrush(new Point(pt.X + 1, pt.Y + 1), false);
                WallAutoBrush(new Point(pt.X + 1, pt.Y - 1), false);
                WallAutoBrush(new Point(pt.X - 1, pt.Y + 1), false);
            }

            if (wmm != null && wpm != null && wmp != null && wpp != null)
            {
                wall.Facing = Map.Wall.WallFacing.CROSS;
                wall.Variation = 0;
                return true;
            }
            else if (wmm != null && wpm != null && wmp == null)
            {
                if (wpp == null)
                {
                    wall.Facing = Map.Wall.WallFacing.SW_CORNER;
                    wall.Variation = 0;
                }
                else
                {
                    wall.Facing = Map.Wall.WallFacing.EAST_T;
                    wall.Variation = 0;
                }
                return true;
            }
            else if (wpp != null && wpm != null && wmm == null)
            {
                if (wmp == null)
                {
                    wall.Facing = Map.Wall.WallFacing.NW_CORNER;
                    wall.Variation = 0;
                }
                else
                {
                    wall.Facing = Map.Wall.WallFacing.NORTH_T;
                    wall.Variation = 0;
                }
                return true;
            }
            else if (wpp != null && wmp != null)
            {
                if (wmm == null)
                {
                    wall.Facing = Map.Wall.WallFacing.NE_CORNER;
                    wall.Variation = 0;
                }
                else
                {
                    wall.Facing = Map.Wall.WallFacing.WEST_T;
                    wall.Variation = 0;
                }
                return true;
            }
            else if (wmm != null && wmp != null)
            {
                if (wpm == null)
                {
                    wall.Facing = Map.Wall.WallFacing.SE_CORNER;
                    wall.Variation = 0;
                }
                else
                {
                    wall.Facing = Map.Wall.WallFacing.SOUTH_T;
                    wall.Variation = 0;
                }
                return true;
            }

            // Normal
            int seed = Environment.TickCount & Int32.MaxValue;

            Random rnd = new Random((wall.Location.X + (int)DateTime.Now.Ticks + seed));
            if (wpm != null || wmp != null)
            {
                wall.Facing = Map.Wall.WallFacing.NORTH;
                if (mapView.WallMakeNewCtrl.wallFacing > 1 && wall.Variation < 1)
                    wall.Variation = (byte)rnd.Next((int)mapView.WallMakeNewCtrl.numWallVari.Value, (int)mapView.WallMakeNewCtrl.numWallVariMax.Value + 1);

                return true;
            }
            if (wmm != null || wpp != null)
            {
                wall.Facing = Map.Wall.WallFacing.WEST;
                if (mapView.WallMakeNewCtrl.wallFacing > 1 && wall.Variation < 1)
                    wall.Variation = (byte)rnd.Next((int)mapView.WallMakeNewCtrl.numWallVari.Value, (int)mapView.WallMakeNewCtrl.numWallVariMax.Value + 1);
                return true;
            }
            return false;
        }
        #endregion

        #region Floor operations

        public static bool FloorPlace(int x, int y)
        {
            return FloorPlace(new Point(x, y));
        }

        public static bool FloorPlace(Point pt)
        {
            if (pt.X < 0 || pt.Y < 0 || pt.X > 252 || pt.Y > 252) return false;

            if (TheMap.Tiles.ContainsKey(pt))
            {
                if (!AllowPlaceOverride) return false;
                TheMap.Tiles.Remove(pt);
            }

            TheMap.Tiles.Add(pt, mapView.TileMakeNewCtrl.GetTile(pt));
            OpUpdatedTiles = true;
            return true;
        }

        public static bool FloorRemove(int x, int y)
        {
            return FloorRemove(new Point(x, y));
        }

        public static bool FloorRemove(Point pt)
        {
        	if (CurrentMode == EditMode.FLOOR_BRUSH)
        	{
        		// Remove multiple tiles if Auto Brush is enabled
	        	if (mapView.TileMakeNewCtrl.BrushSize.Value >= 2)
	            {
	                Point pat = new Point();
	                int i = 0;
	
	                int cols = (int)mapView.TileMakeNewCtrl.BrushSize.Value;
	                int rows = (int)mapView.TileMakeNewCtrl.BrushSize.Value;
	                for (pat = pt; i < rows; i++, pat.X--, pat.Y++)
	                {
	                    for (int j = 0; j < cols; j++)
	                    {
	                        Point pat2 = new Point();
	                        pat2 = pat;
	                        pat2.X += j * 1;
	                        pat2.Y += j * 1;
	                        pat2.Y -= 2;
	                        
	                        if (TheMap.Tiles.ContainsKey(pat2))
	                        	TheMap.Tiles.Remove(pat2);
	                    }
	                }
	                OpUpdatedTiles = true;
	                return true;
	            }
        	}
        	// Remove singular tile
            if (TheMap.Tiles.ContainsKey(pt))
            {
                TheMap.Tiles.Remove(pt);
                OpUpdatedTiles = true;
                return true;
            }
            return false;
        }

        public static bool FloorAutoBrush(int x, int y)
        {
            var edge = mapView.EdgeMakeNewCtrl.GetEdge();
            // TODO: move stuff from MapHelper to this class
            _instance._mapHelper.SetTileMaterial(ThingDb.FloorTileNames[edge.Graphic]);
            _instance._mapHelper.SetEdgeMaterial(ThingDb.EdgeTileNames[edge.Edge]);
            
            if (mapView.TileMakeNewCtrl.BrushSize.Value >= 2)
            {
                Point tilePt = new Point();
                tilePt.X = x;
                tilePt.Y = y;
                Point pat = new Point();
                int i = 0;

                int cols = (int)mapView.TileMakeNewCtrl.BrushSize.Value;
                int rows = (int)mapView.TileMakeNewCtrl.BrushSize.Value;
                for (pat = tilePt; i < rows; i++, pat.X--, pat.Y++)
                {
                    for (int j = 0; j < cols; j++)
                    {
                        Point pat2 = new Point();
                        pat2 = pat;
                        pat2.X += j * 1;
                        pat2.Y += j * 1;
                        pat2.Y -= 2;
                        //(re)place tile + auto edge + auto vari
                        _instance._mapHelper.PlaceTile(pat2.X, pat2.Y);
                        _instance._mapHelper.BrushAutoBlend(pat2.X, pat2.Y);
                    }
                }
            }
            else
            {
                _instance._mapHelper.PlaceTile(x, y);
                _instance._mapHelper.BrushAutoBlend(x, y);
            }
            OpUpdatedTiles = true;
            return true;
        }

        public static bool FloorAutoBrush(Point pt)
        {
            return FloorAutoBrush(pt.X, pt.Y);
        }
        #endregion

        #region Edge operations
        public static bool EdgePlace(int x, int y)
        {
            return EdgePlace(new Point(x, y));
        }

        public static bool EdgePlace(Point pt)
        {
            if (TheMap.Tiles.ContainsKey(pt))
            {
                Map.Tile tile = TheMap.Tiles[pt];
                Map.Tile.EdgeTile edge = mapView.EdgeMakeNewCtrl.GetEdge();

                foreach (Map.Tile.EdgeTile ex in tile.EdgeTiles)
                {
                    // don't create edges with equal EdgeType, Direction and CoverTile
                    if (ex.Edge == edge.Edge && ex.Dir == edge.Dir && ex.Graphic == edge.Graphic) return false;
                }

                tile.EdgeTiles.Add(edge);
                OpUpdatedTiles = true;
                return true;
            }
            return false;
        }

        public static bool EdgeRemove(int x, int y)
        {
            return EdgeRemove(new Point(x, y));
        }

        public static bool EdgeRemove(Point pt)
        {
            if (TheMap.Tiles.ContainsKey(pt))
            {
                Map.Tile tile = TheMap.Tiles[pt];
                byte edgeTypeID = mapView.EdgeMakeNewCtrl.GetEdge().Edge;
                // filter edges with specific type (selected)
                ArrayList newlist = new ArrayList();
                if (!KeyHelper.ShiftKey)
                {
                    int i = tile.EdgeTiles.Count - 1;
                    foreach (Map.Tile.EdgeTile edge in tile.EdgeTiles)
                    {
                        if (i > 0) newlist.Add(edge);
                        i--;
                    }
                    tile.EdgeTiles = newlist;
                }
                else
                {
                    foreach (Map.Tile.EdgeTile edge in tile.EdgeTiles)
                    {
                        if (edge.Edge != edgeTypeID) newlist.Add(edge);
                    }
                    tile.EdgeTiles = newlist;
                }
                OpUpdatedTiles = true;
                return true;
            }
            return false;
        }
        #endregion

		private MapObjectCollection _SelectedObjects = new MapObjectCollection();        
        
		public static MapObjectCollection SelectedObjects
		{
			get
			{
				return _instance._SelectedObjects;
			}
		}
		
        #region Object operations        
        private static int FindUnusedExtent()
        {
            int result = 3; // 2 = host player
            while (result != int.MaxValue)
            {
                bool found = false;

                // check if there are no objects with this extent
                foreach (Map.Object obj in TheMap.Objects)
                {
                    if (obj.Extent == result)
                    {
                        found = true; break;
                    }
                }

                if (found) result++;
                else break; // found unused
            }
            return result;
        }

        public static Map.Object ObjectPlace(string type, float x, float y)
        {
        	OpUpdatedObjects = true;
            return ObjectPlace(type, new PointF(x, y));
        }

        public static Map.Object ObjectPlace(string type, PointF loc)
        {
            if (!ThingDb.Things.ContainsKey(type)) return null;

            Map.Object result = new Map.Object();
            result.Name = type;
            result.Location = loc;
            result.Extent = FindUnusedExtent();

            // смотрим нету ли редактора, устанавливаем стандартные значения
            XferEditor editor = XferEditors.GetEditorForXfer(ThingDb.Things[type].Xfer);
            if (editor != null) editor.SetDefaultData(result);
            else result.NewDefaultExtraData();

            TheMap.Objects.Add(result);
            return result;
        }

        public static bool ObjectRemove(Map.Object obj)
        {
            if (TheMap.Objects.Contains(obj))
            {
                TheMap.Objects.Remove(obj);
                OpUpdatedObjects = true;
                return true;
            }
            return false;
        }

        public static Map.Object ObjectSelect(Point pt)
        {
            double closestDistance = Double.MaxValue;
            Map.Object closest = null;
            
            foreach (Map.Object obj in TheMap.Objects)
            {
                double distance = Math.Pow(pt.X - obj.Location.X, 2) + Math.Pow(pt.Y - obj.Location.Y, 2);

                if (distance < selectRadius && distance < closestDistance)
                {
                    closestDistance = distance;
                    closest = obj;
                }
            }

            return closest;
        }

        #endregion

        private Map.Waypoint _SelectedWaypoint = null;
        
        public static Map.Waypoint SelectedWaypoint
        {
        	get
        	{
        		return _instance._SelectedWaypoint;
        	}
        	set
        	{
        		_instance._SelectedWaypoint = value;
        	}
        }
        
        #region Waypoint operations
        const byte WaypointFlag = 128;
        const double selectRadius = MapView.objectSelectionRadius * MapView.objectSelectionRadius;

        public static Map.Waypoint WaypointSelect(Point pt)
        {
            double closestDistance = Double.MaxValue;
            Map.Waypoint closest = null;

            foreach (Map.Waypoint wp in TheMap.Waypoints)
            {
                double distance = Math.Pow(pt.X - wp.Point.X, 2) + Math.Pow(pt.Y - wp.Point.Y, 2);

                if (distance < selectRadius && distance < closestDistance)
                {
                    closestDistance = distance;
                    closest = wp;
                }
            }

            return closest;
        }

        public static bool WaypointRemove(Map.Waypoint wp)
        {
            if (TheMap.Waypoints.Contains(wp))
            {
                TheMap.Waypoints.Remove(wp);
                return true;
            }
            return false;
        }

        public static Map.Waypoint WaypointPlace(string name, PointF loc, bool enabled)
        {
            int i;
            for (i = 1; TheMap.Waypoints.num_wp.ContainsKey(i); i++) ;
            Map.Waypoint wp = new Map.Waypoint(name, loc, i);
            wp.Flags = enabled ? 1 : 0;
            TheMap.Waypoints.Add(wp);
            TheMap.Waypoints.num_wp.Add(wp.Number, wp);
            return wp;
        }

        public static bool WaypointConnect(Map.Waypoint wp)
        {
            if (wp != null && SelectedWaypoint != null)
            {
                if (wp != SelectedWaypoint)
                {
                    SelectedWaypoint.AddConnByNum(wp, WaypointFlag);
                    return true;
                }
            }
            return false;
        }

        public static bool WaypointUnconnect(Map.Waypoint wp)
        {
            if (wp != null && SelectedWaypoint != null)
            {
                if (wp != SelectedWaypoint)
                {
                    SelectedWaypoint.RemoveConnByNum(wp);
                    return true;
                }
            }
            return false;
        }
        #endregion

        /// <summary>
        /// Marks that latest operation has modified some tiles on the map
        /// </summary>
        public static bool OpUpdatedTiles = false;
        /// <summary>
        /// Marks that latest operation has modified some objects on the map
        /// </summary>
        public static bool OpUpdatedObjects = false;
        
        public static void ResetUpdateTracker()
        {
        	OpUpdatedObjects = false;
        	OpUpdatedTiles = false;
        }
        
        public static void HandleLMouseClick(Point pt)
        {
            Point wallPt = MapView.GetNearestWallPoint(pt);
            Point tilePt = MapView.GetNearestTilePoint(pt);

            // Perform an action depending on current editing mode.
            switch (CurrentMode)
            {
                case EditMode.WALL_PLACE:
                    WallPlace(wallPt);
                    break;
                case EditMode.WALL_CHANGE:
                    WallChange(wallPt);
                    break;
                case EditMode.WALL_BRUSH:
                    WallAutoBrush(wallPt, !KeyHelper.ShiftKey);
                    break;
                case EditMode.FLOOR_PLACE:
                    FloorPlace(tilePt);
                    break;
                case EditMode.FLOOR_BRUSH:
                    FloorAutoBrush(tilePt);
                    break;
                case EditMode.EDGE_PLACE:
                    EdgePlace(tilePt);
                    break;
                case EditMode.OBJECT_PLACE:
                    ObjectPlace(mapView.cboObjCreate.Text, pt.X, pt.Y);
                    break;
                case EditMode.OBJECT_SELECT:
                    var obj = ObjectSelect(pt);
                    if (obj != null)
                    {
                        if (!SelectedObjects.Items.Contains(obj))
                        {
                            // clear selection if not multiselecting
                            if (!KeyHelper.ShiftKey) SelectedObjects.Items.Clear();
                            // put into selection
                            SelectedObjects.Items.Add(obj);
                        }
                        // set drag origin
                        SelectedObjects.Origin = obj;
                    }
                    else
                    {
                        SelectedObjects.Items.Clear();
                        SelectedObjects.Origin = null;
                    }
                    break;
                case EditMode.WAYPOINT_PLACE:
                    WaypointPlace(mapView.waypointName.Text, new PointF(pt.X, pt.Y), mapView.waypointEnabled.Checked);
                    break;
                case EditMode.WAYPOINT_CONNECT:
                case EditMode.WAYPOINT_SELECT:
                    // Connect previously selected waypoint and one that is under cursor currently
                    if (CurrentMode == EditMode.WAYPOINT_CONNECT)
                    {
                        if (KeyHelper.ShiftKey) // Shift unconnects (reverse)
                            WaypointUnconnect(WaypointSelect(pt));
                        else
                            WaypointConnect(WaypointSelect(pt));
                    }
                    // Mark waypoint under cursor as selected, or reset
                    SelectedWaypoint = WaypointSelect(pt);
                    if (SelectedWaypoint != null)
                    {
                        // update info box
                        mapView.waypointName.Text = SelectedWaypoint.Name;
                        mapView.waypointEnabled.Checked = SelectedWaypoint.Flags > 0;
                    }
                    break;
            }
        }

        public static void HandleRMouseClick(Point pt)
        {
            Point wallPt = MapView.GetNearestWallPoint(pt);
            Point tilePt = MapView.GetNearestTilePoint(pt);
            
            switch (CurrentMode)
            {
                case EditMode.FLOOR_BRUSH:
                    FloorRemove(tilePt);
                    break;
                case EditMode.WALL_BRUSH:
                    WallRemove(wallPt);
                    break;
                case EditMode.FLOOR_PLACE:
                    FloorRemove(tilePt);
                    break;
                case EditMode.WALL_PLACE:
                    WallRemove(wallPt);
                    break;
                case EditMode.OBJECT_PLACE:
                    var obj = ObjectSelect(pt);
                    if (obj != null) ObjectRemove(obj);
                    break;
                case EditMode.WAYPOINT_PLACE:
                    var way = WaypointSelect(pt);
                    if (way != null) WaypointRemove(way);
                    break;
                case EditMode.EDGE_PLACE:
                    EdgeRemove(tilePt);
                    break;

            }
        }
        /*
        public void PerformOperation(Operation o)
        {
            // add to history
            if (history.Count >= HISTORY_LIMIT) RemoveLast(history);
            history.Enqueue(o);
            // execute implementation
            o.Perform(this);
        }
		
        private void RemoveLast(Queue<Operation> q)
        {
            var first = q.Peek();
            Operation current = null;
            while (true) {
                current = q.Dequeue();
                if (q.Peek() == first) {
                    break;
                }
                q.Enqueue(current);
            }
        }
        */
    }

    public interface Operation
    {
        bool Perform(MapInterface i);
        void Undo(MapInterface i);
    }
}