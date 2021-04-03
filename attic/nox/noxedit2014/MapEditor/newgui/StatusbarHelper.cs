/*
 * MapEditor
 * Пользователь: AngryKirC
 * Дата: 29.12.2015
 */
using System;
using System.Drawing;
using NoxShared;
using MapEditor.MapInt;

namespace MapEditor.newgui
{
	/// <summary>
	/// Helper class tracking statusbar values.
	/// </summary>
	public class StatusbarHelper
	{
		Map map
		{
			get
			{
				return MapInterface.TheMap;
			}
		}
		
		string statusLocation;
		/// <summary>
		/// Workaround to prevent flickering.
		/// </summary>
		bool updateStatusbars = false;
		string statusMapItem;
		string statusPolygon;
		
		public string StatusLocation
		{
			get
			{
				return statusLocation;
			}
		}
		
		public string StatusMapItem
		{
			get
			{
				return statusMapItem;
			}
		}
		
		public string StatusPolygon
		{
			get
			{
				return statusPolygon;
			}
		}
		
		const int squareSize = MapView.squareSize;
		// Format strings
		const string FORMAT_COORDINATES = "X={0} Y={1}";
		const string FORMAT_WALL_INFO = "{0} #{1}";
		const string FORMAT_TILE_INFO = "{0}-0x{1:x2}";
		const string FORMAT_EDGE_COUNT = " Edges({0}):";
		const string FORMAT_EDGE_INFO = " {0}-0x{1:x2}-{2}-{3}";
		const string FORMAT_OBJECT_INFO = "ThingId: {0} Ext: {1}";
		
		// Used for tracking changes
		Map.Wall prevWall = null;
		Map.Tile prevTile = null;
		Map.Object prevObj = null;
		Map.Polygon prevPoly = null;
		
		public void Update(Point mousePt)
		{
        	var ptFlt = new PointF(mousePt.X, mousePt.Y);
            var tilePt = MapView.GetNearestTilePoint(mousePt);
            var wallPt = MapView.GetNearestWallPoint(mousePt);
        	
        	// Search for walls/tiles/objects under cursor
            Map.Wall wall = map.Walls.ContainsKey(MapView.GetNearestWallPoint(mousePt)) ? map.Walls[MapView.GetNearestWallPoint(mousePt)] : null;
            Map.Tile tile = map.Tiles.ContainsKey(MapView.GetNearestTilePoint(mousePt)) ? map.Tiles[MapView.GetNearestTilePoint(mousePt)] : null;
            Map.Object obj = MapInterface.ObjectSelect(mousePt);
            
            bool iWalls = (MapInterface.CurrentMode >= EditMode.WALL_PLACE && MapInterface.CurrentMode <= EditMode.WALL_CHANGE);
            bool iTiles = (MapInterface.CurrentMode >= EditMode.FLOOR_PLACE && MapInterface.CurrentMode <= EditMode.EDGE_PLACE);
            bool iObjs = (MapInterface.CurrentMode >= EditMode.OBJECT_PLACE && MapInterface.CurrentMode <= EditMode.OBJECT_SELECT);
            
            statusLocation = String.Format(FORMAT_COORDINATES, mousePt.X, mousePt.Y);
            statusMapItem = "";
            statusPolygon = "";
            
            // Wall tracking
            if (iWalls)
            {
            	statusLocation = String.Format(FORMAT_COORDINATES, wallPt.X, wallPt.Y);
            	
            	if (wall != null)
                	statusMapItem = String.Format(FORMAT_WALL_INFO, wall.Material, wall.Variation);
            }
            else
            	prevWall = null;
            
            if (prevWall != wall)
        	{
        		prevWall = wall;
        		updateStatusbars = true;
        	}
               
            // Tile tracking
            if (iTiles)
            {
            	statusLocation = String.Format(FORMAT_COORDINATES, tilePt.X, tilePt.Y);
            	
            	if (tile != null)
            	{
	                statusMapItem = String.Format(FORMAT_TILE_INFO, tile.Graphic, tile.Variation);
	               
	                if (tile.EdgeTiles.Count > 0)
	                {
	                    statusMapItem += String.Format(FORMAT_EDGE_COUNT, tile.EdgeTiles.Count);
	                    
	                    foreach (Map.Tile.EdgeTile edge in tile.EdgeTiles)
	                        statusMapItem += String.Format(FORMAT_EDGE_INFO, ThingDb.FloorTileNames[edge.Graphic], edge.Variation, edge.Dir, ThingDb.EdgeTileNames[edge.Edge]);
	                }
            	}
            }
            else
            	prevTile = null;
            
            if (prevTile != tile)
        	{
        		prevTile = tile;
        		updateStatusbars = true;
        	}
            
            // Object tracking
            if (iObjs && obj != null)
            {
            	statusMapItem = String.Format(FORMAT_OBJECT_INFO, obj.Name, obj.Extent);
            }
            else
            	prevObj = null;
            
            if (prevObj != obj)
        	{
            	prevObj = obj;
        		updateStatusbars = true;
        	}
            
            // Polygon tracking
            Map.Polygon ins = null;
            foreach (Map.Polygon poly in map.Polygons)
            {
            	if (poly.IsPointInside(ptFlt))
            	{
            		statusPolygon = poly.Name;
            		ins = poly;
            		break;
            	}
            }
            
            if (prevPoly != ins)
            {
            	prevPoly = ins;
            	updateStatusbars = true;
            }
		}
		
		public bool ValuesChanged()
		{
			if (updateStatusbars)
			{
				updateStatusbars = false;
				return true;
			}
			return false;
		}
	}
}
