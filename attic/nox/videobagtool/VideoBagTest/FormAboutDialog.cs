using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;

namespace VideoBagTest
{
    public partial class FormAboutDialog : Form
    {
        public FormAboutDialog()
        {
            InitializeComponent();
        }

        private void label3_Click(object sender, EventArgs e)
        {
            label3.Text = "Developed by Real Lazy ass...";
        }
    }
}
