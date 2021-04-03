using System;
using System.Collections;
using System.Diagnostics;
using System.Drawing;
using System.Globalization;
using System.IO;
using System.Text;
using System.Reflection;
using System.Threading;
using System.Windows.Forms;
using System.Runtime.InteropServices;
using NoxShared;
using System.CodeDom;
using System.CodeDom.Compiler;
using System.Drawing.Drawing2D;
using System.Collections.Generic;
using MapEditor.render;
using MapEditor.noxscript2;
using MapEditor.newgui;
using MapEditor.MapInt;

namespace MapEditor
{
	public class MainWindow : Form
    {
        private TabPage largeMap;
		private MainMenu mainMenu1;
		private MenuItem menuItemOpen;
		private MenuItem menuSeparator1;
		private MenuItem menuItemExit;
		private TabControl tabControl1;
		private TabPage mapInfoTab;
		private Label labelTitle;
		private GroupBox groupBox1;
		private TextBox mapSummary;
		private Label labelDescription;
		private Label labelVersion;
		private TextBox mapDescription;
		private Label labelAuthor;
		private Label labelAuthor2;
		private Label labelEmail;
		private Label labelEmail2;
		private TextBox mapAuthor;
		private TextBox mapEmail;
		private TextBox mapEmail2;
		private TextBox mapAuthor2;
		private Label labelDate;
		private TextBox mapDate;
		private TextBox mapVersion;
		private Label labelCopyright;
		private MenuItem menuItemSave;
		private Label minRecLbl;
		private Label maxRecLbl;
		private Label recommendedLbl;
		private Label mapTypeLbl;
		private TextBox mapMinRec;
		private TextBox mapMaxRec;
        private ComboBox mapType;
		private MenuItem menuItemAbout;
		private MenuItem menuItemNew;
		private MenuItem menuItemSaveAs;
		private MenuItem menuGroupMap;
		private MenuItem viewObjects;
		private TextBox mapCopyright;
        private MenuItem menuGroupOptions;
		int mapZoom = 2, mapDimension = 256;

        protected Map map
        {
        	get
        	{
        		return MapInterface.TheMap;
        	}
        }
		public MapView mapView;
		private MenuItem menuGroupFile;
		private System.Windows.Forms.Label label1;
		private System.Windows.Forms.Label label2;
		private System.Windows.Forms.TextBox questTitle;
		private System.Windows.Forms.TextBox questGraphic;
		private System.Windows.Forms.Label label3;
        private System.ComponentModel.IContainer components;
        private MenuItem menuItemGroups;
        private TabPage minimapTab;
        private Panel miniViewPanel;
        private Panel MinimapPanel;
        private GroupBox groupBox2;
        private Button buttonCenter;
        private MenuItem menuItem12;
        private CheckBox chkDevide;
        private CheckBox chkDevide2;
        protected IList cultures;
        private MenuItem menuGroupView;
        private MenuItem menuShowGrid;
        private MenuItem menuUseNewRenderer;
        private Label label4;
        private Panel ambientColorPanel;
		public static MainWindow Instance;
        private TabPage scriptingTab;
        private MenuItem menuItemSettings;
        private MenuItem menuItem1;
        private MenuItem menuItemPolygons;
        private MenuItem menuItemExportImg;
        private const string TITLE_FORMAT = "NoxEdit2014: {0} ( {1} )";

        public MainWindow(string[] args)
        {
        	Instance = this;
        	// Show the loading splash screen
            Splash spl = new Splash();
            spl.Show();
            spl.Refresh();
            // Setup locales
			string m_ExePath = Process.GetCurrentProcess().MainModule.FileName;
            Environment.CurrentDirectory = Path.GetDirectoryName(m_ExePath);
            cultures = GetSupportedCultures();
			InitializeComponent();

			// Set up map type selector (arena by default)
			mapType.Items.AddRange(new ArrayList(Map.MapInfo.MapTypeNames.Values).ToArray());
			mapType.SelectedIndex = 3;
            // load categories xml
            mapView.LoadObjectCategories();
            
            // Keep up shortcut menus with current settings
            menuShowGrid.Checked = EditorSettings.Default.Draw_Grid;
            menuUseNewRenderer.Checked = EditorSettings.Default.Edit_PreviewMode;
            
			LoadNewMap();
			if (args.Length > 0)
			{
				if (File.Exists(args[0])) MapInterface.SwitchMap(args[0]);
			}
            spl.Close();
		}

		#region Windows Form Designer generated code

		/// <summary>

		/// Required method for Designer support - do not modify

		/// the contents of this method with the code editor.

		/// </summary>

