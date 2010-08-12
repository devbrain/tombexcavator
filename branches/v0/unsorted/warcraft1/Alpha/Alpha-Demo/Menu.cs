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

using Alpha.UI;

public class Menu: StyledWidget
{
    // Button to enter the UITest.
    public Button UITestButton;
     
    // Button to start a new game.
    public Button NewGameButton;
    
    // Button to quit the game.
    public Button QuitGameButton;

    // Create a menu widget.
    public Menu(IStyle s): base(s)
    {
        // UI Test Button.
        UITestButton = new Button(s, "UI Test");
        UITestButton.SetSize(0.05, 0.35, 0.35, 0.05);
        UITestButton.Click += new EventHandler(UITestButton_Click);
        AddOnTop(UITestButton);

        // New Game Button.
        NewGameButton = new Button(s, "New Game");
        NewGameButton.SetSize(0.05, 0.45, 0.35, 0.05);
        NewGameButton.Click += new EventHandler(NewGameButton_Click);
        AddOnTop(NewGameButton);

        // Quit Game Button
        QuitGameButton = new Button(s, "Quit Game");
        QuitGameButton.SetSize(0.05, 0.65, 0.35, 0.05);
        QuitGameButton.Click += new EventHandler(QuitGameButton_Click);
        AddOnTop(QuitGameButton);
    }

    public override void SetSize(double x, double y, double w, double h)
    {
        base.SetSize(x, y, w, h);
    }

    // Open the UI test.
    private void UITestButton_Click(object sender, EventArgs e)
    {
        Global.Window.Child = new UITestGround(Global.Style);
    }
  
    // Start a new Game.
    private void NewGameButton_Click(object sender, EventArgs e)
    {
        Global.Window.Child = new GameScreen(Global.Style);
    }
 
    // Quit the game.
    private void QuitGameButton_Click(object sender, EventArgs e)
    {
        Application.Quit();
    }
}

