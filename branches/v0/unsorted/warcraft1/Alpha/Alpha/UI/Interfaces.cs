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
using System.Collections;
using System.Drawing;
using Alpha.Util;

using Alpha.Graphics;

namespace Alpha.UI
{
    // Interface for an event receiver.
    public interface IEventReceiver
    {
        // Called when the widget gains keyboard focus.
        void OnGainKeyFocus();

        // When a key is pressed.
        void OnKeyDown(KeyCode k);

        // When a character is typed.
        void OnCharType(char c);

        // When a key is released.
        void OnKeyUp(KeyCode k);

        // Called when the widget loses keyboard focus.
        void OnLoseKeyFocus();

        // When the widget gains mouse focus.
        void OnGainMouseFocus();

        // Handle mouse events.
        void OnMouseEvent(MouseEventArgs e);

        // When the widget loses mouse focus.
        void OnLoseMouseFocus();
    }

    // Interface for a widget.
    //
    // A widget is a rectangular piece of the screen.
    // It can handle key and mouse events.
    // It has a size and might ooptionally give information
    // about it's desired size.
    //
    // It can contain other widgets. Some containers can control
    // the size of the children (eventually by asking the widgets
    // about their desired sizes).
    public interface IWidget: IEventReceiver
    {
        // If the Widget is hidden from view.
        bool Hidden { get; set; }

        // If the Widget is disabled. Disabled widgets should stop
        // handling events and may change their appearance.
        bool Disabled { get; set; }

        // This is the parent of the widget.
        // When this is changed the widget should remove
        // itself from the current container and add itself
        // in the other. Since IContainer must deal with useless
        // additions/removals, there is no danger of going into
        // an infinite recursion.
        IWidget Parent { get; set; }

        // Get or set the ZOrder.
        // This is the drawing and mouse focus order.
        int ZOrder { get; set; }

        // X Position in the container.
        double PosX { get; set; }

        // Y Position in the container.
        double PosY { get; set; }

        // Width of the widget.
        double Width { get; set; }
        
        // Height of the widget.
        double Height { get; set; }

        // The bounds of the widget.
        RectangleD Bounds { get; set; }

        // Set the dimenstions of the widget
        void SetSize(double x, double y, double w, double h);

        // Draw the Widget.
        void Draw(RectangleD ClipRect, RectangleD WindowClipRect);

        // Get the desired Width of this widget.
        // This is orientative, the widget should be able to cope
        // with any size information.
        double GetDesiredWidth();

        // Get the desired Height of this widget.
        double GetDesiredHeight();

        // Add a child widget.
        // Nothing happens if already a child
        void Add(IWidget widget);

        // Remove child widget.
        // Nothing happens in not a child.
        void Remove(IWidget widget);

        // Add on top of other widgets.
        // Will change ZOrder to the max ZOrder + 100
        void AddOnTop(IWidget widget);

        // Request a recalculation of children sizes. Only containers that resize
        // their children need to respond to that.
        //
        // An IWidget should ONLY request a recalculation from its parent when
        // DesiredWidth/DesiredHeight changes. This is because parents only use that
        // information to calculate the sizes of children, other recalculations are
        // just a waste of time.
        void RecalculateChildSizes();

        // The widget with mouse focus, or null.
        IWidget MouseFocusWidget { get; }
    }

    // Interface for a decorator, something that contains
    // a widget a does something special with it.
    // (It might contain other widgets too).
    public interface IDecorator: IWidget
    {
        IWidget Child { get; set; }
    }

    // Simple interface for a button.
    public interface IButton: IDecorator
    {
        event EventHandler Click;
    }
    
    // Single line editable text entry.
    public interface IEditBox: IWidget
    {
        // Text in the EditBox
        string Text { get; set; }

        // If text is editable.
        // Otherwise it's just a fancy label.
        bool Editable { get; set; }

        // Text alignment inside the editbox.
        HorizontalTextAlignment Alignment { get; set; }

        // Fired when Text changes.
        event EventHandler TextChanged;

        // Fired when the enter key was pressed.
        event EventHandler ReturnPressed;

        // Fired when editing is done.
        // You should now validate and change the value on whatever
        // model you have underneath.
        event EventHandler EditDone;
    }
    
    // Interface for a scroll bar.
    public interface IScrollbar: IWidget
    {
        // This is the current value of this scrollbar.
        // It can range from MinValue to MaxValue (use SetRange to change those).
        //
        // Setting this to a value outside for the MinValue, MaxValue range
        // will result in the value getting silently clamped.
        double Value { get; set; }

        // Minimum value (inclusive) for the scrollbar.
        // Can be changed using SetRange
        double MinValue { get; }
        
        // Maximum value (inclusive) for the scrollbar.
        // Can be changed using SetRange
        double MaxValue { get; }

        // Sets MinValue and MaxValue.
        // Swaps them if min > max.
        void SetRange(double min, double max);
        
        // If this is non-zero then it's the amount of the scrolling
        // interval a certain position covers.
        //
        // Instead of a Value ranging from MinValue to MaxValue,
        // you have an interval from Value to Value + Cover
        // that can be moved between MinValue and MaxValue + Cover
        //
        // This is a "hint" that only affects the drawing (obviousely).
        //
        // Positive value.
        double Cover { get; set; }

