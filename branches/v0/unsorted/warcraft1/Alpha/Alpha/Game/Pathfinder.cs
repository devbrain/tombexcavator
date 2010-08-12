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
    // Pathfinder interface.
    public interface IPathfinder
    {
        // Create map pathfinder
        IPathMap CreatePathMap(GameMap map);

        // Creates an ILocator. You don't absolutely have to use this
        // function to create the locator for an entity, but otherwise
        // you must ensure that the locator will work with the pathfinder.
        ILocator CreateLocator(Entity entity);

        // This returns if the entity is in the path goal or not
        bool EntityInGoal(PathGoal goal, Entity entity);
    }

    // Interface for a map's pathfinder.
    // Huge, eh?
    public interface IPathMap: IUpdatable
    {
    }

    // Interface for locator objects. The pathfinder creates a locator
    // for every entity, and it's responsible for positioning and pathfinding.
    //
    // It should respond to EntityEvents and block when appropiate.
    //
    // The locator can't be told when to block explicitely. You can set
    // Transparent for *some* control. This is intentional.
    //
    // You might want to derive from EntityComponentBase
    public interface ILocator: IEntityComponent
    {
        // If this is set to true then the locator shouln't block
        // other entities. This should default to true.
        bool Transparent { get; set; }

        // Returns true if we can place the entity at x, y on the map.
        bool CanPlace(GameMap map, int x, int y);

        // Find a free spot on the map, closest to x, y.
        //
        // return true if found. The result is in ox, oy.
        bool FindFreeSpot(GameMap map, int x, int y, int range, out int ox, out int oy);

        // Get the path. For visualization purposes.
        // FIXME: Sort of a hack.
        IList GetPath();

        // Status in reaching PathGoal.
        PathStatus PathStatus { get; }

        // This is the locator's movement goal. The locator will try to move
        // towards that goal.
        //
        // The locator will NEVER change this Goal, it uses PathStatus for
        // signalling various states.
        //
        // Null is valid (and the default) and it will make the locator stand still.
        // While the goal is non-null, the Locator controls the actor animation.
        // If somebody else wants to control it, they have to set Goal to null.
        //
        // This has no immediate effect, it's handled in NextCycle().
        PathGoal PathGoal { get; set; }

        // Handle the pathfinder request(PathGoal).
        // This is called every frame by Entity.
        void NextCycle();

        // Advance the locator on the path.
        // Called from the actor, sort of evil.
        //
        // The locator ignore calls to this function if it can't advance.
        void Advance(int amount);
    }

    // This represents an area on the map that you want to find a path to.
    //
    // The member fields should be setup by an Action.
    // The public properties and functions are mostly only of interesnt to the
    // actual Pathfinder code.
    //
    // FIXME: IPathGoal? Do we really need a more complex goal
    // FIXME: min range.
    public class PathGoal
    {
        // Target, center of the goal area.
        private readonly TargetInfo target;
        public TargetInfo Target {
            get {
                return target;
            }
        }

        // Maximum range around the target.
        // If the target is an entity (.HasEntity) and RangeAroundEntity is set
        // to true (the default) then this range is around the body of the target,
        // not around the center.
        private int range;
        public int Range {
            get {
                return range;
            }
        }

        // If Range is around the entity, or around the entity's center.
        // This can be true (but meaningless) for map targets.
        private readonly bool rangeAroundEntity;
        public bool RangeAroundEntity {
            get {
                return rangeAroundEntity;
            }
        }

        // Shortcut to Target.Pos
        public Point Pos {
            get {
                return Target.Pos;
            }
        }

        // Shortcut to Target.X
        public int X {
            get {
                return Target.X;
            }
        }

        // Shortcut to Target.Y
        public int Y {
            get {
                return Target.Y;
            }
        }

        // Explicit constructor.
        public PathGoal(TargetInfo target, int range, bool around)
        {
            if (target.IsEmpty) {
                throw new ArgumentException("Target is empty.", "target");
            }
            if (range < 0) {
                throw new ArgumentOutOfRangeException("range", "Range is negative.");
            }
            this.target = target;
            this.range = range;
            this.rangeAroundEntity = around;
        }

        // Simple goal, just sets target. Range is 0.
        public PathGoal(TargetInfo target): this(target, 0, false)
        {
        }

        // Create a goal around the specified target entity.
        // RangeAroundEntity is true.
        public PathGoal(Entity target, int range)
        {
            if (target == null) {
                throw new ArgumentNullException("target");
            }
            if (target.Removed) {
                throw new ArgumentException("Target Entity removed from the game.", "target");
            }
            if (range < 0) {
                throw new ArgumentOutOfRangeException("range", "Range is negative.");
            }
            this.target = new TargetInfo(target);
            this.range = range;
            this.rangeAroundEntity = true;
        }

        // Returns true is the destination is a single point. This is the simplest kind
        // of destination, used for when you click to move an entity for instance.
        // More complex destinations are used when searching a path for attacking, building
        // and the like.
        public bool IsSinglePoint {
            get {
                return (Target.HasEntity == false || RangeAroundEntity == false)
                    && Range == 0;
            }
        }

        // Check equality. uses ==
        public override bool Equals(Object obj)
        {
            PathGoal other;
            if (obj == null || !(obj is PathGoal)) {
                return false;
            }
            other = (PathGoal)obj;
            return Target == other.Target && Range == other.Range &&
                    RangeAroundEntity == other.RangeAroundEntity;
        }

        // Fuck it.
        public override int GetHashCode()
        {
            throw new NotImplementedException();
        }

        // Checks equality.
        public static bool operator ==(PathGoal a, PathGoal b)
        {
            if ((object)a == null) {
                return (object)b == null;
            } else {
                return a.Equals(b);
            }
        }

        // Checks inequality.
        public static bool operator !=(PathGoal a, PathGoal b)
        {
            return !(a == b);
        }

        public override string ToString()
        {
            return String.Format("PathGoal({0} Range={1} Around={2})",
                    Target, Range, RangeAroundEntity);
        }
    }

    // Possible results of the pathfinder.
    public enum PathStatus {
        // You are already there, go away.
        Reached,
        // A path was found.
        Found,
        // The destination is unreachable.
        Unreachable,
        // No path found, but please try again.
        Wait,
    };

    // The kind of PathfinderSqueeze you want.
    // Squeezing controls the behaviour of the pathfinder in some tight situations
    // No Squeezing(None) will make the pathfinder only do a diagonal movement if it could
    // do a normal movement in both directions.
    // Corner Squeezing(Corner) will allow it to "squeeze" if it could go either way.
    // Full Squeezing will allow entities to move in a diagonal if both straight ways are barred.
    // Text Drawing for Corner Squeezing and Full Squeezing (that would fail on just Corner)
    // AA.##    AA.##
    // AAB##    AAB##
    // .BBC.    .BBC.
    // ..CCD    ##CCD
    // ...DD    ##.DD
    public enum PathfinderSqueeze {
        None,
        Corner,
        Full,
    };
}
