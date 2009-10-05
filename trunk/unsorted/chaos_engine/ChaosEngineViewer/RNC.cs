using System;
using System.Collections.Generic;
using System.Text;
using System.IO;

namespace ChaosEngineViewer {
  public static class RNC {

    private class BitStream {
      public ulong bitbuf;	       /* holds between 16 and 32 bits */
      public int bitcount;		       /* how many bits does bitbuf hold? */
    }

    private class HufElement {
      public ulong code;
      public int codelen;
      public int value;
    }

    private class HufTable {
      public int num;			       /* number of nodes in the tree */
      public HufElement [] table; // [32];
      public HufTable () {
        table = new HufElement [32];
        for (int i = 0; i < 32; i++) {
          table [i] = new HufElement ();
        }
      }
    }

    public static short RNC_FILE_IS_NOT_RNC = -1;
    public static short RNC_HUF_DECODE_ERROR = -2;
    public static short RNC_FILE_SIZE_MISMATCH = -3;
    public static short RNC_PACKED_CRC_ERROR = -4;
    public static short RNC_UNPACKED_CRC_ERROR = -5;
    public static short RNC_HEADER_VAL_ERROR = -6;
    public static short RNC_HUF_EXCEEDS_RANGE = -7;
    public static short RNC_HEADER_SIGNATURE_ERROR = -8;
    public static short RNC_HEADER_SIGNATURE = -9;
    public static short RNC_UNKNOWN_ERROR = -10;

    public static ushort RNC_IGNORE_FILE_IS_NOT_RNC = 0x0001;
    public static ushort RNC_IGNORE_HUF_DECODE_ERROR = 0x0002;
    public static ushort RNC_IGNORE_FILE_SIZE_MISMATCH = 0x0004;
    public static ushort RNC_IGNORE_PACKED_CRC_ERROR = 0x0008;
    public static ushort RNC_IGNORE_UNPACKED_CRC_ERROR = 0x0010;
    public static ushort RNC_IGNORE_HEADER_VAL_ERROR = 0x0020;
    public static ushort RNC_IGNORE_HUF_EXCEEDS_RANGE = 0x0040;
    public static ushort RNC_IGNORE_HEADER_SIGNATURE_ERROR = 0x0080;

    public static ulong RNC1_SIGNATURE = 0x524E4301;       /* "RNC1" */
    public static ulong RNC2_SIGNATURE = 0x524E4302;       /* "RNC2" */

    //private static int CHECKSUMS = 1;
    private static int PROTECTED = 0;

    //private static int NOT_PACKED = 0;
    //private static int PACKED_CRC = -1;
    //private static int UNPACKED_CRC = -2;

    private static int CRC_POLY = 0xA001;

    private static ushort [] _crcPrecalc;

    static RNC () {
      _crcPrecalc = new ushort [256];
      ushort val;
      //computing CRC table
      for (ushort i = 0; i < 256; i++) {
        val = i;

        for (int j = 0; j < 8; j++) {
          if ((val & 1) == 1) {
            val = (ushort) ((val >> 1) ^ CRC_POLY);
          }
          else {
            val = (ushort) (val >> 1);
          }
        }
        _crcPrecalc [i] = val;
      }
    }

    public static int Decompress (string iname, string oname, ushort? key) {
      //Console.WriteLine ("Unpacking {0} to {1}", iname, oname);
      byte [] unpacked;

      int result = Decompress (iname, out unpacked, key);
      if (result != 0) {
        return (result);
      }

      //Write results to a file
      try {
        WriteTofile (oname, unpacked);
      }
      catch {
        return (RNC_UNKNOWN_ERROR);
      }

      return (0);
    }

    public static int Decompress (string iname, string oname) {
      return (Decompress (iname, oname, null));
    }

    public static int Decompress (string iname, out byte [] unpacked, ushort? key) {
      unpacked = null;

      Stream ifp;
      try {
        ifp = new FileStream (iname, System.IO.FileMode.Open);
      }
      catch {
        return (RNC_UNKNOWN_ERROR);
      }

      //byte [] buffer = new byte [4];
      //ifp.Read (buffer, 0, 4);
      //ulong signature = (ulong) ((buffer [0] << 24) | (buffer [1] << 16) | (buffer [2] << 8) | (buffer [3] << 0));
      //if (signature != RNC_SIGNATURE) {
      //  return (1);
      //}
      //ifp.Seek (0, SeekOrigin.Begin);

      byte [] packed = new byte [ifp.Length];
      ifp.Read (packed, 0, (int) ifp.Length);
      ifp.Close ();

      return (Decompress (packed, out unpacked, key));
    }

