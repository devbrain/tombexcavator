using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using System.Threading;
using System.Drawing.Text;
using ChaosEngineViewer.X1CHAOS_CAS;
using ChaosEngineViewer.ACHAOS;
using System.IO;
using System.Reflection;
using ChaosEngineViewer.Properties;
using System.Drawing.Imaging;
using System.Drawing.Drawing2D;

namespace ChaosEngineViewer {
  public partial class FormMain : Form {
    private X1chaosCas _x1chaosCas;
    private SpriteBin _spriteBin = null;
    private SpriteBin _spriteBin2 = null;
    //private Palette _palette = null;
    //private Bitmap [] _bitmaps;
    private static Brush _paletteBrush = new HatchBrush (HatchStyle.Percent60, Color.Transparent, Color.White);
    private static Brush _backgroundBrush = new HatchBrush (HatchStyle.Percent50, Color.Black, Color.LightGray);

    //private LEVXBAK_BIN _ITEMS_BIN1;
    //private LEVXBAK_BIN _ITEMS_BIN2;
    //private LEVXBAK_BIN _ITEMS_BIN3;
    //private LEVXBAK_BIN _ITEMS_BIN4;

    private int _index1 = 0;
    private int _modulo1 = 100;
    private int _index2 = 0;
    private int _modulo2 = 75;
    private int _paletteOffset = 0;
    private static Font _font;
    //private Palette _palette;
    private int _levelIndex;
    private bool _internal = false;
    //byte [] [] _unpacked;

    //private int _x = 0;
    //private int _y = 0;
    private StringDrawer _stringDrawer = new StringDrawer ();

