/*
 * MapEditor
 * Пользователь: AngryKirC
 * Дата: 12.02.2015
 * 
 *
 */
using System;
using System.Collections.Generic;
using System.Drawing;
using NoxShared;

using System.Windows.Forms;
using System.Xml;
using System.Reflection;

using System.IO;

namespace MapEditor.mapgen
{
    /// <summary>
    /// Utility class to be used by Map generator
    /// </summary>
    public class MapHelper
    {
        private Map map;
        private string[] wallMaterialNames = ThingDb.WallNames.ToArray();
        private string[] tileMaterialNames = ThingDb.FloorTileNames.ToArray();
        private string[] edgeMaterialNames = ThingDb.EdgeTileNames.ToArray();
        private List<Point> tilesScanned = new List<Point>();

        private int autoedgeIgnoreTile = -1;
        private byte usingWallMaterial = 0;
        private byte usingTileMaterial = 0;
        private byte usingEdgeMaterial = 0;
        private byte varcount = 0;
        private int incrementObjectExtent = 2;





        public MapHelper(Map map)
        {
            this.map = map;
        }

        /// <summary>
        /// Set tile brush type (material).
        /// </summary>
        public void SetTileMaterial(string materialString)
        {
            usingTileMaterial = (byte)Array.IndexOf(tileMaterialNames, materialString);

        }

        /// <summary>
        /// Set wall brush type (material).
        /// </summary>
        public void SetWallMaterial(string materialString)
        {
            usingWallMaterial = (byte)Array.IndexOf(wallMaterialNames, materialString);
        }

        /// <summary>
        /// Set edge brush type (material).
        /// </summary>
        public void SetEdgeMaterial(string materialString)
        {
            usingEdgeMaterial = (byte)Array.IndexOf(edgeMaterialNames, materialString);
            varcount = (byte)ThingDb.EdgeTiles[usingEdgeMaterial].Variations.Count;

        }

        /// <summary>
        /// Set tile that will be ignored in autoblend calculations.
        /// </summary>
        public void SetAutoblendIgnore(string materialString)
        {
            if (materialString == null)
            {
                autoedgeIgnoreTile = -1;
                return;
            }
            autoedgeIgnoreTile = (byte)Array.IndexOf(tileMaterialNames, materialString);
        }

        public Map.Wall GetWallSnap(int x, int y)
        {
            // fix coordinates
            int div1 = x % 2;
            int div2 = y % 2;
            x += div2 - div1;
            return GetWall(x, y);
        }

        /// <summary>
        /// Search for Wall on the map at specified coordinates
        /// </summary>
        /// <returns>null if there is no wall</returns>
        public Map.Wall GetWall(int x, int y)
        {
            // check coordinates
            if (x >= 255) return null;
            if (y >= 255) return null;
            if (x < 0) return null;
            if (y < 0) return null;

            Point pt = new Point(x, y);
            if (!map.Walls.ContainsKey(pt)) return null;
            return map.Walls[pt];
        }

        public bool PlaceWallSnap(int x, int y)
        {
            // fix coordinates
            int div1 = x % 2;
            int div2 = y % 2;
            x += div1;
            x += div2;
            return PlaceWall(x, y);
        }

        /// <summary>
        /// Place a new Wall onto the Map at specified coordinates
        /// </summary>
        /// <returns>true if wall was successfully placed</returns>
        public bool PlaceWall(int x, int y)
        {
            // check coordinates
            if (x >= 255) return false;
            if (y >= 255) return false;
            if (x < 0) return false;
            if (y < 0) return false;

            Point pt = new Point(x, y);
            map.Walls[pt] = new Map.Wall(pt, Map.Wall.WallFacing.CROSS, usingWallMaterial);
            return true;
        }

        public bool PlaceWall(int x, int y, int facing)
        {
            // check coordinates
            if (x >= 255) return false;
            if (y >= 255) return false;
            if (x < 0) return false;
            if (y < 0) return false;

            Point pt = new Point(x, y);
            map.Walls[pt] = new Map.Wall(pt, (Map.Wall.WallFacing)facing, usingWallMaterial);
            return true;
        }

        public bool RemoveWall(int x, int y)
        {
            // fix coordinates
            int div1 = x % 2;
            int div2 = y % 2;
            x += div2 - div1;

            Point pt = new Point(x, y);
            if (!map.Walls.ContainsKey(pt)) return false;
            map.Walls.Remove(pt);
            return true;
        }

        public bool PlaceTileSnap(int x, int y)
        {
            // fix coordinates
            int div1 = x % 2;
            int div2 = y % 2;
            x += div1 - div2;
            return PlaceTile(x, y);
        }

