using System;
using System.Collections.Generic;
using System.Text;
using System.Drawing;
using System.Drawing.Imaging;
using System.Threading;
using System.Drawing.Text;
using ChaosEngineViewer.LEVSDAT_BIN;
using ChaosEngineViewer.ACHAOS;
using System.Windows.Forms;
using System.Diagnostics;
using ChaosEngineViewer.Properties;
using System.IO;

namespace ChaosEngineViewer.X1CHAOS_CAS {
  public class X1chaosCas {
    private VersionEnum _version;

    // Header
    private int _offsetBlockA_Tiles;
    private int _offsetBlockB_Templates;
    private int _offsetBlockC_Scripted;
    private int _offsetBlockD_Triggers;
    private int _offsetBlockE_Covers;
    private int _offsetBlockF_Items;
    private int _offsetBlockG_Musics;
    private int _fileSize;
    public int _levelIndex;
    private int _mapX;
    private int _mapY;
    private int _tileX;
    private int _tileY;
    private int _indexItemsPack;
    private bool _showHints = true;

    public Palette _palette;
    public LevxbakBin _levxbakBin;
    public LevxbakBin _lev4bakBin;
    public LevxbakBin _itemsBin;

    // BlockA_Tiles
    public int _mapWidth;
    public int _mapHeight;
    private int _mapTileSet;
    public int _paletteIndex;
    private Tile [,] _mapTiles;

    // BlockB_Templates
    private int _nbElementsB_Templates;
    private Template [] _templates;

    // BlockC_Scripted
    private int _nbElementsC_Scripted;
    private int _elementSizeC_Scripted;
    private List<Scripted> _scripteds;

    // BlockD_Triggers
    private int _nbElementsD_Triggers;
    private int _elementSizeD_Triggers;
    private List<Trigger> _triggers;

    // BlockE_Covers
    private int _nbElementsE_Covers;
    private int _elementSizeE_Covers;
    private List<Cover> _covers;

    // BlockF_Items
    private int _nbElementsF_Items;
    private int _elementSizeF_Items;
    private List<Item> _items;

    // BlockG_Musics
    private int [,] _elementsBlockG_MusicsIndex;

    private List<Entity> _entities;

    private int _nbDestroyedNodes;
    private Dictionary<int, int> _spawnLocationsNbUsed;
    private static int _spawnLocationSize;

    public GridMap _gridMap;

    private static Pen [] pensTrigger;
    private static Pen [] pensItem;
    private static Pen [] pensCover;
    private static Pen [] pensScripted;
    private static Pen [] pensEntity;
    private static Pen [] pensDeath;
    private static Pen [] pensTeleporter;
    private static Brush [] brushesTrigger;
    private static Brush [] brushesItem;
    private static Brush [] brushesCover;
    private static Brush [] brushesScripted;
    private static Brush [] brushesEntity;
    private static Brush [] brushesDeath;
    private static Brush [] brushesTeleporter;

    public string _fileName;

    static X1chaosCas () {
      pensTrigger = new Pen [2];
      pensItem = new Pen [2];
      pensCover = new Pen [2];
      pensScripted = new Pen [2];
      pensEntity = new Pen [2];
      pensDeath = new Pen [2];
      pensTeleporter = new Pen [2];

      brushesTrigger = new Brush [2];
      brushesItem = new Brush [2];
      brushesCover = new Brush [2];
      brushesScripted = new Brush [2];
      brushesEntity = new Brush [2];
      brushesDeath = new Brush [2];
      brushesTeleporter = new Brush [2];

      pensTrigger [1] = new Pen (Color.FromArgb (0, 255, 255));
      pensTrigger [0] = new Pen (Color.FromArgb (0, 160, 160));
      pensItem [1] = new Pen (Color.FromArgb (255, 255, 0));
      pensItem [0] = new Pen (Color.FromArgb (160, 160, 0));
      pensCover [1] = new Pen (Color.FromArgb (255, 255, 255));
      pensCover [0] = new Pen (Color.FromArgb (160, 160, 160));
      pensScripted [1] = new Pen (Color.FromArgb (0, 255, 0));
      pensScripted [0] = new Pen (Color.FromArgb (0, 160, 0));
      pensEntity [1] = new Pen (Color.FromArgb (255, 0, 0));
      pensEntity [0] = new Pen (Color.FromArgb (160, 0, 0));
      pensDeath [1] = new Pen (Color.FromArgb (255, 0, 0));
      pensDeath [0] = new Pen (Color.FromArgb (160, 0, 0));
      pensTeleporter [1] = new Pen (Color.FromArgb (255, 0, 255));
      pensTeleporter [0] = new Pen (Color.FromArgb (160, 0, 160));
      for (int i = 0; i < 2; i++) {
        brushesTrigger [i] = new SolidBrush (Color.FromArgb (125, pensTrigger [i].Color));
        brushesItem [i] = new SolidBrush (Color.FromArgb (125, pensItem [i].Color));
        brushesCover [i] = new SolidBrush (Color.FromArgb (125, pensCover [i].Color));
        brushesScripted [i] = new SolidBrush (Color.FromArgb (125, pensScripted [i].Color));
        brushesEntity [i] = new SolidBrush (Color.FromArgb (125, pensEntity [i].Color));
        brushesDeath [i] = new SolidBrush (Color.FromArgb (125, pensDeath [i].Color));
        brushesTeleporter [i] = new SolidBrush (Color.FromArgb (125, pensTeleporter [i].Color));
      }
    }

    public X1chaosCas (VersionEnum version, int levelIndex) {
      _version = version;

      if ((levelIndex < 0) || (levelIndex > 15)) {
        throw new Exception ();
      }
      _levelIndex = levelIndex;
      string baseFolder;
      if (version == VersionEnum.AmigaCD32) {
        baseFolder = Settings.Default.BaseFolderCD32;
      }
      else if (version == VersionEnum.AmigaOCS) {
        baseFolder = Settings.Default.BaseFolderOCS;
      }
      else if (version == VersionEnum.AtariST) {
        baseFolder = Settings.Default.BaseFolderST;
      }
      else {
        throw new Exception ();
      }
      _fileName = Path.Combine (baseFolder, string.Format ("{0}1CHAOS.CAS", (char) ('A' + _levelIndex)));
      //if (fileName [fileName.Length - 11] == 'Q') {
      //  fileName = "D:\\Archivable\\Reverse Engineering\\Chaos Engine\\Chaos Engine\\ChaosEngineCD32\\data\\P1CHAOS.CAS";
      //  specialPalette = true;
      //}
      //_levelIndex = fileName [fileName.Length - 11] - 'A';
      Dump dump_X1CHAOS_CAS = new Dump (FilesManager.GetFile (version, _fileName));

      // Header
      int address = 0;
      _offsetBlockA_Tiles = dump_X1CHAOS_CAS.ReadLong (address);
      address += 4;
      _offsetBlockB_Templates = dump_X1CHAOS_CAS.ReadLong (address);
      address += 4;
      _offsetBlockC_Scripted = dump_X1CHAOS_CAS.ReadLong (address);
      address += 4;
      _offsetBlockD_Triggers = dump_X1CHAOS_CAS.ReadLong (address);
      address += 4;
      _offsetBlockE_Covers = dump_X1CHAOS_CAS.ReadLong (address);
      address += 4;
      _offsetBlockF_Items = dump_X1CHAOS_CAS.ReadLong (address);
      address += 4;
      _offsetBlockG_Musics = dump_X1CHAOS_CAS.ReadLong (address);
      address += 4;
      // file size is a word?
      _fileSize = dump_X1CHAOS_CAS.ReadLong (address);
      address += 4;

      // BlockA_tiles
      address = _offsetBlockA_Tiles;
      _mapWidth = dump_X1CHAOS_CAS.ReadWord (address);
      address += 2;
      _mapHeight = dump_X1CHAOS_CAS.ReadWord (address);
      address += 2;
      _mapTileSet = dump_X1CHAOS_CAS.ReadByte (address);
      address += 1;
      _paletteIndex = dump_X1CHAOS_CAS.ReadByte (address);
      address += 1;

      _mapTiles = new Tile [_mapWidth, _mapHeight];
      for (int y = 0; y < _mapHeight; y++) {
        for (int x = 0; x < _mapWidth; x++) {
          _mapTiles [x, y] = new Tile (dump_X1CHAOS_CAS.ReadWord (address));
          address += 2;
        }
      }

      // BlockB_Templates
      address = _offsetBlockB_Templates;
      _nbElementsB_Templates = dump_X1CHAOS_CAS.ReadWord (address);
      address += 2;
      _templates = new Template [_nbElementsB_Templates];
      for (int i = 0; i < _nbElementsB_Templates; i++) {
        _templates [i] = new Template (dump_X1CHAOS_CAS, address);
        address += 18;
      }

      // BlockC_Scripted
      address = _offsetBlockC_Scripted;
      _nbElementsC_Scripted = dump_X1CHAOS_CAS.ReadWord (address);
      address += 2;
      _elementSizeC_Scripted = dump_X1CHAOS_CAS.ReadWord (address);
      address += 2;
      _scripteds = new List<Scripted> ();
      for (int i = 0; i < _nbElementsC_Scripted; i++) {
        _scripteds.Add (new Scripted (_version, dump_X1CHAOS_CAS, address));
        address += _elementSizeC_Scripted;
      }

      // BlockD_Triggers
      address = _offsetBlockD_Triggers;
      _nbElementsD_Triggers = dump_X1CHAOS_CAS.ReadWord (address);
      address += 2;
      _elementSizeD_Triggers = dump_X1CHAOS_CAS.ReadWord (address);
      address += 2;
      _triggers = new List<Trigger> ();
      for (int i = 0; i < _nbElementsD_Triggers; i++) {
        _triggers.Add (new Trigger (dump_X1CHAOS_CAS, address));
        address += _elementSizeD_Triggers;
      }

      // BlockE_Covers
      address = _offsetBlockE_Covers;
      _nbElementsE_Covers = dump_X1CHAOS_CAS.ReadWord (address);
      address += 2;
      _elementSizeE_Covers = dump_X1CHAOS_CAS.ReadWord (address);
      address += 2;
      _covers = new List<Cover> ();
      for (int i = 0; i < _nbElementsE_Covers; i++) {
        _covers.Add (new Cover (dump_X1CHAOS_CAS, address));
        address += _elementSizeE_Covers;
      }

      // BlockF_Items
      address = _offsetBlockF_Items;
      _nbElementsF_Items = dump_X1CHAOS_CAS.ReadWord (address);
      address += 2;
      _elementSizeF_Items = dump_X1CHAOS_CAS.ReadWord (address);
      address += 2;
      _items = new List<Item> ();
      for (int i = 0; i < _nbElementsF_Items; i++) {
        _items.Add (new Item (dump_X1CHAOS_CAS, address));
        address += _elementSizeF_Items;
      }

      // BlockG_Musics
      address = _offsetBlockG_Musics;
      _elementsBlockG_MusicsIndex = new int [_mapWidth >> 2, _mapHeight >> 2];
      for (int y = 0; y < (_mapHeight >> 2); y++) {
        for (int x = 0; x < (_mapWidth >> 2); x++) {
          _elementsBlockG_MusicsIndex [x, y] = dump_X1CHAOS_CAS.ReadByte (address);
          address += 1;
        }
      }

      //if (fileName [0] == 'P') {
      //  _mapPalette = 41;
      //}
      // special palette for last level (boss)
      if (version == VersionEnum.AmigaCD32) {
        if (_levelIndex == 15) {
          _paletteIndex = 41;
        }
      }
      _palette = new Palette (version, _paletteIndex);

      Console.WriteLine ("TileSet: " + _mapTileSet);
      Console.WriteLine ("PaletteIndex: " + _paletteIndex);
      if (_mapTileSet < 4) {
        _levxbakBin = new LevxbakBin (version, _mapTileSet, _palette, 0);
      }
      else {
        _lev4bakBin = new LevxbakBin (version, 4, _palette, 0);
        _levxbakBin = new LevxbakBin (version, _mapTileSet + 1, _palette, 0);
      }

      _indexItemsPack = _mapTileSet;
      if (_indexItemsPack == 5) {
        _indexItemsPack = 4;
      }
      _indexItemsPack--;

      //string fileNameMulti2 = Path.Combine (Settings.Default.BaseFolder, "SNGARC2.BIN");
      //byte [] [] sngarc2_bin = FilesManager.GetFileMulti (fileNameMulti2);
      //RNC.WriteTofileMulti (Path.Combine (Settings.Default.BaseFolder, "SNGARC2.{0:D3}.BIN"), sngarc2_bin);
      //byte [] [] sngarc_bin = FilesManager.GetFileMulti (fileNameMulti);
      //RNC.WriteTofileMulti (Path.Combine (Settings.Default.BaseFolder, "SNGARC.{0:D3}.BIN"), sngarc_bin);
      string fileNameMulti = Path.Combine (baseFolder, "SNGARC.BIN");

      Dump dumpItemsPack;
      if (version == VersionEnum.AmigaCD32) {
        // 23-26
        dumpItemsPack = new Dump (FilesManager.GetFile (version, fileNameMulti, 23 + _indexItemsPack));
        _itemsBin = new LevxbakBin (dumpItemsPack, _palette, 144);
      }
      else if (version == VersionEnum.AmigaOCS) {
        // 23-26
        dumpItemsPack = new Dump (FilesManager.GetFile (version, fileNameMulti, 23 + _indexItemsPack));
        _itemsBin = new LevxbakBin (dumpItemsPack, _palette, 0);
      }
      else if (version == VersionEnum.AtariST) {
        string fileNameItemsPack = Path.Combine (baseFolder, string.Format ("TOKENS_{0}.BIN", (char) ('1' + _indexItemsPack)));
        dumpItemsPack = new Dump (FilesManager.GetFile (version, fileNameItemsPack));
        _itemsBin = new LevxbakBin (dumpItemsPack, _palette, 0);
      }
      else {
        throw new Exception ();
      }
      //Palette paletteItemsPack = new Palette (dump_ACHAOS, 3);

      //_itemsType = new ItemType [40];
      //for (int i = 0; i < 40; i++) {
      //  _itemsType [i] = new ItemType (dump_ACHAOS, indexItemsPack, i, this);
      //}

      Dump dumpEntities;
      if ((version == VersionEnum.AmigaCD32) ||
          (version == VersionEnum.AmigaOCS)) {
        dumpEntities = new Dump (FilesManager.GetFile (version, fileNameMulti, 7 + _levelIndex));
      }
      else if (version == VersionEnum.AtariST) {
        string fileNameEntities = Path.Combine (baseFolder, string.Format ("{0}1CHAOS.PLA", (char) ('A' + _levelIndex)));
        dumpEntities = new Dump (FilesManager.GetFile (version, fileNameEntities));
      }
      else {
        throw new Exception ();
      }
      int nbEntities = dumpEntities.ReadWord (0) / 20;
      _entities = new List<Entity> ();
      for (int i = 0; i < nbEntities; i++) {
        _entities.Add (new Entity (_version, dumpEntities, 2 + i * 20, _levelIndex));
      }

      foreach (Element00_A_EntityClass entityClass in LevsdatBin.block00_Levels [(int) _version].elements00 [_levelIndex].elements00_A_EntityClasses) {
        SpritesManager.GetSpriteBinA (version, entityClass.GetSpriteDef (version).spriteFileDefAddress, _levelIndex, _paletteIndex);
      }

      InitLevel ();

      _gridMap = new GridMap (this);

      _gridMap._mapBaseDirty = true;
      //_gridMap.RedrawMapWait ();

      //DrawMapBase ();
      //_gridMap.RedrawMapWait ();
    }

