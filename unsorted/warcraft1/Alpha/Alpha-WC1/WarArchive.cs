// Copyright (C) 2004-2005 Crestez Leonard    cleonard@go.ro
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
using System.IO;
using System.Text;
using Color = System.Drawing.Color;
using System.Drawing.Imaging;

using Alpha.Util;
using Alpha.Graphics;
using Alpha.Game;
using Alpha.UI;
using Alpha.Scene;

// Class that encompasses a .WAR archive.
// That file contains a bunch of compressed unnamed(damn) chunks
// Since chunks are unnamed, you more or less have to guess what they mean.
// This class contains methods for getting each chunk, but also for getting it
// in a smarter format.
public class WarArchive
{
    // The .war file strem.
    private FileStream stream;

    // The offset of each chunk in the file.
    private int[] ChunkOffsets;

    // The length of each chunk in the file.
    private int[] ChunkLengths;

    // Returns the number of chunks in the file.
    public int ChunkCount {
        get {
            return ChunkOffsets.Length;
        }
    }

    // Load the file and get basic info.
    // We use lazy decompression, because stuff like cut scenes are HUGE.
    // Completely decompressing everything is like 50 megs, about ten times
    // the size of the original files.
    //
    // The basic file format is like this:
    // Please note that this is based on observations, not a spec.
    // Int32 magic number, must be 0x18 or 0x19?
    // UInt16 number of chunks.
    // UInt16 version must be 0.
    // Int32[ChunkCount] offset of chunk. Chunks are compressed, see GetChunk.
    // For each Chunk you have one int32 containing the uncompressed length and a bunch of flags.
    // Only the least significant 24 bits are part of the number, the rest is the flags.
    // I only know of flags 0, uncompressed, and 0x20, some wierd compression thingy.
    public WarArchive(string filename)
    {
        BinaryReader reader = new BinaryReader(stream = File.OpenRead(filename));
        int i, j;
        uint magic, count;

        magic = reader.ReadUInt32();
        //Console.WriteLine("Archive magic number: {0:x}", magic);
        // 18 seems to be the full version and 19 the demo.
        if (magic != 0x19 && magic != 0x18) {
            throw new Exception("Invalid magic number in war file.");
        }

        count = (uint)reader.ReadUInt32();
        // Both the demo and the full version have precisely 583 chunks.
        //Console.WriteLine("There are {0} chunks in the file.", count);
        if (count > 600) {
            throw new Exception("There are way too many chunks, file must be corrupt");
        }
        ChunkOffsets = new int[count];
        ChunkLengths = new int[count];

        // Some chunks can be missing, they are marked with an offset of -1.
        j = -1;
        for (i = 0; i < ChunkCount; ++i) {
            ChunkOffsets[i] = reader.ReadInt32();
            if (ChunkOffsets[i] != -1) {
                if (ChunkOffsets[i] >= (int)stream.Length || ChunkOffsets[i] < 0) {
                    throw new Exception("Chunk offset out of range");
                }
                if (j != -1) {
                    if (ChunkOffsets[i] < ChunkOffsets[j]) {
                        throw new Exception("Chunk offsets are not ordered");
                    }
                    ChunkLengths[j] = ChunkOffsets[i] - ChunkOffsets[j];
                    //Console.WriteLine("Chunk {0} at offset {1} has length {2}",
                    //        j, ChunkOffsets[j], ChunkLengths[j]);
                }
                j = i;
            } else {
                ChunkLengths[i] = 0;
            }
        }
        ChunkLengths[j] = (int)stream.Length - ChunkOffsets[j];
        //Console.WriteLine("Chunk {0} at offset {1} has length {2}",
        //        j, ChunkOffsets[j], ChunkLengths[j]);
    }

