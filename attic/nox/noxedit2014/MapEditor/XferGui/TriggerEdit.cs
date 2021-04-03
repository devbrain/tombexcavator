/*
 * MapEditor
 * Пользователь: AngryKirC
 * Copyleft - Public Domain
 * Дата: 09.11.2014
 */
using System;
using System.Drawing;
using System.Windows.Forms;
using NoxShared;
using NoxShared.ObjDataXfer;

namespace MapEditor.XferGui
{
	/// <summary>
	/// Description of TriggerEdit.
	/// </summary>
	public partial class TriggerEdit : XferEditor
	{
		private TriggerXfer xfer;
		
		public TriggerEdit()
		{
			//
			// The InitializeComponent() call is required for Windows Forms designer support.
			//
			InitializeComponent();
		}
		
		public override void SetObject(Map.Object obj)
		{
			this.obj = obj;
			// читаем Xfer
			xfer = obj.GetExtraData<TriggerXfer>();
			scriptActivated.Text = xfer.ScriptOnPressed;
			scriptReleased.Text = xfer.ScriptOnReleased;
			scriptCollided.Text = xfer.ScriptOnCollided;
			// единственный способ отличить PressurePlate/Trigger от Button/Lever
			if (ThingDb.Things[obj.Name].ExtentType != "BOX")
			{
				groupBoxArea.Enabled = false;
			}
			else
			{
				sizeX.Value = xfer.SizeX;
				sizeY.Value = xfer.SizeY;
				plateEdgeColor.BackColor = xfer.EdgeColor;
			}
		}
		
		public override Map.Object GetObject()
		{
			// записываем все изменения
			xfer.ScriptOnPressed = scriptActivated.Text;
			xfer.ScriptOnReleased = scriptReleased.Text;
			xfer.ScriptOnCollided = scriptCollided.Text;
			xfer.SizeX = (int) sizeX.Value;
			xfer.SizeY = (int) sizeY.Value;
			xfer.EdgeColor = plateEdgeColor.BackColor;
			return obj;
		}
		
		public override void SetDefaultData(Map.Object obj)
		{
			obj.NewDefaultExtraData();
			xfer = obj.GetExtraData<TriggerXfer>();
			if (ThingDb.Things[obj.Name].ExtentType != "BOX")
			{
				xfer.BackColor = Color.Black;
				xfer.EdgeColor = Color.Black;
			}
		}
		
		void ButtonOKClick(object sender, EventArgs e)
		{
			DialogResult = DialogResult.OK;
			Close();
		}
		
		void PlateEdgeColorClick(object sender, EventArgs e)
		{
			ColorDialog colorDlg = new ColorDialog();
			if (colorDlg.ShowDialog() == DialogResult.OK)
			{
				plateEdgeColor.BackColor = colorDlg.Color;
			}
		}
	}
}