    private void InitLevel () {
      foreach (Item item in new List<Item> (_items)) {
        Tile tile = _mapTiles [item.X, item.Y];
        item.BackedUpTile._00w_tile = tile._00w_tile;
        item.CurrentTile.Height = tile.Height;
        item.CurrentTile.HasTrigger = tile.HasTrigger;
        tile._00w_tile = item.CurrentTile._00w_tile;
        // ajouter les compteurs d'objets
      }

      foreach (Cover cover in new List<Cover> (_covers)) {
        // ajouter le compteur de portails
        Template template = _templates [cover.TemplateIndex];
        for (int y = 0; y < 3; y++) {
          for (int x = 0; x < 3; x++) {
            if (((cover.X + x) >= _mapWidth) || ((cover.Y + y) >= _mapHeight)) {
              continue;
            }
            cover.BackedUpTemplate.tiles [x, y]._00w_tile = 0xFFFF;
            Tile tileCover = template.tiles [x, y];
            if (tileCover.Unk == 0) {
              Tile tileMap = _mapTiles [cover.X + x, cover.Y + y];
              cover.BackedUpTemplate.tiles [x, y]._00w_tile = tileMap._00w_tile & 0x7FFF;
            }
          }
        }
      }

      foreach (Scripted scripted in new List<Scripted> (_scripteds)) {
        int scriptClassIndex = scripted.scriptClassIndex;
        scripted.addressUnk40 = LevsdatBin.block16_ScriptsClasses [(int) _version].addressBlock16 + LevsdatBin.block16_ScriptsClasses [(int) _version].offsetElements16_A [scriptClassIndex];
        scripted.addressUnk44 = LevsdatBin.block16_ScriptsClasses [(int) _version].addressBlock16 + LevsdatBin.block16_ScriptsClasses [(int) _version].offsetElements16_A [scriptClassIndex];
        scripted.offset48 = LevsdatBin.block16_ScriptsClasses [(int) _version].offsetElements16_B [scriptClassIndex] + LevsdatBin.block16_ScriptsClasses [(int) _version].offsetElements16_A [scriptClassIndex];
        scripted.offset50 = LevsdatBin.block16_ScriptsClasses [(int) _version].offsetElements16_D [scriptClassIndex] + LevsdatBin.block16_ScriptsClasses [(int) _version].offsetElements16_A [scriptClassIndex];
        scripted.offset52 = LevsdatBin.block16_ScriptsClasses [(int) _version].offsetElements16_C [scriptClassIndex] + LevsdatBin.block16_ScriptsClasses [(int) _version].offsetElements16_A [scriptClassIndex];
        scripted.TemplateIndex = 0xFFFF;
        scripted.currentDelayBetweenRepeat = 1;
        // hack for scripted having no "If_Delay<0" (doing infinite loop)
        //scripted.currentDelayBetweenRepeat = -1;

        //if (LevsdatBin.block16_ScriptsClasses.elements16 [scriptClassIndex].tempA1 == 0x08) {
        //  int b = LevsdatBin.block16_ScriptsClasses.elements16 [scriptClassIndex].tempA2;
        //  if (b != -1) {
        //    scripted.ingame_templateIndex = b;
        //    scripted.templateIndex = Math.Abs (b);
        //    scripted.initialState = (b < 0) ? 1 : 0;
        //  }
        //}
        for (int y = 0; y < 3; y++) {
          for (int x = 0; x < 3; x++) {
            if (((scripted.X + x) >= _mapWidth) || ((scripted.Y + y) >= _mapHeight)) {
              continue;
            }
            Tile tileMap = _mapTiles [scripted.X + x, scripted.Y + y];
            scripted.backedUpTemplate.tiles [x, y]._00w_tile = tileMap._00w_tile;
          }
        }
      }

      foreach (Cover cover in new List<Cover> (_covers)) {
        if (cover.InitialState == 0) { // ON
          Template template = _templates [cover.TemplateIndex];
          CopyTemplateTo3x3Tiles_SetDstBit15TL (template, cover.X, cover.Y);
        }
      }

      foreach (Scripted scripted in new List<Scripted> (_scripteds)) {
        int templateIndex = (int) scripted.GetScript (1)._instructions [0]._param;
        scripted.TemplateIndex = templateIndex;
        if (templateIndex <= 0x7F) {
          Template template = _templates [scripted.TemplateIndex];
          CopyTemplateTo3x3Tiles_SetDstBit15TL (template, scripted.X, scripted.Y);
        }
      }

      foreach (Entity entity in new List<Entity> (_entities)) {
        if (entity.GetEntityClass (_levelIndex).spriteDefIndex == 8) {
          //_nbDestroyedNodes++;
          //entity.linkId08_b = 0;
          //entity.linkId17_b = 0;
          entity.spriteIndex = 5;
          //UpdateCoversAccordingToDestroyedNodes ();
        }
      }

      _nbDestroyedNodes = 0;
      _spawnLocationsNbUsed = new Dictionary<int, int> ();
      _spawnLocationSize = 24;
      UpdateCoversAccordingToDestroyedNodes ();

    }

    private void Check () {
      int listeningLinkId = 9999;

      foreach (Item a in new List<Item> (_items)) {
        foreach (Item b in new List<Item> (_items)) {
          if (a != b) {
            if ((a.X == b.X) && (a.Y == b.Y)) {
              throw new Exception ();
            }
          }
        }
      }

      foreach (Scripted a in new List<Scripted> (_scripteds)) {
        foreach (Scripted b in new List<Scripted> (_scripteds)) {
          if (a != b) {
            if ((a.X == b.X) && (a.Y == b.Y)) {
              throw new Exception ();
            }
          }
        }
        if (a.linkId18_w == listeningLinkId) {
          throw new Exception ();
        }
        if (-a.linkId56_w == listeningLinkId) {
          throw new Exception ();
        }
      }

      foreach (Trigger a in new List<Trigger> (_triggers)) {
        foreach (Trigger b in new List<Trigger> (_triggers)) {
          if (a != b) {
            if ((a.X == b.X) && (a.Y == b.Y)) {
              throw new Exception ();
            }
          }
        }
      }

      foreach (Cover a in new List<Cover> (_covers)) {
        a.Check ();
        foreach (Cover b in new List<Cover> (_covers)) {
          if (a != b) {
            if ((a.X == b.X) && (a.Y == b.Y)) {
              throw new Exception ();
            }
          }
        }
        if (-a.LinkId27b == listeningLinkId) {
          throw new Exception ();
        }
      }

      foreach (Entity entity in new List<Entity> (_entities)) {
        int w, h;
        entity.GetSize (_levelIndex, _paletteIndex, out w, out h);
        if (((entity.mapX + w) > (_mapWidth * 16)) || ((entity.mapY + h) > (_mapHeight * 16))) {
          entity.mapX = 0;
          entity.mapY = 0;
        }
        if (entity.linkId08_b == listeningLinkId) {
          throw new Exception ();
        }
      }
    }

    private void UpdateCoversAccordingToDestroyedNodes () {
      foreach (Cover cover in new List<Cover> (_covers)) {
        if (cover.IsClosedPortal) {
          if (_nbDestroyedNodes >= cover.NbNodesToActivate) {
            // open portal
            cover.Misc_A = 0x03;
            CopyTemplateTo3x3Tiles (cover.BackedUpTemplate, cover.X, cover.Y);
          }
        }
      }
    }

    private Object _lockSetSelectedTile = new object ();
    public void SetSelectedTile (int windowX, int windowY) {
      lock (_lockSetSelectedTile) {
        _gridMap.WindowToMap (windowX, windowY, out _mapX, out _mapY);
        //_mapX = (mouseX - cursorX * (16 + _space)) + cursorX * 16;
        //_mapY = (mouseY - cursorY * (16 + _space)) + cursorY * 16;

        int tileX, tileY;
        _gridMap.WindowToTile (windowX, windowY, out tileX, out tileY);
        //int tileX = windowX / (16 + _space);
        //int tileY = windowY / (16 + _space);
        if ((tileX >= _mapWidth) || (tileY >= _mapHeight)) {
          return;
        }
        //if ((tileX == _tileX) && (tileY == _tileY)) {
        //  return;
        //}
        _tileX = tileX;
        _tileY = tileY;
        _gridMap.RedrawMap ();
      }
    }

    public void ClickMap (int windowX, int windowY, MouseButtons mouseButton) {
      int tileX, tileY;
      _gridMap.WindowToTile (windowX, windowY, out tileX, out tileY);
      if ((tileX >= _mapWidth) || (tileY >= _mapHeight)) {
        return;
      }
      int mapX, mapY;
      _gridMap.WindowToMap (windowX, windowY, out mapX, out mapY);

      if (mouseButton == MouseButtons.Left) {
        if (!CheckSpawnLocationUnderPlayer (mapX, mapY)) {
          CheckTileUnderEntityOnscreen_Players (tileX, tileY);
          CheckActivateScripted (tileX, tileY);
        }
      }
      else if (mouseButton == MouseButtons.Right) {
        if (!CheckCollisions_Missiles_vs_EntitiesOnscreen20andPlayers (mapX, mapY)) {
          CheckTileUnderEntityOnscreenAll (tileX, tileY);
        }
      }

      _gridMap._mapBaseDirty = true;
      _gridMap.RedrawMap ();
    }

    private bool CheckSpawnLocationUnderPlayer (int mapX, int mapY) {
      bool updated = false;
      Element00_B_SpawnLocation [] spawnLocations = LevsdatBin.block00_Levels [(int) _version].elements00 [_levelIndex].elements00_B_SpawnLocations;
      for (int i = 0; i < spawnLocations.Length; i++) {
        Element00_B_SpawnLocation spawnLocation = spawnLocations [i];
        int dx = mapX - spawnLocation.x;
        int dy = mapY - spawnLocation.y;
        int l = (int) Math.Sqrt (dx * dx + dy * dy);
        if (l < (_spawnLocationSize >> 1)) {
          if (!_spawnLocationsNbUsed.ContainsKey (i)) {
            _spawnLocationsNbUsed.Add (i, 0);
          }
          _spawnLocationsNbUsed [i]++;
          updated = true;
        }
      }
      return (updated);
    }

    private void CheckActivateScripted (int tileX, int tileY) {
      Scripted scripted = FindScripted (tileX, tileY);
      if (scripted == null) {
        return;
      }
      if (IsScriptedActive (scripted)) {
        // activates by time
        scripted.linkId56_w = 0;
        DoScript (scripted);
      }
      //else if (scripted.linkId56_w == 0) {
      //}
    }

    private void CheckActivateScripted_DrawArrows (Scripted scripted) {
      for (int i = 1; i <= 4; i++) {
        Script script = scripted.GetScript (i);
        foreach (Instruction instruction in script._instructions) {
          int real = 0;
          if (instruction == scripted.CurrentInstruction) {
            real = 1;
          }
          if (instruction._type._index == 0x0E) {
            if (scripted.linkId54_w != 0) {
              int mapX, mapY;
              _gridMap.TileToMap (scripted.X, scripted.Y, out mapX, out mapY);
              int linkId = scripted.linkId54_w;
              HandleLinkIdForScriptedsAndCovers_DrawArrows (real, linkId, mapX, mapY);
              HandleLinkIdForEntitiesOffscreenAndOnscreen_DrawArrows (real, linkId, mapX, mapY);
            }
          }
          else if (instruction._type._index == 0x18) {
            //throw new Exception ();
            // todo
            foreach (Entity entity in new List<Entity> (_entities)) {
              // node
              if (entity.GetEntityClass (_levelIndex).spriteDefIndex == 7) {
                // hack
                if (entity.spriteIndex == 5) {
                  continue;
                }
                int mapX, mapY;
                _gridMap.TileToMap (scripted.X, scripted.Y, out mapX, out mapY);
                if (entity.linkId17_b != 0) {
                  int linkId = entity.linkId17_b;
                  HandleLinkIdForScriptedsAndCovers_DrawArrows (real, linkId, mapX, mapY);
                }
                _gridMap.DrawArrowMap (pensEntity [real], mapX, mapY, entity.mapX, entity.mapY);
                //break;
              }
            }
          }
        }
      }
    }

    private void CheckActivateScripted0E_DrawArrows_ReversePost (Scripted scripted, int linkId, int mapX, int mapY) {
      int scriptedMapX, scriptedMapY;
      _gridMap.TileToMap (scripted.X, scripted.Y, out scriptedMapX, out scriptedMapY);
      for (int i = 1; i <= 4; i++) {
        Script script = scripted.GetScript (i);
        foreach (Instruction instruction in script._instructions) {
          int real = 0;
          if (instruction == scripted.CurrentInstruction) {
            real = 1;
          }
          if (instruction._type._index == 0x0E) {
            if (scripted.linkId54_w == linkId) {
              _gridMap.DrawArrowMap (pensScripted [real], scriptedMapX, scriptedMapY, mapX, mapY);
              CheckActivateScripted_DrawArrows_ReversePre (scripted);
            }
          }
          else if (instruction._type._index == 0x18) {
            //throw new Exception ();
            // todo
            //_gridMap.DrawArrowMap (pensScripted [real], scriptedMapX, scriptedMapY, mapX, mapY);
          }
        }
      }
    }


