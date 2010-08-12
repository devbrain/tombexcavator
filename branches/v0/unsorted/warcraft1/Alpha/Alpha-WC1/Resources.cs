// Copyright (C) 2004-2006 Crestez Leonard    cleonard@go.ro
//
// This file is part of Alpha
//
// Alpha is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// Alpha is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Alpha; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

using System;
using System.Collections;
using System.Drawing;

using Alpha.Util;
using Alpha.Graphics;
using Alpha.UI;

// Link to a an archive chunk.
public abstract class ArchiveLink: LazyLink
{
    public readonly WarArchive Archive;

    public readonly int Index;

    public ArchiveLink(bool weak, WarArchive archive, int index): base(weak)
    {
        this.Archive = archive;
        this.Index = index;
    }
}

// Palette Link.
public class PaletteLink: ArchiveLink
{
    // Initializing constructor.
    public PaletteLink(bool weak, WarArchive archive, int index):
        base(weak, archive, index)
    {
    }

    protected override object Create()
    {
        return Archive.GetPaletteChunk(Index);
    }
}

// This is a palette obtained from two others, using CombinePalette
public class CombinePaletteLink: LazyLink
{
    public readonly ILazyLink FirstPalette;

    public readonly ILazyLink SecondPalette;

    // Initializing constructor.
    public CombinePaletteLink(bool weak, ILazyLink firstPal, ILazyLink secondPal): base(weak)
    {
        FirstPalette = firstPal;
        SecondPalette = secondPal;
    }

    protected override object Create()
    {
        Color[] FirstPal = FirstPalette.Get() as Color[];
        Color[] SecondPal = SecondPalette.Get() as Color[];
        return WarArchive.CombinePalette(FirstPal, SecondPal);
    }
}

// This is a palette obtained from two others, using AppendPalette
public class AppendPaletteLink: LazyLink
{
    public readonly ILazyLink FirstPalette;

    public readonly ILazyLink SecondPalette;

    // Initializing constructor.
    public AppendPaletteLink(bool weak, ILazyLink firstPal, ILazyLink secondPal): base(weak)
    {
        FirstPalette = firstPal;
        SecondPalette = secondPal;
    }

    protected override object Create()
    {
        Color[] FirstPal = FirstPalette.Get() as Color[];
        Color[] SecondPal = SecondPalette.Get() as Color[];
        return WarArchive.AppendPalette(FirstPal, SecondPal);
    }
}

// Image Link
public class ImageLink: ArchiveLink
{
    public readonly ILazyLink Palette;

    // Initializing constructor.
    public ImageLink(bool weak, WarArchive archive, int index, ILazyLink palette):
        base(weak, archive, index)
    {
        Palette = palette;
    }

    protected override object Create()
    {
        return Archive.GetImageChunk(Index, (Color[])Palette.Get());
    }
}

// Sprite Link
public class SpriteLink: ArchiveLink
{
    public readonly ILazyLink Palette;

    // Initializing constructor.
    public SpriteLink(bool weak, WarArchive archive, int index, ILazyLink palette):
        base(weak, archive, index)
    {
        Palette = palette;
    }

    protected override object Create()
    {
        return Archive.GetSpriteChunk(Index, (Color[])Palette.Get());
    }
}

// Tileset Link
public class TilesetLink: ArchiveLink
{
    public readonly ILazyLink Palette;

    // Initializing constructor.
    public TilesetLink(bool weak, WarArchive archive, int index, ILazyLink palette):
        base(weak, archive, index)
    {
        Palette = palette;
    }

    protected override object Create()
    {
        return Archive.GetTilesetChunk(Index, (Color[])Palette.Get());
    }
}

// Cursor Link
public class CursorLink: ArchiveLink
{
    public readonly ILazyLink Palette;

    // Initializing constructor.
    public CursorLink(bool weak, WarArchive archive, int index, ILazyLink palette):
        base(weak, archive, index)
    {
        Palette = palette;
    }

    protected override object Create()
    {
        return Archive.GetCursorChunk(Index, (Color[])Palette.Get());
    }
}

public class ResourceManager
{
    // The archive to load resources from.
    public readonly WarArchive Archive;

    // The big resource hashtable.
    // maps strings to lazylinks
    private Hashtable ResourceTable;

    // Resource remappers for various tilesets.
    private Hashtable GrassTilesetRemap;
    private Hashtable SwampTilesetRemap;
    private Hashtable DungeonTilesetRemap;