    public FormMain () {
      InitializeComponent ();

      if (false) {
        //int result2 = RNC.main_unpack ("D:/Archivable/Reverse Engineering/Chaos Engine/Chaos Engine - ST/BLOB.BIN", "D:/Archivable/Reverse Engineering/Chaos Engine/Chaos Engine - ST/BLOB_.BIN");
        //Environment.Exit (0);
        //int result2 = RNC.Decompress ("D:/Archivable/Reverse Engineering/Chaos Engine/Chaos Engine - CD32/LEVSDAT.BIN", "D:/Archivable/Reverse Engineering/Chaos Engine/Chaos Engine - ST/LEVSDAT_CD32.BIN");
        //Environment.Exit (0);
        string srcDir = "D:/Archivable/Reverse Engineering/Chaos Engine/Chaos Engine - ST/";
        string dstDir = Path.Combine (srcDir, "DERNC");
        if (!Directory.Exists (dstDir)) {
          Directory.CreateDirectory (dstDir);
        }
        DirectoryInfo di = new DirectoryInfo (srcDir);
        foreach (FileInfo fi in di.GetFiles ("*.*")) {
          //byte [] file = FilesManager.GetFile (fi.Name);
          string dstFile = Path.Combine (dstDir, fi.Name);
          //Console.WriteLine (dstFile);
          ushort? key = 1234;
          if (fi.Name == "INTRO.BIN.RNC") {
            key = null;
          }

          int result = RNC.Decompress (fi.FullName, dstFile, key);
          if ((result != 0) && (result != -9)) {
            Console.WriteLine ("Error ({0}) while unpacking {1}", result, fi.Name);
            Environment.Exit (0);
          }
          //RNC.WriteTofile (dstFile, file);
        }
        //string fileName = Path.Combine (Settings.Default.BaseFolder, "GROUP.PIC");
        Environment.Exit (0);
      }
      if (false) {
        byte [] [] unpacked;
        int result = RNC.DecompressMulti ("D:/Archivable/Reverse Engineering/Chaos Engine/Chaos Engine - OCS/DERNC/SNGARC2.BIN", out unpacked);

        for (int i = 0; i < unpacked.Length; i++) {
          RNC.WriteTofile (string.Format ("D:/Archivable/Reverse Engineering/Chaos Engine/Chaos Engine - OCS/DERNC/SNGARC2.BIN.{0,3:D3}", i), unpacked [i]);
        }
        Environment.Exit (0);
      }

      Top = Properties.Settings.Default.FormTop;
      Left = Properties.Settings.Default.FormLeft;
      Width = Properties.Settings.Default.FormWidth;
      Height = Properties.Settings.Default.FormHeight;
      WindowState = Properties.Settings.Default.FormWindowState;

      //try {
      //  if (Settings.Default.FormLocation != null) {
      //    Location = Properties.Settings.Default.FormLocation;
      //  }
      //  if (Settings.Default.FormSize != null) {
      //    Size = Properties.Settings.Default.FormSize;
      //  }
      //}
      //catch {
      //}



      //PrivateFontCollection pfc = new PrivateFontCollection();
      //pfc.AddFontFile ("slkscr.ttf");
      //FontFamily ff = pfc.Families [0];
      _font = new Font ("Arial", 6.5f, FontStyle.Regular);

      //RNC.main_unpack ("D:\\Archivable\\Reverse Engineering\\Chaos Engine\\Chaos Engine\\ChaosEngineCD32\\data\\PANEL.BIN_",
      //                 "D:\\Archivable\\Reverse Engineering\\Chaos Engine\\Chaos Engine\\ChaosEngineCD32\\data\\PANEL.BIN__");
      //byte [] [] unpacked;
      //int result = RNC.main_unpack_multi ("D:\\Archivable\\Reverse Engineering\\Chaos Engine\\Chaos Engine\\ChaosEngineCD32\\data\\SNGARC.BIN", out unpacked);

      //for (int i = 0; i < unpacked.Length; i++) {
      //  RNC.WriteTofile (string.Format ("D:\\Archivable\\Reverse Engineering\\Chaos Engine\\Chaos Engine\\ChaosEngineCD32\\data\\TEST.BIN.{0,3:D3}", i), unpacked [i]);
      //}

      //_dump_ACHAOS = new Dump (Properties.Resources.ACHAOS, 32);
      if (false) {
        ChooseBaseFolderCD32 ();
        ChooseBaseFolderST ();
      }
      else {
        string baseDataPath = Path.Combine (Path.GetDirectoryName (Application.ExecutablePath), "data");
        Properties.Settings.Default.BaseFolderCD32 = Path.Combine (baseDataPath, "AmigaCD32");
        Properties.Settings.Default.BaseFolderOCS = Path.Combine (baseDataPath, "AmigaOCS");
        Properties.Settings.Default.BaseFolderST = Path.Combine (baseDataPath, "AtariST");
        Properties.Settings.Default.Save ();
      }
      //_dump_ACHAOS = new Dump (string.Format ("{0}\\ACHAOS", _baseFolder), 32);

      //_GETREADY_BIN = new SPRITES_BIN (0x17FFA, _dump_ACHAOS, _paletteIndex, _paletteOffset);

      //int result = RNC.main_unpack_multi ("D:\\Archivable\\Reverse Engineering\\Chaos Engine\\Chaos Engine\\ChaosEngineCD32\\data\\SNGARC.BIN", out _unpacked);
      //_ITEMS_BIN1 = new LEVXBAK_BIN (new Dump (_unpacked [23]), _palette, _paletteOffset);
      //_ITEMS_BIN2 = new LEVXBAK_BIN (new Dump (_unpacked [24]), _palette, _paletteOffset);
      //_ITEMS_BIN3 = new LEVXBAK_BIN (new Dump (_unpacked [25]), _palette, _paletteOffset);
      //_ITEMS_BIN4 = new LEVXBAK_BIN (new Dump (_unpacked [26]), _palette, _paletteOffset);

      //SetCurrentLevel (Properties.Settings.Default.LevelIndex);
      _UI_chaosEngineVersion.SelectedIndex = Properties.Settings.Default.ChaosEngineVersion;
      _UI_menuSaveAllAsPNG.Enabled = false;
      _internal = true;
      _UI_showHints.Checked = Properties.Settings.Default.ShowHints;
      _UI_levelIndex.Value = Properties.Settings.Default.LevelIndex;
      _UI_spacing.Value = Properties.Settings.Default.Spacing;
      LoadLevel (Properties.Settings.Default.LevelIndex);

      //_spriteBin = SpritesManager.GetSpriteBin (_index, _x1chaosCas._levelIndex, _x1chaosCas._mapPalette);

      //Console.WriteLine ("*****");
      //int i = 13;
      //foreach (SpriteDef spriteDef in Achaos._spriteDefs) {
      //  if (spriteDef.spriteFileDefAddress != 0) {
      //    Console.WriteLine (string.Format ("{0}\t0x{1:X}\t{2}\t0x{3:X}", spriteDef.SpriteFileDef.fileName, spriteDef.spriteFileDefAddress, Enum.GetName (typeof (SpritesManager.SpritesEnum), i), SpritesManager._spriteFileDefAddresses [i]));
      //    i++;
      //  }
      //}

      //string fileNameMulti = Path.Combine (Settings.Default.BaseFolder, "SNGARC.BIN");
      //byte [] [] sngarc_bin = FilesManager.GetFileMulti (fileNameMulti);
      //RNC.WriteTofileMulti (Path.Combine (Settings.Default.BaseFolder, "SNGARC.{0:D3}.BIN"), sngarc_bin);

      if (false) {
        _spriteBin = SpritesManager.GetSpriteBinA (0, SpritesManager.SpritesEnum.getready_bin, _x1chaosCas._levelIndex, _x1chaosCas._paletteIndex);
        //_spriteBin2 = SpritesManager.CreateBasedOnExisting (SpritesManager.SpritesEnum.sp8x8_bin, _x1chaosCas._levelIndex, _x1chaosCas._paletteIndex, "SP8X8.BIN", 10 * 16);
        //_spriteBin = SpritesManager.GetPanelNumbersSmall (_x1chaosCas._levelIndex, _x1chaosCas._paletteIndex);
        //_spriteBin = SpritesManager.GetPanelBackgrounds (_x1chaosCas._levelIndex, _x1chaosCas._paletteIndex);
        _spriteBin2 = CustomSpritesManager.GetCustomSprite ((VersionEnum) _UI_chaosEngineVersion.SelectedIndex, "PanelPowers");
      }
      //CustomSpritesManager.PreLoad ();


      SetButtonTextAndColor (_UI_border, SpriteBin.BorderColor);
      //SetButtonTextAndColor (_UI_background, SpriteBin.BackgroundColor);
      SetButtonTextAndColor (_UI_keyed, SpriteBin.KeyedColor);

      _UI_showInfos.Checked = SpriteBin.ShowInfos;

      _internal = false;

      SetTitle ();
    }

