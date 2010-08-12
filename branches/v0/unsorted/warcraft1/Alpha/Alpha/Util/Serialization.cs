// Copyright (C) 2005 Crestez Leonard    cleonard@go.ro
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
using System.IO;
using System.Reflection;
using System.Runtime.Serialization;
using System.Runtime.Serialization.Formatters;
using System.Runtime.Serialization.Formatters.Binary;

namespace Alpha.Util
{
    // Serialization helpers.
    public sealed class Serialization
    {
        // Saves an object to a stream. Objects with identifiers in idManager won't be saved.
        // The idManager is part of the Streaming context.
        public static void SaveObject(Object obj, IObjectIdManager idManager, Stream stream)
        {
            BinaryFormatter formatter;
            formatter = new BinaryFormatter();
            formatter.TypeFormat = FormatterTypeStyle.TypesAlways;
            formatter.AssemblyFormat = FormatterAssemblyStyle.Simple;
            formatter.Context = new StreamingContext(StreamingContextStates.All, idManager);
            formatter.SurrogateSelector = new SerializationHelper();
            formatter.Serialize(stream, obj);
        }

        // Loads an object from a stream. IdManager should be the same it was on serialization.
        public static object LoadObject(IObjectIdManager idManager, Stream stream)
        {
            BinaryFormatter formatter;
            formatter = new BinaryFormatter();
            formatter.TypeFormat = FormatterTypeStyle.TypesAlways;
            formatter.AssemblyFormat = FormatterAssemblyStyle.Simple;
            formatter.Context = new StreamingContext(StreamingContextStates.All, idManager);
            return formatter.Deserialize(stream);
        }

        // This is a serialization helper, it serves as both a surrogate selection and the
        // actual surrogate.
        // 
        // It return itself as the surrogate for everything.
        // It will save objects in the IdManager (the context has to be an IdManager) as IdentReferences,
        // and won't do anything to the rest.
        //
        // Read more on GetObjectData.
        //
        // This is only for writing, reading is not supported. You should load with no surrogation,
        // the system will create IdentReferences and resolve them by itself.
        // See IdentReference and the docs on IObjectReference.
        private class SerializationHelper: ISurrogateSelector, ISerializationSurrogate
        {
            private ISurrogateSelector next;

            public void ChainSelector(ISurrogateSelector selector)
            {
                next = selector;
            }

            public ISurrogateSelector GetNextSelector()
            {
                return next;
            }

            public ISerializationSurrogate GetSurrogate(Type t, StreamingContext context, out ISurrogateSelector selector)
            {
                selector = GetNextSelector();
                if (t.IsClass) {
                    //Console.WriteLine("Providing a surrogate for {0}", t.FullName);
                    selector = null;
                    return this;
                } else {
                    return null;
                }
            }

            // Serialize an object.
            // This serializes objects in the idManager as IdentReferences, and doesn't do
            // anything special for the rest.
            //
            // .net serialization does some fancy stuff with codedom to improve speed, and since we
            // override serialization for everything, it might turn out to be slower.
            public void GetObjectData(object obj, SerializationInfo sinfo, StreamingContext context)
            {
                IObjectIdManager idManager = (IObjectIdManager)context.Context;

                if (idManager.HasObject(obj)) {
                    sinfo.SetType(typeof(IdentReference));
                    sinfo.AddValue("Ident", idManager.GetIdent(obj));
                    //Console.WriteLine("Replaced {0} with ident", obj);
                } else if (obj is ISerializable) {
                    ((ISerializable)obj).GetObjectData(sinfo, context);
                    //Console.WriteLine("{0} implements ISerializable, serialized itself", obj);
                } else {
                    // Serialize the object by hand. .net could have done it faster, but of well.
                    MemberInfo[] members = FormatterServices.GetSerializableMembers(obj.GetType());
                    foreach (MemberInfo minfo in members) {
                        FieldInfo finfo = (FieldInfo)minfo;
                        sinfo.AddValue(finfo.Name, finfo.GetValue(obj), finfo.FieldType);
                    }
                    //Console.WriteLine("Serialized {0} by hand", obj);
                }
            }

            // Not available, should never be called.
            public object SetObjectData(object obj, SerializationInfo sinfo, StreamingContext context, ISurrogateSelector sel)
            {
                throw new NotImplementedException();
            }
        }

        // When serializing object with identifiers, they are replaced(via surrogates)
        // With an instance of this class. An instance of this is created on deserialization,
        // but before deserialization returns all references to this in the object graph are
        // replaced with the result of GetRealObject.
        //
        // This class is internal to the serialization code. It was made public to prevent
        // compilers whining about unused classes and the like.
        [Serializable]
        public class IdentReference: IObjectReference
        {
            // Identifier for the object this references
            public uint Ident;

            public object GetRealObject(StreamingContext context)
            {
                //Console.WriteLine("Asking for real object with id {0}", Ident);
                IObjectIdManager idManager = (IObjectIdManager)context.Context;

                return idManager.GetObject(Ident);
            }
        }
    }
}