		private void InitializeComponent()
		{
			this.components = new System.ComponentModel.Container();
			System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(MainWindow));
			this.menuGroupFile = new System.Windows.Forms.MenuItem();
			this.menuItemNew = new System.Windows.Forms.MenuItem();
			this.menuItemOpen = new System.Windows.Forms.MenuItem();
			this.menuItemSave = new System.Windows.Forms.MenuItem();
			this.menuItemSaveAs = new System.Windows.Forms.MenuItem();
			this.menuSeparator1 = new System.Windows.Forms.MenuItem();
			this.menuItemExit = new System.Windows.Forms.MenuItem();
			this.menuGroupMap = new System.Windows.Forms.MenuItem();
			this.viewObjects = new System.Windows.Forms.MenuItem();
			this.menuItemGroups = new System.Windows.Forms.MenuItem();
			this.menuItemPolygons = new System.Windows.Forms.MenuItem();
			this.menuItem12 = new System.Windows.Forms.MenuItem();
			this.menuGroupOptions = new System.Windows.Forms.MenuItem();
			this.menuItemSettings = new System.Windows.Forms.MenuItem();
			this.menuItemAbout = new System.Windows.Forms.MenuItem();
			this.mainMenu1 = new System.Windows.Forms.MainMenu(this.components);
			this.menuGroupView = new System.Windows.Forms.MenuItem();
			this.menuShowGrid = new System.Windows.Forms.MenuItem();
			this.menuUseNewRenderer = new System.Windows.Forms.MenuItem();
			this.menuItem1 = new System.Windows.Forms.MenuItem();
			this.menuItemExportImg = new System.Windows.Forms.MenuItem();
			this.mapInfoTab = new System.Windows.Forms.TabPage();
			this.groupBox1 = new System.Windows.Forms.GroupBox();
			this.checkServPlayerLimit = new System.Windows.Forms.CheckBox();
			this.label4 = new System.Windows.Forms.Label();
			this.ambientColorPanel = new System.Windows.Forms.Panel();
			this.label3 = new System.Windows.Forms.Label();
			this.questGraphic = new System.Windows.Forms.TextBox();
			this.questTitle = new System.Windows.Forms.TextBox();
			this.label2 = new System.Windows.Forms.Label();
			this.label1 = new System.Windows.Forms.Label();
			this.mapType = new System.Windows.Forms.ComboBox();
			this.mapTypeLbl = new System.Windows.Forms.Label();
			this.recommendedLbl = new System.Windows.Forms.Label();
			this.maxRecLbl = new System.Windows.Forms.Label();
			this.minRecLbl = new System.Windows.Forms.Label();
			this.mapMaxRec = new System.Windows.Forms.TextBox();
			this.mapMinRec = new System.Windows.Forms.TextBox();
			this.mapCopyright = new System.Windows.Forms.TextBox();
			this.labelCopyright = new System.Windows.Forms.Label();
			this.mapVersion = new System.Windows.Forms.TextBox();
			this.mapDate = new System.Windows.Forms.TextBox();
			this.labelDate = new System.Windows.Forms.Label();
			this.mapAuthor2 = new System.Windows.Forms.TextBox();
			this.mapEmail2 = new System.Windows.Forms.TextBox();
			this.mapEmail = new System.Windows.Forms.TextBox();
			this.mapAuthor = new System.Windows.Forms.TextBox();
			this.labelEmail2 = new System.Windows.Forms.Label();
			this.labelEmail = new System.Windows.Forms.Label();
			this.labelAuthor2 = new System.Windows.Forms.Label();
			this.labelAuthor = new System.Windows.Forms.Label();
			this.labelVersion = new System.Windows.Forms.Label();
			this.mapDescription = new System.Windows.Forms.TextBox();
			this.labelDescription = new System.Windows.Forms.Label();
			this.labelTitle = new System.Windows.Forms.Label();
			this.mapSummary = new System.Windows.Forms.TextBox();
			this.minimapTab = new System.Windows.Forms.TabPage();
			this.MinimapPanel = new System.Windows.Forms.Panel();
			this.groupBox2 = new System.Windows.Forms.GroupBox();
			this.chkDevide2 = new System.Windows.Forms.CheckBox();
			this.chkDevide = new System.Windows.Forms.CheckBox();
			this.buttonCenter = new System.Windows.Forms.Button();
			this.miniViewPanel = new System.Windows.Forms.Panel();
			this.largeMap = new System.Windows.Forms.TabPage();
			this.mapView = new MapEditor.MapView();
			this.tabControl1 = new System.Windows.Forms.TabControl();
			this.scriptingTab = new System.Windows.Forms.TabPage();
			this.aScriptUserControl = new MapEditor.noxscript2.ScriptUserControl();
			this.mapInfoTab.SuspendLayout();
			this.groupBox1.SuspendLayout();
			this.minimapTab.SuspendLayout();
			this.MinimapPanel.SuspendLayout();
			this.groupBox2.SuspendLayout();
			this.largeMap.SuspendLayout();
			this.tabControl1.SuspendLayout();
			this.scriptingTab.SuspendLayout();
			this.SuspendLayout();
			// 
			// menuGroupFile
			// 
			this.menuGroupFile.Index = 0;
			this.menuGroupFile.MenuItems.AddRange(new System.Windows.Forms.MenuItem[] {
			this.menuItemNew,
			this.menuItemOpen,
			this.menuItemSave,
			this.menuItemSaveAs,
			this.menuSeparator1,
			this.menuItemExit});
			this.menuGroupFile.Text = "&File";
			// 
			// menuItemNew
			// 
			this.menuItemNew.Index = 0;
			this.menuItemNew.Shortcut = System.Windows.Forms.Shortcut.CtrlN;
			this.menuItemNew.Text = "&New";
			this.menuItemNew.Click += new System.EventHandler(this.menuItemNew_Click);
			// 
			// menuItemOpen
			// 
			this.menuItemOpen.Index = 1;
			this.menuItemOpen.Shortcut = System.Windows.Forms.Shortcut.CtrlO;
			this.menuItemOpen.Text = "&Open";
			this.menuItemOpen.Click += new System.EventHandler(this.menuItemOpen_Click);
			// 
			// menuItemSave
			// 
			this.menuItemSave.Index = 2;
			this.menuItemSave.Shortcut = System.Windows.Forms.Shortcut.CtrlS;
			this.menuItemSave.Text = "&Save";
			this.menuItemSave.Click += new System.EventHandler(this.menuItemSave_Click);
			// 
			// menuItemSaveAs
			// 
			this.menuItemSaveAs.Index = 3;
			this.menuItemSaveAs.Text = "Save &As...";
			this.menuItemSaveAs.Click += new System.EventHandler(this.menuItemSaveAs_Click);
			// 
			// menuSeparator1
			// 
			this.menuSeparator1.Index = 4;
			this.menuSeparator1.Text = "-";
			// 
			// menuItemExit
			// 
			this.menuItemExit.Index = 5;
			this.menuItemExit.Text = "E&xit";
			this.menuItemExit.Click += new System.EventHandler(this.menuItemExit_Click);
			// 
			// menuGroupMap
			// 
			this.menuGroupMap.Index = 1;
			this.menuGroupMap.MenuItems.AddRange(new System.Windows.Forms.MenuItem[] {
			this.viewObjects,
			this.menuItemGroups,
			this.menuItemPolygons,
			this.menuItem12});
			this.menuGroupMap.Text = "&Map";
			// 
			// viewObjects
			// 
			this.viewObjects.Index = 0;
			this.viewObjects.Shortcut = System.Windows.Forms.Shortcut.CtrlShiftL;
			this.viewObjects.Text = "&List Objects";
			this.viewObjects.Click += new System.EventHandler(this.viewObjects_Click);
			// 
			// menuItemGroups
			// 
			this.menuItemGroups.Index = 1;
			this.menuItemGroups.Shortcut = System.Windows.Forms.Shortcut.CtrlShiftG;
			this.menuItemGroups.Text = "&Groups";
			this.menuItemGroups.Click += new System.EventHandler(this.menuItemGroups_Click);
			// 
			// menuItemPolygons
			// 
			this.menuItemPolygons.Index = 2;
			this.menuItemPolygons.Shortcut = System.Windows.Forms.Shortcut.CtrlShiftP;
			this.menuItemPolygons.Text = "Polygons";
			this.menuItemPolygons.Click += new System.EventHandler(this.menuItemPolygons_Click);
			// 
			// menuItem12
			// 
			this.menuItem12.Index = 3;
			this.menuItem12.Text = "Reorder Extents";
			this.menuItem12.Click += new System.EventHandler(this.menuItemReorderExt_Click);
			// 
			// menuGroupOptions
			// 
			this.menuGroupOptions.Index = 3;
			this.menuGroupOptions.MenuItems.AddRange(new System.Windows.Forms.MenuItem[] {
			this.menuItemSettings});
			this.menuGroupOptions.Text = "&Options";
			// 
			// menuItemSettings
			// 
			this.menuItemSettings.Index = 0;
			this.menuItemSettings.Shortcut = System.Windows.Forms.Shortcut.CtrlG;
			this.menuItemSettings.Text = "Settings";
			this.menuItemSettings.Click += new System.EventHandler(this.SettingsItemClick);
			// 
			// menuItemAbout
			// 
			this.menuItemAbout.Index = 4;
			this.menuItemAbout.Text = "&About";
			this.menuItemAbout.Click += new System.EventHandler(this.menuItemAbout_Click);
			// 
			// mainMenu1
			// 
			this.mainMenu1.MenuItems.AddRange(new System.Windows.Forms.MenuItem[] {
			this.menuGroupFile,
			this.menuGroupMap,
			this.menuGroupView,
			this.menuGroupOptions,
			this.menuItemAbout});
			// 
			// menuGroupView
			// 
			this.menuGroupView.Index = 2;
			this.menuGroupView.MenuItems.AddRange(new System.Windows.Forms.MenuItem[] {
			this.menuShowGrid,
			this.menuUseNewRenderer,
			this.menuItem1,
			this.menuItemExportImg});
			this.menuGroupView.Text = "View";
			// 
			// menuShowGrid
			// 
			this.menuShowGrid.Checked = true;
			this.menuShowGrid.Index = 0;
			this.menuShowGrid.Shortcut = System.Windows.Forms.Shortcut.CtrlK;
			this.menuShowGrid.Text = "&Show Grid";
			this.menuShowGrid.Click += new System.EventHandler(this.menuShowGrid_Click);
			// 
			// menuUseNewRenderer
			// 
			this.menuUseNewRenderer.Index = 1;
			this.menuUseNewRenderer.Shortcut = System.Windows.Forms.Shortcut.CtrlShiftX;
			this.menuUseNewRenderer.Text = "Visual Preview Mode";
			this.menuUseNewRenderer.Click += new System.EventHandler(this.menuUseNewRenderer_Click);
			// 
			// menuItem1
			// 
			this.menuItem1.Index = 2;
			this.menuItem1.Text = "Invert Colors";
			this.menuItem1.Click += new System.EventHandler(this.menuItemInvertColors_Click);
			// 
			// menuItemExportImg
			// 
			this.menuItemExportImg.Index = 3;
			this.menuItemExportImg.Text = "Export Image";
			this.menuItemExportImg.Click += new System.EventHandler(this.exportImageMenu_Click);
			// 
			// mapInfoTab
			// 
			this.mapInfoTab.Controls.Add(this.groupBox1);
			this.mapInfoTab.Location = new System.Drawing.Point(4, 22);
			this.mapInfoTab.Name = "mapInfoTab";
			this.mapInfoTab.Size = new System.Drawing.Size(836, 676);
			this.mapInfoTab.TabIndex = 0;
			this.mapInfoTab.Text = "Map Info";
			this.mapInfoTab.UseVisualStyleBackColor = true;
			// 
			// groupBox1
			// 
			this.groupBox1.Controls.Add(this.checkServPlayerLimit);
			this.groupBox1.Controls.Add(this.label4);
			this.groupBox1.Controls.Add(this.ambientColorPanel);
			this.groupBox1.Controls.Add(this.label3);
			this.groupBox1.Controls.Add(this.questGraphic);
			this.groupBox1.Controls.Add(this.questTitle);
			this.groupBox1.Controls.Add(this.label2);
			this.groupBox1.Controls.Add(this.label1);
			this.groupBox1.Controls.Add(this.mapType);
			this.groupBox1.Controls.Add(this.mapTypeLbl);
			this.groupBox1.Controls.Add(this.recommendedLbl);
			this.groupBox1.Controls.Add(this.maxRecLbl);
			this.groupBox1.Controls.Add(this.minRecLbl);
			this.groupBox1.Controls.Add(this.mapMaxRec);
			this.groupBox1.Controls.Add(this.mapMinRec);
			this.groupBox1.Controls.Add(this.mapCopyright);
			this.groupBox1.Controls.Add(this.labelCopyright);
			this.groupBox1.Controls.Add(this.mapVersion);
			this.groupBox1.Controls.Add(this.mapDate);
			this.groupBox1.Controls.Add(this.labelDate);
			this.groupBox1.Controls.Add(this.mapAuthor2);
			this.groupBox1.Controls.Add(this.mapEmail2);
			this.groupBox1.Controls.Add(this.mapEmail);
			this.groupBox1.Controls.Add(this.mapAuthor);
			this.groupBox1.Controls.Add(this.labelEmail2);
			this.groupBox1.Controls.Add(this.labelEmail);
			this.groupBox1.Controls.Add(this.labelAuthor2);
			this.groupBox1.Controls.Add(this.labelAuthor);
			this.groupBox1.Controls.Add(this.labelVersion);
			this.groupBox1.Controls.Add(this.mapDescription);
			this.groupBox1.Controls.Add(this.labelDescription);
			this.groupBox1.Controls.Add(this.labelTitle);
			this.groupBox1.Controls.Add(this.mapSummary);
			this.groupBox1.Dock = System.Windows.Forms.DockStyle.Top;
			this.groupBox1.Location = new System.Drawing.Point(0, 0);
			this.groupBox1.Name = "groupBox1";
			this.groupBox1.Size = new System.Drawing.Size(836, 560);
			this.groupBox1.TabIndex = 2;
			this.groupBox1.TabStop = false;
			// 
			// checkServPlayerLimit
			// 
			this.checkServPlayerLimit.ImeMode = System.Windows.Forms.ImeMode.NoControl;
			this.checkServPlayerLimit.Location = new System.Drawing.Point(288, 315);
			this.checkServPlayerLimit.Name = "checkServPlayerLimit";
			this.checkServPlayerLimit.Size = new System.Drawing.Size(104, 24);
			this.checkServPlayerLimit.TabIndex = 34;
			this.checkServPlayerLimit.Text = "Server Settings";
			this.checkServPlayerLimit.UseVisualStyleBackColor = true;
			this.checkServPlayerLimit.CheckedChanged += new System.EventHandler(this.CheckServPlayerLimitCheckedChanged);
			// 
			// label4
			// 
			this.label4.AutoSize = true;
			this.label4.ImeMode = System.Windows.Forms.ImeMode.NoControl;
			this.label4.Location = new System.Drawing.Point(256, 358);
			this.label4.Name = "label4";
			this.label4.Size = new System.Drawing.Size(72, 13);
			this.label4.TabIndex = 33;
			this.label4.Text = "Ambient Color";
			// 
			// ambientColorPanel
			// 
			this.ambientColorPanel.Location = new System.Drawing.Point(259, 384);
			this.ambientColorPanel.Name = "ambientColorPanel";
			this.ambientColorPanel.Size = new System.Drawing.Size(69, 36);
			this.ambientColorPanel.TabIndex = 32;
			this.ambientColorPanel.Click += new System.EventHandler(this.ambientColorPanel_Click);
			// 
			// label3
			// 
			this.label3.ImeMode = System.Windows.Forms.ImeMode.NoControl;
			this.label3.Location = new System.Drawing.Point(72, 352);
			this.label3.Name = "label3";
			this.label3.Size = new System.Drawing.Size(64, 24);
			this.label3.TabIndex = 31;
			this.label3.Text = "Quest Intro";
			this.label3.TextAlign = System.Drawing.ContentAlignment.MiddleLeft;
			// 
			// questGraphic
			// 
			this.questGraphic.Location = new System.Drawing.Point(88, 416);
			this.questGraphic.Name = "questGraphic";
			this.questGraphic.Size = new System.Drawing.Size(128, 20);
			this.questGraphic.TabIndex = 30;
			// 
			// questTitle
			// 
			this.questTitle.Location = new System.Drawing.Point(88, 384);
			this.questTitle.Name = "questTitle";
			this.questTitle.Size = new System.Drawing.Size(128, 20);
			this.questTitle.TabIndex = 29;
			// 
			// label2
			// 
			this.label2.ImeMode = System.Windows.Forms.ImeMode.NoControl;
			this.label2.Location = new System.Drawing.Point(32, 416);
			this.label2.Name = "label2";
			this.label2.Size = new System.Drawing.Size(48, 24);
			this.label2.TabIndex = 28;
			this.label2.Text = "Graphic";
			this.label2.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
			// 
			// label1
			// 
			this.label1.ImeMode = System.Windows.Forms.ImeMode.NoControl;
			this.label1.Location = new System.Drawing.Point(32, 384);
			this.label1.Name = "label1";
			this.label1.Size = new System.Drawing.Size(48, 24);
			this.label1.TabIndex = 27;
			this.label1.Text = "Title";
			this.label1.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
			// 
			// mapType
			// 
			this.mapType.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
			this.mapType.FormattingEnabled = true;
			this.mapType.ItemHeight = 13;
			this.mapType.Location = new System.Drawing.Point(88, 24);
			this.mapType.Name = "mapType";
			this.mapType.Size = new System.Drawing.Size(88, 21);
			this.mapType.TabIndex = 26;
			// 
			// mapTypeLbl
			// 
			this.mapTypeLbl.ImeMode = System.Windows.Forms.ImeMode.NoControl;
			this.mapTypeLbl.Location = new System.Drawing.Point(24, 24);
			this.mapTypeLbl.Name = "mapTypeLbl";
			this.mapTypeLbl.Size = new System.Drawing.Size(64, 24);
			this.mapTypeLbl.TabIndex = 25;
			this.mapTypeLbl.Text = "Map Type";
			this.mapTypeLbl.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
			// 
			// recommendedLbl
			// 
			this.recommendedLbl.ImeMode = System.Windows.Forms.ImeMode.NoControl;
			this.recommendedLbl.Location = new System.Drawing.Point(256, 264);
			this.recommendedLbl.Name = "recommendedLbl";
			this.recommendedLbl.Size = new System.Drawing.Size(184, 24);
			this.recommendedLbl.TabIndex = 24;
			this.recommendedLbl.Text = "Recommended Number of Players";
			this.recommendedLbl.TextAlign = System.Drawing.ContentAlignment.MiddleLeft;
			// 
			// maxRecLbl
			// 
			this.maxRecLbl.ImeMode = System.Windows.Forms.ImeMode.NoControl;
			this.maxRecLbl.Location = new System.Drawing.Point(328, 288);
			this.maxRecLbl.Name = "maxRecLbl";
			this.maxRecLbl.Size = new System.Drawing.Size(32, 24);
			this.maxRecLbl.TabIndex = 23;
			this.maxRecLbl.Text = "Max";
			this.maxRecLbl.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
			// 
			// minRecLbl
			// 
			this.minRecLbl.ImeMode = System.Windows.Forms.ImeMode.NoControl;
			this.minRecLbl.Location = new System.Drawing.Point(256, 288);
			this.minRecLbl.Name = "minRecLbl";
			this.minRecLbl.Size = new System.Drawing.Size(32, 24);
			this.minRecLbl.TabIndex = 22;
			this.minRecLbl.Text = "Min";
			this.minRecLbl.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
			// 
			// mapMaxRec
			// 
			this.mapMaxRec.Location = new System.Drawing.Point(360, 288);
			this.mapMaxRec.Name = "mapMaxRec";
			this.mapMaxRec.Size = new System.Drawing.Size(32, 20);
			this.mapMaxRec.TabIndex = 21;
			// 
			// mapMinRec
			// 
			this.mapMinRec.Location = new System.Drawing.Point(288, 288);
			this.mapMinRec.Name = "mapMinRec";
			this.mapMinRec.Size = new System.Drawing.Size(32, 20);
			this.mapMinRec.TabIndex = 20;
			// 
			// mapCopyright
			// 
			this.mapCopyright.Location = new System.Drawing.Point(88, 264);
			this.mapCopyright.Name = "mapCopyright";
			this.mapCopyright.Size = new System.Drawing.Size(128, 20);
			this.mapCopyright.TabIndex = 17;
			// 
			// labelCopyright
			// 
			this.labelCopyright.ImeMode = System.Windows.Forms.ImeMode.NoControl;
			this.labelCopyright.Location = new System.Drawing.Point(8, 264);
			this.labelCopyright.Name = "labelCopyright";
			this.labelCopyright.Size = new System.Drawing.Size(72, 24);
			this.labelCopyright.TabIndex = 16;
			this.labelCopyright.Text = "Copyright";
			this.labelCopyright.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
			// 
			// mapVersion
			// 
			this.mapVersion.Location = new System.Drawing.Point(88, 288);
			this.mapVersion.Name = "mapVersion";
			this.mapVersion.Size = new System.Drawing.Size(128, 20);
			this.mapVersion.TabIndex = 15;
			// 
			// mapDate
			// 
			this.mapDate.Location = new System.Drawing.Point(88, 312);
			this.mapDate.Name = "mapDate";
			this.mapDate.Size = new System.Drawing.Size(128, 20);
			this.mapDate.TabIndex = 14;
			// 
			// labelDate
			// 
			this.labelDate.ImeMode = System.Windows.Forms.ImeMode.NoControl;
			this.labelDate.Location = new System.Drawing.Point(8, 312);
			this.labelDate.Name = "labelDate";
			this.labelDate.Size = new System.Drawing.Size(64, 24);
			this.labelDate.TabIndex = 13;
			this.labelDate.Text = "Date";
			this.labelDate.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
			// 
			// mapAuthor2
			// 
			this.mapAuthor2.Location = new System.Drawing.Point(88, 224);
			this.mapAuthor2.Name = "mapAuthor2";
			this.mapAuthor2.Size = new System.Drawing.Size(128, 20);
			this.mapAuthor2.TabIndex = 12;
			// 
			// mapEmail2
			// 
			this.mapEmail2.Location = new System.Drawing.Point(288, 224);
			this.mapEmail2.Name = "mapEmail2";
			this.mapEmail2.Size = new System.Drawing.Size(160, 20);
			this.mapEmail2.TabIndex = 11;
			// 
			// mapEmail
			// 
			this.mapEmail.Location = new System.Drawing.Point(288, 192);
			this.mapEmail.Name = "mapEmail";
			this.mapEmail.Size = new System.Drawing.Size(160, 20);
			this.mapEmail.TabIndex = 10;
			// 
			// mapAuthor
			// 
			this.mapAuthor.Location = new System.Drawing.Point(88, 192);
			this.mapAuthor.Name = "mapAuthor";
			this.mapAuthor.Size = new System.Drawing.Size(128, 20);
			this.mapAuthor.TabIndex = 9;
			// 
			// labelEmail2
			// 
			this.labelEmail2.ImeMode = System.Windows.Forms.ImeMode.NoControl;
			this.labelEmail2.Location = new System.Drawing.Point(248, 224);
			this.labelEmail2.Name = "labelEmail2";
			this.labelEmail2.Size = new System.Drawing.Size(40, 24);
			this.labelEmail2.TabIndex = 8;
			this.labelEmail2.Text = "Email";
			this.labelEmail2.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
			// 
			// labelEmail
			// 
			this.labelEmail.ImeMode = System.Windows.Forms.ImeMode.NoControl;
			this.labelEmail.Location = new System.Drawing.Point(248, 192);
			this.labelEmail.Name = "labelEmail";
			this.labelEmail.Size = new System.Drawing.Size(40, 24);
			this.labelEmail.TabIndex = 7;
			this.labelEmail.Text = "Email";
			this.labelEmail.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
			// 
			// labelAuthor2
			// 
			this.labelAuthor2.ImeMode = System.Windows.Forms.ImeMode.NoControl;
			this.labelAuthor2.Location = new System.Drawing.Point(8, 224);
			this.labelAuthor2.Name = "labelAuthor2";
			this.labelAuthor2.Size = new System.Drawing.Size(72, 24);
			this.labelAuthor2.TabIndex = 6;
			this.labelAuthor2.Text = "Secondary Author";
			this.labelAuthor2.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
			// 
			// labelAuthor
			// 
			this.labelAuthor.ImeMode = System.Windows.Forms.ImeMode.NoControl;
			this.labelAuthor.Location = new System.Drawing.Point(8, 192);
			this.labelAuthor.Name = "labelAuthor";
			this.labelAuthor.Size = new System.Drawing.Size(72, 24);
			this.labelAuthor.TabIndex = 5;
			this.labelAuthor.Text = "Author";
			this.labelAuthor.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
			// 
			// labelVersion
			// 
			this.labelVersion.ImeMode = System.Windows.Forms.ImeMode.NoControl;
			this.labelVersion.Location = new System.Drawing.Point(8, 288);
			this.labelVersion.Name = "labelVersion";
			this.labelVersion.Size = new System.Drawing.Size(72, 24);
			this.labelVersion.TabIndex = 4;
			this.labelVersion.Text = "Version";
			this.labelVersion.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
			// 
			// mapDescription
			// 
			this.mapDescription.Location = new System.Drawing.Point(88, 88);
			this.mapDescription.Multiline = true;
			this.mapDescription.Name = "mapDescription";
			this.mapDescription.Size = new System.Drawing.Size(360, 88);
			this.mapDescription.TabIndex = 3;
			// 
			// labelDescription
			// 
			this.labelDescription.ImeMode = System.Windows.Forms.ImeMode.NoControl;
			this.labelDescription.Location = new System.Drawing.Point(8, 88);
			this.labelDescription.Name = "labelDescription";
			this.labelDescription.Size = new System.Drawing.Size(80, 24);
			this.labelDescription.TabIndex = 2;
			this.labelDescription.Text = "Description";
			this.labelDescription.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
			// 
			// labelTitle
			// 
			this.labelTitle.ImeMode = System.Windows.Forms.ImeMode.NoControl;
			this.labelTitle.Location = new System.Drawing.Point(8, 56);
			this.labelTitle.Name = "labelTitle";
			this.labelTitle.Size = new System.Drawing.Size(80, 24);
			this.labelTitle.TabIndex = 0;
			this.labelTitle.Text = "Title/Summary";
			this.labelTitle.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
			// 
			// mapSummary
			// 
			this.mapSummary.Location = new System.Drawing.Point(88, 56);
			this.mapSummary.Name = "mapSummary";
			this.mapSummary.Size = new System.Drawing.Size(360, 20);
			this.mapSummary.TabIndex = 1;
			// 
			// minimapTab
			// 
			this.minimapTab.Controls.Add(this.MinimapPanel);
			this.minimapTab.Location = new System.Drawing.Point(4, 22);
			this.minimapTab.Name = "minimapTab";
			this.minimapTab.Size = new System.Drawing.Size(836, 676);
			this.minimapTab.TabIndex = 0;
			this.minimapTab.Text = "Mini Map";
			this.minimapTab.UseVisualStyleBackColor = true;
			// 
			// MinimapPanel
			// 
			this.MinimapPanel.Controls.Add(this.groupBox2);
			this.MinimapPanel.Controls.Add(this.miniViewPanel);
			this.MinimapPanel.Dock = System.Windows.Forms.DockStyle.Fill;
			this.MinimapPanel.Location = new System.Drawing.Point(0, 0);
			this.MinimapPanel.Name = "MinimapPanel";
			this.MinimapPanel.Size = new System.Drawing.Size(836, 676);
			this.MinimapPanel.TabIndex = 0;
			// 
			// groupBox2
			// 
			this.groupBox2.BackgroundImageLayout = System.Windows.Forms.ImageLayout.Center;
			this.groupBox2.Controls.Add(this.chkDevide2);
			this.groupBox2.Controls.Add(this.chkDevide);
			this.groupBox2.Controls.Add(this.buttonCenter);
			this.groupBox2.Dock = System.Windows.Forms.DockStyle.Left;
			this.groupBox2.Location = new System.Drawing.Point(0, 0);
			this.groupBox2.Name = "groupBox2";
			this.groupBox2.Size = new System.Drawing.Size(120, 676);
			this.groupBox2.TabIndex = 2;
			this.groupBox2.TabStop = false;
			// 
			// chkDevide2
			// 
			this.chkDevide2.AutoSize = true;
			this.chkDevide2.ImeMode = System.Windows.Forms.ImeMode.NoControl;
			this.chkDevide2.Location = new System.Drawing.Point(8, 79);
			this.chkDevide2.Name = "chkDevide2";
			this.chkDevide2.Size = new System.Drawing.Size(62, 17);
			this.chkDevide2.TabIndex = 7;
			this.chkDevide2.Text = "Divide2";
			this.chkDevide2.UseVisualStyleBackColor = true;
			this.chkDevide2.CheckedChanged += new System.EventHandler(this.chkDevide2_CheckedChanged);
			// 
			// chkDevide
			// 
			this.chkDevide.AutoSize = true;
			this.chkDevide.ImeMode = System.Windows.Forms.ImeMode.NoControl;
			this.chkDevide.Location = new System.Drawing.Point(8, 56);
			this.chkDevide.Name = "chkDevide";
			this.chkDevide.Size = new System.Drawing.Size(62, 17);
			this.chkDevide.TabIndex = 6;
			this.chkDevide.Text = "Divide1";
			this.chkDevide.UseVisualStyleBackColor = true;
			this.chkDevide.CheckedChanged += new System.EventHandler(this.chkDevide_CheckedChanged);
			// 
			// buttonCenter
			// 
			this.buttonCenter.ImeMode = System.Windows.Forms.ImeMode.NoControl;
			this.buttonCenter.Location = new System.Drawing.Point(18, 19);
			this.buttonCenter.Name = "buttonCenter";
			this.buttonCenter.Size = new System.Drawing.Size(84, 23);
			this.buttonCenter.TabIndex = 1;
			this.buttonCenter.Text = "Go to Center";
			this.buttonCenter.UseVisualStyleBackColor = true;
			this.buttonCenter.Click += new System.EventHandler(this.buttonCenter_Click);
			// 
			// miniViewPanel
			// 
			this.miniViewPanel.AutoSizeMode = System.Windows.Forms.AutoSizeMode.GrowAndShrink;
			this.miniViewPanel.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D;
			this.miniViewPanel.Location = new System.Drawing.Point(126, 3);
			this.miniViewPanel.Name = "miniViewPanel";
			this.miniViewPanel.Size = new System.Drawing.Size(655, 529);
			this.miniViewPanel.TabIndex = 1;
			this.miniViewPanel.Paint += new System.Windows.Forms.PaintEventHandler(this.minimap_Paint);
			this.miniViewPanel.MouseDown += new System.Windows.Forms.MouseEventHandler(this.panel1_MouseDown);
			// 
			// largeMap
			// 
			this.largeMap.Controls.Add(this.mapView);
			this.largeMap.Location = new System.Drawing.Point(4, 22);
			this.largeMap.Name = "largeMap";
			this.largeMap.Size = new System.Drawing.Size(836, 696);
			this.largeMap.TabIndex = 0;
			this.largeMap.Text = "Large Map";
			this.largeMap.UseVisualStyleBackColor = true;
			// 
			// mapView
			// 
			this.mapView.Dock = System.Windows.Forms.DockStyle.Fill;
			this.mapView.Location = new System.Drawing.Point(0, 0);
			this.mapView.Name = "mapView";
			this.mapView.Size = new System.Drawing.Size(836, 696);
			this.mapView.TabIndex = 0;
			// 
			// tabControl1
			// 
			this.tabControl1.Controls.Add(this.largeMap);
			this.tabControl1.Controls.Add(this.minimapTab);
			this.tabControl1.Controls.Add(this.mapInfoTab);
			this.tabControl1.Controls.Add(this.scriptingTab);
			this.tabControl1.Dock = System.Windows.Forms.DockStyle.Fill;
			this.tabControl1.ItemSize = new System.Drawing.Size(63, 18);
			this.tabControl1.Location = new System.Drawing.Point(0, 0);
			this.tabControl1.Name = "tabControl1";
			this.tabControl1.SelectedIndex = 0;
			this.tabControl1.Size = new System.Drawing.Size(844, 722);
			this.tabControl1.TabIndex = 0;
			// 
			// scriptingTab
			// 
			this.scriptingTab.Controls.Add(this.aScriptUserControl);
			this.scriptingTab.Location = new System.Drawing.Point(4, 22);
			this.scriptingTab.Name = "scriptingTab";
			this.scriptingTab.Size = new System.Drawing.Size(836, 696);
			this.scriptingTab.TabIndex = 1;
			this.scriptingTab.Text = "Scripts";
			this.scriptingTab.UseVisualStyleBackColor = true;
			// 
			// aScriptUserControl
			// 
			this.aScriptUserControl.Location = new System.Drawing.Point(8, 8);
			this.aScriptUserControl.Name = "aScriptUserControl";
			this.aScriptUserControl.Size = new System.Drawing.Size(820, 680);
			this.aScriptUserControl.TabIndex = 0;
			// 
			// MainWindow
			// 
			this.AutoScaleBaseSize = new System.Drawing.Size(5, 13);
			this.ClientSize = new System.Drawing.Size(844, 722);
			this.Controls.Add(this.tabControl1);
			this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
			this.KeyPreview = true;
			this.Menu = this.mainMenu1;
			this.MinimumSize = new System.Drawing.Size(840, 680);
			this.Name = "MainWindow";
			this.StartPosition = System.Windows.Forms.FormStartPosition.CenterScreen;
			this.FormClosing += new System.Windows.Forms.FormClosingEventHandler(this.MainWindowFormClosing);
			this.Load += new System.EventHandler(this.MainWindow_Load);
			this.KeyUp += new System.Windows.Forms.KeyEventHandler(this.MainWindow_KeyUp);
			this.mapInfoTab.ResumeLayout(false);
			this.groupBox1.ResumeLayout(false);
			this.groupBox1.PerformLayout();
			this.minimapTab.ResumeLayout(false);
			this.MinimapPanel.ResumeLayout(false);
			this.groupBox2.ResumeLayout(false);
			this.groupBox2.PerformLayout();
			this.largeMap.ResumeLayout(false);
			this.tabControl1.ResumeLayout(false);
			this.scriptingTab.ResumeLayout(false);
			this.ResumeLayout(false);

		}