    // Return an uncompressed chunk as a FastStream. This does decompression every time.
    public FastStream GetChunk(int chunk)
    {
        uint header, flags;
        int finlen, comlen; // Final and compressed length.

        if (chunk < 0 || chunk >= ChunkCount) {
            throw new Exception("Chunk number out of range.");
        }
        if (ChunkOffsets[chunk] == -1 || ChunkLengths[chunk] < 4) {
            throw new Exception("Chunk is missing from data file.");
        }

        this.stream.Seek(ChunkOffsets[chunk], SeekOrigin.Begin);

        header = new BinaryReader(stream).ReadUInt32();
        flags = header >> 24;
        finlen = (int)(header & 0x00FFFFFF);
        comlen = ChunkLengths[chunk];

        //Console.WriteLine("head = {0:x} Flags = {1:x} comlen = {2}, finlen = {3}",
        //        head, flags, comlen, finlen);

        if (flags == 0) {
            // Bunch of flat bytes.
            if (comlen - 4 != finlen) {
                Console.WriteLine("{0} != {1}", comlen - 4, finlen);
                throw new Exception(String.Format("Uncompressed chunk size doesn't match."));
            }
            FastStream output = new FastStream(finlen);
            stream.Read(output.Buffer, 0, output.Size);
            output.Position = 0;
            return output;
        } else if (flags == 0x20) {
            // Compressed data here.
            // I have no idea how this code works, it's more or less a dumb translation
            // of war1gus fugly c code.
            byte[] buf = new byte[4096];
            int bufindex;
            bufindex = 0;
            buf.Initialize();

            FastStream input = new FastStream(comlen);
            stream.Read(input.Buffer, 0, input.Size);
            FastStream output = new FastStream(finlen);

            while (output.Position < finlen) {
                int i, j, bflags;
                byte b;

                bflags = input.ReadByte();
                for (i = 0; i < 8; ++i) {
                    if ((bflags & 1) != 0) {
                        b = input.ReadByte();
                        output.WriteByte(b);
                        buf[bufindex++ & 0xFFF] = b;
                    } else {
                        int o = input.ReadUInt16();
                        j = (o >> 12) + 3;
                        while (j-- != 0) {
                            b = buf[bufindex++ & 0xFFF] = buf[o++ & 0xFFF];
                            output.WriteByte(b);
                        }
                    }
                    if (output.Position >= finlen) {
                        break;
                    }
                    bflags >>= 1;
                }
            }
            output.Position = 0;
            return output;
        } else {
            throw new Exception("Invalid flags for chunk");
        }
    }

    // Returns a certain chunk as a string.
    // It's a trivial ASCII encoded string.
    public string GetTextChunk(int chunk)
    {
        FastStream stream = GetChunk(chunk);
        return System.Text.Encoding.ASCII.GetString(stream.Buffer);
    }

    // Returns a certain chunk as a 256 color palette.
    // WC1 palettes are 6bit per channel. I have no idea why.
    public Color[] GetPaletteChunk(int chunk)
    {
        FastStream stream = GetChunk(chunk);
        Color[] pal = new Color[256];
        int i;

        for (i = 0; i < 256; ++i) {
            pal[i] = Color.FromArgb(63 << 2, 0, 63 << 2);
        }
        if (stream.Size != 128 * 3 && stream.Size != 256 * 3) {
            throw new Exception("Invalid palette size");
        }
        //Console.WriteLine("Loading palette of {0} colors", stream.Size / 3);

        stream.Position = 0;
        for (i = 0; i < 256 && i < stream.Size / 3; ++i) {
            int r = (stream.ReadByte() << 2);
            int g = (stream.ReadByte() << 2);
            int b = (stream.ReadByte() << 2);
            pal[i] = Color.FromArgb(r, g, b);
        }
        return pal;
    }

    // Combine two palettes by some specific rules.
    // 63, 0, 63 is the "switch color", some braindamaged pink
    // If one of the colors is the switch color, the other takes over.
    // If neither is the switch color, the first takes precedence.
    public static Color[] CombinePalette(Color[] a, Color[] b)
    {
        Color[] c = new Color[256];
        Color trans = Color.FromArgb(63 << 2, 0, 63 << 2);
        int i;

        for (i = 0; i < 256; ++i) {
            if (a[i] != trans) {
                c[i] = a[i];
            } else {
                c[i] = b[i];
            }
        }
        return c;
    }

    // Combine two palettes by appending.
    // The two parameters are 128-color palettes.
    // The first part of the result is palette a
    // The second part is palette b. (c[128 + i] = b[i])
    public static Color[] AppendPalette(Color[] a, Color[] b)
    {
        int i;
        Color[] c = new Color[256];

        for (i = 0; i < 128; ++i) c[i] = a[i];
        for (i = 0; i < 128; ++i) c[128 + i] = b[i];

        return c;
    }

    // Returns a chunk as one Image. This is used to store single images, like
    // say title screens and the like. The format is truly trivial, just
    // 2 16-bits for width/height followed by width * height bytes with data.
    public Image GetImageChunk(int chunk, Color[] pal)
    {
        FastStream stream = GetChunk(chunk);

        int width = stream.ReadUInt16();
        int height = stream.ReadUInt16();
        if (width * height + 4 != stream.Size) {
            throw new Exception("Invalid image");
        }
        int[,] image = new int[height, width];
        int x, y;

        //Console.WriteLine("Loading image sized {0}x{1}", width, height);
        for (y = 0; y < height; ++y) {
            for (x = 0; x < width; ++x) {
                int b = stream.ReadByte();
                image[y, x] = pal[b].ToArgb();
                //image[y, x] = pal[((x ^ y) & 1)].ToArgb();
            }
        }
        return new Image(Texture.StrictTexture(image), width, height);
    }

