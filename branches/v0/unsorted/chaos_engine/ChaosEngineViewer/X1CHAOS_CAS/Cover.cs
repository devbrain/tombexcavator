using System;
using System.Collections.Generic;
using System.Text;
using System.Drawing;
using ChaosEngineViewer.LEVSDAT_BIN;

namespace ChaosEngineViewer.X1CHAOS_CAS {
  public class Cover {
    private int _address;

    private int _00w_x;
    private int _02w_y;
    // bytes 4-7 ?
    private Template _08w_backedUpTemplate; // 3x3 tiles
    // byte byte28;
    // byte 29 pattern index
    // if pos vis? if neg invis?
    private int _26b_misc;
    private int _27b_linkId_nbNodes_health;
    private int _28b_unkEffect;
    private int _29b_initialState_templateIndex;

    //public int portalIndex;

    //public string description;

    public Cover (Dump dump, int address) {
      _address = address;

      _00w_x = dump.ReadWord (address);
      address += 2;
      _02w_y = dump.ReadWord (address);
      address += 2;
      // bytes 4-7 ?
      address += 4;
      _08w_backedUpTemplate = new Template (dump, address);
      address += 2 * 9;
      _26b_misc = dump.ReadByte (address);
      //if (_raw26 != 0) {
      //  throw new Exception ();
      //}
      address += 1;
      _27b_linkId_nbNodes_health = dump.ReadByteSigned (address);
      //if (linkId >= 0) {
      //  int j = 10;
      //}
      address += 1;
      _28b_unkEffect = dump.ReadByte (address);
      address += 1;
      _29b_initialState_templateIndex = dump.ReadByteSigned (address);
      address += 1;


      //link26A = _raw26 >> 6 & 0x03; // 11000000 b0: set to 1 ingame? (=1&3)
      //                                       0: cover normal
      //                                       2: portal when hit?
      //link26B = _raw26 >> 5 & 0x01; // 00100000 always 0 if C != 0x1F? (set ingame?)
      //                                       always 1 if C == 0x1F                         
      //portalIndex = _raw26 >> 0 & 0x1F; // 00011111 param
      //if ((unk26B !=0) && (unk26C != 0x1f)) {
      //  int i=0;
      //}
    }

    public void Check () {
      //if ((Misc_A == 0x00) && (Misc_B == 0) && (Misc_PortalIndex == 0) && (LinkId27b == 0)) {
      //  return;
      //}
      if ((Misc_A == 0x00) && (Misc_B == 0) && (Misc_PortalIndex == 0) && (LinkId27b > 0)) {
        return;
      }
      if ((Misc_A == 0x00) && (Misc_B == 1) && (Misc_PortalIndex == 31) && (LinkId27b < 0)) {
        return;
      }
      if ((Misc_A == 0x02) && (Misc_B == 0) && (Misc_PortalIndex >= 0) && (LinkId27b >= 0)) {
        return;
      }
      if ((Misc_A == 0x03) && (Misc_B == 0) && (Misc_PortalIndex >= 0) && (LinkId27b >= 0)) {
        return;
      }
      throw new Exception ();
      // ?
      if (Misc_A == 0x01) {
        throw new Exception ();
      }

      // portal closed & opened
      if (IsPortal) {
        if (NbNodesToActivate < 0) {
          throw new Exception ();
        }
        if (Misc_B == 1) {
          throw new Exception ();
        }
      }

      // classic
      if (IsClassic) {
        if (Misc_A != 0) {
          throw new Exception ();
        }
        if (_27b_linkId_nbNodes_health >= 0) {
          throw new Exception ();
        }
      }

      // classic
      if (_27b_linkId_nbNodes_health < 0) {
        if (Misc_PortalIndex != 31) {
          throw new Exception ();
        }
        if (Misc_B == 0) {
          throw new Exception ();
        }
      }
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

    public Template BackedUpTemplate {
      get {
        return (_08w_backedUpTemplate);
      }
    }

    public int TemplateIndex {
      get {
        return (Math.Abs (_29b_initialState_templateIndex));
      }
    }

    public int Raw26b {
      get {
        return (_26b_misc);
      }
      set {
        _26b_misc = value;
      }
    }

    public int Raw27b {
      get {
        return (_27b_linkId_nbNodes_health);
      }
      set {
        _27b_linkId_nbNodes_health = value;
      }
    }

    public int Misc_A {
      get {
        return ((_26b_misc >> 6) & 0x03);
      }
      set {
        _26b_misc &= ~(0x03 << 6);
        _26b_misc |= ((value & 0x03) << 6);
      }
    }

    public int Misc_B {
      get {
        return ((_26b_misc >> 5) & 0x01);
      }
      set {
        _26b_misc &= ~(0x01 << 5);
        _26b_misc |= ((value & 0x01) << 5);
      }
    }

    public int Misc_PortalIndex {
      get {
        return ((_26b_misc >> 0) & 0x1F);
      }
      set {
        _26b_misc &= ~(0x1F << 0);
        _26b_misc |= ((value & 0x1F) << 0);
      }
    }

    public int LinkId27b {
      get {
        return (_27b_linkId_nbNodes_health);
      }
    }

    public int NbNodesToActivate {
      get {
        return (_27b_linkId_nbNodes_health);
      }
    }

    public bool IsPortal {
      get {
        return ((Misc_A == 0x02) || (Misc_A == 0x03));
      }
    }

    public bool IsClosedPortal {
      get {
        return (Misc_A == 0x02);
      }
    }

    public bool IsOpenedPortal {
      get {
        return (Misc_A == 0x03);
      }
    }

    public bool IsClassic {
      get {
        return (Misc_B == 1);
      }
    }

    public int InitialState {
      get {
        return ((_29b_initialState_templateIndex < 0) ? 1 : 0);
      }
    }

    public string InitialStateString {
      get {
        return ((InitialState == 0) ? "ON" : "OFF");
      }
    }

    public string Description (VersionEnum version, int levelIndex) {
      StringBuilder sb = new StringBuilder ();
      sb.AppendLine (string.Format ("***** COVER *****"));
      sb.AppendLine (string.Format ("address: {0:X6}", 0xB9C22 + _address));
      sb.AppendLine (string.Format ("X: {0}", X));
      sb.AppendLine (string.Format ("Y: {0}", Y));
      //sb.AppendLine (string.Format ("_raw26: 0x{0:X2}", _raw26));
      sb.AppendLine (string.Format ("Misc_A: {0}", Misc_A));
      sb.AppendLine (string.Format ("Misc_B: {0}", Misc_B));
      sb.AppendLine (string.Format ("Misc_PortalIndex: {0}", Misc_PortalIndex));
      sb.AppendLine (string.Format ("LinkId/NbNodes/Health: {0}", _27b_linkId_nbNodes_health));
      sb.AppendLine (string.Format ("28b_unkEffect: 0x{0:X2}", _28b_unkEffect));
      sb.AppendLine (string.Format ("TemplateIndex: {0}", TemplateIndex));
      sb.AppendLine (string.Format (""));
      if (IsPortal) {
        Element00_C_Portal portal = LevsdatBin.block00_Levels [(int) version].elements00 [levelIndex].GetElement00_C_Portal (Misc_PortalIndex);
        sb.AppendLine (string.Format ("LevelIndex: {0}", portal.LevelIndex));
        sb.AppendLine (string.Format ("SpawnLocationIndex: {0}", portal.spawnLocationIndex));
        sb.AppendLine (string.Format ("EnteringHeroEffect: {0}", portal.enteringHeroEffect));
      }
      return (sb.ToString ());
    }
  }
}