    public static int Decompress (string iname, out byte [] unpacked) {
      return (Decompress (iname, out unpacked, null));
    }

    public static int DecompressMulti (string iname, out byte [] [] unpacked, ushort? key) {
      unpacked = null;

      Stream ifp;
      try {
        ifp = new FileStream (iname, System.IO.FileMode.Open);
      }
      catch {
        return (RNC_UNKNOWN_ERROR);
      }

      byte [] raw = new byte [ifp.Length];
      ifp.Read (raw, 0, (int) ifp.Length);
      ifp.Close ();

      return (DecompressMulti (raw, out unpacked, key));
    }

    public static int DecompressMulti (string iname, out byte [] [] unpacked) {
      return (DecompressMulti (iname, out unpacked, null));
    }

    public static int DecompressMulti (byte [] raw, out byte [] [] unpacked, ushort? key) {
      unpacked = null;

      int firstRNC = 0;
      for (; ; ) {
        if (firstRNC > (raw.Length - 4)) {
          return (1);
        }
        if (ReadLongBE (raw, firstRNC) == RNC1_SIGNATURE) {
          break;
        }
        firstRNC++;
      }

      if ((firstRNC % 4) != 0) {
        return (1);
      }
      int nbRNC = firstRNC / 4;
      unpacked = new byte [nbRNC] [];
      for (int i = 0; i < nbRNC; i++) {
        int rncStartOffset = (int) ReadLongBE (raw, i * 4);
        int rncEndOffset = raw.Length;
        // look for the next offset different from the current
        for (int j = i + 1; j < nbRNC; j++) {
          int address = (int) ReadLongBE (raw, j * 4);
          if (address != rncStartOffset) {
            rncEndOffset = address;
            break;
          }
        }

        //if (i == (nbRNC - 1)) {
        //  rncEndOffset = raw.Length;
        //}
        //else {
        //  rncEndOffset = (int) blong (raw, (i + 1) * 4);
        //}

        byte [] packed = new byte [rncEndOffset - rncStartOffset];
        for (int j = 0; j < packed.Length; j++) {
          packed [j] = raw [rncStartOffset + j];
        }
        int result = (Decompress (packed, out unpacked [i], key));
        if (result != 0) {
          return (result);
        }
      }

      return (0);
    }

    public static int DecompressMulti (byte [] raw, out byte [] [] unpacked) {
      return (DecompressMulti (raw, out unpacked, null));
    }

    public static int Decompress (byte [] packed, out byte [] unpacked, ushort? key) {
      //if (main_unpack (packed, out unpacked, RNC.RNC_IGNORE_HEADER_SIGNATURE_ERROR) == 1) {
      //  return (1);
      //}

      //return (0);
      return (Decompress (packed, out unpacked, key, (ushort) (RNC.RNC_IGNORE_HEADER_SIGNATURE_ERROR + RNC.RNC_IGNORE_UNPACKED_CRC_ERROR)));
    }

    public static int Decompress (byte [] packed, out byte [] unpacked) {
      //if (main_unpack (packed, out unpacked, RNC.RNC_IGNORE_HEADER_SIGNATURE_ERROR) == 1) {
      //  return (1);
      //}

      //return (0);
      return (Decompress (packed, out unpacked, null));
    }

    public static int Decompress (byte [] packed, out byte [] unpacked, ushort? key, ushort flags) {
      flags = 0;
      unpacked = null;

      if (packed.Length < 4) {
        return (RNC_UNKNOWN_ERROR);
      }

      ulong signature = ReadSignature (packed);
      int unpackedSize = (int) ReadUnpackedSize (packed);
      int packedSize = (int) ReadPackedSize (packed);
      ushort unpackedCrc = ReadUnpackedCrc (packed);
      ushort packedCrc = ReadPackedCrc (packed);


      if (signature == RNC1_SIGNATURE) {
      }
      else if (signature == RNC2_SIGNATURE) {
      }
      else {
        //else if ((flags & RNC_IGNORE_HEADER_SIGNATURE_ERROR) == 0) {
        //  return (RNC_HEADER_SIGNATURE_ERROR);
        //}
        //unpacked = new byte [packed.Length];
        //for (int i = 0; i < packed.Length; i++) {
        //  unpacked [i] = packed [i];
        //}
        //return (0);
        //return (RNC_HEADER_SIGNATURE);
        ReallocArray (packed, out unpacked, packed.Length);
        return (0);
      }

      if (packedCrc != CalcCrc (packed, 18, packedSize)) {
        if ((flags & RNC_IGNORE_PACKED_CRC_ERROR) == 0) {
          return RNC_PACKED_CRC_ERROR;
        }
      }

      byte [] packed_;
      ReallocArray (packed, out packed_, packed.Length + 8);
      packed = packed_;
      unpacked = new byte [unpackedSize];
      //if (unpackedSize < 0) {
      //  if (unpackedSize == -1) {
      //    // File wasn't RNC to start with
      //    return (RNC_UNKNOWN_ERROR);
      //  }
      //  //printf("Error: %s\n", rnc_error (ulen));
      //  return (RNC_UNKNOWN_ERROR);
      //}
      int result = 0;
      if (signature == RNC1_SIGNATURE) {
        result = UnpackRNC1 (packed, unpacked, key, flags);
        if (result != 0) {
          Console.WriteLine ("RNC ERROR: {0}", result);
        }
      }
      else if (signature == RNC2_SIGNATURE) {
        result = UnpackRNC2 (packed, unpacked, key, flags);
      }

      if (unpackedCrc != CalcCrc (unpacked, 0, unpackedSize)) {
        if ((flags & RNC_IGNORE_UNPACKED_CRC_ERROR) == 0) {
          return RNC_UNPACKED_CRC_ERROR;
        }
      }

      return (result);
    }

