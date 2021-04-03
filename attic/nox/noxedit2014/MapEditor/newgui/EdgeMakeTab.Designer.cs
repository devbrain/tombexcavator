/*
 * MapEditor
 * Пользователь: AngryKirC
 * Дата: 23.01.2015
 */
namespace MapEditor.newgui
{
	partial class EdgeMakeTab
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
            this.comboEdgeType = new System.Windows.Forms.ComboBox();
            this.label1 = new System.Windows.Forms.Label();
            this.listEdgeImages = new System.Windows.Forms.ListView();
            this.SuspendLayout();
            // 
            // comboEdgeType
            // 
            this.comboEdgeType.AutoCompleteMode = System.Windows.Forms.AutoCompleteMode.Append;
            this.comboEdgeType.AutoCompleteSource = System.Windows.Forms.AutoCompleteSource.ListItems;
            this.comboEdgeType.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.comboEdgeType.FormattingEnabled = true;
            this.comboEdgeType.Location = new System.Drawing.Point(88, 8);
            this.comboEdgeType.MaxDropDownItems = 20;
            this.comboEdgeType.Name = "comboEdgeType";
            this.comboEdgeType.Size = new System.Drawing.Size(121, 21);
            this.comboEdgeType.TabIndex = 3;
            this.comboEdgeType.SelectedIndexChanged += new System.EventHandler(this.UpdateListView);
            // 
            // label1
            // 
            this.label1.Location = new System.Drawing.Point(8, 8);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(72, 23);
            this.label1.TabIndex = 2;
            this.label1.Text = "Edge type:";
            // 
            // listEdgeImages
            // 
            this.listEdgeImages.Alignment = System.Windows.Forms.ListViewAlignment.SnapToGrid;
            this.listEdgeImages.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
            this.listEdgeImages.GridLines = true;
            this.listEdgeImages.Location = new System.Drawing.Point(4, 56);
            this.listEdgeImages.Name = "listEdgeImages";
            this.listEdgeImages.Size = new System.Drawing.Size(205, 480);
            this.listEdgeImages.TabIndex = 4;
            this.listEdgeImages.TileSize = new System.Drawing.Size(46, 46);
            this.listEdgeImages.UseCompatibleStateImageBehavior = false;
            this.listEdgeImages.VirtualMode = true;
            this.listEdgeImages.SelectedIndexChanged += new System.EventHandler(this.SelectEdgeDirection);
            // 
            // EdgeMakeTab
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.Controls.Add(this.listEdgeImages);
            this.Controls.Add(this.comboEdgeType);
            this.Controls.Add(this.label1);
            this.Name = "EdgeMakeTab";
            this.Size = new System.Drawing.Size(216, 543);
            this.Enter += new System.EventHandler(this.EdgeMakeTab_Enter);
            this.ResumeLayout(false);

		}
		private System.Windows.Forms.ListView listEdgeImages;
		private System.Windows.Forms.Label label1;
		private System.Windows.Forms.ComboBox comboEdgeType;
	}
}