    // Resource remappers for the different user interfaces.
    private Hashtable HumanUIRemap;
    private Hashtable OrcUIRemap;

    // Get an object.
    // Transparently handles lazylinks
    public object Get(string key)
    {
        return GetLink(key).Get();
    }
 
    // Get an object. Will return the actual links
    public ILazyLink GetLink(string key)
    {
        if (!ResourceTable.Contains(key)) {
            string msg = String.Format("Resource {0} doesn't exist", key);
            throw new ArgumentException(msg, "key");
        }
        return (ILazyLink)ResourceTable[key];
    }

    // Does tileset remap.
    public ILazyLink Remap(ILazyLink res, Tileset tset)
    {
        if (tset == Tileset.Grass && GrassTilesetRemap.Contains(res)) {
            return (ILazyLink)GrassTilesetRemap[res];
        }
        if (tset == Tileset.Swamp && SwampTilesetRemap.Contains(res)) {
            return (ILazyLink)SwampTilesetRemap[res];
        }
        if (tset == Tileset.Dungeon && DungeonTilesetRemap.Contains(res)) {
            return (ILazyLink)DungeonTilesetRemap[res];
        }

        return res;
    }

    // Does race ui remap
    public ILazyLink Remap(ILazyLink res, Race race)
    {
        if (race == Race.Human && HumanUIRemap.Contains(res)) {
            return (ILazyLink)HumanUIRemap[res];
        }
        if (race == Race.Orc && OrcUIRemap.Contains(res)) {
            return (ILazyLink)OrcUIRemap[res];
        }

        return res;
    }

    // Creates the resource manager.
    public ResourceManager(WarArchive archive)
    {
        this.Archive = archive;

        // Create the big resource table.
        ResourceTable = new Hashtable();

        // Create remap tables
        GrassTilesetRemap = new Hashtable();
        SwampTilesetRemap = new Hashtable();
        DungeonTilesetRemap = new Hashtable();
        HumanUIRemap = new Hashtable();
        OrcUIRemap = new Hashtable();

        // Start loading
        LoadPalettes();
        LoadGraphics();
        LoadAnimations();
        LoadUI();
        LoadCursors();
        LoadIcons();
    }

    // Adds an object to the resource manager
    // if val is not a LazyLink it creates a DirectLink to val and adds that.
    private void Add(string key, object val)
    {
        if (ResourceTable.Contains(key)) {
            string msg = String.Format("Resource {0} already exists", key);
            throw new ArgumentException(msg, "key");
        }
        if (val == null) {
            throw new ArgumentNullException("val");
        }
        if (!(val is ILazyLink)) {
            val = new DirectLink(val);
        }
        ResourceTable.Add(key, val);
    }

    // Makes an alias. This just copies the object over.
    private void AddAlias(string alias, string key)
    {
        if (!ResourceTable.Contains(key)) {
            string msg = String.Format("Resource {0} doesn't exist", key);
            throw new ArgumentException(msg, "key");
        }
        if (!ResourceTable.Contains(key)) {
            string msg = String.Format("Resource {0} already exists, can't alias", alias);
            throw new ArgumentException(msg, "alias");
        }
        Add(alias, ResourceTable[key]);
    }

    // Adds racial remaps for a certain resource. The human variant is the default.
    private void AddRaceRemap(string resource)
    {
        AddAlias(resource, "human-" + resource);
        OrcUIRemap.Add(GetLink(resource), GetLink("orc-" + resource));
    }

    // Adds tileset remaps for a certain resource. The grass variant is the default.
    private void AddTilesetRemap(string resource)
    {
        AddAlias(resource, "grass-" + resource);
        SwampTilesetRemap.Add(GetLink(resource), GetLink("swamp-" + resource));
        DungeonTilesetRemap.Add(GetLink(resource), GetLink("dungeon-" + resource));
    }

    // Adds a standard palette link
    private void AddPaletteLink(string name, bool weak, int chunk)
    {
        Add(name, new PaletteLink(false, Archive, chunk));
    }

    // Adds a CombinePaletteLink
    private void AddCombinePaletteLink(string name, bool weak, string firstpal, string secondpal)
    {
        Add(name, new CombinePaletteLink(false, GetLink(firstpal), GetLink(secondpal)));
    }