    private static string [] errors = {
      "No error",
	    "File is not RNC-1 format",
	    "Huffman decode error",
	    "File size mismatch",
	    "CRC error in packed data",
	    "CRC error in unpacked data",
	    "Compressed file header invalid",
	    "Huffman decode leads outside buffers",
	    "Unknown error"
    };

    #region RNC1
    private static int UnpackRNC1 (byte [] packed, byte [] unpacked, ushort? key, ushort flags) {
      BitStream bs = new BitStream ();
      HufTable raw = new HufTable (), dist = new HufTable (), len = new HufTable ();
      ulong ch_count;
      int unpackedSize = (int) ReadUnpackedSize (packed);
      int packedSize = (int) ReadPackedSize (packed);
      //if ((unpackedSize > (1 << 30)) || (packedSize > (1 << 30))) {
      //  return RNC_HEADER_VAL_ERROR;
      //}

      int packedIndex = 18;
      int unpackedIndex = 0;
      int packedIndexEnd = packedIndex + packedSize;
      int unpackedIndexEnd = unpackedIndex + unpackedSize;

      BitreadInit (ref bs, packed, ref packedIndex, packedIndexEnd);
      BitAdvance (ref bs, 2, packed, ref packedIndex, packedIndexEnd);      // discard first two bits

      bool hasKey = false;
      ushort keyTemp = 0;
      if (key != null) {
        hasKey = true;
        keyTemp = (ushort) key;
      }
      while (unpackedIndex < unpackedIndexEnd) {
        if (packedIndexEnd - packedIndex < 6) {
          if ((flags & RNC_IGNORE_HUF_EXCEEDS_RANGE) == 0) {
            return RNC_HUF_EXCEEDS_RANGE;
          }
          else {
            unpackedIndex = unpackedIndexEnd;
            ch_count = 0;
            break;
          }
        }
        ReadHuftable (ref raw, ref bs, packed, ref packedIndex, packedIndexEnd);
        ReadHuftable (ref dist, ref bs, packed, ref packedIndex, packedIndexEnd);
        ReadHuftable (ref len, ref bs, packed, ref packedIndex, packedIndexEnd);
        ch_count = BitRead (ref bs, 0xFFFF, 16, packed, ref packedIndex, packedIndexEnd);
        while (true) {
          long length, posn;

          length = (long) HufRead (ref raw, ref bs, packed, ref packedIndex, packedIndexEnd);
          if (length < 0) {
            if ((flags & RNC_IGNORE_HUF_DECODE_ERROR) == 0) {
              return RNC_HUF_DECODE_ERROR;
            }
            else {
              unpackedIndex = unpackedIndexEnd;
              ch_count = 0;
              break;
            }
          }
          if (length > 0) {
            while (length > 0) {
              length--;
              if ((packedIndex >= packedIndexEnd) || (unpackedIndex >= unpackedIndexEnd)) {
                if ((flags & RNC_IGNORE_HUF_EXCEEDS_RANGE) == 0) {
                  return RNC_HUF_EXCEEDS_RANGE;
                }
                else {
                  unpackedIndex = unpackedIndexEnd;
                  ch_count = 0;
                  break;
                }
              }
              unpacked [unpackedIndex++] = packed [packedIndex++];
              if (hasKey) {
                unpacked [unpackedIndex - 1] ^= (byte) (keyTemp & 0x00FF);
              }
            }
            if (hasKey) {
              keyTemp = (ushort) ((keyTemp >> 1) | ((keyTemp & 1) << 15));
            }
            BitreadFix (ref bs, packed, ref packedIndex, packedIndexEnd);
          }
          if (--ch_count <= 0) {
            break;
          }

          posn = (long) HufRead (ref dist, ref bs, packed, ref packedIndex, packedIndexEnd);
          if (posn == -1) {
            if ((flags & RNC_IGNORE_HUF_DECODE_ERROR) == 0) {
              return RNC_HUF_DECODE_ERROR;
            }
            else {
              unpackedIndex = unpackedIndexEnd;
              ch_count = 0;
              break;
            }
          }
          length = (long) HufRead (ref len, ref bs, packed, ref packedIndex, packedIndexEnd);
          if (length < 0) {
            if ((flags & RNC_IGNORE_HUF_DECODE_ERROR) == 0) {
              return RNC_HUF_DECODE_ERROR;
            }
            else {
              unpackedIndex = unpackedIndexEnd;
              ch_count = 0;
              break;
            }
          }
          posn += 1;
          length += 2;
          while (length > 0) {
            length--;
            if (((unpackedIndex - posn) < 0) || ((unpackedIndex - posn) > unpackedIndexEnd) ||
                (unpackedIndex < 0) || (unpackedIndex > unpackedIndexEnd)) {
              if ((flags & RNC_IGNORE_HUF_EXCEEDS_RANGE) == 0) {
                return RNC_HUF_EXCEEDS_RANGE;
              }
              else {
                unpackedIndex = unpackedIndexEnd - 1;
                ch_count = 0;
                break;
              }
            }
            unpacked [unpackedIndex] = unpacked [unpackedIndex - posn];
            unpackedIndex++;
          }
        }
      }

      if (unpackedIndexEnd != unpackedIndex) {
        if ((flags & RNC_IGNORE_FILE_SIZE_MISMATCH) == 0) {
          return RNC_FILE_SIZE_MISMATCH;
        }
      }

      return (0);
    }

