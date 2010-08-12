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

using Alpha.Util;

namespace Alpha.Game
{
    // This is the master pathfinder class, part of GameData.
    // This class more or less controls everything related to pathfinding in a game.
    public class TilePathfinder: IPathfinder
    {
        // Data this is part of.
        public readonly GameData Data;

        // Size of a pathfinder tile.
        public int TileSize = 300;

        // The number of pathfinder mediums.
        public int MediumCount = 1;

        // The largest entity.
        public int LargestEntity = 1;

        // If pathfinding is smooth. Smooth pathfinding is a lot slower, but
        // looks a better.
        public bool Smooth = true;

        // If this is true then instead of moving "Speed" entities per cycle along the path,
        // entities will move <="Speed" entities on both axes. This is consistent with MANY RTS
        // games, even if it looks ugly. An entity moving diagonally with move sqrt(2) times
        // faster.
        // You probably don't want this. If you do, then you probably don't want smooth
        // pathfinding.
        public bool SquareHack = false;

        // See PathfinderSqueeze.
        public PathfinderSqueeze SqueezeMode = PathfinderSqueeze.Full;

        // Maximum search nodes.
        // This is the number of path tiles it will explore before
        // giving up and returning the most promising path.
        public int MaxSearchNodes = 5000;

        // Maximum search nodes.
        // If it searches this many nodes without finding an improvement then
        // it will give up and return the most promising path.
        public int MaxNonImprovingSearchNodes = 500;

        // The distance, in tiles, from which to start ignoring
        // non-permanent blocking.
        // < 0 disables.
        public int PermBlockDistance = 1;

        // The distance, in tiles, from which to start using
        // heuristics. In this area it's guaranteed to really find
        // the shortest path.
        // < 0 enables heuristics everywhere.
        public int HeuristicDistance = 5;

        // Path Recheck Time. Varies randomly.
        public int MinRecheckTime = 50;
        public int MaxRecheckTime = 100;

        // How many cycles to wait if the pathfinder fails.
        public int MinFailWaitTime = 20;
        public int MaxFailWaitTime = 30;

        // How many cycles to wait if an entity is stalled on the
        // path. This happens when the path is bad (quite common,
        // if another entity walks in).
        public int MinStallWaitTime = 5;
        public int MaxStallWaitTime = 10;

        // How many cycles to wait with no movement(PathStatus.Wait)
        // until giving up. FailWaitTime is the wait for an individual
        // failure, this is the maximum consecutive allowed wait, no
        // matter the cause( throttling included).
        public int MinAllowedWaitTime = 50;
        public int MaxAllowedWaitTime = 100;

        // How many nodes to search every cycle before starting to delay
        // requests. This is done in order to prevent very slow frames.
        // Instead, the pathfinder load is spread around.
        // <= 0 disables.
        public int MaxSearchNodesPerCycle = 10000;

        public TilePathfinder(GameData data)
        {
            this.Data = data;
        }

        public IPathMap CreatePathMap(GameMap map)
        {
            return new TilePathMap(map);
        }

        public ILocator CreateLocator(Entity entity)
        {
            return new TileLocator();
        }

        // Returns if point px, py is a valid destination for entity
        // This is used for fast checks of path reached.
        //
        // TODO: Make this blazing fast, right now we have a trivial implementation.
        // This doesn't really have to be blazing fast right now, but it can be made
        // much much faster.
        public bool IsValidGoalEndPoint(PathGoal goal, TileLocator uloc, int px, int py)
        {
            if (goal.Target.Entity != null && goal.Target.Entity.Locator != null &&
                    goal.Target.Entity.Locator is TileLocator && goal.RangeAroundEntity) {
                TileLocator tloc = (TileLocator)goal.Target.Entity.Locator;
                int range = goal.Range / TileSize;
                return tloc.PX - tloc.PSize - range <= px &&
                       tloc.PX + uloc.PSize + range >= px &&
                       tloc.PY - tloc.PSize - range <= py &&
                       tloc.PY + uloc.PSize + range >= py;
            } else {
                return uloc.EntityToPathCoord(goal.X - goal.Range) <= px &&
                       uloc.EntityToPathCoord(goal.X + goal.Range) >= px &&
                       uloc.EntityToPathCoord(goal.Y - goal.Range) <= py &&
                       uloc.EntityToPathCoord(goal.Y + goal.Range) >= py;
            }
        }

