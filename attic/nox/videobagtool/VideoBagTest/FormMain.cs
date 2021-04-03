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
    public partial class FormMain : Form
    {
        VideoBagConverter converter = null;

        public FormMain()
        {
            InitializeComponent();
            UpdateInfo();
        }

        private void openToolStripMenuItem_Click(object sender, EventArgs e)
        {
            OpenFileDialog ofd = new OpenFileDialog();
            ofd.Filter = "IDX files (*.idx)|*.idx|All files (*.*)|*.*";
            ofd.Title = "Choose an IDX file";

            if (ofd.ShowDialog() == DialogResult.OK)
            {
                string idxPath = ofd.FileName;
                string bagPath = idxPath.Substring(0, idxPath.LastIndexOf('.')) + ".bag";

                if (!File.Exists(bagPath))
                {
                    MessageBox.Show("Unable to find the corresponding .bag file. Please choose it manually.", "Warning", MessageBoxButtons.OK, MessageBoxIcon.Warning);
                    ofd.Filter = "BAG files (*.bag)|*.bag";
                    ofd.Title = "Choose a BAG file";

                    if (ofd.ShowDialog() != DialogResult.OK)
                        return;

                    bagPath = ofd.FileName;
                }

                CloseVideoBag();
                converter = new VideoBagConverter(bagPath, idxPath);

                if (converter.Is8Bit)
                {
                    MessageBox.Show("It seems that you have loaded 8-bit version of video.bag.\r\n Please choose a palette that will be used for decoding.", "Warning", MessageBoxButtons.OK, MessageBoxIcon.Warning);
                    ofd.Filter = "PAL files (*.pal)|*.pal";
                    ofd.Title = "Choose a PAL file";

                    if (ofd.ShowDialog() != DialogResult.OK)
                        return;

                    converter.LoadPalette(ofd.FileName);
                }

                UpdateInfo();
            }
        }

        private void aboutToolStripMenuItem_Click(object sender, EventArgs e)
        {
            new FormAboutDialog().ShowDialog(this);
        }

        private void importToolStripMenuItem_Click(object sender, EventArgs e)
        {
            if (converter == null) return;
            new FormImportDialog(converter).ShowDialog(this);
        }

        private void extractToolStripMenuItem_Click(object sender, EventArgs e)
        {
            if (converter == null) return;
            new FormExportDialog(converter).ShowDialog(this);
        }

        void CloseVideoBag()
        {
            if (converter != null) converter.Close();
        }

        void UpdateInfo()
        {
            if (converter == null)
            {
                textBox_IdxInfo.Text = "No file is loaded";
                return;
            }

            var file = converter.GetBaseFile();

            string template = "IdxPath: {0}\r\nBagPath: {1}\r\n\r\nSections: {2}\r\nIndexEntries: {3}\r\n\r\nUnknown1: {4}\r\nFlags2: {5}\r\nInitialIndexes: {6}\r\n";

            textBox_IdxInfo.Text = String.Format(template, file.IdxPath, file.BagPath, file.SectionCount, file.IndexCount, file.Unknown1, file.Flags2, file.InitialIndexes);
            numericUpDown_Index.Maximum = file.IndexCount;
            numericUpDown_Index.Minimum = 0;
        }

        unsafe void UpdatePreview()
        {
            int index = (int)numericUpDown_Index.Value;
            if (index > converter.GetBaseFile().IndexCount)
                return; // Outside of range

            VideoBagConverter.NoxImageData imageData;
            try
            {
                imageData = converter.PullByIndex(index);
            }
            catch (Exception ex)
            {
                MessageBox.Show("Failed to extract or decode this image: " + ex.Message, "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
                textBox_IdxInfo.Text = "Failed: " + index.ToString();
                return;
            }

            var fileEntry = converter.GetBaseFile().PullFileIndex(index);
            // Set image info text
            string template = "Section: {0}\r\nDataType: {1}\r\nUnknown: {2}\r\nWidth: {3}\r\nHeight: {4}\r\nOffX: {5}\r\nOffY: {6}\r\nFName: {7}";
            textBox_ImgInfo.Text = String.Format(template, fileEntry.SectionId, fileEntry.DataType, fileEntry.Unknown, imageData.Width, imageData.Height, imageData.OffsX, imageData.OffsY, fileEntry.Filename);

            int width = imageData.Width;
            int height = imageData.Height;
            if (height == 0 || width == 0) return; // Removed image

            var format = PixelFormat.Format32bppArgb;
            Bitmap bit = new Bitmap(width, height, format);

            BitmapData bd = bit.LockBits(new Rectangle(0, 0, width, height), ImageLockMode.WriteOnly, format);

            // Unsafe copy
            uint* ptr = (uint*)bd.Scan0.ToPointer();
            fixed (uint* bitptr = imageData.ColorData)
            {
                for (int i = 0; i < imageData.ColorData.Length; i++)
                    ptr[i] = bitptr[i];
            }

            bit.UnlockBits(bd);

            // Set preview image
            pictureBox_Preview.Image = bit;
        }

        private void numericUpDown_Index_ValueChanged(object sender, EventArgs e)
        {
            UpdatePreview();
        }

        private void button_ExpOne_Click(object sender, EventArgs e)
        {
            int index = (int)numericUpDown_Index.Value;
            if (index >= converter.GetBaseFile().IndexCount)
                return; // Outside of range

            var image = pictureBox_Preview.Image;
            var imageNdx = converter.GetBaseFile().PullFileIndex(index);

            if (image != null)
            {
                string fname = imageNdx.Filename;
                if (fname.Contains('.'))
                    fname = fname.Substring(0, imageNdx.Filename.LastIndexOf('.'));

                fname += ".png";
                image.Save(fname, ImageFormat.Png);

                MessageBox.Show("Saved this image to current working folder with filename " + fname, "Info", MessageBoxButtons.OK, MessageBoxIcon.Information);
            }
        }
    }
}