    private static void ReadHuftable (ref HufTable h, ref BitStream bs, byte [] data, ref int p, int pend) {
      int i, j, k, num;
      int [] leaflen = new int [32];
      int leafmax;
      ulong codeb;

      num = (int) BitRead (ref bs, 0x1F, 5, data, ref p, pend);
      if (num == 0) {
        return;
      }

      leafmax = 1;
      for (i = 0; i < num; i++) {
        leaflen [i] = (int) BitRead (ref bs, 0x0F, 4, data, ref p, pend);
        if (leafmax < leaflen [i]) {
          leafmax = leaflen [i];
        }
      }

      codeb = 0L;
      k = 0;
      for (i = 1; i <= leafmax; i++) {
        for (j = 0; j < num; j++) {
          if (leaflen [j] == i) {
            h.table [k].code = Mirror (codeb, i);
            h.table [k].codelen = i;
            h.table [k].value = j;
            codeb++;
            k++;
          }
        }
        codeb <<= 1;
      }

      h.num = k;
    }

    private static ulong HufRead (ref HufTable h, ref BitStream bs, byte [] data, ref int p, int pend) {
      int i;
      ulong val;

      for (i = 0; i < h.num; i++) {
        ulong mask = (ulong) ((1 << h.table [i].codelen) - 1);
        if (BitPeek (bs, mask) == h.table [i].code)
          break;
      }
      if (i == h.num)
        throw new Exception ();
      BitAdvance (ref bs, h.table [i].codelen, data, ref p, pend);

      val = (ulong) h.table [i].value;

      if (val >= 2) {
        val = (ulong) (1 << (((int) val) - 1));
        val |= BitRead (ref bs, val - 1, h.table [i].value - 1, data, ref p, pend);
      }
      return (val);
    }

    static void BitreadInit (ref BitStream bs, byte [] data, ref int p, int pend) {
      if ((pend - p) >= 0) {
        bs.bitbuf = ReadWordLE (data, p);
      }
      else {
        bs.bitbuf = 0;
      }
      bs.bitcount = 16;
    }

    private static void BitreadFix (ref BitStream bs, byte [] data, ref int p, int pend) {
      bs.bitcount -= 16;
      bs.bitbuf &= (ulong) ((1 << bs.bitcount) - 1); // remove the top 16 bits
      if ((pend - p) >= 0) {
        bs.bitbuf |= ((ulong) (ReadWordLE (data, p))) << bs.bitcount;// replace with what's at *p
      }
      bs.bitcount += 16;
    }

    private static ulong BitPeek (BitStream bs, ulong mask) {
      return (bs.bitbuf & mask);
    }

