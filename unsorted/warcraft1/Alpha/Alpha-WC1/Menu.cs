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
using Color = System.Drawing.Color;

using Alpha.UI;
using Alpha.Util;
using Alpha.Graphics;
using Alpha.Game;

// This is the class for the main menu
public class MainMenu: Widget
{
    public readonly WC1Style Style;

    public MainMenu(WC1Style style)
    {
        IButton btn;
        ImageBox box;

        Style = style;

        box = new ImageBox();
        box.Texture = (Image)Global.Resources.Get("warcraft-logo");
        box.SetSize(0, 0, 320, 200);
        this.AddOnTop(box);

        // New Game button
        btn = style.NewMediumButton("New Game");
        btn.SetSize(104, 100, 112, 15);
        btn.Click += new EventHandler(NewGame_Click);
        this.AddOnTop(btn);

        // Quit Game Button
        btn = style.NewMediumButton("Quit Game");
        btn.SetSize(104, 150, 112, 15);
        btn.Click += new EventHandler(QuitGame_Click);
        this.AddOnTop(btn);
    }

    private void NewGame_Click(object sender, EventArgs args)
    {
        Global.Window.Child = new MapPickerMenu(Style);
    }

    private void QuitGame_Click(object sender, EventArgs args)
    {
        Application.Quit();
    }
}

// Just contains the map chunk to load.
public class CampaignGameStarter: IGameStarter
{
    public readonly int MapChunk;

    public CampaignGameStarter(int mapChunk)
    {
        MapChunk = mapChunk;
    }

    public GameState CreateGame()
    {
        WarGameState state;
        
        state = (WarGameState)Global.GameData.CreateGameState();
        Global.Archive.GetMapChunk(state, MapChunk);

        return state;
    }
}

public class MapPickerMenu: Widget
{
    // This Hashtable maps Buttons to chunk numbers
    private Hashtable ButtonToMap;

    private void AddNewButton(int x, int y, string name, int mapChunk)
    {
        IButton btn;

        btn = Global.Style.NewSmallButton(name);
        btn.SetSize(x, y, 54, 15);
        btn.Click += new EventHandler(ChooseMapButton_Click);
        ButtonToMap.Add(btn, mapChunk);
        this.AddOnTop(btn);
    }

    public MapPickerMenu(WC1Style style)
    {
        IButton btn;
        TextLayout lbl;
        ImageBox box;

        box = new ImageBox();
        try {
            box.Texture = (Image)Global.Resources.Get("orc-throne-screen");
        } catch {
            // Demo hack.
            box.Texture = (Image)Global.Resources.Get("warcraft-logo");
        }
        box.SetSize(0, 0, 320, 200);
        this.AddOnTop(box);

        // Cancel button
        btn = style.NewMediumButton("Cancel");
        btn.SetSize(104, 170, 112, 15);
        btn.Click += new EventHandler(CancelButton_Click);
        this.AddOnTop(btn);

        // Human campaign label.
        lbl = new TextLayout(style.Font);
        lbl.SetSize(20, 20, 123, 15);
        lbl.Text = "Human campaign";
        lbl.HorizontalAlignment = HorizontalTextAlignment.Center;
        this.AddOnTop(lbl);

        // Orc campaign label.
        lbl = new TextLayout(style.Font);
        lbl.SetSize(177, 20, 123, 15);
        lbl.Text = "Orc campaign";
        lbl.HorizontalAlignment = HorizontalTextAlignment.Center;
        this.AddOnTop(lbl);

        ButtonToMap = new Hashtable();

        // There are many more levels, all chunks from 117 to 140 are levels
        // This only lets you select campaign levels
        // FIXME: use a list or something

        // Human levels.
        AddNewButton(20,  40, "1", 117);
        AddNewButton(20,  60, "2", 119);
        AddNewButton(20,  80, "3", 121);
        AddNewButton(20, 100, "4", 123);
        AddNewButton(20, 120, "5", 125);
        AddNewButton(20, 140, "6", 127);
        AddNewButton(89,  40, "7", 129);
        AddNewButton(89,  60, "8", 131);
        AddNewButton(89,  80, "9", 133);
        AddNewButton(89, 100, "10", 135);
        AddNewButton(89, 120, "11", 137);
        AddNewButton(89, 140, "12", 139);

        // Orc levels.
        AddNewButton(177,  40, "1", 118);
        AddNewButton(177,  60, "2", 120);
        AddNewButton(177,  80, "3", 122);
        AddNewButton(177, 100, "4", 124);
        AddNewButton(177, 120, "5", 126);
        AddNewButton(177, 140, "6", 128);
        AddNewButton(246,  40, "7", 130);
        AddNewButton(246,  60, "8", 132);
        AddNewButton(246,  80, "9", 134);
        AddNewButton(246, 100, "10", 136);
        AddNewButton(246, 120, "11", 138);
        AddNewButton(246, 140, "12", 140);
    }