    public static string Version {
      get {
        return ("1.03");
      }
    }

    private void SetTitle () {
      //Text = string.Format ("Chaos Engine Utility - v{0}", FormMain.Version);
      StringBuilder sb = new StringBuilder ();
      Text = string.Format ("Chaos Engine Viewer - v{0} - {1}", FormMain.Version, Path.GetFileName (_x1chaosCas._fileName));
    }

    private void ChooseBaseFolderCD32 () {
      string baseFolder = Settings.Default.BaseFolderCD32;

      if (CheckBaseFolderCD32 (baseFolder) != 0) {
        FolderBrowserDialog ofd = new FolderBrowserDialog ();
        ofd.SelectedPath = Path.GetDirectoryName (Application.ExecutablePath);
        ofd.Description = "Select the folder containing the CD32 version of Chaos Engine";
        ofd.ShowNewFolderButton = false;
        DialogResult dg = ofd.ShowDialog ();
        if (dg == DialogResult.OK) {
          baseFolder = ofd.SelectedPath;
        }
        else {
          Environment.Exit (0);
        }
        ofd.Dispose ();
        this.BringToFront ();
        if (CheckBaseFolderCD32 (baseFolder) != 0) {
          Environment.Exit (0);
        }
      }
      Properties.Settings.Default.BaseFolderCD32 = baseFolder;
      Properties.Settings.Default.Save ();
    }

    private void ChooseBaseFolderST () {
      string baseFolder = Settings.Default.BaseFolderST;

      if (CheckBaseFolderST (baseFolder) != 0) {
        FolderBrowserDialog ofd = new FolderBrowserDialog ();
        ofd.SelectedPath = Path.GetDirectoryName (Application.ExecutablePath);
        ofd.Description = "Select the folder containing the ST version of Chaos Engine";
        ofd.ShowNewFolderButton = false;
        DialogResult dg = ofd.ShowDialog ();
        if (dg == DialogResult.OK) {
          baseFolder = ofd.SelectedPath;
        }
        else {
          Environment.Exit (0);
        }
        ofd.Dispose ();
        this.BringToFront ();
        if (CheckBaseFolderST (baseFolder) != 0) {
          Environment.Exit (0);
        }
      }
      Properties.Settings.Default.BaseFolderST = baseFolder;
      Properties.Settings.Default.Save ();
    }

    private int CheckBaseFolderCD32 (string baseFolder) {
      //if (baseFolder == "") {
      //  return (false);
      //}
      if (!File.Exists (Path.Combine (baseFolder, "ACHAOS"))) {
        MessageBox.Show ("Incorrect Chaos Engine CD32 folder", "", MessageBoxButtons.OK, MessageBoxIcon.Warning);
        return (-1);
      }
      Dump dump = new Dump (string.Format ("{0}\\ACHAOS", baseFolder), 0);

      UInt32 crc = CRC32.Calculate (dump.GetBytes ());
      //Console.WriteLine ("{0:X}", crc);
      if (crc != 0xACAF863D) {
        MessageBox.Show ("Incorrect CD32 version of Chaos Engine");
        return (-2);
      }
      return (0);
    }

    private int CheckBaseFolderST (string baseFolder) {
      //if (baseFolder == "") {
      //  return (false);
      //}
      if (!File.Exists (Path.Combine (baseFolder, "CHAOS.BIN"))) {
        MessageBox.Show ("Incorrect Chaos Engine ST folder", "", MessageBoxButtons.OK, MessageBoxIcon.Warning);
        return (-1);
      }
      Dump dump = new Dump (string.Format ("{0}\\CHAOS.BIN", baseFolder), 0);

      UInt32 crc = CRC32.Calculate (dump.GetBytes ());
      //Console.WriteLine ("{0:X}", crc);
      if (crc != 0x747F98A4) {
        MessageBox.Show ("Incorrect CD32 version of Chaos Engine");
        return (-2);
      }
      return (0);
    }

    //bool _resize = false;
    private void MapChanged () {
      //_UI_panel.Text = _X1CHAOS_CAS.GetDescription ();
      _UI_ds.Invalidate ();
    }

