using System;
using System.Collections.Generic;
using System.Text;
using System.Drawing;

namespace ChaosEngineViewer.X1CHAOS_CAS {
  public class Trigger {
    private int _address;

    private int _00w_x;
    private int _02w_y;
    private int _04w_linkId;
    private int _06w_conditionToTrigger;

    public string description;

    public Trigger (Dump dump, int address) {
      _address = address;

      _00w_x = dump.ReadWord (address);
      address += 2;
      _02w_y = dump.ReadWord (address);
      address += 2;
      _04w_linkId = dump.ReadWord (address);
      address += 2;
      _06w_conditionToTrigger = dump.ReadWord (address);
      address += 2;

      StringBuilder sb = new StringBuilder ();
      sb.AppendLine (string.Format ("***** TRIGGER *****"));
      sb.AppendLine (string.Format ("Address: {0:X6}", 0xB9C22 + _address));
      sb.AppendLine (string.Format ("X: {0}", X));
      sb.AppendLine (string.Format ("Y: {0}", Y));
      //sb.AppendLine (string.Format ("_rawUnk04: 0x{0:X2}", _rawUnk04));
      sb.AppendLine (string.Format ("LinkId04w: {0}", LinkId04w));
      //sb.AppendLine (string.Format ("unk01A: {0}", unk01A));
      //sb.AppendLine (string.Format ("unk01B: {0}", unk01B));
      sb.AppendLine (string.Format ("ConditionToTrigger: 0x{0:X4}", ConditionToTrigger));
      sb.AppendLine (string.Format ("({0})", ConditionToTriggerString));
      description = sb.ToString ();
    }

    public int X {
      get {
        return (_00w_x);
      }
    }

    public int Y {
      get {
        return (_02w_y);
      }
    }

    public int LinkId04w {
      get {
        return (_04w_linkId);
      }
    }

    public int ConditionToTrigger {
      get {
        return (_06w_conditionToTrigger);
      }
    }

    public int ConditionToTriggerType {
      get {
        if (_06w_conditionToTrigger == 0) {
          return (0);
        }
        else if ((_06w_conditionToTrigger >= 1) && (_06w_conditionToTrigger <= 8)) {
          return (1);
        }
        else if ((_06w_conditionToTrigger >= 9) && (_06w_conditionToTrigger <= 16)) {
          return (2);
        }
        return (3);
      }
    }

    public string ConditionToTriggerString {
      get {
        int type = ConditionToTriggerType;
        if (type == 0) {
          return ("Always");
        }
        else if (type == 1) {
          return (string.Format ("Spawn location #{0} must have been used at least 1 time", Param));
        }
        else if (type == 2) {
          return (string.Format ("Spawn location #1 must have been used exactly {0} times", Param));
        }
        else {
          throw new Exception ();
        }
      }
    }

    public int Param {
      get {
        int type = ConditionToTriggerType;
        if (type == 1) {
          return (_06w_conditionToTrigger - 1);
        }
        else if (type == 2) {
          return (_06w_conditionToTrigger - 9);
        }
        else {
          throw new Exception ();
        }
      }
    }
  }
}
