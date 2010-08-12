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

using Alpha.Graphics;

namespace Alpha.Scene
{
    // This is an interface for an abstract graphical object.
    // A graphical object can draw itself and define it's boundaries
    // (for visibility checking).
    //
    // This is a minimal interface, for stuff required by Scene.
    //
    // You might want to implement IComparable, to be used by
    // Scene.DrawOrderComparer in case of a tie.
    public interface IGraphicObject
    {
        ISceneNode Node {
            get;
            set;
        }

        // Get the bounds of this object.
        BoundingBox GetBounds();

        // Draw the object.
        // viewVolume is the camera volume, so some objects can only draw parts of themselves.
        void DrawOrtho(BoundingBox viewVolume);
    }

    // Base class for graphical objects.
    // This class contains an explicit reference to the scene node that
    // encapsulates it for inclusion into the scene.
    public abstract class GraphicObjectBase: IGraphicObject
    {
        // This is the node that holds the object.
        private ISceneNode node;
        public ISceneNode Node {
            get {
                return node;
            }
            set {
                node = value;
            }
        }

        // Override to return the actual bounds of the object.
        public virtual BoundingBox GetBounds()
        {
            return BoundingBox.Infinite;
        }

        // Override to draw the object.
        public abstract void DrawOrtho(BoundingBox viewVolume);
    }

    // This is a wrapper for an IGraphicObject when placed in a scene.
    // Scene implementations also implement their own nodex.
    //
    // The IGraphicObject can be linked to an ISceneNode and send events.
    public interface ISceneNode
    {
        IGraphicObject GraphicObject {
            get;
        }

        // IGraphicObject can call this when it wants to remove itself.
        void Remove();
    }

    // This is a wrapper for an IGraphicObject when it's placed inside a scene.
    // Different scene implementations might have their own SceneNodes.
    //
    // In order to add an IGraphicObject to the scene you must first
    // create a SceneNode(Scene.CreateNode) and the add it to the the scene
    //
    // You should store the SceneNode, so that you can remove it later.
    public abstract class SceneNodeBase: ISceneNode
    {
        // This is the graphic object this node wraps.
        public IGraphicObject obj;
        public IGraphicObject GraphicObject {
            get {
                return obj;
            }
        }

        // Obvious constructor.
        public SceneNodeBase(IGraphicObject obj)
        {
            if (obj == null) {
                throw new ArgumentNullException("obj");
            }
            if (obj.Node != null) {
                throw new ArgumentException("Object has already been placed in a scene.", "obj");
            }
            this.obj = obj;
            obj.Node = this;
        }

        public abstract void Remove();
    }

    // This class represents a scene, a 3d world.
    // You can add Graphic Objects in the scene and draw it.
    //
    // Scene implementation can use different space partitioning techniques
    // and return difference SceneNodes from CreateNode
    public abstract class Scene
    {
        // This is used to sort visible object by draw order.
        // If you leave this to null the draw order will be undefined.
        private IComparer drawOrderComparer;
        public IComparer DrawOrderComparer {
            get {
                return drawOrderComparer;
            }
            set {
                drawOrderComparer = value;
            }
        }

        // Draw the scene.
        // The visible area is a bounding box.
        public abstract void DrawOrtho(BoundingBox viewVolume);

        // Creates a SceneNode and adds it to the scene.
        public abstract ISceneNode AddObject(IGraphicObject obj);

        // Remove a SceneNode from the scene.
        public abstract void RemoveNode(ISceneNode node);
    }

    // Derivate of SceneNode that can be used in a linked list.
    public class ListSceneNode: SceneNodeBase
    {
        // Next node.
        private ListSceneNode Next;

        // Previous node.
        private ListSceneNode Prev;

        public readonly TrivialScene Scene;

        // Default constructor.
        public ListSceneNode(IGraphicObject obj, TrivialScene scene): base(obj)
        {
            this.Scene = scene;
            Next = Prev = this;
        }

