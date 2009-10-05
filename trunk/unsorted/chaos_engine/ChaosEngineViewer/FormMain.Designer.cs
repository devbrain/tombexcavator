namespace ChaosEngineViewer {
  partial class FormMain {
    /// <summary>
    /// Required designer variable.
    /// </summary>
    private System.ComponentModel.IContainer components = null;

    /// <summary>
    /// Clean up any resources being used.
    /// </summary>
    /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
    protected override void Dispose (bool disposing) {
      if (disposing && (components != null)) {
        components.Dispose ();
      }
      base.Dispose (disposing);
    }

    #region Windows Form Designer generated code

    /// <summary>
    /// Required method for Designer support - do not modify
    /// the contents of this method with the code editor.
    /// </summary>
    private void InitializeComponent () {
      this.components = new System.ComponentModel.Container ();
      System.Windows.Forms.SplitContainer splitContainer1;
      System.Windows.Forms.ToolStripMenuItem fileToolStripMenuItem;
      System.Windows.Forms.ToolStripSeparator toolStripSeparator1;
      System.Windows.Forms.Label label2;
      System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager (typeof (FormMain));
      this._UI_panel = new System.Windows.Forms.TextBox ();
      this._UI_ds = new ChaosEngineViewer.DrawingSurface ();
      this._UI_menuSaveAsPNG = new System.Windows.Forms.ToolStripMenuItem ();
      this._UI_menuExit = new System.Windows.Forms.ToolStripMenuItem ();
      this._UI_LevelLabel = new System.Windows.Forms.Label ();
      this.menuStrip1 = new System.Windows.Forms.MenuStrip ();
      this._UI_chaosEngineVersion = new System.Windows.Forms.ToolStripComboBox ();
      this._UI_showHints = new System.Windows.Forms.CheckBox ();
      this._UI_levelIndex = new System.Windows.Forms.NumericUpDown ();
      this._UI_spacing = new System.Windows.Forms.NumericUpDown ();
      this._UI_tabs = new System.Windows.Forms.TabControl ();
      this._UI_levelViewer = new System.Windows.Forms.TabPage ();
      this._UI_spriteViewer = new System.Windows.Forms.TabPage ();
      this._UI_showInfos = new System.Windows.Forms.CheckBox ();
      this._UI_border = new System.Windows.Forms.Button ();
      this.label9 = new System.Windows.Forms.Label ();
      this._UI_cols = new System.Windows.Forms.NumericUpDown ();
      this.label8 = new System.Windows.Forms.Label ();
      this.label7 = new System.Windows.Forms.Label ();
      this._UI_paletteOffsetX16 = new System.Windows.Forms.NumericUpDown ();
      this.label6 = new System.Windows.Forms.Label ();
      this._UI_paletteIndex = new System.Windows.Forms.NumericUpDown ();
      this._UI_keyed = new System.Windows.Forms.Button ();
      this.label5 = new System.Windows.Forms.Label ();
      this._UI_mask = new System.Windows.Forms.ComboBox ();
      this.label3 = new System.Windows.Forms.Label ();
      this.label1 = new System.Windows.Forms.Label ();
      this._UI_customSpriteCombo = new System.Windows.Forms.ComboBox ();
      this.customSpriteInfoBindingSource = new System.Windows.Forms.BindingSource (this.components);
      this._UI_dsSpriteViewer = new ChaosEngineViewer.DrawingSurface ();
      this._UI_menuSaveAllAsPNG = new System.Windows.Forms.ToolStripMenuItem ();
      splitContainer1 = new System.Windows.Forms.SplitContainer ();
      fileToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem ();
      toolStripSeparator1 = new System.Windows.Forms.ToolStripSeparator ();
      label2 = new System.Windows.Forms.Label ();
      splitContainer1.Panel1.SuspendLayout ();
      splitContainer1.Panel2.SuspendLayout ();
      splitContainer1.SuspendLayout ();
      this.menuStrip1.SuspendLayout ();
      ((System.ComponentModel.ISupportInitialize) (this._UI_levelIndex)).BeginInit ();
      ((System.ComponentModel.ISupportInitialize) (this._UI_spacing)).BeginInit ();
      this._UI_tabs.SuspendLayout ();
      this._UI_levelViewer.SuspendLayout ();
      this._UI_spriteViewer.SuspendLayout ();
      ((System.ComponentModel.ISupportInitialize) (this._UI_cols)).BeginInit ();
      ((System.ComponentModel.ISupportInitialize) (this._UI_paletteOffsetX16)).BeginInit ();
      ((System.ComponentModel.ISupportInitialize) (this._UI_paletteIndex)).BeginInit ();
      ((System.ComponentModel.ISupportInitialize) (this.customSpriteInfoBindingSource)).BeginInit ();
      this.SuspendLayout ();
      // 
      // splitContainer1
      // 
      splitContainer1.Anchor = ((System.Windows.Forms.AnchorStyles) ((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom)
                  | System.Windows.Forms.AnchorStyles.Left)
                  | System.Windows.Forms.AnchorStyles.Right)));
      splitContainer1.FixedPanel = System.Windows.Forms.FixedPanel.Panel1;
      splitContainer1.Location = new System.Drawing.Point (0, 29);
      splitContainer1.Margin = new System.Windows.Forms.Padding (0);
      splitContainer1.Name = "splitContainer1";
      // 
      // splitContainer1.Panel1
      // 
      splitContainer1.Panel1.Controls.Add (this._UI_panel);
      // 
      // splitContainer1.Panel2
      // 
      splitContainer1.Panel2.Controls.Add (this._UI_ds);
      splitContainer1.Size = new System.Drawing.Size (788, 495);
      splitContainer1.SplitterDistance = 200;
      splitContainer1.TabIndex = 1;
      // 
      // _UI_panel
      // 
      this._UI_panel.BorderStyle = System.Windows.Forms.BorderStyle.None;
      this._UI_panel.Cursor = System.Windows.Forms.Cursors.Default;
      this._UI_panel.Dock = System.Windows.Forms.DockStyle.Fill;
      this._UI_panel.Location = new System.Drawing.Point (0, 0);
      this._UI_panel.Margin = new System.Windows.Forms.Padding (0);
      this._UI_panel.Multiline = true;
      this._UI_panel.Name = "_UI_panel";
      this._UI_panel.ReadOnly = true;
      this._UI_panel.Size = new System.Drawing.Size (200, 495);
      this._UI_panel.TabIndex = 0;
      // 
      // _UI_ds
      // 
      this._UI_ds.BackColor = System.Drawing.SystemColors.WindowFrame;
      this._UI_ds.Dock = System.Windows.Forms.DockStyle.Fill;
      this._UI_ds.Location = new System.Drawing.Point (0, 0);
      this._UI_ds.Margin = new System.Windows.Forms.Padding (0);
      this._UI_ds.Name = "_UI_ds";
      this._UI_ds.Size = new System.Drawing.Size (584, 495);
      this._UI_ds.TabIndex = 0;
      this._UI_ds.Paint += new System.Windows.Forms.PaintEventHandler (this._UI_ds_Paint);
      this._UI_ds.MouseMove += new System.Windows.Forms.MouseEventHandler (this._UI_ds_MouseMove);
      this._UI_ds.Resize += new System.EventHandler (this._UI_ds_Resize);
      this._UI_ds.MouseUp += new System.Windows.Forms.MouseEventHandler (this._UI_ds_MouseUp);
      // 
      // fileToolStripMenuItem
      // 
      fileToolStripMenuItem.DropDownItems.AddRange (new System.Windows.Forms.ToolStripItem [] {
            this._UI_menuSaveAsPNG,
            this._UI_menuSaveAllAsPNG,
            toolStripSeparator1,
            this._UI_menuExit});
      fileToolStripMenuItem.Name = "fileToolStripMenuItem";
      fileToolStripMenuItem.Size = new System.Drawing.Size (35, 21);
      fileToolStripMenuItem.Text = "File";
      // 
      // _UI_menuSaveAsPNG
      // 
      this._UI_menuSaveAsPNG.Name = "_UI_menuSaveAsPNG";
      this._UI_menuSaveAsPNG.Size = new System.Drawing.Size (172, 22);
      this._UI_menuSaveAsPNG.Text = "Save as PNG...";
      this._UI_menuSaveAsPNG.Click += new System.EventHandler (this._UI_menuSaveAsPNG_Click);
      // 
      // toolStripSeparator1
      // 
      toolStripSeparator1.Name = "toolStripSeparator1";
      toolStripSeparator1.Size = new System.Drawing.Size (169, 6);
      // 
      // _UI_menuExit
      // 
      this._UI_menuExit.Name = "_UI_menuExit";
      this._UI_menuExit.Size = new System.Drawing.Size (172, 22);
      this._UI_menuExit.Text = "Exit";
      this._UI_menuExit.Click += new System.EventHandler (this._UI_menuExit_Click);
      // 
      // label2
      // 
      label2.AutoSize = true;
      label2.Location = new System.Drawing.Point (109, 6);
      label2.Name = "label2";
      label2.Size = new System.Drawing.Size (49, 13);
      label2.TabIndex = 2;
      label2.Text = "Spacing:";
      // 
      // _UI_LevelLabel
      // 
      this._UI_LevelLabel.AutoSize = true;
      this._UI_LevelLabel.Location = new System.Drawing.Point (3, 6);
      this._UI_LevelLabel.Name = "_UI_LevelLabel";
      this._UI_LevelLabel.Size = new System.Drawing.Size (56, 13);
      this._UI_LevelLabel.TabIndex = 1;
      this._UI_LevelLabel.Text = "Level X.X:";
      // 
      // menuStrip1
      // 
      this.menuStrip1.Items.AddRange (new System.Windows.Forms.ToolStripItem [] {
            fileToolStripMenuItem,
            this._UI_chaosEngineVersion});
      this.menuStrip1.Location = new System.Drawing.Point (0, 0);
      this.menuStrip1.Name = "menuStrip1";
      this.menuStrip1.Size = new System.Drawing.Size (792, 25);
      this.menuStrip1.TabIndex = 3;
      this.menuStrip1.Text = "menuStrip1";
      // 
      // _UI_chaosEngineVersion
      // 
      this._UI_chaosEngineVersion.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
      this._UI_chaosEngineVersion.FlatStyle = System.Windows.Forms.FlatStyle.Standard;
      this._UI_chaosEngineVersion.Items.AddRange (new object [] {
            "Amiga CD32",
            "Amiga OCS",
            "Atari ST"});
      this._UI_chaosEngineVersion.Name = "_UI_chaosEngineVersion";
      this._UI_chaosEngineVersion.Size = new System.Drawing.Size (96, 21);
      this._UI_chaosEngineVersion.SelectedIndexChanged += new System.EventHandler (this._UI_chaosEngineVersion_SelectedIndexChanged);
      // 
      // _UI_showHints
      // 
      this._UI_showHints.Anchor = ((System.Windows.Forms.AnchorStyles) ((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
      this._UI_showHints.AutoSize = true;
      this._UI_showHints.Location = new System.Drawing.Point (703, 6);
      this._UI_showHints.Name = "_UI_showHints";
      this._UI_showHints.Size = new System.Drawing.Size (80, 17);
      this._UI_showHints.TabIndex = 4;
      this._UI_showHints.Text = "Show Hints";
      this._UI_showHints.UseVisualStyleBackColor = true;
      this._UI_showHints.CheckedChanged += new System.EventHandler (this._UI_showHints_CheckedChanged);
      // 
      // _UI_levelIndex
      // 
      this._UI_levelIndex.Location = new System.Drawing.Point (65, 3);
      this._UI_levelIndex.Maximum = new decimal (new int [] {
            16,
            0,
            0,
            0});
      this._UI_levelIndex.Minimum = new decimal (new int [] {
            1,
            0,
            0,
            -2147483648});
      this._UI_levelIndex.Name = "_UI_levelIndex";
      this._UI_levelIndex.Size = new System.Drawing.Size (38, 20);
      this._UI_levelIndex.TabIndex = 5;
      this._UI_levelIndex.ValueChanged += new System.EventHandler (this._UI_levelIndex_ValueChanged);
      // 
      // _UI_spacing
      // 
      this._UI_spacing.Location = new System.Drawing.Point (164, 3);
      this._UI_spacing.Maximum = new decimal (new int [] {
            3,
            0,
            0,
            0});
      this._UI_spacing.Name = "_UI_spacing";
      this._UI_spacing.Size = new System.Drawing.Size (38, 20);
      this._UI_spacing.TabIndex = 6;
      this._UI_spacing.Value = new decimal (new int [] {
            3,
            0,
            0,
            0});
      this._UI_spacing.ValueChanged += new System.EventHandler (this._UI_spacing_ValueChanged);
      // 
      // _UI_tabs
      // 
      this._UI_tabs.Anchor = ((System.Windows.Forms.AnchorStyles) ((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom)
                  | System.Windows.Forms.AnchorStyles.Left)
                  | System.Windows.Forms.AnchorStyles.Right)));
      this._UI_tabs.Appearance = System.Windows.Forms.TabAppearance.FlatButtons;
      this._UI_tabs.Controls.Add (this._UI_levelViewer);
      this._UI_tabs.Controls.Add (this._UI_spriteViewer);
      this._UI_tabs.Location = new System.Drawing.Point (0, 24);
      this._UI_tabs.Margin = new System.Windows.Forms.Padding (0);
      this._UI_tabs.Name = "_UI_tabs";
      this._UI_tabs.SelectedIndex = 0;
      this._UI_tabs.Size = new System.Drawing.Size (796, 553);
      this._UI_tabs.TabIndex = 1;
      this._UI_tabs.SelectedIndexChanged += new System.EventHandler (this._UI_tabs_SelectedIndexChanged);
      // 
      // _UI_levelViewer
      // 
      this._UI_levelViewer.Controls.Add (splitContainer1);
      this._UI_levelViewer.Controls.Add (label2);
      this._UI_levelViewer.Controls.Add (this._UI_LevelLabel);
      this._UI_levelViewer.Controls.Add (this._UI_spacing);
      this._UI_levelViewer.Controls.Add (this._UI_levelIndex);
      this._UI_levelViewer.Controls.Add (this._UI_showHints);
      this._UI_levelViewer.Location = new System.Drawing.Point (4, 25);
      this._UI_levelViewer.Margin = new System.Windows.Forms.Padding (0);
      this._UI_levelViewer.Name = "_UI_levelViewer";
      this._UI_levelViewer.Size = new System.Drawing.Size (788, 524);
      this._UI_levelViewer.TabIndex = 0;
      this._UI_levelViewer.Text = "Level Viewer";
      this._UI_levelViewer.UseVisualStyleBackColor = true;
      // 
      // _UI_spriteViewer
      // 
      this._UI_spriteViewer.Controls.Add (this._UI_showInfos);
      this._UI_spriteViewer.Controls.Add (this._UI_border);
      this._UI_spriteViewer.Controls.Add (this.label9);
      this._UI_spriteViewer.Controls.Add (this._UI_cols);
      this._UI_spriteViewer.Controls.Add (this.label8);
      this._UI_spriteViewer.Controls.Add (this.label7);
      this._UI_spriteViewer.Controls.Add (this._UI_paletteOffsetX16);
      this._UI_spriteViewer.Controls.Add (this.label6);
      this._UI_spriteViewer.Controls.Add (this._UI_paletteIndex);
      this._UI_spriteViewer.Controls.Add (this._UI_keyed);
      this._UI_spriteViewer.Controls.Add (this.label5);
      this._UI_spriteViewer.Controls.Add (this._UI_mask);
      this._UI_spriteViewer.Controls.Add (this.label3);
      this._UI_spriteViewer.Controls.Add (this.label1);
      this._UI_spriteViewer.Controls.Add (this._UI_customSpriteCombo);
      this._UI_spriteViewer.Controls.Add (this._UI_dsSpriteViewer);
      this._UI_spriteViewer.Location = new System.Drawing.Point (4, 25);
      this._UI_spriteViewer.Margin = new System.Windows.Forms.Padding (0);
      this._UI_spriteViewer.Name = "_UI_spriteViewer";
      this._UI_spriteViewer.Size = new System.Drawing.Size (788, 524);
      this._UI_spriteViewer.TabIndex = 1;
      this._UI_spriteViewer.Text = "Sprite Viewer";
      this._UI_spriteViewer.UseVisualStyleBackColor = true;
      // 
      // _UI_showInfos
      // 
      this._UI_showInfos.Anchor = ((System.Windows.Forms.AnchorStyles) ((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
      this._UI_showInfos.AutoSize = true;
      this._UI_showInfos.Location = new System.Drawing.Point (697, 29);
      this._UI_showInfos.Name = "_UI_showInfos";
      this._UI_showInfos.Size = new System.Drawing.Size (79, 17);
      this._UI_showInfos.TabIndex = 17;
      this._UI_showInfos.Text = "Show Infos";
      this._UI_showInfos.UseVisualStyleBackColor = true;
      this._UI_showInfos.CheckedChanged += new System.EventHandler (this._UI_showInfos_CheckedChanged);
      // 
      // _UI_border
      // 
      this._UI_border.Anchor = ((System.Windows.Forms.AnchorStyles) ((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
      this._UI_border.AutoSize = true;
      this._UI_border.AutoSizeMode = System.Windows.Forms.AutoSizeMode.GrowAndShrink;
      this._UI_border.Font = new System.Drawing.Font ("Lucida Console", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte) (0)));
      this._UI_border.Location = new System.Drawing.Point (608, 0);
      this._UI_border.Name = "_UI_border";
      this._UI_border.Size = new System.Drawing.Size (64, 21);
      this._UI_border.TabIndex = 16;
      this._UI_border.Text = "#FFFFFF";
      this._UI_border.UseVisualStyleBackColor = false;
      this._UI_border.Click += new System.EventHandler (this._UI_border_Click);
      // 
      // label9
      // 
      this.label9.Anchor = ((System.Windows.Forms.AnchorStyles) ((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
      this.label9.AutoSize = true;
      this.label9.Location = new System.Drawing.Point (561, 4);
      this.label9.Name = "label9";
      this.label9.Size = new System.Drawing.Size (41, 13);
      this.label9.TabIndex = 15;
      this.label9.Text = "Border:";
      // 
      // _UI_cols
      // 
      this._UI_cols.Location = new System.Drawing.Point (447, 27);
      this._UI_cols.Maximum = new decimal (new int [] {
            99,
            0,
            0,
            0});
      this._UI_cols.Name = "_UI_cols";
      this._UI_cols.Size = new System.Drawing.Size (40, 20);
      this._UI_cols.TabIndex = 14;
      this._UI_cols.TextAlign = System.Windows.Forms.HorizontalAlignment.Right;
      this._UI_cols.ValueChanged += new System.EventHandler (this._UI_cols_ValueChanged);
      // 
      // label8
      // 
      this.label8.AutoSize = true;
      this.label8.Location = new System.Drawing.Point (411, 30);
      this.label8.Name = "label8";
      this.label8.Size = new System.Drawing.Size (30, 13);
      this.label8.TabIndex = 13;
      this.label8.Text = "Cols:";
      // 
      // label7
      // 
      this.label7.AutoSize = true;
      this.label7.Location = new System.Drawing.Point (267, 30);
      this.label7.Name = "label7";
      this.label7.Size = new System.Drawing.Size (93, 13);
      this.label7.TabIndex = 12;
      this.label7.Text = "Palette OffsetX16:";
      // 
      // _UI_paletteOffsetX16
      // 
      this._UI_paletteOffsetX16.Location = new System.Drawing.Point (365, 27);
      this._UI_paletteOffsetX16.Maximum = new decimal (new int [] {
            15,
            0,
            0,
            0});
      this._UI_paletteOffsetX16.Name = "_UI_paletteOffsetX16";
      this._UI_paletteOffsetX16.Size = new System.Drawing.Size (40, 20);
      this._UI_paletteOffsetX16.TabIndex = 11;
      this._UI_paletteOffsetX16.TextAlign = System.Windows.Forms.HorizontalAlignment.Right;
      this._UI_paletteOffsetX16.ValueChanged += new System.EventHandler (this._UI_paletteOffsetX16_ValueChanged);
      // 
      // label6
      // 
      this.label6.AutoSize = true;
      this.label6.Location = new System.Drawing.Point (143, 30);
      this.label6.Name = "label6";
      this.label6.Size = new System.Drawing.Size (72, 13);
      this.label6.TabIndex = 10;
      this.label6.Text = "Palette Index:";
      // 
      // _UI_paletteIndex
      // 
      this._UI_paletteIndex.Location = new System.Drawing.Point (221, 27);
      this._UI_paletteIndex.Name = "_UI_paletteIndex";
      this._UI_paletteIndex.Size = new System.Drawing.Size (40, 20);
      this._UI_paletteIndex.TabIndex = 9;
      this._UI_paletteIndex.TextAlign = System.Windows.Forms.HorizontalAlignment.Right;
      this._UI_paletteIndex.ValueChanged += new System.EventHandler (this._UI_paletteIndex_ValueChanged);
      // 
      // _UI_keyed
      // 
      this._UI_keyed.Anchor = ((System.Windows.Forms.AnchorStyles) ((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
      this._UI_keyed.AutoSize = true;
      this._UI_keyed.AutoSizeMode = System.Windows.Forms.AutoSizeMode.GrowAndShrink;
      this._UI_keyed.Font = new System.Drawing.Font ("Lucida Console", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte) (0)));
      this._UI_keyed.Location = new System.Drawing.Point (724, 0);
      this._UI_keyed.Name = "_UI_keyed";
      this._UI_keyed.Size = new System.Drawing.Size (64, 21);
      this._UI_keyed.TabIndex = 8;
      this._UI_keyed.Text = "#FFFFFF";
      this._UI_keyed.UseVisualStyleBackColor = true;
      this._UI_keyed.Click += new System.EventHandler (this._UI_keyed_Click);
      // 
      // label5
      // 
      this.label5.Anchor = ((System.Windows.Forms.AnchorStyles) ((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
      this.label5.AutoSize = true;
      this.label5.Location = new System.Drawing.Point (678, 4);
      this.label5.Name = "label5";
      this.label5.Size = new System.Drawing.Size (40, 13);
      this.label5.TabIndex = 7;
      this.label5.Text = "Keyed:";
      // 
      // _UI_mask
      // 
      this._UI_mask.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
      this._UI_mask.FormattingEnabled = true;
      this._UI_mask.Items.AddRange (new object [] {
            "Opaque",
            "Transparent",
            "Keyed"});
      this._UI_mask.Location = new System.Drawing.Point (45, 26);
      this._UI_mask.Name = "_UI_mask";
      this._UI_mask.Size = new System.Drawing.Size (92, 21);
      this._UI_mask.TabIndex = 4;
      this._UI_mask.SelectedIndexChanged += new System.EventHandler (this._UI_mask_SelectedIndexChanged);
      // 
      // label3
      // 
      this.label3.AutoSize = true;
      this.label3.Location = new System.Drawing.Point (3, 30);
      this.label3.Name = "label3";
      this.label3.Size = new System.Drawing.Size (36, 13);
      this.label3.TabIndex = 3;
      this.label3.Text = "Mask:";
      // 
      // label1
      // 
      this.label1.AutoSize = true;
      this.label1.Location = new System.Drawing.Point (3, 4);
      this.label1.Name = "label1";
      this.label1.Size = new System.Drawing.Size (54, 13);
      this.label1.TabIndex = 2;
      this.label1.Text = "Template:";
      // 
      // _UI_customSpriteCombo
      // 
      this._UI_customSpriteCombo.DataSource = this.customSpriteInfoBindingSource;
      this._UI_customSpriteCombo.DisplayMember = "Id";
      this._UI_customSpriteCombo.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
      this._UI_customSpriteCombo.FormattingEnabled = true;
      this._UI_customSpriteCombo.IntegralHeight = false;
      this._UI_customSpriteCombo.Location = new System.Drawing.Point (63, 0);
      this._UI_customSpriteCombo.MaxDropDownItems = 20;
      this._UI_customSpriteCombo.Name = "_UI_customSpriteCombo";
      this._UI_customSpriteCombo.Size = new System.Drawing.Size (284, 21);
      this._UI_customSpriteCombo.TabIndex = 1;
      this._UI_customSpriteCombo.ValueMember = "Id";
      this._UI_customSpriteCombo.SelectedIndexChanged += new System.EventHandler (this._UI_customSpriteCombo_SelectedIndexChanged);
      // 
      // customSpriteInfoBindingSource
      // 
      this.customSpriteInfoBindingSource.DataSource = typeof (ChaosEngineViewer.CustomSpriteInfo);
      // 
      // _UI_dsSpriteViewer
      // 
      this._UI_dsSpriteViewer.Anchor = ((System.Windows.Forms.AnchorStyles) ((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom)
                  | System.Windows.Forms.AnchorStyles.Left)
                  | System.Windows.Forms.AnchorStyles.Right)));
      this._UI_dsSpriteViewer.BackColor = System.Drawing.SystemColors.WindowFrame;
      this._UI_dsSpriteViewer.Location = new System.Drawing.Point (0, 50);
      this._UI_dsSpriteViewer.Margin = new System.Windows.Forms.Padding (0);
      this._UI_dsSpriteViewer.Name = "_UI_dsSpriteViewer";
      this._UI_dsSpriteViewer.Size = new System.Drawing.Size (788, 474);
      this._UI_dsSpriteViewer.TabIndex = 0;
      this._UI_dsSpriteViewer.Paint += new System.Windows.Forms.PaintEventHandler (this._UI_dsSpriteViewer_Paint);
      // 
      // _UI_menuSaveAllAsPNG
      // 
      this._UI_menuSaveAllAsPNG.Name = "_UI_menuSaveAllAsPNG";
      this._UI_menuSaveAllAsPNG.Size = new System.Drawing.Size (172, 22);
      this._UI_menuSaveAllAsPNG.Text = "Save All as PNG...";
      this._UI_menuSaveAllAsPNG.Click += new System.EventHandler (this._UI_menuSaveAllAsPNG_Click);
      // 
      // FormMain
      // 
      this.AutoScaleDimensions = new System.Drawing.SizeF (6F, 13F);
      this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
      this.ClientSize = new System.Drawing.Size (792, 573);
      this.Controls.Add (this._UI_tabs);
      this.Controls.Add (this.menuStrip1);
      this.Icon = ((System.Drawing.Icon) (resources.GetObject ("$this.Icon")));
      this.KeyPreview = true;
      this.MinimumSize = new System.Drawing.Size (640, 480);
      this.Name = "FormMain";
      this.StartPosition = System.Windows.Forms.FormStartPosition.Manual;
      this.Text = "Chaos Engine Utility";
      this.Load += new System.EventHandler (this.FormMain_Load);
      this.FormClosed += new System.Windows.Forms.FormClosedEventHandler (this.FormMain_FormClosed);
      this.KeyDown += new System.Windows.Forms.KeyEventHandler (this.FormMain_KeyDown);
      splitContainer1.Panel1.ResumeLayout (false);
      splitContainer1.Panel1.PerformLayout ();
      splitContainer1.Panel2.ResumeLayout (false);
      splitContainer1.ResumeLayout (false);
      this.menuStrip1.ResumeLayout (false);
      this.menuStrip1.PerformLayout ();
      ((System.ComponentModel.ISupportInitialize) (this._UI_levelIndex)).EndInit ();
      ((System.ComponentModel.ISupportInitialize) (this._UI_spacing)).EndInit ();
      this._UI_tabs.ResumeLayout (false);
      this._UI_levelViewer.ResumeLayout (false);
      this._UI_levelViewer.PerformLayout ();
      this._UI_spriteViewer.ResumeLayout (false);
      this._UI_spriteViewer.PerformLayout ();
      ((System.ComponentModel.ISupportInitialize) (this._UI_cols)).EndInit ();
      ((System.ComponentModel.ISupportInitialize) (this._UI_paletteOffsetX16)).EndInit ();
      ((System.ComponentModel.ISupportInitialize) (this._UI_paletteIndex)).EndInit ();
      ((System.ComponentModel.ISupportInitialize) (this.customSpriteInfoBindingSource)).EndInit ();
      this.ResumeLayout (false);
      this.PerformLayout ();

    }

    #endregion

    private DrawingSurface _UI_ds;
    private System.Windows.Forms.TextBox _UI_panel;
    private System.Windows.Forms.MenuStrip menuStrip1;
    private System.Windows.Forms.ToolStripMenuItem _UI_menuSaveAsPNG;
    private System.Windows.Forms.ToolStripMenuItem _UI_menuExit;
    private System.Windows.Forms.CheckBox _UI_showHints;
    private System.Windows.Forms.NumericUpDown _UI_levelIndex;
    private System.Windows.Forms.NumericUpDown _UI_spacing;
    private System.Windows.Forms.Label _UI_LevelLabel;
    private System.Windows.Forms.TabControl _UI_tabs;
    private System.Windows.Forms.TabPage _UI_levelViewer;
    private System.Windows.Forms.TabPage _UI_spriteViewer;
    private DrawingSurface _UI_dsSpriteViewer;
    private System.Windows.Forms.Label label1;
    private System.Windows.Forms.ComboBox _UI_customSpriteCombo;
    private System.Windows.Forms.BindingSource customSpriteInfoBindingSource;
    private System.Windows.Forms.ComboBox _UI_mask;
    private System.Windows.Forms.Label label3;
    private System.Windows.Forms.Button _UI_keyed;
    private System.Windows.Forms.Label label5;
    private System.Windows.Forms.Label label6;
    private System.Windows.Forms.NumericUpDown _UI_paletteIndex;
    private System.Windows.Forms.Label label7;
    private System.Windows.Forms.NumericUpDown _UI_paletteOffsetX16;
    private System.Windows.Forms.NumericUpDown _UI_cols;
    private System.Windows.Forms.Label label8;
    private System.Windows.Forms.Button _UI_border;
    private System.Windows.Forms.Label label9;
    private System.Windows.Forms.CheckBox _UI_showInfos;
    private System.Windows.Forms.ToolStripComboBox _UI_chaosEngineVersion;
    private System.Windows.Forms.ToolStripMenuItem _UI_menuSaveAllAsPNG;
  }
}

