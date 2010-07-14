using System;
using System.Collections.Generic;
using System.Text;
using System.IO;

namespace ChaosEngineViewer {
  public class Dump {
    private byte [] _datas;

    public Dump (string fileName, int offset) {
      Stream stream = new FileStream (fileName, System.IO.FileMode.Open);

      _datas = new byte [stream.Length];
      stream.Seek (offset, SeekOrigin.Begin);
      stream.Read (_datas, 0, (int) stream.Length-offset);
      stream.Close ();
    }

    public Dump (string fileName) : this (fileName, 0) {
    }

    public Dump (byte [] datas) {
      _datas = datas;
    }

    public Dump (byte [] datas, int offset) {
      _datas = new byte [datas.Length - offset];
      for (int i = 0; i < _datas.Length; i++) {
        _datas [i] = datas [offset + i];
      }
    }

    public byte [] GetBytes () {
      return (_datas);
    }

    public int ReadByte (long address) {
      return (_datas [address]);
    }

    public int ReadByteSigned (long address) {
      int b = _datas [address];
      if (b >= 0x80) {
        b -= 0xFF + 1;
      }
      return (b);
    }

    public int ReadWord (long address) {
      return ((_datas [address] << 8) + (_datas [address + 1]));
    }

    public int ReadWordSigned (long address) {
      int word = (_datas [address] << 8) + (_datas [address + 1]);
      if (word >= 0x08000) {
        word -= 0xFFFF + 1;
      }
      return (word);
    }

    public int ReadLong (long address) {
      return ((_datas [address] << 24) + (_datas [address + 1] << 16) + (_datas [address + 2] << 8) + (_datas [address + 3]));
    }

    public string ReadString (long address) {
      StringBuilder sb = new StringBuilder ();
      while (ReadByte (address) != 0) {
        sb.Append ((char) (ReadByte (address)));
        address++;
      }
      return (sb.ToString ());
    }

    public string GetString (int i) {
      //if ((i == 18) || (i == 19)) {
      //  int j = 5;
      //}
      return (ReadString (0x14CB2 + ReadWord (0x15126 + i * 2)));
    }

    public int Size {
      get {
        return (_datas.Length);
      }
    }
  }
}