    private void CheckActivateScripted18_DrawArrows_ReversePost (Scripted scripted, int linkId, int mapX, int mapY) {
      int scriptedMapX, scriptedMapY;
      _gridMap.TileToMap (scripted.X, scripted.Y, out scriptedMapX, out scriptedMapY);
      for (int i = 1; i <= 4; i++) {
        Script script = scripted.GetScript (i);
        foreach (Instruction instruction in script._instructions) {
          int real = 0;
          if (instruction == scripted.CurrentInstruction) {
            real = 1;
          }
          if (instruction._type._index == 0x0E) {
            if (scripted.linkId54_w == linkId) {
              _gridMap.DrawArrowMap (pensScripted [real], scriptedMapX, scriptedMapY, mapX, mapY);
              CheckActivateScripted_DrawArrows_ReversePre (scripted);
            }
          }
          else if (instruction._type._index == 0x18) {
            //throw new Exception ();
            // todo
            //_gridMap.DrawArrowMap (pensScripted [real], scriptedMapX, scriptedMapY, mapX, mapY);
          }
        }
      }
    }
    private void CheckActivateScripted_DrawArrows_ReversePre (Scripted scripted) {
      int scriptedMapX, scriptedMapY;
      _gridMap.TileToMap (scripted.X, scripted.Y, out scriptedMapX, out scriptedMapY);
      if (scripted.linkId18_w != 0) {
        HandleLinkIdForScriptedsAndCovers_DrawArrows_Reverse (scripted.linkId18_w, scriptedMapX, scriptedMapY);
      }
      if (!IsScriptedActive (scripted)) {
        HandleLinkIdForScriptedsAndCovers_DrawArrows_Reverse (-scripted.linkId56_w, scriptedMapX, scriptedMapY);
      }
    }

    private void DoScript (Scripted scripted) {
      bool changeMade = false;
      //bool loopReached = false;
      Dictionary<int, bool?> visitedNode = new Dictionary<int, bool?> ();
      for (; ; ) {
        //if (!loopReached) {
        if (!visitedNode.ContainsKey (scripted.GetCurrentInstructionUniqueKey ())) {
          visitedNode.Add (scripted.GetCurrentInstructionUniqueKey (), null);
        }
        //}
        Instruction instruction = scripted.CurrentInstruction;
        if (DoInstruction (instruction, scripted, ref changeMade)) {
          break;
        }
        if (scripted.CurrentScript.HasIfDelay ()) {
          if ((visitedNode.ContainsKey (scripted.GetCurrentInstructionUniqueKey ())) &&
              (scripted.currentDelayBetweenRepeat < 0)) {
            break;
          }
        }
        else {
          if (visitedNode.ContainsKey (scripted.GetCurrentInstructionUniqueKey ())) {
            break;
          }
        }
        //if (instruction._type._stop) {
        //  break;
        //}
      }
    }

    private bool DoInstruction (Instruction instruction, Scripted scripted, ref bool changeMade) {
      // SetActivatingEvent
      if (instruction._type._index == 0x00) {
        int linkId56_w = (int) instruction._param;
        // hack: we don't want to wait for time
        //if (linkId56_w == 0) {
        //  scripted.linkId56_w = linkId56_w;
        //}
        if (linkId56_w >= 0) {
          scripted.linkId56_w = 0;
        }
        scripted.GoNextInstruction ();
      }
      // Spawn_If_RepeatCount=0_Then_SetActivatingEvent0_&_ExecuteScript3
      else if (instruction._type._index == 0x02) {
        if (changeMade) {
          return (true);
        }
        changeMade = true;
        int entityClassIndex = scripted.spawnedEntityClassIndex;
        SpriteDef spriteDef = LevsdatBin.block00_Levels [(int) _version].elements00 [_levelIndex].elements00_A_EntityClasses [entityClassIndex].GetSpriteDef (_version);
        Entity entity = new Entity (_version, scripted.X * 16 + spriteDef.xOffset, scripted.Y * 16 + spriteDef.yOffset, 0, entityClassIndex, scripted.linkId16_w, _levelIndex);
        _entities.Add (entity);
        scripted.GoNextInstruction ();
        if (scripted.repeatCount == 0) {
          scripted.SetCurrentScript (3);
          scripted.linkId56_w = 0;
        }
      }
      // RewindScript
      else if (instruction._type._index == 0x04) {
        scripted.RewindScript ();
      }
      // RemoveScripted
      else if (instruction._type._index == 0x06) {
        //if (changeMade) {
        //  return (true);
        //}
        //changeMade = true;
        Tile tileMap = _mapTiles [scripted.X, scripted.Y];
        tileMap.Unk = 0;
        _scripteds.Remove (scripted);
        return (true);
      }
      // SetTemplate1
      else if (instruction._type._index == 0x08) {
        if (scripted.TemplateIndex != instruction._param) {
          if ((instruction._param != 0) &&
              (((scripted.TemplateIndexOld != instruction._param) || (scripted.TemplateIndex != 0)))) {
            if (changeMade) {
              return (true);
            }
            changeMade = true;
          }
        }
        scripted.GoNextInstruction ();
        if (scripted.TemplateIndex != instruction._param) {
          scripted.TemplateIndex = (int) instruction._param;
          if (scripted.TemplateIndex != 0) {
            Template template = null;
            if (instruction._param == 0xFF) {
              template = scripted.backedUpTemplate;
            }
            else {
              template = _templates [(int) instruction._param];
            }
            CopyTemplateTo3x3Tiles_SetDstBit15TL (template, scripted.X, scripted.Y);
            if (instruction._param == 0xFF) {
              Tile tileMap = _mapTiles [scripted.X, scripted.Y];
              tileMap.Unk = 0;
            }
          }
        }
      }
      // SetDelayBetweenRepeat&ExecuteScript1
      else if (instruction._type._index == 0x0A) {
        scripted.SetCurrentScript (1);
        scripted.currentDelayBetweenRepeat = scripted.delayBetweenRepeat;
        //scripted.currentDelayBetweenRepeat = -1;
      }
      // If_Delay<0_&_RepeatCount>0orFF_Then_DecRepeatCount(!=FF)_&_ExecuteScript2
      else if (instruction._type._index == 0x0C) {
        scripted.GoNextInstruction ();
        scripted.currentDelayBetweenRepeat--;
        if (scripted.currentDelayBetweenRepeat < 0) {
          if (scripted.repeatCount != 0) {
            if (scripted.repeatCount > 0) {
              scripted.repeatCount--;
            }
            scripted.SetCurrentScript (2);
          }
        }
        else {
          // hack to loop only one time
          scripted.currentDelayBetweenRepeat = 0;
        }
      }
      // TriggerLinkId54
      else if (instruction._type._index == 0x0E) {
        //if (scripted.linkId54_w != 0) {
        //  if (changeMade) {
        //    return (true);
        //  }
        //  changeMade = true;
        //}
        scripted.GoNextInstruction ();
        if (scripted.linkId54_w != 0) {
          int linkId = scripted.linkId54_w;
          scripted.linkId54_w = 0;
          HandleLinkIdForScriptedsAndCovers (linkId);
          HandleLinkIdForEntitiesOffscreenAndOnscreen (linkId);
        }
      }
      // SetTemplate2
      else if (instruction._type._index == 0x10) {
        if (scripted.TemplateIndex != instruction._param) {
          if ((instruction._param != 0) &&
              (((scripted.TemplateIndexOld != instruction._param) || (scripted.TemplateIndex != 0)))) {
            if (changeMade) {
              return (true);
            }
            changeMade = true;
          }
        }
        scripted.GoNextInstruction ();
        if (scripted.TemplateIndex != instruction._param) {
          scripted.TemplateIndex = (int) instruction._param;
          if (scripted.TemplateIndex != 0) {
            Template template = null;
            if (instruction._param == 0xFF) {
              template = scripted.backedUpTemplate;
            }
            else {
              template = _templates [(int) instruction._param];
            }
            CopyTemplateTo3x3Tiles_SetDstBit15TL (template, scripted.X, scripted.Y);
          }
        }
      }
      // DoExplosion2
      else if (instruction._type._index == 0x12) {
        scripted.GoNextInstruction ();
      }
      // DoExplosion1
      else if (instruction._type._index == 0x14) {
        scripted.GoNextInstruction ();
      }
      // PlaySoundUnk
      else if (instruction._type._index == 0x16) {
        scripted.GoNextInstruction ();
      }
      // ?
      else if (instruction._type._index == 0x18) {
        //throw new Exception ();
        // todo
        foreach (Entity entity in new List<Entity> (_entities)) {
          // node
          if (entity.GetEntityClass (_levelIndex).spriteDefIndex == 7) {
            // hack
            if (entity.spriteIndex == 5) {
              continue;
            }
            if (entity.linkId17_b != 0) {
              int linkId = entity.linkId17_b;
              HandleLinkIdForScriptedsAndCovers (linkId);
            }
            _nbDestroyedNodes++;
            entity.linkId08_b = 0;
            entity.linkId17_b = 0;
            entity.spriteIndex = 5;
            UpdateCoversAccordingToDestroyedNodes ();
            break;
            //_entities.Remove (entity);
          }
        }
        scripted.GoNextInstruction ();
      }
      else {
        throw new Exception ();
      }
      return (false);
    }

    private bool CheckCollisions_Missiles_vs_EntitiesOnscreen20andPlayers (int mapX, int mapY) {
      Entity entity = FindEntity (mapX, mapY);
      if (entity == null) {
        return (false);
      }
      if (entity.linkId17_b != 0) {
        int linkId = entity.linkId17_b;
        entity.linkId17_b = 0;
        HandleLinkIdForScriptedsAndCovers (linkId);
        HandleLinkIdForEntitiesOffscreenAndOnscreen (linkId);
      }
      // hack for nodes
      if (entity.GetEntityClass (_levelIndex).spriteDefIndex == 7) {
        _nbDestroyedNodes++;
        entity.linkId08_b = 0;
        entity.linkId17_b = 0;
        entity.spriteIndex = 5;
        UpdateCoversAccordingToDestroyedNodes ();
      }
      // electric node
      else if (entity.GetEntityClass (_levelIndex).spriteDefIndex == 8) {
      }
      // where in asm?
      else {
        _entities.Remove (entity);
      }
      return (true);
    }

    private void CheckCollisions_Missiles_vs_EntitiesOnscreen20andPlayers_DrawArrows (int mapX, int mapY) {
      Entity entity = FindEntity (mapX, mapY);
      if (entity == null) {
        return;
      }
      if (entity.linkId17_b != 0) {
        int linkId = entity.linkId17_b;
        HandleLinkIdForScriptedsAndCovers_DrawArrows (1, linkId, entity.mapX, entity.mapY);
        HandleLinkIdForEntitiesOffscreenAndOnscreen_DrawArrows (1, linkId, entity.mapX, entity.mapY);
      }
      //if (entity.GetEntityClass (_levelIndex).spriteDefIndex == 7) {
      //  entity.linkId08_b = 0;
      //  entity.linkId17_b = 0;
      //  entity.spriteIndex = 3;
      //}
      //else {
      //  _entities.Remove (entity);
      //}
    }

    private void CheckCollisions_Missiles_vs_EntitiesOnscreen20andPlayers_DrawArrows_Reverse (int mapX, int mapY) {
      Entity entity = FindEntity (mapX, mapY);
      if (entity == null) {
        return;
      }
      if (entity.linkId08_b != 0) {
        HandleLinkIdForEntitiesOffscreenAndOnscreen_DrawArrows_ReversePre (entity.linkId08_b, entity.mapX, entity.mapY);
      }
      if (entity.GetEntityClass (_levelIndex).spriteDefIndex == 7) {
        // hack
        if (entity.spriteIndex != 5) {
          foreach (Scripted scripted in new List<Scripted> (_scripteds)) {
            for (int i = 1; i <= 4; i++) {
              Script script = scripted.GetScript (i);
              foreach (Instruction instruction in script._instructions) {
                int real = 0;
                if (instruction == scripted.CurrentInstruction) {
                  real = 1;
                }
                if (instruction._type._index == 0x18) {
                  //throw new Exception ();
                  // todo
                  int mapX2, mapY2;
                  _gridMap.TileToMap (scripted.X, scripted.Y, out mapX2, out mapY2);
                  CheckActivateScripted_DrawArrows_ReversePre (scripted);
                  _gridMap.DrawArrowMap (pensEntity [real], mapX2, mapY2, entity.mapX, entity.mapY);
                }
              }
            }
          }
        }
      }
      //if (entity.GetEntityClass (_levelIndex).spriteDefIndex == 7) {
      //  entity.linkId08_b = 0;
      //  entity.linkId17_b = 0;
      //  entity.spriteIndex = 3;
      //}
      //else {
      //  _entities.Remove (entity);
      //}
    }

    private void CheckTileUnderEntityOnscreenAll (int tileX, int tileY) {
      Tile tileMap = _mapTiles [tileX, tileY];

      if (tileMap.Type == (int) Tile.TypesEnum.Empty) {
      }
      else if (tileMap.Type == (int) Tile.TypesEnum.Scripted) {
        // hit by missile
        int tileX2, tileY2;
        if (FindHitLocation (tileX, tileY, out tileX2, out tileY2)) {
          Scripted scripted = FindScripted (tileX2, tileY2);
          if (scripted != null) {
            scripted.SetCurrentScript (4);
            // kills by missile
            scripted.linkId56_w = 0;
            DoScript (scripted);
          }
        }
      }
      else if (tileMap.Type == (int) Tile.TypesEnum.Cover) {
        // hit by missile
        int tileX2, tileY2;
        if (FindHitLocation (tileX, tileY, out tileX2, out tileY2)) {
          Cover cover = FindCover (tileX2, tileY2);
          if (cover != null) {
            if ((cover.Raw26b != 0) || (cover.Raw27b != 0)) {
              if (cover.Misc_A <= 0x01) {
                if (cover.Misc_B == 0) {
                  CopyTemplateTo3x3Tiles_SetDstBit15TL (cover.BackedUpTemplate, tileX2, tileY2);
                  _covers.Remove (cover);
                }
              }
            }
          }
        }
      }
      else if (tileMap.Type == (int) Tile.TypesEnum.Wall) {
      }
    }

