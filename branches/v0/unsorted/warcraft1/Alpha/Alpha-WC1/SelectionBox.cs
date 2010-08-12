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

using Alpha.Util;
using Alpha.Graphics;
using Alpha.UI;
using Alpha.Game;

// Small class representing an unit selection box.
// This is a simple widget which displays information about
// the currently selected units.
//
// FIXME: Don't derive from Widget, it's a composite?
public class SelectionBox: Widget
{
    // Images.
    private Texture UnitIconsOne;
    private Texture UnitIconsOneMagic;
    private Texture UnitIconsOneBuild;
    private Texture UnitIconsTwo;
    private Texture UnitIconsThree;
    private Texture UnitIconsFour;

    // Entity boxes
    private ImageBox[] UnitBox;

    // Background
    private ImageBox Background;

    // The GameScreen this is attached to.
    public readonly GameScreen Screen;

    // Get the selected entities.
    public IList SelectedEntities {
        get {
            return Screen.SelectedEntities;
        }
    }

    // The race, human or orc.
    // Changing this changes only changes the apperance.
    private WC1Style style;
    public WC1Style Style {
        get {
            return style;
        }
        set {
            style = value;

            ILazyLink link = Global.Resources.GetLink("ui-unit-icon-panel");
            Sprite spr = (Sprite)Screen.RemapResource(link).Get();
            UnitIconsOne = spr[0];
            UnitIconsOneMagic = spr[1];
            UnitIconsOneBuild = spr[2];
            UnitIconsTwo = spr[3];
            UnitIconsThree = spr[4];
            UnitIconsFour = spr[5];
        }
    }

    // Create a new SelectionBox, and attach to GameScreen.
    public SelectionBox(WC1Style style, GameScreen screen)
    {
        this.Screen = screen;

        // Create Background.
        Background = new ImageBox();
        Background.SetSize(0, 0, 66, 46);
        this.AddOnTop(Background);

        // Create unit boxes.
        UnitBox = new ImageBox[4];

        UnitBox[0] = new ImageBox();
        UnitBox[0].SetSize(2, 1, 27, 19);
        this.AddOnTop(UnitBox[0]);

        UnitBox[1] = new ImageBox();
        UnitBox[1].SetSize(36, 1, 27, 19);
        this.AddOnTop(UnitBox[1]);

        UnitBox[2] = new ImageBox();
        UnitBox[2].SetSize(2, 23, 27, 19);
        this.AddOnTop(UnitBox[2]);

        UnitBox[3] = new ImageBox();
        UnitBox[3].SetSize(36, 23, 27, 19);
        this.AddOnTop(UnitBox[3]);

        Style = style;
    }

    // Update background and unit boxes.
    // Called every frame, the easy way.
    protected override void Refresh()
    {
        int i;

        base.Refresh();

        // Set the background.
        if (SelectedEntities.Count == 0) {
            Background.Texture = null;
        } else if (SelectedEntities.Count == 1) {
            Unit unit = (Unit)SelectedEntities[0];
            // Spell casters.
            if (unit.Type.SpellCaster || unit.TimeToLive > 0) {
                Background.Texture = UnitIconsOneMagic;
            // Buildings
            } else if (unit.Type.Building && unit.OrderQueue.Action is ActivityAction) {
                Background.Texture = UnitIconsOneBuild;
            // Single units.
            } else {
                Background.Texture = UnitIconsOne;
            }
        } else if (SelectedEntities.Count == 2) {
            Background.Texture = UnitIconsTwo;
        } else if (SelectedEntities.Count == 3) {
            Background.Texture = UnitIconsThree;
        } else if (SelectedEntities.Count > 3) {
            Background.Texture = UnitIconsFour;
        }

        // Move the Portrait if there's only one unit.
        if (SelectedEntities.Count == 1) {
            UnitBox[0].SetSize(4, 4, 27, 19);
        } else {
            UnitBox[0].SetSize(2, 1, 27, 19);
        }

        // Show portraits for the first 4 entities.
        for (i = 0; i < 4; ++i) {
            if (i < SelectedEntities.Count) {
                ILazyLink link = ((Unit)SelectedEntities[i]).Type.Icon;
                UnitBox[i].Texture = (Image)Screen.RemapResource(link).Get();
            } else {
                UnitBox[i].Texture = null;
            }
        }
    }

    // Draw the widget.
    // FIXME: use widgets for health bars and stuff.
    public override void Draw(RectangleD ClipRect, RectangleD WinClipRect)
    {
        double amount;
        base.Draw(ClipRect, WinClipRect);
        if (SelectedEntities.Count == 1) {
            Unit unit = SelectedEntities[0] as Unit;

            // Hack: health bar.
            amount = (double)27 * unit.HitPoints / unit.Type.MaxHitPoints;
            GL.Disable(Feature.Texture);
            GL.Color(0, 255, 0, 192);
            GL.Begin(DrawMode.Quads);
            if (amount < 0) {
                amount = 0;
            }
            GL.Vertex(35, 20);
            GL.Vertex(35, 23);
            GL.Vertex(35 + amount, 23);
            GL.Vertex(35 + amount, 20);
            GL.End();

            // Spell casters and summoned units.
            if (unit.Type.SpellCaster || unit.TimeToLive > 0) {
                if (unit.Type.SpellCaster) {
                    // spell point bar.
                    amount = (double)27 * unit.SpellPoints / Unit.MaxSpellPoints;
                } else {
                    // Lifetime bar.
                    amount = (double)27 * unit.TimeToLive / Unit.MaxTimeToLive;
                }
                GL.Disable(Feature.Texture);
                GL.Color(0, 0, 255, 192);
                GL.Begin(DrawMode.Quads);
                if (amount < 0) {
                    amount = 0;
                }
                GL.Vertex(35, 9);
                GL.Vertex(35, 12);
                GL.Vertex(35 + amount, 12);
                GL.Vertex(35 + amount, 9);
                GL.End();
            }

            // Training facilities.
            if (unit.OrderQueue.Action is ActivityAction) {
                ActivityAction act = (ActivityAction)unit.OrderQueue.Action;
                amount = (double)64 * act.Progress / act.Duration;
                GL.Disable(Feature.Texture);
                GL.Color(255, 0, 0, 128);
                GL.Begin(DrawMode.Quads);
                if (amount < 0) {
                    amount = 0;
                }
                GL.Vertex(1, 36);
                GL.Vertex(1, 43);
                GL.Vertex(1 + amount, 43);
                GL.Vertex(1 + amount, 36);
                GL.End();
            }
        }
    }
}


