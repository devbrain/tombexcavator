namespace VideoBagTest
{
    partial class FormImportDialog
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
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(FormImportDialog));
            this.textBox1 = new System.Windows.Forms.TextBox();
            this.label1 = new System.Windows.Forms.Label();
            this.button_Exit = new System.Windows.Forms.Button();
            this.button_Start = new System.Windows.Forms.Button();
            this.label2 = new System.Windows.Forms.Label();
            this.comboBox_dataType = new System.Windows.Forms.ComboBox();
            this.textBox_Dir = new System.Windows.Forms.TextBox();
            this.button_Browse = new System.Windows.Forms.Button();
            this.SuspendLayout();
            // 
            // textBox1
            // 
            this.textBox1.Location = new System.Drawing.Point(12, 12);
            this.textBox1.Multiline = true;
            this.textBox1.Name = "textBox1";
            this.textBox1.ReadOnly = true;
            this.textBox1.ScrollBars = System.Windows.Forms.ScrollBars.Vertical;
            this.textBox1.Size = new System.Drawing.Size(356, 113);
            this.textBox1.TabIndex = 0;
            this.textBox1.Text = resources.GetString("textBox1.Text");
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(12, 153);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(76, 13);
            this.label1.TabIndex = 1;
            this.label1.Text = "Directory path:";
            // 
            // button_Exit
            // 
            this.button_Exit.Location = new System.Drawing.Point(251, 227);
            this.button_Exit.Name = "button_Exit";
            this.button_Exit.Size = new System.Drawing.Size(93, 23);
            this.button_Exit.TabIndex = 2;
            this.button_Exit.Text = "Take me out";
            this.button_Exit.UseVisualStyleBackColor = true;
            this.button_Exit.Click += new System.EventHandler(this.button_Exit_Click);
            // 
            // button_Start
            // 
            this.button_Start.Enabled = false;
            this.button_Start.Location = new System.Drawing.Point(27, 227);
            this.button_Start.Name = "button_Start";
            this.button_Start.Size = new System.Drawing.Size(166, 23);
            this.button_Start.TabIndex = 3;
            this.button_Start.Text = "Start the process";
            this.button_Start.UseVisualStyleBackColor = true;
            this.button_Start.Click += new System.EventHandler(this.button_Start_Click);
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Location = new System.Drawing.Point(12, 185);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(56, 13);
            this.label2.TabIndex = 4;
            this.label2.Text = "Data type:";
            // 
            // comboBox_dataType
            // 
            this.comboBox_dataType.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.comboBox_dataType.Enabled = false;
            this.comboBox_dataType.FormattingEnabled = true;
            this.comboBox_dataType.Items.AddRange(new object[] {
            "0//Tile",
            "1//Edge",
            "3//Sprite",
            "4//Dynamic",
            "5//TransparentSprite",
            "6//Dynamic"});
            this.comboBox_dataType.Location = new System.Drawing.Point(84, 182);
            this.comboBox_dataType.Name = "comboBox_dataType";
            this.comboBox_dataType.Size = new System.Drawing.Size(121, 21);
            this.comboBox_dataType.TabIndex = 5;
            // 
            // textBox_Dir
            // 
            this.textBox_Dir.Location = new System.Drawing.Point(94, 150);
            this.textBox_Dir.Name = "textBox_Dir";
            this.textBox_Dir.Size = new System.Drawing.Size(212, 20);
            this.textBox_Dir.TabIndex = 6;
            // 
            // button_Browse
            // 
            this.button_Browse.Location = new System.Drawing.Point(312, 148);
            this.button_Browse.Name = "button_Browse";
            this.button_Browse.Size = new System.Drawing.Size(56, 23);
            this.button_Browse.TabIndex = 7;
            this.button_Browse.Text = "Browse";
            this.button_Browse.UseVisualStyleBackColor = true;
            this.button_Browse.Click += new System.EventHandler(this.button_Browse_Click);
            // 
            // FormImportDialog
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(380, 262);
            this.ControlBox = false;
            this.Controls.Add(this.button_Browse);
            this.Controls.Add(this.textBox_Dir);
            this.Controls.Add(this.comboBox_dataType);
            this.Controls.Add(this.label2);
            this.Controls.Add(this.button_Start);
            this.Controls.Add(this.button_Exit);
            this.Controls.Add(this.label1);
            this.Controls.Add(this.textBox1);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedDialog;
            this.MaximizeBox = false;
            this.MinimizeBox = false;
            this.Name = "FormImportDialog";
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterParent;
            this.Text = "Import files to video.bag";
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.TextBox textBox1;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.Button button_Exit;
        private System.Windows.Forms.Button button_Start;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.ComboBox comboBox_dataType;
        private System.Windows.Forms.TextBox textBox_Dir;
        private System.Windows.Forms.Button button_Browse;

    }
}