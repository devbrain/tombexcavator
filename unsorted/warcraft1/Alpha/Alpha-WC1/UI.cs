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

using Alpha.UI;
using Alpha.Util;
using Alpha.Graphics;

// Warcraft UI style.
public class WC1Style: IEditBoxStyle
{
    // UI font.
    private IFont font;
    public IFont Font {
        get {
            return font;
        }
    }

    // Builds a button style, by loading textures prefixed with component.
    private ImageButtonStyle GetButtonStyle(string component)
    {
        ImageButtonStyle style;

        style = new ImageButtonStyle();
        style.NormalTexture = (Image)Global.Resources.Get(component + "-normal");
        style.PressedTexture = (Image)Global.Resources.Get(component + "-pressed");

        return style;
    }

    // Get a race-specific resource
    public object GetRaceResource(string resource)
    {
        ILazyLink res = Global.Resources.GetLink(resource);
        return Global.Resources.Remap(res, Race).Get();
    }

    // Get a race-specific texture
    public Texture GetRaceTexture(string tex)
    {
        return (Image)GetRaceResource(tex);
    }

    // Create a stock big button. (192x15, huge).
    public IButton NewBigButton(string text)
    {
        return new Button(GetButtonStyle("ui-big-button"), Font, text);
    }

    // Create a stock medium button, (112x15) like in the main menu.
    public IButton NewMediumButton(string text)
    {
        return new Button(GetButtonStyle("ui-medium-button"), Font, text);
    }

    // Create a stock small button. (52x15)
    public IButton NewSmallButton(string text)
    {
        return new Button(GetButtonStyle("ui-small-button"), Font, text);
    }

    // Create a stock left arrow button.
    public IButton NewLeftArrow()
    {
        return new Button(GetButtonStyle("ui-left-arrow"));
    }

    // Create a stock right arrow button.
    public IButton NewRightArrow()
    {
        return new Button(GetButtonStyle("ui-right-arrow"));
    }

    // Create a menu button, used to show the main menu in the game screen.
    public IButton NewMenuButton()
    {
        return new Button(GetButtonStyle("ui-menu-button"));
    }

    private Race race;
    public Race Race {
        get {
            return race;
        }
    }

    // Blind edit box.
    public void DrawEditBox(RectangleD rect, WidgetDrawFlags flags)
    {
    }

    // Doesn't take any space.
    public RectangleD GetEditBoxInnerBounds(RectangleD rect, WidgetDrawFlags flags)
    {
        return new RectangleD(0, 0, rect.Width, rect.Height);
    }

    // Draw a carret.
    // This draws a tiny yellow rect.
    public void DrawCarret(IFont f, double x, double y)
    {
        double h = font.GetHeight();

        GL.Color(Color.Yellow);
        GL.Disable(Feature.Texture);
        GL.Begin(DrawMode.Quads);
        GL.Vertex(x, y);
        GL.Vertex(x, y + h);
        GL.Vertex(x + h / 4, y + h);
        GL.Vertex(x + h / 4, y);
        GL.End();
    }

    // Create the style
    public WC1Style(IFont font, Race race)
    {
        this.font = font;
        this.race = race;
    }
}
