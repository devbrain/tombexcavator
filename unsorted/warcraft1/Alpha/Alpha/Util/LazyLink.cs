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
    // Simple interface for a lazy link.
    //
    // Just call .Get to get to your object; see LazyLink for details.
    public interface ILazyLink
    {
        // Get the object it links to.
        object Get();
    }

    // This is a simple class to implement Lazy loading.
    //
    // It's a way to hold a reference to an object, but only create it
    // when it's actually required, speeding up initialization.
    //
    // It can also be made "weak" (default is strong however).
    // If it's weak, then if you don't hold an external reference to the object
    // it will be garbage collected and a new one will be build next time you call Get.
    //
    // Keep in mind the following:
    // * Linked objects should be read-only
    // * The result from Create() is cached, so Create() shouldn't depend
    // on anything outside LazyLink.
    // * From the above, LazyLink should be readonly.
    //
    // No, it doesn't make sense for LazyLink to be struct.
    // Really, it doesn't.
    public abstract class LazyLink: ILazyLink
    {
        // Weak reference to the object.
        private WeakReference WeakRef;

        // Strong reference to the object.
        private object StrongRef;

        // If the reference is weak.
        private bool Weak;

        // Get the object refered to by this LazyLink.
        public object Get()
        {
            if (!Weak) {
                if (StrongRef == null) {
                    StrongRef = Create();
                }
                return StrongRef;
            } else {
                // Can this blow up? Let's hope it doesn't do any fancy optimizations.
                object obj = null;
                if (WeakRef != null) {
                    obj = WeakRef.Target;
                }
                if (obj == null) {
                    obj = Create();
                    WeakRef = new WeakReference(obj);
                }
                return obj;
            }
        }

        // Lose the reference to the object, even if it's strong.
        // Has no effect on Weak LazyLinks.
        public void ForceUnload()
        {
            StrongRef = null;
        }

        // Create the object. This is only called when needed, that is, the object from the
        // previous call to Create was destroyed.
        protected abstract object Create();

        // Create a new lazy link.
        public LazyLink(bool weak)
        {
            Weak = weak;
        }

        // Create a new lazy link, by default a strong one.
        public LazyLink(): this(false)
        {
        }
    }
  
    // Trivial ILazyLink implementation, just links to an already loaded object.
    public class DirectLink: ILazyLink
    {
        // The object this link refers to.
        public readonly object Object;
        
        // Just returns object;
        public object Get()
        {
            return Object;
        }

        // Create a new strong link to an existing object.
        public DirectLink(object obj)
        {
            Object = obj;
        }
    }

    // This creates a strong (non-weak) link to another LazyLink
    public class StrongLink: LazyLink
    {
        // Link reference.
        public readonly ILazyLink Link;

        // Get the object from Link.
        protected override object Create()
        {
            return Link.Get();
        }

        // Create a new non-weak link to another link.
        public StrongLink(ILazyLink link): base(false)
        {
            Link = link;
        }
    }

    // Link to an element of an array.
    // Yes, weak is is meaningfull
    public class ArrayElementLink: LazyLink
    {
        // Link to the array.
        public readonly ILazyLink ArrayLink;

        // Index in the array.
        public readonly int Index;

        // This will LazyLink.Get the array and return a certain element.
        protected override object Create()
        {
            return (ArrayLink.Get() as object[])[Index];
        }

        // Constructor, parameters are obvious.
        public ArrayElementLink(bool weak, ILazyLink array, int index): base(weak)
        {
            ArrayLink = array;
            Index = index;
        }
    }
}