    private void LoadLevel (int level) {
      _levelIndex = (level + 16) % 16;
      //string fileName = string.Format ("{0}1CHAOS.CAS", (char) ('A' + _currentLevel));
      if (_x1chaosCas != null) {
        _x1chaosCas._gridMap.Dispose ();
      }
      _x1chaosCas = new X1chaosCas ((VersionEnum) _UI_chaosEngineVersion.SelectedIndex, _levelIndex);
      _x1chaosCas._gridMap.MapChangedEvent += new GridMap.MapChangedHandler (MapChanged);
      _x1chaosCas.ShowHints = _UI_showHints.Checked;
      _x1chaosCas._gridMap.Spacing = (int) _UI_spacing.Value;

      SetTitle ();
      string levelNumber = string.Format ("Level ({0}.{1}):", _levelIndex / 4 + 1, _levelIndex % 4 + 1);
      _UI_LevelLabel.Text = levelNumber;

      //MapChanged ();
      _UI_panel.Text = _x1chaosCas.Description ();
      _x1chaosCas._gridMap.RedrawMapWait ();
      _UI_ds.AutoScrollMinSize = new Size (_x1chaosCas.Bitmap.Width, _x1chaosCas.Bitmap.Height);
      _UI_ds.Select ();

      //SpritesManager.Dump ();
    }

    private void _UI_ds_Paint (object sender, PaintEventArgs e) {
      //if (_resize) {
      //  _UI_ds.AutoScrollMinSize = new Size (_x1chaosCas.Bitmap.Width, _x1chaosCas.Bitmap.Height);
      //}

      //if ((e.ClipRectangle.Width != _UI_ds.ClientRectangle.Width) || (e.ClipRectangle.Height != _UI_ds.ClientRectangle.Height)) {
      //  _UI_ds.Invalidate ();
      //  return;
      //}
      //bool showPalette = true;
      Graphics g = e.Graphics;
      //g.Clear (Color.Black);
      //int x = 0;
      //int y = 0;
      //int i = 0;
      //while (i < _LEVXBAK_BIN.Bitmaps.Length) {
      //  g.DrawImage (_LEVXBAK_BIN.Bitmaps [i], x, y);
      //  x += 16;
      //  if ((x + 16) > _UI_ds.Width) {
      //    x = 0;
      //    y += 16;
      //  }
      //  i++;
      //}
      //if (showPalette) {
      //  g.DrawString (string.Format ("#: {0}  Size: {1}", _paletteIndex.ToString (), _palette.Size), _font, Brushes.White, 0, y3);
      //}
      g.TranslateTransform (_UI_ds.AutoScrollPosition.X, _UI_ds.AutoScrollPosition.Y);
      //g.DrawImage (_X1CHAOS_CAS.Bitmap, _UI_ds.AutoScrollPosition.X, _UI_ds.AutoScrollPosition.Y);
      g.DrawImage (_x1chaosCas.Bitmap, 0, 0);

      //_X1CHAOS_CAS.DrawRectangle (g, Pens.White, _x, _y, 1, 1);
      //g.TranslateTransform (-_UI_ds.AutoScrollPosition.X, -_UI_ds.AutoScrollPosition.Y);
      //g.DrawImage (_ITEMS_BIN1.Bitmap, 0, 40 * 0);
      //g.DrawImage (_ITEMS_BIN2.Bitmap, 0, 40 * 1);
      //g.DrawImage (_ITEMS_BIN3.Bitmap, 0, 40 * 2);
      //g.DrawImage (_ITEMS_BIN4.Bitmap, 0, 40 * 3);

      //g.DrawImage (_X1CHAOS_CAS._LEVXBAK_BIN.Bitmap, 0, 0);
      //if (_X1CHAOS_CAS._LEV4BAK_BIN != null) {
      //  g.DrawImage (_X1CHAOS_CAS._LEV4BAK_BIN.Bitmap, 0, _X1CHAOS_CAS._LEVXBAK_BIN.Bitmap.Height+16);
      //}
      //if (false) {
      //  //g.Clear (Color.Black);
      //  //if (CustomSpritesManager.CustomSpritesManager.NbCustomSprites != 0) {
      //  int i = _index1 % CustomSpritesManager.GetNbCustomSprites (_UI_chaosEngineVersion.SelectedIndex);
      //  CustomSpriteInfo info = CustomSpritesManager.GetCustomSpriteInfo (_UI_chaosEngineVersion.SelectedIndex, i);
      //  SpriteBin s1 = CustomSpritesManager.GetCustomSprite (_UI_chaosEngineVersion.SelectedIndex, i);
      //  //SpriteBin s1 = CustomSpritesManager.GetCustomSprite ("Interleaved ST.gfx");
      //  //SpriteBin s2 = CustomSpritesManager.GetCustomSprite ("am_equip");
      //  //SpriteBin s3 = CustomSpritesManager.GetCustomSprite ("am_dead3");
      //  _stringDrawer.SetTextAlignment (StringAlignment.Near, StringAlignment.Near);
      //  _stringDrawer.DrawString (g, Brushes.White, string.Format ("{0} {1} ({2})", info._id, info._com, info._file), 0, 0);
      //  g.DrawImage (s1.Bitmap, 0, 8);
      //  //g.DrawImage (s2.Bitmap, s1.Bitmap.Width, 0);
      //  //g.DrawImage (s3.Bitmap, s1.Bitmap.Width+s2.Bitmap.Width, 0);
      //  //}
      //  if (_palette != null) {
      //    _stringDrawer.DrawString (g, Brushes.White, string.Format ("{0}", info._palIndex), 0, 8 + s1.Bitmap.Height);
      //    g.DrawImage (_palette.Bitmap, 0, 16 + s1.Bitmap.Height);
      //  }
      //}

      if (false) {
        if (_spriteBin != null) {
          g.Clear (Color.Black);
          g.DrawImage (_spriteBin.Bitmap, 0, 0);
          if (_spriteBin2 != null) {
            g.DrawImage (_spriteBin2.Bitmap, _spriteBin.Bitmap.Width + 8, 0);
          }
        }
      }
      //if (false) {
      //  if (_bitmaps != null) {
      //    g.Clear (Color.Black);
      //    int x = 0;
      //    foreach (Bitmap bitmap in _bitmaps) {
      //      g.DrawImage (bitmap, x, 0);
      //      x += bitmap.Width + 1;
      //    }
      //  }
      //}

      // heroes head
      //int x = 0;
      //foreach (KeyValuePair<string, Bitmap> kvp in SpritesManager.GetHeroesHead (_x1chaosCas._paletteIndex)) {
      //  g.DrawImage (kvp.Value, x, 0);
      //  x += 24;
      //}


      //g.DrawImage (SpritesManager.GetSpriteBin (SpritesManager.SpritesEnum.lizman_bin, _x1chaosCas._mapPalette).Bitmap, 0, 0);

      //Palette palette = Achaos.GetPalette (_X1CHAOS_CAS._mapPalette);
      //g.DrawImage (palette.Bitmap, _UI_ds.Width - palette.Bitmap.Width, _UI_ds.Height - palette.Bitmap.Height);
      //g.DrawString (string.Format ("Palette: {0}  Offset: {1}  PaletteLevel: {2}", _paletteIndex.ToString (), _paletteOffset.ToString (), _X1CHAOS_CAS._mapPalette), _font, Brushes.White, 0, _UI_ds.Height - 16);
    }

