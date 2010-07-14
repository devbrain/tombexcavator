using System;
using System.Collections.Generic;
using System.Text;

namespace ChaosEngineViewer.LEVSDAT_BIN {
  public class Element16_ScriptClass {
    private int _index;

    public int tempA1;
    public int tempA2;
    public int tempB1;
    public int tempB2;
    public int tempC1;
    public int tempC2;
    public int tempD1;
    public int tempD2;

    private Script [] scripts;

    private static Dictionary<int, int> _defaultTemplateIndex;
    private static Dictionary<int, int> _activatedTemplateIndex;

    static Element16_ScriptClass () {
      //if (_defaultTemplateIndex == null) {
      _defaultTemplateIndex = new Dictionary<int, int> ();
      _activatedTemplateIndex = new Dictionary<int, int> ();
      _defaultTemplateIndex.Add (12, 0x21);
      _activatedTemplateIndex.Add (12, 0x20);
      _defaultTemplateIndex.Add (14, 0x2A);
      _activatedTemplateIndex.Add (14, 0x29);
      //}
    }

    public Element16_ScriptClass (Dump dump, int index, int addressBlock16, int offsetElement16_A, int offsetElement16_B, int offsetElement16_C, int offsetElement16_D) {
      _index = index;

      tempA1 = dump.ReadByte (addressBlock16 + offsetElement16_A);
      tempA2 = dump.ReadByteSigned (addressBlock16 + offsetElement16_A + 1);
      tempB1 = dump.ReadByte (addressBlock16 + offsetElement16_B);
      tempB2 = dump.ReadByteSigned (addressBlock16 + offsetElement16_B + 1);
      tempC1 = dump.ReadByte (addressBlock16 + offsetElement16_C);
      if (tempC1 == 0x08) {
        tempC2 = dump.ReadByteSigned (addressBlock16 + offsetElement16_C + 1);
      }
      tempD1 = dump.ReadByte (addressBlock16 + offsetElement16_D);
      tempD2 = dump.ReadByteSigned (addressBlock16 + offsetElement16_D + 1);

      scripts = new Script [4];
      scripts [0] = new Script (dump, addressBlock16 + offsetElement16_A);
      scripts [1] = new Script (dump, addressBlock16 + offsetElement16_B);
      scripts [2] = new Script (dump, addressBlock16 + offsetElement16_D);
      scripts [3] = new Script (dump, addressBlock16 + offsetElement16_C);
    }

    public Script GetScript (int scriptIndex) {
      return (scripts [scriptIndex - 1]);
    }

    public int? GetDefaultTemplateIndex () {
      if (!_defaultTemplateIndex.ContainsKey (_index)) {
        return (null);
      }
      return (_defaultTemplateIndex [_index]);
    }

    public int? GetActivatedTemplateIndex () {
      if (!_activatedTemplateIndex.ContainsKey (_index)) {
        return (null);
      }
      return (_activatedTemplateIndex [_index]);
    }
  }
}