		private MapEditor.noxscript2.ScriptUserControl aScriptUserControl;
		private System.Windows.Forms.CheckBox checkServPlayerLimit;
		#endregion

		[STAThread]
		static void Main(string[] args)
		{
			Logger.Init();
			
            #if !DEBUG
            try
            {
            #endif
                Application.Run(new MainWindow(args));
            #if !DEBUG
			}
			catch (Exception ex)
			{
				new ExceptionDialog(ex, "Exception in main loop").ShowDialog();
				Environment.Exit(-1);
			}
			#endif
		}
		
        public void UpdateMapInfo()
        {
            mapView.SelectedObjects.Items.Clear();

            mapType.SelectedIndex = Map.MapInfo.MapTypeNames.IndexOfKey(map.Info.Type);
            mapSummary.Text = map.Info.Summary;
            mapDescription.Text = map.Info.Description;

            mapAuthor.Text = map.Info.Author;
            mapEmail.Text = map.Info.Email;
            mapAuthor2.Text = map.Info.Author2;
            mapEmail2.Text = map.Info.Email2;

            mapVersion.Text = map.Info.Version;
            mapCopyright.Text = map.Info.Copyright;
            mapDate.Text = map.Info.Date;

            mapMinRec.Text = String.Format("{0}", map.Info.RecommendedMin);
            mapMaxRec.Text = String.Format("{0}", map.Info.RecommendedMax);
            checkServPlayerLimit.Checked = false;
            // если стоят два нуля значит нужно использовать настройки сервера
            // игра при загрузке ставит 2 - 16
            if (map.Info.RecommendedMin == 0 && map.Info.RecommendedMax == 0)
            	checkServPlayerLimit.Checked = true;

            questTitle.Text = map.Info.QIntroTitle;
            questGraphic.Text = map.Info.QIntroGraphic;

            ambientColorPanel.BackColor = map.Ambient.AmbientColor;
            // меняем заголовок окна
            Text = string.Format(TITLE_FORMAT, map.FileName, map.Info.Summary);
            
            // читаем скрипты
            try
            {
            	aScriptUserControl.UpdateForMap(map);
            }
            catch (Exception ex)
            {
            	MessageBox.Show(ex.Message, "Failed to load scripts!", MessageBoxButtons.OK, MessageBoxIcon.Error);
            }
            
            mapView.MapRenderer.UpdateCanvas(true, true);
            Invalidate(true);
        }