    private static void BitAdvance (ref BitStream bs, int n, byte [] data, ref int p, int pend) {
      bs.bitbuf >>= n;
      bs.bitcount -= n;
      if (bs.bitcount < 16) {
        p += 2;
        if ((pend - p) >= 0) {
          bs.bitbuf |= ((ulong) (ReadWordLE (data, p))) << bs.bitcount;
        }
        bs.bitcount += 16;
      }
    }

    private static ulong BitRead (ref BitStream bs, ulong mask, int n, byte [] data, ref int p, int pend) {
      ulong result = BitPeek (bs, mask);
      BitAdvance (ref bs, n, data, ref p, pend);
      return (result);
    }

    private static ulong Mirror (ulong x, int n) {
      ulong top = (ulong) (1 << (n - 1));
      ulong bottom = 1;

      while (top > bottom) {
        ulong mask = top | bottom;
        ulong masked = x & mask;
        if (masked != 0 && masked != mask) {
          x ^= mask;
        }
        top >>= 1;
        bottom <<= 1;
      }
      return (x);
    }
    #endregion

    #region RNC2
    private static int UnpackRNC2 (byte [] packed, byte [] unpacked, ushort? key, ushort flags) {
      //ReallocArray (data, out packed, data.Length + 4);
      //ulong id = ReadLongBE (packed, packedIndex + 0);
      //if (id != PACK_ID) {
      //  return (NOT_PACKED);
      //}
      //ulong unpackedSize = ReadLongBE (packed, packedIndex + 4);
      //ulong packedSize = ReadLongBE (packed, packedIndex + 8);
      //ushort unpacked_crc = ReadWordBE (packed, packedIndex + 12);
      //ushort packed_crc = ReadWordBE (packed, packedIndex + 14);
      //ushort unk = read_short (input_tab, input + 16);

      //int packedIndex = 18;
      //int packedIndexEnd = packedIndex + packedSize;
      //int unpackedIndexEnd = unpackedIndex + unpackedSize;

      //if (CHECKSUMS != 0) {
      //  ushort crc = rnc_crc (packed, packedIndex, packed_size);
      //  if (crc != packed_crc) {
      //    return (PACKED_CRC);
      //  }
      //}

      //unpacked = new byte [unpackedSize];

      byte bitbuf;
      int packedIndex = 18;
      int unpackedIndex = 0;
      bool carry = reload (out bitbuf, packed, ref packedIndex, true);
      carry = getbit (ref bitbuf);
      while (!xLoop (ref bitbuf, packed, ref packedIndex, unpacked, ref unpackedIndex, ref key)) {
        //WriteOutputTab (unpacked);
      }

      //if (CHECKSUMS != 0) {
      //  ushort crc = rnc_crc (output_tab, 0, unpacked_size);
      //  if (crc != unpacked_crc) {
      //    return (UNPACKED_CRC);
      //  }
      //}
      //WriteOutputTab ();
      return (0);
    }

    private static bool xLoop (ref byte bitbuf, byte [] packed, ref int packedIndex, byte [] unpacked, ref int unpackedIndex, ref ushort? key) {
      bool carry;
      for (; ; ) {
        for (; ; ) {
          carry = getbit (ref  bitbuf); // xLoop
          if (carry) {
            break;
          }
          getraw (packed, ref packedIndex, unpacked, ref unpackedIndex, ref key);

          //carry = getbit ();
          //if (carry) {
          //  break;
          //}
          //getraw ();
        }
        if (bitbuf != 0) { // Chkz
          break;
        }
        carry = reload (out bitbuf, packed, ref packedIndex, carry); // GetBits
        if (carry) {
          break;
        }
        getraw (packed, ref packedIndex, unpacked, ref unpackedIndex, ref key);
      }
      //if (unpackedIndex >= 0x72500) {
      //  //WriteOutputTab ();
      //  int z = 0;
      //}
      return (String (ref bitbuf, packed, ref packedIndex, unpacked, ref unpackedIndex, ref key));
    }

    private static bool getbit (ref byte bitbuf) {
      bool carryOut = (bitbuf >> 7) == 1;
      bitbuf <<= 1;
      return (carryOut);
    }

    private static bool reload (out byte bitbuf, byte [] packed, ref int packedIndex, bool carryIn) {
      bitbuf = packed [packedIndex++];
      bool carryOut = (bitbuf >> 7) == 1;
      bitbuf <<= 1;
      if (carryIn) {
        bitbuf |= 1;
      }
      return (carryOut);
    }

