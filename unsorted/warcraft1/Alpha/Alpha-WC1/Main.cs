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
using System.IO;
using System.Drawing;

using Alpha.Util;
using Alpha.Graphics;
using Alpha.UI;

// Static class containing global information.
class Global
{
    // The main window.
    public static Window Window;

    // The main font.
    public static SimpleFont Font;

    // The big archive with the game resources.
    public static WarArchive Archive;

    // Game resources in a more accessible format.
    public static ResourceManager Resources;

    // The big GameData object.
    public static WarGameData GameData;

    // Game drawing information.
    public static GameGraphicFactory Graphics;

    // Game controller. Implements most game UI logic.
    public static GameController GameUI;

    // The current style.
    public static WC1Style Style;

    // The current style.
    public static WC1Style HumanStyle;

    // The current style.
    public static WC1Style OrcStyle;

    // Dump the contents of the warcraft data file to the chunks dir.
    private static void DumpChunks()
    {
        int i;
        Directory.CreateDirectory("chunks");

        for (i = 0; i < Archive.ChunkCount; ++i) {
            try {
                FastStream s;
                FileStream f;

                s = Archive.GetChunk(i);
                f = File.Open(String.Format("chunks/chunk-{0,3}", i), FileMode.OpenOrCreate, FileAccess.Write);
                f.Write(s.Buffer, 0, s.Size);
                f.Close();
                Console.WriteLine("Wrote chunk {0}, size {1}.", i, s.Size);
            } catch (Exception e) {
                Console.WriteLine("Failed on chunk {0}, skipped. Message: {1}", i, e.Message);
            }
        }
    }

    // Main program.
    // TODO: proper handling of command line arguments.
    // Maybe do a command line class in Alpha?
    public static void Main(string[] args)
    {
        Application.Init();

        if (args.Length < 1) {
            Console.WriteLine("Usage: ./Alpha-WC1.exe <path to warcraft 1 dir>");
            return;
        }
        // Create the main window. This also intializes opengl so it much go first.
        Window = new SdlWindow(640, 400, false);

        // Open the data archive.
        LoadArchive(args[args.Length - 1]);

        if (args.Length == 2 && args[0] == "--dump-chunks") {
            DumpChunks();
            return;
        }

        // Load the resources.
        Resources = new ResourceManager(Archive);

        // Load game data.
        GameData = new WarGameData();

        // Create GameGraphicFactory
        Graphics = new GameGraphicFactory();
        
        // Load button sets.
        GameUI = new GameController();

        // Load styles and fonts.
        CreateStyles();

        // Start the menu.
        SetupWindow();
        Window.Child = new MainMenu(Style);

        // Print how long it took.
        Console.WriteLine("Loading took {0} miliseconds", Environment.TickCount - Application.StartClock);

        // We call the Run() function which will initialize the main loop and start the game.
        Application.Run();
        Window.Dispose();
    }

    private static void CreateStyles()
    {
        // Load font.
        ImageGridFontData fontData = new ImageGridFontData("Data/font2.png");
        fontData.Margin = 0.025;
        Font = new Alpha.Graphics.SimpleFont(fontData, 0.02);

        // Load styles.
        Style = HumanStyle = new WC1Style(Font, Race.Human);
        OrcStyle = new WC1Style(Font, Race.Orc);
        ((SimpleFont)Style.Font).Height = 8;
        // Window.Style.Font = new Alpha.Font(ImageGridFontData.CreateFromSystemFont(), 7);
    }

    // Attach event handlers.
    private static void SetupWindow()
    {
        Window.Title = "Alpha WC1";
        Window.Close += new EventHandler(Window_Close);
        Window.Resize += new ResizeEventHandler(Window_Resize);
        Window.KeyUp += new KeyEventHandler(Window_KeyUp);
        Window.DefaultCursor = (Cursor)Global.Resources.Get("cursor-arrow");
        UpdateWindowSize();
    }

