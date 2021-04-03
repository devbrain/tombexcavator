/*
 * MapEditor
 * Пользователь: AngryKirC
 * Дата: 12.02.2015
 */
namespace MapEditor.newgui
{
	partial class PolygonEditor
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
			this.listBoxPolygons = new System.Windows.Forms.ListBox();
			this.buttonDown = new System.Windows.Forms.Button();
			this.buttonUp = new System.Windows.Forms.Button();
			this.groupBox1 = new System.Windows.Forms.GroupBox();
			this.label1 = new System.Windows.Forms.Label();
			this.groupBox2 = new System.Windows.Forms.GroupBox();
			this.buttonPoints = new System.Windows.Forms.Button();
			this.buttonNew = new System.Windows.Forms.Button();
			this.buttonDelete = new System.Windows.Forms.Button();
			this.buttonModify = new System.Windows.Forms.Button();
			this.buttonDone = new System.Windows.Forms.Button();
			this.groupBox1.SuspendLayout();
			this.groupBox2.SuspendLayout();
			this.SuspendLayout();
			// 
			// listBoxPolygons
			// 
			this.listBoxPolygons.FormattingEnabled = true;
			this.listBoxPolygons.Location = new System.Drawing.Point(8, 32);
			this.listBoxPolygons.Name = "listBoxPolygons";
			this.listBoxPolygons.ScrollAlwaysVisible = true;
			this.listBoxPolygons.Size = new System.Drawing.Size(136, 160);
			this.listBoxPolygons.TabIndex = 0;
			this.listBoxPolygons.DoubleClick += new System.EventHandler(this.ButtonModifyClick);
			// 
			// buttonDown
			// 
			this.buttonDown.Location = new System.Drawing.Point(64, 24);
			this.buttonDown.Name = "buttonDown";
			this.buttonDown.Size = new System.Drawing.Size(48, 23);
			this.buttonDown.TabIndex = 3;
			this.buttonDown.Text = "Down";
			this.buttonDown.UseVisualStyleBackColor = true;
			this.buttonDown.Click += new System.EventHandler(this.ButtonDownClick);
			// 
			// buttonUp
			// 
			this.buttonUp.Location = new System.Drawing.Point(8, 24);
			this.buttonUp.Name = "buttonUp";
			this.buttonUp.Size = new System.Drawing.Size(48, 23);
			this.buttonUp.TabIndex = 4;
			this.buttonUp.Text = "Up";
			this.buttonUp.UseVisualStyleBackColor = true;
			this.buttonUp.Click += new System.EventHandler(this.ButtonUpClick);
			// 
			// groupBox1
			// 
			this.groupBox1.Controls.Add(this.buttonDown);
			this.groupBox1.Controls.Add(this.buttonUp);
			this.groupBox1.Location = new System.Drawing.Point(152, 16);
			this.groupBox1.Name = "groupBox1";
			this.groupBox1.Size = new System.Drawing.Size(120, 64);
			this.groupBox1.TabIndex = 5;
			this.groupBox1.TabStop = false;
			this.groupBox1.Text = "Priority";
			// 
			// label1
			// 
			this.label1.Location = new System.Drawing.Point(8, 8);
			this.label1.Name = "label1";
			this.label1.Size = new System.Drawing.Size(100, 23);
			this.label1.TabIndex = 6;
			this.label1.Text = "All polygons";
			// 
			// groupBox2
			// 
			this.groupBox2.Controls.Add(this.buttonPoints);
			this.groupBox2.Controls.Add(this.buttonNew);
			this.groupBox2.Controls.Add(this.buttonDelete);
			this.groupBox2.Controls.Add(this.buttonModify);
			this.groupBox2.Location = new System.Drawing.Point(152, 80);
			this.groupBox2.Name = "groupBox2";
			this.groupBox2.Size = new System.Drawing.Size(120, 80);
			this.groupBox2.TabIndex = 7;
			this.groupBox2.TabStop = false;
			this.groupBox2.Text = "Operations";
			// 
			// buttonPoints
			// 
			this.buttonPoints.Location = new System.Drawing.Point(64, 16);
			this.buttonPoints.Name = "buttonPoints";
			this.buttonPoints.Size = new System.Drawing.Size(48, 23);
			this.buttonPoints.TabIndex = 3;
			this.buttonPoints.Text = "Points";
			this.buttonPoints.UseVisualStyleBackColor = true;
			this.buttonPoints.Click += new System.EventHandler(this.ButtonPointsClick);
			// 
			// buttonNew
			// 
			this.buttonNew.Location = new System.Drawing.Point(8, 48);
			this.buttonNew.Name = "buttonNew";
			this.buttonNew.Size = new System.Drawing.Size(48, 24);
			this.buttonNew.TabIndex = 2;
			this.buttonNew.Text = "New";
			this.buttonNew.UseVisualStyleBackColor = true;
			this.buttonNew.Click += new System.EventHandler(this.ButtonNewClick);
			// 
			// buttonDelete
			// 
			this.buttonDelete.Location = new System.Drawing.Point(64, 48);
			this.buttonDelete.Name = "buttonDelete";
			this.buttonDelete.Size = new System.Drawing.Size(48, 23);
			this.buttonDelete.TabIndex = 1;
			this.buttonDelete.Text = "Delete";
			this.buttonDelete.UseVisualStyleBackColor = true;
			this.buttonDelete.Click += new System.EventHandler(this.ButtonDeleteClick);
			// 
			// buttonModify
			// 
			this.buttonModify.Location = new System.Drawing.Point(8, 16);
			this.buttonModify.Name = "buttonModify";
			this.buttonModify.Size = new System.Drawing.Size(48, 23);
			this.buttonModify.TabIndex = 0;
			this.buttonModify.Text = "Modify";
			this.buttonModify.UseVisualStyleBackColor = true;
			this.buttonModify.Click += new System.EventHandler(this.ButtonModifyClick);
			// 
			// buttonDone
			// 
			this.buttonDone.Location = new System.Drawing.Point(176, 168);
			this.buttonDone.Name = "buttonDone";
			this.buttonDone.Size = new System.Drawing.Size(72, 23);
			this.buttonDone.TabIndex = 8;
			this.buttonDone.Text = "Done";
			this.buttonDone.UseVisualStyleBackColor = true;
			this.buttonDone.Click += new System.EventHandler(this.ButtonDoneClick);
			// 
			// PolygonEditor
			// 
			this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
			this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
			this.ClientSize = new System.Drawing.Size(284, 208);
			this.Controls.Add(this.buttonDone);
			this.Controls.Add(this.groupBox2);
			this.Controls.Add(this.label1);
			this.Controls.Add(this.groupBox1);
			this.Controls.Add(this.listBoxPolygons);
			this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedDialog;
			this.MaximizeBox = false;
			this.MinimizeBox = false;
			this.Name = "PolygonEditor";
			this.StartPosition = System.Windows.Forms.FormStartPosition.CenterParent;
			this.Text = "PolygonEditor";
			this.Load += new System.EventHandler(this.PolygonEditorLoad);
			this.groupBox1.ResumeLayout(false);
			this.groupBox2.ResumeLayout(false);
			this.ResumeLayout(false);
		}
		private System.Windows.Forms.Button buttonModify;
		private System.Windows.Forms.Button buttonDelete;
		private System.Windows.Forms.Button buttonNew;
		private System.Windows.Forms.Button buttonPoints;
		private System.Windows.Forms.Button buttonDone;
		private System.Windows.Forms.GroupBox groupBox2;
		private System.Windows.Forms.Label label1;
		private System.Windows.Forms.GroupBox groupBox1;
		private System.Windows.Forms.Button buttonUp;
		private System.Windows.Forms.Button buttonDown;
		private System.Windows.Forms.ListBox listBoxPolygons;
	}
}
