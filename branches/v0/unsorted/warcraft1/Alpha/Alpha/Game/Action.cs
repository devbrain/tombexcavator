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
    // Action interface.
    // You should derive from ActionBase instead of implementing from scratch.
    //
    // Starting the action is done with IEntityComponent.Attach.
    // .Stop(true) will always be called before Detach.
    public interface IAction: IEntityComponent
    {
        // This is called to stop the action.
        // If force is false then the action doesn't have to stop right
        // away, but if force=true then GetRollover() MUST return false.
        //
        // Implementors should handle extra calls.
        void Stop(bool force);

        // If this returns true then the OrderQueue will start execute
        // the next order (if one is available). This should return true
        // for finished actions or for idle actions.
        bool GetRollover();

        // This function gets called whenever the actor animation
        // reaches the "hot-spot": sword hits, bullet fires etc.
        // FIXME: hack, better idea? events suck.
        void AnimationHotspot();

        // Called every cycle to execute the action.
        void NextCycle();
    }

    // Possible states for an action.
    public enum ActionState
    {
        NotAttached,
        Running,
        Stopping,
        Finished,
    }

    // Base IAction implementation. It will register itself
    // as an EntityComponent and receive events. It also implements
    // basic ActionState advancement.
    public abstract class ActionBase: EntityComponentBase, IAction
    {
        // The state of the action. Can changed in children using SetActionState.
        private ActionState actionState;
        protected ActionState ActionState {
            get {
                return actionState;
            }
        }

        // By default this checks that ActionState is Finished.
        // You should override this in idle actions to always return true.
        public virtual bool GetRollover()
        {
            return ActionState == ActionState.Finished;
        }

        // Set the current action state.
        protected void SetActionState(ActionState newstate)
        {
            if (newstate < actionState) {
                throw new InvalidOperationException(String.Format("Can't go to {0} from {1}", newstate, actionState));
            }
            /*if (newstate != ActionState) {
                Console.WriteLine("{0} going from {1} to {2}", this, ActionState, newstate);
                Console.WriteLine(Environment.StackTrace);
            }*/
            actionState = newstate;
        }

        // Start the action, moves from ActionState.NotStarted to ActionState.Running.
        public override void Attach(Entity entity)
        {
            base.Attach(entity);
            SetActionState(ActionState.Running);
        }

        // Detach the action. Checks ActionState.Finished.
        public override void Detach()
        {
            if (ActionState != ActionState.Finished) {
                throw new InvalidOperationException("Action not finished, can't detach");
            }
            actionState = ActionState.NotAttached;
            base.Detach();
        }

        // If this is true then the action will automagically advance from
        // Stopping to Finished if Actor.CanBreak() returns false.
        // FIXME: hack.
        //
        // Default to true, override to disable.
        protected virtual bool StandardSmooth()
        {
            return true;
        }

        // Execute the current action.
        // The base impl only checks a valid ActionState.
        public override void NextCycle()
        {
            base.NextCycle();
            if (ActionState == ActionState.Stopping &&
                    StandardSmooth() && Entity.Actor.CanBreak()) {
                SetActionState(ActionState.Finished);
            }
        }

        // This is called to stop the action.
        // Sets ActionState.Stopping or ActionState.Finished
        // depending on the value of force.
        public virtual void Stop(bool force)
        {
            CheckAttached();
            if (ActionState == ActionState.Finished) {
                return;
            }
            if (force) {
                SetActionState(ActionState.Finished);
            } else {
                SetActionState(ActionState.Stopping);
            }
        }

        // This function gets called whenever the actor animation
        // reaches the "hot-spot": sword hits, bullet fires etc.
        public virtual void AnimationHotspot()
        {
        }

        // Default constructor, sets ActionState.NotStarted
        public ActionBase()
        {
            actionState = ActionState.NotAttached;
        }

        // Shortcut.
        protected bool EntityInGoal(PathGoal goal)
        {
            return Data.Pathfinder.EntityInGoal(goal, Entity);
        }

        // Checks if the entity has a path goal.
        protected bool HasPathGoal {
            get {
                return Entity.Locator.PathGoal != null;
            }
        }

        // Shortcut to PathStatus.
        protected PathStatus PathStatus {
            get {
                return Entity.Locator.PathStatus;
            }
        }

        // Sets Locator.PathGoal
        protected void SetPathGoal(PathGoal goal)
        {
            Entity.Locator.PathGoal = goal;
        }

        // Sets Locator.PathGoal
        protected void SetPathGoal(TargetInfo target, int range, bool around)
        {
            Entity.Locator.PathGoal = new PathGoal(target, range, around);
        }

        // Set a single point PathGoal. (PathGoal.IsSinglePoint).
        protected void SetPathGoal(TargetInfo target)
        {
            Entity.Locator.PathGoal = new PathGoal(target);
        }

        // Sets PathGoal to an area around an entity (PathGoal.RangeAroundEntity).
        protected void SetPathGoal(Entity target, int range)
        {
            Entity.Locator.PathGoal = new PathGoal(target, range);
        }
    }

    // Simple idle action, doesn't do anything.
    public class ActionIdle: ActionBase
    {
        // Always true.
        public override bool GetRollover()
        {
            return true;
        }
    }

    // Basic interface you should implement if the action has a tangible target.
    // If you do, you get to use TargetActionDrawer for free
    //
    // Nothing forces the target to be constant.
    public interface ITargetAction
    {
        // The target for this action.
        TargetInfo Target { get; }
    }

    // Base class for actions from TargetOrders
    public abstract class TargetAction: ActionBase, ITargetAction
    {
        // Covenience prop.
        private TargetInfo target;
        public TargetInfo Target {
            get {
                return target;
            }
        }

        // Constructor, must set target.
        public TargetAction(TargetInfo target)
        {
            this.target = target;
        }
    }

    // Movement action. Will follow the target.
    public class MoveAction: TargetAction
    {
        // Execute the action.
        public override void NextCycle()
        {
            base.NextCycle();
            // Pathfinder gave up. damn.
            if (PathStatus == PathStatus.Unreachable) {
                Stop(false);
            }
            // Following doesn't stop when reached, only normal movement.
            if (PathStatus == PathStatus.Reached && Target.HasEntity == false) {
                Stop(false);
            }
        }

        // Start the action, SetPathGoal(Target);
        public override void Attach(Entity entity)
        {
            base.Attach(entity);
            SetPathGoal(Target);
        }

        // Stops moving.
        public override void Stop(bool force)
        {
            base.Stop(force);
        }

        // Constructor
        public MoveAction(TargetInfo target): base(target)
        {
        }
    }

    // Go to a TargetInfo and use an ability on it.
    public class AbilityAction: TargetAction
    {
        // The ability to use.
        public readonly IAbility Ability;

        // Get the PathGoal.
        // This is centered around the target.
        private PathGoal GetPathGoal()
        {
            return new PathGoal(Target, Ability.GetRange(Entity), true);
        }

        // Execute the action.
        public override void NextCycle()
        {
            base.NextCycle();
            if (Entity.Actor.CanBreak() == false) {
                return;
            }

            // If we can't target the entity anymore we give up.
            if (Ability.Validate(Entity, Target) != null) {
                Stop(true);
                return;
            }
            // Pathfinder gave up. damn.
            if (PathStatus == PathStatus.Unreachable) {
                Stop(false);
            }

            if (HasPathGoal) {
                if (PathStatus == PathStatus.Reached) {
                    SetPathGoal(null);
                    Utilities.OrientEntity(Entity, Target);
                    Entity.Actor.SetAnimation(Ability.GetAnimation(Entity));
                }
            } else {
                Utilities.OrientEntity(Entity, Target);
                if (!EntityInGoal(GetPathGoal())) {
                    SetPathGoal(GetPathGoal());
                }
            }
        }

        // Start the action, get moving.
        public override void Attach(Entity entity)
        {
            base.Attach(entity);
            SetPathGoal(GetPathGoal());
        }

        // Stop the action.
        public override void Stop(bool force)
        {
            base.Stop(force);
            SetPathGoal(null);
        }

        // Handle animation Hotspot.
        public override void AnimationHotspot()
        {
            base.AnimationHotspot();
            Ability.Execute(Entity, Target);
            if (!Ability.GetRepeatCast(Entity, Target)) {
                Stop(false);
            }
        }

        // Constructor
        public AbilityAction(TargetInfo target, IAbility abil): base(target)
        {
            this.Ability = abil;
        }
    }

    // Action that executes and activity.
    // Order must ensure CanExecute (via IsValid).
    public class ActivityAction: ActionBase
    {
        // The activity to execute.
        public readonly IActivity Activity;

        // Current progress.
        private int progress;
        public int Progress {
            get {
                return progress;
            }
        }

        // Total duration
        public int Duration {
            get {
                return Activity.GetDuration(Entity);
            }
        }

        // Start the activity.
        public override void Attach(Entity entity)
        {
            base.Attach(entity);
            Activity.Start(entity);
            progress = 0;
        }

        // Stop the activity. There is no smoothness.
        public override void Stop(bool force)
        {
            if (ActionState != ActionState.Finished) {
                base.Stop(true);
                Activity.Abort(Entity, Progress);
            }
        }

        // Next cycle.
        public override void NextCycle()
        {
            base.NextCycle();
            ++progress;
            if (Progress >= Duration) {
                Activity.Finish(Entity);
                SetActionState(ActionState.Finished);
            }
        }

        // No standard Actor.CanBreak smoothing.
        protected override bool StandardSmooth()
        {
            return false;
        }

        // Constructor.
        public ActivityAction(IActivity activity)
        {
            if (activity == null) {
                throw new ArgumentNullException("activity");
            }
            this.Activity = activity;
        }
    }
}