    // Returns a certain chunk as a Sprite.
    // Here is the format, sort of.
    // UInt16: the number of "frames" of the image
    // Byte * 2: max width and height of the image.
    // After that we have "count" frames. For every frame only a rect
    // area of it is actually kept, the rest is considered transparent.
    // Byte * 2: X/Y Offset of the data in the frame
    // Byte * 2: Width/Height of the data in the frame
    // Int32: File Offset of the actual data.
    public Sprite GetSpriteChunk(int chunk, Color[] pal)
    {
        FastStream stream;
        Sprite sprite;
        int[,] frame;
        int i;
        int frameCount, spriteWidth, spriteHeight;
       
        stream = GetChunk(chunk);
        frameCount = stream.ReadUInt16();
        spriteWidth = stream.ReadByte();
        spriteHeight = stream.ReadByte();
        sprite = new Sprite(frameCount, spriteWidth, spriteHeight);

        // FIXME FIXME FIXME: UGLY UGLY UGLY HACK HACK HACK
        pal[0] = Color.FromArgb(0, 0, 0, 0);

        for (i = 0; i < sprite.FrameCount; ++i) {
            int offset, oldoffset;
            int x, y, xoffset, yoffset, width, height;

            xoffset = stream.ReadByte();
            yoffset = stream.ReadByte();
            width = stream.ReadByte();
            height = stream.ReadByte();
            offset = stream.ReadInt32();

            if (xoffset + width > spriteWidth || yoffset + height > spriteHeight) {
                throw new System.Exception("Corrupt frame header, sub-rect out of range.");
            }
            if (width * height > 0 && offset + width * height > stream.Size) {
                throw new System.Exception("Corrupt frame header, data out of stream.");
            }

            if (width * height > 0) {
                int log_width, log_height;
                for (log_width = 0; (1 << log_width) < width + 2; ++log_width) {
                }
                for (log_height = 0; (1 << log_height) < height + 2; ++log_height) {
                }
                frame = new int[1 << log_height, 1 << log_width];

                oldoffset = stream.Position;
                stream.Position = offset;
                for (y = 1; y < height + 1; ++y) {
                    for (x = 1; x < width + 1; ++x) {
                        frame[y, x] = pal[stream.ReadByte()].ToArgb();
                        //frame[y, x] = pal[((x ^ y) & 1)].ToArgb();
                    }
                }

                sprite[i] = Texture.StrictTexture(frame);
                sprite[i].XScale *= (double)spriteWidth / (1 << log_width);
                sprite[i].YScale *= (double)spriteHeight / (1 << log_height);
                sprite[i].XTrans = - (double)(xoffset - 1) / spriteWidth * sprite[i].XScale;
                sprite[i].YTrans = - (double)(yoffset - 1) / spriteHeight * sprite[i].YScale;
                
                stream.Position = oldoffset;
            }
        }
        return sprite;
    }

    // Returns a chunk as a Cursor, usable in the UI right away.
    //
    // Here is the format:
    // UInt16 Hotspot X
    // UInt16 Hotspot Y
    // UInt16 Width
    // UInt16 Height
    // Width * Height Bytes with image data, indexes into palette.
    // This matches really well with Alpha.UI.ImageCursor
    public Cursor GetCursorChunk(int chunk, Color[] pal)
    {
        ImageCursor cur = new ImageCursor();
        FastStream data = GetChunk(chunk);
        int x, y, width, height;
        int[,] image;
      
        // We add 0.5 because we want the hotspot to be the center of that pixel.
        cur.HotX = data.ReadUInt16() + 0.5;
        cur.HotY = data.ReadUInt16() + 0.5;
        cur.Width = width = data.ReadUInt16();
        cur.Height = height = data.ReadUInt16();

        if (cur.Width * cur.Height + 8 != data.Size) {
            throw new Exception("Invalid cursor");
        }

        // FIXME FIXME FIXME: UGLY UGLY UGLY HACK HACK HACK
        pal[0] = Color.FromArgb(0, 0, 0, 0);

        //Console.WriteLine("Loading cursor sized {0}x{1} with the hotspot at {2}, {3}", width, height, cur.HotX, cur.HotY);
        image = new int[height, width];

        for (y = 0; y < height; ++y) {
            for (x = 0; x < width; ++x) {
                int b = data.ReadByte();
                image[y, x] = pal[b].ToArgb();
                //image[y, x] = pal[((x ^ y) & 1)].ToArgb();
            }
        }
        cur.Image = Texture.StrictTexture(image);
        return cur;
    }