    // Adds a AppendPaletteLink
    private void AddAppendPaletteLink(string name, bool weak, string firstpal, string secondpal)
    {
        Add(name, new AppendPaletteLink(false, GetLink(firstpal), GetLink(secondpal)));
    }

    // Build the cache.
    // This function classifies data from warcraft 1 and puts "links" to it in
    // the cache, in a much nicer format (with actual "names")
    public void LoadPalettes()
    {
        // Numbered palettes
        AddPaletteLink("pal-191", false, 191);
        AddPaletteLink("pal-194", false, 194);
        AddPaletteLink("pal-197", false, 197);
        AddPaletteLink("pal-210", false, 210);
        AddPaletteLink("pal-211", false, 211);
        AddPaletteLink("pal-217", false, 217);
        AddPaletteLink("pal-255", false, 255);
        AddPaletteLink("pal-260", false, 260);
        AddPaletteLink("pal-262", false, 262); // Unused?
        AddPaletteLink("pal-413", true, 413);
        AddPaletteLink("pal-414", true, 414);
        AddPaletteLink("pal-416", true, 416);
        AddPaletteLink("pal-418", true, 418);
        AddPaletteLink("pal-423", true, 423);
        AddPaletteLink("pal-424", true, 424);
        AddPaletteLink("pal-457", true, 457);
        AddPaletteLink("pal-459", true, 459);

        // Main palette, used for the map and units.
        AddCombinePaletteLink("grass-main-pal", false, "pal-191", "pal-217");
        AddCombinePaletteLink("swamp-main-pal", false, "pal-194", "pal-217");
        AddCombinePaletteLink("dungeon-main-pal", false, "pal-197", "pal-217");
        AddTilesetRemap("main-pal");

        // UI Palettes
        AddAppendPaletteLink("human-ui-pal", false, "pal-191", "pal-210");
        AddAppendPaletteLink("orc-ui-pal", false, "pal-191", "pal-211");
        AddRaceRemap("ui-pal");

        // Icon Palettes
        AddAppendPaletteLink("grass-icon-pal", false, "pal-191", "pal-210");
        AddAppendPaletteLink("swamp-icon-pal", false, "pal-194", "pal-210");
        AddAppendPaletteLink("dungeon-icon-pal", false, "pal-197", "pal-210");
        AddTilesetRemap("icon-pal");
    }

    // Adds an image
    private void AddImage(string name, bool weak, int chunk, string pal)
    {
        Add(name, new ImageLink(weak, Archive, chunk, GetLink(pal)));
    }

    // Adds an animation
    private void AddSprite(string name, bool weak, int chunk, string pal)
    {
        Add(name, new SpriteLink(weak, Archive, chunk, GetLink(pal)));
    }

    // Loads all sorts of graphics.
    public void LoadGraphics()
    {
        // Misc logos and backgrounds.
        AddImage("hot-keys-explanation-screen", true, 254, "pal-255");
        AddImage("blizzard-logo", true, 216, "pal-217");
        AddImage("warcraft-logo-lower-part", true, 243, "pal-260");
        AddImage("warcraft-logo-upper-part", true, 258, "pal-260");
        AddImage("warcraft-logo", true, 261, "pal-260");

        // Misc stuff.
        AddImage("human-score-screen", true, 411, "pal-413");
        AddImage("orc-score-screen", true, 412, "pal-414");
        AddRaceRemap("score-screen");
        AddImage("winner-image", true, 415, "pal-416");
        AddImage("loser-image", true, 417, "pal-418");
        AddImage("text-victory", true, 419, "pal-414"); // Looks bad, palette?
        AddImage("text-defeat", true, 420, "pal-414"); // Looks bad, palette?

        // Human briefing
        AddAlias("human-briefing-pal", "pal-423");
        AddImage("human-briefing-screen", true, 421, "human-briefing-pal");
        AddSprite("human-briefing-left-man", true, 428, "human-briefing-pal");
        AddSprite("human-briefing-right-man", true, 429, "human-briefing-pal");
        AddSprite("human-briefing-left-torch", true, 430, "human-briefing-pal");
        AddSprite("human-briefing-right-torch", true, 431, "human-briefing-pal");

        // Orc briefing
        AddAlias("orc-briefing-pal", "pal-424");
        AddImage("orc-briefing-screen", true, 422, "orc-briefing-pal");
        AddSprite("orc-briefing-fireplace", true, 425, "orc-briefing-pal");
        AddSprite("orc-briefing-left-orc", true, 426, "orc-briefing-pal");
        AddSprite("orc-briefing-right-orc", true, 427, "orc-briefing-pal");

        // Human throne?
        AddAlias("human-throne-pal", "pal-457");
        AddImage("human-throne-screen", true, 456, "human-throne-pal");
        AddImage("human-throne", true, 470, "human-throne-pal");

        // Orc throne?
        AddAlias("orc-throne-pal", "pal-459");
        AddImage("orc-throne-screen", true, 458, "orc-throne-pal");
        AddSprite("orc-throne-torches", true, 450, "orc-throne-pal");
        AddImage("orc-throne", true, 471, "pal-260");
    }

