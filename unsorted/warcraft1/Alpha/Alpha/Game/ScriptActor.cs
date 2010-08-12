// Copyright (C) 2005-2006 Crestez Leonard    cleonard@go.ro
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
    // Base of all script nodes.
    // Defines the next node.
    public class ScriptNode
    {
        // Next node.
        private ScriptNode next;
        public ScriptNode Next {
            get {
                return next;
            }
            set {
                next = value;
            }
        }

        // Convenience constructor.
        public ScriptNode(ScriptNode next)
        {
            this.next = next;
        }

        // Empty default constructor.
        public ScriptNode()
        {
        }
    }

    // Script node that makes the animation halt for a while.
    public class ScriptNodeWait: ScriptNode
    {
        // Amount of "cycles" to wait.
        // For an actor this is generally the number of game cycles.
        private int waitTime;
        public int WaitTime {
            get {
                return waitTime;
            }
            set {
                waitTime = value;
            }
        }

        // Convenience constructor.
        public ScriptNodeWait(ScriptNode next, int waitTime): base(next)
        {
            this.WaitTime = waitTime;
        }

        // Empty default constructor.
        public ScriptNodeWait()
        {
        }
    }

    // Script node that changes the animation frame.
    // This can be used for both sprites and key-frame models.
    public class ScriptNodeChangeFrame: ScriptNode
    {
        // New animation frame to change to.
        private int newFrame;
        public int NewFrame {
            get {
                return newFrame;
            }
            set {
                newFrame = value;
            }
        }

        // Convenience constructor.
        public ScriptNodeChangeFrame(ScriptNode next, int newFrame): base(next)
        {
            this.NewFrame = newFrame;
        }

        // Empty default constructor.
        public ScriptNodeChangeFrame()
        {
        }
    }

    // Node that breaks the current action
    // Used by actors.
    public class ScriptNodeBreakAction: ScriptNode
    {
        // Convenience constructor.
        public ScriptNodeBreakAction(ScriptNode next): base(next)
        {
        }

        // Empty default constructor.
        public ScriptNodeBreakAction()
        {
        }
    }

    // Script node for movement.
    public class ScriptNodeMove: ScriptNode
    {
        // Movement distance.
        private int moveDistance;
        public int MoveDistance {
            get {
                return moveDistance;
            }
            set {
                moveDistance = value;
            }
        }

        // Convenience constructor
        public ScriptNodeMove(ScriptNode next, int moveDist): base(next)
        {
            this.MoveDistance = moveDist;
        }

        // Empty default constructor.
        public ScriptNodeMove()
        {
        }
    }

    // Action hotspot
    // This is the time when the missile is fired, the tree is cut, damage is dealt, etc.
    public class ScriptNodeAction: ScriptNode
    {
        // Convenience constructor.
        public ScriptNodeAction(ScriptNode next): base(next)
        {
        }

        // Empty default constructor.
        public ScriptNodeAction()
        {
        }
    }

    // Script-based actor.
    public class ScriptActor: ActorBase
    {
        // Constructor.
        public ScriptActor(IList entryPoints, IActorAnimationMapper animMap)
        {
            this.entryPoints = entryPoints;
            this.animMap = animMap;
        }

        // Array of entry points.
        // This is the starting ScriptNode for each animation.
        //
        // The entry point for a certain ActorAnimation is determined by
        // the animation mapper, but you can reference an entry point directly
        // by -1 - index.
        private IList entryPoints;
        public IList EntryPoints {
            get {
                return ArrayList.ReadOnly(entryPoints);
            }
        }

        // The animation mapper, which maps ActorAnimations to entry points.
        private IActorAnimationMapper animMap;
        public IActorAnimationMapper AnimMap {
            get {
                return animMap;
            }
        }

        // Get the entry code for this animation.
        private ScriptNode GetEntryPoint(ActorAnimation anim)
        {
            int index;
            if ((int)anim < 0) {
                index = -1 - (int)anim;
            } else {
                index = AnimMap.MapAnimation(anim);
            }
            if (index < 0 || index >= entryPoints.Count) {
                return null;
            }
            return entryPoints[index] as ScriptNode;
        }

        // If the animation is valid.
        public override bool IsValidAnimation(ActorAnimation anim)
        {
            return GetEntryPoint(anim) != null;
        }

        // Set the current script node from the entry points array.
        public override void StartAnimation(ActorAnimation anim)
        {
            CurrentNode = GetEntryPoint(anim);
            if (CurrentNode == null) {
                throw new ArgumentException("Invalid animation", "anim");
            }
            Wait = 0;
            canBreak = false;
            base.StartAnimation(anim);
        }

        // Current animation frame.
        private int frame;
        public int Frame {
            get {
                return frame;
            }
            set {
                frame = value;
            }
        }

        private int Wait = 0;

        // If the animation can break right now.
        private bool canBreak;
        public override bool CanBreak()
        {
            return canBreak;
        }

        // Current script node.
        private ScriptNode CurrentNode;

        // Called in every cycle.
        public override void NextCycle()
        {
            base.NextCycle();

            if (!ControlsMovement) {
                AdvanceLocator(1);
            }

            --Wait;
            canBreak = false;
            while (Wait <= 0) {
                if (CurrentNode == null) {
                    throw new Exception("Null script node, animation broken");
                }

                if (CurrentNode is ScriptNodeWait) {
                    Wait = (CurrentNode as ScriptNodeWait).WaitTime;
                } else if (CurrentNode is ScriptNodeBreakAction) {
                    //Console.WriteLine("Break Point");
                    canBreak = true;
                    Wait = 1;
                } else if (CurrentNode is ScriptNodeChangeFrame) {
                    //Console.WriteLine("Frame Change");
                    Frame = (CurrentNode as ScriptNodeChangeFrame).NewFrame;
                } else if (CurrentNode is ScriptNodeMove) {
                    //Console.WriteLine("Advance Locator");
                    if (ControlsMovement) {
                        AdvanceLocator((CurrentNode as ScriptNodeMove).MoveDistance);
                    }
                } else if (CurrentNode is ScriptNodeAction) {
                    //Console.WriteLine("Animation Hotspot");
                    Entity.AnimationHotspot();
                }
                CurrentNode = CurrentNode.Next;
            }
        }

        // If this is set to true then movement script nodes are
        // ignored and instead AdvanceLocator(1) is called every cycle.
        private bool controlsMovement;
        public bool ControlsMovement {
            get {
                return controlsMovement;
            }
            set {
                controlsMovement = value;
            }
        }
    }

    // Produces an actor using an AnimatedSprite
    public class AnimatedSpriteActorType: GameDataObject, IActorFactory
    {
        // ScriptAnimatedObject.EntryPoints
        public IList EntryPoints;

        // If the actor controls movement.
        public bool ControlsMovement = true;

        // The animation mapper.
        public IActorAnimationMapper AnimMap;

        // Drawing tag.
        public int DrawTag;

        // Create a new actor.
        public IActor CreateActor(Entity entity)
        {
            ScriptActor actor = new ScriptActor(EntryPoints, AnimMap);

            actor.ControlsMovement = ControlsMovement;
            actor.DrawTag = DrawTag;

            return actor;
        }

        // Constructors.
        public AnimatedSpriteActorType(GameData data): base(data)
        {
        }

        public AnimatedSpriteActorType(GameState state): base(state)
        {
        }
    }
}

