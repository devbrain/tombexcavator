/*
 * MapEditor
 * Пользователь: AngryKirC
 * Дата: 15.01.2015
 */
namespace MapEditor.newgui
{
	partial class TileMakeTab
	{
		/// <summary>
		/// Designer variable used to keep track of non-visual components.
		/// </summary>
		private System.ComponentModel.IContainer components = null;
		
		/// <summary>
		/// Disposes resources used by the control.
		/// </summary>
		/// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
		protected override void Dispose(bool disposing)
		{
			if (disposing) {
				if (components != null) {
					components.Dispose();
				}
			}
			base.Dispose(disposing);
		}
		
		/// <summary>
		/// This method is required for Windows Forms designer support.
		/// Do not change the method contents inside the source code editor. The Forms designer might
		/// not be able to load this method if it was changed manually.
		/// </summary>
		private void InitializeComponent()
		{
			this.label1 = new System.Windows.Forms.Label();
			this.comboTileType = new System.Windows.Forms.ComboBox();
			this.listTileImages = new System.Windows.Forms.ListView();
			this.label2 = new System.Windows.Forms.Label();
			this.BrushSize = new System.Windows.Forms.NumericUpDown();
			this.label3 = new System.Windows.Forms.Label();
			this.comboIgnoreTile = new System.Windows.Forms.ComboBox();
			this.groupBox1 = new System.Windows.Forms.GroupBox();
			this.WallBlockBrush = new System.Windows.Forms.CheckBox();
			this.label5 = new System.Windows.Forms.Label();
			this.labelSep1 = new System.Windows.Forms.Label();
			this.checkAutoVari = new System.Windows.Forms.CheckBox();
			this.buttonMode = new SwitchModeButton();
			((System.ComponentModel.ISupportInitialize)(this.BrushSize)).BeginInit();
			this.groupBox1.SuspendLayout();
			this.SuspendLayout();
			// 
			// label1
			// 
			this.label1.AutoSize = true;
			this.label1.Location = new System.Drawing.Point(10, 21);
			this.label1.Name = "label1";
			this.label1.Size = new System.Drawing.Size(50, 13);
			this.label1.TabIndex = 0;
			this.label1.Text = "Tile type:";
			// 
			// comboTileType
			// 
			this.comboTileType.AutoCompleteMode = System.Windows.Forms.AutoCompleteMode.Append;
			this.comboTileType.AutoCompleteSource = System.Windows.Forms.AutoCompleteSource.ListItems;
			this.comboTileType.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
			this.comboTileType.FormattingEnabled = true;
			this.comboTileType.Location = new System.Drawing.Point(76, 15);
			this.comboTileType.MaxDropDownItems = 20;
			this.comboTileType.Name = "comboTileType";
			this.comboTileType.Size = new System.Drawing.Size(128, 21);
			this.comboTileType.TabIndex = 1;
			this.comboTileType.SelectedIndexChanged += new System.EventHandler(this.UpdateListView);
			// 
			// listTileImages
			// 
			this.listTileImages.Alignment = System.Windows.Forms.ListViewAlignment.SnapToGrid;
			this.listTileImages.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
			this.listTileImages.GridLines = true;
			this.listTileImages.Location = new System.Drawing.Point(4, 207);
			this.listTileImages.Name = "listTileImages";
			this.listTileImages.Size = new System.Drawing.Size(205, 322);
			this.listTileImages.TabIndex = 3;
			this.listTileImages.TileSize = new System.Drawing.Size(46, 46);
			this.listTileImages.UseCompatibleStateImageBehavior = false;
			this.listTileImages.VirtualMode = true;
			this.listTileImages.SelectedIndexChanged += new System.EventHandler(this.ChangeTileType);
			// 
			// label2
			// 
			this.label2.AutoSize = true;
			this.label2.Location = new System.Drawing.Point(11, 66);
			this.label2.Name = "label2";
			this.label2.Size = new System.Drawing.Size(74, 13);
			this.label2.TabIndex = 4;
			this.label2.Text = "Current Mode:";
			// 
			// BrushSize
			// 
			this.BrushSize.Location = new System.Drawing.Point(157, 49);
			this.BrushSize.Maximum = new decimal(new int[] {
			6,
			0,
			0,
			0});
			this.BrushSize.Minimum = new decimal(new int[] {
			1,
			0,
			0,
			0});
			this.BrushSize.Name = "BrushSize";
			this.BrushSize.Size = new System.Drawing.Size(38, 20);
			this.BrushSize.TabIndex = 9;
			this.BrushSize.Value = new decimal(new int[] {
			1,
			0,
			0,
			0});
			// 
			// label3
			// 
			this.label3.AutoSize = true;
			this.label3.Location = new System.Drawing.Point(93, 51);
			this.label3.Name = "label3";
			this.label3.Size = new System.Drawing.Size(58, 13);
			this.label3.TabIndex = 8;
			this.label3.Text = "Brush size:";
			this.label3.TextAlign = System.Drawing.ContentAlignment.TopRight;
			// 
			// comboIgnoreTile
			// 
			this.comboIgnoreTile.AutoCompleteMode = System.Windows.Forms.AutoCompleteMode.Append;
			this.comboIgnoreTile.AutoCompleteSource = System.Windows.Forms.AutoCompleteSource.ListItems;
			this.comboIgnoreTile.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
			this.comboIgnoreTile.FormattingEnabled = true;
			this.comboIgnoreTile.Location = new System.Drawing.Point(84, 22);
			this.comboIgnoreTile.MaxDropDownItems = 20;
			this.comboIgnoreTile.Name = "comboIgnoreTile";
			this.comboIgnoreTile.Size = new System.Drawing.Size(116, 21);
			this.comboIgnoreTile.TabIndex = 10;
			// 
			// groupBox1
			// 
			this.groupBox1.Controls.Add(this.WallBlockBrush);
			this.groupBox1.Controls.Add(this.label5);
			this.groupBox1.Controls.Add(this.BrushSize);
			this.groupBox1.Controls.Add(this.label3);
			this.groupBox1.Controls.Add(this.comboIgnoreTile);
			this.groupBox1.Location = new System.Drawing.Point(4, 120);
			this.groupBox1.Name = "groupBox1";
			this.groupBox1.Size = new System.Drawing.Size(206, 77);
			this.groupBox1.TabIndex = 12;
			this.groupBox1.TabStop = false;
			this.groupBox1.Text = "Auto Brush Options";
			// 
			// WallBlockBrush
			// 
			this.WallBlockBrush.AutoSize = true;
			this.WallBlockBrush.Location = new System.Drawing.Point(14, 50);
			this.WallBlockBrush.Name = "WallBlockBrush";
			this.WallBlockBrush.Size = new System.Drawing.Size(76, 17);
			this.WallBlockBrush.TabIndex = 13;
			this.WallBlockBrush.Text = "Wall block";
			this.WallBlockBrush.UseVisualStyleBackColor = true;
			// 
			// label5
			// 
			this.label5.Location = new System.Drawing.Point(6, 23);
			this.label5.Name = "label5";
			this.label5.Size = new System.Drawing.Size(77, 19);
			this.label5.TabIndex = 12;
			this.label5.Text = "Tile to Ignore:";
			this.label5.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
			// 
			// labelSep1
			// 
			this.labelSep1.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D;
			this.labelSep1.Location = new System.Drawing.Point(8, 52);
			this.labelSep1.Name = "labelSep1";
			this.labelSep1.Size = new System.Drawing.Size(201, 2);
			this.labelSep1.TabIndex = 17;
			// 
			// checkAutoVari
			// 
			this.checkAutoVari.AutoSize = true;
			this.checkAutoVari.CheckAlign = System.Drawing.ContentAlignment.MiddleRight;
			this.checkAutoVari.Checked = true;
			this.checkAutoVari.CheckState = System.Windows.Forms.CheckState.Checked;
			this.checkAutoVari.Location = new System.Drawing.Point(10, 89);
			this.checkAutoVari.Name = "checkAutoVari";
			this.checkAutoVari.Size = new System.Drawing.Size(91, 17);
			this.checkAutoVari.TabIndex = 2;
			this.checkAutoVari.Text = "Auto variation";
			this.checkAutoVari.UseVisualStyleBackColor = true;
			// 
			// buttonMode
			// 
			this.buttonMode.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold);
			this.buttonMode.Location = new System.Drawing.Point(100, 60);
			this.buttonMode.Name = "buttonMode";
			this.buttonMode.Size = new System.Drawing.Size(90, 23);
			this.buttonMode.TabIndex = 19;
			this.buttonMode.Text = " ";
			this.buttonMode.UseVisualStyleBackColor = true;
			// 
			// TileMakeTab
			// 
			this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
			this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
			this.Controls.Add(this.buttonMode);
			this.Controls.Add(this.labelSep1);
			this.Controls.Add(this.groupBox1);
			this.Controls.Add(this.listTileImages);
			this.Controls.Add(this.comboTileType);
			this.Controls.Add(this.label1);
			this.Controls.Add(this.checkAutoVari);
			this.Controls.Add(this.label2);
			this.Name = "TileMakeTab";
			this.Size = new System.Drawing.Size(216, 543);
			((System.ComponentModel.ISupportInitialize)(this.BrushSize)).EndInit();
			this.groupBox1.ResumeLayout(false);
			this.groupBox1.PerformLayout();
			this.ResumeLayout(false);
			this.PerformLayout();

		}
		private System.Windows.Forms.ListView listTileImages;
		private System.Windows.Forms.ComboBox comboTileType;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.Label label2;
        public System.Windows.Forms.NumericUpDown BrushSize;
        private System.Windows.Forms.Label label3;
        public System.Windows.Forms.ComboBox comboIgnoreTile;
        private System.Windows.Forms.GroupBox groupBox1;
        private System.Windows.Forms.Label label5;
        public System.Windows.Forms.CheckBox WallBlockBrush;
        private System.Windows.Forms.Label labelSep1;
        private System.Windows.Forms.CheckBox checkAutoVari;
        public SwitchModeButton buttonMode;
	}
}