    private void ChooseMapButton_Click(object sender, EventArgs args)
    {
        if (!ButtonToMap.Contains(sender)) {
            throw new Exception("Sender must be wrong.");
        }

        IGameStarter starter;
        SinglePlayerGame game;
       
        starter = new CampaignGameStarter((int)ButtonToMap[sender]);
        game = new SinglePlayerGame(starter.CreateGame());
        game.SetCycleLength(15);
        game.ProfilingEnabled = false;
        game.StartGame(Application.ThisClock);

        // FIXME: use the race.
        if ((int)ButtonToMap[sender] % 2 != 0) {
            Global.Window.Child = new GameScreen(Global.HumanStyle);
        } else {
            Global.Window.Child = new GameScreen(Global.OrcStyle);
        }
        (Global.Window.Child as GameScreen).Game = game;
        Console.WriteLine("*Hint*: Press enter to show the command line");
        Console.WriteLine("*Hint*: \"player X\": Changes the current player to X");
        Console.WriteLine("*Hint*: \"there can be only one\": toggle invincibility");
        Console.WriteLine("*Hint*: \"hurry up guys\": toggle fast building");
        Console.WriteLine("*Hint*: \"pot of gold\": 10000 gold and 5000 wood");
        Console.WriteLine("*Hint*: \"mana burn\": toggle fast mana regeneration");
    }

    private void CancelButton_Click(object sender, EventArgs args)
    {
        Global.Window.Child = new MainMenu(Global.Style);
    }
}

// Just a silly blocker.
// Used by the GameScreen when a menu is activated.
//
// FIXME: This doesn't match the effect in WC1
// That effects needs the color matrix.
public class Blocker: Widget
{
    public Blocker()
    {
        SetSize(0, 0, 320, 200);
    }

    public override void Draw(RectangleD r1, RectangleD r2)
    {
        GL.Disable(Feature.Texture);
        GL.Color(127, 0, 0, 190);
        GL.Begin(DrawMode.Quads);
        GL.Vertex(0, 0);
        GL.Vertex(Width, 0);
        GL.Vertex(Width, Height);
        GL.Vertex(0, Height);
        GL.End();
        base.Draw(r1, r2);
    }
}

// Base class for dialog-style menus.
// They have a Cancel event, and can be called from just about everywhere.
public class GameDialog: Widget
{
    public event EventHandler Cancel;

    protected void OnCancel()
    {
        if (Cancel != null) {
            Cancel(this, new EventArgs());
        }
    }

    // Launch another dialog.
    // It hides the current dialog and places the other dialog on top.
    public void LaunchSubdialog(GameDialog dialog)
    {
        dialog.Cancel += new EventHandler(Subdialog_Cancel);
        Hidden = true;
        Parent.AddOnTop(dialog);
    }

    private void Subdialog_Cancel(object sender, EventArgs args)
    {
        Hidden = false;
    }
}

// The big menu shown when clicking the menu button in the game screen.
public class GameMainMenu: GameDialog
{
    public readonly WC1Style Style;

