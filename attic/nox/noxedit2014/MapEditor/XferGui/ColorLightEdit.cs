/*
 * MapEditor
 * Пользователь: AngryKirC
 * Copyleft - Public Domain
 * Дата: 20.11.2014
 */
using System;
using System.Drawing;
using System.Windows.Forms;
using NoxShared.ObjDataXfer;

namespace MapEditor.XferGui
{
	/// <summary>
	/// Description of ColorLightEdit.
	/// </summary>
	public partial class ColorLightEdit : XferEditor
	{
		private InvisibleLightXfer xfer;
		
		public ColorLightEdit()
		{
			//
			// The InitializeComponent() call is required for Windows Forms designer support.
			//
			InitializeComponent();
		}
		
		public override void SetObject(NoxShared.Map.Object obj)
		{
			this.obj = obj;
			xfer = obj.GetExtraData<InvisibleLightXfer>();
		}
	}
}
