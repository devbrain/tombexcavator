using System;
using System.Reflection;
using System.Collections;

namespace ChaosEngineViewer {
  /// <summary>
  /// <b>BaseObject</b> class is an abstract class for you to derive from. <br>
  /// Every class that will be dirived from this class will support the <b>Clone</b> method automaticly.<br>
  /// The class implements the interface <i>ICloneable</i> and there for every object that will be derived <br>
  /// from this object will support the <i>ICloneable</i> interface as well.
  /// </summary>
  public static class Cloner<T> {
    /// <summary>
    /// Clone the object, and returning a reference to a cloned object.
    /// </summary>
    /// <returns>Reference to the new cloned object.</returns>
    public static T Clone (T src) {
      T dst = (T) Activator.CreateInstance (src.GetType ());

      BindingFlags bf = BindingFlags.Instance | BindingFlags.NonPublic | BindingFlags.Public | BindingFlags.Static;

      FieldInfo [] srcfields = src.GetType ().GetFields (bf);
      FieldInfo [] dstfields = dst.GetType ().GetFields (bf);

      foreach (FieldInfo srcField in srcfields) {
        FieldInfo dstField = null;
        foreach (FieldInfo field in dstfields) {
          if (field.Name == srcField.Name) {
            dstField = field;
            break;
          }
        }
        if (dstField == null) {
          throw new Exception ();
        }



        //We query if the fiels support the ICloneable interface.
        Type ICloneType = srcField.FieldType.GetInterface ("ICloneable", true);

        if (ICloneType != null) {
          //Getting the ICloneable interface from the object.
          ICloneable IClone = (ICloneable) srcField.GetValue (src);

          //We use the clone method to set the new value to the field.
          dstField.SetValue (dst, IClone.Clone ());
        }
        else {
          //If the field doesn't support the ICloneable interface then just set it.
          dstField.SetValue (dst, srcField.GetValue (src));
        }

        //Now we check if the object support the IEnumerable interface, so if it does
        //we need to enumerate all its items and check if they support the ICloneable interface.
        Type IEnumerableType = srcField.FieldType.GetInterface ("IEnumerable", true);
        if (IEnumerableType != null) {
          //Get the IEnumerable interface from the field.
          IEnumerable IEnum = (IEnumerable) srcField.GetValue (src);

          //This version support the IList and the IDictionary interfaces to iterate
          //on collections.
          Type IListType = dstField.FieldType.GetInterface ("IList", true);
          Type IDicType = dstField.FieldType.GetInterface ("IDictionary", true);

          int j = 0;
          if (IListType != null) {
            //Getting the IList interface.
            IList list = (IList) dstField.GetValue (dst);

            foreach (object obj in IEnum) {
              //Checking to see if the current item support the ICloneable interface.
              ICloneType = obj.GetType ().GetInterface ("ICloneable", true);

              if (ICloneType != null) {
                //If it does support the ICloneable interface, we use it to set the clone of
                //the object in the list.
                ICloneable clone = (ICloneable) obj;

                list [j] = clone.Clone ();
              }

              //NOTE: If the item in the list is not support the ICloneable interface then
              // in the cloned list this item will be the same item as in the original list
              //(as long as this type is a reference type).

              j++;
            }
          }
          else if (IDicType != null) {
            //Getting the dictionary interface.
            IDictionary dic = (IDictionary) dstField.GetValue (dst);
            j = 0;
            foreach (DictionaryEntry de in IEnum) {
              //Checking to see if the item support the ICloneable interface.
              ICloneType = de.Value.GetType ().GetInterface ("ICloneable", true);

              if (ICloneType != null) {
                ICloneable clone = (ICloneable) de.Value;

                dic [de.Key] = clone.Clone ();
              }
              j++;
            }
          }
        }
      }
      return (dst);
    }
  }
}