    private void _UI_ds_Resize (object sender, EventArgs e) {
      _UI_ds.Invalidate ();
    }

    private void _UI_ds_MouseUp (object sender, MouseEventArgs e) {
      //_modulo1 = CustomSpritesManager.GetNbCustomSprites ((VersionEnum) _UI_chaosEngineVersion.SelectedIndex);
      //if (e.Button == MouseButtons.Left) {
      //  //_index1 = (_index1 + 1 + _modulo1) % _modulo1;
      //  //_index2 = 0;

      //  //_paletteOffset = 0;
      //  //_palette = new Palette (_dump_ACHAOS, _index);
      //  _paletteOffset = (_paletteOffset + 16 + _x1chaosCas._palette.Size) % _x1chaosCas._palette.Size;
      //}
      //else {
      //  _index1 = (_index1 - 1 + _modulo1) % _modulo1;
      //  _paletteOffset = (_paletteOffset - 16 + _x1chaosCas._palette.Size) % _x1chaosCas._palette.Size;
      //}
      //int i = _index1 % CustomSpritesManager.GetNbCustomSprites (_UI_chaosEngineVersion.SelectedIndex);
      //CustomSpriteInfo info = CustomSpritesManager.GetCustomSpriteInfo (_UI_chaosEngineVersion.SelectedIndex, i);
      //_palette = new Palette (info._palIndex);
      //_spriteBin = SpritesManager.GetSpriteBinB (SpritesManager.SpritesEnum.brigand_bin, _x1chaosCas._paletteIndex, _paletteOffset);
      //CustomSpritesManager.PreLoad ();
      //SpriteDef spriteDef = new SpriteDef (_index);
      //_spriteBin = spriteDef.GetSpriteBin (_x1chaosCas._levelIndex, _x1chaosCas._mapPalette);
      //Console.WriteLine (spriteDef.SpriteFileDef.fileName);
      //_SPRITES_BIN = new SPRITES_BIN (0x18088, _dump_ACHAOS, _paletteIndex, _paletteOffset);
      //_ITEMS_BIN1 = new LEVXBAK_BIN (new Dump (_unpacked [23]), _palette, _paletteOffset);
      //_ITEMS_BIN2 = new LEVXBAK_BIN (new Dump (_unpacked [24]), _palette, _paletteOffset);
      //_ITEMS_BIN3 = new LEVXBAK_BIN (new Dump (_unpacked [25]), _palette, _paletteOffset);
      //_ITEMS_BIN4 = new LEVXBAK_BIN (new Dump (_unpacked [26]), _palette, _paletteOffset);
      _x1chaosCas.ClickMap (e.X - _UI_ds.AutoScrollPosition.X, e.Y - _UI_ds.AutoScrollPosition.Y, e.Button);
      _UI_ds.Invalidate ();
    }