        // This actually implements EntityInGoal.
        public bool EntityInGoal(PathGoal goal, TileLocator uloc)
        {
            return IsValidGoalEndPoint(goal, uloc, uloc.PX, uloc.PY);
        }

        // This returns if the entity is in the path goal or not
        // This is just a quick call to IsValidEndPoint
        public bool EntityInGoal(PathGoal goal, Entity entity)
        {
            return EntityInGoal(goal, (TileLocator)entity.Locator);
        }
    }

    // Just a collection of layers.
    public class TilePathMap: IPathMap
    {
        // The map this PathMap covers.
        public readonly GameMap Map;

        // Get the pathfinder. Just a shortcut.
        public TilePathfinder Pathfinder {
            get {
                return (TilePathfinder)Map.Data.Pathfinder;
            }
        }

        // The number of layers.
        public int LayerCount {
            get {
                return Pathfinder.MediumCount;
            }
        }

        // The actual layers.
        private TilePathLayer[] layers;

        // Gets a pathfinder layer.
        public TilePathLayer GetLayer(int index)
        {
            return layers[index];
        }

        // Index, returns a certain layer.
        public TilePathLayer this[int index] {
            get {
                return GetLayer(index);
            }
        }

        public TilePathMap(GameMap map)
        {
            int w, h, i;
            this.Map = map;
            layers = new TilePathLayer[LayerCount];

            w = (Map.Width - 1) / Pathfinder.TileSize + 1;
            h = (Map.Height - 1) / Pathfinder.TileSize + 1;

            for (i = 0; i < LayerCount; ++i) {
                layers[i] = new TilePathLayer(this, i, w, h);
            }
        }

        // Update for a new cycle.
        public void NextCycle()
        {
            // NextCycle individual layers.
            foreach (TilePathLayer layer in layers) {
                layer.NextCycle();
            }
        }
    }

    // This is a class for a simple tile-based pathfinder layer.
    // It can find paths for little squares on a BlockTable.
    public class TilePathLayer
    {
        // The real block table, including moving entities.
        private BlockTable block;
        public BlockTable Block {
            get {
                return block;
            }
        }

        // The pseudo-permanent block table, without moving entities.
        private BlockTable permBlock;
        public BlockTable PermBlock {
            get {
                return permBlock;
            }
        }

        // The PathMap this layer is a part of.
        public readonly TilePathMap PathMap;

        // The gamestate we're a part of.
        public GameState State {
            get {
                return PathMap.Map.State;
            }
        }

        // Number of the layer.
        // It should hold that PathMap.GetLayer(LayerId) == this.
        public readonly int LayerId;

        // The pathfinder. keep a reference here because
        // otherwise there are so many redirections it gets slow.
        public readonly TilePathfinder Pathfinder;

        // Get the width, in tiles.
        public int Width {
            get {
                return Block.Width;
            }
        }

        // Get the height, in tiles.
        public int Height {
            get {
                return Block.Height;
            }
        }

        // Constructor.
        public TilePathLayer(TilePathMap pmap, int layer, int w, int h)
        {
            PathMap = pmap;
            LayerId = layer;
            Pathfinder = pmap.Pathfinder;

            block = new BlockTable(w, h, Pathfinder.LargestEntity);
            permBlock = new BlockTable(w, h, Pathfinder.LargestEntity);
            Matrix = new int[Width, Height];
            Nodes = new PathNode[Pathfinder.MaxSearchNodes];
            FirstNodeNumber = LastNodeNumber = 1;
            Heap = new IntHeap();
            SearchNodesThisCycle = 0;

            if (Pathfinder.PermBlockDistance >= 0) {
                int sbvSize = Pathfinder.LargestEntity + 2 * Pathfinder.PermBlockDistance;
                SavedBlockValues = new int[sbvSize, sbvSize];
            }
        }

        // Update for a new cycle.
        public void NextCycle()
        {
            /*if (SearchNodesThisCycle != 0) {
                Console.WriteLine("{0}: Pathfinder searched {1} nodes",
                        State.Cycle, SearchNodesThisCycle);
            }*/
            SearchNodesThisCycle = 0;
        }

