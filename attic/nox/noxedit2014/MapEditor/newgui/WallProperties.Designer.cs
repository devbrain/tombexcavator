/*
 * MapEditor
 * Пользователь: AngryKirC
 * Дата: 24.01.2015
 */
namespace MapEditor.newgui
{
	partial class WallProperties
	{
		/// <summary>
		/// Designer variable used to keep track of non-visual components.
		/// </summary>
		private System.ComponentModel.IContainer components = null;
		
		/// <summary>
		/// Disposes resources used by the form.
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
            this.buttonDone = new System.Windows.Forms.Button();
            this.groupBox1 = new System.Windows.Forms.GroupBox();
            this.comboWallState = new System.Windows.Forms.ComboBox();
            this.label3 = new System.Windows.Forms.Label();
            this.checkListFlags = new System.Windows.Forms.CheckedListBox();
            this.numericCloseDelay = new System.Windows.Forms.NumericUpDown();
            this.label2 = new System.Windows.Forms.Label();
            this.polygonGroup = new System.Windows.Forms.NumericUpDown();
            this.label1 = new System.Windows.Forms.Label();
            this.checkDestructable = new System.Windows.Forms.CheckBox();
            this.groupBox1.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.numericCloseDelay)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.polygonGroup)).BeginInit();
            this.SuspendLayout();
            // 
            // buttonDone
            // 
            this.buttonDone.Location = new System.Drawing.Point(200, 176);
            this.buttonDone.Name = "buttonDone";
            this.buttonDone.Size = new System.Drawing.Size(75, 23);
            this.buttonDone.TabIndex = 0;
            this.buttonDone.Text = "Done";
            this.buttonDone.UseVisualStyleBackColor = true;
            this.buttonDone.Click += new System.EventHandler(this.ButtonDoneClick);
            // 
            // groupBox1
            // 
            this.groupBox1.Controls.Add(this.comboWallState);
            this.groupBox1.Controls.Add(this.label3);
            this.groupBox1.Controls.Add(this.checkListFlags);
            this.groupBox1.Controls.Add(this.numericCloseDelay);
            this.groupBox1.Controls.Add(this.label2);
            this.groupBox1.Location = new System.Drawing.Point(8, 8);
            this.groupBox1.Name = "groupBox1";
            this.groupBox1.Size = new System.Drawing.Size(272, 136);
            this.groupBox1.TabIndex = 1;
            this.groupBox1.TabStop = false;
            this.groupBox1.Text = "Secret Wall";
            this.groupBox1.Enter += new System.EventHandler(this.groupBox1_Enter);
            // 
            // comboWallState
            // 
            this.comboWallState.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.comboWallState.FormattingEnabled = true;
            this.comboWallState.Items.AddRange(new object[] {
            "Closed",
            "Closing",
            "Open",
            "Opening"});
            this.comboWallState.Location = new System.Drawing.Point(24, 80);
            this.comboWallState.Name = "comboWallState";
            this.comboWallState.Size = new System.Drawing.Size(121, 21);
            this.comboWallState.TabIndex = 4;
            this.comboWallState.SelectedIndexChanged += new System.EventHandler(this.ComboWallStateSelectedIndexChanged);
            // 
            // label3
            // 
            this.label3.Location = new System.Drawing.Point(32, 48);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(112, 23);
            this.label3.TabIndex = 3;
            this.label3.Text = "Wall status && flags";
            // 
            // checkListFlags
            // 
            this.checkListFlags.FormattingEnabled = true;
            this.checkListFlags.Items.AddRange(new object[] {
            "Scripted",
            "Auto-Open",
            "Auto-Close",
            "UnkFlag8"});
            this.checkListFlags.Location = new System.Drawing.Point(160, 56);
            this.checkListFlags.Name = "checkListFlags";
            this.checkListFlags.Size = new System.Drawing.Size(104, 64);
            this.checkListFlags.TabIndex = 2;
            this.checkListFlags.ItemCheck += new System.Windows.Forms.ItemCheckEventHandler(this.CheckListFlagsItemCheck);
            this.checkListFlags.SelectedIndexChanged += new System.EventHandler(this.checkListFlags_SelectedIndexChanged);
            // 
            // numericCloseDelay
            // 
            this.numericCloseDelay.Location = new System.Drawing.Point(168, 24);
            this.numericCloseDelay.Name = "numericCloseDelay";
            this.numericCloseDelay.Size = new System.Drawing.Size(88, 20);
            this.numericCloseDelay.TabIndex = 1;
            this.numericCloseDelay.Value = new decimal(new int[] {
            1,
            0,
            0,
            0});
            this.numericCloseDelay.ValueChanged += new System.EventHandler(this.NumericCloseDelayValueChanged);
            // 
            // label2
            // 
            this.label2.Location = new System.Drawing.Point(8, 24);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(152, 23);
            this.label2.TabIndex = 0;
            this.label2.Text = "Open/close delay (seconds)";
            // 
            // polygonGroup
            // 
            this.polygonGroup.Location = new System.Drawing.Point(152, 152);
            this.polygonGroup.Maximum = new decimal(new int[] {
            255,
            0,
            0,
            0});
            this.polygonGroup.Name = "polygonGroup";
            this.polygonGroup.Size = new System.Drawing.Size(48, 20);
            this.polygonGroup.TabIndex = 2;
            this.polygonGroup.ValueChanged += new System.EventHandler(this.PolygonGroupValueChanged);
            // 
            // label1
            // 
            this.label1.Location = new System.Drawing.Point(8, 152);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(136, 23);
            this.label1.TabIndex = 3;
            this.label1.Text = "Polygon (Minimap) Group";
            // 
            // checkDestructable
            // 
            this.checkDestructable.Location = new System.Drawing.Point(8, 176);
            this.checkDestructable.Name = "checkDestructable";
            this.checkDestructable.Size = new System.Drawing.Size(160, 24);
            this.checkDestructable.TabIndex = 4;
            this.checkDestructable.Text = "Destructable (Breakable)";
            this.checkDestructable.UseVisualStyleBackColor = true;
            this.checkDestructable.CheckedChanged += new System.EventHandler(this.CheckDestructableCheckedChanged);
            // 
            // WallProperties
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(291, 208);
            this.Controls.Add(this.checkDestructable);
            this.Controls.Add(this.label1);
            this.Controls.Add(this.polygonGroup);
            this.Controls.Add(this.groupBox1);
            this.Controls.Add(this.buttonDone);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedDialog;
            this.MaximizeBox = false;
            this.MinimizeBox = false;
            this.Name = "WallProperties";
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterParent;
            this.Text = "Wall Properties";
            this.Load += new System.EventHandler(this.WallProperties_Load);
            this.groupBox1.ResumeLayout(false);
            ((System.ComponentModel.ISupportInitialize)(this.numericCloseDelay)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.polygonGroup)).EndInit();
            this.ResumeLayout(false);

		}
		private System.Windows.Forms.ComboBox comboWallState;
		private System.Windows.Forms.Label label3;
		private System.Windows.Forms.CheckedListBox checkListFlags;
		private System.Windows.Forms.Label label2;
		private System.Windows.Forms.NumericUpDown numericCloseDelay;
		private System.Windows.Forms.CheckBox checkDestructable;
		private System.Windows.Forms.Label label1;
		private System.Windows.Forms.NumericUpDown polygonGroup;
		private System.Windows.Forms.GroupBox groupBox1;
        private System.Windows.Forms.Button buttonDone;
	}
}
