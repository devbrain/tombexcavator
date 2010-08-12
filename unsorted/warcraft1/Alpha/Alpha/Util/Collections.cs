// Copyright (C) 2004-2005 Crestez Leonard    cleonard@go.ro
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

namespace Alpha.Util
{
    // Readonly interface for an object id manager.
    // Methods should be self-explanatory.
    public interface IObjectIdManager
    {
        // If id is assigned
        bool HasIdent(uint id);

        // If obj has an id assigned;
        bool HasObject(object obj);

        // Get the object with the specified id.
        // Throws an exception if id not found.
        object GetObject(uint id);

        // Get the ident of the specified object
        // Throws an exception if object doesn't have an id.
        uint GetIdent(object obj);
    }

    // This class manages identifiers for a bunch of objects.
    // You can think of it as a two-way Hashtable where one side is always uint.
    public class ObjectIdManager: IObjectIdManager, IEnumerable
    {
        // Underlying storage: maps objects to identifiers.
        private Hashtable ObjectToIdent;

        // Underlying storage: maps identifiers to objects.
        private Hashtable IdentToObject;

        // Minimum and maximum identifiers.
        private uint minId = 0;
        private uint maxId = UInt32.MaxValue;

        // Minimum value for object IDs
        public uint MinimumId {
            get {
                return minId;
            }
        }

        // Maximum value for obejct IDs
        public uint MaximumId {
            get {
                return maxId;
            }
        }

        // Sets the range for idents, including min and max.
        // This only affects new objects, it won't move old objects.
        public void SetIdRange(uint min, uint max)
        {
            if (max < min) {
                throw new Exception("max must be smaller that min");
            }
            minId = min;
            maxId = max;
            CurrentId = Math.Max(min, Math.Min(max, CurrentId));
        }

        // Current id. This increases and then wraps around.
        // Ids always increase, so that an ID is not reused, except possibly MUCH MUCH later.
        private uint CurrentId = 0;

        // Default constructor.
        public ObjectIdManager()
        {
            ObjectToIdent = new Hashtable();
            IdentToObject = new Hashtable();
        }

        // Returns the number of elements in the collection.
        public int Count {
            get {
                return IdentToObject.Count;
            }
        }

        // Returns true if we have an object with the specified ident.
        public bool HasIdent(uint ident)
        {
            return IdentToObject.Contains(ident);
        }

        // Returns true if we have the specified object.
        public bool HasObject(object obj)
        {
            if (obj == null) {
                return false;
            }
            return ObjectToIdent.Contains(obj);
        }

        // Gets the ident for an object.
        public uint GetIdent(object obj)
        {
            if (!HasObject(obj)) {
                throw new Exception("Object doesn't have identifier");
            }
            return (uint)ObjectToIdent[obj];
        }

        // Gets the object for an ident.
        public object GetObject(uint ident)
        {
            if (!HasIdent(ident)) {
                throw new Exception("Invalid identifier");
            }
            return (object)IdentToObject[ident];
        }

        // Adds a new object to be managed.
        // obj can't be null.
        public void AddObject(object obj)
        {
            uint startId = CurrentId;
            if (obj == null) {
                throw new ArgumentNullException("obj");
            }
            while (HasIdent(CurrentId)) {
                if (CurrentId == MaximumId) {
                    CurrentId = MinimumId;
                } else {
                    ++CurrentId;
                }
                // Wait, we've been here before
                // This means we ran out of idents
                if (CurrentId == startId) {
                    throw new Exception("Ran out of valid identifiers");
                }
            }
            ObjectToIdent.Add(obj, CurrentId);
            IdentToObject.Add(CurrentId, obj);
        }

        // Removes an object. No effect if HasObject(obj) == false.
        public void RemObject(object obj)
        {
            if (HasObject(obj)) {
                IdentToObject.Remove(GetIdent(obj));
                ObjectToIdent.Remove(obj);
            }
        }

        // This will enumerate over the objects in the collection.
        public IEnumerator GetEnumerator()
        {
            return new DictionaryEnumeratorWrapper(IdentToObject.GetEnumerator());
        }

        // Calls GetEnumerator();
        IEnumerator IEnumerable.GetEnumerator()
        {
            return GetEnumerator();
        }
    }

    // A simple enumerable collection.
    // Implemented on top of a hashtable.
    //
    // This is a very simple way to store a bunch of objects in order is not important
    // It's smaller and easier to use than a hashtable.
    // In particular, you don't enumerate DictionaryEntry structs.
    //
    // Implemention ICollection is useless.
    // ICollection from 1.0 is a worthless interface.
    public class Collection: IEnumerable
    {
        // Backing store.
        private Hashtable hash;

        // Create a new Collection
        public Collection()
        {
            hash = new Hashtable();
        }

        // Returns the number of items in the collection.
        public int Count {
            get {
                return hash.Count;
            }
        }

        // Returns true if the specified object is in the bag.
        public bool Contains(object obj)
        {
            return hash.Contains(obj);
        }

        // Add an object to the bag.
        public void Add(object obj)
        {
            hash.Add(obj, obj);
        }

        // Remove an object from the bag.
        public void Remove(object obj)
        {
            hash.Remove(obj);
        }

        // Create an array that contains the elements of this collection;
        public object[] CloneToArray()
        {
            object[] array = new object[Count];
            CopyTo(array, 0);
            return array;
        }

