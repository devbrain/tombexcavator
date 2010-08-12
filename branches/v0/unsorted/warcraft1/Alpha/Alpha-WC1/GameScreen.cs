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
using System.Text.RegularExpressions;

using Alpha.Util;
using Alpha.Graphics;
using Alpha.UI;
using Alpha.Game;
using Alpha.Scene;
using Alpha.GameUI;

// A game screen is a viewer/controller for a Game.
// It's a widget derivate, but it's a huge widget and was designed as to
// ocupy the entire window.
//
// This class contains most of the game's in-game ui. I made it as
// close to Warcraft's actual UI as possible, so it's more or less
// hardcoded as a 320x200 window.
public class GameScreen: Widget, IGameScreen
{
    // The actual game we are playing with.
    private Game game;
    public Game Game {
        get {
            return game;
        }
        set {
            game = value;
            Player = (WarPlayer)Game.State.GetPlayer(0);
            View.MapSceneBuilder = Global.Graphics.CreateSceneBuilder(Game.State.Map, Player);
            View.Scene.AddObject(SelectionGraphic);

            // Debug pathfinding.
            //MapDrawerGraphic pfgraf = new MapDrawerGraphic(Game.State.Map, Player, new PathfinderDrawer());
            //pfgraf.DrawLayer = -90;
            //View.Scene.AddObject(pfgraf);
        }
    }

    // The current player, player playing at this console.
    private WarPlayer player;
    public WarPlayer Player {
        get {
            return player;
        }
        set {
            if (value == null) {
                throw new ArgumentNullException("value");
            }
            if (value.State != Game.State) {
                throw new ArgumentException("value");
            }
            if (player == value) {
                return;
            }
            player = value;
            SelectionGraphic.Player = player;
            SelectedEntities = new ArrayList();
        }
    }

    // Implicit implementation.
    Player IGameScreen.Player {
        get {
            return this.Player;
        }
    }

    // Remap UI resource for drawing.
    // Goes through both race remap and tileset remap.
    public ILazyLink RemapResource(ILazyLink res)
    {
        if (Style != null) {
            res = Global.Resources.Remap(res, Style.Race);
        }
        if (View.Map is SimpleTileMap) {
            Tileset tset = ((WarGameData)View.Map.Data).GetMapTilesetEnum(View.Map);
            res = Global.Resources.Remap(res, tset);
        }
        return res;
    }

    // The gameview;
    private OrthoMapView view;
    public OrthoMapView View {
        get {
            return view;
        }
    }

    // The map.
    public GameMap Map {
        get {
            return View.Map;
        }
    }

    // Selection drawer.
    private SelectionGraphic SelectionGraphic;

    // Small label at the bottom
    private EditBox BottomLine;

    // Fillers.
    private ImageBox UpperFiller;
    private ImageBox LowerFiller;
    private ImageBox RightFiller;
    private ImageBox MinimapBackground;
    private ImageBox EntityPanelBackground;

    // Amount of resources available.
    private TextLayout WoodLabel;
    private TextLayout GoldLabel;

    // Button for the main menu.
    private IButton MenuButton;

    // This displays the current selection
    private SelectionBox SelectionBox;

    // Button panel
    private ButtonPanel ButtonPanel;

    // Scroller widget.
    private MapScroller MapScroller;

    // Current UnitButtons
    private IList unitButtons;
    public IList UnitButtons {
        get {
            return unitButtons;
        }
        set {
            if (value == null) {
                throw new ArgumentNullException("value");
            }
            unitButtons = value;
        }
    }

    // These are the currently selected entities.
    private IList selectedEntities = new ArrayList();
    public IList SelectedEntities {
        get {
            return ArrayList.ReadOnly(selectedEntities);
        }
        set {
            // Select new entities.
            selectedEntities = Global.GameUI.FilterSelection(this, value);

            // Update.
            SelectionGraphic.SelectedEntities = SelectedEntities;
            UnitButtons = Global.GameUI.GetSelectionButtons(this, SelectedEntities);

            // Fire event.
            if (SelectedEntitiesChanged != null) {
                SelectedEntitiesChanged(this, new EventArgs());
            }
        }
    }

