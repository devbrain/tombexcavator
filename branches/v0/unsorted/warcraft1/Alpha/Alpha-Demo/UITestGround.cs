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
using Alpha.Graphics;

public class UITestGround: StyledWidget
{
    public TextLayout TextBox;

    public TextLayout ListBoxLabel;

    public EditBox EditBox;

    public ListBox ListBox;

    public ListModel ListModel;

    // Enter the menu.
    public UITestGround(Style s): base(s)
    {
        int i;
        TextLayout label;
        Frame frame;

        // UI Test Button.
        Button ExitTestButton = new Button(s, "Exit");
        ExitTestButton.SetSize(0.05, 0.55, 0.35, 0.05);
        ExitTestButton.Click += new EventHandler(ExitTestButton_Click);
        AddOnTop(ExitTestButton);

        label = new TextLayout(s.Font);
        label.SetSize(0.15, 0.85, 0.6, 0.05);
        label.Text = "User Interface Test Ground";
        label.Font = new Alpha.Graphics.SimpleFont(Global.Style.FontData, 0.02);
        label.HorizontalAlignment = HorizontalTextAlignment.Left;
        AddOnTop(label);

        string text = "";
        text += "First Line\n";
        text += "Second Line\n";
        text += "Third Line is really really really very incredibly big and long and huge\n";
        text += "\n";
        text += "Fourth Line empty\n";
        for (i = 1; i <= 30; ++i) {
            text += String.Format("Beeblebrox {0}\n", i);
        }
        frame = new Frame(s);
        // FIXME: hack
        AddOnTop(frame);
        frame.Child = new Scroller(s);
        (frame.Child as Scroller).Child = TextBox = new TextLayout(s.Font, text);
        TextBox.VerticalAlignment = VerticalTextAlignment.Top;
        frame.SetSize(0.5, 0.1, 0.4, 0.4);

        frame = new Frame(s);
        AddOnTop(frame);
        frame.SetSize(1.0, 0.1, 0.25, 0.4);
        frame.Child = new Scroller(s);
        (frame.Child as Scroller).Child = ListBox = new ListBox(s);
        ListBox.Model = ListModel = new ListModel();
        ListModel.Add("Hello");
        ListModel.Add("World");
        ListModel.Add("Eat");
        ListModel.Add("Me");
        ListModel.Add("Who");
        ListModel.Add("Are");
        ListModel.Add("You");
        ListModel.Add("You");
        ListModel.Add("You");
        ListModel.Add("You");
        ListModel.Add("You");
        ListModel.Add("You");
        ListModel.Add("And");
        ListModel.Add("And");
        ListModel.Add("And");
        ListModel.Add("And");
        ListModel.Add("And");
        ListModel.Add("Me");
        ListModel.Add("Me");
        ListModel.Add("Me");
        ListBox.ItemSelected += new ItemSelectEventHandler(OnItemSelected);

        ListBoxLabel = label = new TextLayout(s.Font);
        label.SetSize(0.9, 0.55, 0.4, 0.04);
        label.Text = "choose item above";
        label.Font = new SimpleFont(Global.Style.FontData, 0.02);
        label.HorizontalAlignment = HorizontalTextAlignment.Center;
        AddOnTop(label);

        label = new TextLayout(s.Font);
        label.SetSize(0.55, 0.70, 0.6, 0.05);
        label.Text = "Red go Left";
        label.Font = new SimpleFont(Global.Style.FontData, 0.02);
        ((SimpleFont)label.Font).Color = Color.Red;
        label.HorizontalAlignment = HorizontalTextAlignment.Left;
        AddOnTop(label);

        label = new TextLayout(s.Font);
        label.SetSize(0.55, 0.75, 0.6, 0.05);
        label.Text = "Green go Center";
        label.Font = new SimpleFont(Global.Style.FontData, 0.02);
        ((SimpleFont)label.Font).Color = Color.Green;
        label.HorizontalAlignment = HorizontalTextAlignment.Center;
        AddOnTop(label);

        label = new TextLayout(s.Font);
        label.SetSize(0.55, 0.80, 0.6, 0.05);
        label.Text = "Blue go Right";
        label.Font = new SimpleFont(Global.Style.FontData, 0.02);
        ((SimpleFont)label.Font).Color = Color.Blue;
        label.HorizontalAlignment = HorizontalTextAlignment.Right;
        AddOnTop(label);

        CheckBox checkbox;

        checkbox = new CheckBox(s);
        checkbox.SetSize(0.5, 0.70, 0.05, 0.05);
        AddOnTop(checkbox);

        checkbox = new CheckBox(s);
        checkbox.SetSize(0.5, 0.75, 0.05, 0.05);
        AddOnTop(checkbox);
 
        checkbox = new CheckBox(s);
        checkbox.SetSize(0.5, 0.80, 0.05, 0.05);
        AddOnTop(checkbox);
 
        EditBox = new EditBox(s);
        EditBox.Text = "Press enter to add text";
        AddOnTop(EditBox);
        // FIXME: hack
        EditBox.Style = Global.Style;
        EditBox.SetSize(0.53, 0.6, 0.6, 0.04);
        EditBox.CharType += new CharEventHandler(EditBox_CharType);
        //EditBox.Font = new Alpha.Font(Demo.Window.Style.FontData, 0.02);
    }

    public void EditBox_CharType(object sender, CharEventArgs args)
    {
        if (args.Char == '\n' || args.Char == '\r') {
            ListModel.Add(EditBox.Text);
            TextBox.Text = TextBox.Text + EditBox.Text + '\n';
            EditBox.Text = "";
        }
    }

    // An item from the listbox was chosen
    private void OnItemSelected(object sender, ItemSelectEventArgs e)
    {
        ListBoxLabel.Text = ListModel[e.Index];
    }
    
    // Exit the UI test.
    private void ExitTestButton_Click(object sender, EventArgs e)
    {
        Global.Window.Child = new Menu(Global.Style);
    }
}