    // load the data archive
    private static void LoadArchive(string datadir)
    {
        string path;
        char sep = Path.DirectorySeparatorChar;

        Archive = null;

        // Try the full version.
        path = GetCaseInsensitivePath(datadir, "fdata" + sep + "data.war");
        if (path != null) {
            try {
                Console.WriteLine("Detected full version");
                Archive = new WarArchive(path);
                return;
            } catch (Exception e) {
                throw new Exception("Can't load Warcraft 1 data", e);
            }
        }
       
        // Try the demo version.
        path = GetCaseInsensitivePath(datadir, "demodata" + sep + "data.war");
        if (path != null) {
            try {
                Console.WriteLine("Detected demo version");
                Archive = new WarArchive(path);
                return;
            } catch (Exception e) {
                throw new Exception("Can't load Warcraft 1 data", e);
            }
        }

        // Try the mac demo version
        path = GetCaseInsensitivePath(datadir, "war demo data");
        if (path != null) {
            try {
                Console.WriteLine("Detected mac demo version");
                Archive = new WarArchive(path);
                return;
            } catch (Exception e) {
                throw new Exception("Can't load Warcraft 1 data", e);
            }
        }
 
        throw new Exception("Can't load warcraft 1 data, neither full nor demo version detected.");
    }

    // Get a Case-insensitive path, takes the dir and path relative to that dir.
    // It's the same as dir + path, except path is case-insensitive.
    //
    // Returns null if not found.
    public static string GetCaseInsensitivePath(string dir, string path)
    {
        int pos = path.IndexOf(Path.DirectorySeparatorChar);

        if (pos >= 0) {
            string subdir = path.Substring(0, pos);
            path = path.Substring(pos + 1);

            foreach (string s in Directory.GetDirectories(dir)) {
                if (s.ToLower() == Path.Combine(dir, subdir).ToLower()) {
                    return GetCaseInsensitivePath(s + Path.DirectorySeparatorChar, path);
                }
            }
            return null;
        } else {
            foreach (string s in Directory.GetFiles(dir)) {
                if (s.ToLower() == Path.Combine(dir, path).ToLower()) {
                    return s;
                }
            }
            return null;
        }
    }

    // Make a screenshot. Screenshots are named "Alpha-WC1-Screenshot-{0}.png",
    // where {0} is a number, possibly containing more than one digit.
    // Screenshots are made in the current directory (whatever that is.)
    //
    // Fails/succeeds with a message on the Console.
    public static void MakeScreenShot()
    {
        try {
            int cnt = 1;
            string filename;
            Bitmap bmp;
            while (true) {
                filename = String.Format("Alpha-WC1-Screenshot-{0}.png", cnt);
                if (File.Exists(filename) == false && Directory.Exists(filename) == false) {
                    break;
                }
                ++cnt;
            }
            bmp = Window.ScreenShot();
            bmp.Save(filename, System.Drawing.Imaging.ImageFormat.Png);
            Console.WriteLine("Screenshot saved in {0}", filename);
        } catch {
            Console.WriteLine("Screenshot failed");
        }
    }

    private static void Window_KeyUp(object o, KeyEventArgs e)
    {
        // Make a screenshot if PrintScreen is pressed.
        // This works everywhere.
        if (e.Key == KeyCode.PrintScreen) {
            MakeScreenShot();
        }
    }

    private static void UpdateWindowSize()
    {
        if (200 / Window.Height > 320 / Window.Width) {
            Window.XScale = 200 / Window.Height;
            Window.YScale = 200 / Window.Height;
            Window.XTrans = (320 - Window.Width * 200 / Window.Height) / 2;
            Window.YTrans = 0;
        } else {
            Window.XScale = 320 / Window.Width;
            Window.YScale = 320 / Window.Width;
            Window.XTrans = 0;
            Window.YTrans = (200 - Window.Height * 320 / Window.Width) / 2;
        }
    }

    private static void Window_Resize(object o, ResizeEventArgs e)
    {
        UpdateWindowSize();
    }

    private static void Window_Close(object o, EventArgs e)
    {
        Application.Quit();
    }
}
