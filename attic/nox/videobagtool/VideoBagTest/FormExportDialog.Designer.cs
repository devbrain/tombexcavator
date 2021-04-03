namespace VideoBagTest
{
    partial class FormExportDialog
    {
        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows Form Designer generated code

        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            this.button_Exit = new System.Windows.Forms.Button();
            this.button_Start = new System.Windows.Forms.Button();
            this.checkBox_SeparateDirs = new System.Windows.Forms.CheckBox();
            this.label1 = new System.Windows.Forms.Label();
            this.label2 = new System.Windows.Forms.Label();
            this.progressBar_Export = new System.Windows.Forms.ProgressBar();
            this.button_Browse = new System.Windows.Forms.Button();
            this.textBox_Dir = new System.Windows.Forms.TextBox();
            this.label3 = new System.Windows.Forms.Label();
            this.numericUpDown_SecStart = new System.Windows.Forms.NumericUpDown();
            this.numericUpDown_SecEnd = new System.Windows.Forms.NumericUpDown();
            this.checkBox_OriginalNames = new System.Windows.Forms.CheckBox();
            this.checkBox_ExportOffsets = new System.Windows.Forms.CheckBox();
            ((System.ComponentModel.ISupportInitialize)(this.numericUpDown_SecStart)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.numericUpDown_SecEnd)).BeginInit();
            this.SuspendLayout();
            // 
            // button_Exit
            // 
            this.button_Exit.Location = new System.Drawing.Point(250, 227);
            this.button_Exit.Name = "button_Exit";
            this.button_Exit.Size = new System.Drawing.Size(93, 23);
            this.button_Exit.TabIndex = 2;
            this.button_Exit.Text = "Take me out";
            this.button_Exit.UseVisualStyleBackColor = true;
            this.button_Exit.Click += new System.EventHandler(this.button_Exit_Click);
            // 
            // button_Start
            // 
            this.button_Start.Location = new System.Drawing.Point(27, 227);
            this.button_Start.Name = "button_Start";
            this.button_Start.Size = new System.Drawing.Size(166, 23);
            this.button_Start.TabIndex = 3;
            this.button_Start.Text = "Start the process";
            this.button_Start.UseVisualStyleBackColor = true;
            this.button_Start.Click += new System.EventHandler(this.button_Start_Click);
            // 
            // checkBox_SeparateDirs
            // 
            this.checkBox_SeparateDirs.AutoSize = true;
            this.checkBox_SeparateDirs.Location = new System.Drawing.Point(15, 69);
            this.checkBox_SeparateDirs.Name = "checkBox_SeparateDirs";
            this.checkBox_SeparateDirs.Size = new System.Drawing.Size(191, 17);
            this.checkBox_SeparateDirs.TabIndex = 4;
            this.checkBox_SeparateDirs.Text = "Separate directory for each section";
            this.checkBox_SeparateDirs.UseVisualStyleBackColor = true;
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(12, 17);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(142, 13);
            this.label1.TabIndex = 5;
            this.label1.Text = "Export sections starting from:";
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Location = new System.Drawing.Point(12, 41);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(105, 13);
            this.label2.TabIndex = 6;
            this.label2.Text = "Ending at (inclusive):";
            // 
            // progressBar_Export
            // 
            this.progressBar_Export.Location = new System.Drawing.Point(15, 184);
            this.progressBar_Export.Name = "progressBar_Export";
            this.progressBar_Export.Size = new System.Drawing.Size(353, 23);
            this.progressBar_Export.TabIndex = 7;
            // 
            // button_Browse
            // 
            this.button_Browse.Location = new System.Drawing.Point(312, 155);
            this.button_Browse.Name = "button_Browse";
            this.button_Browse.Size = new System.Drawing.Size(56, 23);
            this.button_Browse.TabIndex = 10;
            this.button_Browse.Text = "Browse";
            this.button_Browse.UseVisualStyleBackColor = true;
            this.button_Browse.Click += new System.EventHandler(this.button_Browse_Click);
            // 
            // textBox_Dir
            // 
            this.textBox_Dir.Location = new System.Drawing.Point(91, 157);
            this.textBox_Dir.Name = "textBox_Dir";
            this.textBox_Dir.Size = new System.Drawing.Size(212, 20);
            this.textBox_Dir.TabIndex = 9;
            // 
            // label3
            // 
            this.label3.AutoSize = true;
            this.label3.Location = new System.Drawing.Point(15, 159);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(64, 13);
            this.label3.TabIndex = 8;
            this.label3.Text = "Export path:";
            // 
            // numericUpDown_SecStart
            // 
            this.numericUpDown_SecStart.Location = new System.Drawing.Point(160, 15);
            this.numericUpDown_SecStart.Name = "numericUpDown_SecStart";
            this.numericUpDown_SecStart.Size = new System.Drawing.Size(120, 20);
            this.numericUpDown_SecStart.TabIndex = 11;
            // 
            // numericUpDown_SecEnd
            // 
            this.numericUpDown_SecEnd.Location = new System.Drawing.Point(160, 39);
            this.numericUpDown_SecEnd.Name = "numericUpDown_SecEnd";
            this.numericUpDown_SecEnd.Size = new System.Drawing.Size(120, 20);
            this.numericUpDown_SecEnd.TabIndex = 12;
            // 
            // checkBox_OriginalNames
            // 
            this.checkBox_OriginalNames.AutoSize = true;
            this.checkBox_OriginalNames.Location = new System.Drawing.Point(15, 92);
            this.checkBox_OriginalNames.Name = "checkBox_OriginalNames";
            this.checkBox_OriginalNames.Size = new System.Drawing.Size(196, 17);
            this.checkBox_OriginalNames.TabIndex = 13;
            this.checkBox_OriginalNames.Text = "Original filenames instead of indexes";
            this.checkBox_OriginalNames.UseVisualStyleBackColor = true;
            // 
            // checkBox_ExportOffsets
            // 
            this.checkBox_ExportOffsets.AutoSize = true;
            this.checkBox_ExportOffsets.Location = new System.Drawing.Point(15, 115);
            this.checkBox_ExportOffsets.Name = "checkBox_ExportOffsets";
            this.checkBox_ExportOffsets.Size = new System.Drawing.Size(158, 17);
            this.checkBox_ExportOffsets.TabIndex = 14;
            this.checkBox_ExportOffsets.Text = "Export Type+Render offsets";
            this.checkBox_ExportOffsets.UseVisualStyleBackColor = true;
            // 
            // FormExportDialog
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(380, 262);
            this.ControlBox = false;
            this.Controls.Add(this.checkBox_ExportOffsets);
            this.Controls.Add(this.checkBox_OriginalNames);
            this.Controls.Add(this.numericUpDown_SecEnd);
            this.Controls.Add(this.numericUpDown_SecStart);
            this.Controls.Add(this.button_Browse);
            this.Controls.Add(this.textBox_Dir);
            this.Controls.Add(this.label3);
            this.Controls.Add(this.progressBar_Export);
            this.Controls.Add(this.label2);
            this.Controls.Add(this.label1);
            this.Controls.Add(this.checkBox_SeparateDirs);
            this.Controls.Add(this.button_Start);
            this.Controls.Add(this.button_Exit);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedDialog;
            this.MaximizeBox = false;
            this.MinimizeBox = false;
            this.Name = "FormExportDialog";
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterParent;
            this.Text = "Export section(s)";
            ((System.ComponentModel.ISupportInitialize)(this.numericUpDown_SecStart)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.numericUpDown_SecEnd)).EndInit();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Button button_Exit;
        private System.Windows.Forms.Button button_Start;
        private System.Windows.Forms.CheckBox checkBox_SeparateDirs;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.ProgressBar progressBar_Export;
        private System.Windows.Forms.Button button_Browse;
        private System.Windows.Forms.TextBox textBox_Dir;
        private System.Windows.Forms.Label label3;
        private System.Windows.Forms.NumericUpDown numericUpDown_SecStart;
        private System.Windows.Forms.NumericUpDown numericUpDown_SecEnd;
        private System.Windows.Forms.CheckBox checkBox_OriginalNames;
        private System.Windows.Forms.CheckBox checkBox_ExportOffsets;

    }
}