    // Returns a chunk as a tileset. Actually, it only returns the images, not
    // a Tileset class in the Alpha sense. The format is really really truly wierd.
    // First of all, it's actually stored in three chunks.
    //
    // The Palette is at chunk + 1, sort of. Actually the palette must be combined
    // with some wierd rules with pal 217, you should read the code.
    // At chunk you have a bunch of raw image data.
    // At chunk - 1 it stores 4 16-bit offsets in the image data for each tile,
    // with the lower 2 bits as flip information. So each 16 * 16 tile is made
    // out of 4 smaller 8*8 minitiles. 
    // The offset is actually (offset & 0xFFFC) << 1, two bytes are flip info on x/y.
    //
    // There seems to be no pathfinder (block/non-block) information in the tileset.
    // Maybe it's in the map?
    public Texture[] GetTilesetChunk(int chunk, Color[] pal)
    {
        FastStream data = GetChunk(chunk);
        FastStream info = GetChunk(chunk - 1);

        int count = (int)info.Size / 8;
        int[,] img;
        Texture[] res = new Texture[count];
        int i;

        //Console.WriteLine("Loading tileset of {0} tiles", count);

        img = new int[16, 16];
        for (i = 0; i < count; ++i) {
            DecodeMiniTile(img, 0, 0, data, info.ReadUInt16(), pal);
            DecodeMiniTile(img, 8, 0, data, info.ReadUInt16(), pal);
            DecodeMiniTile(img, 0, 8, data, info.ReadUInt16(), pal);
            DecodeMiniTile(img, 8, 8, data, info.ReadUInt16(), pal);
            res[i] = Texture.StrictTexture(img);
        }

        return res;
    }

    // Decode a 8x8 mini-tile, see GetTilesetChunk.
    // It decodes into img, using pal, adding addx and addy to coordinates.
    // It reads data from the Stream, at offset o. Of course, it's not a
    // real offset, the lower 2 bits are flip info. Yay blizzard.
    private void DecodeMiniTile(int[,] img, int addx, int addy, FastStream stream, int offset, Color[] pal)
    {
        bool flipx = (offset & 2) != 0, flipy = (offset & 1) != 0;
        int x, y;
        stream.Position = (offset & 0xFFFC) << 1;
        for (y = 0; y < 8; ++y) {
            for (x = 0; x < 8; ++x) {
                int cy = addy + (flipy ? (7 - y) : y);
                int cx = addx + (flipx ? (7 - x) : x);
                img[cy, cx] = pal[stream.ReadByte()].ToArgb();
                //img[cy, cx] = pal[(x ^ y) & 1].ToArgb();
            }
        }
    }

    // Setup the default players in a GameState, acording to WC1
    // FIXME: this is wrong, should read info from map.
    private void MakeDefaultPlayers(GameState state)
    {
        int i, j;
        for (i = 0; i < 5; ++i) {
            state.CreatePlayer(i);
        }
        // Colors.
        state.GetPlayer(0).Color = Color.Blue;
        state.GetPlayer(1).Color = Color.Red;
        state.GetPlayer(2).Color = Color.Magenta;
        state.GetPlayer(3).Color = Color.Yellow;
        state.GetPlayer(4).Color = Color.Gray;

        // 4 is neutral, the rest are fighting each other.
        // FIXME: wrong
        for (i = 0; i <= 3; ++i) {
            state.GetPlayer(i).Relation[4] = PlayerRelation.Neutral;
            state.GetPlayer(4).Relation[i] = PlayerRelation.Neutral;
            for (j = 0; j <= 3; ++j) {
                state.GetPlayer(i).Relation[j] = PlayerRelation.Enemy;
            }
        }
        for (i = 0; i <= 4; ++i) {
            state.GetPlayer(i).Relation[i] = PlayerRelation.Friend | PlayerRelation.SharedVision;
        }
    }