    private void CheckTileUnderEntityOnscreenAll_DrawArrows (int tileX, int tileY) {
      Tile tileMap = _mapTiles [tileX, tileY];

      if (tileMap.Type == (int) Tile.TypesEnum.Empty) {
      }
      else if (tileMap.Type == (int) Tile.TypesEnum.Scripted) {
        // hit by missile
        int tileX2, tileY2;
        if (FindHitLocation (tileX, tileY, out tileX2, out tileY2)) {
          Scripted scripted = FindScripted (tileX2, tileY2);
          if (scripted != null) {
            int mapX1, mapY1, mapX2, mapY2;
            _gridMap.TileToMap (tileX, tileY, out mapX1, out mapY1);
            _gridMap.TileToMap (tileX2, tileY2, out mapX2, out mapY2);
            _gridMap.DrawArrowMap (pensDeath [1], mapX1, mapY1, mapX2, mapY2);
            CheckActivateScripted_DrawArrows (scripted);
          }
        }
        // kills by missile
        //scripted.linkId56_w = 0;
        //DoScript (scripted);
      }
      else if (tileMap.Type == (int) Tile.TypesEnum.Cover) {
        // hit by missile
        int tileX2, tileY2;
        if (FindHitLocation (tileX, tileY, out tileX2, out tileY2)) {
          Cover cover = FindCover (tileX2, tileY2);
          if (cover != null) {
            if ((cover.Raw26b != 0) || (cover.Raw27b != 0)) {
              if (cover.Misc_A <= 0x01) {
                if (cover.Misc_B == 0) {
                  int mapX1, mapY1, mapX2, mapY2;
                  _gridMap.TileToMap (tileX, tileY, out mapX1, out mapY1);
                  _gridMap.TileToMap (tileX2, tileY2, out mapX2, out mapY2);
                  _gridMap.DrawArrowMap (pensDeath [1], mapX1, mapY1, mapX2, mapY2);
                }
              }
            }
          }
        }
      }
      else if (tileMap.Type == (int) Tile.TypesEnum.Wall) {
      }
    }

    private void CheckTileUnderEntityOnscreen_Players (int tileX, int tileY) {
      Tile tileMap = _mapTiles [tileX, tileY];

      bool checkTrigger = false;
      if (tileMap.Type == (int) Tile.TypesEnum.Empty) {
        checkTrigger = true;
      }
      else if (tileMap.Type == (int) Tile.TypesEnum.Wall) {
      }
      else if (tileMap.Type == (int) Tile.TypesEnum.Stair) {
        checkTrigger = true;
      }
      else if (tileMap.Type == (int) Tile.TypesEnum.Scripted) {
      }
      else if (tileMap.Type == (int) Tile.TypesEnum.Cover) {
        Cover cover = FindTopLeftCover (tileX, tileY);
        if (cover != null) {
          if (cover.IsOpenedPortal) {
            Element00_C_Portal portal = LevsdatBin.block00_Levels [(int) _version].elements00 [_levelIndex].GetElement00_C_Portal (cover.Misc_PortalIndex);
            if (!_spawnLocationsNbUsed.ContainsKey (portal.spawnLocationIndex)) {
              _spawnLocationsNbUsed.Add (portal.spawnLocationIndex, 0);
            }
            _spawnLocationsNbUsed [portal.spawnLocationIndex]++;
            //_covers.Remove (cover);
            //throw new Exception ();
            // TODO: teleport
            // handle portal
            //if (portal.LevelIndex == _levelIndex) {
            // add health
            //}
          }
        }
      }
      else if (tileMap.Type == (int) Tile.TypesEnum.Item) {
        //if (tileMap.HasTrigger == 0) {
        Item item = FindItem (tileX, tileY);
        tileMap._00w_tile = item.BackedUpTile._00w_tile;
        // handle item
        _items.Remove (item);
        //}
        //else {
        checkTrigger = true;
        //}
      }
      else {
        checkTrigger = true;
      }
      checkTrigger = true;
      if (checkTrigger) {
        Trigger trigger = FindTrigger (tileX, tileY);
        if (trigger != null) {
          if (IsTriggerActive (trigger)) {
            HandleTriggersUnderEntityOnscreen (trigger);
          }
        }
      }
    }

    private void CheckTileUnderEntityOnscreen_Players_DrawArrows (int tileX, int tileY) {
      Tile tileMap = _mapTiles [tileX, tileY];

      bool checkTrigger = false;
      if (tileMap.Type == (int) Tile.TypesEnum.Empty) {
        checkTrigger = true;
      }
      if (tileMap.Type == (int) Tile.TypesEnum.Wall) {
      }
      if (tileMap.Type == (int) Tile.TypesEnum.Stair) {
        checkTrigger = true;
      }
      if (tileMap.Type == (int) Tile.TypesEnum.Scripted) {
      }
      Cover cover = FindCover (tileX, tileY);
      if (cover == null) {
        if (tileMap.Type == (int) Tile.TypesEnum.Cover) {
          cover = FindTopLeftCover (tileX, tileY);
        }
      }
      //Cover cover = FindTopLeftCover (tileX, tileY);
      if (cover != null) {
        if (cover.IsPortal) {
          int real = 0;
          if (IsPortalOpenedAndActive (cover)) {
            real = 1;
          }
          //if (cover.Misc_A == 0x03) {
          //  real = 1;
          //}
          int mapX, mapY, coverMapX, coverMapY;
          _gridMap.TileToMap (tileX, tileY, out mapX, out mapY);
          _gridMap.TileToMap (cover.X, cover.Y, out coverMapX, out coverMapY);
          _gridMap.DrawArrowMap (pensCover [real], mapX, mapY, coverMapX, coverMapY);

          Element00 element00 = LevsdatBin.block00_Levels [(int) _version].elements00 [_levelIndex];
          Element00_C_Portal portal = element00.GetElement00_C_Portal (cover.Misc_PortalIndex);
          if (portal.LevelIndex == _levelIndex) {
            Element00_B_SpawnLocation spawnLocation = element00.elements00_B_SpawnLocations [portal.spawnLocationIndex];
            _gridMap.DrawArrowMap (pensTeleporter [real], coverMapX, coverMapY, spawnLocation.x, spawnLocation.y);
          }
        }
      }
      if (tileMap.Type == (int) Tile.TypesEnum.Item) {
        //if (tileMap.HasTrigger == 0) {
        checkTrigger = true;
        //}
      }
      checkTrigger = true;
      if (checkTrigger) {
        Trigger trigger = FindTrigger (tileX, tileY);
        if (trigger != null) {
          int real = 0;
          if (IsTriggerActive (trigger)) {
            real = 1;
          }
          HandleTriggersUnderEntityOnscreen_DrawArrows (real, trigger);
        }
      }
    }

    private void CheckTileUnderEntityOnscreen_Players_DrawArrows_Reverse (int tileX, int tileY) {
      Tile tileMap = _mapTiles [tileX, tileY];
      Cover cover = FindCover (tileX, tileY);
      if (cover == null) {
        if (tileMap.Type == (int) Tile.TypesEnum.Cover) {
          cover = FindTopLeftCover (tileX, tileY);
        }
      }
      if (cover != null) {
        int coverMapX, coverMapY;
        _gridMap.TileToMap (cover.X, cover.Y, out coverMapX, out coverMapY);
        if (cover.LinkId27b != 0) {
          HandleLinkIdForScriptedsAndCovers_DrawArrows_Reverse (-cover.LinkId27b, coverMapX, coverMapY);
        }
      }
    }

    private void HandleTriggersUnderEntityOnscreen (Trigger trigger) {
      //if (tileMap.HasTrigger == 0) {
      //  return;
      //}
      //if (trigger.ConditionToTrigger != 0) {
      //  throw new Exception ();
      //  // TODO
      //}

      int linkId = trigger.LinkId04w;
      HandleLinkIdForScriptedsAndCovers (linkId);
      HandleLinkIdForEntitiesOffscreenAndOnscreen (linkId);

      List<Trigger> triggersTemp = new List<Trigger> (_triggers);
      foreach (Trigger trigger2 in triggersTemp) {
        if (trigger.LinkId04w == trigger2.LinkId04w) {
          Tile tileMap = _mapTiles [trigger2.X, trigger2.Y];
          tileMap.HasTrigger = 0;
          _triggers.Remove (trigger2);
        }
      }
    }

    private void HandleTriggersUnderEntityOnscreen_DrawArrows (int real, Trigger trigger) {
      //Tile tileMap = _mapTiles [tileX, tileY];
      //if (tileMap.HasTrigger == 0) {
      //  pen = penTriggerDark;
      //}
      if (trigger.ConditionToTrigger != 0) {
        //pen = penTriggerDark;
      }

      //Color color = Color.FromArgb (125, pensTrigger [real].Color);
      //Brush brush = new SolidBrush (color);
      //foreach (Trigger trigger2 in _triggers) {
      //  if (trigger.LinkId04w == trigger2.LinkId04w) {
      //    _gridMap.FillRectangleTile (brush, trigger2.X, trigger2.Y, 1, 1);
      //  }
      //}

      int mapX, mapY;
      _gridMap.TileToMap (trigger.X, trigger.Y, out mapX, out mapY);
      int linkId = trigger.LinkId04w;
      HandleLinkIdForScriptedsAndCovers_DrawArrows (real, linkId, mapX, mapY);
      HandleLinkIdForEntitiesOffscreenAndOnscreen_DrawArrows (real, linkId, mapX, mapY);
    }

    private void HandleTriggersUnderEntityOnscreen_DrawArrows_ReversePost (Trigger trigger, int linkId, int mapX, int mapY) {
      if (linkId == trigger.LinkId04w) {

        if (trigger.ConditionToTrigger != 0) {
          //pen = penTriggerDark;
        }

        int real = 0;
        if (IsTriggerActive (trigger)) {
          real = 1;
        }
        //Color color = Color.FromArgb (125, pensTrigger [real].Color);
        //Brush brush = new SolidBrush (color);
        //_gridMap.FillRectangleTile (brush, trigger.X, trigger.Y, 1, 1);
        int triggerMapX, triggerMapY;
        _gridMap.TileToMap (trigger.X, trigger.Y, out triggerMapX, out triggerMapY);
        _gridMap.DrawArrowMap (pensTrigger [real], triggerMapX, triggerMapY, mapX, mapY);
      }
    }

    private void HandleLinkIdForScriptedsAndCovers (int linkId) {
      //List<Scripted> scriptedsToRemove = new List<Scripted> ();
      foreach (Scripted scripted in new List<Scripted> (_scripteds)) {
        if (linkId == scripted.linkId18_w) {
          scripted.SetCurrentScript (4);
          scripted.linkId18_w = 0;
          // kills by event
          scripted.linkId56_w = 0;
          DoScript (scripted);
        }
        else if (linkId == -scripted.linkId56_w) {
          // activates by event
          scripted.linkId56_w = 0;
          DoScript (scripted);
        }
      }
      //foreach (Scripted scripted in scriptedsToRemove) {
      //  _scripteds.Remove (scripted);
      //}

      foreach (Cover cover in new List<Cover> (_covers)) {
        if (cover.IsClassic) {
          if (linkId == -cover.LinkId27b) {
            if (cover.InitialState == 1) { // OFF
              Template template = _templates [cover.TemplateIndex];
              CopyTemplateTo3x3Tiles_SetDstBit15TL (template, cover.X, cover.Y);
            }
            else { // ON
              Template template = cover.BackedUpTemplate;
              CopyTemplateTo3x3Tiles (template, cover.X, cover.Y);
            }
            // hack to hide cover, /!\ for portal: should be kept to get portal infos when door opened?
            _covers.Remove (cover);
            //cover.Raw26b = 0;
            //cover.Raw27b = 0;
          }
        }
      }
      //foreach (Cover cover in coversToRemove) {
      //  _covers.Remove (cover);
      //}
    }

    private Dictionary<int, bool?> _handleLinkIdForScriptedsAndCovers_DrawArrows_linkIds = new Dictionary<int, bool?> ();
    private void HandleLinkIdForScriptedsAndCovers_DrawArrows (int real, int linkId, int mapX, int mapY) {
      if (_handleLinkIdForScriptedsAndCovers_DrawArrows_linkIds.ContainsKey (linkId)) {
        return;
      }
      _handleLinkIdForScriptedsAndCovers_DrawArrows_linkIds.Add (linkId, null);
      foreach (Scripted scripted in new List<Scripted> (_scripteds)) {
        if (linkId == scripted.linkId18_w) {
          int mapX1, mapY1;
          _gridMap.TileToMap (scripted.X, scripted.Y, out mapX1, out mapY1);
          _gridMap.DrawArrowMap (pensDeath [real], mapX, mapY, mapX1, mapY1);
          CheckActivateScripted_DrawArrows (scripted);
        }
        else if (linkId == -scripted.linkId56_w) {
          int mapX1, mapY1;
          _gridMap.TileToMap (scripted.X, scripted.Y, out mapX1, out mapY1);
          _gridMap.DrawArrowMap (pensScripted [real], mapX, mapY, mapX1, mapY1);
          CheckActivateScripted_DrawArrows (scripted);
        }
      }

      foreach (Cover cover in new List<Cover> (_covers)) {
        if (cover.IsClassic) {
          if (linkId == -cover.LinkId27b) {
            int mapX1, mapY1;
            _gridMap.TileToMap (cover.X, cover.Y, out mapX1, out mapY1);
            _gridMap.DrawArrowMap (pensCover [real], mapX, mapY, mapX1, mapY1);
          }
        }
        //else {
        //  if (linkId == -cover.LinkId27b) {
        //    throw new Exception ();
        //    int mapX1, mapY1;
        //    _gridMap.TileToMap (cover.X, cover.Y, out mapX1, out mapY1);
        //    _gridMap.DrawArrowMap (pensCover [real], mapX, mapY, mapX1, mapY1);
        //  }
        //}
      }
    }