		private void menuItemOpen_Click(object sender, EventArgs e)
		{
			OpenFileDialog fd = new OpenFileDialog();

			fd.Filter = "Nox Map Files (*.map)|*.map|Compressed Map Files (*.nxz)|*.nxz";

			if (fd.ShowDialog() == DialogResult.OK && File.Exists(fd.FileName))
			{
				// Load map
				MapInterface.SwitchMap(fd.FileName);
			}
		}

		private void menuItemExit_Click(object sender, EventArgs e)
		{
			Close();
		}

		protected override void OnClosed(EventArgs e)
		{
			Logger.Close();
			Environment.Exit(0);
		}

		private void menuItemSave_Click(object sender, EventArgs e)
		{
			if (map == null) return;
			else if (map.FileName == "")
			{
				// Ask user to choose filename
				menuItemSaveAs.PerformClick();
				return;
			}

			//TODO: check lengths for each to make sure they aren't too long
			map.Info.Type = (Map.MapInfo.MapType) Map.MapInfo.MapTypeNames.GetKey(mapType.SelectedIndex);//FIXME: default to something if unspecified
			map.Info.Summary = mapSummary.Text;
			map.Info.Description = mapDescription.Text;

			map.Info.Author = mapAuthor.Text;
			map.Info.Email = mapEmail.Text;
			map.Info.Author2 = mapAuthor2.Text;
			map.Info.Email2 = mapEmail2.Text;

			map.Info.Version = mapVersion.Text;
			map.Info.Copyright = mapCopyright.Text;
			map.Info.Date = mapDate.Text;
			
			if (checkServPlayerLimit.Checked)
			{
				map.Info.RecommendedMax = 16;
				map.Info.RecommendedMin = 2;
			}
			else
			{
				map.Info.RecommendedMin = mapMinRec.Text.Length == 0 ? (byte) 0 : Convert.ToByte(mapMinRec.Text);
				map.Info.RecommendedMax = mapMaxRec.Text.Length == 0 ? (byte) 0 : Convert.ToByte(mapMaxRec.Text);
			}
			map.Info.QIntroTitle = questTitle.Text;
			map.Info.QIntroGraphic = questGraphic.Text;

			map.WriteMap();
			if (EditorSettings.Default.Save_ExportNXZ)
			{
				try
				{
					map.WriteNxz();
				}
				catch (Exception ex)
				{
					Logger.Log("Failed to write .nxz file! \n" + ex.Message);
					MessageBox.Show("Couldn't write the compressed map. Map compression is still buggy. Try changing your map in any way and saving again.");
				}
			}
		}