    // Event called when SelectedEntities changes.
    public event EventHandler SelectedEntitiesChanged;

    // The race of the user interface. Can be 'human' or 'orc'
    // This sets images in the interface from the data files.
    private WC1Style style;
    public WC1Style Style {
        get {
            return style;
        }
        set {
            style = value;

            UpperFiller.Texture = Style.GetRaceTexture("ui-upper-filler");
            RightFiller.Texture = Style.GetRaceTexture("ui-right-filler");
            LowerFiller.Texture = Style.GetRaceTexture("ui-lower-filler");
            MinimapBackground.Texture = Style.GetRaceTexture("ui-minimap-info");
            EntityPanelBackground.Texture = Style.GetRaceTexture("ui-unit-panel");

            SelectionBox.Style = Style;
        }
    }

    // Return true if orders should be queued now, of false otherwise.
    public bool QueueOrders {
        get {
            return (Global.Window.IsKeyDown(KeyCode.LeftShift) ||
                    Global.Window.IsKeyDown(KeyCode.RightShift));
        }
    }
    
    // Default constructor. This will build and position the widgets.
    public GameScreen(WC1Style style)
    {
        // Fillers.
        UpperFiller = new ImageBox();
        UpperFiller.SetSize(72, 0, 240, 12);
        this.AddOnTop(UpperFiller);
        RightFiller = new ImageBox();
        RightFiller.SetSize(312, 0, 8, 200);
        this.AddOnTop(RightFiller);
        LowerFiller = new ImageBox();
        LowerFiller.SetSize(72, 188, 240, 12);
        this.AddOnTop(LowerFiller);
        MinimapBackground = new ImageBox();
        MinimapBackground.SetSize(0, 0, 72, 72);
        this.AddOnTop(MinimapBackground);
        EntityPanelBackground = new ImageBox();
        EntityPanelBackground.SetSize(0, 72, 72, 128);
        this.AddOnTop(EntityPanelBackground);

        // Create button panel.
        ButtonPanel = new ButtonPanel(this);
        ButtonPanel.SetSize(3, 117, 64, 70);
        this.AddOnTop(ButtonPanel);

        // Create the Menu Button.
        MenuButton = style.NewMenuButton();
        MenuButton.SetSize(3, 188, 65, 12);
        MenuButton.Click += new EventHandler(MenuButton_Click);
        this.AddOnTop(MenuButton);

        // Create the GameView.
        view = new OrthoMapView();
        double margin = 1.3;
        View.SetSize(72 - margin, 12 - margin, 240 + 2 * margin, 176 + 2 * margin);
        View.Select += new SelectEventHandler(GameView_Select);
        View.Zoom = 16.0 / WarGameData.TileSize;
        View.ClampView = true;
        View.CenterX = View.CenterY = 0;
        View.MapMouseEvent += new MouseEventHandler(View_MapMouseEvent);
        this.AddOnTop(View);

        // Draws current selection in the scene.
        SelectionGraphic = SelectionGraphic.CreateDefault();

        // Create map scroller.
        MapScroller = new MapScroller(this);
        MapScroller.KeyScrollSpeed = 10 * WarGameData.TileSize;
        MapScroller.MouseScrollSpeed = 5 * WarGameData.TileSize;

        // Create the selection box (thing that show currently selected entities).
        SelectionBox = new SelectionBox(style, this);
        SelectionBox.SetSize(2, 72, 66, 46);
        this.AddOnTop(SelectionBox);

        // Create a small info label.
        BottomLine = new EditBox(style);
        BottomLine.SetSize(72, 189.5, 240, 12);
        BottomLine.Editable = false;
        BottomLine.ReturnPressed += new EventHandler(BottomLine_ReturnPressed);
        this.AddOnTop(BottomLine);

        // Small labels for wood and gold.
        GoldLabel = new TextLayout(style.Font);
        GoldLabel.SetSize(72, -1, 200, 12);
        GoldLabel.HorizontalAlignment = HorizontalTextAlignment.Right;
        this.AddOnTop(GoldLabel);
        WoodLabel = new TextLayout(style.Font);
        WoodLabel.SetSize(72, -1, 100, 12);
        WoodLabel.HorizontalAlignment = HorizontalTextAlignment.Right;
        this.AddOnTop(WoodLabel);

        // Misc stuff.
        Application.NewFrame += new EventHandler(Application_NewFrame);
        Style = style;
        LastStatPrint = Application.ThisClock;
        SelectedEntities = new Entity[0];
    }