    // Decode a WC1 map. This is not fully functional, it only does terrain.
    //
    // This function creates a new Map in the GameState and puts entities on it.
    // Theoretically you could load multiple WC1 map chunks into one gamestate,
    // but it's not supported, because WC1 maps contain everything pertaining to a
    // scenario (upgrades, spells, resources, etc).
    //
    // This returns the new map.
    // Should be called after GameData was loaded.
    //
    // -- Here is a long and incomplete description of the data --
    //
    // FIXME: The map information chunk is not completely decoded. This is from
    // my observations and war1gus code. There are offsets in the file, probably
    // a lot of the mistery data is pointed to by those offsets.
    // Offset ranges are inclusive (0x0000 - 0x0003 means 4 bytes).
    // An intereseting observation is that WC1 has 5 players (4 + neutral).
    //
    // 0x0000 - 0x0003: DWord: Allowed units/upgrades, see AllowId
    //
    //     Here we have some per-player data, upgrades and spells (spells are
    //     actually upgrades too, so...
    //     The data is 5 bytes per player (but player 5 is neutral).
    //     Upgrade values are from 0 to 2, spells are 0 and 1
    //     Orc and Human spells/upgrades overlap, so the first upgrade
    //     is either arrows or spears, etc.
    //
    // 0x0004 - 0x0008: 5xByte: Upgrade: Ranged Weapons, arrows / spears.
    // 0x0009 - 0x000D: 5xByte: Upgrade: Melee Weapons, swords / axes.
    // 0x000E - 0x0012: 5xByte: Upgrade: Rider speed, horses / wolves.
    // 0x0013 - 0x0017: 5xByte: Spell: summon scorpions / summon spiders.
    // 0x0018 - 0x001C: 5xByte: Spell: rain of fire / cloud of poison.
    // 0x001D - 0x0021: 5xByte: Spell: summon water elemental / summon daemon.
    // 0x0022 - 0x0026: 5xByte: Spell: healing / raise dead.
    // 0x0027 - 0x002B: 5xByte: Spell: far seeing / dark vision.
    // 0x002C - 0x0030: 5xByte: Spell: invisibility / unholy armor.
    // 0x0031 - 0x0035: 5xByte: Upgrade: Shields.
    //
    // 0x0036 - 0x0039: 0xFFFFFFFF: This is a constant marker.
    //
    // 0x005C - 0x0069: 5xDWord: Lumber for each player.
    // 0x0070 - 0x0083: 5xDWord: Gold for each player.
    // 
    // 0x0088 - 0x008C: 5xByte: Mistery data, per player. human/orc/neutral? human/ai/neutral?
    //
    // 0x0094 - 0x0095: Word: Offset to a null-terminated string with a short briefing.
    //
    //     Here are some chunk numbers, please see Terrain Data.
    //     Chunk number seem to be off (+2). Probably array base trouble.
    // 0x00D0 - 0x00D1: Word: Chunk number with terrain tile data.
    // 0x00D2 - 0x00D3: Word: Chunk number with terrain flags information. 
    // 0x00D4 - 0x00D6: Word: Chunk number of tileset palette.
    // 0x00D6 - 0x00D7: Word: Chunk number of tileset info.
    // 0x00D8 - 0x00D9: Word: Chunk number of tileset image data.
    // 
    //     This header stuff continues until another FF FF FF FF, but it's
    //     not predictable where. After that you see another int16 offset, into unit data.
    //
    // 
    // * Unit Data:
    // Unit data consist of multiple records like this:
    // byte X: X position on the map.
    // byte Y: Y position on the map.
    // byte Type: Type of the unit.
    // byte player: Player that owns the unit (0 - 4). Gold mines have 4, neutral.
    //
    // If Type is 0x32 then there are also some values for gold. There are two bytes.
    // The first seems to be constant 0xFE, the second is aprox gold value/250. I'm
    // not sure how to read gold the right way. The current way sort of works, but it
    // has small differences.
    //
    // Unit data is finished by a "fake" unit with x = y = 0xFF. This is obviousely out
    // of the map.
    //
    // Then we have roads, as 5 bytes: x1, y1, x2, y2, type
    // This means a road from x1 x1 to x2 y2. They do cross. Type seems to be the race.
    // This is also ended by x1 = FF x2 = FF
    //
    // After that we have walls, written exactly the same way as roads.
    //
    //
    // * Terrain Data
    //
    // Decoding the terrain chunk is easy, it's just a 64*64 array of Int16
    // that reference tiles in the Tileset array.
    //
    // There's also a second terrain chunk, with flags.
    // It's also a 64 * 64 array of Int16. It's not completely decoded,
    // but the here are some flags:
    //
    // 0x0000: Normal ground
    // 0x000C: Door(dungeon only)
    // 0x0020: Dungeon entrance? no idea.
    // 0x0040: Forest
    // 0x0080: Water
    // 0x0010: Bridge
    //
    // I use the above for map passability information.
    public SimpleTileMap GetMapChunk(WarGameState state, int mapchunk)
    {
        SimpleTileMap tmap;
        FastStream map;
        int x, y, i;
        WarPlayer[] Players;
       
        // Initialize
        map = GetChunk(mapchunk);
        MakeDefaultPlayers(state);
        tmap = GetMapTileset(map).CreateMap(state, 64, 64) as SimpleTileMap;

        // Begin reading stuff
        // Since most of this stuff is unsupported, we just print it.
        map.Position = 0;
        Console.WriteLine();
        Console.WriteLine("Loading map from chunk {0}", mapchunk);
        state.AllowedFeatures = map.ReadUInt32();

        // Shortcuts to players.
        Players = new WarPlayer[5];
        for (i = 0; i < 5; ++i) {
            Players[i] = (WarPlayer)tmap.State.GetPlayer(i);
        }

        // Per player data
        Console.WriteLine("Player Number:  1       2       3       4       5       ");

        // Load upgrades.
        Console.WriteLine("Starting Upgrade Levels:");
        for (x = 0; x < (int)UpgradeId.Max; ++x) {
            Console.Write("{0,-16}", (UpgradeId)x + ":");
            for (y = 0; y < 5; ++y) {
                byte level = map.ReadByte();
                Console.Write("{0}       ", level);
                ((WarPlayer)state.GetPlayer(y)).UpgradeLevels[x] = level;
            }
            Console.WriteLine();
        }

        if (map.ReadUInt32() != 0xFFFFFFFF) {
            throw new Exception("Wrong map format");
        }

        // Resources
        map.Position = 0x005C;
        Console.WriteLine("Resources:");
        Console.Write("Wood:           ");
        for (i = 0; i < 5; ++i) {
            int amount = map.ReadInt32();
            Players[i].WoodAmount = amount;
            Console.Write("{0,-8}", amount);
        }
        Console.WriteLine();

        Console.Write("Gold:           ");
        for (i = 0; i < 5; ++i) {
            int amount = map.ReadInt32();
            Players[i].GoldAmount = amount;
            Console.Write("{0,-8}", amount);
        }
        Console.WriteLine();

        // Briefing
        Console.WriteLine("Map Objective:");
        Console.WriteLine("\"{0}\"", GetMapObjective(map));

        int TerrainTilesChunk, TerrainFlagsChunk;
        map.Position = 0x00D0;
        TerrainTilesChunk = map.ReadInt16() - 2;
        TerrainFlagsChunk = map.ReadInt16() - 2;

        // Read Terrain Tiles.
        Console.WriteLine("Terrain tiles in chunk {0}", TerrainTilesChunk);
        FastStream terrain = GetChunk(TerrainTilesChunk);
        for (y = 0; y < 64; ++y) {
            for (x = 0; x < 64; ++x) {
                tmap.SetTileType(x, y, terrain.ReadUInt16());
                tmap.InitTile(x, y);
            }
        }

        // Read Terrain Flags.
        Console.WriteLine("Terrain flags in chunk {0}", TerrainFlagsChunk);
        FastStream flags = GetChunk(TerrainFlagsChunk);
        int mts = WarGameData.TileSize;
        int pts =  ((TilePathfinder)Global.GameData.Pathfinder).TileSize;
        for (y = 0; y < 64; ++y) {
            for (x = 0; x < 64; ++x) {
                int tflags = flags.ReadUInt16();
                int x1, y1, x2, y2;
                x1 = mts * x / pts;
                y1 = mts * y / pts;
                x2 = mts * (x + 1) / pts - 1;
                y2 = mts * (y + 1) / pts - 1;

                BlockTable bt1 = ((TilePathMap)tmap.PathMap)[0].Block;
                BlockTable bt2 = ((TilePathMap)tmap.PathMap)[0].PermBlock;
                if (tflags == 0) {
                } else if (tflags == 0x0040) {
                    bt1.Block(x1, y1, x2, y2);
                    bt2.Block(x1, y1, x2, y2);
                } else if (tflags == 0x0080) {
                    bt1.Block(x1, y1, x2, y2);
                    bt2.Block(x1, y1, x2, y2);
                } else if (tflags == 0x0010) {
                } else if (tflags == 0x000C) {
                    bt1.Block(x1, y1, x2, y2);
                    bt2.Block(x1, y1, x2, y2);
                } else if (tflags == 0x0020) {
                } else {
                    throw new Exception(String.Format("Invalid value in pathmap {0:X8}", tflags));
                }
            }
        }

        // Seek units
        map.Position = 0x3a;
        while (map.PeekUInt32() != 0xFFFFFFFF) {
            ++map.Position;
        }
        map.Position += 4;
        map.Position = map.ReadUInt16();

        // Read units
        while (true) {
            int type, player;
            int ux, uy, gold;

            x = map.ReadByte();
            y = map.ReadByte();
            if (x == 0xFF && y == 0xFF) {
                break;
            }
            type = map.ReadByte();
            player = map.ReadByte();
            if (type == 0x32) {
                // FIXME: NOT SURE
                if (map.PeekByte() != 0xFE) {
                    throw new Exception(String.Format("Byte after goldmine is 0x{0:X2}, not 0xfe", map.PeekByte()));
                }
                map.ReadByte();
                gold = ((map.ReadByte() * 254) / 100 * 100);
            } else {
                gold = 0;
            }

            if (Global.GameData.Units[type] == null) {
                Console.WriteLine("Unknown unit {0} at {1} {2} belonging to player {3}", type, x, y, player);
            } else {
                UnitType utype = Global.GameData.Units[type];
                ux = ((x + utype.Size) * mts) / 2;
                uy = ((y + utype.Size) * mts) / 2;
                Unit u = (Unit)Utilities.TryCreatePlaceEntity(
                        utype, state.GetPlayer(player), tmap, ux, uy, 0);

                if (u != null) {
                    u.TimeToLive = -1;
                    u.GoldAmount = gold;
                    //Console.WriteLine("Placed unit {0} at {1} {2} belonging to player {3}", u.Type.Name, x, y, player);
                } else {
                    Console.WriteLine("Can't place unit {0} at {1} {2}, belonging to player {3}",
                            utype.Name, x, y, player);
                }
            }
        }

#if false
        // Read roads.
        while (true) {
            int x1, y1, x2, y2, t;
            x1 = map.ReadByte();
            y1 = map.ReadByte();
            if (x1 == 0xFF && y1 == 0xFF) {
                break;
            }
            x2 = map.ReadByte();
            y2 = map.ReadByte();
            t = map.ReadByte();
            Console.WriteLine("road {0} {1} to {2} {3} type {4}", x1, y1, x2, y2, t);
        }

        // Read walls.
        while (true) {
            int x1, y1, x2, y2, t;
            x1 = map.ReadByte();
            y1 = map.ReadByte();
            if (x1 == 0xFF && y1 == 0xFF) {
                break;
            }
            x2 = map.ReadByte();
            y2 = map.ReadByte();
            t = map.ReadByte();
            Console.WriteLine("wall {0} {1} to {2} {3} type {4}", x1, y1, x2, y2, t);
        }
#endif
        
        return tmap;
    }