        protected ListSceneNode RootNode {
            get {
                return Scene.RootNode;
            }
            set {
                Scene.RootNode = value;
            }
        }

        // Add the node to the list.
        public void AddToList()
        {
            if (RootNode == null) {
                RootNode = this;
                Next = Prev = this;
            } else {
                this.Next = RootNode.Next;
                this.Prev = RootNode;
                RootNode.Next.Prev = this;
                RootNode.Next = this;
            }
        }

        // Remove the node from the list.
        public void RemoveFromList()
        {
            this.Next.Prev = this.Prev;
            this.Prev.Next = this.Next;
            if (this == RootNode) {
                RootNode = this.Next;
                if (this == RootNode) {
                    RootNode = null;
                }
            }
        }

        public override void Remove()
        {
            RemoveFromList();
        }

        // Select the Graphic objects from the list that are inside the viewVolume.
        // returns an ArrayList of visible graphical objects.
        public static ArrayList SelectFromList(ListSceneNode root, BoundingBox viewVolume)
        {
            ListSceneNode node;
            ArrayList list = new ArrayList();

            if (root == null) {
                return list;
            }

            node = root;
            do {
                if (BoundingBox.Intersection(viewVolume, node.GraphicObject.GetBounds())) {
                    list.Add(node.GraphicObject);
                }
                node = node.Next;
            } while (node != root);

            return list;
        }
    }

    // Trivial implementation of scene, with no space partitioning.
    // Just check visibility for every object and draw it.
    public class TrivialScene: Scene
    {
        // Root of the nodes. 
        public ListSceneNode RootNode = null;

        // Draw a rectangle-shaped region from the scene.
        public override void DrawOrtho(BoundingBox viewVolume)
        {
            ArrayList objects = ListSceneNode.SelectFromList(RootNode, viewVolume);
            if (DrawOrderComparer != null) {
                objects.Sort(DrawOrderComparer);
            }
            foreach (IGraphicObject obj in objects) {
                obj.DrawOrtho(viewVolume);
            }
        }

        // Add a node to the scene.
        public override ISceneNode AddObject(IGraphicObject obj)
        {
            ListSceneNode node = new ListSceneNode(obj, this);
            node.AddToList();
            return node;
        }

        // Remove a node from the scene.
        public override void RemoveNode(ISceneNode node)
        {
            if (!(node is ListSceneNode)) {
                throw new ArgumentException("node");
            }
            node.Remove();
        }
    }

    // Objects which support fancy draw order sorting.
    //
    // Object with a lower DrawLayer get drawn first.
    // Ties are broken using GetDrawLevel.
    //
    // Objects which don't implement this interface are treaded as
    // if these functions returned 0.
    public interface IDrawOrderable: IGraphicObject
    {
        // See interface description
        int GetDrawLayer();

        // See interface description
        double GetDrawLevel();
    }

    // IComparer to be used in a scene.
    // Compare IDrawOrderable objects.
    // if objects implement IComparable that takes precedence.
    public class DrawOrderComparer: IComparer
    {
        public int Compare(object a, object b)
        {
            int dlayera, dlayerb;
            double dlevela, dlevelb;

            if (a == b) {
                return 0;
            }

            // IComparable is king here.
            if (a is IComparable) return ((IComparable)a).CompareTo(b);
            if (b is IComparable) return -((IComparable)b).CompareTo(a);

            // Check layers.
            dlayera = a is IDrawOrderable ? ((IDrawOrderable)a).GetDrawLayer() : 0;
            dlayerb = b is IDrawOrderable ? ((IDrawOrderable)b).GetDrawLayer() : 0;
            if (dlayera != dlayerb) {
                return dlayera - dlayerb;
            }

            // Check levels.
            dlevela = a is IDrawOrderable ? ((IDrawOrderable)a).GetDrawLevel() : 0;
            dlevelb = b is IDrawOrderable ? ((IDrawOrderable)b).GetDrawLevel() : 0;
            return Math.Sign(dlevela - dlevelb);
        }
    }
}