        /// <summary>
        /// Places tile onto the map.
        /// </summary>
        /// <returns>true on success, false otherwise</returns>
        public bool PlaceTile(int x, int y)
        {
            // check coordinates testudo
            if (x >= 255) return false;
            if (y >= 255) return false;
            if (x <= 0) return false;
            if (y <= 0) return false;
            int cols = ThingDb.FloorTiles[usingTileMaterial].numCols;
            int rows = ThingDb.FloorTiles[usingTileMaterial].numRows;
            ushort vari = (ushort)(((x + y) / 2 % cols) + (((y % rows) + 1 + cols - ((x + y) / 2) % cols) % rows) * cols);
            Point pt = new Point(x, y);
            map.Tiles[pt] = new Map.Tile(pt, usingTileMaterial, vari);


            return true;
        }

        public Map.Tile GetTileSnap(int x, int y)
        {
            // fix coordinates
            int div1 = x % 2;
            int div2 = y % 2;
            x += div1 - div2;
            return GetTile(x, y);
        }

        public Map.Tile GetTile(int x, int y, bool o = false)
        {
            // check coordinates
            if (x >= 255) return null;
            if (y >= 255) return null;
            if (x <= 0) return null;
            if (y <= 0) return null;

            Point pt = new Point(x, y);
            if (!map.Tiles.ContainsKey(pt)) return null;
            return map.Tiles[pt];
        }




        public int CountNeighborTiles(int x, int y)
        {
            int count = 0;
            if (GetTile(x + 2, y) != null) count++;
            if (GetTile(x + 1, y - 1) != null) count++;
            if (GetTile(x, y - 2) != null) count++;
            if (GetTile(x - 1, y - 1) != null) count++;
            if (GetTile(x - 2, y) != null) count++;
            if (GetTile(x - 1, y + 1) != null) count++;
            if (GetTile(x + 2, y) != null) count++;
            if (GetTile(x + 1, y + 1) != null) count++;
            return count;
        }