    private void FormMain_KeyDown (object sender, KeyEventArgs e) {
      if (e.KeyData == Keys.Add) {
        _index2 = (_index2 + 1 + _modulo2) % _modulo2;
        //_UI_levelIndex.UpButton ();
      }
      else if (e.KeyData == Keys.Subtract) {
        _index2 = (_index2 - 1 + _modulo2) % _modulo2;
        //_UI_levelIndex.DownButton ();
      }
      _UI_ds.Invalidate ();
    }

    //private bool _isDrawing = false;
    private void _UI_ds_MouseMove (object sender, MouseEventArgs e) {
      //if (!_isDrawing) {
      //  _isDrawing = true;
      _x1chaosCas.SetSelectedTile (e.X - _UI_ds.AutoScrollPosition.X, e.Y - _UI_ds.AutoScrollPosition.Y);
      //_X1CHAOS_CAS.RedrawMap ();
      //panel1.Text = _x1chaosCas.GetDescription ();
      _UI_panel.Text = _x1chaosCas.Description ();
      //  _isDrawing = false;
      //}
      //_UI_ds.Invalidate ();
    }

    private void FormMain_Load (object sender, EventArgs e) {
      _UI_ds.Select ();
      Activate ();
    }

    private void FormMain_FormClosed (object sender, FormClosedEventArgs e) {
      Properties.Settings.Default.FormTop = Top;
      Properties.Settings.Default.FormLeft = Left;
      Properties.Settings.Default.FormWidth = Width;
      Properties.Settings.Default.FormHeight = Height;
      Properties.Settings.Default.FormWindowState = WindowState;
      Properties.Settings.Default.LevelIndex = _levelIndex;
      Properties.Settings.Default.ShowHints = _UI_showHints.Checked;
      Properties.Settings.Default.Spacing = _x1chaosCas._gridMap.Spacing;

      Properties.Settings.Default.ShowInfos = SpriteBin.ShowInfos;
      Properties.Settings.Default.BorderColor = SpriteBin.BorderColor;
      Properties.Settings.Default.BackgroundColor = SpriteBin.BackgroundColor;
      Properties.Settings.Default.KeyedColor = SpriteBin.KeyedColor;

      Properties.Settings.Default.ChaosEngineVersion = _UI_chaosEngineVersion.SelectedIndex;

      Settings.Default.Save ();
      System.Environment.Exit (0);
    }

    private void _UI_menuSaveAsPNG_Click (object sender, EventArgs e) {
      SaveFileDialog sfd = new SaveFileDialog ();
      sfd.DefaultExt = "png";
      sfd.AddExtension = true;
      sfd.CheckPathExists = true;
      sfd.Filter = "PNG files (*.png)|*.png";
      sfd.OverwritePrompt = true;

      DialogResult dg = sfd.ShowDialog ();
      if (dg == DialogResult.OK) {
        //_x1chaosCas._isSaving = true;
        //_x1chaosCas._gridMap.RedrawMapWait ();
        //_x1chaosCas._gridMap.RedrawMapWait ();
        if (_UI_tabs.SelectedIndex == 0) {
          _x1chaosCas._gridMap.BitmapBase.Save (sfd.FileName, ImageFormat.Png);
        }
        else {
          CustomSpriteInfo info = (CustomSpriteInfo) _UI_customSpriteCombo.SelectedItem;
          CustomSpritesManager.GetCustomSprite ((VersionEnum) _UI_chaosEngineVersion.SelectedIndex, info._id).Bitmap.Save (sfd.FileName, ImageFormat.Png);
        }
        //_x1chaosCas._isSaving = false;
        //_x1chaosCas._gridMap.RedrawMapWait ();
      }
    }

    private void _UI_menuSaveAllAsPNG_Click (object sender, EventArgs e) {
      FolderBrowserDialog ofd = new FolderBrowserDialog ();
      ofd.SelectedPath = Path.GetDirectoryName (Application.ExecutablePath);
      ofd.Description = "Select the folder where the sprites will be saved";
      ofd.ShowNewFolderButton = true;
      DialogResult dg = ofd.ShowDialog ();
      if (dg == DialogResult.OK) {
        foreach (CustomSpriteInfo info in _UI_customSpriteCombo.Items) {
          string fileName = Path.Combine (ofd.SelectedPath, String.Format ("{0}.png", info.Id));
          CustomSpritesManager.GetCustomSprite ((VersionEnum) _UI_chaosEngineVersion.SelectedIndex, info._id).Bitmap.Save (fileName, ImageFormat.Png);
        }
      }
    }

