using System;
using System.Collections.Generic;
using System.Text;
using System.IO;

namespace ChaosEngineViewer {
  public static class _RNC {
    /*
     * dernc.c   decompress RNC data
     *
     * Compiled normally, this file is a well-behaved, re-entrant code
     * module exporting only `rnc_ulen', `rnc_unpack' and `rnc_error'.
     * Compiled with MAIN_DERNC defined, it's a standalone program which will
     * decompress argv[1] into argv[2].
     *
     * in/out buffers should have 8 redundant "safe bytes" at end.
     */

    /*
     * Routines
     */
    //long rnc_ulen (void *packed);
    //long rnc_unpack (void *packed, void *unpacked, uint flags);
    ////long rnc_unpack (void *packed, void *unpacked, unsigned int flags, long *leeway);
    //char *rnc_error (long errcode);
    //long rnc_crc (void *data, ulong len);

    /*
     * Error returns
     */
    public static short RNC_FILE_IS_NOT_RNC = -1;
    public static short RNC_HUF_DECODE_ERROR = -2;
    public static short RNC_FILE_SIZE_MISMATCH = -3;
    public static short RNC_PACKED_CRC_ERROR = -4;
    public static short RNC_UNPACKED_CRC_ERROR = -5;
    public static short RNC_HEADER_VAL_ERROR = -6;
    public static short RNC_HUF_EXCEEDS_RANGE = -7;
    public static short RNC_HEADER_SIGNATURE_ERROR = -8;
    public static short RNC_HEADER_SIGNATURE2 = -9;
    public static short RNC_UNKNOWN_ERROR = -10;

    /*
     * Flags to ignore errors
     */
    public static ushort RNC_IGNORE_FILE_IS_NOT_RNC = 0x0001;
    public static ushort RNC_IGNORE_HUF_DECODE_ERROR = 0x0002;
    public static ushort RNC_IGNORE_FILE_SIZE_MISMATCH = 0x0004;
    public static ushort RNC_IGNORE_PACKED_CRC_ERROR = 0x0008;
    public static ushort RNC_IGNORE_UNPACKED_CRC_ERROR = 0x0010;
    public static ushort RNC_IGNORE_HEADER_VAL_ERROR = 0x0020;
    public static ushort RNC_IGNORE_HUF_EXCEEDS_RANGE = 0x0040;
    public static ushort RNC_IGNORE_HEADER_SIGNATURE_ERROR = 0x0080;

    /*
     * The compressor needs this define
     */
    public static ulong RNC_SIGNATURE1 = 0x524E4301;       /* "RNC1" */
    public static ulong RNC_SIGNATURE2 = 0x524E4302;       /* "RNC2" */

    private static ushort [] _crctab;

    static _RNC () {
      _crctab = new ushort [256];
      ushort val;
      //computing CRC table
      for (ushort i = 0; i < 256; i++) {
        val = i;

        for (int j = 0; j < 8; j++) {
          if ((val & 1) == 1) {
            val = (ushort) ((val >> 1) ^ 0xA001);
          }
          else {
            val = (ushort) (val >> 1);
          }
        }
        _crctab [i] = val;
      }
    }
    /*
    int main_unpack (char *pname, char *iname, char *oname);
    int copy_file (char *iname, char *oname);
    */

    //int show_usage(char *fname)
    //{
    //  printf("usage:\n");
    //  printf("    %s <files>\n", fname);
    //    printf(" or\n");
    //    printf("    %s -o <infile> <outfile>\n", fname);
    //}

    //int main(int argc, char **argv) {
    //    int mode=0;
    //    int i;

    //    printf("\nBullfrog RNC files decompressor");
    //    printf("\n-------------------------------\n");