    // Loads animations.
    private void LoadAnimations()
    {
        int i;

        // Unit sprites.
        for (i = 279; i <= 330; ++i) {
            AddSprite("grass-anim-" + i, false, i, "grass-main-pal");
            AddSprite("swamp-anim-" + i, false, i, "swamp-main-pal");
            AddSprite("dungeon-anim-" + i, false, i, "dungeon-main-pal");
            AddTilesetRemap("anim-" + i);
        }

        // Effect sprites.
        for (i = 347; i <= 358; ++i) {
            AddSprite("anim-" + i, false, i, "ui-pal");
        }
    }

    // Adds an UI image.
    private void AddUIImage(string name, int chunk)
    {
        Add(name, new ImageLink(true, Archive, chunk, GetLink("ui-pal")));
    }

    // Adds an UI image with a race remap.
    private void AddRaceUIImage(string name, int hchunk, int ochunk)
    {
        Add("human-" + name, new ImageLink(true, Archive, hchunk, GetLink("human-ui-pal")));
        Add("orc-" + name, new ImageLink(true, Archive, ochunk, GetLink("orc-ui-pal")));
        AddRaceRemap(name);
    }

    // Load user interface graphics.
    private void LoadUI()
    {
        // Generic UI.
        AddUIImage("ui-big-button-normal", 237);
        AddUIImage("ui-big-button-pressed", 238);
        AddUIImage("ui-medium-button-normal", 239);
        AddUIImage("ui-medium-button-pressed", 240);
        AddUIImage("ui-small-button-normal", 241);
        AddUIImage("ui-small-button-pressed", 242);
        AddUIImage("ui-left-arrow-normal", 244);
        AddUIImage("ui-left-arrow-pressed", 245);
        AddUIImage("ui-right-arrow-normal", 246);
        AddUIImage("ui-right-arrow-pressed", 247);
        AddUIImage("ui-large-empty-frame", 248); // Where is it used?
        AddUIImage("ui-ok-button-normal", 256);
        AddUIImage("ui-ok-button-pressed", 257);
        AddUIImage("ui-menu-button-normal", 362);
        AddUIImage("ui-menu-button-pressed", 363);
        AddUIImage("ui-icon-frame-normal", 364);
        AddUIImage("ui-icon-frame-pressed", 365);

        // Human in-game UI
        AddRaceUIImage("ui-upper-filler", 218, 219);
        AddRaceUIImage("ui-right-filler", 220, 221);
        AddRaceUIImage("ui-lower-filler", 222, 223);
        AddRaceUIImage("ui-minimap-map", 224, 225);
        AddRaceUIImage("ui-unit-panel", 226, 227);
        AddRaceUIImage("ui-minimap-info", 228, 229);
        AddRaceUIImage("ui-menu-panel", 233, 234);
        AddRaceUIImage("ui-horizontal-panel", 235, 236);
        AddRaceUIImage("ui-save-load-panel", 249, 250);

        // Unit panel.
        AddSprite("human-ui-unit-icon-panel", true, 360, "human-ui-pal");
        AddSprite("orc-ui-unit-icon-panel", true, 359, "orc-ui-pal");
        AddRaceRemap("ui-unit-icon-panel");
    }

    // Add a single cursor.
    private void AddCursor(string name, int chunk)
    {
        Add(name, new CursorLink(false, Archive, chunk, GetLink("ui-pal")));
    }