    private void _UI_menuExit_Click (object sender, EventArgs e) {
      Close ();
    }

    private void _UI_levelIndex_ValueChanged (object sender, EventArgs e) {
      if (_internal) {
        return;
      }
      _internal = true;
      int levelIndex = (((int) _UI_levelIndex.Value) + 16) % 16;
      _UI_levelIndex.Value = levelIndex;
      _internal = false;
      LoadLevel (levelIndex);
    }

    private void _UI_showHints_CheckedChanged (object sender, EventArgs e) {
      if (_internal) {
        return;
      }
      _x1chaosCas.ShowHints = _UI_showHints.Checked;
      _x1chaosCas._gridMap.RedrawMap ();
    }

    private void _UI_spacing_ValueChanged (object sender, EventArgs e) {
      if (_internal) {
        return;
      }
      _x1chaosCas._gridMap.Spacing = (int) _UI_spacing.Value;
      _x1chaosCas._gridMap.RedrawMapWait ();
      _UI_ds.AutoScrollMinSize = new Size (_x1chaosCas.Bitmap.Width, _x1chaosCas.Bitmap.Height);
    }

    private void _UI_customSpriteCombo_SelectedIndexChanged (object sender, EventArgs e) {
      CustomSpriteInfo info = (CustomSpriteInfo) _UI_customSpriteCombo.SelectedItem;
      if (info == null) {
        return;
      }
      _internal = true;
      _UI_mask.SelectedIndex = (int) info._mask;
      _UI_paletteIndex.Value = info._palIndex;
      _UI_paletteOffsetX16.Value = info._palOffsetX16;
      _UI_cols.Value = info._cols;
      _internal = false;

      _UI_dsSpriteViewer.Invalidate ();
    }

    private void _UI_dsSpriteViewer_Paint (object sender, PaintEventArgs e) {
      //e.Graphics.Clear (Color.Purple);
      e.Graphics.FillRectangle (_backgroundBrush, 0, 0, _UI_dsSpriteViewer.Width, _UI_dsSpriteViewer.Height);
      CustomSpriteInfo info = (CustomSpriteInfo) _UI_customSpriteCombo.SelectedItem;
      SpriteBin spriteBin = CustomSpritesManager.GetCustomSprite ((VersionEnum) _UI_chaosEngineVersion.SelectedIndex, info.Id);
      e.Graphics.DrawImage (spriteBin.Bitmap, 0, 0);

      Palette palette = new Palette ((VersionEnum) _UI_chaosEngineVersion.SelectedIndex, info._palIndex);
      //_stringDrawer.DrawString (g, Brushes.White, string.Format ("{0}", info._palIndex), 0, 8 + s1.Bitmap.Height);
      int x = _UI_dsSpriteViewer.Width - palette.Bitmap.Width;
      //e.Graphics.FillRectangle (Brushes.Black, x, 0, _UI_dsSpriteViewer.Width, _UI_dsSpriteViewer.Height);
      e.Graphics.DrawImage (palette.Bitmap, x, 0);
      e.Graphics.FillRectangle (_paletteBrush, x, 0, palette.Bitmap.Width, info._palOffsetX16 * 17);
    }

    private void _UI_mask_SelectedIndexChanged (object sender, EventArgs e) {
      if (_internal) {
        return;
      }
      CustomSpriteInfo info = (CustomSpriteInfo) _UI_customSpriteCombo.SelectedItem;
      info._mask = (SpriteBin.MasksEnum) _UI_mask.SelectedIndex;
      CustomSpritesManager.LoadCustomSprite ((VersionEnum) _UI_chaosEngineVersion.SelectedIndex, info);
      _UI_dsSpriteViewer.Invalidate ();
    }

    private void _UI_border_Click (object sender, EventArgs e) {
      ColorDialog cd = new ColorDialog ();
      cd.AllowFullOpen = true;
      cd.AnyColor = true;
      cd.Color = SpriteBin.BorderColor;
      cd.FullOpen = true;
      if (cd.ShowDialog () == DialogResult.OK) {
        SpriteBin.BorderColor = cd.Color;
        SetButtonTextAndColor (_UI_border, SpriteBin.BorderColor);
        CustomSpritesManager.ClearCache ();
        _UI_dsSpriteViewer.Invalidate ();
      }
    }

    //private void _UI_background_Click (object sender, EventArgs e) {
    //  ColorDialog cd = new ColorDialog ();
    //  cd.AllowFullOpen = true;
    //  cd.AnyColor = true;
    //  cd.Color = SpriteBin.BackgroundColor;
    //  cd.FullOpen = true;
    //  if (cd.ShowDialog () == DialogResult.OK) {
    //    SpriteBin.BackgroundColor = cd.Color;
    //    SetButtonTextAndColor (_UI_background, SpriteBin.BackgroundColor);
    //    CustomSpritesManager.ClearCache ();
    //    _UI_dsSpriteViewer.Invalidate ();
    //  }
    //}

