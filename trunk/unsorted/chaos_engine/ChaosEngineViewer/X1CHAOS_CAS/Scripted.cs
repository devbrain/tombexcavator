using System;
using System.Collections.Generic;
using System.Text;
using System.Drawing;
using ChaosEngineViewer.LEVSDAT_BIN;

namespace ChaosEngineViewer.X1CHAOS_CAS {
  public class Scripted {
    private VersionEnum _version;
    public int _address;

    private int _00w_x;
    private int _02w_y;
    // 4-7 4 bytes
    public int health;
    private int templateIndex;
    public int scriptClassIndex;
    public int spawnedEntityClassIndex;
    public int linkId16_w;
    //public int linkId17_b;
    public int linkId18_w;
    public Template backedUpTemplate;
    public int unk38;
    public int addressUnk40;
    public int addressUnk44;
    public int offset48;
    public int offset50;
    public int offset52;
    public int linkId54_w;
    public int linkId56_w;
    public int repeatCount;
    public int delayBetweenRepeat;
    public int currentDelayBetweenRepeat;

    //public int templateIndex;
    //public int initialState;

    public string description;
    //public static Dictionary<int, int> _scriptsSpawnMonster = new Dictionary<int, bool> ();
    private int currentScriptIndex;
    private int currentInstructionIndex;
    private int templateIndexOld;

    public Scripted (VersionEnum version, Dump dump, int address) {
      _version = version;
      _address = address;

      _00w_x = dump.ReadWord (address);
      address += 2;
      _02w_y = dump.ReadWord (address);
      address += 2;
      address += 4;
      health = dump.ReadWord (address);
      address += 2;
      templateIndex = dump.ReadWord (address);
      address += 2;
      scriptClassIndex = dump.ReadWord (address);
      address += 2;
      spawnedEntityClassIndex = dump.ReadWord (address);
      address += 2;
      linkId16_w = dump.ReadWord (address);
      if (linkId16_w > 255) {
        throw new Exception ();
      }
      address += 2;
      //linkId17_b = dump.ReadByte (address);
      //address += 1;
      linkId18_w = dump.ReadWord (address);
      address += 2;
      backedUpTemplate = new Template (dump, address);
      address += 18;
      unk38 = dump.ReadWord (address);
      address += 2;
      addressUnk40 = dump.ReadLong (address);
      address += 4;
      addressUnk44 = dump.ReadLong (address);
      address += 4;
      offset48 = dump.ReadWord (address);
      address += 2;
      offset50 = dump.ReadWord (address);
      address += 2;
      offset52 = dump.ReadWord (address);
      address += 2;
      linkId54_w = dump.ReadWord (address);
      //if (linkId54_w != 0) {
      //  int j = 10;
      //}
      address += 2;
      linkId56_w = dump.ReadWordSigned (address);
      address += 2;
      repeatCount = dump.ReadByteSigned (address);
      address += 1;
      delayBetweenRepeat = dump.ReadByte (address);
      address += 1;
      currentDelayBetweenRepeat = dump.ReadWord (address);
      address += 2;

      SetCurrentScript (1);
      templateIndex = -1;
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

    public int TemplateIndex {
      get {
        return (templateIndex);
      }
      set {
        if (templateIndex != 0) {
          templateIndexOld = templateIndex;
        }
        templateIndex = value;
      }
    }

    public int TemplateIndexOld {
      get {
        return (templateIndexOld);
      }
    }

    public void SetCurrentScript (int scriptIndex) {
      currentScriptIndex = scriptIndex;
      RewindScript ();
    }

    public Script GetScript (int scriptIndex) {
      return (LevsdatBin.block16_ScriptsClasses [(int) _version].elements16 [scriptClassIndex].GetScript (scriptIndex));
    }

    public Script CurrentScript {
      get {
        return (GetScript (currentScriptIndex));
      }
    }

    public int CurrentScriptIndex {
      get {
        return (currentScriptIndex);
      }
    }

    public Instruction CurrentInstruction {
      get {
        return (CurrentScript._instructions [currentInstructionIndex]);
      }
    }

    public void GoNextInstruction () {
      currentInstructionIndex++;
    }

    public void RewindScript () {
      currentInstructionIndex = 0;
    }

    public int GetCurrentInstructionUniqueKey () {
      return ((CurrentScriptIndex << 16) | (currentInstructionIndex));
    }

    public string Description {
      get {
        StringBuilder sb = new StringBuilder ();
        sb.AppendLine (string.Format ("***** SCRIPTED *****"));
        sb.AppendLine (string.Format ("address: {0:X6} ({1:X6})", _address, 0xB9C22 + _address));
        sb.AppendLine (string.Format ("x: {0}", _00w_x));
        sb.AppendLine (string.Format ("y: {0}", _02w_y));
        sb.AppendLine (string.Format ("health: {0}", health));
        sb.AppendLine (string.Format ("scriptClassIndex: {0}", scriptClassIndex));
        sb.AppendLine (string.Format ("spawnedEntityClassIndex: {0}", spawnedEntityClassIndex));
        sb.AppendLine (string.Format ("unk16: {0}", linkId16_w));
        //sb.AppendLine (string.Format ("linkId17_b (Instruction 18): {0}", linkId17_b));
        sb.AppendLine (string.Format ("linkId18_b: {0}", linkId18_w));
        sb.AppendLine (string.Format ("linkId54_w (Instruction 0E): {0}", linkId54_w));
        sb.AppendLine (string.Format ("linkId56_b or time: {0}", linkId56_w));
        sb.AppendLine (string.Format ("repeatCount: {0}", repeatCount));
        sb.AppendLine (string.Format ("delayBetweenRepeat: {0}", delayBetweenRepeat));
        
        sb.AppendLine (string.Format ("templateIndex: {0}", templateIndex));
        sb.AppendLine (string.Format ("Script #1 address: 0x{0:X4}", addressUnk44));
        sb.AppendLine (string.Format ("Script #2 address: 0x{0:X4}", addressUnk44 + offset48));
        sb.AppendLine (string.Format ("Script #3 address: 0x{0:X4}", addressUnk44 + offset50));
        sb.AppendLine (string.Format ("Script #4 address: 0x{0:X4}", addressUnk44 + offset52));
        return (sb.ToString ());
      }
    }
  }
}
