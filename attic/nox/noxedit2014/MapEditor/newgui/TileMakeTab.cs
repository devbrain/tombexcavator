/*
 * MapEditor
 * Пользователь: AngryKirC
 * Дата: 15.01.2015
 */
using System;
using System.Drawing;
using System.Windows.Forms;
using System.Collections.Generic;
using MapEditor.MapInt;
using MapEditor.videobag;
using NoxShared;

namespace MapEditor.newgui
{
    /// <summary>
    /// Tile creation GUI
    /// </summary>
    public partial class TileMakeTab : UserControl
    {
        public bool TabLoaded = false;
        private List<string> sortedTileNames;
        private MapView mapView;
        private VideoBagCachedProvider videoBag = null;
        private int tileVariation;
        private int tileTypeID;
        public bool AutoVari
        {
            get
            {
                return checkAutoVari.Checked;
            }
        }

        public TileMakeTab()
        {
            //
            // The InitializeComponent() call is required for Windows Forms designer support.
            //
            InitializeComponent();
            // setup listview handlers
            listTileImages.RetrieveVirtualItem += new RetrieveVirtualItemEventHandler(listTileImages_RetrieveVirtualItem);
        	// setup modes
			buttonMode.SetStates(new EditMode[] { EditMode.FLOOR_PLACE, EditMode.FLOOR_BRUSH });
        }

        public void SetMapView(MapView view)
        {
            mapView = view;
            // provides access to images
            videoBag = mapView.MapRenderer.VideoBag;
            // sort tile names and add them into combobox
            sortedTileNames = new List<string>(ThingDb.FloorTileNames.ToArray());
            sortedTileNames.Sort();
            comboTileType.Items.AddRange(sortedTileNames.ToArray());
            comboIgnoreTile.Items.Add("NONE");
            comboIgnoreTile.Items.AddRange(sortedTileNames.ToArray());
            comboTileType.SelectedIndex = 0;
            comboIgnoreTile.SelectedIndex = 0;
        }

        void listTileImages_RetrieveVirtualItem(object sender, RetrieveVirtualItemEventArgs e)
        {
            ListViewItem item = new ListViewItem("", e.ItemIndex);
            item.BackColor = Color.White;
            e.Item = item;
        }

        private int GetSelTileTypeIndex()
        {
            int selectedIndex = comboTileType.SelectedIndex;
            string tileName = comboTileType.Items[selectedIndex].ToString();
            int index = ThingDb.FloorTileNames.IndexOf(tileName);

            if (index > 0) return index;
            return 0;
        }

        private List<uint> GetVariationsForType(int ttype)
        {
            return ThingDb.FloorTiles[ttype].Variations;
        }

        void UpdateListView(object sender, EventArgs e)
        {
            // принуждаем обновить данные
            tileVariation = 0;
            tileTypeID = GetSelTileTypeIndex();
            listTileImages.VirtualListSize = 0;
            listTileImages.VirtualListSize = GetVariationsForType(tileTypeID).Count;
            // если не создан
            if (listTileImages.LargeImageList == null)
                listTileImages.LargeImageList = new ImageList();
            // обновляем ImageList
            ImageList imglist = listTileImages.LargeImageList;
            imglist.Images.Clear();
            imglist.ImageSize = new Size(46, 46);
            List<uint> variations = GetVariationsForType(tileTypeID);
            // грузим только первые 90 картинок
            int varns = variations.Count;
            if (varns > 90) varns = 90;
            for (int varn = 0; varn < varns; varn++)
            {
                imglist.Images.Add(videoBag.GetBitmap((int)variations[varn]));
            }
        }

        public Map.Tile GetTile(Point loc, bool fake = false)
        {
            // проверяем координаты
            if (loc.X < 0 || loc.Y < 0 || loc.X > 252 || loc.Y > 252) return null;

            ushort vari = (ushort)tileVariation;
            if (checkAutoVari.Checked)
            {
                int x = loc.X;
                int y = loc.Y;
                int cols = ThingDb.FloorTiles[tileTypeID].numCols;
                int rows = ThingDb.FloorTiles[tileTypeID].numRows;

                vari = (ushort)(((x + y) / 2 % cols) + (((y % rows) + 1 + cols - ((x + y) / 2) % cols) % rows) * cols);
            }

            return new Map.Tile(loc, (byte)tileTypeID, vari);
        }

        void ChangeTileType(object sender, EventArgs e)
        {
            if (listTileImages.SelectedIndices.Count > 0)
            {
                tileVariation = listTileImages.SelectedIndices[0];
                // update mapinterface
                //mapView.GetMapInt().FloorSetData((byte)tileTypeID, tileVariation);
            }
        }
    }
}