    private static void getraw (byte [] packed, ref int packedIndex, byte [] unpacked, ref int unpackedIndex, ref ushort? key) {
      if (key == null) {
        unpacked [unpackedIndex++] = packed [packedIndex++];
      }
      else {
        byte d0 = packed [packedIndex++];
        d0 ^= (byte) (key & 0x00FF);
        unpacked [unpackedIndex++] = d0;
        key = (ushort) ((key >> 1) | (key << 15));
      }
    }

    private static void getrawREP (byte [] packed, ref int packedIndex, byte [] unpacked, ref int unpackedIndex, ref ushort? key, ushort pos) {
      for (int i = 0; i <= pos; i++) {
        if (key == null) {
          unpacked [unpackedIndex++] = packed [packedIndex++];
          unpacked [unpackedIndex++] = packed [packedIndex++];
          unpacked [unpackedIndex++] = packed [packedIndex++];
          unpacked [unpackedIndex++] = packed [packedIndex++];
        }
        else {
          byte d0 = packed [packedIndex++];
          d0 ^= (byte) (key & 0x00FF);
          unpacked [unpackedIndex++] = d0;
          d0 = packed [packedIndex++];
          d0 ^= (byte) (key & 0x00FF);
          unpacked [unpackedIndex++] = d0;
          d0 = packed [packedIndex++];
          d0 ^= (byte) (key & 0x00FF);
          unpacked [unpackedIndex++] = d0;
          d0 = packed [packedIndex++];
          d0 ^= (byte) (key & 0x00FF);
          unpacked [unpackedIndex++] = d0;
        }
      }
      if (PROTECTED != 0) {
        key = (ushort) ((key >> 1) | ((key & 1) << 15));
      }
    }

    private static void Raw (ref byte bitbuf, byte [] packed, ref int packedIndex, byte [] unpacked, ref int unpackedIndex, ref ushort? key, ushort pos) {
      for (int i = 0; i < 4; i++) {
        bool carry = getbit (ref bitbuf);
        if (bitbuf == 0) { // Back0
          carry = reload (out bitbuf, packed, ref packedIndex, carry);
        }
        pos <<= 1;
        if (carry) {
          pos |= 1;
        }
      }
      pos += 2;
      getrawREP (packed, ref packedIndex, unpacked, ref unpackedIndex, ref key, pos);
    }

    private static void GetLen (ref byte bitbuf, byte [] packed, ref int packedIndex, byte [] unpacked, ref int unpackedIndex, ref ushort? key, ushort len, ushort pos) {
      bool carry = getbit (ref bitbuf);
      if (bitbuf == 0) { // Back1
        carry = reload (out bitbuf, packed, ref packedIndex, carry);
      }
      len <<= 1;
      if (carry) {
        len |= 1;
      }
      carry = getbit (ref bitbuf);
      if (bitbuf == 0) { // Back2
        carry = reload (out bitbuf, packed, ref packedIndex, carry);
      }
      if (!carry) {
        Copy (ref bitbuf, packed, ref packedIndex, unpacked, ref unpackedIndex, len, pos);
      }
      else {
        len--;
        carry = getbit (ref bitbuf);
        if (bitbuf == 0) { // Back3
          carry = reload (out bitbuf, packed, ref packedIndex, carry);
        }
        len <<= 1;
        if (carry) {
          len |= 1;
        }
        if ((len & 0x00FF) == 9) {
          Raw (ref bitbuf, packed, ref packedIndex, unpacked, ref unpackedIndex, ref key, pos);
        }
        else {
          Copy (ref bitbuf, packed, ref packedIndex, unpacked, ref unpackedIndex, len, pos);
        }
      }
    }

    private static void Copy (ref byte bitbuf, byte [] packed, ref int packedIndex, byte [] unpacked, ref int unpackedIndex, ushort len, ushort pos) {
      bool carry = getbit (ref bitbuf);
      if (bitbuf == 0) { // Back4
        carry = reload (out bitbuf, packed, ref packedIndex, carry);
      }
      if (!carry) {
        ByteDisp2 (ref bitbuf, packed, ref packedIndex, unpacked, ref unpackedIndex, len, pos);
      }
      else {
        carry = getbit (ref bitbuf);
        if (bitbuf == 0) { // Back5
          carry = reload (out bitbuf, packed, ref packedIndex, carry);
        }
        pos <<= 1;
        if (carry) {
          pos |= 1;
        }
        carry = getbit (ref bitbuf);
        if (bitbuf == 0) { // Back6
          carry = reload (out bitbuf, packed, ref packedIndex, carry);
        }
        if (carry) {
          BigDisp (ref bitbuf, packed, ref packedIndex, unpacked, ref unpackedIndex, len, pos);
        }
        else {
          if (pos != 0) {
            ByteDisp (ref bitbuf, packed, ref packedIndex, unpacked, ref unpackedIndex, len, pos);
          }
          else {
            pos++;
            Another (ref bitbuf, packed, ref packedIndex, unpacked, ref unpackedIndex, len, pos);
          }
        }
      }
    }