        /// <summary>
        /// Improved TileAutoBlend
        /// </summary>
        public void BrushAutoBlend(int x, int y)
        {
            //int varcount = ThingDb.EdgeTiles[usingEdgeMaterial].Variations.Count;
            if (varcount > 20) return;
            // Better be safe than sorry - MudEdge may cause crash
            if (GetTile(x, y) == null) return;
            // Enforce using single tile type, because we don't want to blend into tiles with same ID
            if (GetTile(x, y).graphicId != usingTileMaterial) return;
            Map.Tile nUp, nDown, nLeft, nRight, ncUR, ncUL, ncDR, ncDL;
            bool enUp = false, encUR = false, enDown = false, encDL = false, enLeft = false, encUL = false, enRight = false, encDR = false;
            bool WallBlock = MainWindow.Instance.mapView.TileMakeNewCtrl.WallBlockBrush.Checked;
            int selectedIndex = MainWindow.Instance.mapView.TileMakeNewCtrl.comboIgnoreTile.SelectedIndex;
            string tileName = MainWindow.Instance.mapView.TileMakeNewCtrl.comboIgnoreTile.Items[selectedIndex].ToString();
            int indexIgnor = ThingDb.FloorTileNames.IndexOf(tileName);
            autoedgeIgnoreTile = indexIgnor;
            bool? nUpBlocked = null;
            bool? nDownBlocked = null;
            bool nDownBlocked2 = false;
            bool ncDRBlocked = false;
            bool? nRightBlocked = null;
            bool encULBlocked = false;
            bool ncURBlocked = false;
            bool ncDLBlocked = false;
            nUp = GetTile(x, y + 2);
            ncUR = GetTile(x + 1, y + 1);
            nDown = GetTile(x, y - 2);
            ncDL = GetTile(x - 1, y - 1);
            nLeft = GetTile(x - 2, y);
            ncUL = GetTile(x - 1, y + 1);
            nRight = GetTile(x + 2, y);
            ncDR = GetTile(x + 1, y - 1);


            // Edge limit..?
            int edgeMax = 8;

            // Mark these tiles edge-able or notx+1, y + 1
            if (nUp != null)
            {
                if (!(GetWall(x, y + 2) != null && WallBlock && ((int)GetWall(x, y + 2).Facing == 9 || (int)GetWall(x, y + 2).Facing == 7 || (int)GetWall(x, y + 2).Facing == 2)))
                {
                    if (nUp.graphicId != usingTileMaterial && nUp.graphicId != autoedgeIgnoreTile) enUp = true;
                    if (nUp.EdgeTiles.Count > edgeMax) nUp.EdgeTiles.Clear();
                }
                else if (GetWall(x, y + 2) != null) nUpBlocked = ((int)GetWall(x, y + 2).Facing == 7) ? false : true;

            }

            //////////////////////////////////////////////////////////////////////////////////////////////////////////

            if (ncUR != null && (nUpBlocked == false || nUpBlocked == null) && !(GetWall(x + 1, y + 1) != null && WallBlock && ((int)GetWall(x + 1, y + 1).Facing == 0 || (int)GetWall(x + 1, y + 1).Facing == 10 || (int)GetWall(x + 1, y + 1).Facing == 2 || (int)GetWall(x + 1, y + 1).Facing == 6 || (int)GetWall(x + 1, y + 1).Facing == 6 || (int)GetWall(x + 1, y + 1).Facing == 9 || (int)GetWall(x + 1, y + 1).Facing == 3 || (int)GetWall(x + 1, y + 1).Facing == 5)))
            {
                if (ncUR.graphicId != usingTileMaterial && ncUR.graphicId != autoedgeIgnoreTile) encUR = true;
                if (ncUR.EdgeTiles.Count > edgeMax) ncUR.EdgeTiles.Clear();
            }
            else if (GetWall(x + 1, y + 1) != null) ncURBlocked = ((int)GetWall(x + 1, y + 1).Facing == 7) ? true : false;



            ///////////////////////////////////////////////////////////////////////////////////////////////////////////// 

            if (nDown != null)
            {
                if (!(GetWall(x, y) != null && WallBlock && ((int)GetWall(x, y).Facing == 7 || (int)GetWall(x, y).Facing == 9 || (int)GetWall(x, y).Facing == 2)))
                {
                    if (nDown.graphicId != usingTileMaterial && nDown.graphicId != autoedgeIgnoreTile) enDown = true;
                    if (nDown.EdgeTiles.Count > edgeMax) nDown.EdgeTiles.Clear();
                    //***********************99***************************
                }
                else
                {
                    nDownBlocked2 = true;
                    nDownBlocked = ((int)GetWall(x, y).Facing == 9) ? true : false;
                }
            }

            /////////////////////////////////////////////////////////////////////////////////////////////////////////////

            if (ncDL != null && (nDownBlocked == false || nDownBlocked == null) && !(GetWall(x, y) != null && WallBlock && ((int)GetWall(x, y).Facing == 0 || (int)GetWall(x, y).Facing == 2 || (GetWall(x - 1, y + 1) != null && (int)GetWall(x - 1, y + 1).Facing == 7) || (int)GetWall(x, y).Facing == 10 || (int)GetWall(x, y).Facing == 5 || (int)GetWall(x, y).Facing == 0 || (int)GetWall(x, y).Facing == 6 || (int)GetWall(x, y).Facing == 3)))
            {

                if (ncDL.graphicId != usingTileMaterial && ncDL.graphicId != autoedgeIgnoreTile) encDL = true;
                if (ncDL.EdgeTiles.Count > edgeMax) ncDL.EdgeTiles.Clear();
            }
            else if (GetWall(x - 1, y + 1) != null) ncDLBlocked = ((int)GetWall(x - 1, y + 1).Facing == 7) ? true : false;// or 7


            //////////////////////////////////////////////////////////////////////////////////////////////////////////



            if (nLeft != null && !(GetWall(x - 1, y + 1) != null && WallBlock && (int)GetWall(x - 1, y + 1).Facing == 10))
            {
                if (nLeft.graphicId != usingTileMaterial && nLeft.graphicId != autoedgeIgnoreTile) enLeft = true;
                if (nLeft.EdgeTiles.Count > edgeMax) nLeft.EdgeTiles.Clear();
            }

            //////////////////////////////////////////////////////////////////////////


            if (ncUL != null && (nUpBlocked == false || nUpBlocked == null) && !(GetWall(x - 1, y + 1) != null && WallBlock && ((int)GetWall(x - 1, y + 1).Facing == 1 || (int)GetWall(x - 1, y + 1).Facing == 2 || (int)GetWall(x - 1, y + 1).Facing == 8 || (int)GetWall(x - 1, y + 1).Facing == 9 || (int)GetWall(x - 1, y + 1).Facing == 9 || (int)GetWall(x - 1, y + 1).Facing == 4 || (int)GetWall(x - 1, y + 1).Facing == 6 || (int)GetWall(x - 1, y + 1).Facing == 5)))
            {
                if (ncUL.graphicId != usingTileMaterial && ncUL.graphicId != autoedgeIgnoreTile) encUL = true;
                if (ncUL.EdgeTiles.Count > edgeMax) ncUL.EdgeTiles.Clear();
            }
            else if (GetWall(x - 1, y + 1) != null) encULBlocked = ((int)GetWall(x - 1, y + 1).Facing == 9) ? true : false;// or 7



            //////////////////////////////////////////////////////////////////////////////////////////////////////////





            if (nRight != null)
            {
                if (!(GetWall(x + 1, y + 1) != null && WallBlock && (int)GetWall(x + 1, y + 1).Facing == 8))
                {
                    nRightBlocked = true;
                    if (nRight.graphicId != usingTileMaterial && nRight.graphicId != autoedgeIgnoreTile) enRight = true;
                    if (nRight.EdgeTiles.Count > edgeMax) nRight.EdgeTiles.Clear();
                }
                else nRightBlocked = false;
            }


            /////////////////////////////////////////////////////////////////////////////////////////////////////////


            if (ncDR != null && (nDownBlocked == false || nDownBlocked == null) && !(GetWall(x, y) != null && WallBlock && ((int)GetWall(x, y).Facing == 1 || (int)GetWall(x, y).Facing == 8 || (int)GetWall(x, y).Facing == 4 || (int)GetWall(x, y).Facing == 2 || (int)GetWall(x, y).Facing == 4 || (int)GetWall(x, y).Facing == 5 || (int)GetWall(x, y).Facing == 6 || (int)GetWall(x, y).Facing == 4)))
            {
                if (ncDR.graphicId != usingTileMaterial && ncDR.graphicId != autoedgeIgnoreTile) encDR = true;
                if (ncDR.EdgeTiles.Count > edgeMax) ncDR.EdgeTiles.Clear();
            }
            else if (GetWall(x, y) != null) ncDRBlocked = ((int)GetWall(x, y).Facing == 8) ? true : false;



            // Corners
            if (enUp && (encUR || ncUR == null || ncURBlocked) && encUL)
            {
                RemoveSameEdges(x, y + 2, 15);
                AddTileEdge(x, y + 2, (varcount < 20) ? ReMapVar(15, true) : 15);
            }


            if (enDown && (encDR || ncDR == null || ncDRBlocked) && encDL)
            {
                RemoveSameEdges(x, y - 2, 0);
                AddTileEdge(x, y - 2, 0);
            }

            //MessageBox.Show(ncDLBlocked.ToString());
            if (enLeft && (encUL || ncUL == null) && (ncDLBlocked || encDL))
            {
                RemoveSameEdges(x - 2, y, 11);
                AddTileEdge(x - 2, y, (varcount < 20) ? ReMapVar(11, true) : 11);
            }


            if (enRight && encUR && encDR)
            {
                RemoveSameEdges(x + 2, y, 4);
                AddTileEdge(x + 2, y, (varcount < 20) ? ReMapVar(4, true) : 4);
            }
            bool encUR2 = encUR;
            bool encUL2 = encUL;
            bool encDR2 = encDR;
            bool encDL2 = encDL;
            // Doublesides
            if (!enUp)
            {
                if (encUL)
                {
                    RemoveSideEdges(x - 1, y + 1, 19);
                    encUL2 = false;
                    AddTileEdge(x - 1, y + 1, (varcount < 20) ? ReMapVar(19, true) : 19);
                }
                if (encUR)
                {
                    //RemoveCornerEdges(x, y);
                    RemoveSideEdges(x + 1, y + 1, 17);
                    encUR2 = false;
                    AddTileEdge(x + 1, y + 1, (varcount < 20) ? ReMapVar(17, true) : 17);
                }
            }
            if (!enDown && nDown != null)
            {
                //MessageBox.Show(nDownBlocked2.ToString());
                if (encDL && nDownBlocked2 == false)
                {
                    RemoveSideEdges(x - 1, y - 1, 19);
                    RemoveSideEdges(x - 1, y - 1, 0);
                    encDL2 = false;
                    AddTileEdge(x - 1, y - 1, (varcount < 20) ? ReMapVar(19, true) : 19);
                }
                if (encDR)
                {
                    //RemoveCornerEdges(x, y);
                    RemoveSideEdges(x + 1, y - 1, 17);
                    encDR2 = false;
                    AddTileEdge(x + 1, y - 1, (varcount < 20) ? ReMapVar(17, true) : 17);
                }
            }
            if (!enLeft)
            {
                if (encDL)
                {
                    //RemoveCornerEdges(x, y);
                    RemoveSideEdges(x - 1, y - 1, 16);
                    encDL2 = false;
                    AddTileEdge(x - 1, y - 1, (varcount < 20) ? ReMapVar(16, true) : 16);
                }
                if (encUL)
                {
                    //RemoveCornerEdges(x, y);
                    RemoveSideEdges(x - 1, y + 1, 18);
                    encUL2 = false;
                    AddTileEdge(x - 1, y + 1, (varcount < 20) ? ReMapVar(18, true) : 18);
                }
            }
            if (!enRight && nRight != null) //vyresit
            {
                if (encDR)
                {
                    //RemoveCornerEdges(x, y);
                    RemoveSideEdges(x + 1, y - 1, 16);
                    encDR2 = false;
                    AddTileEdge(x + 1, y - 1, (varcount < 20) ? ReMapVar(16, true) : 16);
                }
                if (encUR && nRightBlocked != false)//vyresit
                {
                    //RemoveCornerEdges(x, y);
                    RemoveSideEdges(x + 1, y + 1, 18);
                    encUR2 = false;
                    AddTileEdge(x + 1, y + 1, (varcount < 20) ? ReMapVar(18, true) : 18);
                }
            }
            // Sides
            //RemoveSameEdges(x - 1, y - 1, 579); hroch


            if (encUR2) { RemoveSameEdges(x + 1, y + 1, 6); AddTileEdge(x + 1, y + 1, ReMapVar(6, true, x)); }
            if (encUL2) { RemoveSameEdges(x - 1, y + 1, 12); AddTileEdge(x - 1, y + 1, ReMapVar(12, true, x)); }
            if (encDR2) { RemoveSameEdges(x + 1, y - 1, 1); AddTileEdge(x + 1, y - 1, ReMapVar(1, true, x)); }
            if (encDL2) { RemoveSameEdges(x - 1, y - 1, 5); AddTileEdge(x - 1, y - 1, ReMapVar(5, true, x)); }

        }