        // This is a pathfinder node.
        public struct PathNode
        {
            // The node we got here from.
            public int Prev;

            // Position of the PathNode, in entity coords.
            public Point EPos;

            // X position.
            public int EX {
                get {
                    return EPos.X;
                }
                set {
                    EPos.X = value;
                }
            }

            // Y position.
            public int EY {
                get {
                    return EPos.Y;
                }
                set {
                    EPos.Y = value;
                }
            }

            // The distance until here.
            public int Dist;

            // If we've already expanded from this node.
            public bool Expanded;
        }

        // This is stuff used globally during find ONE path.
        // It's for internal use of FindPath and it's helpers.
        
        // The PathNode number for a certain tile
        // values => FirstNodeNumber are from the current FindPath, the
        // others are from older FindPaths and not visited in the current
        // FindPath. All numbers are < LastNodeNumber.
        //
        // Nodes[Matrix - FirstNodeNumber] is the actual node.
        public int[,] Matrix;

        // Large array of PathNodes.
        public PathNode[] Nodes;

        // First and Last valid node numbers.
        public int FirstNodeNumber;
        public int LastNodeNumber;
        
        // Heap used as a priority queue.
        private IntHeap Heap;
      
        // The pathfinder request.
        private PathGoal Goal;

        // Shortcut
        private TileLocator CLoc;

        // Center of Destination, in entity coords.
        private int TargetEX, TargetEY;

        // HackBlockTable saves old stuff here.
        private int[,] SavedBlockValues;

        // How many nodes were expanded this cycle.
        // If this goes over TilePathfinder.MaxNodesPerCycle FindPath
        // throttles (returns PathStatus.Wait for 1 cycles).
        private int SearchNodesThisCycle;

        // Used by Hack/FixBlockTable
        private Rectangle GetHackArea()
        {
            Rectangle area = new Rectangle();

            area.X1 = Math.Max(0, CLoc.PX - CLoc.PSize + 1 - Pathfinder.PermBlockDistance);
            area.Y1 = Math.Max(0, CLoc.PY - CLoc.PSize + 1 - Pathfinder.PermBlockDistance);
            area.X2 = Math.Min(Width - 1, CLoc.PX + Pathfinder.PermBlockDistance);
            area.Y2 = Math.Min(Height - 1, CLoc.PY + Pathfinder.PermBlockDistance);

            if (area.X2 - area.X1 > SavedBlockValues.GetLength(0)) {
                Console.WriteLine("Tiny hack {0}/{1}", area.X2 - area.X1,
                        SavedBlockValues.GetLength(0));
                throw new Exception("SavedBlockValues too small, bug in constructor");
            }

            return area;
        }

        // This is hacks the block table. It copies a small part of
        // RealBlock to PermBlock.
        private void HackBlockTable()
        {
            if (Pathfinder.PermBlockDistance < 0) {
                return;
            }

            int x, y;
            Rectangle area = GetHackArea();

            for (x = area.X1; x <= area.X2; ++x) {
                for (y = area.Y1; y <= area.Y2; ++y) {
                    SavedBlockValues[x - area.X1, y - area.Y1] = PermBlock[x, y];
                    PermBlock[x, y] = Block[x, y];
                }
            }

            PermBlock.UpdateForArea(area);

            // Swap. Eeeek.
            BlockTable swp;
            swp = block;
            block = permBlock;
            permBlock = swp;
        }

        // And none the wiser...
        private void FixBlockTable()
        {
            if (Pathfinder.PermBlockDistance < 0) {
                return;
            }

            // Swap back.
            BlockTable swp;
            swp = block;
            block = permBlock;
            permBlock = swp;

            int x, y;
            Rectangle area = GetHackArea();

            for (x = area.X1; x <= area.X2; ++x) {
                for (y = area.Y1; y <= area.Y2; ++y) {
                    PermBlock[x, y] = (SByte)SavedBlockValues[x - area.X1, y - area.Y1];
                }
            }

            PermBlock.UpdateForArea(area);
        }

        // Heuristic function. distance from ex, ey to the end.
        // FIXME: improve somehow?
        private int Heuristic(int ex, int ey)
        {
            return IntMath.Distance(TargetEX, TargetEY, ex, ey);
        }