    private static void Another (ref byte bitbuf, byte [] packed, ref int packedIndex, byte [] unpacked, ref int unpackedIndex, ushort len, ushort pos) {
      bool carry = getbit (ref bitbuf);
      if (bitbuf == 0) { // Back7
        carry = reload (out bitbuf, packed, ref packedIndex, carry);
      }
      pos <<= 1;
      if (carry) {
        pos |= 1;
      }
      ByteDisp (ref bitbuf, packed, ref packedIndex, unpacked, ref unpackedIndex, len, pos);
    }

    private static void ByteDisp (ref byte bitbuf, byte [] packed, ref int packedIndex, byte [] unpacked, ref int unpackedIndex, ushort len, ushort pos) {
      //bool carry = (pos >> 15) == 1;
      //pos <<= 8;
      //if (carry) {
      //  pos |= 1;
      //}
      pos = (ushort) ((pos << 8) | (pos >> 8));
      ByteDisp2 (ref bitbuf, packed, ref packedIndex, unpacked, ref unpackedIndex, len, pos);
    }

    private static void ByteDisp2 (ref byte bitbuf, byte [] packed, ref int packedIndex, byte [] unpacked, ref int unpackedIndex, ushort len, ushort pos) {
      pos = (ushort) ((pos & 0xFF00) | packed [packedIndex++]);
      int unpackedIndexTemp = unpackedIndex - pos - 1;
      bool carry = (len & 0x0001) == 1;
      len >>= 1;
      if (carry) {
        unpacked [unpackedIndex++] = unpacked [unpackedIndexTemp++];
      }
      if (pos == 0) {
        byte pos2 = unpacked [unpackedIndexTemp];
        for (int i = 0; i < len; i++) {
          unpacked [unpackedIndex++] = pos2;
          unpacked [unpackedIndex++] = pos2;
        }
      }
      else {
        for (int i = 0; i < len; i++) {
          unpacked [unpackedIndex++] = unpacked [unpackedIndexTemp++];
          unpacked [unpackedIndex++] = unpacked [unpackedIndexTemp++];
        }
      }
    }

    private static bool String (ref byte bitbuf, byte [] packed, ref int packedIndex, byte [] unpacked, ref int unpackedIndex, ref ushort? key) {
      ushort len = 2;
      ushort pos = 0;

      //if (packedIndex >= 0x11D) {
      //  int z = 0;
      //}
      bool carry = getbit (ref bitbuf);
      if (bitbuf == 0) { // Back8
        carry = reload (out bitbuf, packed, ref packedIndex, carry);
      }
      if (!carry) {
        GetLen (ref bitbuf, packed, ref packedIndex, unpacked, ref unpackedIndex, ref key, len, pos);
      }
      else {
        return (Smalls (ref bitbuf, packed, ref packedIndex, unpacked, ref unpackedIndex, len, pos));
      }
      return (false);
    }

    private static bool Smalls (ref byte bitbuf, byte [] packed, ref int packedIndex, byte [] unpacked, ref int unpackedIndex, ushort len, ushort pos) {
      bool carry = getbit (ref bitbuf);
      if (bitbuf == 0) { // Back9
        carry = reload (out bitbuf, packed, ref packedIndex, carry);
      }
      if (!carry) {
        ByteDisp2 (ref bitbuf, packed, ref packedIndex, unpacked, ref unpackedIndex, len, pos);
      }
      else {
        len++;
        carry = getbit (ref bitbuf);
        if (bitbuf == 0) { // Back10
          carry = reload (out bitbuf, packed, ref packedIndex, carry);
        }
        if (!carry) {
          Copy (ref bitbuf, packed, ref packedIndex, unpacked, ref unpackedIndex, len, pos);
        }
        else {
          //len |= input_tab [input++];
          //if ((len & 0x00FF) == 0) {
          len = (ushort) ((len & 0xFF00) | packed [packedIndex++]);
          if ((len & 0x00FF) == 0) {
            return (OverNout (ref bitbuf, packed, ref packedIndex));
          }
          else {
            len += 8;
            Copy (ref bitbuf, packed, ref packedIndex, unpacked, ref unpackedIndex, len, pos);
          }
        }
      }
      return (false);
    }

