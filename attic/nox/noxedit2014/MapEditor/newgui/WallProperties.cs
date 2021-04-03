/*
 * MapEditor
 * Пользователь: AngryKirC
 * Дата: 24.01.2015
 */
using System;
using System.Drawing;
using System.Windows.Forms;
using NoxShared;

namespace MapEditor.newgui
{
	/// <summary>
	/// GUI editor for special wall properties
	/// </summary>
	public partial class WallProperties : Form
	{
		private Map.Wall wall;
        public byte flags;
		public WallProperties()
		{
			//
			// The InitializeComponent() call is required for Windows Forms designer support.
			//
			InitializeComponent();
			
			comboWallState.SelectedIndex = 0;
		}
		
		public void SetWall(Map.Wall wall)
		{
			this.wall = wall;
			// Open/close delay
			numericCloseDelay.Value = wall.Secret_OpenWaitSeconds;
			// Flags
			if ((wall.Secret_ScanFlags & 1) == 1) checkListFlags.SetItemChecked(0, true);
			if ((wall.Secret_ScanFlags & 2) == 2) checkListFlags.SetItemChecked(1, true);
			if ((wall.Secret_ScanFlags & 4) == 4) checkListFlags.SetItemChecked(2, true);
			if ((wall.Secret_ScanFlags & 8) == 8) checkListFlags.SetItemChecked(3, true);

            // State
			if (wall.Secret_WallState > 0) comboWallState.SelectedIndex = wall.Secret_WallState - 1;
			// Destructable
			checkDestructable.Checked = wall.Destructable;
			// Minimap
			polygonGroup.Value = wall.Minimap;
		}
		
		void ButtonDoneClick(object sender, EventArgs e)
		{
            flags = 0;
            //wall.Secret_ScanFlags = 0;
            if (checkListFlags.GetItemChecked(0)) flags |= 1;
            if (checkListFlags.GetItemChecked(1)) flags |= 2;
            if (checkListFlags.GetItemChecked(2)) flags |= 4;
            if (checkListFlags.GetItemChecked(3)) flags |= 8;
            //MessageBox.Show(flags.ToString());
            wall.Secret_ScanFlags = flags;

			DialogResult = DialogResult.OK;
			Close();
		}
		
		void CheckDestructableCheckedChanged(object sender, EventArgs e)
		{
			wall.Destructable = checkDestructable.Checked;
		}
		
		void ComboWallStateSelectedIndexChanged(object sender, EventArgs e)
		{
			if (wall == null) return;
			wall.Secret_WallState = (byte) (comboWallState.SelectedIndex + 1);
		}
		
		void NumericCloseDelayValueChanged(object sender, EventArgs e)
		{
			wall.Secret_OpenWaitSeconds = (int) numericCloseDelay.Value;
		}
		
		void PolygonGroupValueChanged(object sender, EventArgs e)
		{
			wall.Minimap = (byte) polygonGroup.Value;
		}
		
		void CheckListFlagsItemCheck(object sender, ItemCheckEventArgs e)
		{
            /*
            flags = wall.Secret_ScanFlags;
			if (e.Index == 0 && e.NewValue == CheckState.Checked) flags |= 1;
			if (e.Index == 1 && e.NewValue == CheckState.Checked) flags |= 2;
			if (e.Index == 2 && e.NewValue == CheckState.Checked) flags |= 4;
			if (e.Index == 3 && e.NewValue == CheckState.Checked) flags |= 8;
			wall.Secret_ScanFlags = flags;
            */
           // if (e.Index == 0 && e.NewValue == CheckState.Checked) 
             //   


        }

        private void checkListFlags_SelectedIndexChanged(object sender, EventArgs e)
        {

        }

        private void WallProperties_Load(object sender, EventArgs e)
        {

        }

        private void groupBox1_Enter(object sender, EventArgs e)
        {

        }

        

        
	}
}