    private Dictionary<int, bool?> _handleLinkIdForScriptedsAndCovers_DrawArrows_Reverse_linkIds = new Dictionary<int, bool?> ();
    private void HandleLinkIdForScriptedsAndCovers_DrawArrows_Reverse (int linkId, int mapX, int mapY) {
      if (_handleLinkIdForScriptedsAndCovers_DrawArrows_Reverse_linkIds.ContainsKey (linkId)) {
        return;
      }
      _handleLinkIdForScriptedsAndCovers_DrawArrows_Reverse_linkIds.Add (linkId, null);
      foreach (Scripted scripted in new List<Scripted> (_scripteds)) {
        CheckActivateScripted0E_DrawArrows_ReversePost (scripted, linkId, mapX, mapY);
      }

      foreach (Entity entity in new List<Entity> (_entities)) {
        HandleLinkIdForEntitiesOffscreenAndOnscreen_DrawArrows_ReversePost (entity, linkId, mapX, mapY);
      }

      foreach (Trigger trigger in new List<Trigger> (_triggers)) {
        HandleTriggersUnderEntityOnscreen_DrawArrows_ReversePost (trigger, linkId, mapX, mapY);
      }
    }

    private void HandleLinkIdForEntitiesOffscreenAndOnscreen (int linkId) {
      foreach (Entity entity in new List<Entity> (_entities)) {
        if (linkId == entity.linkId08_b) {
          entity.linkId08_b = 0;
          if (entity.linkId17_b != 0) {
            HandleLinkIdForScriptedsAndCovers (entity.linkId17_b);
          }
          if (entity.GetEntityClass (_levelIndex).spriteDefIndex == 8) {
            entity.spriteIndex = 3;
          }
          else {
            _entities.Remove (entity);
          }
        }
      }
    }

    private void HandleLinkIdForEntitiesOffscreenAndOnscreen_DrawArrows (int real, int linkId, int mapX, int mapY) {
      foreach (Entity entity in new List<Entity> (_entities)) {
        if (linkId == entity.linkId08_b) {
          if (entity.linkId17_b != 0) {
            HandleLinkIdForScriptedsAndCovers_DrawArrows (real, entity.linkId17_b, entity.mapX, entity.mapY);
          }
          //if (entity.GetEntityClass (_levelIndex).spriteDefIndex == 8) {
          //  throw new Exception ();
          //}
          //else {
          //int mapX1, mapY1;
          //_gridMap.TileToMap (entity.mapX, entity.mapY, out mapX1, out mapY1);
          _gridMap.DrawArrowMap (pensEntity [real], mapX, mapY, entity.mapX, entity.mapY);
          //}
        }
      }
    }

    private void HandleLinkIdForEntitiesOffscreenAndOnscreen_DrawArrows_ReversePost (Entity entity, int linkId, int mapX, int mapY) {
      if (linkId == entity.linkId17_b) {
        _gridMap.DrawArrowMap (pensEntity [1], entity.mapX, entity.mapY, mapX, mapY);
        if (entity.linkId08_b != 0) {
          if (entity.GetEntityClass (_levelIndex).spriteDefIndex == 8) {
            throw new Exception ();
          }
          HandleLinkIdForEntitiesOffscreenAndOnscreen_DrawArrows_ReversePre (entity.linkId08_b, entity.mapX, entity.mapY);
        }
      }
    }

    private void HandleLinkIdForEntitiesOffscreenAndOnscreen_DrawArrows_ReversePre (int linkId, int mapX, int mapY) {
      foreach (Scripted scripted in new List<Scripted> (_scripteds)) {
        CheckActivateScripted0E_DrawArrows_ReversePost (scripted, linkId, mapX, mapY);
      }
      foreach (Trigger trigger in new List<Trigger> (_triggers)) {
        HandleTriggersUnderEntityOnscreen_DrawArrows_ReversePost (trigger, linkId, mapX, mapY);
      }
      foreach (Entity entity in new List<Entity> (_entities)) {
        HandleLinkIdForEntitiesOffscreenAndOnscreen_DrawArrows_ReversePost (entity, linkId, mapX, mapY);
      }
    }

    private void HighlightSameLinkIdOut (int linkId) {
      if (linkId == 0) {
        return;
      }
      foreach (Trigger trigger in new List<Trigger> (_triggers)) {
        if (trigger.LinkId04w == linkId) {
          _gridMap.FillRectangleTile (brushesTrigger [1], trigger.X, trigger.Y, 1, 1);
        }
      }
      foreach (Scripted scripted in new List<Scripted> (_scripteds)) {
        if (scripted.linkId54_w == linkId) {
          _gridMap.FillRectangleTile (brushesScripted [1], scripted.X, scripted.Y, 1, 1);
        }
        //if (scripted.linkId17_b == linkId) {
        //  _gridMap.FillRectangleTile (brushesScripted [1], scripted.X, scripted.Y, 1, 1);
        //}
      }
      foreach (Entity entity in new List<Entity> (_entities)) {
        if (entity.linkId17_b == linkId) {
          int w, h;
          entity.GetSize (_levelIndex, _paletteIndex, out w, out h);
          _gridMap.FillRectangleMap (brushesEntity [1], entity.mapX, entity.mapY, w, h, GridMap.ImageAlignment.Near, GridMap.ImageAlignment.Far);
        }
      }
    }

    private bool IsTriggerActive (Trigger trigger) {
      if (_mapTiles [trigger.X, trigger.Y].HasTrigger == 0) {
        return (false);
      }
      else {
        if (trigger.ConditionToTriggerType == 1) {
          if (!_spawnLocationsNbUsed.ContainsKey (trigger.Param)) {
            return (false);
          }
        }
        else if (trigger.ConditionToTriggerType == 2) {
          if (!_spawnLocationsNbUsed.ContainsKey (1)) {
            return (false);
          }
          else {
            if (_spawnLocationsNbUsed [1] < trigger.Param) {
              return (false);
            }
          }
        }
      }
      return (true);
    }

    private bool IsCoverActive (Cover cover) {
      //if ((cover._raw26 != 0) || (cover.linkId27_b != 0)) {
      //  real = 1;
      //}
      if (cover.IsClassic) {
        if (cover.LinkId27b < 0) {
          return (true);
        }
      }
      //if (cover.IsInactivePortal) {
      //  if (cover.NbNodesToActivate > 0) {
      //    //real = 1;
      //  }
      //  if (cover.NbNodesToActivate < 0) {
      //    //throw new Exception ();
      //  }
      //}
      if (IsPortalOpenedAndActive (cover)) {
        return (true);
      }
      return (false);
    }

    private bool IsPortalOpenedAndActive (Cover cover) {
      if (cover.IsOpenedPortal) {
        Tile tile = _mapTiles [cover.X, cover.Y];
        if (tile.Type == (int) Tile.TypesEnum.Cover) {
          return (true);
        }
      }
      return (false);
    }

    private bool IsScriptedActive (Scripted scripted) {
      if (scripted.linkId56_w < 0) {
        return (false);
      }
      return (true);
    }

    private void HighlightSameLinkIdIn (int linkId) {
      if (linkId == 0) {
        return;
      }
      foreach (Scripted scripted in new List<Scripted> (_scripteds)) {
        if (scripted.linkId18_w == linkId) {
          _gridMap.FillRectangleTile (brushesScripted [1], scripted.X, scripted.Y, 1, 1);
        }
        if (!IsScriptedActive (scripted)) {
          if (scripted.linkId56_w < 0) {
            if (-scripted.linkId56_w == linkId) {
              _gridMap.FillRectangleTile (brushesScripted [1], scripted.X, scripted.Y, 1, 1);
            }
          }
        }
      }
      foreach (Entity entity in new List<Entity> (_entities)) {
        if (entity.linkId08_b == linkId) {
          int w, h;
          entity.GetSize (_levelIndex, _paletteIndex, out w, out h);
          _gridMap.FillRectangleMap (brushesEntity [1], entity.mapX, entity.mapY, w, h, GridMap.ImageAlignment.Near, GridMap.ImageAlignment.Far);
        }
      }
      foreach (Cover cover in new List<Cover> (_covers)) {
        if (cover.LinkId27b < 0) {
          if (-cover.LinkId27b == linkId) {
            _gridMap.FillRectangleTile (brushesCover [1], cover.X, cover.Y, 1, 1);
          }
        }
      }
    }

