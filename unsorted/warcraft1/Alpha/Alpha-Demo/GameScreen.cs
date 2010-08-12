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

using Alpha.UI;
using Alpha.Game;
using Alpha.GameUI;

// Creates a new random game.
public class RandomGameStarter: IGameStarter
{
    // Starting GameState.SyncRandSeed.
    public readonly uint RandomSeed;

    public RandomGameStarter(uint rand)
    {
        RandomSeed = rand;
    }

    public RandomGameStarter()
    {
        RandomSeed = (uint)new System.Random().Next();
    }
 
    public GameState CreateGame()
    {
        GameState state;
        HeightMap map;

        state = Global.GameData.CreateGameState();
        state.SyncSeed = RandomSeed;
        map = Global.GameData.Tileset.CreateMap(state, 20, 20) as HeightMap;
        map.RandomizeHeights(20, 70);
        // Create a player with id 0.
        state.CreatePlayer(0);

        return state;
    }
}

public class GameScreen: StyledWidget
{
    // The actual game.
    public Game Game;

    // Used to record the game.
    private ReplayRecorder Recorder;

    // The Scene view.
    public OrthoMapView View;

    // Selection drawer.
    public SelectionGraphic SelectionGraphic;

    // Button to create a new unit
    public Button NewUnitButton;

    // Button to quit to the main menu
    public Button QuitGameButton;

    // Button to replay the Game.
    public Button ReplayGameButton;

    // Button to restart the Game.
    public Button RestartGameButton;

    // Information label.
    public TextLayout InfoLabel;

    private IList selectedUnits;

    // The list of selected entities.
    public IList SelectedUnits {
        get {
            return selectedUnits;
        }
        set {
            selectedUnits = value;
            SelectionGraphic.SelectedEntities = value;
        }
    }

    // If there is only one selected one, this is the one.
    public Unit SelectedUnit {
        get {
            if (SelectedUnits.Count != 1) {
                throw new InvalidOperationException(
                        "SelectedUnit is only valid if there is exactly one unit selected");
            }
            return SelectedUnits[0] as Unit;
        }
    }

    public GameScreen(IStyle s): base(s)
    {
        // Fixme: game should be set by the outside.
        NewGame();

        // Create a new entity Button.
        NewUnitButton = new Button(s, "Spawn new unit");
        NewUnitButton.SetSize(0.05, 0.05, 0.35, 0.05);
        NewUnitButton.Click += new EventHandler(NewUnitButton_Click);
        AddOnTop(NewUnitButton);

        // Create a quit to menu Button.
        QuitGameButton = new Button(s, "Quit to menu");
        QuitGameButton.SetSize(0.05, 0.15, 0.35, 0.05);
        QuitGameButton.Click += new EventHandler(QuitGameButton_Click);
        AddOnTop(QuitGameButton);

        // Create a restart game button.
        RestartGameButton = new Button(s, "Restart Game");
        RestartGameButton.SetSize(0.05, 0.25, 0.35, 0.05);
        RestartGameButton.Click += new EventHandler(RestartGameButton_Click);
        AddOnTop(RestartGameButton);

        // Create a replay game button.
        ReplayGameButton = new Button(s, "Replay Game");
        ReplayGameButton.SetSize(0.05, 0.35, 0.35, 0.05);
        ReplayGameButton.Click += new EventHandler(ReplayGameButton_Click);
        AddOnTop(ReplayGameButton);

        // Create a small info label.
        InfoLabel = new TextLayout(s.Font);
        InfoLabel.SetSize(0.45, 0.95, Global.Window.Width * Global.Window.XScale - 0.5, 0.05);
        AddOnTop(InfoLabel);

        // Create a selection graphic.
        SelectionGraphic = SelectionGraphic.CreateDefault();

        // Create a MapView.
        View = new OrthoMapView();
        View.MapSceneBuilder = Global.Graphics.CreateSceneBuilder(Game.State.Map, null);
        View.SetSize(0.45, 0.05, Global.Window.Width * Global.Window.XScale - 0.5, 0.9);
        View.CenterX = View.Map.Width / 2;
        View.CenterY = View.Map.Height / 2;
        View.Zoom = 0.0001;
        View.ClampView = true;
        View.MapMouseEvent += new MouseEventHandler(View_MapMouseEvent);
        View.Select += new SelectEventHandler(View_Select);
        AddOnTop(View);

        View.Scene.AddObject(SelectionGraphic);

        Application.NewFrame += new EventHandler(View_NewFrame);
        // We initalize with and empty list, SelectedUnits is never null.
        SelectedUnits = new Unit[0];
    }

