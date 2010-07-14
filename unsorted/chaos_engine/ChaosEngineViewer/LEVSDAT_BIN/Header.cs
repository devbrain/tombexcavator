using System;
using System.Collections.Generic;
using System.Text;

namespace ChaosEngineViewer.LEVSDAT_BIN {
  public class Header {
    public int addressBlock00;
    public int addressBlock01;
    public int addressBlock02;
    public int addressBlock03;
    public int addressBlock04;
    public int addressBlock05;
    public int addressBlock06;
    public int addressBlock07;
    public int addressBlock08;
    public int addressBlock09;
    public int addressBlock10;
    public int addressBlock11;
    public int addressBlock12;
    public int addressBlock13;
    public int addressBlock14;
    public int addressBlock15;
    public int addressBlock16;
    public int addressBlock17;
    public int addressBlock18;
    public string ends;
    // 2 bytes

    public Header (Dump dump) {
      addressBlock00 = dump.ReadLong (00 * 4);
      addressBlock01 = dump.ReadLong (01 * 4);
      addressBlock02 = dump.ReadLong (02 * 4);
      addressBlock03 = dump.ReadLong (03 * 4);
      addressBlock04 = dump.ReadLong (04 * 4);
      addressBlock05 = dump.ReadLong (05 * 4);
      addressBlock06 = dump.ReadLong (06 * 4);
      addressBlock07 = dump.ReadLong (07 * 4);
      addressBlock08 = dump.ReadLong (08 * 4);
      addressBlock09 = dump.ReadLong (09 * 4);
      addressBlock10 = dump.ReadLong (10 * 4);
      addressBlock11 = dump.ReadLong (11 * 4);
      addressBlock12 = dump.ReadLong (12 * 4);
      addressBlock13 = dump.ReadLong (13 * 4);
      addressBlock14 = dump.ReadLong (14 * 4);
      addressBlock15 = dump.ReadLong (15 * 4);
      addressBlock16 = dump.ReadLong (16 * 4);
      addressBlock17 = dump.ReadLong (17 * 4);
      addressBlock18 = dump.ReadLong (18 * 4);
      ends = dump.ReadString (19 * 4);
    }
  }
}
