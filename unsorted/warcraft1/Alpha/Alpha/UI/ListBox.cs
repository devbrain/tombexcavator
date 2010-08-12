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
using System.Collections;

using Alpha.Graphics;
using Alpha.Util;

namespace Alpha.UI
{
    // Trivial IListModel implementation, using an ArrayList.
    // TODO: implement IList
    // Is that worth it before switching to .net 2.0 or a cellrenderer?
    public class ListModel: IListModel
    {
        // Backing list.
        private readonly ArrayList list;

        // Fired when something changes about the list model.
        public event EventHandler Changed;

        // Fire the changed event.
        protected void OnChanged()
        {
            if (Changed != null) {
                Changed(this, new EventArgs());
            }
        }

        // Constructor.
        public ListModel()
        {
            list = new ArrayList();
        }

        // Get the number of items.
        public int Count {
            get {
                return list.Count;
            }
        }

        // Get a certain item.
        public string GetItem(int index)
        {
            return (string)list[index];
        }

        public string this[int index] {
            get {
                return list[index] as string;
            }
            set {
                list[index] = value;
                OnChanged();
            }
        }

        // Add a new item.
        public void Add(string item)
        {
            list.Add(item);
            OnChanged();
        }

        // Remove a certain element.
        public void Remove(int index)
        {
            list.RemoveAt(index);
            OnChanged();
        }
    }

    // A list from which you can choose one entry
    public class ListBox: StyledWidget, IListBox
    {
        // The model shown in this ListBox
        private IListModel model;
        public IListModel Model {
            get {
                return model;
            }
            set {
                if (Model != null) {
                    Model.Changed -= new EventHandler(Model_Changed);
                }
                model = value;
                if (Model != null) {
                    Model.Changed += new EventHandler(Model_Changed);
                    SelectedIndex = SelectedIndex;
                    RecalculateChildSizes();
                }
            }
        }

        // The index of the Highlighted item from the listbox
        // silently clamped to 0, Model.Count
        // Meaningless while the model is null, but kept nevertheless.
        private int selectedIndex;
        public int SelectedIndex {
            get {
                return selectedIndex;
            }
            set {
                if (Model == null) {
                    selectedIndex = value;
                } else {
                    selectedIndex = Math.Max(Math.Min(value, Model.Count), 0);
                }
            }
        }

        // Event for when the model changes.
        private void Model_Changed(object sender, EventArgs args)
        {
            RecalculateChildSizes();
        }
        
        // An item from the list is selected
        public event ItemSelectEventHandler ItemSelected;
          
        // The draw function for the list
        public override void Draw(RectangleD ClipRect, RectangleD WindowClipRect)
        {
            int i;

            PushSelfScissor(ClipRect, WindowClipRect);
            GL.Disable(Feature.Texture);
            GL.Color(Style.SelectionColor);
            /*Style.DrawRect(RectangleD.FromXYWidthHeight(
                    0, selectedIndex * Font.GetHeight(),
                    Width, Font.GetHeight()));*/
            for (i = 0; i < Model.Count; ++i) {
                if (Model.GetItem(i) != null) {
                    FontServices.DrawString(Font, Model.GetItem(i), 0, i * Font.GetHeight());
                }
            }
            GL.PopScissor();

            base.Draw(ClipRect, WindowClipRect);
        }

        public ListBox(IStyle s): base(s)
        {
            selectedIndex = 0;
        }

        public override void OnMouseEvent(MouseEventArgs e)
        {
            int selected;
            if (e.EventType == MouseEventType.ButtonDown && e.Button == MouseButton.Left) {
                selected = (int)(e.Location.Y / Font.GetHeight());
                if (selected < Model.Count) {
                    selectedIndex = selected;
                    OnItemSelected(selectedIndex);
                }
            }
            base.OnMouseEvent(e);
        }
 
        protected virtual void OnItemSelected(int index)
        {
            if (ItemSelected != null) {
                ItemSelected(this, new ItemSelectEventArgs(index));
            }
        }

        // Returns the length of the longest item
        public override double GetDesiredWidth()
        {
            if (Model == null) {
                return 0;
            }
            double res = 0;
            int i;
            for (i = 0; i < Model.Count; ++i) {
                if (Model.GetItem(i) != null) {
                    res = Math.Max(res, FontServices.GetStringWidth(Font, Model.GetItem(i)));
                }
            }
            return res;
        }

        // Ask enough height to display all the rows.
        public override double GetDesiredHeight()
        {
            if (Model == null) {
                return 0;
            }
            return Model.Count * Font.GetHeight();
        }
    }
}