		private void menuItemAbout_Click(object sender, EventArgs e)
		{
			AboutDialog dlg = new AboutDialog();
			dlg.ShowDialog();
		}
		
        void LoadNewMap()
        {
        	MapInterface.SwitchMap(null);
            UpdateMapInfo();
        }
        
		private void menuItemNew_Click(object sender, EventArgs e)
        {
            LoadNewMap();
		}

		private void menuItemSaveAs_Click(object sender, EventArgs e)
		{
			SaveFileDialog fd = new SaveFileDialog();
			fd.Filter = "Nox Map Files (*.map)|*.map";

			if (fd.ShowDialog() == DialogResult.OK)//&& fd.FileName)
			{
				map.FileName = fd.FileName;
				menuItemSave.PerformClick();
			}
		}

		private void viewObjects_Click(object sender, EventArgs e)
		{
			ObjectListDialog objLd = new ObjectListDialog();
			objLd.objTable = map.Objects;
			objLd.map = this.mapView;
			objLd.Show();
			objLd.Owner = this;
		}

		public static IList GetSupportedCultures()
		{
			ArrayList list = new ArrayList();
			list.Add(CultureInfo.InvariantCulture);
			foreach (CultureInfo culture in CultureInfo.GetCultures(CultureTypes.AllCultures))
			{
				try
				{
					Assembly.GetExecutingAssembly().GetSatelliteAssembly(culture);
					list.Add(culture);//won't get added if not found (exception will be thrown)
				}
				catch (Exception) {}
			}
			return list;
		}