        /// <summary>
        /// Automatically add edges for specified tile.
        /// </summary>
        public void TileAutoBlend(int x, int y)
        {
            if (GetTile(x, y) == null) return;
            // Enforce using single tile type, because we don't want to blend into tiles with same ID
            if (GetTile(x, y).graphicId != usingTileMaterial) return;
            Map.Tile nUp, nDown, nLeft, nRight, ncUR, ncUL, ncDR, ncDL;
            bool enUp = false, encUR = false, enDown = false, encDL = false, enLeft = false, encUL = false, enRight = false, encDR = false;

            nUp = GetTile(x, y + 2);
            ncUR = GetTile(x + 1, y + 1);
            nDown = GetTile(x, y - 2);
            ncDL = GetTile(x - 1, y - 1);
            nLeft = GetTile(x - 2, y);
            ncUL = GetTile(x - 1, y + 1);
            nRight = GetTile(x + 2, y);
            ncDR = GetTile(x + 1, y - 1);

            // Mark these tiles edge-able or not
            if (nUp != null)
            {
                if (nUp.graphicId != usingTileMaterial && nUp.graphicId != autoedgeIgnoreTile) enUp = true;
            }
            if (ncUR != null)
            {
                if (ncUR.graphicId != usingTileMaterial && ncUR.graphicId != autoedgeIgnoreTile) encUR = true;
            }
            if (nDown != null)
            {
                if (nDown.graphicId != usingTileMaterial && nDown.graphicId != autoedgeIgnoreTile) enDown = true;
            }
            if (ncDL != null)
            {
                if (ncDL.graphicId != usingTileMaterial && ncDL.graphicId != autoedgeIgnoreTile) encDL = true;
            }
            if (nLeft != null)
            {
                if (nLeft.graphicId != usingTileMaterial && nLeft.graphicId != autoedgeIgnoreTile) enLeft = true;
            }
            if (ncUL != null)
            {
                if (ncUL.graphicId != usingTileMaterial && ncUL.graphicId != autoedgeIgnoreTile) encUL = true;
            }
            if (nRight != null)
            {
                if (nRight.graphicId != usingTileMaterial && nRight.graphicId != autoedgeIgnoreTile) enRight = true;
            }
            if (ncDR != null)
            {
                if (ncDR.graphicId != usingTileMaterial && ncDR.graphicId != autoedgeIgnoreTile) encDR = true;
            }

            // Corners
            if (enUp && encUR && encUL) AddTileEdge(x, y + 2, (usingEdgeMaterial == 0) ? ReMapVar(15, true) : 15);

            if (enDown && encDR && encDL) AddTileEdge(x, y - 2, 0);
            if (enLeft && encUL && encDL) AddTileEdge(x - 2, y, (usingEdgeMaterial == 0) ? ReMapVar(11, true) : 11);
            if (enRight && encUR && encDR) AddTileEdge(x + 2, y, (usingEdgeMaterial == 0) ? ReMapVar(4, true) : 4);
            // Doublesides
            if (enUp && !encUR && !encUL) { nUp.EdgeTiles.Clear(); AddTileEdge(x, y + 2, (usingEdgeMaterial == 0) ? ReMapVar(18, true) : 18); }
            if (enDown && !encDR && !encDL) { nDown.EdgeTiles.Clear(); AddTileEdge(x, y - 2, (usingEdgeMaterial == 0) ? ReMapVar(16, true) : 16); }
            if (enLeft && !encUL && !encDL) { nLeft.EdgeTiles.Clear(); AddTileEdge(x - 2, y, (usingEdgeMaterial == 0) ? ReMapVar(19, true) : 19); }
            if (enRight && !encUR && !encDR) { nRight.EdgeTiles.Clear(); AddTileEdge(x + 2, y, (usingEdgeMaterial == 0) ? ReMapVar(17, true) : 17); }
            // Sides
            if (encUR && ncUR.EdgeTiles.Count == 0) AddTileEdge(x + 1, y + 1, (usingEdgeMaterial == 0) ? ReMapVar(6, true) : 6);
            if (encUL && ncUL.EdgeTiles.Count == 0) AddTileEdge(x - 1, y + 1, (usingEdgeMaterial == 0) ? ReMapVar(12, true) : 12);
            if (encDR && ncDR.EdgeTiles.Count == 0) AddTileEdge(x + 1, y - 1, (usingEdgeMaterial == 0) ? ReMapVar(1, true) : 1);
            if (encDL && ncDL.EdgeTiles.Count == 0) AddTileEdge(x - 1, y - 1, (usingEdgeMaterial == 0) ? ReMapVar(5, true) : 5);
        }