    private void BottomLine_ReturnPressed(object obj, EventArgs args)
    {
        if (BottomLine.Editable == false) {
            BottomLine.Text = "";
            BottomLine.Editable = true;
        } else {
            HandleTextCommand(BottomLine.Text);
            BottomLine.Text = "";
            BottomLine.Editable = false;
        }
    }

    private void HandleTextCommand(string cmd)
    {
        if (Regex.IsMatch(cmd, @"\A\s*player\s+[0-5]\s*\Z", RegexOptions.IgnoreCase)) {
            int param = -1;
            try {
                param = Int32.Parse(Regex.Match(cmd, @"[0-9]+").Value);
            } catch {
                param = -1;
            }
            if (param < 0 || param > 4) {
                Console.WriteLine("Bad player number");
            } else {
                Console.WriteLine("Switch to player {0}", param);
                Player = (WarPlayer)Game.State.GetPlayer(param);
            }
        } else if (cmd.Trim().ToLower() == "there can be only one") {
            Game.SendCommand(new CheatCommand(Player, Cheat.Invincibility));
        } else if (cmd.Trim().ToLower() == "pot of gold") {
            Game.SendCommand(new CheatCommand(Player, Cheat.Resource));
        } else if (cmd.Trim().ToLower() == "mana burn") {
            Game.SendCommand(new CheatCommand(Player, Cheat.Mana));
        } else if (cmd.Trim().ToLower() == "hurry up guys") {
            Game.SendCommand(new CheatCommand(Player, Cheat.BuildSpeed));
        } else {
            Console.WriteLine("Unrecognized console command: {0}", cmd);
        }
    }

    // Callback for target selection.
    private ITargeter Targeter;

    // Targetting mode, which is on when you are selecting a target or something similar.
    public bool Targetting {
        get {
            return Targeter != null;
        }
    }

    // Start targetting mode.
    public void StartTargetting(ITargeter targeter)
    {
        if (targeter == null) {
            throw new ArgumentNullException("targeter");
        }
        StopTargetting();
        this.Targeter = targeter;

        // Disable rect selection.
        View.SelectionEnabled = false;
    }

    // Stop targetting mode.
    // If not in targetting mode this doesn't do anything.
    public void StopTargetting()
    {
        if (!Targetting) {
            return;
        }
        Targeter.Cancel(this);
        Targeter = null;

        // Enable rect selection.
        View.SelectionEnabled = true;
    }

    // Called for on-click action.
    private void View_MapMouseEvent(object o, MouseEventArgs e)
    {
        if (e.EventType == MouseEventType.ButtonUp) {
            if (Targetting) {
                if (e.Button == MouseButton.Right) {
                    StopTargetting();
                }
                if (e.Button == MouseButton.Left) {
                    if (Targetting) {
                        Targeter.Select(this, View.Map, (int)e.X, (int)e.Y);
                    }
                }
            } else {
                if (e.Button == MouseButton.Right) {
                    Global.GameUI.DefaultTargetHandler(this, (int)e.X, (int)e.Y);
                }
            }
        }
    }

    // When the selected entities change.
    private void GameView_Select(object o, SelectEventArgs e)
    {
        SelectedEntities = e.EntityList;
    }

    // Mouse location in the widget.
    // TODO: Track MouseX / MouseY in Widget?
    public PointD MouseLoc = new PointD(5, 5);

    public override void OnMouseEvent(MouseEventArgs e)
    {
        MouseLoc = e.Location;
        base.OnMouseEvent(e);
    }

