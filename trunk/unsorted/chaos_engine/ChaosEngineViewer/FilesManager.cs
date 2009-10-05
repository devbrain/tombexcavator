using System;
using System.Collections.Generic;
using System.Text;
using System.IO;
using System.Windows.Forms;

namespace ChaosEngineViewer {
  public static class FilesManager {
    private static Dictionary<string, object> _filesDatas = new Dictionary<string, object> ();

    private static ushort? GetKey (VersionEnum version) {
      if ((version == VersionEnum.AmigaCD32) ||
          (version == VersionEnum.AmigaOCS)) {
        return (null);
      }
      else if (version == VersionEnum.AtariST) {
        return (1234);
      }
      else {
        throw new Exception ();
      }
    }

    public static byte [] GetFile (VersionEnum version, string fileName) {
      if (!File.Exists (fileName)) {
        MessageBox.Show (string.Format ("File not found:\n{0}", Path.GetFileName (fileName)), "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
        Environment.Exit (0);
      }
      if (!_filesDatas.ContainsKey (fileName)) {
        LoadFile (version, fileName);
      }
      byte [] unpacked = (byte []) (_filesDatas [fileName]);
      return (unpacked);
    }

    public static byte [] [] GetArchive (VersionEnum version, string fileName) {
      if (!File.Exists (fileName)) {
        MessageBox.Show (string.Format ("File not found:\n{0}", Path.GetFileName (fileName)), "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
        Environment.Exit (0);
      }
      if (!_filesDatas.ContainsKey (fileName)) {
        LoadArchive (version, fileName);
      }
      byte [] [] unpacked = (byte [] []) (_filesDatas [fileName]);
      return (unpacked);
    }

    public static byte [] GetFile (VersionEnum version, string fileName, int? archive) {
      if (archive == null) {
        return (GetFile (version, fileName));
      }
      if (!File.Exists (fileName)) {
        MessageBox.Show (string.Format ("File not found:\n{0}", Path.GetFileName (fileName)), "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
        Environment.Exit (0);
      }
      if (!_filesDatas.ContainsKey (fileName)) {
        LoadArchive (version, fileName);
      }
      byte [] [] unpacked = (byte [] []) (_filesDatas [fileName]);
      return (unpacked [(int) archive]);
    }

    private static void LoadFile (VersionEnum version, string fileName) {
      Console.WriteLine (string.Format ("Loading file: {0}", fileName));
      byte [] unpacked;
      int result = RNC.Decompress (fileName, out unpacked, GetKey (version));
      //if (result != 0) {
      //  Console.WriteLine (result);
      //}
      _filesDatas [fileName] = unpacked;
    }

    private static void LoadArchive (VersionEnum version, string fileName) {
      Console.WriteLine (string.Format ("Loading file multi: {0}", fileName));
      byte [] [] unpacked;
      int result = RNC.DecompressMulti (fileName, out unpacked, GetKey (version));
      _filesDatas [fileName] = unpacked;
    }
  }
}