        // Clean up private vars for running FindPath.
        private void InitFindPath()
        {
            int x, y;

            // 2 * just to be safe.
            if (LastNodeNumber > Int32.MaxValue - 2 * Pathfinder.MaxSearchNodes) {
                // Clean Matrix.
                for (x = 0; x < Width; ++x) {
                    for (y = 0; y < Height; ++y) {
                        Matrix[x, y] = 0;
                    }
                }
                LastNodeNumber = 1;
            }
            FirstNodeNumber = LastNodeNumber;
            Heap.Clear();
        }

        // Update(or create) a PathNode for a certain tile.
        // px, py position of tile in path coords.
        // prev is the Prev node.
        // d is the distance.
        //
        // If the node doesn't exists it is created. If the distance
        // is shorter Prev and Dist is updated and the node is added
        // to the heap again. Otherwise nothing happens.
        private void UpdateMapNode(int px, int py, int prev, int d)
        {
            int c;
            if (Matrix[px, py] < FirstNodeNumber) {
                c = (Matrix[px, py] = LastNodeNumber++) - FirstNodeNumber;
                Nodes[c].EX = CLoc.PathToEntityCoord(px);
                Nodes[c].EY = CLoc.PathToEntityCoord(py);
            } else {
                c = Matrix[px, py] - FirstNodeNumber;
                if (Nodes[c].Dist < d) {
                    return;
                }
                //Console.WriteLine("Adding {0},{1} {2} again to the heap", px, py, c);
            }
            Nodes[c].Prev = prev;
            Nodes[c].Dist = d;
            Nodes[c].Expanded = false;
            if (Math.Max(Math.Abs(px - CLoc.PX), Math.Abs(py - CLoc.PY)) > Pathfinder.HeuristicDistance) {
                Heap.Add(c, d + Heuristic(Nodes[c].EX, Nodes[c].EY));
            } else {
                Heap.Add(c, d);
            }
        }

        // Return an actual path for entity to point dx, dy.
        // dx, dy is the closest point we found possible, in U-coords
        private PathStatus TrackPath(int dx, int dy, out IList path)
        {
            path = new ArrayList();

            // Add finish point.
            if (Pathfinder.Smooth) {
                path.Add(new Point(
                    Math.Max(CLoc.PathToEntityCoord(0), dx),
                    Math.Max(CLoc.PathToEntityCoord(0), dy)));
            }

            // Backtrack.
            //Console.WriteLine("Nodes from {0} to {1} inclusive",
            //        FirstNodeNumber, LastNodeNumber - 1);
            int cn = Matrix[CLoc.EntityToPathCoord(dx), CLoc.EntityToPathCoord(dy)] - FirstNodeNumber;
            while (cn != -1 && path.Count < 100000) {
                //Console.WriteLine("Node {0} at {1}", cn, Nodes[cn].EPos);
                path.Add(Nodes[cn].EPos);
                cn = Nodes[cn].Prev;
            }
            if (path.Count >= 100000) {
                throw new Exception("Path too long, bug here");
            }

            // Post-process path.
            path.Add(CLoc.EPos);
            ((ArrayList)path).Reverse();
            PathfinderUtils.ConcatPath((ArrayList)path);
            if (Pathfinder.Smooth) {
                SimpleSmooth((ArrayList)path);
            }

            if (path.Count == 1) {
                // path.Count == 1 means we haven't moved at all.
                // Since we made a reached checked waaay back then it means we can't
                // get any closer.
                if ((Point)path[0] != CLoc.EPos || Pathfinder.EntityInGoal(Goal, CLoc)) {
                    throw new Exception("Bug here");
                }
                //Console.WriteLine("Can't get any closer");
                return PathStatus.Wait + State.SyncRand(
                        Pathfinder.MinFailWaitTime, Pathfinder.MaxFailWaitTime);
            }

            return PathStatus.Found;
        }