        private void menuItemGroups_Click(object sender, EventArgs e)
        {
            GroupDialog gd = new GroupDialog();
            gd.GroupD = map.Groups;
            gd.ShowDialog(this);
            map.Groups = gd.GroupD;
        }

        private void exportImageMenu_Click(object sender, EventArgs e)
        {
            SaveFileDialog sfd = new SaveFileDialog();
            sfd.Filter = "Windows Bitmap|*.bmp|JPEG Image|*.jpg|PNG Image|*.png";   
            sfd.AddExtension = true;
            sfd.ValidateNames = true;
            sfd.OverwritePrompt = true;
            sfd.ShowDialog();
            if (sfd.FileName != "")
            {
                Bitmap mapBitmap = mapView.MapToImage();
                if (mapBitmap != null)
                {
                    System.Drawing.Imaging.ImageFormat imageFormat;
                    switch (sfd.FilterIndex)
                    {
                        case 1:
                            imageFormat = System.Drawing.Imaging.ImageFormat.Bmp; // Does this export?
                            break;
                        case 2:
                            imageFormat = System.Drawing.Imaging.ImageFormat.Jpeg; // Only PNG right?
                            break;
                        case 3:
                            imageFormat = System.Drawing.Imaging.ImageFormat.Png; // Why have the other options?
                            break;
                        default:
                            return;
                    }
                    mapBitmap.Save(sfd.FileName, imageFormat);
                }
            }
        }