    public GameMainMenu(WC1Style style)
    {
        ImageBox box;
        IButton btn;

        Style = style;

        SetSize(0, 0, 320, 200);

        box = new ImageBox();
        box.Texture = style.GetRaceTexture("ui-menu-panel");
        box.SetSize(120, 20, 152, 136);
        this.AddOnTop(box);

        btn = style.NewMediumButton("Save Game");
        btn.SetSize(21, 28, 112, 15);
        btn.Click += new EventHandler(SaveGame_Click);
        box.AddOnTop(btn);
        
        btn = style.NewMediumButton("Load Game");
        btn.SetSize(21, 48, 112, 15);
        btn.Click += new EventHandler(LoadGame_Click);
        box.AddOnTop(btn);

        btn = style.NewMediumButton("Options");
        btn.SetSize(21, 68, 112, 15);
        btn.Click += new EventHandler(Continue_Click);
        box.AddOnTop(btn);

        btn = style.NewMediumButton("Restart");
        btn.SetSize(21, 88, 112, 15);
        btn.Click += new EventHandler(Restart_Click);
        box.AddOnTop(btn);

        btn = style.NewSmallButton("Resume");
        btn.SetSize(21, 108, 54, 15);
        btn.Click += new EventHandler(Continue_Click);
        box.AddOnTop(btn);

        btn = style.NewSmallButton("Quit");
        btn.SetSize(80, 108, 54, 15);
        btn.Click += new EventHandler(Quit_Click);
        box.AddOnTop(btn);
    }

    private void SaveGame_Click(object sender, EventArgs args)
    {
        LaunchSubdialog(new GameSaveMenu(Style));
    }

    private void LoadGame_Click(object sender, EventArgs args)
    {
        LaunchSubdialog(new GameLoadMenu(Style));
    }

    private void Restart_Click(object sender, EventArgs args)
    {
        LaunchSubdialog(new GameRestartMenu(Style));
    }

    private void Continue_Click(object sender, EventArgs args)
    {
        Parent = null;
        OnCancel();
    }

    private void Quit_Click(object sender, EventArgs args)
    {
        LaunchSubdialog(new GameQuitMenu(Style));
    }
}

// Dialog shown when clicking quit.
public class GameQuitMenu: GameDialog
{
    public GameQuitMenu(WC1Style style)
    {
        ImageBox box;
        IButton btn;
        TextLayout layout;

        SetSize(0, 0, 320, 200);

        box = new ImageBox();
        box.Texture = style.GetRaceTexture("ui-horizontal-panel");
        box.SetSize(17, 70, 286, 48);
        this.AddOnTop(box);

        layout = new TextLayout(style.Font, "Quit to OS or Main Menu?");
        layout.SetSize(0, 4, 286, 16);
        layout.HorizontalAlignment = HorizontalTextAlignment.Center;
        layout.VerticalAlignment = VerticalTextAlignment.Center;
        box.AddOnTop(layout);

        btn = style.NewSmallButton("OS");
        btn.SetSize(12, 29, 54, 15);
        btn.Click += new EventHandler(QuitOs_Click);
        box.AddOnTop(btn);
        
        btn = style.NewSmallButton("Menu");
        btn.SetSize(114, 29, 54, 15);
        btn.Click += new EventHandler(QuitMenu_Click);
        box.AddOnTop(btn);

        btn = style.NewSmallButton("Cancel");
        btn.SetSize(216, 29, 54, 15);
        btn.Click += new EventHandler(Cancel_Click);
        box.AddOnTop(btn);
    }

    private void QuitOs_Click(object sender, EventArgs args)
    {
        Application.Quit();
    }

    private void QuitMenu_Click(object sender, EventArgs args)
    {
        // FIXME: HACK
        (Global.Window.Child as GameScreen).Exit();
        Global.Window.Child = new MainMenu(Global.Style);
    }

    private void Cancel_Click(object sender, EventArgs args)
    {
        OnCancel();
        Parent = null;
    }
}