    // Updates the cursor.
    protected override void Refresh()
    {
        base.Refresh();

        MapScroller.Refresh();
        if (MouseFocus == false || View.MouseFocus == false) {
            return;
        }

        // Select entities under the cursor.
        // FIXME: horrible hack. Sort of.
        Alpha.Util.Point mapPoint = (Alpha.Util.Point)View.ScreenToMap(
                new PointD(MouseLoc.X - View.PosX, MouseLoc.Y - view.PosY));
        IList list = View.Map.Cache.SelectList(mapPoint);

        if (Targetting) {
            if (list.Count == 0) {
                ParentWindow.FrameCursor = (Cursor)Global.Resources.Get("cursor-yellow-crosshair");
            } else if (Player.ShouldAttack((Entity)list[0])) {
                ParentWindow.FrameCursor = (Cursor)Global.Resources.Get("cursor-red-crosshair");
            } else {
                ParentWindow.FrameCursor = (Cursor)Global.Resources.Get("cursor-green-crosshair");
            }
        } else {
            if (list.Count == 0) {
                ParentWindow.FrameCursor = (Cursor)Global.Resources.Get("cursor-arrow");
            } else {
                ParentWindow.FrameCursor = (Cursor)Global.Resources.Get("cursor-magnifying-glass");
            }
        }
        if (View.SelectionUnderway) {
            ParentWindow.FrameCursor = (Cursor)Global.Resources.Get("cursor-small-green-crosshair");
        }
    }

    private int LastStatPrint = 0;

    // Updates done every frame.
    private void Update()
    {
        Game.Update(Application.ThisClock);
        if (Application.ThisClock - LastStatPrint > 5000 &&
                Game.ProfilingEnabled && Game.CyclesCalculated >= 100) {
            LastStatPrint = Application.ThisClock;
            int c = Game.CyclesCalculated, t = Game.TimeSpent;
            Console.WriteLine("{0} cycles in {1} ticks {2} average {3} capacity",
                    c, t, (double)t / c, (100.0 * t / c) / Game.CycleLength);
        }

        if (Parent == null || blocker != null) {
            return;
        }

        // FIXME: hack
        ParentWindow.KeyFocusWidget = BottomLine;

        GoldLabel.Text = Player.GoldAmount.ToString();
        WoodLabel.Text = Player.WoodAmount.ToString();

        // FIXME: hack to refilter.
        SelectedEntities = SelectedEntities;
    }

    // This is called by Application every frame.
    private void Application_NewFrame(object o, EventArgs e)
    {
        Update();
    }

    // Disconnect everything.
    public void Exit()
    {
        Application.NewFrame -= new EventHandler(Application_NewFrame);
        Global.Window.Child = null;
    }

    private Blocker blocker;

    // When the user clicks the menu button.
    private void MenuButton_Click(object o, EventArgs e)
    {
        GameDialog dialog = new GameMainMenu(Style);
        dialog.Cancel += new EventHandler(Dialog_Cancel);
        AddOnTop(blocker = new Blocker());
        AddOnTop(dialog);
        if (Game is SinglePlayerGame) {
            ((SinglePlayerGame)Game).PauseGame();
        }
    }

    private void Dialog_Cancel(object sender, EventArgs args)
    {
        Remove(blocker);
        blocker = null;
        if (Game is SinglePlayerGame) {
            ((SinglePlayerGame)Game).StartGame(Application.ThisClock);
        }
    }
}

// Simple widget used to scroll the map.
// You're supposed to add this over the whole screen.
//
// FIXME: make it a widget, separate from GameScreen.
// FIXME: uses Window.IsKeyDown, should track key events.
public class MapScroller
{
    public GameScreen Screen;

    // The view to scroll.
    public OrthoMapView View {
        get {
            return Screen.View;
        }
    }

    // Keyboard scroll speed, in U-Coords / second.
    public double KeyScrollSpeed;

    // Mouse scroll speed, in U-Coords / second.
    public double MouseScrollSpeed;

    // Mouse scroll margin. If the cursor is that many pixels far from the edge,
    // it scrolls.
    public int MouseScrollMargin = 8;

    // FIXME: horrible horrible hack.
    public PointD MouseLoc {
        get {
            return Screen.MouseLoc;
        }
    }