        // Expands a node.
        // Long boring code.
        private void ExpandFrom(int cnode)
        {
            // Position in path coods.
            int x = CLoc.EntityToPathCoord(Nodes[cnode].EX);
            int y = CLoc.EntityToPathCoord(Nodes[cnode].EY);

            // Distance to other nodes, used below.
            int dist;

            // If the neighbouring tiles are free.
            // freeXX where X is on of m, e, p: z-1,z,z+1;
            bool freeMM = Block[x - 1, y - 1] >= CLoc.PSize;
            bool freeME = Block[x - 1, y    ] >= CLoc.PSize;
            bool freeMP = Block[x - 1, y + 1] >= CLoc.PSize;
            bool freeEM = Block[x    , y - 1] >= CLoc.PSize;
            bool freeEP = Block[x    , y + 1] >= CLoc.PSize;
            bool freePM = Block[x + 1, y - 1] >= CLoc.PSize;
            bool freePE = Block[x + 1, y    ] >= CLoc.PSize;
            bool freePP = Block[x + 1, y + 1] >= CLoc.PSize;

            // Straight walk.
            dist = Nodes[cnode].Dist + Pathfinder.TileSize;

            if (freeME) {
                UpdateMapNode(x - 1, y    , cnode, dist);
            }
            if (freeEM) {
                UpdateMapNode(x    , y - 1, cnode, dist);
            }
            if (freeEP) {
                UpdateMapNode(x    , y + 1, cnode, dist);
            }
            if (freePE) {
                UpdateMapNode(x + 1, y    , cnode, dist);
            }

            // Diagonals, depending on squeeze.
            // Boooring.
            // FIXME: cache diagonal cost? seems pointless.
            dist = Nodes[Matrix[x, y] - FirstNodeNumber].Dist + 
                    IntMath.Sqrt(2 * Pathfinder.TileSize * Pathfinder.TileSize);
            if (Pathfinder.SqueezeMode == PathfinderSqueeze.None) {
                if (freeMM && freeME && freeEM) {
                    UpdateMapNode(x - 1, y - 1, cnode, dist);
                }
                if (freeMP && freeME && freeMP) {
                    UpdateMapNode(x - 1, y + 1, cnode, dist);
                }
                if (freePM && freePE && freeEM) {
                    UpdateMapNode(x + 1, y - 1, cnode, dist);
                }
                if (freePP && freePE && freeEP) {
                    UpdateMapNode(x + 1, y + 1, cnode, dist);
                }
            } else if (Pathfinder.SqueezeMode == PathfinderSqueeze.Corner) {
                if (freeMM && (freeME || freeEM)) {
                    UpdateMapNode(x - 1, y - 1, cnode, dist);
                }
                if (freeMP && (freeME || freeMP)) {
                    UpdateMapNode(x - 1, y + 1, cnode, dist);
                }
                if (freePM && (freePE || freeEM)) {
                    UpdateMapNode(x + 1, y - 1, cnode, dist);
                }
                if (freePP && (freePE || freeEP)) {
                    UpdateMapNode(x + 1, y + 1, cnode, dist);
                }
            } else if (Pathfinder.SqueezeMode == PathfinderSqueeze.Full) {
                if (freeMM) {
                    UpdateMapNode(x - 1, y - 1, cnode, dist);
                }
                if (freeMP) {
                    UpdateMapNode(x - 1, y + 1, cnode, dist);
                }
                if (freePM) {
                    UpdateMapNode(x + 1, y - 1, cnode, dist);
                }
                if (freePP) {
                    UpdateMapNode(x + 1, y + 1, cnode, dist);
                }
            }
        }

        // Add a start node. It has -1 as a prev node,
        // and the distance is calculated from CLoc.EPos
        private void AddStartNode(int px, int py)
        {
            int dist = IntMath.Distance(CLoc.EX, CLoc.EY,
                    CLoc.PathToEntityCoord(px), CLoc.PathToEntityCoord(py));
            UpdateMapNode(px, py, -1, dist);
        }

        // Add start nodes.
        private void AddStartNodes()
        {
            int px = CLoc.PX;
            int py = CLoc.PY;
            AddStartNode(px, py);
            if (Block[px + 1, py] >= CLoc.PSize) {
                AddStartNode(px + 1, py);
            }
            if (Block[px, py + 1] >= CLoc.PSize) {
                AddStartNode(px, py + 1);
            }
            // Fucking squeeze. MUST check everything to be sure.
            if (Block[px + 1, py + 1] >= CLoc.PSize &&
                    Block[px, py + 1] >= CLoc.PSize &&
                    Block[px + 1, py] >= CLoc.PSize) {
                AddStartNode(px + 1, py + 1);
            }
        }