    // Disconnect events and the like.
    // After this and removing from the parent, it should get collected.
    private void Exit()
    {
        Application.NewFrame -= new EventHandler(View_NewFrame);
    }

    private void View_NewFrame(object o, EventArgs e)
    {
        if (Game == null) {
            return;
        }
        InfoLabel.Text = String.Format("FPS: {0:f} CPS: {1:f} C: {2}",
                Application.Fps, 1000.0 / Game.CycleLength, Game.State.Cycle);
        Game.Update(Application.ThisClock);
        if (Global.Window.IsKeyDown(KeyCode.Up)) {
            View.CenterY -= 100;
        }
        if (Global.Window.IsKeyDown(KeyCode.Down)) {
            View.CenterY += 100;
        }
        if (Global.Window.IsKeyDown(KeyCode.Left)) {
            View.CenterX -= 100;
        }
        if (Global.Window.IsKeyDown(KeyCode.Right)) {
            View.CenterX += 100;
        }
    }

    private void QuitGameButton_Click(object o, EventArgs e)
    {
        Exit();
        Global.Window.Child = new Menu(Style);
    }

    public void NewGame()
    {
        IGameStarter starter = new RandomGameStarter();
        Game = new SinglePlayerGame(starter.CreateGame());
        Recorder = new ReplayRecorder(starter, Game);
        (Game as SinglePlayerGame).StartGame(Application.ThisClock);
    }

    private void OnNewScene()
    {
        View.MapSceneBuilder = Global.Graphics.CreateSceneBuilder(Game.State.Map, null);
        View.Scene.AddObject(SelectionGraphic);
    }

    private void RestartGameButton_Click(object o, EventArgs e)
    {
        NewGame();
        OnNewScene();
    }

    private void ReplayGameButton_Click(object o, EventArgs e)
    {
        if (Recorder == null) {
            Console.WriteLine("No replay available, sorry");
            return;
        }
        Recorder.Stop();
        Game = new ReplayGame(Recorder.Replay);
        Recorder = new ReplayRecorder(Recorder.Replay.Starter, Game);
        (Game as ReplayGame).StartGame(Application.ThisClock);
        SelectedUnits = new Entity[0];
        OnNewScene();
    }

    private void NewUnitButton_Click(object o, EventArgs e)
    {
        int x, y;

        if (Game is ReplayGame) {
            Console.WriteLine("Replay, can't modify, sorry");
            return;
        }
        UnitType t;
        if (SelectedUnits.Count == 1) {
            x = SelectedUnit.X;
            y = SelectedUnit.Y;
            t = (SelectedUnit as Unit).Type;
        } else {
            x = (int)View.CenterX;
            y = (int)View.CenterY;
            t = (Game.State.SyncRand(2) == 1) ? Global.GameData.BigUnit : Global.GameData.SmallUnit;
        }
        Game.SendCommand(new SpawnEntityCommand(t, Game.State.Map, Game.State.GetPlayer(0), x, y));
    }

    private void View_MapMouseEvent(object o, MouseEventArgs e)
    {
        if (e.EventType == MouseEventType.ButtonUp && e.Button == MouseButton.Right) {
            if (Game is ReplayGame) {
                Console.WriteLine("Replay, can't modify, sorry");
                return;
            }
            TargetInfo target = new TargetInfo(Game.State.Map, (int)e.X, (int)e.Y);
            Game.SendCommand(new OrderCommand(SelectedUnits, new MoveOrder(target), false));
        }
    }

    private void View_Select(object o, SelectEventArgs e)
    {
        SelectedUnits = e.EntityList;
    }

    public override void SetSize(double x, double y, double w, double h)
    {
        View.SetSize(0.45, 0.05, w - 0.5, 0.9);
        base.SetSize(x, y, w, h);
    }
}