// Dialog shown when clicking restart.
public class GameRestartMenu: GameDialog
{
    public GameRestartMenu(WC1Style style)
    {
        ImageBox box;
        IButton btn;
        TextLayout layout;

        SetSize(0, 0, 320, 200);

        box = new ImageBox();
        box.Texture = style.GetRaceTexture("ui-horizontal-panel");
        box.SetSize(17, 70, 286, 48);
        this.AddOnTop(box);

        layout = new TextLayout(style.Font, "Are you sure you want to restart?");
        layout.SetSize(0, 4, 286, 16);
        layout.HorizontalAlignment = HorizontalTextAlignment.Center;
        layout.VerticalAlignment = VerticalTextAlignment.Center;
        box.AddOnTop(layout);

        btn = style.NewSmallButton("Ok");
        btn.SetSize(12, 29, 54, 15);
        btn.Click += new EventHandler(Ok_Click);
        box.AddOnTop(btn);
        
        btn = style.NewSmallButton("Cancel");
        btn.SetSize(216, 29, 54, 15);
        btn.Click += new EventHandler(Cancel_Click);
        box.AddOnTop(btn);
    }

    private void Ok_Click(object sender, EventArgs args)
    {
        throw new NotImplementedException();
    }

    private void Cancel_Click(object sender, EventArgs args)
    {
        OnCancel();
        Parent = null;
    }
}

// Save game dialog.
public class GameSaveMenu: GameDialog
{
    // Array of text fields with names.
    private IEditBox[] Fields;

    public GameSaveMenu(WC1Style style)
    {
        ImageBox box;
        IButton btn;
        TextLayout layout;
        int i;

        SetSize(0, 0, 320, 200);

        box = new ImageBox();
        box.Texture = style.GetRaceTexture("ui-save-load-panel");
        box.SetSize(52, 23, 223, 146);
        this.AddOnTop(box);

        layout = new TextLayout(style.Font, "Save Game");
        layout.SetSize(0, 0, 223, 16);
        layout.HorizontalAlignment = HorizontalTextAlignment.Center;
        layout.VerticalAlignment = VerticalTextAlignment.Center;
        box.AddOnTop(layout);

        Fields = new IEditBox[7];
        for (i = 0; i < Fields.Length; ++i) {
            Fields[i] = new EditBox(style);
            Fields[i].SetSize(20, 18 + 15 * i, 184, 15);
            Fields[i].Text = String.Format("save slot {0}", i);
            box.AddOnTop(Fields[i]);
        }

        btn = style.NewSmallButton("Cancel");
        btn.SetSize(82, 126, 54, 15);
        btn.Click += new EventHandler(Cancel_Click);
        box.AddOnTop(btn);
    }

    private void Cancel_Click(object sender, EventArgs args)
    {
        OnCancel();
        Parent = null;
    }
}

// Load game dialog.
public class GameLoadMenu: GameDialog
{
    // Array of text fields with names.
    private TextLayout[] Fields;

    public GameLoadMenu(WC1Style style)
    {
        ImageBox box;
        IButton btn;
        TextLayout layout;
        int i;

        SetSize(0, 0, 320, 200);

        box = new ImageBox();
        box.Texture = style.GetRaceTexture("ui-save-load-panel");
        box.SetSize(52, 23, 223, 146);
        this.AddOnTop(box);

        layout = new TextLayout(style.Font, "Load Game");
        layout.SetSize(0, 0, 223, 16);
        layout.HorizontalAlignment = HorizontalTextAlignment.Center;
        layout.VerticalAlignment = VerticalTextAlignment.Center;
        box.AddOnTop(layout);

        Fields = new TextLayout[7];
        for (i = 0; i < Fields.Length; ++i) {
            Fields[i] = new TextLayout(style.Font);
            Fields[i].SetSize(20, 18 + 15 * i, 184, 15);
            Fields[i].Text = String.Format("load slot {0}", i);
            box.AddOnTop(Fields[i]);
        }

        btn = style.NewSmallButton("Cancel");
        btn.SetSize(82, 126, 54, 15);
        btn.Click += new EventHandler(Cancel_Click);
        box.AddOnTop(btn);
    }

    private void Cancel_Click(object sender, EventArgs args)
    {
        OnCancel();
        Parent = null;
    }
}