        // Find a path for that entity all the way to the PathGoal.
        // The return value is a PathStatus:
        //     Reached: The entity was already there.
        //     Unreachable: Total failure, the entity should abort pathfinding.
        //     Found: Found a path.
        //     >= Wait: Try again, after status - PathStatus.Wait cycles.
        //
        // path is null if result != Found.
        //
        // Even if the actual goal is not reachable it will try to move as close
        // as possible to the goal.
        public PathStatus FindPath(Entity entity, PathGoal goal, out IList path)
        {
            // Best node found so far, and the score for that node.
            // Smallest score wins.
            int bnode, bscore;
            // Number of nodes explored.
            int tick;
            // Number of nodes explored since last improvement.
            int btick;

            path = null;
            PathStatus result;

            if (SearchNodesThisCycle > Pathfinder.MaxSearchNodesPerCycle &&
                    Pathfinder.MaxSearchNodesPerCycle > 0) {
                //Console.WriteLine("Already done {0} nodes, throttling", SearchNodesThisCycle);
                return PathStatus.Wait + 1;
            }

            // Fill in some usefull variables.
            Goal = goal;
            CLoc = (TileLocator)entity.Locator as TileLocator;
            TargetEX = Goal.X;
            TargetEY = Goal.Y;

            if (CLoc == null) {
                throw new ArgumentException("entity", "Entity doesn't have a TileLocator");
            }

            // Check if we are already there.
            if (Pathfinder.EntityInGoal(Goal, CLoc)) {
                //Console.WriteLine("FindPath: Already in goal");
                return PathStatus.Reached;
            }

            // If the locator is transparent then return a straight line.
            if (CLoc.Transparent) {
                path = new ArrayList(2);
                path.Add(new Point(CLoc.EX, CLoc.EY));
                path.Add(new Point(TargetEX, TargetEY));
                return PathStatus.Found;
            }

            // If the entity can't even move on it's own tile, something is wrong.
            // The pathfinder's response is to give up.
            // This can happen if:
            //   A: The entity overlaps another entity. Nothing to do.
            //   B: Entity.PSize > LargestEntity The pathfinder can't move these entities.
            //
            // While it's theoretically possible to find a path for entities bigger than
            // LargestEntity it's not exactly practical (it would be really way too slow).
            if (CLoc.PSize > Block[CLoc.PX, CLoc.PY]) {
                return PathStatus.Unreachable;
            }

            // Cleanup.
            InitFindPath();
            HackBlockTable();

            // Add starting points.
            AddStartNodes();

            //Console.WriteLine("FindPath from {0} {1} to {2} {3}",
            //        CLoc.PX, CLoc.PY, TargetPX, TargetPY);

            // Loop around.
            bnode = 0;
            bscore = Int32.MaxValue;
            tick = btick = 0;
            while (Heap.Count > 0) {
                // Current node.
                int cnode = Heap.Pop();
                int ex = Nodes[cnode].EX;
                int ey = Nodes[cnode].EY;
                int px = CLoc.EntityToPathCoord(ex);
                int py = CLoc.EntityToPathCoord(ey);

                ++SearchNodesThisCycle;

                // Don't expand twice.
                if (Nodes[cnode].Expanded) {
                    continue;
                }
                Nodes[cnode].Expanded = true;

                //Console.WriteLine("Got to {0}, {1} {2}", cx, cy, c);

                // Are we there yet?
                if (Pathfinder.IsValidGoalEndPoint(Goal, CLoc, px, py)) {
                    //Console.WriteLine("FindPath: Reached goal, Tracking path.");
                    if (Goal.IsSinglePoint) {
                        result = TrackPath(Goal.X, Goal.Y, out path);
                    } else {
                        result = TrackPath(ex, ey, out path);
                    }
                    FixBlockTable();
                    return result;
                }

                // Update partial solution
                // FIXME: Crappy score formula
                ++tick;
                ++btick;
                if (Heuristic(ex, ey) + Nodes[cnode].Dist / 3 < bscore) {
                    bscore = Heuristic(ex, ey) + Nodes[cnode].Dist / 3;
                    bnode = cnode;
                    btick = 0;
                }

                // Check node limit
                if (btick > Pathfinder.MaxNonImprovingSearchNodes ||
                        LastNodeNumber - FirstNodeNumber + 10 > Pathfinder.MaxSearchNodes) {
                    //Console.WriteLine("Node limit");
                    break;
                }

                // Expand the node.
                ExpandFrom(cnode);
            }
            //Console.WriteLine("FindPath: Giving up, Tracking path.");
            result = TrackPath(Nodes[bnode].EPos.X, Nodes[bnode].EPos.Y, out path);
            FixBlockTable();
            return result;
        }

