using System;
using System.Collections.Generic;
using System.Text;
using ChaosEngineViewer.X1CHAOS_CAS;

namespace ChaosEngineViewer.LEVSDAT_BIN {
  public class Script {
    public List<Instruction> _instructions;

    public Script (Dump dump, int address) {
      _instructions = CreateScript (dump, address);
    }

    private List<Instruction> CreateScript (Dump dump, int address) {
      List<Instruction> instructions = new List<Instruction> ();
      for (; ; ) {
        int instructionIndex = dump.ReadByte (address++);
        InstructionType instructionType = InstructionType.Get (instructionIndex);
        int? param = null;
        if (instructionType._param) {
          param = dump.ReadByte (address++);
        }
        Instruction instruction = new Instruction (instructionType, param);
        instructions.Add (instruction);
        if (instructionType._eos) {
          break;
        }
      }
      return (instructions);
    }

    public Instruction GetFirstSetTemplateInstruction () {
      foreach (Instruction instruction in _instructions) {
        if (instruction._type._index == 0x08) {
          return (instruction);
        }
        if (instruction._type._index == 0x10) {
          return (instruction);
        }
      }
      return (null);
    }

    public bool HasSpawnEntity () {
      foreach (Instruction instruction in _instructions) {
        if (instruction._type._index == 0x12) {
          return (true);
        }
        if (instruction._type._index == 0x02) {
          return (true);
        }
      }
      return (false);
    }

    public bool HasIfDelay () {
      foreach (Instruction instruction in _instructions) {
        if (instruction._type._index == 0x0C) {
          return (true);
        }
      }
      return (false);
    }

    public string Dump () {
      StringBuilder sb1 = new StringBuilder ();
      StringBuilder sb2 = new StringBuilder ();
      foreach (Instruction instruction in _instructions) {
        sb1.Append (string.Format ("{0:X2}", instruction._type._index));
        sb2.Append (string.Format ("{0}", instruction._type._name));
        if (instruction._param != null) {
          sb1.Append (string.Format (" {0:X2}", instruction._param));
          sb2.Append (string.Format (" ({0})", instruction._param));
        }
        if (instruction != _instructions [_instructions.Count - 1]) {
          sb1.Append (string.Format (", "));
          sb2.Append (string.Format (", "));
        }
      }
      //sb1.Remove (sb1.Length - 1, 1);
      //sb2.Remove (sb2.Length - 1, 1);
      sb1.AppendLine ();
      sb1.Append (sb2.ToString ());
      return (sb1.ToString ());
    }

    public string Dump (Scripted scripted) {
      StringBuilder sb1 = new StringBuilder ();
      StringBuilder sb2 = new StringBuilder ();
      foreach (Instruction instruction in _instructions) {
        sb1.Append (instruction.DumpHex ());
        sb2.Append (instruction.Dump (scripted));
        if (instruction != _instructions [_instructions.Count - 1]) {
          sb1.Append (string.Format (", "));
          sb2.Append (string.Format (", "));
        }
      }
      //sb1.Remove (sb1.Length - 1, 1);
      //sb2.Remove (sb2.Length - 1, 1);
      sb1.AppendLine ();
      sb1.Append (sb2.ToString ());
      return (sb1.ToString ());
    }
  }

  public class InstructionType {
    public int _index;
    public bool _param;
    public bool _stop;
    public bool _eos;
    public string _name;

    private static Dictionary<int, InstructionType> _instructionTypes;

    public InstructionType (int index, bool param, bool stop, bool eos, string name) {
      _index = index;
      _param = param;
      _stop = stop;
      _eos = eos;
      _name = name;
    }

    public static InstructionType Get (int index) {
      if (_instructionTypes == null) {
        _instructionTypes = new Dictionary<int, InstructionType> ();
        _instructionTypes.Add (0x00, new InstructionType (0x00, true, true, false, "SetScriptState"));
        _instructionTypes.Add (0x02, new InstructionType (0x02, false, false, false, "Spawn_If_RepeatCount=0_Then_SetActivatingEvent0_&_ExecuteScript3"));
        _instructionTypes.Add (0x04, new InstructionType (0x04, false, false, true, "Rewind"));
        _instructionTypes.Add (0x06, new InstructionType (0x06, false, true, true, "RemoveScriptedFromList"));
        _instructionTypes.Add (0x08, new InstructionType (0x08, true, true, false, "SetTemplate1"));
        _instructionTypes.Add (0x0A, new InstructionType (0x0A, false, false, true, "SetDelayBetweenRepeat&ExecuteScript1"));
        _instructionTypes.Add (0x0C, new InstructionType (0x0C, false, true, false, "If_Delay<0_&_RepeatCount>0orFF_Then_DecRepeatCount(!=FF)_&_ExecuteScript2"));
        _instructionTypes.Add (0x0E, new InstructionType (0x0E, false, false, false, "TriggerLinkId54"));
        _instructionTypes.Add (0x10, new InstructionType (0x10, true, true, false, "SetTemplate2"));
        _instructionTypes.Add (0x12, new InstructionType (0x12, false, false, false, "DoExplosion2"));
        _instructionTypes.Add (0x14, new InstructionType (0x14, true, false, false, "DoExplosion1"));
        _instructionTypes.Add (0x16, new InstructionType (0x16, true, false, false, "PlaySoundUnk"));
        _instructionTypes.Add (0x18, new InstructionType (0x18, false, false, false, "Destroy1Node?"));
      }
      return (_instructionTypes [index]);
    }
  }

  public class Instruction {
    public InstructionType _type;
    public int? _param;

    public Instruction (InstructionType type, int? param) {
      _type = type;
      _param = param;
    }

    public string DumpHex () {
      StringBuilder sb = new StringBuilder ();
      sb.Append (string.Format ("{0:X2}", _type._index));
      if (_param != null) {
        sb.Append (string.Format (" {0:X2}", _param));
      }
      return (sb.ToString ());
    }

    public string Dump (Scripted scripted) {
      StringBuilder sb = new StringBuilder ();
      sb.Append (string.Format ("{0}", _type._name));
      if (_type._index == 0x0A) {
        sb.Append (string.Format (" <{0}>", scripted.delayBetweenRepeat));
      }
      if (_type._index == 0x0E) {
        sb.Append (string.Format (" <{0}>", scripted.linkId54_w));
      }
      if (_param != null) {
        sb.Append (string.Format (" ({0})", _param));
      }
      return (sb.ToString ());
    }
  }
}
