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
    // This class is responsible with placing an entity on the map
    // and finding paths for it.
    public class TileLocator: EntityComponentBase, ILocator
    {
        // X pos, in entity coords.
        public int EX {
            get {
                return Entity.X;
            }
        }

        // Y pos, in entity coords.
        public int EY {
            get {
                return Entity.Y;
            }
        }

        // Position as a Point in entity coords.
        public Point EPos {
            get {
                return new Point(EX, EY);
            }
        }

        // X pos, in path coords.
        // Calculated from EX on the fly.
        public int PX {
            get {
                return EntityToPathCoord(EX);
            }
        }
        
        // Y coordinate, in path coords.
        // Calculated from EY on the fly.
        public int PY {
            get {
                return EntityToPathCoord(EY);
            }
        }

        // Position as a Point in path coords.
        public Point PPos {
            get {
                return new Point(PX, PY);
            }
        }

        // Convert entity to path coords.
        // It returns what would the PX of an entity with EX = x be.
        public int EntityToPathCoord(int ez)
        {
            return (ez + DoubleTileTranslation / 2) / PathTileSize;
        }

        // Convert entity to path coords.
        public Point EntityToPathCoord(Point p)
        {
            return new Point((p.X + DoubleTileTranslation / 2) / PathTileSize,
                             (p.Y + DoubleTileTranslation / 2) / PathTileSize);
        }

        // Convert path to entity coords. This will take PSize into acount.
        // You can always count that:
        //   EntityToPathCoord(uz = PathToEntityCoord(pz)) == pz.
        // There are multiple uz values for which the above is true, PathTileSize
        // values to be exact. This returns the middle value, rounded down.
        public int PathToEntityCoord(int pz)
        {
            return pz * PathTileSize - (DoubleTileTranslation - PathTileSize) / 2;
        }

        // Convert path to entity coords.
        public Point PathToEntityCoord(Point p)
        {
            return new Point(p.X * PathTileSize - (DoubleTileTranslation - PathTileSize) / 2,
                             p.Y * PathTileSize - (DoubleTileTranslation - PathTileSize) / 2);
        }

        // The layer of the pathfinder to be placed or moved around in. 
        // Can only be changed while OffMap.
        private int movingMedium;
        public int MovingMedium {
            get {
                return movingMedium;
            }
            set {
                if (!Entity.OffMap) {
                    throw new InvalidOperationException("Remove from map first.\n");
                }
                movingMedium = value;
            }
        }

        // The size of the entity.
        // This can only be modifyed while OffMap
        private int size;
        public int Size {
            get {
                return size;
            }
            set {
                if (size < 0) {
                    throw new ArgumentOutOfRangeException("size");
                }
                if (Entity.OffMap) {
                    size = value;
                    psize = (size - 1) / PathTileSize + 1;
                    dtt = ((size - 1) / PathTileSize) * PathTileSize + 1;
                } else {
                    throw new InvalidOperationException("Remove from the map first.");
                }
            }
        }

        // Get the size of the entity in P-coords. This is used in pathfinding.
        // Cached.
        private int psize;
        public int PSize {
            get {
                return psize;
            }
        }

        // This is used in entity-to-path coordinate transformations.
        // It's pretty wierd, this formula ensures that px, py is the tile of the
        // lower-right(++) corner of the entity.
        // Cached.
        private int dtt;
        internal int DoubleTileTranslation {
            get {
                return dtt;
            }
        }

        // Shortcut
        private TilePathMap PathMap {
            get {
                return (TilePathMap)Entity.Map.PathMap;
            }
        }

        // Shortcut
        private TilePathfinder Pathfinder {
            get {
                return (TilePathfinder)Data.Pathfinder;
            }
        }

        // Shortcut
        private TilePathLayer PathLayer {
            get {
                return PathMap[MovingMedium];
            }
        }

        // Shortcut
        private int PathTileSize {
            get {
                return Pathfinder.TileSize;
            }
        }

        // If this is set to true then it won't actually block anymore.
        // This is very useful.
        private bool transparent = true;
        public bool Transparent {
            get {
                return transparent;
            }
            set {
                CheckAttached();
                if (transparent == value) {
                    return;
                }
                if (Entity.OffMap) {
                    transparent = value;
                } else {
                    Unblock();
                    transparent = value;
                    Block();
                    OnGoalChange();
                }
            }
        }

        // If this is true then block in PermBlock too.
        private bool permBlock;
        private bool PermBlock {
            get {
                return permBlock;
            }
            set {
                if (permBlock == value) {
                    return;
                }
                if (Entity.OffMap == false && Transparent == false) {
                    if (value) {
                        PathLayer.PermBlock.Block(PX - PSize + 1, PY - PSize + 1, PX, PY);
                    } else {
                        PathLayer.PermBlock.Unblock(PX - PSize + 1, PY - PSize + 1, PX, PY);
                    }
                }
                permBlock = value;
            }
        }

        // Block at a certain location.
        // Depends on Transparent and PermBlock
        private void Block(GameMap map, Point p)
        {
            TilePathLayer layer = ((TilePathMap)map.PathMap)[MovingMedium];
            if (Transparent == false) {
                if (PermBlock) {
                    layer.PermBlock.Block(p.X - PSize + 1, p.Y - PSize + 1, p.X, p.Y);
                    //Console.WriteLine("PermBlock {0} {1}", p.X, p.Y);
                }
                //Console.WriteLine("Block {0} {1}", p.X, p.Y);
                layer.Block.Block(p.X - PSize + 1, p.Y - PSize + 1, p.X, p.Y);
            }
        }

        // Shortcut
        private void Block()
        {
            Block(Entity.Map, PPos);
        }

        // Unblock at a certain location.
        // Depends on Transparent and PermBlock
        private void Unblock(GameMap map, Point p)
        {
            TilePathLayer layer = ((TilePathMap)map.PathMap)[MovingMedium];
            if (Transparent == false) {
                if (PermBlock) {
                    layer.PermBlock.Unblock(p.X - PSize + 1, p.Y - PSize + 1, p.X, p.Y);
                    //Console.WriteLine("PermUnblock {0} {1}", p.X, p.Y);
                }
                //Console.WriteLine("Unblock {0} {1}", p.X, p.Y);
                layer.Block.Unblock(p.X - PSize + 1, p.Y - PSize + 1, p.X, p.Y);
            }
        }

        // Shortcut
        private void Unblock()
        {
            Unblock(Entity.Map, PPos);
        }

        // Handle placing on the map.
        public override void EntityPlacedOnMap(GameMap map, Point pos)
        {
            // HACK: Initialize permblock.
            // It's cleanly recalculated every frame so the actual value
            // here doesn't really matter.
            permBlock = !Entity.CanMove();
            Block(map, EntityToPathCoord(pos));
        }

        // Handle removing from the map.
        public override void EntityRemovedFromMap(GameMap map, Point pos)
        {
            Unblock(map, EntityToPathCoord(pos));
            PathGoal = null;
        }

        // Handle the unit moving on the map.
        // Only reblock if we really have to.
        public override void EntityMoved(Point oldpos, Point newpos)
        {
            Point oldppos = EntityToPathCoord(oldpos);
            Point newppos = EntityToPathCoord(newpos);
            if (oldppos != newppos) {
                Unblock(Entity.Map, oldppos);
                Block(Entity.Map, newppos);
            }
        }

        // Returns if we can place the entity at px, py in path coords.
        private bool CanPlacePathCoord(GameMap map, int px, int py)
        {
            if (Transparent) {
                return true;
            } else {
                return ((TilePathMap)map.PathMap)[MovingMedium].Block.AreaIsFree(
                        px - PSize + 1, py - PSize + 1, px, py);
            }
        }

        // Returns if we can place the entity at ex, euy in entity coords.
        public bool CanPlace(GameMap map, int ex, int ey)
        {
            return CanPlacePathCoord(map, EntityToPathCoord(ex), EntityToPathCoord(ey));
        }

        // Find a free spot on the map, closest to ex, ey.
        // It does a silly spiral search to find a good spot.
        //
        // Coords are in entity coords.
        //
        // return true if found. The result is in ox, oy.
        public bool FindFreeSpot(GameMap map, int ex, int ey, int range, out int ox, out int oy)
        {
            int i, j, px, py;

            ox = ex;
            oy = ey;

            // Check if x, y is free.
            if (CanPlace(map, ex, ey)) {
                return true;
            }

            // support huge ranges.
            if (range > Int32.MaxValue - PathTileSize + 1) {
                range = Int32.MaxValue;
            } else {
                range = (range + PathTileSize - 1) / PathTileSize;
            }

            // Look around.
            px = EntityToPathCoord(ex);
            py = EntityToPathCoord(ey);

            for (i = 1; i <= range; ++i) {
                for (j = 0; j < 2 * i; ++j) {
                    if (CanPlacePathCoord(map, px - i + j, py - i)) {
                        ox = PathToEntityCoord(px - i + j);
                        oy = PathToEntityCoord(py - i);
                        return true;
                    }
                }
                for (j = 0; j < 2 * i; ++j) {
                    if (CanPlacePathCoord(map, px + i, py - i + j)) {
                        ox = PathToEntityCoord(px + i);
                        oy = PathToEntityCoord(py - i + j);
                        return true;
                    }
                }
                for (j = 0; j < 2 * i; ++j) {
                    if (CanPlacePathCoord(map, px + i - j, py + i)) {
                        ox = PathToEntityCoord(px + i - j);
                        oy = PathToEntityCoord(py + i);
                        return true;
                    }
                }
                for (j = 0; j < 2 * i; ++j) {
                    if (CanPlacePathCoord(map, px - i, py + i - j)) {
                        ox = PathToEntityCoord(px - i);
                        oy = PathToEntityCoord(py + i - j);
                        return true;
                    }
                }
            }
            return false;
        }

        // This is a list of points, in order, that the entity must walk on
        // to reach PathGoal. This can be null, when we don't have a goal or we
        // reached the goal.
        //
        // The path contains both the start and the end point. The entity moves on the
        // path one segment at a time, and deletes the first segment when it's done with it.
        // This means that the entity is between Path[0] and Path[1].
        private IList Path;

        // Get the path.
        public IList GetPath()
        {
            return Path == null ? null : ArrayList.ReadOnly(Path);
        }

        // Status in reaching PathGoal.
        private PathStatus pathStatus;
        public PathStatus PathStatus {
            get {
                return pathStatus;
            }
        }

        // The goal of this entity, as a pathfinder request.
        // The entity will try to move towards that location.
        private PathGoal pathGoal;
        public PathGoal PathGoal {
            get {
                return pathGoal;
            }
            set {
                if (PathGoal != value) {
                    pathGoal = value;
                    OnGoalChange();
                }
            }
        }

        // Called when the pathfinder goal is changed.
        private void OnGoalChange()
        {
            if (PathGoal != null) {
                // Console.WriteLine("{0}: E{1}: New path goal", State.Cycle, State.GetIdent(Entity));
                // Sort of a hack, ensures resetting MaxWaitCycle.
                pathStatus = PathStatus.Reached;
                OnChangePathStatus(PathStatus.Wait);
            }
            Path = null;
        }

        // Next time we want to do a path check.
        private int NextPathCheck;

        // If the pathfinder doesn't find a path until MaxWaitCycle,
        // we give up. This is reset when a path is found.
        private int MaxWaitCycle;

        // Target position at last check
        private Point LastPathCheckTargetPos;

        // Call this to change PathStatus
        //
        // It uses the same PathStatus conventions as TilePathfinder.FindPath:
        // status >= PathStatus.Wait means wait status - PathStatus.Wait cycles.
        private void OnChangePathStatus(PathStatus status)
        {
            if (status >= PathStatus.Wait) {
                // Handle waiting.

                // If we just started waiting update MaxWaitCycle.
                if (PathStatus != PathStatus.Wait) {
                    MaxWaitCycle = State.Cycle + State.SyncRand(
                            Pathfinder.MinAllowedWaitTime, Pathfinder.MaxAllowedWaitTime);
                }

                // Update NextPathCheck and PathSattus
                NextPathCheck = State.Cycle + status - PathStatus.Wait;

                // Give up if we have to wait or too long.
                if (NextPathCheck > MaxWaitCycle) {
                    //Console.WriteLine("{0}: E{1}: I give up",
                    //        State.Cycle, State.GetIdent(Entity));
                    pathStatus = PathStatus.Unreachable;
                } else {
                    pathStatus = PathStatus.Wait;
                    //Console.WriteLine("{0}: E{1}: Wait until {2}, max {3}",
                    //        State.Cycle, State.GetIdent(Entity), NextPathCheck, MaxWaitCycle);
                }
            } else if (status == PathStatus.Found) {
                // Found
                pathStatus = PathStatus.Found;
                NextPathCheck = State.Cycle + State.SyncRand(
                        Pathfinder.MinRecheckTime, Pathfinder.MaxRecheckTime);
                LastPathCheckTargetPos = PathGoal.Pos;
                //if (!Transparent) {
                //    Console.WriteLine("{0}: E{1}: Found path, next check {2}",
                //            State.Cycle, State.GetIdent(Entity), NextPathCheck);
                //}
            } else if (status == PathStatus.Reached || status == PathStatus.Unreachable) {
                // Unreachable
                pathStatus = status;
                //Console.WriteLine("{0}: E{1}: PathStatus {2}",
                //        State.Cycle, State.GetIdent(Entity), PathStatus);
            } else {
                throw new ArgumentException("status");
            }
        }

        // Micro-movement start and end locations.
        private Point MoveStart;
        private Point MoveEnd;

        // The distance between MoveStart and MoveEnd.
        private int MoveDist;

        // How much the entity moved from MoveStartX/Y to MoveEndX/Y
        // MovedDist = -1 stands as a reset marker, if it's -1 then
        // we have to load Start/End from the path.
        private int MovedDist = -1;

        // Reset entity entity micro-movement.
        // Just call this whenever the path changes.
        private void OnPathChange()
        {
            // Nothing to do dude.
            if (Path == null) {
                return;
            }

            // Remove duplicate points.
            while (Path.Count >= 2 && (Point)Path[0] == (Point)Path[1]) {
                Path.RemoveAt(1);
            }

            // Nothing to do.
            if (Path.Count < 2) {
                return;
            }

            MoveStart = (Point)Path[0];
            MoveEnd = (Point)Path[1];
            if (MoveStart.X != Entity.X || MoveStart.Y != Entity.Y) {
                throw new Exception("First element in path should be the entity's position.");
            }
            MovedDist = 0;

            // Calculate the length of the whole segment
            // It's surprising that this is all we have to do for the Square Movement Hack.
            if (Pathfinder.SquareHack) {
                MoveDist = Math.Max(Math.Abs(MoveEnd.X - MoveStart.X), Math.Abs(MoveEnd.Y - MoveStart.Y));
            } else {
                MoveDist = IntMath.Distance(MoveStart, MoveEnd);
            }

            // Update the orientation, only if the segment is non-zero
            Utilities.OrientEntityAlong(Entity, MoveEnd - MoveStart);

            //Console.WriteLine("Reset Micro Movement, from {0}, {1} to {2}, {3}",
            //        MoveStart.X, MoveStart.Y, MoveEnd.X, MoveEnd.Y);
        }

        // This moves the entity a certain distance, following the path.
        public void Advance(int distance)
        {
            // Nothing to do dude.
            if (Path == null || Path.Count < 2) {
                return;
            }

            // Calculate the new value of MovedDist
            int newMovedDist;
            newMovedDist = Math.Min(MoveDist, MovedDist + distance);
            Unblock();
            newMovedDist = PathLayer.EntityWalk(this, MoveStart, MoveEnd, MovedDist, newMovedDist, MoveDist);

            // Not moved, blocked on path.
            if (newMovedDist == MovedDist) {
                Block();
                //Console.WriteLine("{0}: E{1}: Blocked on path",
                //        State.Cycle, State.GetIdent(Entity));
                OnChangePathStatus(PathStatus.Wait + State.SyncRand(
                            Pathfinder.MinStallWaitTime, Pathfinder.MaxStallWaitTime));
                Path = null;
                return;
            }

            // Do the actual movement.
            int x, y, px, py, z = newMovedDist;
            while (true) {
                x = MoveStart.X + (MoveEnd.X - MoveStart.X) * z / MoveDist;
                y = MoveStart.Y + (MoveEnd.Y - MoveStart.Y) * z / MoveDist;
                px = EntityToPathCoord(x);
                py = EntityToPathCoord(y);
                if (PSize > PathLayer.Block[px, py] && Transparent == false) {
                    //Console.WriteLine("{0} {1}/p{2} {3} taken", x, y, px, py);
                    if (--z > MovedDist) {
                        continue;
                    }
                }
                break;
            }
            Block();
            MovedDist = newMovedDist;
            Entity.MoveOnMap(x, y);

            // Handle end of segment.
            if (MovedDist == MoveDist) {
                Path.RemoveAt(0);
                OnPathChange();
            }
        }

        // Determine if we need to find a new path, called from NextCycle.
        private bool NeedNewPath()
        {
            // Pathfinding is free, so we always do it.
            if (Transparent) {
                return true;
            }

            // I was told to wait, and I'm not done, so I won't bug the pathfinder again.
            // This might be used by the pathfinder as a way to distribute the load. maybe?
            if (PathStatus == PathStatus.Wait && State.Cycle < NextPathCheck) {
                return false;
            }

            // If the path is bad (we are finished, etc) search for a new one.
            if ((Path == null) || (Path.Count < 2)) {
                //Console.WriteLine("NeedNewPath: Bad path (most likely blocked)");
                return true;
            }

            // Smoothness hack. Don't do the rest of the checks except when
            // Actor.CanBreak smoothly.
            if (Entity.Actor != null && !Entity.Actor.CanBreak()) {
                return false;
            }
            
            // We have to check again every now and then.
            // Pathfinding for transparent entities is free so we always do it.
            if (State.Cycle >= NextPathCheck) {
                //Console.WriteLine("NeedNewPath: RecheckTime");
                return true;
            }

            if (PathGoal.Target.HasEntity) {
                int distTargetMoved, distToTarget;
                distTargetMoved = IntMath.Distance(LastPathCheckTargetPos, PathGoal.Pos);
                distToTarget = IntMath.Distance(LastPathCheckTargetPos, Entity.Pos) - PathGoal.Range;
                if (distTargetMoved > PathTileSize / 2 && distToTarget / distTargetMoved > 3) {
                    //Console.WriteLine("NeedNewPath: Target moved.");
                    return true;
                }
            }

            // Unless we have a reason to find a new path, we shouldn't.
            // Pathfinding is expensive.
            return false;
        }

        // Handle the pathfinder request(PathGoal).
        // This is called every frame!
        public override void NextCycle()
        {
            if (PathGoal == null || Entity.OffMap || PathStatus == PathStatus.Unreachable) {
                // Nothing to do, waiting.
                PermBlock = true;
                return;
            }

            // Give up on invalid target.
            if (Entity.ValidateTarget(PathGoal.Target) != null) {
                OnChangePathStatus(PathStatus.Unreachable);
                PermBlock = true;
                Path = null;
                return;
            }

            // Are we there yet?
            if (Pathfinder.EntityInGoal(PathGoal, this)) {
                // If so, stop moving, hooray.
                Path = null;
                OnChangePathStatus(PathStatus.Reached);
            } else {
                if (NeedNewPath()) {
                    // So we need a new path. Let's look for it.
                    Unblock();
                    OnChangePathStatus(PathLayer.FindPath(Entity, PathGoal, out Path));
                    Block();
                    //if (!Transparent) {
                    //    Console.WriteLine("{1}: E{0} Searched for a new path, result: {2}",
                    //            State.GetIdent(Entity), State.Cycle, PathStatus);
                    //}
                    OnPathChange();
                }
            }

            // Update actor animation.
            if (Entity.Actor != null) {
                if (PathStatus == PathStatus.Found) {
                    Entity.Actor.SetAnimation(ActorAnimation.Move);
                } else {
                    Entity.Actor.SetAnimation(ActorAnimation.Idle);
                }
            }
            PermBlock = (PathStatus != PathStatus.Found);
        }

        public override void Attach(Entity entity)
        {
            base.Attach(entity);
            OnGoalChange();
        }
    }
}