        // See PathfinderUtils.LineWalk
        public int EntityWalk(TileLocator uloc, Point startCoord, Point endCoord,
                int startPosition, int endPosition, int length)
        {
            if (uloc.Transparent) {
                return endPosition;
            }

            // Conversions from absolut coords to tile cord have the form of
            //     TileX = (X + transX) / TileSize;
            //     TileY = (Y + transY) / TileSize;
            // For path tile conversions we have
            //     PathX = (X + uloc.DoubleTileTranslation() / 2) / PathTileSize
            //     PathY = (Y + uloc.DoubleTileTranslation() / 2) / PathTileSize
            // All we have to do is add DoubleTileTranslation() / 2 and we're down to
            // a simple division to convert.
            startCoord.X += uloc.DoubleTileTranslation / 2;
            endCoord.X += uloc.DoubleTileTranslation / 2;
            startCoord.Y += uloc.DoubleTileTranslation / 2;
            endCoord.Y += uloc.DoubleTileTranslation / 2;

            /*Console.WriteLine("Size: {0} start {1} {2} end {3} {4} sp {5} ep {6} len {7}",
                    uloc.PSize, startCoord.X, startCoord.Y, endCoord.X, endCoord.Y,
                    startPosition, endPosition, length);*/

            return PathfinderUtils.FastLineWalk(Block.Table, uloc.PSize, Pathfinder.TileSize,
                    startCoord, endCoord, startPosition, endPosition, length);

            /* This code checks FastLineWalk, assuming the above is right.
            int sres;
            int fres;
            sres = PathfinderUtils.SlowLineWalk(Block.Table, uloc.PSize, pts,
                    startCoord, endCoord, startPosition, endPosition, length);
            fres = PathfinderUtils.FastLineWalk(Block.Table, uloc.PSize, pts,
                    startCoord, endCoord, startPosition, endPosition, length);
            if (sres != fres) {
                Console.WriteLine("Size: {0} start {1} {2} end {3} {4} sp {5} ep {6} len {7} res {8},{9}",
                        uloc.PSize, startCoord.X, startCoord.Y, endCoord.X, endCoord.Y,
                        startPosition, endPosition, length, sres, fres);
                Console.WriteLine("Entity walk is WRONG, s{0} != f{1}", sres, fres);
                throw new Exception();
            }
            return fres;
            */
        }

        // Checks if an entity can walk from startCoord to endCoord.
        // Uses EntityWalk
        public bool EntityCanWalk(TileLocator uloc, Point startCoord, Point endCoord)
        {
            int SegLength = IntMath.Distance(startCoord, endCoord);
            int res = EntityWalk(uloc, startCoord, endCoord, 0, SegLength, SegLength);
            if (res != SegLength) {
                //Console.WriteLine("{0} != {1}", res, SegLength);
            }
            return res == SegLength;
        }

        public bool EntityCanWalk(TileLocator uloc, int sx, int sy, int ex, int ey)
        {
            return EntityCanWalk(uloc, new Point(sx, sy), new Point(ex, ey));
        }

        // Smooth out a path, try to get as many straight lines as possible.
        public void SimpleSmooth(ArrayList path)
        {
            int i, j = 1;
            Point a, b, c;
            if (path.Count < 3) {
                return;
            }
            a = (Point)path[0];
            b = (Point)path[1];
            for (i = 2; i < path.Count; ++i) {
                c = (Point)path[i];
                if (EntityCanWalk(CLoc, a, c)) {
                    path[j] = b = c;
                } else {
                    a = b;
                    path[++j] = b = c;
                }
            }
            path.RemoveRange(j + 1, path.Count - j - 1);
        }
    }
}
