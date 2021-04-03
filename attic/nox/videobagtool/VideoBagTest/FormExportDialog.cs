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
    public partial class FormExportDialog : Form
    {
        VideoBagConverter conv;

        public FormExportDialog(VideoBagConverter conv)
        {
            InitializeComponent();
            this.conv = conv;

            // Set the limits
            numericUpDown_SecStart.Maximum = conv.GetBaseFile().SectionCount;
            numericUpDown_SecEnd.Maximum = numericUpDown_SecStart.Maximum;
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

        void ToggleButtons(bool enable)
        {
            button_Start.Enabled = enable;
            button_Exit.Enabled = enable;
        }

        private unsafe void DoExport()
        {
            string path = textBox_Dir.Text;
            bool useFullNames = checkBox_OriginalNames.Checked;
            bool separateDirs = checkBox_SeparateDirs.Checked;
            bool exportOffsets = checkBox_ExportOffsets.Checked;

            int start = (int)numericUpDown_SecStart.Value;
            int end = (int)numericUpDown_SecEnd.Value;

            progressBar_Export.Minimum = start;
            progressBar_Export.Maximum = end;

            var allIndexes = conv.GetBaseFile().GetAllFileIndexes();

            for (int sectId = start; sectId <= end; sectId++)
            {
                string modpath = path + "\\";
                if (separateDirs)
                {
                    modpath += sectId + "\\";
                    Directory.CreateDirectory(modpath);
                }

                // Iterate through all indexes, and find all files belonging to that section
                for (int singleIndex = 0; singleIndex < allIndexes.Length; singleIndex++)
                {
                    var fi = allIndexes[singleIndex];

                    if (fi.SectionId == sectId) // Export that one
                    {
                        string filename = modpath;
                        if (useFullNames)
                        {
                            int li = fi.Filename.LastIndexOf('.');
                            if (li > 0)
                                filename += fi.Filename.Substring(0, li);
                            else
                                filename += fi.Filename;
                        }
                        else
                        {
                            filename += singleIndex.ToString();
                        }

                        var imageData = conv.PullByIndex(singleIndex);
                        int width = imageData.Width;
                        int height = imageData.Height;
                        if (height == 0 || width == 0) continue; // Removed image

                        var format = PixelFormat.Format32bppArgb;
                        Bitmap bit = new Bitmap(width, height, format);

                        BitmapData bd = bit.LockBits(new Rectangle(0, 0, width, height), ImageLockMode.WriteOnly, format);

                        // Unsafe copy
                        uint* ptr = (uint*)bd.Scan0.ToPointer();
                        fixed (uint* bitptr = imageData.ColorData)
                        {
                            for (int x = 0; x < imageData.ColorData.Length; x++)
                                ptr[x] = bitptr[x];
                        }

                        bit.UnlockBits(bd);

                        // Save image file
                        bit.Save(filename + ".png", ImageFormat.Png);
                        // Save offsets 
                        if (exportOffsets)
                        {
                            using (var stream = new StreamWriter(File.OpenWrite(filename + ".ndx"), Encoding.UTF8))
                            {
                                stream.WriteLine(fi.DataType);
                                stream.WriteLine(imageData.OffsX);
                                stream.WriteLine(imageData.OffsY);
                            }
                        }
                    }
                }

                progressBar_Export.Value = sectId;
                progressBar_Export.Invalidate();
            }
        }

        private void button_Start_Click(object sender, EventArgs e)
        {
            string path = textBox_Dir.Text.TrimEnd('\\');
            bool useFullNames = checkBox_OriginalNames.Checked;
            bool separateDirs = checkBox_SeparateDirs.Checked;
            bool exportOffsets = checkBox_ExportOffsets.Checked;

            if (!Directory.Exists(path))
            {
                MessageBox.Show("Specified directory does not exist, please create it beforehand.", "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
                return;
            }

            ToggleButtons(false);
            DoExport();
            ToggleButtons(true);
        }
    }
}