        private void minimap_Paint(object sender, PaintEventArgs e)
        {
            if (map == null) return;
            
            Graphics graphics = e.Graphics;
            graphics.CompositingMode = CompositingMode.SourceCopy;
            graphics.Clear(Color.Black);
            
            // обновляем размер миникарты
            miniViewPanel.Width = mapDimension * mapZoom;
            miniViewPanel.Height = mapDimension * mapZoom;
            // отрисовка
            Bitmap bitmap = new Bitmap(miniViewPanel.Width, miniViewPanel.Height);
            MinimapRenderer minimap = new MinimapRenderer(bitmap, map);
            minimap.LockBitmap();
            minimap.DrawMinimap(mapZoom);
            bitmap = minimap.UnlockBitmap();
            graphics.DrawImage(bitmap, 0, 0, bitmap.Width, bitmap.Height);
            bitmap.Dispose();
            // линии деления
            if (chkDevide.Checked)
            	graphics.DrawLine(new Pen(Color.Aqua, 1), new Point(0, 0), new Point(512, 512));
            if( chkDevide2.Checked)
            	graphics.DrawLine(new Pen(Color.Aqua, 1), new Point(512, 0), new Point(0, 512));
        }

        private void panel1_MouseDown(object sender, MouseEventArgs e)
        {
            Rectangle minimapBounds = new Rectangle(new Point(0, 0), new Size(mapDimension * mapZoom, mapDimension * mapZoom));
            if (minimapBounds.Contains(e.X, e.Y))
            {
                mapView.CenterAtPoint(new Point(e.X / mapZoom * MapView.squareSize, e.Y / mapZoom * MapView.squareSize));
                //myMap.SetLoc((int)(e.X / mapZoom * MapView.squareSize)-mapView.WidthMod, (int)(e.Y / mapZoom * MapView.squareSize));
                if (e.Button == MouseButtons.Left)
                    tabControl1.SelectTab("largeMap");
            }
        }

