using System;
using System.Drawing;
using System.Windows.Forms;
using NoxShared;
using TileEdgeDirection = NoxShared.Map.Tile.EdgeTile.Direction;
using Mode = MapEditor.MapInt.EditMode;
using MapEditor.render;
using MapEditor.newgui;
using MapEditor.objgroups;
using MapEditor.MapInt;
using NoxShared.ObjDataXfer;

namespace MapEditor
{
    public class MapView : UserControl
    {
        public bool TileTabLoaded = false;
        bool renderingOk = true;
        int WidthMod = 0;
        int winX = 0;
        int winY = 0;
        StatusbarHelper statusHelper = new StatusbarHelper();
        public const int squareSize = 23;
        public const int objectSelectionRadius = 8;
        protected Button currentButton;
        public MapObjectCollection SelectedObjects { get { return MapInterface.SelectedObjects; } }
        public MapViewRenderer MapRenderer;
        public PolygonEditor PolygonEditDlg = null;
        public Point mouseLocation;
        private Map Map
        {
        	get
        	{
        		return MapInterface.TheMap;
        	}
        }
        
   		/* Controls */
        private MenuItem contextMenuDelete;
        private MenuItem contextMenuProperties;
        private MenuItem menuItem3;
        private StatusBarPanel statusMapItem;
        private StatusBar statusBar;
        private StatusBarPanel statusLocation;
        private StatusBarPanel statusMode;
        private Panel scrollPanel;//WARNING: the form designer is not happy with this
        public MapView.FlickerFreePanel mapPanel;
        private MenuItem contextMenuCopy;
        private MenuItem contextMenuPaste;
        private ContextMenu contextMenu;
        private Timer tmrInvalidate;
        private System.ComponentModel.IContainer components;
        private GroupBox groupAdv;
        private TabControl tabMapTools;
        private TabPage tabObjectWps;
        private GroupBox objectGroup;
        public ComboBox cboObjCreate;
        private PictureBox objectPreview;
        private RadioButton radFullSnap;
        private RadioButton radCenterSnap;
        private RadioButton radNoSnap;
        private GroupBox extentsGroup;
        private RadioButton radioExtentShowColl;
        private RadioButton radioExtentsShowAll;
        private RadioButton radioExtentsHide;

        //16bit windows needs the deprectated version of DoubleBufferring to work. Otherwise it crashes :\
        public class FlickerFreePanel : Panel 
        {
            public FlickerFreePanel() : base() 
            {SetStyle(ControlStyles.OptimizedDoubleBuffer | ControlStyles.AllPaintingInWmPaint | ControlStyles.UserPaint | ControlStyles.Opaque, true);} 
        // set styles to reduce flicker and painting over twice
        }