    //    if (argc==1)
    //    {
    //        show_usage(*argv);
    //      return 0;
    //    }
    //    for (i=1; i < argc; i++)
    //  if (!strcmp (argv[i], "-o"))
    //      mode=i;
    //    if (mode && argc != 4)
    //    {
    //        show_usage(*argv);
    //      return 1;
    //    }
    //    switch (mode)
    //    {
    //      case 0 :
    //  for (i=1; i < argc; i++)
    //      {
    //        printf("Extracting %s...\n",argv[i]);
    //        if (main_unpack (*argv, argv[i], argv[i]))
    //            return 1;
    //        }
    //  return 0;
    //      case 1 :
    //    printf("Extracting %s to %s...\n",argv[2],argv[3]);
    //  return main_unpack (*argv, argv[2], argv[3]);
    //      case 2 :
    //    printf("Extracting %s to %s...\n",argv[1],argv[3]);
    //  return main_unpack (*argv, argv[1], argv[3]);
    //      case 3 :
    //    printf("Extracting %s to %s...\n",argv[1],argv[2]);
    //  return main_unpack (*argv, argv[1], argv[2]);
    //      default :
    //  fprintf (stderr, "Internal fault.\n");
    //    }
    //    return 1;
    //}
    public static int main_unpack (string iname, string oname, ushort? key) {
      //Console.WriteLine ("Unpacking {0} to {1}", iname, oname);
      byte [] unpacked;

      int result = main_unpack (iname, out unpacked, key);
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

    public static int main_unpack (string iname, string oname) {
      return (main_unpack (iname, oname, null));
    }

    public static int main_unpack (string iname, out byte [] unpacked, ushort? key) {
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

      return (main_unpack (packed, out unpacked, key));
    }

    public static int main_unpack (string iname, out byte [] unpacked) {
      return (main_unpack (iname, out unpacked, null));
    }

    public static int main_unpack_multi (string iname, out byte [] [] unpacked, ushort? key) {
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

      return (main_unpack_multi (raw, out unpacked, key));
    }

    public static int main_unpack_multi (string iname, out byte [] [] unpacked) {
      return (main_unpack_multi (iname, out unpacked, null));
    }

    public static int main_unpack_multi (byte [] raw, out byte [] [] unpacked, ushort? key) {
      unpacked = null;

      int firstRNC = 0;
      for (; ; ) {
        if (firstRNC > (raw.Length - 4)) {
          return (1);
        }
        if (blong (raw, firstRNC) == RNC_SIGNATURE1) {
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
        int rncStartOffset = (int) blong (raw, i * 4);
        int rncEndOffset = raw.Length;
        // look for the next offset different from the current
        for (int j = i + 1; j < nbRNC; j++) {
          int address = (int) blong (raw, j * 4);
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
        int result = (main_unpack (packed, out unpacked [i], key));
        if (result != 0) {
          return (result);
        }
      }

      return (0);
    }

    public static int main_unpack_multi (byte [] raw, out byte [] [] unpacked) {
      return (main_unpack_multi (raw, out unpacked, null));
    }

    public static int main_unpack (byte [] packed, out byte [] unpacked, ushort? key) {
      //if (main_unpack (packed, out unpacked, RNC.RNC_IGNORE_HEADER_SIGNATURE_ERROR) == 1) {
      //  return (1);
      //}

      //return (0);
      return (main_unpack (packed, out unpacked, key, (ushort) (RNC.RNC_IGNORE_HEADER_SIGNATURE_ERROR + RNC.RNC_IGNORE_UNPACKED_CRC_ERROR)));
    }

    public static int main_unpack (byte [] packed, out byte [] unpacked) {
      //if (main_unpack (packed, out unpacked, RNC.RNC_IGNORE_HEADER_SIGNATURE_ERROR) == 1) {
      //  return (1);
      //}

      //return (0);
      return (main_unpack (packed, out unpacked, null));
    }

    private static int main_unpack (byte [] packed, out byte [] unpacked, ushort? key, ushort flags) {
      unpacked = null;

      //Checking if the file is RNC
      if (packed.Length < 4) {
        return (RNC_UNKNOWN_ERROR);
      }

      if (blong (packed, 0) != RNC_SIGNATURE1) {
        if (blong (packed, 0) == RNC_SIGNATURE2) {
          return (RNC_HEADER_SIGNATURE2);
        }
        //else if ((flags & RNC_IGNORE_HEADER_SIGNATURE_ERROR) == 0) {
        //  return (RNC_HEADER_SIGNATURE_ERROR);
        //}
        else {
          unpacked = new byte [packed.Length];
          for (int i = 0; i < packed.Length; i++) {
            unpacked [i] = packed [i];
          }
          return (0);
        }
      }

      //Reading compressed data, 8 bytes in buffer are for safety
      byte [] packed_;
      ReallocArray (packed, out packed_, packed.Length + 8);
      //byte [] temp = packed;
      //packed = new byte [packed.Length + 8];
      //for (int i = 0; i < temp.Length; i++) {
      //  packed [i] = temp [i];
      //}

      //Getting unpacked file size & allocating space
      long ulen = (long) rnc_ulen (packed_);
      if (ulen < 0) {
        if (ulen == -1) {
          // File wasn't RNC to start with
          return (RNC_UNKNOWN_ERROR);
        }
        //printf("Error: %s\n", rnc_error (ulen));
        return (RNC_UNKNOWN_ERROR);
      }

      //Creating output buffer, 8 bytes are for safety
      unpacked = new byte [ulen];

      //Do the decompression
      int result = rnc_unpack (packed_, unpacked, key, flags);
      if (result != 0) {
        Console.WriteLine ("RNC ERROR: {0}", result);
      }
      return (result);
    }

    public static void WriteTofile (string fileName, byte [] data) {
      Stream ofp = new FileStream (fileName, System.IO.FileMode.CreateNew);
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

    //int copy_file (char *iname, char *oname)
    //{
    //    char *sysbuf;

    //    sysbuf = malloc (strlen (iname)+strlen(oname)+6);
    //    if (!sysbuf)
    //    {
    //  fprintf (stderr, "Out of memory.\n");
    //  return 1;
    //    }
    //    strcpy (sysbuf, "cp ");
    //    strcat (sysbuf, iname);
    //    strcat (sysbuf, " ");
    //    strcat (sysbuf, oname);
    //    system (sysbuf);
    //    free (sysbuf);
    //    return 0;
    //}


    private class bit_stream {
      public ulong bitbuf;	       /* holds between 16 and 32 bits */
      public int bitcount;		       /* how many bits does bitbuf hold? */
    } ;

    private class huf_element {
      public ulong code;
      public int codelen;
      public int value;
    } ;

    private class huf_table {
      public int num;			       /* number of nodes in the tree */
      public huf_element [] table; // [32];
      public huf_table () {
        table = new huf_element [32];
        for (int i = 0; i < 32; i++) {
          table [i] = new huf_element ();
        }
      }
    } ;

    //static void read_huftable (huf_table *h, bit_stream *bs,
    //                   byte **p, byte *pend);
    //static ulong huf_read (huf_table *h, bit_stream *bs,
    //             byte **p,byte *pend);

    //static void bitread_init (bit_stream *bs, byte **p, byte *pend);
    //static void bitread_fix (bit_stream *bs, byte **p, byte *pend);
    //static ulong bit_peek (bit_stream *bs, ulong mask);
    //static void bit_advance (bit_stream *bs, int n,
    //                   byte **p, byte *pend);
    //static ulong bit_read (bit_stream *bs, ulong mask,
    //             int n, byte **p, byte *pend);

    //static ulong blong (byte *p);
    //static ulong llong (byte *p);
    //static ulong bword (byte *p);
    //static ulong lword (byte *p);

    //static ulong mirror (ulong x, int n);

    /*
     * Return an error string corresponding to an error return code.
     */
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

    private static string rnc_error (long errcode) {
      errcode = -errcode;
      if (errcode < 0) {
        errcode = 0;
      }
      if (errcode >= errors.Length) {
        errcode = errors.Length - 1;
      }
      return (errors [errcode]);
    }

    // Return the uncompressed length of a packed data block, or a
    // negative error code.
    private static ulong rnc_ulen (byte [] packed) {
      //  byte *p = packed;
      //  if (blong (p) != RNC_SIGNATURE)
      //return RNC_FILE_IS_NOT_RNC;
      return (blong (packed, 4));
    }

    // Decompress a packed data block. Returns the unpacked length if
    // successful, or negative error codes if not.

    // If COMPRESSOR is defined, it also returns the leeway number
    // (which gets stored at offset 16 into the compressed-file header)
    // in `*leeway', if `leeway' isn't NULL.
    private static int rnc_unpack (byte [] packed, byte [] unpacked, ushort? key, ushort flags) {
      //#ifdef COMPRESSOR
      //     , long *leeway
      //#endif
      //     )
      long input = 0;
      long output = 0;
      long inputend, outputend;
      bit_stream bs = new bit_stream ();
      huf_table raw = new huf_table (), dist = new huf_table (), len = new huf_table ();
      ulong ch_count;
      long ret_len, inp_len;
      ulong out_crc;
      //#ifdef COMPRESSOR
      //    long lee = 0;
      //#endif
      //if (blong (packed, input) != RNC_SIGNATURE) {
      //  if ((flags & RNC_IGNORE_HEADER_SIGNATURE_ERROR) == 0) {
      //    return RNC_HEADER_SIGNATURE_ERROR;
      //  }
      //  else {
      //    for (int i=0; i<unpacked.Length; i++) {
      //      unpacked [i] = packed [i];
      //    }
      //    return (0);
      //  }
      //}
      ret_len = (long) blong (packed, input + 4);
      inp_len = (long) blong (packed, input + 8);
      if ((ret_len > (1 << 30)) || (inp_len > (1 << 30))) {
        return RNC_HEADER_VAL_ERROR;
      }

      outputend = output + ret_len;
      inputend = input + 18 + inp_len;

      input += 18;		       // skip header

      // Check the packed-data CRC. Also save the unpacked-data CRC
      // for later.

      if (rnc_crc (packed, input, inputend - input) != bword (packed, input - 4)) {
        if ((flags & RNC_IGNORE_PACKED_CRC_ERROR) == 0) {
          return RNC_PACKED_CRC_ERROR;
        }
      }
      out_crc = bword (packed, input - 6);

      bitread_init (ref bs, packed, ref input, inputend);
      bit_advance (ref bs, 2, packed, ref input, inputend);      // discard first two bits

      // Process chunks.

      bool hasKey = false;
      ushort keyTemp = 0;
      if (key != null) {
        hasKey = true;
        keyTemp = (ushort) key;
      }
      while (output < outputend) {
        //#ifdef COMPRESSOR
        //    long this_lee;
        //#endif
        if (inputend - input < 6) {
          if ((flags & RNC_IGNORE_HUF_EXCEEDS_RANGE) == 0) {
            return RNC_HUF_EXCEEDS_RANGE;
          }
          else {
            output = outputend;
            ch_count = 0;
            break;
          }
        }
        read_huftable (ref raw, ref bs, packed, ref input, inputend);
        read_huftable (ref dist, ref bs, packed, ref input, inputend);
        read_huftable (ref len, ref bs, packed, ref input, inputend);
        ch_count = bit_read (ref bs, 0xFFFF, 16, packed, ref input, inputend);
        while (true) {
          long length, posn;

          length = (long) huf_read (ref raw, ref bs, packed, ref input, inputend);
          //	    if (length == -1) Tomasz Lis's modification
          if (length < 0) {
            if ((flags & RNC_IGNORE_HUF_DECODE_ERROR) == 0) {
              return RNC_HUF_DECODE_ERROR;
            }
            else {
              output = outputend;
              ch_count = 0;
              break;
            }
          }
          if (length > 0) {
            while (length > 0) {
              length--;
              if ((input >= inputend) || (output >= outputend)) {
                if ((flags & RNC_IGNORE_HUF_EXCEEDS_RANGE) == 0) {
                  return RNC_HUF_EXCEEDS_RANGE;
                }
                else {
                  output = outputend;
                  ch_count = 0;
                  break;
                }
              }
              unpacked [output++] = packed [input++];
              if (hasKey) {
                unpacked [output - 1] ^= (byte) (keyTemp & 0x00FF);
              }
            }
            if (hasKey) {
              keyTemp = (ushort) ((keyTemp >> 1) | ((keyTemp & 1) << 15));
            }
            bitread_fix (ref bs, packed, ref input, inputend);
          }
          if (--ch_count <= 0) {
            break;
          }

          posn = (long) huf_read (ref dist, ref bs, packed, ref input, inputend);
          if (posn == -1) {
            if ((flags & RNC_IGNORE_HUF_DECODE_ERROR) == 0) {
              return RNC_HUF_DECODE_ERROR;
            }
            else {
              output = outputend;
              ch_count = 0;
              break;
            }
          }
          length = (long) huf_read (ref len, ref bs, packed, ref input, inputend);
          //	    if (length == -1) Tomasz Lis's modification
          if (length < 0) {
            if ((flags & RNC_IGNORE_HUF_DECODE_ERROR) == 0) {
              return RNC_HUF_DECODE_ERROR;
            }
            else {
              output = outputend;
              ch_count = 0;
              break;
            }
          }
          posn += 1;
          length += 2;
          while (length > 0) {
            length--;
            if (((output - posn) < 0) || ((output - posn) > outputend) ||
                (output < 0) || (output > outputend)) {
              if ((flags & RNC_IGNORE_HUF_EXCEEDS_RANGE) == 0) {
                return RNC_HUF_EXCEEDS_RANGE;
              }
              else {
                output = outputend - 1;
                ch_count = 0;
                break;
              }
            }
            unpacked [output] = unpacked [output - posn];
            output++;
          }
          //#ifdef COMPRESSOR
          //      this_lee = (inputend - input) - (outputend - output);
          //      if (lee < this_lee)
          //        lee = this_lee;
          //#endif
        }
      }

      if (outputend != output) {
        if ((flags & RNC_IGNORE_FILE_SIZE_MISMATCH) == 0) {
          return RNC_FILE_SIZE_MISMATCH;
        }
      }

      //#ifdef COMPRESSOR
      //    if (leeway)
      //      *leeway = lee;
      //#endif

      // Check the unpacked-data CRC.

      if (rnc_crc (unpacked, outputend - ret_len, ret_len) != out_crc) {
        if ((flags & RNC_IGNORE_UNPACKED_CRC_ERROR) == 0) {
          return RNC_UNPACKED_CRC_ERROR;
        }
      }

      return (0);
    }

    // Read a Huffman table out of the bit stream and data stream given.
    private static void read_huftable (ref huf_table h, ref bit_stream bs, byte [] data, ref long p, long pend) {
      int i, j, k, num;
      int [] leaflen = new int [32];
      int leafmax;
      ulong codeb;	       // big-endian form of code

      num = (int) bit_read (ref bs, 0x1F, 5, data, ref p, pend);
      if (num == 0) {
        return;
      }

      leafmax = 1;
      for (i = 0; i < num; i++) {
        leaflen [i] = (int) bit_read (ref bs, 0x0F, 4, data, ref p, pend);
        if (leafmax < leaflen [i]) {
          leafmax = leaflen [i];
        }
      }

      codeb = 0L;
      k = 0;
      for (i = 1; i <= leafmax; i++) {
        for (j = 0; j < num; j++) {
          if (leaflen [j] == i) {
            h.table [k].code = mirror (codeb, i);
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

    // Read a value out of the bit stream using the given Huffman table.
    private static ulong huf_read (ref huf_table h, ref bit_stream bs, byte [] data, ref long p, long pend) {
      int i;
      ulong val;

      for (i = 0; i < h.num; i++) {
        ulong mask = (ulong) ((1 << h.table [i].codelen) - 1);
        if (bit_peek (bs, mask) == h.table [i].code)
          break;
      }
      if (i == h.num)
        throw new Exception ();
      bit_advance (ref bs, h.table [i].codelen, data, ref p, pend);

      val = (ulong) h.table [i].value;

      if (val >= 2) {
        val = (ulong) (1 << (((int) val) - 1));
        val |= bit_read (ref bs, val - 1, h.table [i].value - 1, data, ref p, pend);
      }
      return (val);
    }

    // Initialises a bit stream with the first two bytes of the packed
    // data.
    // Checks pend for proper buffer pointers range.
    static void bitread_init (ref bit_stream bs, byte [] data, ref long p, long pend) {
      if ((pend - p) >= 0) {
        bs.bitbuf = lword (data, p);
      }
      else {
        bs.bitbuf = 0;
      }
      bs.bitcount = 16;
    }

    // Fixes up a bit stream after literals have been read out of the
    // data stream.
    // Checks pend for proper buffer pointers range.
    private static void bitread_fix (ref bit_stream bs, byte [] data, ref long p, long pend) {
      bs.bitcount -= 16;
      bs.bitbuf &= (ulong) ((1 << bs.bitcount) - 1); // remove the top 16 bits
      if ((pend - p) >= 0) {
        bs.bitbuf |= (lword (data, p) << bs.bitcount);// replace with what's at *p
      }
      bs.bitcount += 16;
    }

    // Returns some bits.
    private static ulong bit_peek (bit_stream bs, ulong mask) {
      return (bs.bitbuf & mask);
    }

    // Advances the bit stream.
    // Checks pend for proper buffer pointers range.
    private static void bit_advance (ref bit_stream bs, int n, byte [] data, ref long p, long pend) {
      bs.bitbuf >>= n;
      bs.bitcount -= n;
      if (bs.bitcount < 16) {
        p += 2;
        if ((pend - p) >= 0) {
          bs.bitbuf |= (lword (data, p) << bs.bitcount);
        }
        bs.bitcount += 16;
      }
    }

    // Reads some bits in one go (ie the above two routines combined).
    private static ulong bit_read (ref bit_stream bs, ulong mask, int n, byte [] data, ref long p, long pend) {
      ulong result = bit_peek (bs, mask);
      bit_advance (ref bs, n, data, ref p, pend);
      return (result);
    }

    // Return the big-endian longword at p.
    private static ulong blong (byte [] p, long offset) {
      ulong n;
      n = p [offset + 0];
      n = (n << 8) + p [offset + 1];
      n = (n << 8) + p [offset + 2];
      n = (n << 8) + p [offset + 3];
      return (n);
    }

    // Return the little-endian longword at p.
    private static ulong llong (byte [] p, long offset) {
      ulong n;
      n = p [offset + 3];
      n = (n << 8) + p [offset + 2];
      n = (n << 8) + p [offset + 1];
      n = (n << 8) + p [offset + 0];
      return (n);
    }

    // Return the big-endian word at p.
    private static ulong bword (byte [] p, long offset) {
      ulong n;
      n = p [offset + 0];
      n = (n << 8) + p [offset + 1];
      return (n);
    }

    // Return the little-endian word at p.
    private static ulong lword (byte [] p, long offset) {
      ulong n;
      n = p [offset + 1];
      n = (n << 8) + p [offset + 0];
      return (n);
    }

    // Mirror the bottom n bits of x.
    private static ulong mirror (ulong x, int n) {
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

    // Calculate a CRC, the RNC way. It re-computes its CRC table every
    // time it's run, but who cares? ;-)
    private static ulong rnc_crc (byte [] data, long p, long len) {
      //ushort [] crctab = new ushort [256];
      ushort val;
      ////computing CRC table
      //for (ushort i = 0; i < 256; i++) {
      //  val = i;

      //  for (int j = 0; j < 8; j++) {
      //    if ((val & 1) == 1) {
      //      val = (ushort) ((val >> 1) ^ 0xA001);
      //    }
      //    else {
      //      val = (ushort) (val >> 1);
      //    }
      //  }
      //  crctab [i] = val;
      //}

      val = 0;
      while (len > 0) {
        len--;
        val ^= data [p++];
        val = (ushort) ((val >> 8) ^ _crctab [val & 0xFF]);
      }

      return (val);
    }
  }
}
