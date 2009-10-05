using System;
using System.Collections.Generic;
using System.Text;
using System.IO;
using ChaosEngineViewer.Properties;

namespace ChaosEngineViewer.LEVSDAT_BIN {
  public static class LevsdatBin {
    private static Dump [] dump;

    private static Header [] header;
    public static Block00_Levels [] block00_Levels;
    public static Block16_ScriptsClasses [] block16_ScriptsClasses;

    static LevsdatBin () {
      string fileName0 = Path.Combine (Settings.Default.BaseFolderCD32, "LEVSDAT.BIN");
      string fileName1 = Path.Combine (Settings.Default.BaseFolderOCS, "LEVSDAT.BIN");
      string fileName2 = Path.Combine (Settings.Default.BaseFolderST, "LEVSDAT.BIN");
      dump = new Dump [3];
      dump [(int) VersionEnum.AmigaCD32] = new Dump (FilesManager.GetFile (VersionEnum.AmigaCD32, fileName0));
      dump [(int) VersionEnum.AmigaOCS] = new Dump (FilesManager.GetFile (VersionEnum.AmigaOCS, fileName1));
      dump [(int) VersionEnum.AtariST] = new Dump (FilesManager.GetFile (VersionEnum.AtariST, fileName2));

      header = new Header [3];
      header [(int) VersionEnum.AmigaCD32] = new Header (dump [(int) VersionEnum.AmigaCD32]);
      header [(int) VersionEnum.AmigaOCS] = new Header (dump [(int) VersionEnum.AmigaOCS]);
      header [(int) VersionEnum.AtariST] = new Header (dump [(int) VersionEnum.AtariST]);

      block00_Levels = new Block00_Levels [3];
      block00_Levels [(int) VersionEnum.AmigaCD32] = new Block00_Levels (dump [(int) VersionEnum.AmigaCD32], header [(int) VersionEnum.AmigaCD32].addressBlock00);
      block00_Levels [(int) VersionEnum.AmigaOCS] = new Block00_Levels (dump [(int) VersionEnum.AmigaOCS], header [(int) VersionEnum.AmigaOCS].addressBlock00);
      block00_Levels [(int) VersionEnum.AtariST] = new Block00_Levels (dump [(int) VersionEnum.AtariST], header [(int) VersionEnum.AtariST].addressBlock00);

      block16_ScriptsClasses = new Block16_ScriptsClasses [3];
      block16_ScriptsClasses [(int) VersionEnum.AmigaCD32] = new Block16_ScriptsClasses (dump [(int) VersionEnum.AmigaCD32], header [(int) VersionEnum.AmigaCD32].addressBlock16);
      block16_ScriptsClasses [(int) VersionEnum.AmigaOCS] = new Block16_ScriptsClasses (dump [(int) VersionEnum.AmigaOCS], header [(int) VersionEnum.AmigaOCS].addressBlock16);
      block16_ScriptsClasses [(int) VersionEnum.AtariST] = new Block16_ScriptsClasses (dump [(int) VersionEnum.AtariST], header [(int) VersionEnum.AtariST].addressBlock16);
    }
  }
}
