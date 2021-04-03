using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using System.IO;
using System.Drawing.Imaging;

using OpenNoxLibrary.Media;

namespace VideoBagTest
{
    public partial class FormImportDialog : Form
    {
        VideoBagConverter conv;
        List<string> log = new List<string>();

        public FormImportDialog(VideoBagConverter conv)
        {
            InitializeComponent();

            this.conv = conv;
            comboBox_dataType.SelectedIndex = 0;
        }

        private void button_Exit_Click(object sender, EventArgs e)
        {
            Close();
        }

        private void button_Browse_Click(object sender, EventArgs e)
        {
            var fbd = new FolderBrowserDialog();
            if (fbd.ShowDialog() == DialogResult.OK)
            {
                textBox_Dir.Text = fbd.SelectedPath;
            }
        }

        private void DoImport()
        {
            string path = textBox_Dir.Text;
            byte defaultDatatype = (byte)comboBox_dataType.SelectedIndex;
            defaultDatatype = defaultDatatype > 1 ? (byte)(defaultDatatype + 1) : defaultDatatype;

            if (defaultDatatype != 3 && defaultDatatype != 5)
            {
                MessageBox.Show("Unsupported data type. Not implemented ATM.", "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
                return;
            }

            if (!Directory.Exists(path))
            {
                MessageBox.Show("Specified directory does not exist.", "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
                return;
            }

            List<string> log = new List<string>();

            log.Add("Looking for .png files in " + path + " ...");
            string[] pngFiles = Directory.GetFiles(path, "*.png");

            int count = pngFiles.Length;
            log.Add(count.ToString() + " files found.");

            if (count == 0)
            {
                log.Add("No files to import, exiting.");
                WriteLog(path, log);
                return;
            }

            byte[][] imageData = new byte[count][];
            byte[] dataTypes = new byte[count];
            uint[] renderOffsX = new uint[count];
            uint[] renderOffsY = new uint[count];

            // Look up for .ndx files
            for (int i = 0; i < count; i++)
            {
                string f = pngFiles[i];
                if (File.Exists(path + f))
                {
                    log.Add("Found index file " + path + f);
                    string[] lines = File.ReadAllLines(path + f, Encoding.UTF8);
                    // Parse the file
                    dataTypes[i] = byte.Parse(lines[0]);
                    renderOffsX[i] = uint.Parse(lines[1]);
                    renderOffsY[i] = uint.Parse(lines[2]);
                }
                else
                {
                    dataTypes[i] = defaultDatatype;
                }
            }
            //TODO

            WriteLog(path, log);
        }

        void WriteLog(string path, List<string> contents)
        {
            File.WriteAllLines(path + "import.log", contents.ToArray());
        }

        void ToggleButtons(bool enable)
        {
            button_Start.Enabled = enable;
            button_Exit.Enabled = enable;
        }

        private void button_Start_Click(object sender, EventArgs e)
        {
            ToggleButtons(false);
            DoImport();
            ToggleButtons(true);
        }
    }
}