        private void buttonCenter_Click(object sender, EventArgs e)
        {
        	mapView.CenterAtPoint(new Point((mapDimension / 2) * MapView.squareSize, (mapDimension / 2) * MapView.squareSize));
        	tabControl1.SelectTab("largeMap");
        }

        private void menuItemReorderExt_Click(object sender, EventArgs e)
        {
            int val = 20;
            foreach (Map.Object obj in map.Objects)
            {
                obj.Extent = val++;
            }
        }

        private void menuItemInvertColors_Click(object sender, EventArgs e)
        {
            menuItem1.Checked = !menuItem1.Checked;
            if (menuItem1.Checked)
                mapView.MapRenderer.ColorLayout.InvertColors();
            else
                 mapView.MapRenderer.ColorLayout.ResetColors();
        }

        private void MainWindow_KeyUp(object sender, KeyEventArgs e)
        {
           /* if (e.KeyCode == Keys.F1)
            {
                HelpBrowser hlp = new HelpBrowser();
                hlp.Show();
            }*/
        }

        private void MainWindow_Load(object sender, EventArgs e)
        {
        }

        private void chkDevide_CheckedChanged(object sender, EventArgs e)
        {
            miniViewPanel.Refresh();
        }

        private void chkDevide2_CheckedChanged(object sender, EventArgs e)
        {
            miniViewPanel.Refresh();
        }

        private void menuShowGrid_Click(object sender, EventArgs e)
        {
            bool check = !menuShowGrid.Checked;
            menuShowGrid.Checked = check;
            // Update settings
            EditorSettings.Default.Draw_Grid = check;
            EditorSettings.Default.Save();
        }

        private void menuUseNewRenderer_Click(object sender, EventArgs e)
        {
            bool check = !menuUseNewRenderer.Checked;
            menuUseNewRenderer.Checked = check;
            // Update settings
            EditorSettings.Default.Edit_PreviewMode = check;
            EditorSettings.Default.Save();
            mapView.MapRenderer.UpdateCanvas(true, true);
        }

        private void ambientColorPanel_Click(object sender, EventArgs e)
        {
            ColorDialog color = new ColorDialog();
            color.Color = ambientColorPanel.BackColor;
            if (color.ShowDialog(this) == DialogResult.OK)
            {
                ambientColorPanel.BackColor = color.Color;
                map.Ambient.AmbientColor = color.Color;
            }
        }
        
		void MainWindowFormClosing(object sender, FormClosingEventArgs e)
		{
			if (MessageBox.Show("Are you sure you wish to close?", "CLOSING EDITOR!", MessageBoxButtons.OKCancel, MessageBoxIcon.Question) == DialogResult.Cancel)
                e.Cancel = true;
		}
		
		void CheckServPlayerLimitCheckedChanged(object sender, EventArgs e)
		{
			mapMaxRec.Enabled = !checkServPlayerLimit.Checked;
			mapMinRec.Enabled = !checkServPlayerLimit.Checked;
		}
		
		void SettingsItemClick(object sender, EventArgs e)
		{
			SettingsDialog settings = new SettingsDialog();
			settings.ShowDialog(this);
		}

        private void menuItemPolygons_Click(object sender, EventArgs e)
        {
            PolygonEditor editor = mapView.PolygonEditDlg;
            editor.ShowDialog();
        }
	}
}