    public void DrawMapBase () {
      Check ();
      _gridMap.Clear (Color.Black);

      // BlockA_Tiles
      for (int y = 0; y < _mapHeight; y++) {
        for (int x = 0; x < _mapWidth; x++) {
          Tile tile = _mapTiles [x, y];
          tile.Draw (x, y, this, _gridMap);
          //if (tile.Unk == 1) {
          //  _gridMap.SetTextAlignment (StringAlignment.Center, StringAlignment.Center);
          //  _gridMap.DrawStringTile (Brushes.White, "X", x, y, 8, 8);
          //}
          //if (tile.Type == (int) Tile.TypesEnum.Scripted) {
          //  _gridMap.SetTextAlignment (StringAlignment.Center, StringAlignment.Center);
          //  _gridMap.DrawStringTile (Brushes.White, "Scripted", x, y, 8, 8);
          //}
          //else if (tile.Type == (int) Tile.TypesEnum.Item) {
          //  //    Item item = FindItem (x, y);
          //  //Bitmap bitmap = GetBitmapFromTile (item.);
          //  tile.Draw (x, y, this, _gridMap);
          //}
          //else {
          //  //throw new Exception ();
          //}
          //Bitmap bitmap = GetBitmapFromTile (tile.index);
          //if (bitmap != null) {
          //  _g.DrawImage (bitmap, x * (16 + _space) + _space, y * (16 + _space) + _space);
          //  //DrawString (string.Format ("{0:X2}", tile.index), x, y, 0, 4, Brushes.White);
          //  //DrawString (tile.height.ToString (), x, y, 0, 4, Brushes.White);
          //  //DrawString (tile.type.ToString (), x, y, 0, 4, Brushes.White);
          //  //int offset = _offsetBlockA_Tiles + 6 + (y * _mapWidth + x) * 2;
          //  //if ((offset % 4) == 0) {
          //  //  DrawString (string.Format ("{0:X}", offset), x, y, -4, 4, Brushes.White);
          //  //}
          //  // 0x2600 in file cas
          //}
          //else {
          //  _g.FillRectangle (Brushes.Purple, x * (16 + _space) + _space, y * (16 + _space) + _space, 16, 16);
          //}
          //DrawString (tile.type.ToString (), x, y, 0, 0, Brushes.White);
        }
      }

      // BlockF_Items
      if (false) {
        foreach (Item item in new List<Item> (_items)) {
          item.CurrentTile.Draw (item.X, item.Y, this, _gridMap);
          //Bitmap bitmap = GetBitmapFromTile (elementF.tile.index);
          //if (bitmap != null) {
          //  _g.DrawImage (bitmap, elementF.x * (16 + _space) + _space, elementF.y * (16 + _space) + _space);
          _gridMap.DrawRectangleTile (Pens.Pink, item.X, item.Y, 1, 1);
          //  //FillRectangle (Brushes.Pink, elementF.x, elementF.y, 1, 1);
          //}
          //else {
          //  FillRectangle (Brushes.Pink, elementF.x, elementF.y, 1, 1);
          //  //DrawString (string.Format ("{0:X2}", elementF.tile.index), elementF.x, elementF.y, 0, 2, Brushes.White);
          //  //DrawString (elementF.tile.type.ToString (), elementF.x, elementF.y, 0, 2, Brushes.White);
          //}
          //DrawString (string.Format ("{0:X2}", elementF.tile.index), elementF.x, elementF.y, 0, 4, Brushes.White);
          //_gridMap.SetTextAlignment (StringAlignment.Far, StringAlignment.Far);
          //_gridMap.DrawStringTile (Brushes.Pink, item.ItemIndex.ToString (), item.x, item.y, 16, 16);
          //DrawString (elementF.tile.type.ToString (), elementF.x, elementF.y, 0, 2, Brushes.White);
        }
      }

      if (true) {
        if (_showHints) {
          Color color = Color.FromArgb (96, pensDeath [1].Color);
          Brush brush = new SolidBrush (color);
          for (int y = 0; y < _mapHeight; y++) {
            for (int x = 0; x < _mapWidth; x++) {
              Tile tile = _mapTiles [x, y];
              if (tile.Type == (int) Tile.TypesEnum.Scripted) {
                int tileX2, tileY2;
                if (FindHitLocation (x, y, out tileX2, out tileY2)) {
                  Scripted scripted = FindScripted (tileX2, tileY2);
                  if (scripted != null) {
                    _gridMap.FillRectangleTile (brush, x, y, 1, 1);
                  }
                }
              }
              else if (tile.Type == (int) Tile.TypesEnum.Cover) {
                int tileX2, tileY2;
                if (FindHitLocation (x, y, out tileX2, out tileY2)) {
                  Cover cover = FindCover (tileX2, tileY2);
                  if (cover != null) {
                    if ((cover.Raw26b != 0) || (cover.Raw27b != 0)) {
                      if (cover.Misc_A <= 0x01) {
                        if (cover.Misc_B == 0) {
                          _gridMap.FillRectangleTile (brush, x, y, 1, 1);
                        }
                      }
                    }
                  }
                }
              }
            }
          }
        }
      }

      if (true) {
        foreach (Entity entity in new List<Entity> (_entities)) {
          Element00_A_EntityClass entityClass = entity.GetEntityClass (_levelIndex);
          //SpriteBin spriteBin = SpritesManager.GetSpriteBin (entityClass.SpriteDef.spriteFileDefAddress, _levelIndex, _palette.Index);
          bool isDefault;
          Bitmap bitmap = entity.GetBitmap (_levelIndex, _paletteIndex, out isDefault);
          //bool showName = true;
          //if (spriteBin != null) {
          //  bitmap = spriteBin.GetBitmap (0);
          //  if (entity.spriteIndex != null) {
          //    showName = false;
          //    bitmap = spriteBin.GetBitmap ((int) entity.spriteIndex);
          //  }
          //}
          //if (bitmap != null) {
          _gridMap.DrawImageMap (bitmap, entity.mapX, entity.mapY, GridMap.ImageAlignment.Near, GridMap.ImageAlignment.Far);
          //}
          //else {
          //  _gridMap.DrawCircleMap (Pens.Blue, entity.mapX, entity.mapY, 12);
          //}
          //Bitmap bitmap = spriteBin.GetBitmap ((int) spriteIndex);
          //}
          //else {
          //  showName = true;
          //  _gridMap.DrawCircleMap (Pens.Cyan, entity.x, entity.y, 24);
          //}
          if (isDefault) {
            if (_showHints) {
              _gridMap.SetTextAlignment (StringAlignment.Near, StringAlignment.Far);
              _gridMap.DrawStringMap (Brushes.White, entityClass.GetSpriteDef (_version).SpriteFileDef._fileName, entity.mapX, entity.mapY);
            }
          }
        }
      }

      // Items
      if (true) {
        if (_showHints) {
          foreach (Item item in new List<Item> (_items)) {
            Pen pen = pensItem [1];
            if (_mapTiles [item.X, item.Y].Type != (int) Tile.TypesEnum.Item) {
              pen = pensItem [0];
            }
            _gridMap.DrawRectangleTile (pen, item.X, item.Y, 1, 1);
          }
        }
      }

      // Covers
      if (true) {
        if (_showHints) {
          foreach (Cover cover in new List<Cover> (_covers)) {
            int real = 0;
            if (IsCoverActive (cover)) {
              real = 1;
            }
            //Tile tile = _mapTiles [cover.X, cover.Y];
            ////if ((cover._raw26 != 0) || (cover.linkId27_b != 0)) {
            ////  real = 1;
            ////}
            //if (cover.LinkId27b < 0) {
            //  real = 1;
            //}
            //if (cover.Misc_A == 0x02) {
            //  if (cover.NbNodesToActivate > 0) {
            //    //real = 1;
            //  }
            //  if (cover.NbNodesToActivate < 0) {
            //    throw new Exception ();
            //  }
            //}
            //else if (cover.Misc_A == 0x03) {
            //  if (tile.Type == (int) Tile.TypesEnum.Cover) {
            //    real = 1;
            //  }
            //}
            //if ((cover.Raw26b != 0) || (cover.Raw27b != 0)) {
            //if (tile.Type == (int) Tile.TypesEnum.Cover) {
            //}
            //if (cover.InitialState == 1) {
            //  pen = pensCover [0];
            //}
            Pen pen = pensCover [real];
            _gridMap.DrawRectangleTile (pen, cover.X, cover.Y, 1, 1);
            //}
            if (cover.IsClosedPortal) {
              if (cover.NbNodesToActivate > 0) {
                _gridMap.SetTextAlignment (StringAlignment.Center, StringAlignment.Center);
                _gridMap.DrawStringTile (Brushes.White, string.Format ("N: {0}", cover.NbNodesToActivate), cover.X, cover.Y, 8, 8);
              }
            }
            else if (cover.IsOpenedPortal) {
              Element00 element00 = LevsdatBin.block00_Levels [(int) _version].elements00 [_levelIndex];
              Element00_C_Portal portal = element00.GetElement00_C_Portal (cover.Misc_PortalIndex);
              if (portal.LevelIndex == _levelIndex) {
                _gridMap.SetTextAlignment (StringAlignment.Center, StringAlignment.Center);
                _gridMap.DrawStringTile (Brushes.White, string.Format ("S: {0}", portal.spawnLocationIndex), cover.X, cover.Y, 8, 8);
              }
              else {
                _gridMap.SetTextAlignment (StringAlignment.Center, StringAlignment.Near);
                _gridMap.DrawStringTile (Brushes.White, string.Format ("L: {0}", portal.LevelIndex), cover.X, cover.Y, 8, 0);
                _gridMap.SetTextAlignment (StringAlignment.Center, StringAlignment.Far);
                _gridMap.DrawStringTile (Brushes.White, string.Format ("S: {0}", portal.spawnLocationIndex), cover.X, cover.Y, 8, 16);
              }
            }
          }
        }
      }

      // Scripted
      if (true) {
        if (_showHints) {
          foreach (Scripted scripted in new List<Scripted> (_scripteds)) {
            int real = 0;
            if (IsScriptedActive (scripted)) {
              real = 1;
            }
            Pen pen = pensScripted [real];
            _gridMap.DrawRectangleTile (pen, scripted.X, scripted.Y, 1, 1);
          }
        }
      }

      // Triggers
      if (true) {
        if (_showHints) {
          foreach (Trigger trigger in new List<Trigger> (_triggers)) {
            int real = 0;
            if (IsTriggerActive (trigger)) {
              real = 1;
            }
            //if (_mapTiles [trigger.X, trigger.Y].HasTrigger == 0) {
            //  real = 0;
            //}
            //else {
            //  if (trigger.ConditionToTriggerType == 1) {
            //    if (!_spawnLocationsNbUsed.ContainsKey (trigger.Param)) {
            //      real = 0;
            //    }
            //  }
            //  else if (trigger.ConditionToTriggerType == 2) {
            //    if (!_spawnLocationsNbUsed.ContainsKey (1)) {
            //      real = 0;
            //    }
            //    else {
            //      if (_spawnLocationsNbUsed [1] < trigger.Param) {
            //        real = 0;
            //      }
            //    }
            //  }
            //}
            Pen pen = pensTrigger [real];
            _gridMap.DrawRectangleTile (pen, trigger.X, trigger.Y, 1, 1);
            if (trigger.ConditionToTriggerType != 0) {
              _gridMap.SetTextAlignment (StringAlignment.Center, StringAlignment.Center);
              _gridMap.DrawStringTile (Brushes.White, string.Format ("C{0}", trigger.ConditionToTriggerType), trigger.X, trigger.Y, 8, 8);
            }
            //_gridMap.DrawStringTile (Brushes.White, elementD.triggeredLinkId.ToString (), elementD.x, elementD.y, 1, 0);
            //_gridMap.SetTextAlignment (StringAlignment.Near, StringAlignment.Far);
            //if ((elementD._rawUnk06 >= 1) && (elementD._rawUnk06 <= 8)) {
            //  int spawnLocationIndex = elementD._rawUnk06 - 1;
            //  _gridMap.DrawStringTile (Brushes.Yellow, string.Format ("S: {0}", spawnLocationIndex), elementD.x, elementD.y, 0, 16);
            //}
            //else if ((elementD._rawUnk06 >= 9) && (elementD._rawUnk06 <= 16)) {
            //  int nb = elementD._rawUnk06 - 9;
            //  _gridMap.DrawStringTile (Brushes.Yellow, string.Format ("S1x{0}", nb), elementD.x, elementD.y, 0, 16);
            //}
            //else if ((elementD._rawUnk06 >= 17) && (elementD._rawUnk06 <= 255)) {
            //  throw new Exception ();
            //}
          }
        }
      }

      // Entities
      if (true) {
        if (_showHints) {
          foreach (Entity entity in new List<Entity> (_entities)) {
            Pen pen = pensEntity [0];
            if ((entity.linkId08_b != 0) || (entity.linkId17_b != 0)) {
              pen = pensEntity [1];
            }
            int w, h;
            entity.GetSize (_levelIndex, _paletteIndex, out w, out h);
            _gridMap.DrawRectangleMap (pen, entity.mapX, entity.mapY, w, h, GridMap.ImageAlignment.Near, GridMap.ImageAlignment.Far);
            //_gridMap.SetTextAlignment (StringAlignment.Near, StringAlignment.Near);
            //_gridMap.DrawStringTile (Brushes.White, elementD.triggeredLinkId.ToString (), elementD.x, elementD.y, 1, 0);
            //_gridMap.SetTextAlignment (StringAlignment.Near, StringAlignment.Far);
            //if ((elementD._rawUnk06 >= 1) && (elementD._rawUnk06 <= 8)) {
            //  int spawnLocationIndex = elementD._rawUnk06 - 1;
            //  _gridMap.DrawStringTile (Brushes.Yellow, string.Format ("S: {0}", spawnLocationIndex), elementD.x, elementD.y, 0, 16);
            //}
            //else if ((elementD._rawUnk06 >= 9) && (elementD._rawUnk06 <= 16)) {
            //  int nb = elementD._rawUnk06 - 9;
            //  _gridMap.DrawStringTile (Brushes.Yellow, string.Format ("S1x{0}", nb), elementD.x, elementD.y, 0, 16);
            //}
            //else if ((elementD._rawUnk06 >= 17) && (elementD._rawUnk06 <= 255)) {
            //  throw new Exception ();
            //}
          }
        }
      }

      // BlockB_Templates
      //if (false) {
      //  int xo = 0;
      //  int yo = 0;
      //  foreach (ElementB_Templates elementB in _elementsB_Templates) {
      //    _gBase.DrawRectangle (Pens.White, xo * 3 * 16 + xo, yo * 3 * 16 + yo, 3 * 16 + 1, 3 * 16 + 1);
      //    for (int y = 0; y < 3; y++) {
      //      for (int x = 0; x < 3; x++) {
      //        Tile tile = elementB.tiles [x, y];
      //        if (tile != null) {
      //          Bitmap bitmap = GetBitmapFromTile (tile.index);
      //          if (bitmap != null) {
      //            _gBase.DrawImage (bitmap, 1 + xo * 3 * 16 + xo + x * 16, 1 + yo * 3 * 16 + yo + y * 16);
      //          }
      //          else {
      //            _gBase.FillRectangle (Brushes.Black, 1 + xo * 3 * 16 + xo + x * 16, 1 + yo * 3 * 16 + yo + y * 16, 16, 16);
      //          }
      //        }
      //      }
      //    }
      //    xo++;
      //    if ((xo * 3 * 16 + xo + 3 * 16 + 1) > _bitmapBase.Width) {
      //      xo = 0;
      //      yo++;
      //    }
      //  }
      //}

    }