    // Returns the tileset of a map.
    private SimpleTileMapTileset GetMapTileset(FastStream map)
    {
        int a, b, c;
        map.Position = 0x00D4;
        a = map.ReadInt16();
        b = map.ReadInt16();
        c = map.ReadInt16();
        if (a == 191 && b == 192 && c == 193) {
            return Global.GameData.GrassTileset;
        }
        if (a == 194 && b == 195 && c == 196) {
            return Global.GameData.SwampTileset;
        }
        if (a == 197 && b == 198 && c == 199) {
            return Global.GameData.DungeonTileset;
        }
        throw new Exception(String.Format("Unknown tileset {0}/{1}/{2}", a, b, c));
    }

    // Get the map objective from a map. Used By GetMapChunk
    private string GetMapObjective(FastStream map)
    {
        int x;
        map.Position = 0x94;
        map.Position = x = map.ReadUInt16();
        while (map.PeekByte() != 0) {
            ++map.Position;
        }
        return new ASCIIEncoding().GetString(map.Buffer, x, map.Position - x);
    }

    // Returns a ScriptNode, and advances the stream position correctly.
    private static ScriptNode ReadScriptNode(FastStream stream)
    {
        Byte b = stream.ReadByte();
        ScriptNode res;
        if (b == 0x00) {
            res = new ScriptNodeBreakAction();
        } else if (b == 0x01) {
            res = new ScriptNodeWait();
            (res as ScriptNodeWait).WaitTime = stream.ReadByte();
        } else if (b == 0x03) {
            res = new ScriptNode(); // Empty, but jump
            int z = stream.ReadUInt16();
            stream.Position = z;
        } else if (b == 0x04) {
            res = new ScriptNodeChangeFrame();
            (res as ScriptNodeChangeFrame).NewFrame = stream.ReadByte();
        } else if (b == 0x05) {
            res = new ScriptNodeMove();
            (res as ScriptNodeMove).MoveDistance = stream.ReadByte();
        } else if (b == 0x06) {
            // This is a combination of 0x04 and 0x05
            res = new ScriptNodeMove();
            (res as ScriptNodeMove).MoveDistance = stream.ReadByte();
            res.Next = new ScriptNodeChangeFrame();
            (res.Next as ScriptNodeChangeFrame).NewFrame = stream.ReadByte();
        } else if (b == 0x07) {
            //Console.WriteLine("FIXME, anim opcode 0x07 not implemented");
            stream.ReadByte(); // Throw away the parameter.
            res = new ScriptNode();
        } else if (b == 0x0A) {
            // Handle A as a wait instruction.
            // NOT SURE
            res = new ScriptNodeWait();
            (res as ScriptNodeWait).WaitTime = stream.ReadByte();
        } else if (b == 0x02) {
            //Console.WriteLine("FIXME, anim opcode 0x02 not implemented");
            res = new ScriptNode();
        } else if (b == 0x08) {
            res = new ScriptNodeAction();
        } else if (b == 0x09) {
            //Console.WriteLine("FIXME, anim opcode 0x09 not implemented");
            res = new ScriptNode();
        } else {
            throw new Exception(String.Format("Unrecognised opcode {0}", b));
        }
        return res;
    }