        /// <summary>
        /// Add new edge to tile at specified coordinates
        /// </summary>
        public bool AddTileEdge(int x, int y, int dir)
        {
            Map.Tile tile = GetTile(x, y);
            if (tile == null) return false;
            // No need to check coords here, they are already checked in GetTileNoSnap
            int cols = ThingDb.FloorTiles[usingTileMaterial].numCols;
            int rows = ThingDb.FloorTiles[usingTileMaterial].numRows;
            ushort vari = (ushort)(((x + y) / 2 % cols) + (((y % rows) + 1 + cols - ((x + y) / 2) % cols) % rows) * cols);
            tile.EdgeTiles.Add(new Map.Tile.EdgeTile(usingTileMaterial, vari, (Map.Tile.EdgeTile.Direction)dir, usingEdgeMaterial));
            return true;
        }


        public int ReMapVar(int input, bool set = false, int seed = 25)
        {
            //MessageBox.Show(usingEdgeMaterial.ToString());
            if (set)
            {
                Random rnd = new Random(seed + (int)DateTime.Now.Ticks);
                int[] v1 = new int[3] { 1, 2, 3 };
                int[] v5 = new int[3] { 5, 7, 9 };
                int[] v6 = new int[3] { 6, 8, 10 };
                int[] v12 = new int[3] { 12, 13, 14 };
                int[] v5grassDense = new int[2] { 7, 9 };

                if (varcount >= 20)
                {
                    if (input == 1) return v1[rnd.Next(0, 3)];
                    if (input == 5) return (usingEdgeMaterial == 17) ? v5[rnd.Next(1, 3)] : v5[rnd.Next(0, 3)];
                    if (input == 6) return v6[rnd.Next(0, 3)];
                    if (input == 12) return v12[rnd.Next(0, 3)];
                }
                else
                {
                    if (input == 0) return 0;
                    if (input == 1) return 1;
                    if (input == 2) return 1;
                    if (input == 3) return 1;
                    if (input == 4) return 2;
                    if (input == 5) return 3;
                    if (input == 6) return 4;
                    if (input == 7) return 3;
                    if (input == 8) return 4;    //STUPID MUD
                    if (input == 9) return 3;

                    if (input == 10) return 4;
                    if (input == 11) return 5;
                    if (input == 12) return 6;
                    if (input == 13) return 6;
                    if (input == 14) return 6;

                    if (input == 15) return 7;
                    if (input == 16) return 8;
                    if (input == 17) return 9;
                    if (input == 18) return 10;
                    if (input == 19) return 11;
                }
            }
            else
            {
                if (varcount == 20)
                {
                    if (input == 1) return 1;
                    if (input == 2) return 1;
                    if (input == 3) return 1;

                    if (input == 5) return 5;
                    if (input == 7) return 5;
                    if (input == 9) return 5;

                    if (input == 6) return 6;
                    if (input == 8) return 6;
                    if (input == 10) return 6;

                    if (input == 12) return 12;
                    if (input == 13) return 12;
                    if (input == 14) return 12;

                    return input;
                }
                else
                {
                    if (input == 0) return 0;
                    if (input == 1) return 1;

                    if (input == 2) return 4;
                    if (input == 3) return 5;
                    if (input == 4) return 6;

                    if (input == 5) return 11;    //STUPID MUD
                    if (input == 6) return 12;

                    if (input == 7) return 15;
                    if (input == 8) return 16;
                    if (input == 9) return 17;
                    if (input == 10) return 18;
                    if (input == 11) return 19;
                }
            }
            return 0;
        }