    public void DrawMap () {
      //List<Trigger> triggersTemp = new List<Trigger> (_triggers);
      //List<Cover> coversTemp = new List<Cover> (_covers);
      //List<Scripted> scriptedsTemp = new List<Scripted> (_scripteds);
      //List<Entity> entitiesTemp = new List<Entity> (_entities);
      //List<Item> itemsTemp = new List<Item> (_items);
      _gridMap.RestoreBackground ();

      //Trigger triggerHover = null;
      //foreach (Trigger elementD in triggersTemp) {
      //  if ((elementD.x == _tileX) && (elementD.y == _tileY)) {
      //    triggerHover = elementD;
      //    break;
      //  }
      //}

      //Cover coverHover = null;
      //foreach (Cover elementE in coversTemp) {
      //  if ((elementE.x == _tileX) && (elementE.y == _tileY)) {
      //    coverHover = elementE;
      //    break;
      //  }
      //}

      //Scripted scriptedHover = null;
      //foreach (Scripted elementC in scriptedsTemp) {
      //  if ((elementC.x == _tileX) && (elementC.y == _tileY)) {
      //    scriptedHover = elementC;
      //    break;
      //  }
      //}


      if (true) {
        // Spawn Locations
        bool highlightSpawnLocation = false;
        Element00_B_SpawnLocation [] spawnLocations = LevsdatBin.block00_Levels [(int) _version].elements00 [_levelIndex].elements00_B_SpawnLocations;
        for (int i = 0; i < spawnLocations.Length; i++) {
          Element00_B_SpawnLocation spawnLocation = spawnLocations [i];
          int dx = _mapX - spawnLocation.x;
          int dy = _mapY - spawnLocation.y;
          int l = (int) Math.Sqrt (dx * dx + dy * dy);
          if (_showHints) {
            _gridMap.DrawCircleMap (pensTeleporter [0], spawnLocation.x, spawnLocation.y, _spawnLocationSize);
          }
          if (l < (_spawnLocationSize >> 1)) {
            _gridMap.FillCircleMap (brushesTeleporter [1], spawnLocation.x, spawnLocation.y, _spawnLocationSize - 2);
            //_gridMap.DrawCircleMap (pensTeleporter [1], spawnLocation.x, spawnLocation.y, _spawnLocationSize);
            highlightSpawnLocation = true;

            foreach (Cover cover in new List<Cover> (_covers)) {
              if (cover.IsPortal) {
                int real = 0;
                if (IsPortalOpenedAndActive (cover)) {
                  real = 1;
                }
                //if (cover.Misc_A == 0x03) {
                //  real = 1;
                //}

                Element00 element00 = LevsdatBin.block00_Levels [(int) _version].elements00 [_levelIndex];
                Element00_C_Portal portal = element00.GetElement00_C_Portal (cover.Misc_PortalIndex);
                if (portal.LevelIndex == _levelIndex) {
                  //Element00_B_SpawnLocation spawnLocation = element00.elements00_B_SpawnLocations [portal.spawnLocationIndex];
                  if (portal.spawnLocationIndex == i) {
                    int coverMapX, coverMapY;
                    _gridMap.TileToMap (cover.X, cover.Y, out coverMapX, out coverMapY);
                    _gridMap.DrawArrowMap (pensTeleporter [real], coverMapX, coverMapY, spawnLocation.x, spawnLocation.y);
                  }
                }
              }
            }
          }
          if (_showHints) {
            _gridMap.SetTextAlignment (StringAlignment.Center, StringAlignment.Center);
            _gridMap.DrawStringMap (Brushes.White, string.Format ("S: {0}", i), spawnLocation.x, spawnLocation.y);
          }
        }

        if (!highlightSpawnLocation) {
          Trigger trigger = FindTrigger (_tileX, _tileY);
          if (trigger != null) {
            HighlightSameLinkIdOut (trigger.LinkId04w);
          }
          Scripted scripted = FindScripted (_tileX, _tileY);
          if (scripted != null) {
            HighlightSameLinkIdIn (scripted.linkId18_w);
            //HighlightSameLinkIdOut (scripted.linkId17_b);
            if (!IsScriptedActive (scripted)) {
              HighlightSameLinkIdIn (-scripted.linkId56_w);
            }
            HighlightSameLinkIdOut (scripted.linkId54_w);
          }
          Entity entity = FindEntity (_mapX, _mapY);
          if (entity != null) {
            HighlightSameLinkIdIn (entity.linkId08_b);
            HighlightSameLinkIdOut (entity.linkId17_b);
          }
          Cover cover = FindCover (_tileX, _tileY);
          if (cover == null) {
            Tile tileMap = _mapTiles [_tileX, _tileY];
            if (tileMap.Type == (int) Tile.TypesEnum.Cover) {
              cover = FindTopLeftCover (_tileX, _tileY);
            }
          }
          if (cover != null) {
            if (cover.LinkId27b < 0) {
              HighlightSameLinkIdIn (-cover.LinkId27b);
            }
          }

          _handleLinkIdForScriptedsAndCovers_DrawArrows_Reverse_linkIds.Clear ();
          CheckTileUnderEntityOnscreen_Players_DrawArrows_Reverse (_tileX, _tileY);
          _handleLinkIdForScriptedsAndCovers_DrawArrows_Reverse_linkIds.Clear ();
          CheckCollisions_Missiles_vs_EntitiesOnscreen20andPlayers_DrawArrows_Reverse (_mapX, _mapY);
          if (scripted != null) {
            _handleLinkIdForScriptedsAndCovers_DrawArrows_Reverse_linkIds.Clear ();
            CheckActivateScripted_DrawArrows_ReversePre (scripted);
          }

          _handleLinkIdForScriptedsAndCovers_DrawArrows_linkIds.Clear ();
          CheckTileUnderEntityOnscreen_Players_DrawArrows (_tileX, _tileY);
          _handleLinkIdForScriptedsAndCovers_DrawArrows_linkIds.Clear ();
          CheckCollisions_Missiles_vs_EntitiesOnscreen20andPlayers_DrawArrows (_mapX, _mapY);
          _handleLinkIdForScriptedsAndCovers_DrawArrows_linkIds.Clear ();
          CheckTileUnderEntityOnscreenAll_DrawArrows (_tileX, _tileY);
          if (scripted != null) {
            _handleLinkIdForScriptedsAndCovers_DrawArrows_linkIds.Clear ();
            CheckActivateScripted_DrawArrows (scripted);
          }
          //Pen pen = penTriggerLight;
          //if (_mapTiles [triggerHover.x, triggerHover.y].HasTrigger == 0) {
          //  pen = penTriggerDark;
          //}
          //if (triggerHover._rawUnk06 != 0) {
          //  pen = penTriggerDark;
          //}
          //int mapX, mapY;
          //_gridMap.TileToMap (triggerHover.x, triggerHover.y, out mapX, out mapY);
          //HandleLinkIdForScriptedAndCovers_DrawArrows (pen, triggerHover.linkId04_w, mapX, mapY);
          //HandleLinkIdForEntitiesOffscreenAndOnscreen_DrawArrows (pen, triggerHover.linkId04_w, mapX, mapY);

          //foreach (Entity entity in _entities) {
          //  if (triggerHover.linkId04_w == entity.linkId08_b) {
          //    _gridMap.DrawArrowTile (pen, triggerHover.x, triggerHover.y, entity.x, entity.y);
          //  }
          //}
        }
      }

      _gridMap.DrawRectangleTile (Pens.White, _tileX, _tileY, 1, 1);

      // draw templates
      if (false) {
        _gridMap.Clear (Color.Black);
        int nb = 5;
        for (int i = 0; i < _templates.Length; i++) {
          Template template = _templates [i];
          //Bitmap bitmap = _itemsBin.GetBitmap (i);
          int x = (i % nb) * 3;
          int y = (i / nb) * 3;
          template.Draw (x, y, this, _gridMap);
          _gridMap.SetTextAlignment (StringAlignment.Center, StringAlignment.Center);
          _gridMap.DrawStringTile (Brushes.White, i.ToString (), x, y, 8, 8);
        }
      }

      if (false) {
        _gridMap.Clear (Color.Black);
        int w = 150;
        int h = 100;
        int nb = 5;
        for (int i = 0; i < 40; i++) {
          ItemType itemType = Achaos.GetItemType (_version, _indexItemsPack, i);
          Bitmap bitmap = _itemsBin.GetBitmap (i);
          int x = (i % nb) * w + w;
          int y = (i / nb) * h + h;
          itemType.Draw (bitmap, x, y, _gridMap);
        }
      }
      return;




















      // BlockE_Covers
      //if (false) {
      //  foreach (Cover elementE in _covers) {
      //    Template elementB = _templates [elementE.TemplateIndex];
      //    if (((triggerHover != null) && ((-elementE.linkId27_b) == triggerHover.linkId04_w)) ||
      //        ((coverHover != null) && ((-elementE.linkId27_b) == (-coverHover.linkId27_b)))) {
      //      if (elementE.InitialState == 1) {
      //        elementB.Draw (elementE.x, elementE.y, this, _gridMap);
      //      }
      //    }
      //    else {
      //      if (elementE.InitialState == 0) {
      //        elementB.Draw (elementE.x, elementE.y, this, _gridMap);
      //      }
      //    }
      //    _gridMap.DrawRectangleTile (Pens.Yellow, elementE.x, elementE.y, 1, 1);
      //    //DrawString (_gBackBuffer, Brushes.White, elementE.linkId.ToString (), elementE.x, elementE.y, 0, 0);
      //    //DrawString (_gBackBuffer, Brushes.Yellow, elementE.initialStateString, elementE.x, elementE.y, 0, 8);
      //  }

      //  foreach (Cover elementE in _covers) {
      //    int portalIndex = elementE.Unk26_PortalIndex;
      //    if (portalIndex != 0x1F) {
      //      if (elementE.Unk26_A == 2) {
      //        Element00 element00 = LevsdatBin.block00_Levels.elements00 [_levelIndex];
      //        Element00_C_Portal portal = element00.GetElement00_C_Portal (portalIndex);
      //        if (portal.LevelIndex == _levelIndex) {
      //          Element00_B_SpawnLocation spawnLocation = element00.elements00_B_SpawnLocations [portal.spawnLocationIndex];
      //          _gridMap.DrawLineMap (Pens.Yellow, elementE.x * 16 + 8, elementE.y * 16 + 8, spawnLocation.x, spawnLocation.y);
      //        }
      //        else {
      //          _gridMap.SetTextAlignment (StringAlignment.Center, StringAlignment.Near);
      //          _gridMap.DrawStringTile (Brushes.Yellow, string.Format ("L: {0}", portal.LevelIndex), elementE.x, elementE.y, 8, 0);
      //          _gridMap.SetTextAlignment (StringAlignment.Center, StringAlignment.Far);
      //          _gridMap.DrawStringTile (Brushes.Yellow, string.Format ("S: {0}", portal.spawnLocationIndex), elementE.x, elementE.y, 8, 16);
      //        }
      //      }
      //      else {
      //        _gridMap.SetTextAlignment (StringAlignment.Near, StringAlignment.Near);
      //        _gridMap.DrawStringTile (Brushes.White, "?".ToString (), elementE.x, elementE.y, 0, 0);
      //      }
      //    }
      //    else {
      //      _gridMap.SetTextAlignment (StringAlignment.Near, StringAlignment.Near);
      //      _gridMap.DrawStringTile (Brushes.White, elementE.Unk26_A.ToString (), elementE.x, elementE.y, 0, 0);
      //      _gridMap.SetTextAlignment (StringAlignment.Near, StringAlignment.Far);
      //      _gridMap.DrawStringTile (Brushes.Yellow, elementE.Unk26_PortalIndex.ToString (), elementE.x, elementE.y, 0, 16);
      //      _gridMap.SetTextAlignment (StringAlignment.Far, StringAlignment.Far);
      //      _gridMap.DrawStringTile (Brushes.Cyan, elementE.Unk26_B.ToString (), elementE.x, elementE.y, 16, 16);
      //    }
      //  }
      //}

      // BlockD_Triggers to BlockE_Covers links
      //if (false) {
      //  //foreach (ElementD_Triggers elementD in _elementsD_Triggers) {
      //  List<ElementD_Triggers> triggersTemp = new List<ElementD_Triggers> ();
      //  if (cover != null) {
      //    foreach (ElementD_Triggers elementD in _elementsD_Triggers) {
      //      if (elementD.linkId == (-cover.linkId)) {
      //        triggersTemp.Add (elementD);
      //      }
      //    }
      //  }
      //  else if (trigger != null) {
      //    triggersTemp.Add (trigger);
      //  }
      //  foreach (ElementD_Triggers elementD in triggersTemp) {
      //    foreach (ElementE_Covers elementE in _elementsE_Covers) {
      //      if ((-elementE.linkId) == elementD.linkId) {
      //        //if ((-elementD.linkId) == elementE.linkId) {
      //        Pen pen = Pens.LightGreen;
      //        if ((cover != null) && (elementE != cover)) {
      //          pen = Pens.DarkOliveGreen;
      //        }
      //        DrawLine (_gBackBuffer, pen, elementD.x, elementD.y, elementE.x, elementE.y);
      //      }
      //    }
      //  }
      //}

      // BlockE_Covers & BlockD_Triggers links
      //if (false) {
      //  int linkId = -1;
      //  if (triggerHover != null) {
      //    linkId = triggerHover.linkId04_w;
      //  }
      //  else if (coverHover != null) {
      //    linkId = -coverHover.linkId27_b;
      //  }
      //  if (linkId != -1) {
      //    Pen pen1 = new Pen (Color.DarkGreen);
      //    Pen pen2 = new Pen (Color.LightGreen);
      //    foreach (Trigger elementD in _triggers) {
      //      if (elementD.linkId04_w == linkId) {
      //        foreach (Cover elementE in _covers) {
      //          if ((-elementE.linkId27_b) == linkId) {
      //            Pen pen = pen1;
      //            if ((triggerHover == elementD) || (coverHover == elementE)) {
      //              pen = pen2;
      //            }
      //            _gridMap.DrawArrowTile (pen, elementD.x, elementD.y, elementE.x, elementE.y);
      //          }
      //        }
      //      }
      //    }
      //  }
      //}

      // BlockC_Scripted & BlockD_Triggers links
      //if (false) {
      //  int linkId = -1;
      //  if (triggerHover != null) {
      //    linkId = triggerHover.linkId04_w;
      //  }
      //  else if (scriptedHover != null) {
      //    linkId = -scriptedHover.linkId56_w;
      //  }
      //  if (linkId != -1) {
      //    Pen pen1 = new Pen (Color.DarkGreen);
      //    Pen pen2 = new Pen (Color.LightGreen);
      //    foreach (Trigger elementD in _triggers) {
      //      if (elementD.linkId04_w == linkId) {
      //        foreach (Scripted elementC in _scripteds) {
      //          if ((-elementC.linkId56_w) == linkId) {
      //            Pen pen = pen1;
      //            if ((triggerHover == elementD) || (scriptedHover == elementC)) {
      //              pen = pen2;
      //            }
      //            _gridMap.DrawArrowTile (pen, elementD.x, elementD.y, elementC.x, elementC.y);
      //          }
      //        }
      //      }
      //    }
      //  }
      //}

      // BlockE_Scripted & BlockD_Triggers links
      //if (false) {
      //  int linkId = -1;
      //  if (triggerHover != null) {
      //    linkId = triggerHover.linkId04_w;
      //  }
      //  else if (scriptedHover != null) {
      //    linkId = scriptedHover.linkId18_b;
      //  }
      //  if (linkId != -1) {
      //    Pen pen1 = new Pen (Color.DarkRed);
      //    Pen pen2 = new Pen (Color.Red);
      //    foreach (Trigger elementD in _triggers) {
      //      if (elementD.linkId04_w == linkId) {
      //        foreach (Scripted elementC in _scripteds) {
      //          if (elementC.linkId18_b == linkId) {
      //            Pen pen = pen1;
      //            if ((triggerHover == elementD) || (scriptedHover == elementC)) {
      //              pen = pen2;
      //            }
      //            _gridMap.DrawArrowTile (pen, elementD.x, elementD.y, elementC.x, elementC.y);
      //          }
      //        }
      //      }
      //    }
      //  }
      //}

      // BlockC_Scripted
      //if (false) {
      //  foreach (Scripted scripted in _scripteds) {
      //    Element16_ScriptClass element16 = LevsdatBin.block16_ScriptsClasses.elements16 [scripted.scriptClassIndex];
      //    if ((_tileX != scripted.x) || (_tileY != scripted.y)) {
      //      int? defaultTemplateIndex = element16.GetDefaultTemplateIndex ();
      //      if (defaultTemplateIndex == null) {
      //        if (scripted.templateIndex >= 0) {
      //          defaultTemplateIndex = scripted.templateIndex;
      //        }
      //      }
      //      if (defaultTemplateIndex != null) {
      //        Template elementB = _templates [(int) defaultTemplateIndex];
      //        elementB.Draw (scripted.x, scripted.y, this, _gridMap);
      //      }
      //    }
      //    else {
      //      Console.WriteLine ();
      //      Console.WriteLine (string.Format ("ScriptClass: {0}", scripted.scriptClassIndex));
      //      //Console.WriteLine (element16.script1.Dump (scripted));
      //      //Console.WriteLine (element16.script2.Dump (scripted));
      //      //Console.WriteLine (element16.script3.Dump (scripted));
      //      //Console.WriteLine (element16.script4.Dump (scripted));

      //      int? templateIndexActivated = element16.GetActivatedTemplateIndex ();
      //      if (templateIndexActivated == null) {
      //        //templateIndexActivated = element16.script2.GetFirstSetTemplateInstruction ();
      //      }
      //      if (templateIndexActivated != null) {
      //        Template elementB = _templates [(int) templateIndexActivated];
      //        elementB.Draw (scripted.x, scripted.y, this, _gridMap);
      //      }
      //      else {
      //        throw new Exception ();
      //      }
      //      if (element16.script2.HasSpawnEntity ()) {
      //        int entityClassIndex = scripted.spawnedEntityClassIndex;
      //        Element00_A_EntityClass element00_A = LevsdatBin.block00_Levels.elements00 [_levelIndex].elements00_A_EntitiesClasses [entityClassIndex];
      //        SpriteBin spriteBin = SpritesManager.GetSpriteBin (element00_A.spriteDefIndex, _palette.Index);
      //        if (spriteBin != null) {
      //          Bitmap bitmap = spriteBin.GetBitmap (0);
      //          //_gridMap.DrawImageTile (bitmap, elementC.x, elementC.y);
      //        }
      //        else {
      //          _gridMap.DrawCircleTile (Pens.Cyan, scripted.x, scripted.y, 24);
      //        }
      //      }
      //    }
      //    _gridMap.DrawRectangleTile (Pens.Orange, scripted.x, scripted.y, 1, 1);
      //    _gridMap.SetTextAlignment (StringAlignment.Near, StringAlignment.Near);
      //    _gridMap.DrawStringTile (Brushes.White, scripted.linkId56_w.ToString (), scripted.x, scripted.y, 0, 0);
      //    _gridMap.SetTextAlignment (StringAlignment.Near, StringAlignment.Far);
      //    _gridMap.DrawStringTile (Brushes.White, scripted.linkId18_b.ToString (), scripted.x, scripted.y, 0, 16);
      //    if (scripted.scriptClassIndex == 48) {
      //      _gridMap.DrawCircleMap (Pens.Beige, scripted.x * 16 + 8, scripted.y * 16 + 8, 32);
      //    }
      //    //DrawString (elementC.templateIndex.ToString (), elementC.x, elementC.y, 0, 0, Brushes.White);
      //    //DrawString (elementC.initialState.ToString (), elementC.x, elementC.y, 0, 8, Brushes.Yellow);
      //  }
      //}

      // BlockG_Musics
      //if (false) {
      //  for (int y = 0; y < (_mapHeight >> 2); y++) {
      //    for (int x = 0; x < (_mapWidth >> 2); x++) {
      //      _gridMap.DrawRectangleTile (Pens.White, x * 4, y * 4, 4, 4);
      //      //_g.DrawRectangle (Pens.White, x * (16 + _space) * 4, y * (16 + _space) * 4, 4 * (16 + _space), 4 * (16 + _space));
      //      //DrawString (_elementsBlockG_MusicsIndex [x, y].ToString (), x * 4, y * 4, 30, 30, Brushes.White);
      //    }
      //  }
      //}

      // Entities
      //if (false) {
      //  foreach (Entity entity in _entities) {
      //    Element00_A_EntityClass entityClass = LevsdatBin.block00_Levels.elements00 [_levelIndex].elements00_A_EntitiesClasses [entity.entityClassIndex];
      //    bool showName = false;
      //    //if (spriteBin != null) {
      //    Bitmap bitmap = entityClass.GetDefaultSprite (_palette.Index);
      //    if (bitmap == null) {
      //      showName = true;
      //      SpriteBin spriteBin = SpritesManager.GetSpriteBin (entityClass.spriteDefIndex, _palette.Index);
      //      bitmap = spriteBin.GetBitmap (0);
      //    }
      //    //Bitmap bitmap = spriteBin.GetBitmap ((int) spriteIndex);
      //    _gridMap.DrawCircleMap (Pens.Blue, entity.x, entity.y, 24);
      //    _gridMap.DrawImageMap (bitmap, entity.x, entity.y, GridMap.ImageAlignment.Near, GridMap.ImageAlignment.Far);
      //    //}
      //    //else {
      //    //  showName = true;
      //    //  _gridMap.DrawCircleMap (Pens.Cyan, entity.x, entity.y, 24);
      //    //}
      //    if (showName) {
      //      _gridMap.DrawStringMap (Brushes.White, entityClass.SpriteDef.SpriteFileDef.fileName, entity.x, entity.y);
      //    }
      //  }
      //}

      // Misc
      //for (int y = 0; y < _mapHeight; y++) {
      //  for (int x = 0; x < _mapWidth; x++) {
      //    if (x % 8 == 0) {
      //      int offset = _offsetBlockA_Tiles + 6 + (y * _mapWidth + x) * 2;
      //      _g.DrawString (string.Format ("{0:X}", offset), FormMain._font, Brushes.White, x * (16 + space) + space, y * (16 + space) + space);
      //    }
      //    int tile = _mapTiles [x, y];
      //    _g.DrawString (string.Format ("{0:X4}", tile), FormMain._font, Brushes.White, x * (16 + space) + space, y * (16 + space) + space + 8);
      //  }
      //}


      if (false) {
        _gridMap.DrawImageWindow (_itemsBin.Bitmap, 0, 0);
      }
    }