    private static void BigDisp (ref byte bitbuf, byte [] packed, ref int packedIndex, byte [] unpacked, ref int unpackedIndex, ushort len, ushort pos) {
      bool carry = getbit (ref bitbuf);
      if (bitbuf == 0) { // Back11
        carry = reload (out bitbuf, packed, ref packedIndex, carry);
      }
      pos <<= 1;
      if (carry) {
        pos |= 1;
      }
      pos |= 4;
      carry = getbit (ref bitbuf);
      if (bitbuf == 0) { // Back12
        carry = reload (out bitbuf, packed, ref packedIndex, carry);
      }
      if (carry) {
        ByteDisp (ref bitbuf, packed, ref packedIndex, unpacked, ref unpackedIndex, len, pos);
      }
      else {
        Another (ref bitbuf, packed, ref packedIndex, unpacked, ref unpackedIndex, len, pos);
      }
    }

    private static bool OverNout (ref byte bitbuf, byte [] packed, ref int packedIndex) {
      bool carry = getbit (ref bitbuf);
      if (bitbuf == 0) { // Check4end
        carry = reload (out bitbuf, packed, ref packedIndex, carry);
      }
      if (!carry) {
        return (true);
      }
      return (false);
    }
    #endregion




    private static void WriteOutputTab (byte [] unpacked) {
      FileStream stream = new FileStream ("temp.bin", FileMode.Create);
      stream.Write (unpacked, 0, (int) unpacked.Length);
      stream.Close ();
    }

    private static ushort CalcCrc (byte [] data, int p, int len) {
      ushort val = 0;
      while (len > 0) {
        len--;
        val ^= data [p++];
        val = (ushort) ((val >> 8) ^ _crcPrecalc [val & 0xFF]);
      }
      return (val);
    }

    public static void WriteTofile (string fileName, byte [] data) {
      Stream ofp = new FileStream (fileName, System.IO.FileMode.Create);
      ofp.Write (data, 0, (int) data.Length);
      ofp.Close ();
    }

    public static void WriteTofileMulti (string fileName, byte [] [] dataMulti) {
      for (int i = 0; i < dataMulti.Length; i++) {
        byte [] data = dataMulti [i];
        RNC.WriteTofile (string.Format (fileName, i), data);
      }
    }

    private static void ReallocArray (byte [] src, out byte [] dst, int newSize) {
      dst = new byte [newSize];
      int size = Math.Min (src.Length, newSize);
      for (int i = 0; i < size; i++) {
        dst [i] = src [i];
      }
    }

    private static ulong ReadSignature (byte [] packed) {
      return (ReadLongBE (packed, 0));
    }

    private static ulong ReadUnpackedSize (byte [] packed) {
      return (ReadLongBE (packed, 4));
    }

    private static ulong ReadPackedSize (byte [] packed) {
      return (ReadLongBE (packed, 8));
    }

    private static ushort ReadUnpackedCrc (byte [] packed) {
      return (ReadWordBE (packed, 12));
    }

    private static ushort ReadPackedCrc (byte [] packed) {
      return (ReadWordBE (packed, 14));
    }

    private static ulong ReadLongBE (byte [] data, int address) {
      ulong result = (ulong) ((data [address + 0] << 24)
                            | (data [address + 1] << 16)
                            | (data [address + 2] << 08)
                            | (data [address + 3] << 00));
      //ulong result = 0;
      //for (int i = 0; i < 4; i++) {
      //  result = (ulong) ((result << 8) | data [address++]);
      //}
      return (result);
    }

    private static ulong ReadLongLE (byte [] data, int address) {
      ulong result = (ulong) ((data [address + 3] << 24)
                            | (data [address + 2] << 16)
                            | (data [address + 1] << 08)
                            | (data [address + 0] << 00));
      //for (int i = 0; i < 4; i++) {
      //  result = ((ulong) result) | (ulong) (data [address++] << (i * 8));
      //}
      return (result);
    }

    private static ushort ReadWordBE (byte [] data, int address) {
      ushort result = (ushort) ((data [address + 0] << 08)
                              | (data [address + 1] << 00));
      //ushort result = 0;
      //for (int i = 0; i < 2; i++) {
      //  result = (ushort) ((result << 8) | data [address++]);
      //}
      return (result);
    }

    private static ushort ReadWordLE (byte [] data, int address) {
      ushort result = (ushort) ((data [address + 1] << 08)
                              | (data [address + 0] << 00));
      //ushort result = 0;
      //for (int i = 0; i < 2; i++) {
      //  result |= (ushort) (data [address++] << (i * 8));
      //}
      return (result);
    }
  }
}
