/*
 * MapEditor
 * Пользователь: AngryKirC
 * Дата: 15.01.2015
 */
namespace MapEditor.newgui
{
	partial class WallMakeTab
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
			this.comboWallSet = new System.Windows.Forms.ComboBox();
			this.label2 = new System.Windows.Forms.Label();
			this.numMapGroup = new System.Windows.Forms.NumericUpDown();
			this.label3 = new System.Windows.Forms.Label();
			this.numWallVari = new System.Windows.Forms.NumericUpDown();
			this.wallBtnContainer = new System.Windows.Forms.Panel();
			this.wallBtn13 = new System.Windows.Forms.Button();
			this.wallBtn11 = new System.Windows.Forms.Button();
			this.wallBtn7 = new System.Windows.Forms.Button();
			this.wallBtn12 = new System.Windows.Forms.Button();
			this.wallBtn10 = new System.Windows.Forms.Button();
			this.wallBtn6 = new System.Windows.Forms.Button();
			this.wallBtn3 = new System.Windows.Forms.Button();
			this.wallBtn9 = new System.Windows.Forms.Button();
			this.wallBtn5 = new System.Windows.Forms.Button();
			this.wallBtn2 = new System.Windows.Forms.Button();
			this.wallBtn8 = new System.Windows.Forms.Button();
			this.wallBtn4 = new System.Windows.Forms.Button();
			this.wallBtn1 = new System.Windows.Forms.Button();
			this.numWallVariMax = new System.Windows.Forms.NumericUpDown();
			this.autovari = new System.Windows.Forms.CheckBox();
			this.label4 = new System.Windows.Forms.Label();
			this.label5 = new System.Windows.Forms.Label();
			this.labelSep1 = new System.Windows.Forms.Label();
			this.labelSep2 = new System.Windows.Forms.Label();
			this.buttonMode = new SwitchModeButton();
			((System.ComponentModel.ISupportInitialize)(this.numMapGroup)).BeginInit();
			((System.ComponentModel.ISupportInitialize)(this.numWallVari)).BeginInit();
			this.wallBtnContainer.SuspendLayout();
			((System.ComponentModel.ISupportInitialize)(this.numWallVariMax)).BeginInit();
			this.SuspendLayout();
			// 
			// label1
			// 
			this.label1.Location = new System.Drawing.Point(10, 21);
			this.label1.Name = "label1";
			this.label1.Size = new System.Drawing.Size(80, 23);
			this.label1.TabIndex = 0;
			this.label1.Text = "Wall type:";
			// 
			// comboWallSet
			// 
			this.comboWallSet.AutoCompleteMode = System.Windows.Forms.AutoCompleteMode.Append;
			this.comboWallSet.AutoCompleteSource = System.Windows.Forms.AutoCompleteSource.ListItems;
			this.comboWallSet.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
			this.comboWallSet.FormattingEnabled = true;
			this.comboWallSet.Location = new System.Drawing.Point(89, 15);
			this.comboWallSet.MaxDropDownItems = 16;
			this.comboWallSet.Name = "comboWallSet";
			this.comboWallSet.Size = new System.Drawing.Size(112, 21);
			this.comboWallSet.TabIndex = 1;
			this.comboWallSet.SelectedIndexChanged += new System.EventHandler(this.UpdateBtnImages);
			this.comboWallSet.MouseDown += new System.Windows.Forms.MouseEventHandler(this.WallSetFirst);
			// 
			// label2
			// 
			this.label2.Location = new System.Drawing.Point(3, 120);
			this.label2.Name = "label2";
			this.label2.Size = new System.Drawing.Size(87, 18);
			this.label2.TabIndex = 2;
			this.label2.Text = "Minimap group:";
			this.label2.TextAlign = System.Drawing.ContentAlignment.TopRight;
			// 
			// numMapGroup
			// 
			this.numMapGroup.Location = new System.Drawing.Point(96, 118);
			this.numMapGroup.Maximum = new decimal(new int[] {
			255,
			0,
			0,
			0});
			this.numMapGroup.Name = "numMapGroup";
			this.numMapGroup.Size = new System.Drawing.Size(46, 20);
			this.numMapGroup.TabIndex = 3;
			// 
			// label3
			// 
			this.label3.Location = new System.Drawing.Point(2, 145);
			this.label3.Name = "label3";
			this.label3.Size = new System.Drawing.Size(58, 18);
			this.label3.TabIndex = 6;
			this.label3.Text = "Variation:";
			this.label3.TextAlign = System.Drawing.ContentAlignment.TopRight;
			// 
			// numWallVari
			// 
			this.numWallVari.Location = new System.Drawing.Point(66, 143);
			this.numWallVari.Maximum = new decimal(new int[] {
			14,
			0,
			0,
			0});
			this.numWallVari.Name = "numWallVari";
			this.numWallVari.Size = new System.Drawing.Size(47, 20);
			this.numWallVari.TabIndex = 7;
			this.numWallVari.ValueChanged += new System.EventHandler(this.UpdateBtnImages);
			// 
			// wallBtnContainer
			// 
			this.wallBtnContainer.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
			this.wallBtnContainer.Controls.Add(this.wallBtn13);
			this.wallBtnContainer.Controls.Add(this.wallBtn11);
			this.wallBtnContainer.Controls.Add(this.wallBtn7);
			this.wallBtnContainer.Controls.Add(this.wallBtn12);
			this.wallBtnContainer.Controls.Add(this.wallBtn10);
			this.wallBtnContainer.Controls.Add(this.wallBtn6);
			this.wallBtnContainer.Controls.Add(this.wallBtn3);
			this.wallBtnContainer.Controls.Add(this.wallBtn9);
			this.wallBtnContainer.Controls.Add(this.wallBtn5);
			this.wallBtnContainer.Controls.Add(this.wallBtn2);
			this.wallBtnContainer.Controls.Add(this.wallBtn8);
			this.wallBtnContainer.Controls.Add(this.wallBtn4);
			this.wallBtnContainer.Controls.Add(this.wallBtn1);
			this.wallBtnContainer.Location = new System.Drawing.Point(0, 174);
			this.wallBtnContainer.Name = "wallBtnContainer";
			this.wallBtnContainer.Size = new System.Drawing.Size(208, 418);
			this.wallBtnContainer.TabIndex = 8;
			// 
			// wallBtn13
			// 
			this.wallBtn13.BackColor = System.Drawing.Color.White;
			this.wallBtn13.FlatAppearance.MouseDownBackColor = System.Drawing.Color.LightGray;
			this.wallBtn13.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
			this.wallBtn13.Location = new System.Drawing.Point(8, 328);
			this.wallBtn13.Name = "wallBtn13";
			this.wallBtn13.Size = new System.Drawing.Size(64, 80);
			this.wallBtn13.TabIndex = 12;
			this.wallBtn13.UseVisualStyleBackColor = false;
			// 
			// wallBtn11
			// 
			this.wallBtn11.BackColor = System.Drawing.Color.White;
			this.wallBtn11.FlatAppearance.MouseDownBackColor = System.Drawing.Color.LightGray;
			this.wallBtn11.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
			this.wallBtn11.Location = new System.Drawing.Point(136, 248);
			this.wallBtn11.Name = "wallBtn11";
			this.wallBtn11.Size = new System.Drawing.Size(64, 80);
			this.wallBtn11.TabIndex = 11;
			this.wallBtn11.UseVisualStyleBackColor = false;
			// 
			// wallBtn7
			// 
			this.wallBtn7.BackColor = System.Drawing.Color.White;
			this.wallBtn7.FlatAppearance.MouseDownBackColor = System.Drawing.Color.LightGray;
			this.wallBtn7.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
			this.wallBtn7.Location = new System.Drawing.Point(72, 248);
			this.wallBtn7.Name = "wallBtn7";
			this.wallBtn7.Size = new System.Drawing.Size(64, 80);
			this.wallBtn7.TabIndex = 10;
			this.wallBtn7.UseVisualStyleBackColor = false;
			// 
			// wallBtn12
			// 
			this.wallBtn12.BackColor = System.Drawing.Color.White;
			this.wallBtn12.FlatAppearance.MouseDownBackColor = System.Drawing.Color.LightGray;
			this.wallBtn12.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
			this.wallBtn12.Location = new System.Drawing.Point(8, 248);
			this.wallBtn12.Name = "wallBtn12";
			this.wallBtn12.Size = new System.Drawing.Size(64, 80);
			this.wallBtn12.TabIndex = 9;
			this.wallBtn12.UseVisualStyleBackColor = false;
			// 
			// wallBtn10
			// 
			this.wallBtn10.BackColor = System.Drawing.Color.White;
			this.wallBtn10.FlatAppearance.MouseDownBackColor = System.Drawing.Color.LightGray;
			this.wallBtn10.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
			this.wallBtn10.Location = new System.Drawing.Point(136, 168);
			this.wallBtn10.Name = "wallBtn10";
			this.wallBtn10.Size = new System.Drawing.Size(64, 80);
			this.wallBtn10.TabIndex = 8;
			this.wallBtn10.UseVisualStyleBackColor = false;
			// 
			// wallBtn6
			// 
			this.wallBtn6.BackColor = System.Drawing.Color.White;
			this.wallBtn6.FlatAppearance.MouseDownBackColor = System.Drawing.Color.LightGray;
			this.wallBtn6.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
			this.wallBtn6.Location = new System.Drawing.Point(72, 168);
			this.wallBtn6.Name = "wallBtn6";
			this.wallBtn6.Size = new System.Drawing.Size(64, 80);
			this.wallBtn6.TabIndex = 7;
			this.wallBtn6.UseVisualStyleBackColor = false;
			// 
			// wallBtn3
			// 
			this.wallBtn3.BackColor = System.Drawing.Color.White;
			this.wallBtn3.FlatAppearance.MouseDownBackColor = System.Drawing.Color.LightGray;
			this.wallBtn3.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
			this.wallBtn3.Location = new System.Drawing.Point(8, 168);
			this.wallBtn3.Name = "wallBtn3";
			this.wallBtn3.Size = new System.Drawing.Size(64, 80);
			this.wallBtn3.TabIndex = 6;
			this.wallBtn3.UseVisualStyleBackColor = false;
			// 
			// wallBtn9
			// 
			this.wallBtn9.BackColor = System.Drawing.Color.White;
			this.wallBtn9.FlatAppearance.MouseDownBackColor = System.Drawing.Color.LightGray;
			this.wallBtn9.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
			this.wallBtn9.Location = new System.Drawing.Point(136, 88);
			this.wallBtn9.Name = "wallBtn9";
			this.wallBtn9.Size = new System.Drawing.Size(64, 80);
			this.wallBtn9.TabIndex = 5;
			this.wallBtn9.UseVisualStyleBackColor = false;
			// 
			// wallBtn5
			// 
			this.wallBtn5.BackColor = System.Drawing.Color.White;
			this.wallBtn5.FlatAppearance.MouseDownBackColor = System.Drawing.Color.LightGray;
			this.wallBtn5.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
			this.wallBtn5.Location = new System.Drawing.Point(72, 88);
			this.wallBtn5.Name = "wallBtn5";
			this.wallBtn5.Size = new System.Drawing.Size(64, 80);
			this.wallBtn5.TabIndex = 4;
			this.wallBtn5.UseVisualStyleBackColor = false;
			// 
			// wallBtn2
			// 
			this.wallBtn2.BackColor = System.Drawing.Color.White;
			this.wallBtn2.FlatAppearance.MouseDownBackColor = System.Drawing.Color.LightGray;
			this.wallBtn2.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
			this.wallBtn2.Location = new System.Drawing.Point(8, 88);
			this.wallBtn2.Name = "wallBtn2";
			this.wallBtn2.Size = new System.Drawing.Size(64, 80);
			this.wallBtn2.TabIndex = 3;
			this.wallBtn2.UseVisualStyleBackColor = false;
			// 
			// wallBtn8
			// 
			this.wallBtn8.BackColor = System.Drawing.Color.White;
			this.wallBtn8.FlatAppearance.MouseDownBackColor = System.Drawing.Color.LightGray;
			this.wallBtn8.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
			this.wallBtn8.Location = new System.Drawing.Point(136, 8);
			this.wallBtn8.Name = "wallBtn8";
			this.wallBtn8.Size = new System.Drawing.Size(64, 80);
			this.wallBtn8.TabIndex = 2;
			this.wallBtn8.UseVisualStyleBackColor = false;
			// 
			// wallBtn4
			// 
			this.wallBtn4.BackColor = System.Drawing.Color.White;
			this.wallBtn4.FlatAppearance.MouseDownBackColor = System.Drawing.Color.LightGray;
			this.wallBtn4.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
			this.wallBtn4.Location = new System.Drawing.Point(72, 8);
			this.wallBtn4.Name = "wallBtn4";
			this.wallBtn4.Size = new System.Drawing.Size(64, 80);
			this.wallBtn4.TabIndex = 1;
			this.wallBtn4.UseVisualStyleBackColor = false;
			// 
			// wallBtn1
			// 
			this.wallBtn1.BackColor = System.Drawing.Color.White;
			this.wallBtn1.FlatAppearance.MouseDownBackColor = System.Drawing.Color.LightGray;
			this.wallBtn1.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
			this.wallBtn1.Location = new System.Drawing.Point(8, 8);
			this.wallBtn1.Name = "wallBtn1";
			this.wallBtn1.Size = new System.Drawing.Size(64, 80);
			this.wallBtn1.TabIndex = 0;
			this.wallBtn1.UseVisualStyleBackColor = false;
			// 
			// numWallVariMax
			// 
			this.numWallVariMax.Location = new System.Drawing.Point(154, 88);
			this.numWallVariMax.Maximum = new decimal(new int[] {
			14,
			0,
			0,
			0});
			this.numWallVariMax.Name = "numWallVariMax";
			this.numWallVariMax.Size = new System.Drawing.Size(47, 20);
			this.numWallVariMax.TabIndex = 10;
			this.numWallVariMax.ValueChanged += new System.EventHandler(this.numWallVariMax_ValueChanged);
			// 
			// autovari
			// 
			this.autovari.AutoSize = true;
			this.autovari.BackColor = System.Drawing.Color.Transparent;
			this.autovari.CheckAlign = System.Drawing.ContentAlignment.MiddleRight;
			this.autovari.Checked = true;
			this.autovari.CheckState = System.Windows.Forms.CheckState.Checked;
			this.autovari.Location = new System.Drawing.Point(10, 89);
			this.autovari.Name = "autovari";
			this.autovari.Size = new System.Drawing.Size(91, 17);
			this.autovari.TabIndex = 11;
			this.autovari.Text = "Auto variation";
			this.autovari.UseVisualStyleBackColor = false;
			// 
			// label4
			// 
			this.label4.AutoSize = true;
			this.label4.Location = new System.Drawing.Point(111, 91);
			this.label4.Name = "label4";
			this.label4.Size = new System.Drawing.Size(30, 13);
			this.label4.TabIndex = 12;
			this.label4.Text = "Max:";
			this.label4.TextAlign = System.Drawing.ContentAlignment.TopRight;
			// 
			// label5
			// 
			this.label5.Location = new System.Drawing.Point(11, 66);
			this.label5.Name = "label5";
			this.label5.Size = new System.Drawing.Size(79, 20);
			this.label5.TabIndex = 13;
			this.label5.Text = "Current Mode:";
			// 
			// labelSep1
			// 
			this.labelSep1.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D;
			this.labelSep1.Location = new System.Drawing.Point(8, 52);
			this.labelSep1.Name = "labelSep1";
			this.labelSep1.Size = new System.Drawing.Size(201, 2);
			this.labelSep1.TabIndex = 16;
			// 
			// labelSep2
			// 
			this.labelSep2.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D;
			this.labelSep2.Location = new System.Drawing.Point(8, 112);
			this.labelSep2.Name = "labelSep2";
			this.labelSep2.Size = new System.Drawing.Size(201, 2);
			this.labelSep2.TabIndex = 17;
			// 
			// buttonMode
			// 
			this.buttonMode.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold);
			this.buttonMode.Location = new System.Drawing.Point(100, 60);
			this.buttonMode.Name = "buttonMode";
			this.buttonMode.Size = new System.Drawing.Size(90, 23);
			this.buttonMode.TabIndex = 18;
			this.buttonMode.Text = " ";
			this.buttonMode.UseVisualStyleBackColor = true;
			// 
			// WallMakeTab
			// 
			this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
			this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
			this.Controls.Add(this.buttonMode);
			this.Controls.Add(this.labelSep2);
			this.Controls.Add(this.labelSep1);
			this.Controls.Add(this.label5);
			this.Controls.Add(this.numMapGroup);
			this.Controls.Add(this.numWallVari);
			this.Controls.Add(this.numWallVariMax);
			this.Controls.Add(this.label4);
			this.Controls.Add(this.autovari);
			this.Controls.Add(this.wallBtnContainer);
			this.Controls.Add(this.label3);
			this.Controls.Add(this.label2);
			this.Controls.Add(this.comboWallSet);
			this.Controls.Add(this.label1);
			this.Name = "WallMakeTab";
			this.Size = new System.Drawing.Size(216, 595);
			this.Load += new System.EventHandler(this.WallMakeTab_Load);
			((System.ComponentModel.ISupportInitialize)(this.numMapGroup)).EndInit();
			((System.ComponentModel.ISupportInitialize)(this.numWallVari)).EndInit();
			this.wallBtnContainer.ResumeLayout(false);
			((System.ComponentModel.ISupportInitialize)(this.numWallVariMax)).EndInit();
			this.ResumeLayout(false);
			this.PerformLayout();

		}
		private System.Windows.Forms.Button wallBtn13;
		private System.Windows.Forms.Button wallBtn12;
		private System.Windows.Forms.Button wallBtn7;
		private System.Windows.Forms.Button wallBtn11;
		private System.Windows.Forms.Button wallBtn1;
		private System.Windows.Forms.Button wallBtn4;
		private System.Windows.Forms.Button wallBtn8;
		private System.Windows.Forms.Button wallBtn2;
		private System.Windows.Forms.Button wallBtn5;
		private System.Windows.Forms.Button wallBtn9;
		private System.Windows.Forms.Button wallBtn3;
		private System.Windows.Forms.Button wallBtn6;
		private System.Windows.Forms.Button wallBtn10;
		private System.Windows.Forms.Panel wallBtnContainer;
		public System.Windows.Forms.NumericUpDown numWallVari;
		private System.Windows.Forms.Label label3;
		private System.Windows.Forms.NumericUpDown numMapGroup;
		private System.Windows.Forms.Label label2;
		public System.Windows.Forms.ComboBox comboWallSet;
		private System.Windows.Forms.Label label1;
        public System.Windows.Forms.NumericUpDown numWallVariMax;
        private System.Windows.Forms.CheckBox autovari;
        private System.Windows.Forms.Label label4;
        private System.Windows.Forms.Label label5;
        private System.Windows.Forms.Label labelSep1;
        private System.Windows.Forms.Label labelSep2;
        public SwitchModeButton buttonMode;
	}
}