        public bool RemoveSideEdges(int x, int y, int xdir)
        {
            Map.Tile tile = GetTile(x, y);
            if (tile == null) return false;

            System.Collections.ArrayList list = new System.Collections.ArrayList();
            foreach (Map.Tile.EdgeTile edge in tile.EdgeTiles)
            {
                int dir = ReMapVar((int)edge.Dir);

                bool ok = true;
                int[] sides = new int[4] { 16, 17, 18, 19 };//xdir
                int[] corners = new int[4] { 0, 4, 11, 15 };//dir

                if (varcount < 20)
                {
                    sides = new int[4] { 8, 9, 10, 11 };
                    corners = new int[4] { 0, 2, 5, 7 };
                }

                for (int i = 0; i < sides.Length; i++)
                {
                    for (int j = 0; j < corners.Length; j++)
                    {

                        if ((dir == corners[j] && xdir == sides[i]))
                        {
                            //MessageBox.Show(dir.ToString() + " == " + corners[j].ToString() + " && " + xdir.ToString() + " == " + sides[i].ToString());
                            ok = false;
                            break;
                        }
                    }
                    if (ok == false) break;

                }
                if (ok && !(edge.Graphic == usingTileMaterial && (dir == 6 || dir == 12 || dir == 1 || dir == 5 || dir == xdir)))
                    list.Add(edge);

            }
            tile.EdgeTiles = list;
            return true;
        }
        public void RemoveCornerEdges(int x, int y)
        {
            RemoveSameEdges(x, y - 2, 0);
            RemoveSameEdges(x + 2, y, 4);
            RemoveSameEdges(x - 2, y, 11);
            RemoveSameEdges(x, y + 2, 15);
            // RemoveSameEdges(x-1, y - 1, 0);
        }