    // Constructor, ties to GameScreen.
    public MapScroller(GameScreen screen)
    {
        this.Screen = screen;
    }

    // Refresh, gets called every frame.
    public void Refresh()
    {
        // Get the parrent window.
        Window window = Screen.ParentWindow;
        if (window == null) {
            return;
        }

        // Handle keyboard scrolling.
        double keyScroll = Application.LastFrameLength * KeyScrollSpeed;
        if (window.IsKeyDown(KeyCode.Up)) {
            View.CenterY -= keyScroll;
        }
        if (window.IsKeyDown(KeyCode.Down)) {
            View.CenterY += keyScroll;
        }
        if (window.IsKeyDown(KeyCode.Left)) {
            View.CenterX -= keyScroll;
        }
        if (window.IsKeyDown(KeyCode.Right)) {
            View.CenterX += keyScroll;
        }

        // Handler mouse scrolling.
        double mouseScroll = Application.LastFrameLength * MouseScrollSpeed;
        if (Screen.MouseFocus) {
            if (MouseLoc.X < MouseScrollMargin && MouseLoc.Y > Screen.Height - MouseScrollMargin) {
                View.CenterX -= mouseScroll;
                View.CenterY += mouseScroll;
            } else if (MouseLoc.X > Screen.Width - MouseScrollMargin && MouseLoc.Y > Screen.Height - MouseScrollMargin) {
                View.CenterX += mouseScroll;
                View.CenterY += mouseScroll;
            } else if (MouseLoc.X > Screen.Width - MouseScrollMargin && MouseLoc.Y < MouseScrollMargin) {
                View.CenterX += mouseScroll;
                View.CenterY -= mouseScroll;
            } else if (MouseLoc.X < MouseScrollMargin && MouseLoc.Y < MouseScrollMargin) {
                View.CenterX -= mouseScroll;
                View.CenterY -= mouseScroll;
            } else if (MouseLoc.X < MouseScrollMargin) {
                View.CenterX -= mouseScroll;
            } else if (MouseLoc.X > Screen.Width - MouseScrollMargin) {
                View.CenterX += mouseScroll;
            } else if (MouseLoc.Y < MouseScrollMargin) {
                View.CenterY -= mouseScroll;
            } else if (MouseLoc.Y > Screen.Height - MouseScrollMargin) {
                View.CenterY += mouseScroll;
            }
        }

        // Setup cursor.
        if (!Screen.MouseFocus) {
            return;
        } else if (MouseLoc.X < MouseScrollMargin && MouseLoc.Y > Screen.Height - MouseScrollMargin) {
            window.FrameCursor = (Cursor)Global.Resources.Get("cursor-arrow-down-left");
        } else if (MouseLoc.X > Screen.Width - MouseScrollMargin && MouseLoc.Y > Screen.Height - MouseScrollMargin) {
            window.FrameCursor = (Cursor)Global.Resources.Get("cursor-arrow-down-right");
        } else if (MouseLoc.X > Screen.Width - MouseScrollMargin && MouseLoc.Y < MouseScrollMargin) {
            window.FrameCursor = (Cursor)Global.Resources.Get("cursor-arrow-up-right");
        } else if (MouseLoc.X < MouseScrollMargin && MouseLoc.Y < MouseScrollMargin) {
            window.FrameCursor = (Cursor)Global.Resources.Get("cursor-arrow-up-left");
        } else if (MouseLoc.X < MouseScrollMargin) {
            window.FrameCursor = (Cursor)Global.Resources.Get("cursor-arrow-left");
        } else if (MouseLoc.X > Screen.Width - MouseScrollMargin) {
            window.FrameCursor = (Cursor)Global.Resources.Get("cursor-arrow-right");
        } else if (MouseLoc.Y < MouseScrollMargin) {
            window.FrameCursor = (Cursor)Global.Resources.Get("cursor-arrow-up");
        } else if (MouseLoc.Y > Screen.Height - MouseScrollMargin) {
            window.FrameCursor = (Cursor)Global.Resources.Get("cursor-arrow-down");
        } else {
            window.FrameCursor = (Cursor)Global.Resources.Get("cursor-arrow");
        }
    }
}