        // The amount to jump when one of the buttons is clicked.
        // Positive value.
        double ButtonJumpAmount { get; set; }
    }

    // Simple checkbox.
    // Can be checked/unchecked.
    // Fires an event when that changes.
    public interface ICheckBox: IWidget
    {
        // If the checkbox is checked or not.
        bool IsChecked { get; set; }

        // Event fired when IsChecked changes.
        event EventHandler Toggle;
    }

    // Interface for a list box, an IWidget that can draw a bunch
    // of strings. The actual strings are not mainainted by the
    // list box, instead the list box uses an IListModel.
    //
    // This will eventually be expanded to a full CellRenderer.
    //
    // IListModel is the model, IListView is the view, and you get
    // to write the controller (or whatever).
    //
    // Hint on using list boxes: it's best to disconnet the model
    // when doing a lot of changes, or you will get a ton of
    // resizing. However, resizing is not that expensive right now.
    public interface IListBox: IWidget
    {
        // The model behind it.
        IListModel Model { get; set; }
    }

    // EventArgs for an item select event(list boxes, combo boxes etc)
    public class ItemSelectEventArgs: EventArgs
    {
        // The index of the selected item.
        public readonly int Index;

        public ItemSelectEventArgs(int index)
        {
            Index = index;
        }
    }

    // Event handler for a Item Select Event(list boxes and combo boxes).
    public delegate void ItemSelectEventHandler(object sender, ItemSelectEventArgs args);

    // This is a model of a list used by the IListBox.
    // It has events to mark change.
    public interface IListModel
    {
        // Number of lines.
        int Count { get; }

        // Get one element
        string GetItem(int index);

        // When something about the model changes.
        // NOTE: This includes element value changes
        // TODO: Split into many smaller events.
        event EventHandler Changed;
    }

    // Used by TextLayout to draw the carret, when visible.
    // Sort of a hack.
    //
    // Should figure out a better way to handle carret drawing.
    public interface ICarretDrawer
    {
        // Draw a tiny carret at x, y, in a text with Font f
        void DrawCarret(IFont font, double x, double y);
    }

    // From here starts style-related stuff, that will be moved eventually.
    
    // Types of borders for a Frame widget
    public enum FrameType
    {
        // Gives a 3D raised appearance.
        Raised,

        // Gives a 3D sunken appearance.
        Sunken,

        // Just give it an outline that's not raised/sunken
        Outline,

        // Sunken frame that contains actual content, not widgets.
        // Suitable for text boxes, list boxes, etc.
        Content,
    }

    // Style information for widgets that deal with text.
    public interface ITextStyle: ICarretDrawer
    {
        // The default font used by widget with this style.
        IFont Font { get; }
    }

    // Button style information.
    public interface IButtonStyle
    {
        // Draw a simple button widget.
        void DrawButton(RectangleD rect, WidgetDrawFlags flags);
            
        // Get the inner bounds of a button.
        RectangleD GetButtonInnerBounds(RectangleD rect, WidgetDrawFlags flags);
    }

    // Edit box style information.
    // FIXME: is this really necessary?
    public interface IEditBoxStyle: ITextStyle
    {
        // Draw a simple editbox widget.
        void DrawEditBox(RectangleD rect, WidgetDrawFlags flags);

        // Get the inner bounds of an editbox.
        RectangleD GetEditBoxInnerBounds(RectangleD rect, WidgetDrawFlags flags);
    }

    // Frame style information.
    public interface IFrameStyle
    {
        // Draw a rectangular frame.
        void DrawFrame(RectangleD rect, WidgetDrawFlags flags, FrameType ft);

        // Get the inner bounds of a certain frame.
        RectangleD GetFrameInnerBounds(RectangleD rect, WidgetDrawFlags flags, FrameType ft);
    }

    // Scrollbar style information.
    public interface IScrollbarStyle
    {
        // How wide is a scrollbar.
        // This is actually the height for a horizontal scrollbar.
        double ScrollbarLateralSize { get; }

        // Minimum size for scrollbar buttons.
        double ScrollbarButtonSize { get; }

        // Minimum size for scrollbar knobs.
        double ScrollbarMinKnobSize { get; }

        // Draw a scrollbar button, one of those little things with arrows on them.
        void DrawScrollbarButton(RectangleD rect, WidgetDrawFlags flags, bool xaxis, bool increase);

        // Draw a scrollbar knob.
        // The knob is the big thing that moves around between the buttons.
        void DrawScrollbarKnob(RectangleD rect, WidgetDrawFlags flags, bool xaxis);
    }

    // Style information for a checkbox
    public interface ICheckBoxStyle
    {
        // Draw a checkbox.
        void DrawCheckBox(RectangleD rect, WidgetDrawFlags flags, bool check);

        // Get the desired width of a checkbox.
        double GetCheckBoxWidth(WidgetDrawFlags flags, bool check);

        // Get the desired height of a checkbox.
        double GetCheckBoxHeight(WidgetDrawFlags flags, bool check);
    }

    // The big style interface
    public interface IStyle:
            ITextStyle,
            IButtonStyle,
            IEditBoxStyle,
            IFrameStyle,
            IScrollbarStyle,
            ICheckBoxStyle
    {
        // Color to draw selections in.
        Color SelectionColor { get; }
    }
}