    private void _UI_keyed_Click (object sender, EventArgs e) {
      ColorDialog cd = new ColorDialog ();
      cd.AllowFullOpen = true;
      cd.AnyColor = true;
      cd.Color = SpriteBin.KeyedColor;
      cd.FullOpen = true;
      if (cd.ShowDialog () == DialogResult.OK) {
        SpriteBin.KeyedColor = cd.Color;
        SetButtonTextAndColor (_UI_keyed, SpriteBin.KeyedColor);
        CustomSpritesManager.ClearCache ();
        _UI_dsSpriteViewer.Invalidate ();
      }
    }

    private void SetButtonTextAndColor (Button button, Color color) {
      button.Text = GetTextFromColor (color);
      button.BackColor = color;
      if (color.GetBrightness () > 0.65) {
        button.ForeColor = Color.Black;
      }
      else {
        button.ForeColor = Color.White;
      }
    }

    private string GetTextFromColor (Color color) {
      return (string.Format ("#{0:X2}{1:X2}{2:X2}", color.R, color.G, color.B));
    }

    private void _UI_paletteIndex_ValueChanged (object sender, EventArgs e) {
      if (_internal) {
        return;
      }
      CustomSpriteInfo info = (CustomSpriteInfo) _UI_customSpriteCombo.SelectedItem;
      info._palIndex = (int) _UI_paletteIndex.Value;
      CustomSpritesManager.LoadCustomSprite ((VersionEnum) _UI_chaosEngineVersion.SelectedIndex, info);
      _UI_dsSpriteViewer.Invalidate ();
    }

    private void _UI_paletteOffsetX16_ValueChanged (object sender, EventArgs e) {
      if (_internal) {
        return;
      }
      CustomSpriteInfo info = (CustomSpriteInfo) _UI_customSpriteCombo.SelectedItem;
      info._palOffsetX16 = (int) _UI_paletteOffsetX16.Value;
      CustomSpritesManager.LoadCustomSprite ((VersionEnum) _UI_chaosEngineVersion.SelectedIndex, info);
      _UI_dsSpriteViewer.Invalidate ();
    }

    private void _UI_cols_ValueChanged (object sender, EventArgs e) {
      if (_internal) {
        return;
      }
      CustomSpriteInfo info = (CustomSpriteInfo) _UI_customSpriteCombo.SelectedItem;
      info._cols = (int) _UI_cols.Value;
      CustomSpritesManager.GetCustomSprite ((VersionEnum) _UI_chaosEngineVersion.SelectedIndex, info._id).CreateSummaryBitmap (info._cols);
      _UI_dsSpriteViewer.Invalidate ();
    }

    private void _UI_showInfos_CheckedChanged (object sender, EventArgs e) {
      if (_internal) {
        return;
      }
      SpriteBin.ShowInfos = _UI_showInfos.Checked;
      CustomSpritesManager.ClearCache ();
      _UI_dsSpriteViewer.Invalidate ();
    }

    private void _UI_chaosEngineVersion_SelectedIndexChanged (object sender, EventArgs e) {
      if (_internal) {
        return;
      }
      _internal = true;
      if (_UI_chaosEngineVersion.SelectedIndex == ((int) VersionEnum.AmigaCD32)) {
        _UI_paletteIndex.Maximum = 74;
      }
      else if (_UI_chaosEngineVersion.SelectedIndex == ((int) VersionEnum.AmigaOCS)) {
        _UI_paletteIndex.Maximum = 40;
      }
      else if (_UI_chaosEngineVersion.SelectedIndex == ((int) VersionEnum.AtariST)) {
        _UI_paletteIndex.Maximum = 37;
      }
      else {
        throw new Exception ();
      }
      _internal = false;
      //_UI_customSpriteCombo.DataSource = null;
      string id = (string)_UI_customSpriteCombo.SelectedValue;
      _UI_customSpriteCombo.DataSource = CustomSpritesManager.GetCustomSpriteInfos ((VersionEnum) _UI_chaosEngineVersion.SelectedIndex);
      if (id != null) {
        foreach (CustomSpriteInfo info in _UI_customSpriteCombo.Items) {
          if (info.Id == id) {
            _UI_customSpriteCombo.SelectedValue = id;
            break;
          }
        }
      }
      //_UI_customSpriteCombo
      //_UI_customSpriteCombo.DisplayMember = "Id";
      //chaosEngineVersionToolStripMenuItem.DropDown.Close ();
      _UI_dsSpriteViewer.Invalidate ();

      LoadLevel ((int) _UI_levelIndex.Value);

      //_internal = false;
      //_UI_customSpriteCombo.SelectedIndex = 0;
    }

    private void _UI_tabs_SelectedIndexChanged (object sender, EventArgs e) {
      if (_UI_tabs.SelectedIndex == 0) {
        _UI_menuSaveAllAsPNG.Enabled = false;
      }
      else {
        _UI_menuSaveAllAsPNG.Enabled = true;
      }
    }
  }
}