    private void CopyTemplateTo3x3Tiles_SetDstBit15TL (Template templateSrc, int xDst, int yDst) {
      for (int y = 0; y < 3; y++) {
        for (int x = 0; x < 3; x++) {
          if (((xDst + x) >= _mapWidth) || ((yDst + y) >= _mapHeight)) {
            continue;
          }
          Tile tileTemplate = templateSrc.tiles [x, y];
          Tile tileMap = _mapTiles [xDst + x, yDst + y];
          if (tileTemplate.Unk == 0) {
            tileMap._00w_tile = tileTemplate._00w_tile;
          }
          if ((x == 0) && (y == 0)) {
            tileMap.Unk = 1;
          }
        }
      }
    }

    private void CopyTemplateTo3x3Tiles (Template templateSrc, int xDst, int yDst) {
      for (int y = 0; y < 3; y++) {
        for (int x = 0; x < 3; x++) {
          if (((xDst + x) >= _mapWidth) || ((yDst + y) >= _mapHeight)) {
            continue;
          }
          Tile tileTemplate = templateSrc.tiles [x, y];
          Tile tileMap = _mapTiles [xDst + x, yDst + y];
          if (tileTemplate.Unk == 0) {
            tileMap._00w_tile = tileTemplate._00w_tile;
          }
        }
      }
    }

    private Item FindItem (int tileX, int tileY) {
      foreach (Item item in new List<Item> (_items)) {
        if ((item.X == tileX) && (item.Y == tileY)) {
          return (item);
        }
      }
      return (null);
    }

    private Scripted FindScripted (int tileX, int tileY) {
      foreach (Scripted scripted in new List<Scripted> (_scripteds)) {
        if ((scripted.X == tileX) && (scripted.Y == tileY)) {
          return (scripted);
        }
      }
      return (null);
    }

    private Cover FindTopLeftCover (int tileX, int tileY) {
      int topLeftX = tileX;
      int topLeftY = tileY;
      for (; ; ) {
        Tile tileMap2 = _mapTiles [topLeftX - 1, topLeftY];
        if (tileMap2.Type != (int) Tile.TypesEnum.Cover) {
          break;
        }
        topLeftX--;
      }
      for (; ; ) {
        Tile tileMap2 = _mapTiles [topLeftX, topLeftY - 1];
        if (tileMap2.Type != (int) Tile.TypesEnum.Cover) {
          break;
        }
        topLeftY--;
      }
      return (FindCover (topLeftX, topLeftY));
    }

    private static int [] _findHitLocationTab = { 0, -1, -1, -1, 0, 0, -1, 0, -2, -2, -1, -2, 0, -2, -2, -1, -2, 0 };
    private bool FindHitLocation (int tileX, int tileY, out int tileX2, out int tileY2) {
      tileX2 = -1;
      tileY2 = -1;

      for (int i = 0; i < 9; i++) {
        int x = tileX + _findHitLocationTab [i * 2 + 0];
        int y = tileY + _findHitLocationTab [i * 2 + 1];
        if ((x >= 0) && (y >= 0)) {
          if (_mapTiles [x, y].Unk == 1) {
            tileX2 = x;
            tileY2 = y;
            return (true);
          }
        }
      }
      return (false);
      // tableau!!
      //if (_mapTiles [tileX, tileY - 1].Unk == 1) {
      //  tileX2 = tileX;
      //  tileY2 = tileY - 1;
      //}
      //else if (_mapTiles [tileX - 1, tileY - 1].Unk == 1) {
      //  tileX2 = tileX - 1;
      //  tileY2 = tileY - 1;
      //}
      //else if (_mapTiles [tileX, tileY].Unk == 1) {
      //  tileX2 = tileX;
      //  tileY2 = tileY;
      //}
      //else if (_mapTiles [tileX - 1, tileY].Unk == 1) {
      //  tileX2 = tileX - 1;
      //  tileY2 = tileY;
      //}
      //else if (_mapTiles [tileX - 2, tileY - 2].Unk == 1) {
      //  tileX2 = tileX - 2;
      //  tileY2 = tileY - 2;
      //}
      //else if (_mapTiles [tileX - 1, tileY - 2].Unk == 1) {
      //  tileX2 = tileX - 1;
      //  tileY2 = tileY - 2;
      //}
      //else if (_mapTiles [tileX, tileY - 2].Unk == 1) {
      //  tileX2 = tileX;
      //  tileY2 = tileY - 2;
      //}
      //else if (_mapTiles [tileX - 2, tileY - 1].Unk == 1) {
      //  tileX2 = tileX - 2;
      //  tileY2 = tileY - 1;
      //}
      //else if (_mapTiles [tileX - 2, tileY].Unk == 1) {
      //  tileX2 = tileX - 2;
      //  tileY2 = tileY;
      //}
      //if (tileX2 == -1) {
      //  return (false);
      //}
      //return (true);
    }

    private Cover FindCover (int tileX, int tileY) {
      foreach (Cover cover in new List<Cover> (_covers)) {
        if ((cover.X == tileX) && (cover.Y == tileY)) {
          return (cover);
        }
      }
      return (null);
    }

    private Trigger FindTrigger (int tileX, int tileY) {
      foreach (Trigger trigger in new List<Trigger> (_triggers)) {
        if ((trigger.X == tileX) && (trigger.Y == tileY)) {
          return (trigger);
        }
      }
      return (null);
    }

    private Entity FindEntity (int mapX, int mapY) {
      foreach (Entity entity in new List<Entity> (_entities)) {
        int w, h;
        entity.GetSize (_levelIndex, _paletteIndex, out w, out h);
        if ((entity.mapX <= mapX) && ((entity.mapX + w) > mapX) &&
            (entity.mapY <= mapY) && ((entity.mapY + h) > mapY)) {
          return (entity);
        }
      }
      return (null);
    }

    public Bitmap GetBitmapFromTile (int tileIndex) {
      Bitmap bitmap = null;
      if (tileIndex < 380) {
        if (_mapTileSet < 4) {
          bitmap = _levxbakBin.GetBitmap (tileIndex);
        }
        else {
          if (tileIndex < 240) {
            bitmap = _lev4bakBin.GetBitmap (tileIndex);
          }
          else {
            bitmap = _levxbakBin.GetBitmap (tileIndex - 240);
          }
        }
      }
      else if (tileIndex < 420) {
        bitmap = _itemsBin.GetBitmap (tileIndex - 380);
      }

      return (bitmap);
    }

    public Bitmap Bitmap {
      get {
        return (_gridMap.Bitmap);
      }
    }

    public Bitmap BitmapBase {
      get {
        return (_gridMap.BitmapBase);
      }
    }

    public bool ShowHints {
      set {
        _gridMap._mapBaseDirty = true;
        _showHints = value;
      }
    }

    public string Description () {
      //int x2 = mouseX / (16 + _space);
      //int y2 = mouseY / (16 + _space);
      //if ((x2 >= _mapWidth) || (y2 >= _mapHeight)) {
      //  return ("");
      //}
      //x = x2;
      //y = y2;

      StringBuilder sb = new StringBuilder ();
      int offset = (_tileY * _mapWidth + _tileX) * 2;
      sb.AppendLine (string.Format ("LevelIndex: {0} ({1}.{2})", _levelIndex, _levelIndex / 4 + 1, _levelIndex % 4 + 1));
      sb.AppendLine (string.Format ("Map: ({0},{1})  Cursor: ({2},{3})", _mapX, _mapY, _tileX, _tileY));
      sb.AppendLine (string.Format ("Offset: {0:X4}", offset));
      sb.AppendLine (string.Format ("Address: {0:X4}", 0xB9C22 + 0x40 + 0x06 + offset));
      sb.AppendLine ();
      sb.AppendLine (_mapTiles [_tileX, _tileY].Description);

      foreach (Scripted scripted in new List<Scripted> (_scripteds)) {
        if ((scripted.X == _tileX) && (scripted.Y == _tileY)) {
          sb.AppendLine (scripted.Description);
          if (scripted.TemplateIndex != 0xFF) {
            sb.AppendLine (_templates [scripted.TemplateIndex].description);
          }
        }
      }


      foreach (Trigger trigger in new List<Trigger> (_triggers)) {
        if ((trigger.X == _tileX) && (trigger.Y == _tileY)) {
          sb.AppendLine (trigger.description);
        }
      }

      foreach (Cover cover in new List<Cover> (_covers)) {
        if ((cover.X == _tileX) && (cover.Y == _tileY)) {
          sb.AppendLine (cover.Description (_version, _levelIndex));
          sb.AppendLine (_templates [cover.TemplateIndex].description);
        }
      }

      foreach (Item item in new List<Item> (_items)) {
        if ((item.X == _tileX) && (item.Y == _tileY)) {
          sb.AppendLine (item.Description);
        }
      }

      foreach (Entity entity in new List<Entity> (_entities)) {
        int w, h;
        entity.GetSize (_levelIndex, _paletteIndex, out w, out h);
        if ((entity.mapX <= _mapX) && ((entity.mapX + w) > _mapX) &&
            (entity.mapY <= _mapY) && ((entity.mapY + h) > _mapY)) {
          sb.AppendLine (entity.Description);
          sb.AppendLine (entity.GetEntityClass (_levelIndex).Description);
        }
      }

      Scripted scripted2 = FindScripted (_tileX, _tileY);
      if (scripted2 != null) {
        Console.WriteLine ();
        Console.WriteLine (string.Format ("ScriptClass: {0}", scripted2.scriptClassIndex));
        Console.WriteLine (string.Format ("Script #1: {0}", scripted2.GetScript (1).Dump (scripted2)));
        Console.WriteLine (string.Format ("Script #2: {0}", scripted2.GetScript (2).Dump (scripted2)));
        Console.WriteLine (string.Format ("Script #3: {0}", scripted2.GetScript (3).Dump (scripted2)));
        Console.WriteLine (string.Format ("Script #4: {0}", scripted2.GetScript (4).Dump (scripted2)));
        Console.WriteLine (string.Format ("Current Script: #{0}", scripted2.CurrentScriptIndex));
        Console.WriteLine (string.Format ("Next Instruction: {0} / {1}", scripted2.CurrentInstruction.DumpHex (), scripted2.CurrentInstruction.Dump (scripted2)));
      }

      return (sb.ToString ());
    }
  }
}
