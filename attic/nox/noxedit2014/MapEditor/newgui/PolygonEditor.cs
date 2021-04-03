/*
 * MapEditor
 * Пользователь: AngryKirC
 * Дата: 12.02.2015
 */
using System;
using System.Drawing;
using System.Windows.Forms;
using MapEditor.MapInt;
using NoxShared;

namespace MapEditor.newgui
{
	/// <summary>
	/// Description of PolygonEditor.
	/// </summary>
	public partial class PolygonEditor : Form
	{
		private Map subjMap
		{
			get
			{
				return MapInterface.TheMap;
			}
		}
		private PolygonDialog polygonDlg = new PolygonDialog();
		public Map.Polygon SelectedPolygon = null;
		
		public PolygonEditor()
		{
			//
			// The InitializeComponent() call is required for Windows Forms designer support.
			//
			InitializeComponent();
		}
		
		private void UpdatePolygonList()
		{
			int oldIndex = listBoxPolygons.SelectedIndex;
			int oldCount = listBoxPolygons.Items.Count;
			// Update items
			listBoxPolygons.Items.Clear();
			foreach (Map.Polygon p in subjMap.Polygons)
			{
				listBoxPolygons.Items.Add(p.Name);
			}
			// Restore selection
			if (oldIndex >= 0)
			{
				int index = listBoxPolygons.Items.Count - (oldCount - oldIndex);
				listBoxPolygons.SelectedIndex = index;
			}
		}
		
		void ButtonModifyClick(object sender, EventArgs e)
		{
			if (listBoxPolygons.SelectedIndex < 0) return;
			
			polygonDlg.Polygon = (Map.Polygon) subjMap.Polygons[listBoxPolygons.SelectedIndex];
			polygonDlg.ShowDialog();
			subjMap.Polygons[listBoxPolygons.SelectedIndex] = polygonDlg.Polygon;
			UpdatePolygonList();
		}
		
		void ButtonNewClick(object sender, EventArgs e)
		{
			polygonDlg.Polygon = null;
			if (polygonDlg.ShowDialog() == DialogResult.OK && polygonDlg.Polygon != null)
			{
				subjMap.Polygons.Insert(0, polygonDlg.Polygon);
				UpdatePolygonList();
			}
		}
		
		void ButtonDeleteClick(object sender, EventArgs e)
		{
			if (listBoxPolygons.SelectedIndex < 0) return;
			
			// Fix logic
			Map.Polygon poly = (Map.Polygon) subjMap.Polygons[listBoxPolygons.SelectedIndex];
			if (poly == SelectedPolygon) SelectedPolygon = null;
			
			subjMap.Polygons.RemoveAt(listBoxPolygons.SelectedIndex);
			UpdatePolygonList();
		}
		
		void PolygonEditorLoad(object sender, EventArgs e)
		{
			if (subjMap != null)
			{
				UpdatePolygonList();
			}
		}
		
		void ButtonDoneClick(object sender, EventArgs e)
		{
			this.DialogResult = DialogResult.OK;
			this.Close();
		}
		
		void ButtonPointsClick(object sender, EventArgs e)
		{
			if (listBoxPolygons.SelectedIndex < 0) return;
			
			// update mapinterface
			MapInterface.CurrentMode = MapInt.EditMode.POLYGON_RESHAPE;
			SelectedPolygon = (Map.Polygon) subjMap.Polygons[listBoxPolygons.SelectedIndex];
		}
		
		void ButtonUpClick(object sender, EventArgs e)
		{
			int index = listBoxPolygons.SelectedIndex;
			if (index < 0) return;
			
			object poly = subjMap.Polygons[index];
			subjMap.Polygons.RemoveAt(index);
			// Decrement index
			if (index > 0) { index = index - 1; }
			subjMap.Polygons.Insert(index, poly);
			listBoxPolygons.SelectedIndex = index;
			UpdatePolygonList();
		}
		
		void ButtonDownClick(object sender, EventArgs e)
		{
			int index = listBoxPolygons.SelectedIndex;
			if (index < 0) return;
			
			object poly = subjMap.Polygons[index];
			subjMap.Polygons.RemoveAt(index);
			// Increment index
			if (index < subjMap.Polygons.Count) { index = index + 1; }
			subjMap.Polygons.Insert(index, poly);
			listBoxPolygons.SelectedIndex = index;
			UpdatePolygonList();
		}
	}
}