        // Copy elements to an array.
        public void CopyTo(System.Array a, int index)
        {
            int k = 0;
            foreach (object obj in this) {
                a.SetValue(obj, index + k);
                ++k;
            }
        }

        // Cvlear the collection.
        public void Clear()
        {
            hash.Clear();
        }

        // Get an enumerator over the elements of the collection.
        public IEnumerator GetEnumerator()
        {
            return new DictionaryEnumeratorWrapper(hash.GetEnumerator());
        }
    }

    // This is an enumerable collection that ensures a certain order.
    // That order is not specified and doesn't matter; however that
    // order is guaranteed to be the same if the operations are the same.
    //
    // You can't say that about a Hashtable
    public class SyncronizedOrderCollection: IEnumerable
    {
        // Linked list node.
        private class ListNode
        {
            // Actual object this refers to.
            public readonly object Value;

            // Previous node in the list.
            public ListNode Prev;

            // Next node in the list.
            public ListNode Next;

            // Constructor.
            public ListNode(object obj)
            {
                Value = obj;
            }
        }

        private class ListEnumerator: IEnumerator
        {
            // Current node.
            private ListNode Node;

            // Reference to the collection.
            // Required for the Reset operation.
            private readonly SyncronizedOrderCollection Collection;

            // Constructor.
            public ListEnumerator(SyncronizedOrderCollection col)
            {
                Collection = col;
                Node = Collection.ListRoot;
            }

            // Reset to before the first position;
            public void Reset()
            {
                Node = Collection.ListRoot;
            }

            // Return the current object.
            public object Current {
                get {
                    if (Node.Value == null) {
                        throw new InvalidOperationException();
                    }
                    return Node.Value;
                }
            }

            // Advance the iterator.
            public bool MoveNext()
            {
                if (Node == null) {
                    return false;
                }
                Node = Node.Next;
                if (Node.Value == null) {
                    Node = null;
                    return false;
                }
                return true;
            }
        }

        // Maps objects to ListNodes
        private Hashtable Hash;

        // Some node from the list.
        // Can be null.
        private ListNode ListRoot;

        // Constructor.
        public SyncronizedOrderCollection()
        {
            Hash = new Hashtable();
            ListRoot = new ListNode(null);
            ListRoot.Next = ListRoot.Prev = ListRoot;
        }

        // Returns the number of items in the collection.
        public int Count {
            get {
                return Hash.Count;
            }
        }

        // Returns true if the specified object is inside
        public bool Contains(object obj)
        {
            return Hash.Contains(obj);
        }

        // Add an object to the bag.
        public void Add(object obj)
        {
            if (obj == null) {
                throw new ArgumentNullException("obj");
            }
            if (!Contains(obj)) {
                ListNode node = new ListNode(obj);
                node.Next = ListRoot;
                node.Prev = ListRoot.Prev;
                ListRoot.Prev.Next = node;
                ListRoot.Prev = node;
                Hash.Add(obj, node);
            }
        }

        // Remove an object from the bag.
        public void Remove(object obj)
        {
            if (!Contains(obj)) {
                return;
            }
            ListNode node = (ListNode)Hash[obj];
            node.Prev.Next = node.Next;
            node.Next.Prev = node.Prev;
            Hash.Remove(obj);
        }

        // Create an array that contains the elements of this collection;
        public object[] CloneToArray()
        {
            object[] array = new object[Count];
            CopyTo(array, 0);
            return array;
        }

        // Copy elements to an array.
        public void CopyTo(System.Array a, int index)
        {
            int k = 0;
            foreach (object obj in this) {
                a.SetValue(obj, index + k);
                ++k;
            }
        }

        // Cvlear the collection.
        public void Clear()
        {
            Hash.Clear();
        }

        // Get an enumerator over the elements of the collection.
        public IEnumerator GetEnumerator()
        {
            return new ListEnumerator(this);
        }
    }

    // This class wraps an IDictionaryEnumerator and shows only the value
    // from the key/value pair.
    //
    // IDictionary (implemented by Hashtable) is IEnumerable, but the
    // enumerated object is a DictionaryEntry, with two elements, key and value.
    //
    // So, if you wanted to enumerate over all the value object in the dictionary
    // you would have to do:
    // for (DictionaryEntry e in SomeCollection) {
    //     SomeObjType val = (ObjType)e.Value;
    //     // Continue with val
    // }
    //
    // This class is an IEnumerator that wraps around an IDictionaryEnumerator
    // and returns IDictionaryEnumerator.Value for IEnumerator.Current.
    public class DictionaryEnumeratorWrapper: IEnumerator
    {
        // Underlying enumerator.
        private IDictionaryEnumerator dictEnum;

        // Create a new enumerator on top of the IDictionaryEnumerator.
        public DictionaryEnumeratorWrapper(IDictionaryEnumerator dictEnum)
        {
            this.dictEnum = dictEnum;
        }

        // Move to the next element.
        bool IEnumerator.MoveNext()
        {
            return dictEnum.MoveNext();
        }

        // Reset to the first element.
        void IEnumerator.Reset()
        {
            dictEnum.Reset();
        }

        // Get the current object.
        // This returns IDictionaryEnumerator.Value
        object IEnumerator.Current {
            get {
                return dictEnum.Value;
            }
        }
    }
}
