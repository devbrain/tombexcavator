// Copyright (C) 2006 Crestez Leonard    cleonard@go.ro
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
using Color = System.Drawing.Color;
using System.Collections;

using Alpha.UI;
using Alpha.GameUI;
using Alpha.Util;
using Alpha.Graphics;

// This is a standard WC1 31x24 button with a 27x19 icon
// You can resize it, but I wouldn't.
public class IconButton: Widget
{
    // Icon to display.
    public Texture Icon;

    // Normal button background image.
    public Texture BackgroundNormal;

    // Pressed button background image.
    public Texture BackgroundPressed;

    // Draw the button.
    public override void Draw(RectangleD ClipRect, RectangleD WinClipRect)
    {
        Texture tex;
        if (Pressed) {
            tex = BackgroundPressed;
        } else {
            tex = BackgroundNormal;
        }

        tex.Bind();
        GL.Begin(DrawMode.Quads);
        GL.Color(255, 255, 255);
        GL.TexCoord(0, 0);
        GL.Vertex(0, 0, 0);
        GL.TexCoord(1, 0);
        GL.Vertex(Width, 0, 0);
        GL.TexCoord(1, 1);
        GL.Vertex(Width, Height, 0);
        GL.TexCoord(0, 1);
        GL.Vertex(0, Height, 0);
        GL.End();
        tex.Unbind();

        if (Icon != null) {
            int z = Pressed ? 1 : 0;
            Icon.Bind();
            GL.Begin(DrawMode.Quads);
            GL.Color(255, 255, 255);
            
            GL.TexCoord(0, 0);
            GL.Vertex(2, 2 + z);
            GL.TexCoord(1, 0);
            GL.Vertex(Width - 2, 2 + z);
            GL.TexCoord(1, 1);
            GL.Vertex(Width - 2, Height - 3 + z);
            GL.TexCoord(0, 1);
            GL.Vertex(2, Height - 3 + z);

            GL.End();
            Icon.Unbind();
        }
        base.Draw(ClipRect, WinClipRect);
    }

    // When the user click the widget. A click is defined as a released left mouse button.
    public event EventHandler Click;

    // Override OnMouseEvent to generate click events.
    public override void OnMouseEvent(MouseEventArgs e)
    {
        if (e.EventType == MouseEventType.ButtonUp && e.Button == MouseButton.Left && Pressed) {
            if (Click != null) {
                Click(this, new EventArgs());
            }
        }
        base.OnMouseEvent(e);
    }
}

// Widget for the unit button panel.
// Display and events, no logic.
//
// FIXME: This calls .Execute. Bad?
// FIXME: Move to Alpha somehow?
// FIXME: At least add a reasonable implementation in alpha?
public class ButtonPanel: Widget
{
    // The GameScreen this is a part of.
    public readonly GameScreen Screen;

    // The actual buttons.
    public IconButton[] IconButtons;

    // Shortcut to GameScreen.Buttons
    public IList UnitButtons {
        get {
            return Screen.UnitButtons;
        }
    }

    // Constructor. Requires a GameScreen.
    public ButtonPanel(GameScreen screen)
    {
        this.Screen = screen;
        int i;

        // Create action buttons.
        IconButtons = new IconButton[6];
        for (i = 0; i < 6; ++i) {
            IconButtons[i] = new IconButton();
            IconButtons[i].BackgroundNormal = (Image)Global.Resources.Get("ui-icon-frame-normal");
            IconButtons[i].BackgroundPressed = (Image)Global.Resources.Get("ui-icon-frame-pressed");
            IconButtons[i].Icon = null;
            IconButtons[i].SetSize(33 * (i % 2), 23 * (i / 2), 31, 24);
            IconButtons[i].Click += new EventHandler(IconButton_Click);
            this.AddOnTop(IconButtons[i]);
        }
    }

    // Handle click events.
    // Ugly, but it works.
    private void IconButton_Click(object sender, EventArgs args)
    {
        int pos;
        for (pos = 0; pos < IconButtons.Length; ++pos) {
            if (IconButtons[pos] == sender && IconButtons[pos].Hidden == false) {
                break;
            }
        }
        if (pos == IconButtons.Length) {
            return;
        }
        foreach (UnitButton button in UnitButtons) {
            if (button.PositionHint == pos && button.GetVisible(Screen)) {
                button.Execute(Screen);
                return;
            }
        }
    }

    // Refresh, set visibility, icons, etc.
    // Called every frame.
    protected override void Refresh()
    {
        int i;

        base.Refresh();

        // Hide everything first.
        for (i = 0; i < IconButtons.Length; ++i) {
            IconButtons[i].Hidden = true;
            IconButtons[i].Icon = null;
        }

        // Do the monkey.
        foreach (UnitButton button in UnitButtons) {
            int pos = button.PositionHint;
            if (pos < 0 || pos > IconButtons.Length) {
                throw new IndexOutOfRangeException("PositionHint out of range");
            }
            
            // Ignore invisible items.
            if (!button.GetVisible(Screen)) {
                continue;
            }

            IconButtons[pos].Hidden = false;
            ILazyLink icon = button.Icon;
            if (icon == null) {
                throw new Exception("CommandButtonInfo.IconLink is null");
            }
            IconButtons[pos].Icon = (Image)(Screen.RemapResource(icon).Get());
        }
    }
}
