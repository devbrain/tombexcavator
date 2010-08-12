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
    // Base class for target finding actions.
    // Actions that do target finding are rather complex, so you're better of
    // deriving from this.
    public abstract class TargetFinderActionBase: ActionBase
    {
        // The ability we have found to use on FoundTarget.
        // If this is null then we haven't found anything.
        private IAbility foundAbility;
        public IAbility FoundAbility {
            get {
                return foundAbility;
            }
        }

        // The target to use foundAbility on.
        // Undefined if HasFoundTarget is false.
        private TargetInfo foundTarget;
        public TargetInfo FoundTarget {
            get {
                return foundTarget;
            }
        }

        // If we have found a target.
        public bool HasFoundTarget {
            get {
                return FoundAbility != null;
            }
        }

        // Override this to return the path goal for when you
        // don't have a target. For instance, if you are guarding some
        // other entity, return a PathGoal to follow that entity.
        // If you are patrolling return the next point in the path, etc.
        //
        // This is the path goal when no target is found.
        protected abstract PathGoal GetNormalGoal();

        // If the action should finish when GetNormalGoal is reached.
        // By default this is false.
        protected virtual bool FinishWhenReached {
            get {
                return false;
            }
        }

        // Get the pathfinder goal.
        // FoundTarget if it exists, otherwise is calls GetNormalGoal.
        private PathGoal GetPathGoal()
        {
            if (HasFoundTarget) {
                return new PathGoal(FoundTarget, FoundAbility.GetRange(Entity), true);
            } else {
                return GetNormalGoal();
            }
        }

        // This function launches the actual ability.
        // You are supposed to be close enough to the target
        // This function doesn't do any checks.
        private void LaunchAbility()
        {
            SetPathGoal(null);
            Utilities.OrientEntity(Entity, FoundTarget);
            Entity.Actor.SetAnimation(ActorAnimation.Action);
        }

        // Checks if the target is valid, return true or false
        private bool ValidTarget(IAbility abil, TargetInfo target)
        {
            return abil.Validate(Entity, target) == null;
        }

        // This will look for a target. If a target is found in range, LaunchAbility is called.
        // If the target is not in range, the entity is instructed to move in range.
        // If no target is found, the entity goes to GetNormalGoal.
        //
        // This more or less forces a complete reset of the action. You can think of it as some
        // sort of "ResetAI" function.
        protected void FindTarget()
        {
            //Console.WriteLine("{0}.FindTarget", this);
            LastTargetSearch = State.Cycle;
            Entity.TargetFinder.FindTarget(Entity, ref foundAbility, ref foundTarget);
            if (HasFoundTarget && ValidTarget(FoundAbility, FoundTarget)) {
                PathGoal goal = GetPathGoal();
                if (Data.Pathfinder.EntityInGoal(goal, Entity)) {
                    // The target is in range, launching.
                    LaunchAbility();
                } else {
                    // Target not in range, moving towards it.
                    SetPathGoal(GetPathGoal());
                }
            } else {
                // No target.
                Reset();
            }
        }

        // This function gives up on the current target(if any) and
        // goes to GetNormalGoal();
        protected void Reset()
        {
            foundAbility = null;
            foundTarget = TargetInfo.Empty;
            SetPathGoal(GetNormalGoal());
        }

        // The interval at which to check for a better target.
        // A decent value is something like a second or something.
        public int TargetSearchInterval = 100;

        // Last cycle we did a FindTarget();
        private int LastTargetSearch;

        // Set LastTargetSearch to a random value.
        // This is to prevent Target finding happening all at once.
        protected void ResetLastSearchTime()
        {
            LastTargetSearch = State.Cycle - State.SyncRand(TargetSearchInterval);
        }

        // This checks if it's time to do another FindTarget() check.
        // This will only call FindTarget if sufficient time has passed.
        private void CheckFindTarget()
        {
            if (State.Cycle - LastTargetSearch >=
                    State.SyncRand(TargetSearchInterval) + TargetSearchInterval / 2) {
                FindTarget();
            }
        }

        // This is the big function. You shouldn't override this in
        // descendants, instead you should override some of the smaller
        // functions to control how it works.
        public override void NextCycle()
        {
            base.NextCycle();
            if (ActionState != ActionState.Running) {
                return;
            }
            //if (Entity.Actor.CanBreak() == false) {
            //    return;
            //}

            if (HasFoundTarget) {
                // Check if the target is still good. If it's not, find another target.
                if (PathStatus == PathStatus.Unreachable ||
                        !ValidTarget(FoundAbility, FoundTarget)) {
                    Reset();
                    CheckFindTarget();
                    return;
                }

                // If the entity HasPathGoal and HasFoundTarget then it's following.
                // If HasPathGoal is false then we are casting.
                if (HasPathGoal) {
                    // We are following the target, check if we have reached it
                    if (PathStatus == PathStatus.Reached) {
                        LaunchAbility();
                    }
                } else {
                    CheckFindTarget();
                }
            } else {
                if ((PathStatus == PathStatus.Reached && FinishWhenReached) ||
                        PathStatus == PathStatus.Unreachable) {
                    //Console.WriteLine("E{0} Stop TargetFinding", State.GetIdent(Entity));
                    Stop(false);
                } else {
                    CheckFindTarget();
                }
            }
        }

        // Override starting
        public override void Attach(Entity entity)
        {
            base.Attach(entity);
            Reset();
            ResetLastSearchTime();
        }

        // Stop the action.
        public override void Stop(bool force)
        {
            base.Stop(force);
        }

        // Launches the ability.
        public override void AnimationHotspot()
        {
            base.AnimationHotspot();
            // Don't throw up just because the actor is stupid.
            if (!HasFoundTarget) {
                return;
            }
            // If the ability doesn't want to try again, reset.
            FoundAbility.Execute(Entity, FoundTarget);
            if (!FoundAbility.GetRepeatCast(Entity, FoundTarget)) {
                FindTarget();
            }
        }
    }

    // Action that moves and finds targets along the way
    public class HuntAction: TargetFinderActionBase, ITargetAction
    {
        // The target to move to.
        private TargetInfo target;
        public TargetInfo Target {
            get {
                return target;
            }
        }

        // This simply return the Target.
        protected override PathGoal GetNormalGoal()
        {
            return new PathGoal(Target);
        }

        // When we reach the target, we are done.
        protected override bool FinishWhenReached {
            get {
                return true;
            }
        }

        // Constructor
        public HuntAction(TargetInfo target)
        {
            this.target = target;
        }
    }

    // Action that guards a target.
    // Will only go a certain distance from the guarded target.
    //
    // If target if null it will guard the current location.
    public class GuardAction: TargetFinderActionBase
    {
        // The target of this action; the target to guard.
        // This can be an entity, not only a location.
        //
        // If you set this to the entity itself, it will guard the current
        // position. However, the current position changes when the entity
        // moves. It will, in fact, guard the original position of the entity
        // That original position is only moved when the entity is moved
        // around the map by force, not by normal movement.
        //
        // This also means that if Target.Entity == Entity, the Target is
        // in fact fake. Therefore the real target is always GuardTarget,
        // not this.
        public readonly TargetInfo Target;

        // The actual target to guard.
        // See the comments on Target.
        private TargetInfo guardTarget;
        public TargetInfo GuardTarget {
            get {
                return guardTarget;
            }
        }

        // This is also an idling action.
        public override bool GetRollover()
        {
            return true;
        }

        // Return to the guard target.
        protected override PathGoal GetNormalGoal()
        {
            return new PathGoal(GuardTarget);
        }

        // We have to modify the guard target when the entity is moved by force.
        public override void EntityPlacedOnMap(GameMap map, Point newpos)
        {
            if (Target.HasEntity && Target.Entity == Entity) {
                guardTarget = new TargetInfo(Entity.Map, Entity.X, Entity.Y);
                Reset();
                ResetLastSearchTime();
            }
        }

        // Constructor
        public GuardAction(TargetInfo target)
        {
            this.Target = target;
            if (Target.IsEmpty) {
                throw new InvalidOperationException("Target is empty.");
            }
        }

        public override void Attach(Entity entity)
        {
            if (Target.Entity == entity && entity.OffMap == false) {
                guardTarget = new TargetInfo(entity.Map, entity.X, entity.Y);
            } else {
                guardTarget = Target;
            }
            base.Attach(entity);
        }
    }
}