        #region Component Designer generated code
        /// <summary> 
        /// Required method for Designer support - do not modify 
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
        	this.components = new System.ComponentModel.Container();
        	this.contextMenuDelete = new System.Windows.Forms.MenuItem();
        	this.menuItem3 = new System.Windows.Forms.MenuItem();
        	this.contextMenuProperties = new System.Windows.Forms.MenuItem();
        	this.statusBar = new System.Windows.Forms.StatusBar();
        	this.statusMode = new System.Windows.Forms.StatusBarPanel();
        	this.statusLocation = new System.Windows.Forms.StatusBarPanel();
        	this.statusMapItem = new System.Windows.Forms.StatusBarPanel();
        	this.statusPolygon = new System.Windows.Forms.StatusBarPanel();
        	this.groupAdv = new System.Windows.Forms.GroupBox();
        	this.tabMapTools = new System.Windows.Forms.TabControl();
        	this.tabWalls = new System.Windows.Forms.TabPage();
        	this.WallMakeNewCtrl = new MapEditor.newgui.WallMakeTab();
        	this.tabTiles = new System.Windows.Forms.TabPage();
        	this.TileMakeNewCtrl = new MapEditor.newgui.TileMakeTab();
        	this.tabEdges = new System.Windows.Forms.TabPage();
        	this.EdgeMakeNewCtrl = new MapEditor.newgui.EdgeMakeTab();
        	this.tabObjectWps = new System.Windows.Forms.TabPage();
        	this.groupGridSnap = new System.Windows.Forms.GroupBox();
        	this.radFullSnap = new System.Windows.Forms.RadioButton();
        	this.radNoSnap = new System.Windows.Forms.RadioButton();
        	this.radCenterSnap = new System.Windows.Forms.RadioButton();
        	this.waypointGroup = new System.Windows.Forms.GroupBox();
        	this.buttonWaypointMode = new SwitchModeButton();
        	this.waypointName = new System.Windows.Forms.TextBox();
        	this.label1 = new System.Windows.Forms.Label();
        	this.waypointEnabled = new System.Windows.Forms.CheckBox();
        	this.extentsGroup = new System.Windows.Forms.GroupBox();
        	this.radioExtentsShowAll = new System.Windows.Forms.RadioButton();
        	this.radioExtentsHide = new System.Windows.Forms.RadioButton();
        	this.radioExtentShowColl = new System.Windows.Forms.RadioButton();
        	this.objectGroup = new System.Windows.Forms.GroupBox();
        	this.buttonObjectMode = new SwitchModeButton();
        	this.label6 = new System.Windows.Forms.Label();
        	this.objectCategoriesBox = new System.Windows.Forms.ComboBox();
        	this.cboObjCreate = new System.Windows.Forms.ComboBox();
        	this.objectPreview = new System.Windows.Forms.PictureBox();
        	this.scrollPanel = new System.Windows.Forms.Panel();
        	this.mapPanel = new MapEditor.MapView.FlickerFreePanel();
        	this.contextMenu = new System.Windows.Forms.ContextMenu();
        	this.contextMenuCopy = new System.Windows.Forms.MenuItem();
        	this.contextMenuPaste = new System.Windows.Forms.MenuItem();
        	this.tmrInvalidate = new System.Windows.Forms.Timer(this.components);
        	((System.ComponentModel.ISupportInitialize)(this.statusMode)).BeginInit();
        	((System.ComponentModel.ISupportInitialize)(this.statusLocation)).BeginInit();
        	((System.ComponentModel.ISupportInitialize)(this.statusMapItem)).BeginInit();
        	((System.ComponentModel.ISupportInitialize)(this.statusPolygon)).BeginInit();
        	this.groupAdv.SuspendLayout();
        	this.tabMapTools.SuspendLayout();
        	this.tabWalls.SuspendLayout();
        	this.tabTiles.SuspendLayout();
        	this.tabEdges.SuspendLayout();
        	this.tabObjectWps.SuspendLayout();
        	this.groupGridSnap.SuspendLayout();
        	this.waypointGroup.SuspendLayout();
        	this.extentsGroup.SuspendLayout();
        	this.objectGroup.SuspendLayout();
        	((System.ComponentModel.ISupportInitialize)(this.objectPreview)).BeginInit();
        	this.scrollPanel.SuspendLayout();
        	this.SuspendLayout();
        	// 
        	// contextMenuDelete
        	// 
        	this.contextMenuDelete.Index = 2;
        	this.contextMenuDelete.Text = "Delete";
        	this.contextMenuDelete.Click += new System.EventHandler(this.contextMenuDelete_Click);
        	// 
        	// menuItem3
        	// 
        	this.menuItem3.Index = 3;
        	this.menuItem3.Text = "-";
        	// 
        	// contextMenuProperties
        	// 
        	this.contextMenuProperties.Index = 4;
        	this.contextMenuProperties.Text = "Properties";
        	this.contextMenuProperties.Click += new System.EventHandler(this.contextMenuProperties_Click);
        	// 
        	// statusBar
        	// 
        	this.statusBar.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F);
        	this.statusBar.ImeMode = System.Windows.Forms.ImeMode.NoControl;
        	this.statusBar.Location = new System.Drawing.Point(0, 657);
        	this.statusBar.Name = "statusBar";
        	this.statusBar.Panels.AddRange(new System.Windows.Forms.StatusBarPanel[] {
			this.statusMode,
			this.statusLocation,
			this.statusMapItem,
			this.statusPolygon});
        	this.statusBar.ShowPanels = true;
        	this.statusBar.Size = new System.Drawing.Size(859, 22);
        	this.statusBar.SizingGrip = false;
        	this.statusBar.TabIndex = 1;
        	// 
        	// statusMode
        	// 
        	this.statusMode.AutoSize = System.Windows.Forms.StatusBarPanelAutoSize.Contents;
        	this.statusMode.MinWidth = 0;
        	this.statusMode.Name = "statusMode";
        	this.statusMode.ToolTipText = "Currently active editor mode";
        	this.statusMode.Width = 10;
        	// 
        	// statusLocation
        	// 
        	this.statusLocation.MinWidth = 0;
        	this.statusLocation.Name = "statusLocation";
        	this.statusLocation.ToolTipText = "Location (depends on mode)";
        	// 
        	// statusMapItem
        	// 
        	this.statusMapItem.AutoSize = System.Windows.Forms.StatusBarPanelAutoSize.Contents;
        	this.statusMapItem.MinWidth = 0;
        	this.statusMapItem.Name = "statusMapItem";
        	this.statusMapItem.ToolTipText = "Info about the map element";
        	this.statusMapItem.Width = 10;
        	// 
        	// statusPolygon
        	// 
        	this.statusPolygon.AutoSize = System.Windows.Forms.StatusBarPanelAutoSize.Contents;
        	this.statusPolygon.MinWidth = 0;
        	this.statusPolygon.Name = "statusPolygon";
        	this.statusPolygon.ToolTipText = "Name of the polygon";
        	this.statusPolygon.Width = 10;
        	// 
        	// groupAdv
        	// 
        	this.groupAdv.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
			| System.Windows.Forms.AnchorStyles.Left)));
        	this.groupAdv.Controls.Add(this.tabMapTools);
        	this.groupAdv.Location = new System.Drawing.Point(0, 0);
        	this.groupAdv.Name = "groupAdv";
        	this.groupAdv.Size = new System.Drawing.Size(250, 653);
        	this.groupAdv.TabIndex = 28;
        	this.groupAdv.TabStop = false;
        	// 
        	// tabMapTools
        	// 
        	this.tabMapTools.Controls.Add(this.tabWalls);
        	this.tabMapTools.Controls.Add(this.tabTiles);
        	this.tabMapTools.Controls.Add(this.tabEdges);
        	this.tabMapTools.Controls.Add(this.tabObjectWps);
        	this.tabMapTools.Location = new System.Drawing.Point(7, 19);
        	this.tabMapTools.Name = "tabMapTools";
        	this.tabMapTools.SelectedIndex = 0;
        	this.tabMapTools.Size = new System.Drawing.Size(236, 628);
        	this.tabMapTools.TabIndex = 29;
        	this.tabMapTools.SelectedIndexChanged += new System.EventHandler(this.TabMapToolsSelectedIndexChanged);
        	// 
        	// tabWalls
        	// 
        	this.tabWalls.BackColor = System.Drawing.Color.LightGray;
        	this.tabWalls.Controls.Add(this.WallMakeNewCtrl);
        	this.tabWalls.Location = new System.Drawing.Point(4, 22);
        	this.tabWalls.Name = "tabWalls";
        	this.tabWalls.Padding = new System.Windows.Forms.Padding(3);
        	this.tabWalls.Size = new System.Drawing.Size(228, 602);
        	this.tabWalls.TabIndex = 4;
        	this.tabWalls.Text = "Walls";
        	// 
        	// WallMakeNewCtrl
        	// 
        	this.WallMakeNewCtrl.BackColor = System.Drawing.Color.Transparent;
        	this.WallMakeNewCtrl.Location = new System.Drawing.Point(8, 8);
        	this.WallMakeNewCtrl.Name = "WallMakeNewCtrl";
        	this.WallMakeNewCtrl.SelectedWallFacing = 0;
        	this.WallMakeNewCtrl.Size = new System.Drawing.Size(216, 598);
        	this.WallMakeNewCtrl.TabIndex = 0;
        	// 
        	// tabTiles
        	// 
        	this.tabTiles.BackColor = System.Drawing.Color.LightGray;
        	this.tabTiles.Controls.Add(this.TileMakeNewCtrl);
        	this.tabTiles.Location = new System.Drawing.Point(4, 22);
        	this.tabTiles.Name = "tabTiles";
        	this.tabTiles.Size = new System.Drawing.Size(192, 74);
        	this.tabTiles.TabIndex = 5;
        	this.tabTiles.Text = "Tiles";
        	// 
        	// TileMakeNewCtrl
        	// 
        	this.TileMakeNewCtrl.BackColor = System.Drawing.Color.Transparent;
        	this.TileMakeNewCtrl.Location = new System.Drawing.Point(8, 8);
        	this.TileMakeNewCtrl.Name = "TileMakeNewCtrl";
        	this.TileMakeNewCtrl.Size = new System.Drawing.Size(216, 536);
        	this.TileMakeNewCtrl.TabIndex = 0;
        	this.TileMakeNewCtrl.TabStop = false;
        	// 
        	// tabEdges
        	// 
        	this.tabEdges.BackColor = System.Drawing.Color.LightGray;
        	this.tabEdges.Controls.Add(this.EdgeMakeNewCtrl);
        	this.tabEdges.Location = new System.Drawing.Point(4, 22);
        	this.tabEdges.Name = "tabEdges";
        	this.tabEdges.Size = new System.Drawing.Size(192, 74);
        	this.tabEdges.TabIndex = 6;
        	this.tabEdges.Text = "Edges";
        	// 
        	// EdgeMakeNewCtrl
        	// 
        	this.EdgeMakeNewCtrl.BackColor = System.Drawing.Color.Transparent;
        	this.EdgeMakeNewCtrl.Location = new System.Drawing.Point(8, 8);
        	this.EdgeMakeNewCtrl.Name = "EdgeMakeNewCtrl";
        	this.EdgeMakeNewCtrl.Size = new System.Drawing.Size(216, 536);
        	this.EdgeMakeNewCtrl.TabIndex = 1;
        	this.EdgeMakeNewCtrl.TabStop = false;
        	// 
        	// tabObjectWps
        	// 
        	this.tabObjectWps.BackColor = System.Drawing.Color.LightGray;
        	this.tabObjectWps.Controls.Add(this.groupGridSnap);
        	this.tabObjectWps.Controls.Add(this.waypointGroup);
        	this.tabObjectWps.Controls.Add(this.extentsGroup);
        	this.tabObjectWps.Controls.Add(this.objectGroup);
        	this.tabObjectWps.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(238)));
        	this.tabObjectWps.Location = new System.Drawing.Point(4, 22);
        	this.tabObjectWps.Name = "tabObjectWps";
        	this.tabObjectWps.Size = new System.Drawing.Size(192, 74);
        	this.tabObjectWps.TabIndex = 2;
        	this.tabObjectWps.Text = "Objects/WayPoints";
        	// 
        	// groupGridSnap
        	// 
        	this.groupGridSnap.Controls.Add(this.radFullSnap);
        	this.groupGridSnap.Controls.Add(this.radNoSnap);
        	this.groupGridSnap.Controls.Add(this.radCenterSnap);
        	this.groupGridSnap.Location = new System.Drawing.Point(116, 285);
        	this.groupGridSnap.Name = "groupGridSnap";
        	this.groupGridSnap.Size = new System.Drawing.Size(104, 88);
        	this.groupGridSnap.TabIndex = 32;
        	this.groupGridSnap.TabStop = false;
        	this.groupGridSnap.Text = "Grid alignment";
        	// 
        	// radFullSnap
        	// 
        	this.radFullSnap.AutoSize = true;
        	this.radFullSnap.ImeMode = System.Windows.Forms.ImeMode.NoControl;
        	this.radFullSnap.Location = new System.Drawing.Point(6, 65);
        	this.radFullSnap.Name = "radFullSnap";
        	this.radFullSnap.Size = new System.Drawing.Size(69, 17);
        	this.radFullSnap.TabIndex = 28;
        	this.radFullSnap.Text = "Full Snap";
        	this.radFullSnap.UseVisualStyleBackColor = true;
        	this.radFullSnap.CheckedChanged += new System.EventHandler(this.radFullSnap_CheckedChanged);
        	// 
        	// radNoSnap
        	// 
        	this.radNoSnap.AutoSize = true;
        	this.radNoSnap.Checked = true;
        	this.radNoSnap.Location = new System.Drawing.Point(6, 20);
        	this.radNoSnap.Name = "radNoSnap";
        	this.radNoSnap.Size = new System.Drawing.Size(67, 17);
        	this.radNoSnap.TabIndex = 26;
        	this.radNoSnap.TabStop = true;
        	this.radNoSnap.Text = "No Snap";
        	this.radNoSnap.UseVisualStyleBackColor = true;
        	this.radNoSnap.CheckedChanged += new System.EventHandler(this.radNoSnap_CheckedChanged);
        	// 
        	// radCenterSnap
        	// 
        	this.radCenterSnap.BackColor = System.Drawing.Color.Transparent;
        	this.radCenterSnap.Font = new System.Drawing.Font("Microsoft Sans Serif", 6.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(238)));
        	this.radCenterSnap.ImeMode = System.Windows.Forms.ImeMode.NoControl;
        	this.radCenterSnap.Location = new System.Drawing.Point(6, 42);
        	this.radCenterSnap.Name = "radCenterSnap";
        	this.radCenterSnap.Size = new System.Drawing.Size(97, 17);
        	this.radCenterSnap.TabIndex = 27;
        	this.radCenterSnap.Text = "Center/Door Snap";
        	this.radCenterSnap.UseVisualStyleBackColor = false;
        	this.radCenterSnap.CheckedChanged += new System.EventHandler(this.radCenterSnap_CheckedChanged);
        	// 
        	// waypointGroup
        	// 
        	this.waypointGroup.Controls.Add(this.buttonWaypointMode);
        	this.waypointGroup.Controls.Add(this.waypointName);
        	this.waypointGroup.Controls.Add(this.label1);
        	this.waypointGroup.Controls.Add(this.waypointEnabled);
        	this.waypointGroup.Location = new System.Drawing.Point(13, 387);
        	this.waypointGroup.Name = "waypointGroup";
        	this.waypointGroup.Size = new System.Drawing.Size(200, 103);
        	this.waypointGroup.TabIndex = 30;
        	this.waypointGroup.TabStop = false;
        	this.waypointGroup.Text = "Waypoint properties";
        	// 
        	// buttonWaypointMode
        	// 
        	this.buttonWaypointMode.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold);
        	this.buttonWaypointMode.Location = new System.Drawing.Point(55, 72);
        	this.buttonWaypointMode.Name = "buttonWaypointMode";
        	this.buttonWaypointMode.Size = new System.Drawing.Size(89, 23);
        	this.buttonWaypointMode.TabIndex = 20;
        	this.buttonWaypointMode.Text = " ";
        	this.buttonWaypointMode.UseVisualStyleBackColor = true;
        	// 
        	// waypointName
        	// 
        	this.waypointName.Location = new System.Drawing.Point(64, 22);
        	this.waypointName.Name = "waypointName";
        	this.waypointName.Size = new System.Drawing.Size(120, 20);
        	this.waypointName.TabIndex = 0;
        	this.waypointName.TextChanged += new System.EventHandler(this.WaypointNameTextChanged);
        	// 
        	// label1
        	// 
        	this.label1.Location = new System.Drawing.Point(11, 24);
        	this.label1.Name = "label1";
        	this.label1.Size = new System.Drawing.Size(48, 23);
        	this.label1.TabIndex = 1;
        	this.label1.Text = "Name";
        	// 
        	// waypointEnabled
        	// 
        	this.waypointEnabled.Location = new System.Drawing.Point(64, 42);
        	this.waypointEnabled.Name = "waypointEnabled";
        	this.waypointEnabled.Size = new System.Drawing.Size(72, 24);
        	this.waypointEnabled.TabIndex = 2;
        	this.waypointEnabled.Text = "Enabled";
        	this.waypointEnabled.UseVisualStyleBackColor = true;
        	this.waypointEnabled.CheckedChanged += new System.EventHandler(this.WaypointEnabledCheckedChanged);
        	// 
        	// extentsGroup
        	// 
        	this.extentsGroup.Controls.Add(this.radioExtentsShowAll);
        	this.extentsGroup.Controls.Add(this.radioExtentsHide);
        	this.extentsGroup.Controls.Add(this.radioExtentShowColl);
        	this.extentsGroup.Location = new System.Drawing.Point(8, 285);
        	this.extentsGroup.Name = "extentsGroup";
        	this.extentsGroup.Size = new System.Drawing.Size(105, 88);
        	this.extentsGroup.TabIndex = 29;
        	this.extentsGroup.TabStop = false;
        	this.extentsGroup.Text = "Extents Settings";
        	// 
        	// radioExtentsShowAll
        	// 
        	this.radioExtentsShowAll.AutoSize = true;
        	this.radioExtentsShowAll.ImeMode = System.Windows.Forms.ImeMode.NoControl;
        	this.radioExtentsShowAll.Location = new System.Drawing.Point(6, 65);
        	this.radioExtentsShowAll.Name = "radioExtentsShowAll";
        	this.radioExtentsShowAll.Size = new System.Drawing.Size(66, 17);
        	this.radioExtentsShowAll.TabIndex = 1;
        	this.radioExtentsShowAll.Text = "Show All";
        	this.radioExtentsShowAll.UseVisualStyleBackColor = true;
        	this.radioExtentsShowAll.CheckedChanged += new System.EventHandler(this.radioButton3_CheckedChanged);
        	// 
        	// radioExtentsHide
        	// 
        	this.radioExtentsHide.AutoSize = true;
        	this.radioExtentsHide.Checked = true;
        	this.radioExtentsHide.ImeMode = System.Windows.Forms.ImeMode.NoControl;
        	this.radioExtentsHide.Location = new System.Drawing.Point(6, 19);
        	this.radioExtentsHide.Name = "radioExtentsHide";
        	this.radioExtentsHide.Size = new System.Drawing.Size(47, 17);
        	this.radioExtentsHide.TabIndex = 0;
        	this.radioExtentsHide.TabStop = true;
        	this.radioExtentsHide.Text = "Hide";
        	this.radioExtentsHide.UseVisualStyleBackColor = true;
        	this.radioExtentsHide.CheckedChanged += new System.EventHandler(this.radioButton2_CheckedChanged);
        	// 
        	// radioExtentShowColl
        	// 
        	this.radioExtentShowColl.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(238)));
        	this.radioExtentShowColl.Location = new System.Drawing.Point(6, 42);
        	this.radioExtentShowColl.Name = "radioExtentShowColl";
        	this.radioExtentShowColl.Size = new System.Drawing.Size(97, 17);
        	this.radioExtentShowColl.TabIndex = 0;
        	this.radioExtentShowColl.Text = "Show Colliding";
        	this.radioExtentShowColl.UseVisualStyleBackColor = true;
        	this.radioExtentShowColl.CheckedChanged += new System.EventHandler(this.radioButton1_CheckedChanged);
        	// 
        	// objectGroup
        	// 
        	this.objectGroup.Controls.Add(this.buttonObjectMode);
        	this.objectGroup.Controls.Add(this.label6);
        	this.objectGroup.Controls.Add(this.objectCategoriesBox);
        	this.objectGroup.Controls.Add(this.cboObjCreate);
        	this.objectGroup.Controls.Add(this.objectPreview);
        	this.objectGroup.Location = new System.Drawing.Point(24, 11);
        	this.objectGroup.Name = "objectGroup";
        	this.objectGroup.Size = new System.Drawing.Size(182, 259);
        	this.objectGroup.TabIndex = 24;
        	this.objectGroup.TabStop = false;
        	this.objectGroup.Text = "Objects list";
        	// 
        	// buttonObjectMode
        	// 
        	this.buttonObjectMode.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold);
        	this.buttonObjectMode.Location = new System.Drawing.Point(44, 224);
        	this.buttonObjectMode.Name = "buttonObjectMode";
        	this.buttonObjectMode.Size = new System.Drawing.Size(96, 23);
        	this.buttonObjectMode.TabIndex = 32;
        	this.buttonObjectMode.Text = " ";
        	this.buttonObjectMode.UseVisualStyleBackColor = true;
        	// 
        	// label6
        	// 
        	this.label6.Location = new System.Drawing.Point(12, 198);
        	this.label6.Name = "label6";
        	this.label6.Size = new System.Drawing.Size(50, 23);
        	this.label6.TabIndex = 31;
        	this.label6.Text = "Category";
        	// 
        	// objectCategoriesBox
        	// 
        	this.objectCategoriesBox.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
        	this.objectCategoriesBox.FormattingEnabled = true;
        	this.objectCategoriesBox.Location = new System.Drawing.Point(68, 195);
        	this.objectCategoriesBox.Name = "objectCategoriesBox";
        	this.objectCategoriesBox.Size = new System.Drawing.Size(104, 21);
        	this.objectCategoriesBox.TabIndex = 30;
        	this.objectCategoriesBox.SelectedIndexChanged += new System.EventHandler(this.ObjectCategoriesBoxSelectedIndexChanged);
        	// 
        	// cboObjCreate
        	// 
        	this.cboObjCreate.FormattingEnabled = true;
        	this.cboObjCreate.Location = new System.Drawing.Point(12, 168);
        	this.cboObjCreate.MaxDropDownItems = 15;
        	this.cboObjCreate.Name = "cboObjCreate";
        	this.cboObjCreate.Size = new System.Drawing.Size(160, 21);
        	this.cboObjCreate.TabIndex = 23;
        	this.cboObjCreate.SelectedIndexChanged += new System.EventHandler(this.CboObjCreateSelectedIndexChanged);
        	// 
        	// objectPreview
        	// 
        	this.objectPreview.BackColor = System.Drawing.Color.Black;
        	this.objectPreview.BackgroundImageLayout = System.Windows.Forms.ImageLayout.Center;
        	this.objectPreview.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D;
        	this.objectPreview.Location = new System.Drawing.Point(24, 24);
        	this.objectPreview.Name = "objectPreview";
        	this.objectPreview.Size = new System.Drawing.Size(133, 133);
        	this.objectPreview.SizeMode = System.Windows.Forms.PictureBoxSizeMode.CenterImage;
        	this.objectPreview.TabIndex = 25;
        	this.objectPreview.TabStop = false;
        	// 
        	// scrollPanel
        	// 
        	this.scrollPanel.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
			| System.Windows.Forms.AnchorStyles.Left) 
			| System.Windows.Forms.AnchorStyles.Right)));
        	this.scrollPanel.AutoScroll = true;
        	this.scrollPanel.Controls.Add(this.mapPanel);
        	this.scrollPanel.Location = new System.Drawing.Point(249, 0);
        	this.scrollPanel.Name = "scrollPanel";
        	this.scrollPanel.Size = new System.Drawing.Size(608, 653);
        	this.scrollPanel.TabIndex = 6;
        	this.scrollPanel.Scroll += new System.Windows.Forms.ScrollEventHandler(this.scrollPanel_Scroll);
        	// 
        	// mapPanel
        	// 
        	this.mapPanel.Location = new System.Drawing.Point(0, 7);
        	this.mapPanel.Name = "mapPanel";
        	this.mapPanel.Size = new System.Drawing.Size(5888, 5888);
        	this.mapPanel.TabIndex = 0;
        	this.mapPanel.Paint += new System.Windows.Forms.PaintEventHandler(this.mapPanel_Paint);
        	this.mapPanel.MouseDown += new System.Windows.Forms.MouseEventHandler(this.mapPanel_MouseDown);
        	this.mapPanel.MouseLeave += new System.EventHandler(this.mapPanel_MouseLeave);
        	this.mapPanel.MouseMove += new System.Windows.Forms.MouseEventHandler(this.mapPanel_MouseMove);
        	this.mapPanel.MouseUp += new System.Windows.Forms.MouseEventHandler(this.mapPanel_MouseUp);
        	// 
        	// contextMenu
        	// 
        	this.contextMenu.MenuItems.AddRange(new System.Windows.Forms.MenuItem[] {
			this.contextMenuCopy,
			this.contextMenuPaste,
			this.contextMenuDelete,
			this.menuItem3,
			this.contextMenuProperties});
        	this.contextMenu.Popup += new System.EventHandler(this.contextMenu_Popup);
        	// 
        	// contextMenuCopy
        	// 
        	this.contextMenuCopy.Index = 0;
        	this.contextMenuCopy.Text = "Copy";
        	this.contextMenuCopy.Click += new System.EventHandler(this.contextMenuCopy_Click);
        	// 
        	// contextMenuPaste
        	// 
        	this.contextMenuPaste.Index = 1;
        	this.contextMenuPaste.Text = "Paste";
        	this.contextMenuPaste.Click += new System.EventHandler(this.contextMenuPaste_Click);
        	// 
        	// tmrInvalidate
        	// 
        	this.tmrInvalidate.Enabled = true;
        	this.tmrInvalidate.Tick += new System.EventHandler(this.tmrInvalidate_Tick);
        	// 
        	// MapView
        	// 
        	this.Controls.Add(this.groupAdv);
        	this.Controls.Add(this.scrollPanel);
        	this.Controls.Add(this.statusBar);
        	this.Name = "MapView";
        	this.Size = new System.Drawing.Size(859, 679);
        	((System.ComponentModel.ISupportInitialize)(this.statusMode)).EndInit();
        	((System.ComponentModel.ISupportInitialize)(this.statusLocation)).EndInit();
        	((System.ComponentModel.ISupportInitialize)(this.statusMapItem)).EndInit();
        	((System.ComponentModel.ISupportInitialize)(this.statusPolygon)).EndInit();
        	this.groupAdv.ResumeLayout(false);
        	this.tabMapTools.ResumeLayout(false);
        	this.tabWalls.ResumeLayout(false);
        	this.tabTiles.ResumeLayout(false);
        	this.tabEdges.ResumeLayout(false);
        	this.tabObjectWps.ResumeLayout(false);
        	this.groupGridSnap.ResumeLayout(false);
        	this.groupGridSnap.PerformLayout();
        	this.waypointGroup.ResumeLayout(false);
        	this.waypointGroup.PerformLayout();
        	this.extentsGroup.ResumeLayout(false);
        	this.extentsGroup.PerformLayout();
        	this.objectGroup.ResumeLayout(false);
        	((System.ComponentModel.ISupportInitialize)(this.objectPreview)).EndInit();
        	this.scrollPanel.ResumeLayout(false);
        	this.ResumeLayout(false);

        }
        private System.Windows.Forms.GroupBox groupGridSnap;
        public System.Windows.Forms.CheckBox waypointEnabled;
        private System.Windows.Forms.Label label1;
        public System.Windows.Forms.TextBox waypointName;
        private System.Windows.Forms.GroupBox waypointGroup;
        public MapEditor.newgui.EdgeMakeTab EdgeMakeNewCtrl;
        private System.Windows.Forms.TabPage tabEdges;
        public TileMakeTab TileMakeNewCtrl;
        private System.Windows.Forms.TabPage tabTiles;
        public WallMakeTab WallMakeNewCtrl;
        private System.Windows.Forms.TabPage tabWalls;
        private System.Windows.Forms.Label label6;
        private System.Windows.Forms.ComboBox objectCategoriesBox;
        private System.Windows.Forms.StatusBarPanel statusPolygon;
        #endregion

        public MapView()
        {
            InitializeComponent();
          
            WidthMod = groupAdv.Width;
            MapRenderer = new MapViewRenderer(this);

            // setup window handlers
            if (MainWindow.Instance != null)
            {
            	MainWindow.Instance.KeyDown += new KeyEventHandler(MapView_DeletePressed);
				MainWindow.Instance.KeyDown += new KeyEventHandler(StartShiftSelecting);
				MainWindow.Instance.KeyUp += new KeyEventHandler(StopShiftSelecting);
            	MainWindow.Instance.MouseWheel += new MouseEventHandler(MouseWheelEventHandler);
            }
            
            // initialize tabs
            WallMakeNewCtrl.SetMapView(this);
            TileMakeNewCtrl.SetMapView(this);
            EdgeMakeNewCtrl.SetMapView(this);
            PolygonEditDlg = new PolygonEditor();
            // initialize buttons
            buttonObjectMode.SetStates(new EditMode[] { EditMode.OBJECT_SELECT, EditMode.OBJECT_PLACE });
            buttonWaypointMode.SetStates(new EditMode[] { EditMode.WAYPOINT_PLACE, EditMode.WAYPOINT_SELECT, EditMode.WAYPOINT_CONNECT });
            // alter initial mode
            tabMapTools.SelectedTab = tabObjectWps;
            MapInterface.CurrentMode = EditMode.OBJECT_SELECT;
        }
        
        void MapView_DeletePressed(object sender, KeyEventArgs e)
        {
        	if (e.KeyCode == Keys.Delete)
        	{
            	DeleteSelectedObjects();
            	mapPanel.Invalidate();
        	}
        	// I won't make this feature accessible from menus, cuz it's kinda unusable at the moment 
        	if (e.KeyCode == Keys.F7)
        	{
        		new MapGeneratorDlg(this).ShowDialog();
        	}
        }

        void MouseWheelEventHandler(object sender, MouseEventArgs e)
        {
        	// rotate some objects
        	if (MapInterface.CurrentMode == EditMode.OBJECT_SELECT && !SelectedObjects.IsEmpty)
        	{
        		foreach (Map.Object obj in SelectedObjects)
        		{
        			if (obj.CanBeRotated())
        			{
        				// doors
        				if (ThingDb.Things[obj.Name].Xfer == "DoorXfer")
        				{
    						DoorXfer door = obj.GetExtraData<DoorXfer>();
    						sbyte facing = (sbyte) door.Direction;
    						if (e.Delta >= 90) facing += 8;
    						if (e.Delta <= 90) facing -= 8;
    						
    						if (facing > 24) facing = 0;
    						if (facing < 0) facing = 24;
    						door.Direction = (DoorXfer.DOORS_DIR) facing;
        				}
        				// sentry beams
        				if (ThingDb.Things[obj.Name].Xfer == "SentryXfer")
        				{
    						SentryXfer xfer = obj.GetExtraData<SentryXfer>();
    						// rad2deg
    						int rotatDegrees = (int) (xfer.BasePosRadian * 180 / Math.PI);
    						// rotate(lel)
    						rotatDegrees += e.Delta / 30;
    						if (rotatDegrees > 360) rotatDegrees -= 360;
    						// deg2rad
    						xfer.BasePosRadian = (float) (rotatDegrees * Math.PI / 180);
        				}
        			}
        		}
        	}
        }

        void StopShiftSelecting(object sender, KeyEventArgs e)
        {
        	if (!e.Shift) MapInterface.KeyHelper.ShiftKey = false;
        }

        void StartShiftSelecting(object sender, KeyEventArgs e)
        {
        	if (e.Shift) MapInterface.KeyHelper.ShiftKey = true;
        }
        
        private void mapPanel_MouseDown(object sender, MouseEventArgs e)
        {
        	Point pt = new Point(e.X, e.Y);

            if (e.Button.Equals(MouseButtons.Middle))
            {
                // re-center camera
                CenterAtPoint(pt);
                mapPanel.Invalidate();
            }
           	
            // Open properties if shift is hold, show context menu otherwise
            if (MapInterface.CurrentMode == EditMode.OBJECT_SELECT && e.Button.Equals(MouseButtons.Right))
            {
            	// TODO: ShowObjectProperties(MapInterface.ObjectSelect(pt));
            	contextMenu.Show(mapPanel, pt);
            }
            
            // reshape polygon if special mode is active
            if (MapInterface.CurrentMode == EditMode.POLYGON_RESHAPE)
            {
            	if (PolygonEditDlg.SelectedPolygon != null && e.Button.Equals(MouseButtons.Left))
            		PolygonEditDlg.SelectedPolygon.Points.Add(new PointF(e.X, e.Y));
            }
            
            // pass into mapinterface handlers (operations)
            if (e.Button.Equals(MouseButtons.Left))
            	MapInterface.HandleLMouseClick(pt);
            else if (e.Button.Equals(MouseButtons.Right))
            	MapInterface.HandleRMouseClick(pt);
            
            // update the visible map part
            MapRenderer.UpdateCanvas(MapInterface.OpUpdatedObjects, MapInterface.OpUpdatedTiles);
            MapInterface.ResetUpdateTracker();
        }


        private void mapPanel_MouseMove(object sender, MouseEventArgs e)
        {
            mouseLocation = new Point(e.X, e.Y);
            
            // Update statusbar contents
            statusHelper.Update(mouseLocation);
            statusLocation.Text = statusHelper.StatusLocation;
            if (statusHelper.ValuesChanged())
            {
            	statusMapItem.Text = statusHelper.StatusMapItem;
            	statusPolygon.Text = statusHelper.StatusPolygon;
            }

            if (e.Button.Equals(MouseButtons.Left))
            {
            	Point pt = mouseLocation;
            	
            	// call handlers for some mouse operations
                if (MapInterface.CurrentMode == EditMode.FLOOR_BRUSH || MapInterface.CurrentMode == EditMode.FLOOR_PLACE || MapInterface.CurrentMode == EditMode.WALL_BRUSH)
            		MapInterface.HandleLMouseClick(pt);
            	
            	// Snap to grid
            	if (EditorSettings.Default.Edit_SnapGrid)
                	pt = new Point((int)Math.Round((decimal)(pt.X / squareSize)) * squareSize, (int)Math.Round((decimal)(pt.Y / squareSize)) * squareSize);
                if (EditorSettings.Default.Edit_SnapHalfGrid)
                	pt = new Point((int)Math.Round((decimal)((pt.X / (squareSize)) * squareSize) + squareSize / 2), (int)Math.Round((decimal)((pt.Y / (squareSize)) * squareSize) + squareSize / 2));
            	
            	// moving waypoints
                if (MapInterface.CurrentMode == EditMode.WAYPOINT_SELECT)
	            {
	                if (MapInterface.SelectedWaypoint != null)
	                {
	                    MapInterface.SelectedWaypoint.Point.X = pt.X; // Move the waypoint
	                    MapInterface.SelectedWaypoint.Point.Y = pt.Y;
	                    
	                    mapPanel.Invalidate(); // Repaint the screen
	                }
	            }
	            
	            // moving objects
	            if (!SelectedObjects.IsEmpty)
	            {
	            	if (MapInterface.CurrentMode == EditMode.OBJECT_SELECT)
	            	{
	            		if (SelectedObjects.Origin != null)
	            		{
	            			float closestX = SelectedObjects.Origin.Location.X;
	            			float closestY = SelectedObjects.Origin.Location.Y;
	            			// update position of all objects relative
		            		foreach (Map.Object co in SelectedObjects)
		            		{
		            			float relX = closestX - co.Location.X;
		            			float relY = closestY - co.Location.Y;
		            			co.Location = new PointF(pt.X - relX, pt.Y - relY);
		            		}
		            		mapPanel.Invalidate(); // Repaint the screen
	            		}            		
	            	}
	            }
            }
            
            if (e.Button.Equals(MouseButtons.Right))
            {
                if (MapInterface.CurrentMode == EditMode.FLOOR_BRUSH || MapInterface.CurrentMode == EditMode.FLOOR_PLACE || MapInterface.CurrentMode == EditMode.WALL_BRUSH)
            		MapInterface.HandleRMouseClick(mouseLocation);
            }
            // update the visible map part
            MapRenderer.UpdateCanvas(MapInterface.OpUpdatedObjects, MapInterface.OpUpdatedTiles);
        	MapInterface.ResetUpdateTracker();
        }

        /// <summary>
        /// Удаляет предметы, содержащиеся в SelectedObjects, с карты.
        /// </summary>
        public void DeleteSelectedObjects()
        {
        	if (!SelectedObjects.IsEmpty)
        	{
        		foreach (Map.Object o in SelectedObjects) MapInterface.ObjectRemove(o);
            	SelectedObjects.Items.Clear();
        	}
        	MapRenderer.UpdateCanvas(true, false);
        }
        
        private void contextMenuDelete_Click(object sender, EventArgs e)
        {
        	DeleteSelectedObjects();
        	mapPanel.Invalidate();
        }
        
        private SwitchModeButton buttonWaypointMode;
        private SwitchModeButton buttonObjectMode;
        
        private void contextMenuProperties_Click(object sender, EventArgs e)
        {
        	if (!SelectedObjects.IsEmpty)
        	{
        		ShowObjectProperties(SelectedObjects.Items[0]);
        	}
        }
        
        private void ShowObjectProperties(Map.Object obj)
        {
        	if (obj == null) return;
    		// working on the object clone (we should be able to rollback changes)
    		int ndx = Map.Objects.IndexOf(obj);
    		var propDlg = new ObjectPropertiesDialog();
    		propDlg.Object = (Map.Object) Map.Objects[ndx];
    		if (propDlg.ShowDialog() == DialogResult.OK)
    		{
    			// update object reference to updated version
    			Map.Objects[ndx] = propDlg.Object;
    			SelectedObjects.Items[0] = propDlg.Object;
    			// hint renderer
    			MapRenderer.UpdateCanvas(true, false);
    			mapPanel.Invalidate();
    		}
        }

        private void mapPanel_MouseUp(object sender, MouseEventArgs e)
        {
        }
        
        private void mapPanel_Paint(object sender, PaintEventArgs e)
        {
        	// Something goes wrong / there is no map
            if (Map == null || !renderingOk) return;
            
            try
            {
            	// Render map
           		MapRenderer.RenderTo(e.Graphics);
            }
            catch (Exception ex)
            {
            	renderingOk = false;
            	new ExceptionDialog(ex, "Exception in rendering routine").ShowDialog();
            	Environment.Exit(-1);
            }
        }

        public static Point GetNearestTilePoint(Point pt)
        {
            pt.Offset(0, -squareSize);
            return GetNearestWallPoint(pt);
        }
        
        public static Point GetNearestWallPoint(Point pt)
        {
            Point tl = new Point((pt.X / squareSize) * squareSize, (pt.Y / squareSize) * squareSize);
            if (tl.X / squareSize % 2 == tl.Y / squareSize % 2)
                return new Point(tl.X / squareSize, tl.Y / squareSize);
            else
            {
                Point left = new Point(tl.X, tl.Y + squareSize / 2);
                Point right = new Point(tl.X + squareSize, tl.Y + squareSize / 2);
                Point top = new Point(tl.X + squareSize / 2, tl.Y);
                Point bottom = new Point(tl.X + squareSize / 2, tl.Y + squareSize);
                Point closest = left;
                foreach (Point point in new Point[] { left, right, top, bottom })
                    if (Distance(point, pt) < Distance(closest, pt))
                        closest = point;

                if (closest == left)
                    return new Point(tl.X / squareSize - 1, tl.Y / squareSize);
                else if (closest == right)
                    return new Point(tl.X / squareSize + 1, tl.Y / squareSize);
                else if (closest == top)
                    return new Point(tl.X / squareSize, tl.Y / squareSize - 1);
                else
                    return new Point(tl.X / squareSize, tl.Y / squareSize + 1);
            }
        }

        private static int Distance(Point a, Point b)
        {
            return (int)Math.Sqrt(Math.Pow(a.X - b.X, 2) + Math.Pow(a.Y - b.Y, 2));
        }
        
        private void scrollPanel_Scroll(object sender, ScrollEventArgs e)
        {
        	// update visible area
        	MapRenderer.UpdateCanvas(true, true);
            mapPanel.Invalidate(new Rectangle(scrollPanel.HorizontalScroll.Value, scrollPanel.VerticalScroll.Value, scrollPanel.Width, scrollPanel.Height));
        }

        public void CenterAtPoint(Point centerAt)
        {
            if (mapPanel.ClientRectangle.Contains(centerAt))
            {
                int Y = centerAt.Y - scrollPanel.Height / 2;
                int X = centerAt.X - scrollPanel.Width / 2;
                if (Y < 0)
                    Y = 0;
                if (X < 0)
                    X = 0;
                scrollPanel.VerticalScroll.Value = Y;
                scrollPanel.HorizontalScroll.Value = X;
                winX = centerAt.X - WidthMod;
                winY = centerAt.Y;
                MapRenderer.UpdateCanvas(true, true);
                mapPanel.Invalidate();
            }
        }
        
        private void contextMenu_Popup(object sender, EventArgs e)
        {
        	bool enable = true;
        	if (SelectedObjects.IsEmpty)
        		enable = false;
        	
        	// These are inaccessible if there are no selected objects
        	contextMenuCopy.Enabled = enable;
        	contextMenuProperties.Enabled = enable;
        	contextMenuDelete.Enabled = enable;
        }

        private void contextMenuCopy_Click(object sender, EventArgs e)
        {
            if (!SelectedObjects.IsEmpty)
            	Clipboard.SetDataObject(SelectedObjects.Clone(), false);
        }

        private void contextMenuPaste_Click(object sender, EventArgs e)
        {
            if (Clipboard.GetDataObject().GetDataPresent(typeof(MapObjectCollection)))
            {
            	MapObjectCollection collection = (MapObjectCollection) Clipboard.GetDataObject().GetData(typeof(MapObjectCollection));
            	// find closest object
            	double dist = double.MaxValue;
            	float closestX = 0, closestY = 0;
            	foreach (Map.Object ot in collection)
            	{
            		float dx = ot.Location.X - mouseLocation.X;
            		float dy = ot.Location.Y - mouseLocation.Y;
            		double ndist = Math.Sqrt(dx * dx + dy * dy);
            		if (ndist < dist)
            		{
            			dist = ndist;
            			closestX = ot.Location.X;
            			closestY = ot.Location.Y;
            		}
            	}
            	// clear (old) selection, duplicate objects
            	SelectedObjects.Items.Clear();
            	SelectedObjects.Origin = null;
            	Map.Object clone;
            	foreach (Map.Object o in collection)
            	{
            		clone = (Map.Object) o.Clone();
            		// calc relative coordinates
            		float relX = closestX - o.Location.X;
            		float relY = closestY - o.Location.Y;
            		clone.Location = new PointF(mouseLocation.X - relX, mouseLocation.Y - relY);
            		Map.Objects.Add(clone);
            		// add into selection
            		SelectedObjects.Items.Add(clone);
            	}
            	// update canvas
            	MapRenderer.UpdateCanvas(true, false);
            }
        }

        /// <summary>
        /// Renders entire map to a new bitmap, using current settings
        /// </summary>
        public Bitmap MapToImage()
        {
            if (Map == null)
                return null;

            Bitmap bitmap = new Bitmap(5880, 5880);
            using (Graphics g = Graphics.FromImage(bitmap))
            { 
            	MapRenderer.RenderTo(g); 
            }
            return bitmap;
        }

        private void tmrInvalidate_Tick(object sender, EventArgs e)
        {
        	if (MapInterface.ModeIsUpdated)
        	{
        		// Update mode text.
        		statusMode.Text = String.Format("Mode: {0}", MapInterface.CurrentMode);
        		MapInterface.ModeIsUpdated = false;
        	}
        	
            mapPanel.Invalidate();
        }

        private void mapPanel_MouseLeave(object sender, EventArgs e)
        {
        
        
        }
        
        public Map.Tile GetCurrentTileVar(Point tilePt)
        {
           	return TileMakeNewCtrl.GetTile(tilePt);
        }

        private void radFullSnap_CheckedChanged(object sender, EventArgs e)
        {
            EditorSettings.Default.Edit_SnapGrid = false;
            EditorSettings.Default.Edit_SnapHalfGrid = radFullSnap.Checked;
        }

        private void radCenterSnap_CheckedChanged(object sender, EventArgs e)
        {
            EditorSettings.Default.Edit_SnapGrid = radCenterSnap.Checked;
            EditorSettings.Default.Edit_SnapHalfGrid = false;
        }

        private void radNoSnap_CheckedChanged(object sender, EventArgs e)
        {
            EditorSettings.Default.Edit_SnapGrid = false;
            EditorSettings.Default.Edit_SnapHalfGrid = false;
        }

        // Radio button handlers - switch extent view
        private void radioButton2_CheckedChanged(object sender, EventArgs e)
        {
            EditorSettings.Default.Draw_Extents = false;
            EditorSettings.Default.Draw_AllExtents = false;
        }

        private void radioButton1_CheckedChanged(object sender, EventArgs e)
        {
            EditorSettings.Default.Draw_Extents = true;
            EditorSettings.Default.Draw_AllExtents = false;
        }

        private void radioButton3_CheckedChanged(object sender, EventArgs e)
        {
            EditorSettings.Default.Draw_Extents = true;
            EditorSettings.Default.Draw_AllExtents = true;
        }

        void CboObjCreateSelectedIndexChanged(object sender, EventArgs e)
        {
            object thingName = cboObjCreate.SelectedItem;
            // Update object image
            if (thingName != null)
            {
                ThingDb.Thing tt = ThingDb.Things[(string)thingName];
                Bitmap icon = null;
                if (tt.SpritePrettyImage > 0 && (tt.Class & ThingDb.Thing.ClassFlags.MONSTER) == 0)
                {
                    icon = MapRenderer.VideoBag.GetBitmap(tt.SpritePrettyImage);
                }
                else if (tt.SpriteMenuIcon > 0)
                {
                    icon = MapRenderer.VideoBag.GetBitmap(tt.SpriteMenuIcon);
                }
                objectPreview.BackgroundImage = icon;
            }
        }
        
        internal void LoadObjectCategories()
		{
        	try
        	{
        		// Load object list file
				ObjectCategory[] categories = XMLCategoryListReader.ReadCategories("categories.xml");
				objectCategoriesBox.Items.AddRange(categories);
        	}
        	catch (Exception ex)
        	{
        		Logger.Log("Failed to load object category listfile: " + ex.Message);
        	}
        	// If object list file is either empty or failed to load, create "All Objects"
        	if (objectCategoriesBox.Items.Count < 1)
        	{
        		ObjectCategory catDefault = new ObjectCategory("All Objects");
        		catDefault.Rules.Add(new IncludeRule("", IncludeRule.IncludeRuleType.ANYTHING));
        		objectCategoriesBox.Items.Add(catDefault);
        	}
        	objectCategoriesBox.SelectedIndex = 0;
		}
        
        void ObjectCategoriesBoxSelectedIndexChanged(object sender, EventArgs e)
        {
        	object selItem = objectCategoriesBox.SelectedItem;
        	if (selItem != null)
        	{
        		cboObjCreate.Items.Clear();
        		ObjectCategory category = (ObjectCategory) selItem;
        		// update object list contents
        		cboObjCreate.Items.AddRange(category.GetThings());
        		// update selection
        		cboObjCreate.SelectedIndex = 0;
        	}
        }
        
        public Map.Object GetObjectUnderCursor()
        {
        	if (MapInterface.CurrentMode != EditMode.OBJECT_SELECT && MapInterface.CurrentMode != EditMode.OBJECT_PLACE) return null; 
            return MapInterface.ObjectSelect(mouseLocation);
        }
        
        public Map.Wall GetWallUnderCursor()
        {
        	if (MapInterface.CurrentMode != EditMode.WALL_PLACE && MapInterface.CurrentMode != EditMode.WALL_CHANGE) return null;
        	Point wallPt = GetNearestWallPoint(mouseLocation);
        	if (!Map.Walls.ContainsKey(wallPt)) return null;
        	return Map.Walls[wallPt];
        }
        
        void WaypointNameTextChanged(object sender, EventArgs e)
        {
        	if (MapInterface.SelectedWaypoint != null)
        	{
        		MapInterface.SelectedWaypoint.Name = waypointName.Text;
        	}
        }
        
        void WaypointEnabledCheckedChanged(object sender, EventArgs e)
        {
        	if (MapInterface.SelectedWaypoint != null)
        	{
        		MapInterface.SelectedWaypoint.Flags = waypointEnabled.Checked ? 1 : 0;
        	}
        }

		void TabMapToolsSelectedIndexChanged(object sender, EventArgs e)
		{
			// Alter current mode depending on the tab
			var page = tabMapTools.SelectedTab;
			if (page == tabTiles)
				MapInterface.CurrentMode = TileMakeNewCtrl.buttonMode.SelectedMode;
			else if (page == tabWalls)
				MapInterface.CurrentMode = WallMakeNewCtrl.buttonMode.SelectedMode;
			else if (page == tabEdges)
				MapInterface.CurrentMode = EditMode.EDGE_PLACE;
			else
				MapInterface.CurrentMode = buttonObjectMode.SelectedMode;
		}
    }
}