        public bool RemoveSameEdges(int x, int y, int dir)
        {
            Map.Tile tile = GetTile(x, y);
            if (tile == null) return false;

            System.Collections.ArrayList list = new System.Collections.ArrayList();
            foreach (Map.Tile.EdgeTile edge in tile.EdgeTiles)
            {
                int edgeDir = ReMapVar((int)edge.Dir);
                if (!(edge.Graphic == usingTileMaterial && (edgeDir == dir || (edgeDir == 0 && dir == 5) || (edgeDir == 4 && dir == 6) || (edgeDir == 15 && dir == 6) || (edgeDir == 11 && dir == 5) || (edgeDir == 0 && dir == 1) || (edgeDir == 4 && dir == 1) || (edgeDir == 11 && dir == 12) || (edgeDir == 15 && dir == 12))))
                    list.Add(edge);

            }
            tile.EdgeTiles = list;
            return true;
        }

        public bool RemoveTile(int x, int y)
        {
            Point pt = new Point(x, y);
            if (!map.Tiles.ContainsKey(pt)) return false;
            map.Tiles.Remove(pt);
            return true;
        }

        public void ResetRecursion()
        {
            tilesScanned.Clear();
        }

        int tileldX, tileldY;
        int tileurX, tileurY;
        public Rectangle DetermineTileCluster2(int x, int y)
        {
            tileldX = x; tileldY = y;
            tileurX = x; tileurY = y;
            FindTileCluster2(x, y);
            return new Rectangle(tileldX, tileldY, tileurX - tileldX, tileurY - tileldY);
        }

