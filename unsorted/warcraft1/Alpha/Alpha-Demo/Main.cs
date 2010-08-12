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
using System.Drawing;
using System.IO;

using Alpha.UI;
using Alpha.Graphics;

// Static class containing global variables.
// Global variables are only allowed in here.
class Global
{
    // The main window.
    public static Window Window;

    // A silly background box.
    public static ImageBox BackgroundBox;

    // The main style.
    public static Style Style;

    // Gameplayer info.
    public static GameData GameData;

    // Game drawing information.
    public static GameGraphicFactory Graphics;
    
    public static void Main()
    {
        //Console.WriteLine("I'm executing in {0}", System.IO.Directory.GetCurrentDirectory());
        Application.Init();

        // Create the main window.
        Window = new SdlWindow(800, 600);
        Window.Title = "Alpha Demo";
        Window.Resize += new ResizeEventHandler(Window_Resize);
        Window.Close += new EventHandler(Window_Close);
        Window.KeyUp += new KeyEventHandler(Window_KeyUp);

        // Create the style.
        Style = new Style(new Alpha.Graphics.SimpleFont(new ImageGridFontData("Data/font2.png"), 0.02));
        ((ImageGridFontData)Style.FontData).Margin = 0.025;
        ((ImageGridFontData)Style.FontData).XTranslate = 0.15;

        // Create an universal background.
        BackgroundBox = new ImageBox();
        BackgroundBox.Texture = new Texture(new Bitmap("Data/luvr2.jpg"));
        Window.AddOnTop(BackgroundBox);
        
        // Set the sizes.
        Window.XScale = 1.0 / Window.Height;
        Window.YScale = 1.0 / Window.Height;
        BackgroundBox.SetSize(0, 0, Window.Width * Window.XScale, Window.Height * Window.YScale);

        // These are very important.
        GameData = new GameData();
        Graphics = new GameGraphicFactory();

        // Start the Menu.
        Global.Window.Child = new Menu(Style);

        // We call the Run() function which will initialize the main loop and start the game.
        Application.Run();
        Window.Dispose();
    }

    // Make a screenshot. Screenshots are named "Alpha-Demo-Screenshot-{0}.png",
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
                filename = String.Format("Alpha-Demo-Screenshot-{0}.png", cnt);
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

    private static void Window_Close(object o, EventArgs e)
    {
        Application.Quit();
    }

    private static void Window_Resize(object o, ResizeEventArgs e)
    {
        Window.XScale = 1.0 / e.Height;
        Window.YScale = 1.0 / e.Height;
        if (Window.Width * Window.XScale < 4.0 / 3) {
            BackgroundBox.SetSize(0, 0, 4.0 / 3, 1);
        } else {
            BackgroundBox.SetSize(0, 0, Window.Width * Window.XScale, Window.Width * Window.XScale * 3 / 4);
        }
    }
}