    // Decode the animation script chunk.
    //
    // There is only one such chunk in WC1, 212, but
    // it might be different for demos and the like
    //
    // Format information:
    // The chunk begins with a bunch(first 67 are valid) int16 offsets into the file,
    // First 51 are for units, the rest are for missiles
    //
    // Every unit has 6 animations, every missile has animations. The animations
    // are word offsets into the file, where actual script data is found.
    // It's noticeable that a lot of those animations are 1356 or 134C,
    // at the end.
    //
    // The 6 unit animations are:
    //         0: Constant 1356, no idea.
    //         1: Death animation.
    //         2: Idle animation.
    //         3: Move animation.
    //         4: Attack/action animation (also used for resources, repairing, etc)
    //         5: This is generally 134C. Skeletons use this for the raise from the dead animation.
    //            Buildings also have this. I assume it's a spawn animation.
    //
    // The 3 missile animations are:
    //         0: Spawn
    //         1: Move
    //         2: Hit
    // I'm not exactly sure
    //
    // Animation 51, has 3 animations so it qulifies as a missile, but it's used for
    // corpses. Anim 1 is a human corpse, Anim 2 is an orc corpse.
    //
    // The actual script code is noticeable for small byte values. op-codes
    // are 1 bytes each, ranging from 0 to 1, but can have parameters.
    // Here is a list of opcodes:
    //         00       : can break action here.
    //         01 xx    : wait xx cycles.
    //         02       : unknown
    //         03 xx xx : jump to xx xx.
    //         04 xx    : change display frame to xx.
    //         05 xx    : move sprite xx pixels in the direction of movement
    //         06 xx yy : combination of 04 and 05, equivalent to 05 xx 04 yy
    //         07 xx    : wait xx cycles if we didn't upgrade speed, used for paladins and wolf riders.
    //         08       : Either attack sound or attack damage, can't really tell.
    //         09       : Either attack sound or attack damage, can't really tell.
    //         0A xx    : Appears in peasant/peon. Maybe a removed speed upgrade?
    // I don't know any other opcodes, these are probably the only valid ones.
    public static ScriptNode[][] GetAnimations(FastStream stream)
    {
        int[] HeaderOffsets;
        int[][] AnimOffsets;
        ScriptNode[] Nodes;
        ScriptNode prevnode;
        int i, j;

        HeaderOffsets = new int[WarGameData.EntityCount];
        AnimOffsets = new int[WarGameData.EntityCount][];
        Nodes = new ScriptNode[stream.Size];
      
        // Header information, reads the offsets of individual entry points.
        stream.Position = 0;
        for (i = 0; i < WarGameData.EntityCount; ++i) {
            HeaderOffsets[i] = stream.ReadUInt16();
        }
        for (i = 0; i < WarGameData.EntityCount; ++i) {
            stream.Position = HeaderOffsets[i];
            if (i < WarGameData.UnitCount) {
                AnimOffsets[i] = new int[6];
            } else {
                AnimOffsets[i] = new int[3];
            }
            for (j = 0; j < AnimOffsets[i].Length; ++j) {
                AnimOffsets[i][j] = stream.ReadUInt16();
            }
        }

        // Parse the actual animation nodes.
        for (i = 0; i < WarGameData.EntityCount; ++i) {
            for (j = 0; j < AnimOffsets[i].Length; ++j) {
                try {
                    stream.Position = AnimOffsets[i][j];
                    prevnode = null;
                    while (Nodes[stream.Position] == null) {
                        int oldpos;
                        ScriptNode nextnode;
                        oldpos = stream.Position;
                        nextnode = ReadScriptNode(stream);
                        Nodes[oldpos] = nextnode;
                        if (prevnode != null) {
                            prevnode.Next = nextnode;
                        }
                        for (; nextnode.Next != null; nextnode = nextnode.Next) {
                        }
                        prevnode = nextnode;
                    }
                    if (prevnode != null) {
                        if (prevnode.Next != null) {
                            throw new Exception("This sux");
                        }
                        prevnode.Next = Nodes[stream.Position];
                    }
                } catch (Exception e) {
                    Console.WriteLine("Failed loading animation {0}.{1}", i, j);
                    Console.WriteLine("Caught exception: {0}", e.Message);
                }
            }
        }

        // Create and return EntryPoints array
        ScriptNode[][] EntryPoints = new ScriptNode[AnimOffsets.Length][];
        for (i = 0; i < WarGameData.EntityCount; ++i) {
            EntryPoints[i] = new ScriptNode[AnimOffsets[i].Length];
            for (j = 0; j < EntryPoints[i].Length; ++j) {
                EntryPoints[i][j] = Nodes[AnimOffsets[i][j]];
            }
        }
        return EntryPoints;
    }
}