        private void FindTileCluster2(int x, int y)
        {
            if (tilesScanned.Contains(new Point(x, y))) return;
            tilesScanned.Add(new Point(x, y));
            bool a = (GetTile(x + 1, y - 1) != null);
            bool b = (GetTile(x - 1, y - 1) != null);
            bool c = (GetTile(x - 1, y + 1) != null);
            bool d = (GetTile(x + 1, y + 1) != null);

            if (c && x < tileldX && y > tileldY)
            {
                tileldX = x;
                tileldY = y;
            }

            if (d && x > tileurX && y > tileurY)
            {
                tileurX = x;
                tileurY = y;
            }

            if (a) FindTileCluster2(x + 1, y - 1);
            if (b) FindTileCluster2(x - 1, y - 1);
            if (c) FindTileCluster2(x - 1, y + 1);
            if (d) FindTileCluster2(x + 1, y + 1);
        }

        public int CountNeighborTiles2(int x, int y)
        {
            int count = 0;
            if (tilesScanned.Contains(new Point(x, y))) return count;
            //if (tilesScanned.Count > recurLimit) return count;
            tilesScanned.Add(new Point(x, y));
            bool a = (GetTile(x + 1, y - 1) != null);
            bool b = (GetTile(x - 1, y - 1) != null);
            bool c = (GetTile(x - 1, y + 1) != null);
            bool d = (GetTile(x + 1, y + 1) != null);
            if (!a && !b && !c && !d) return 0;
            if (a) count += 1 + CountNeighborTiles2(x + 1, y - 1);
            if (b) count += 1 + CountNeighborTiles2(x - 1, y - 1);
            if (c) count += 1 + CountNeighborTiles2(x - 1, y + 1);
            if (d) count += 1 + CountNeighborTiles2(x + 1, y + 1);
            return count;
        }

        public void FillTilesIsom(Point leftCorner, Point size)
        {
            int y = leftCorner.Y;
            for (int x = leftCorner.X; x < leftCorner.X + size.X; x++)
            {
                for (int xx = 0; xx < size.Y; xx++)
                {
                    PlaceTileSnap(xx + x, y + xx);
                }
                y--;
            }
        }

        public bool CheckTilesExistIsom(Point leftCorner, Point size)
        {
            Map.Tile result;
            int y = leftCorner.Y;
            for (int x = leftCorner.X; x < leftCorner.X + size.X; x++)
            {
                for (int xx = 0; xx < size.Y; xx++)
                {
                    result = GetTileSnap(xx + x, y + xx);
                    if (result != null) return true;
                }
                y--;
            }
            return false;
        }

        public List<Point> FindUnusedTilesIsom(Point leftCorner, Point size)
        {
            List<Point> result = new List<Point>();
            int y = leftCorner.Y;
            for (int x = leftCorner.X; x < leftCorner.X + size.X; x++)
            {
                for (int xx = 0; xx < size.Y; xx++)
                {
                    if (GetTileSnap(xx + x, y + xx) == null) result.Add(new Point(xx + x, y + xx));
                }
                y--;
            }
            return result;
        }

        /// <summary>
        /// Places an object onto the map.
        /// </summary>
        public Map.Object PlaceNormalObject(string name, float px, float py)
        {
            // Check coordinates
            if (px <= 0 || py <= 0 || px >= 5830 || py >= 5830) return null;

            Map.Object obj = new Map.Object(name, new PointF(px, py));
            obj.Extent = incrementObjectExtent++;
            obj.ReadRule1 = 0x3C; // DefaultXfer
            obj.ReadRule2 = 0x40;
            obj.CreateFlags = 0x1000000; // ENABLED
            map.Objects.Add(obj);
            return obj;
        }

        /// <summary>
        /// Returns distance to closest (to point px, py) object on the map with specified thingType
        /// </summary>
        public float DistanceToClosestObjectOfType(string type, float px, float py)
        {
            float result = float.MaxValue; float dist;
            foreach (Map.Object obj in map.Objects)
            {
                if (obj.Name == type)
                {
                    dist = GeneratorUtil.DistanceSq(px, py, obj.Location.X, obj.Location.Y);
                    if (dist < result)
                    {
                        result = dist;
                    }
                }
            }
            return (float)Math.Sqrt(result);
        }

        /// <summary>
        /// Returns distance to closest (to point px, py) object on the map with specified thingType
        /// </summary>
        public float DistanceToClosestObjectOfType(string[] type, float px, float py)
        {
            float result = float.MaxValue; float dist;
            foreach (Map.Object obj in map.Objects)
            {
                if (Array.IndexOf(type, obj.Name) >= 0)
                {
                    dist = GeneratorUtil.DistanceSq(px, py, obj.Location.X, obj.Location.Y);
                    if (dist < result)
                    {
                        result = dist;
                    }
                }
            }
            return (float)Math.Sqrt(result);
        }
    }
}