    // Load cursors
    private void LoadCursors()
    {
        AddCursor("cursor-arrow", 263);
        AddCursor("cursor-invalid-arrow", 264);
        AddCursor("cursor-yellow-crosshair", 265);
        AddCursor("cursor-red-crosshair", 266);
        AddCursor("cursor-green-crosshair", 267);
        AddCursor("cursor-magnifying-glass", 268);
        AddCursor("cursor-small-green-crosshair", 269);
        AddCursor("cursor-watch", 270);
        AddCursor("cursor-arrow-up", 271);
        AddCursor("cursor-arrow-up-right", 272);
        AddCursor("cursor-arrow-right", 273);
        AddCursor("cursor-arrow-down-right", 274);
        AddCursor("cursor-arrow-down", 275);
        AddCursor("cursor-arrow-down-left", 276);
        AddCursor("cursor-arrow-left", 277);
        AddCursor("cursor-arrow-up-left", 278);
    }

    private void AddIconAlias(string name, int id)
    {
        string sid = id.ToString();
        AddAlias("icon-" + name, "icon-" + sid);
        AddAlias("grass-icon-" + name, "grass-icon-" + sid);
        AddAlias("swamp-icon-" + name, "swamp-icon-" + sid);
        AddAlias("dungeon-icon-" + name, "dungeon-icon-" + sid);
    }

    // Load the icons.
    public void LoadIcons()
    {
        int i;
        int IconCount;

        // Load icons.
        AddSprite("grass-icons", false, 361, "grass-icon-pal");
        AddSprite("swamp-icons", false, 361, "swamp-icon-pal");
        AddSprite("dungeon-icons", false, 361, "dungeon-icon-pal");
        AddTilesetRemap("icons");

        // This causes a load of the icons. So what?
        IconCount = ((Sprite)Get("icons")).FrameCount;
        for (i = 0; i < IconCount; ++i) {
            string sid = String.Format("icon-{0}", i);
            Add("grass-" + sid, new SpriteImageLink(false, GetLink("grass-icons"), i));
            Add("swamp-" + sid, new SpriteImageLink(false, GetLink("swamp-icons"), i));
            Add("dungeon-" + sid, new SpriteImageLink(false, GetLink("dungeon-icons"), i));
            AddTilesetRemap(sid);
        }

        AddIconAlias("human-walk", 33);
        AddIconAlias("orc-walk", 34);
        AddIconAlias("repair", 35);
        AddIconAlias("harvest", 36);
        AddIconAlias("build-basic", 37);
        AddIconAlias("build-advanced", 38);
        AddIconAlias("return-resources", 39);
        AddIconAlias("cancel", 40);

        // Special attacks.
        AddIconAlias("cleric-attack", 59);
        AddIconAlias("conjurer-attack", 60);
        AddIconAlias("necrolyte-attack", 61);
        AddIconAlias("warlock-attack", 62);

        // Various upgrades
        AddIconAlias("human-sword", 63);
        AddIconAlias("human-sword-1", 63);
        AddIconAlias("human-sword-2", 64);
        AddIconAlias("human-sword-3", 65);

        AddIconAlias("orc-axe", 66);
        AddIconAlias("orc-axe-1", 66);
        AddIconAlias("orc-axe-2", 67);
        AddIconAlias("orc-axe-3", 68);

        AddIconAlias("orc-wolf", 69);
        AddIconAlias("orc-wolf-1", 69);
        AddIconAlias("orc-wolf-2", 70);

        AddIconAlias("human-bow", 71);
        AddIconAlias("human-bow-1", 71);
        AddIconAlias("human-bow-2", 72);
        AddIconAlias("human-bow-3", 73);

        AddIconAlias("orc-spear", 74);
        AddIconAlias("orc-spear-1", 74);
        AddIconAlias("orc-spear-2", 75);
        AddIconAlias("orc-spear-3", 76);

        AddIconAlias("human-horse", 77);
        AddIconAlias("human-horse-1", 77);
        AddIconAlias("human-horse-2", 78);

        AddIconAlias("human-shield", 79);
        AddIconAlias("human-shield-1", 79);
        AddIconAlias("human-shield-2", 80);
        AddIconAlias("human-shield-3", 81);

        AddIconAlias("orc-shield", 82);
        AddIconAlias("orc-shield-1", 82);
        AddIconAlias("orc-shield-2", 83);
        AddIconAlias("orc-shield-3", 84);

        // Spells
        AddIconAlias("heal", 85);
        AddIconAlias("holy-vision", 86);
        AddIconAlias("invisibility", 87);
        AddIconAlias("fire-rain", 88);
        AddIconAlias("raise-dead", 89);
        AddIconAlias("dark-vision", 90);
        AddIconAlias("unholy-armor", 91);
        AddIconAlias("poison-cloud", 92);
    }
}
