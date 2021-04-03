/*
 * MapEditor
 * Пользователь: AngryKirC
 * Дата: 15.01.2015
 */
using System;
using System.Drawing;
using System.Windows.Forms;
using System.Collections.Generic;
using NoxShared;
using MapEditor.MapInt;
using MapEditor.videobag;

namespace MapEditor.newgui
{
	/// <summary>
	/// Wall creation GUI
	/// </summary>
	public partial class WallMakeTab : UserControl
	{
		private MapView mapView;
		private VideoBagCachedProvider videoBag = null;
		public Button[] WallSelectButtons;
		public int wallFacing = 0;

		public int SelectedWallFacing
		{
			get
			{
				return wallFacing;
			}
			set
			{
				if (value > 10) wallFacing = 0;
				else if (value < 0) wallFacing = 10;
				else wallFacing = value;				
			}
		}


		public int MinimapGroup
		{
			get
			{
				return (int) numMapGroup.Value;
			}
		}
		private bool wallWindowed = false;
		private int blackWallIndex = 0;
		private List<string> sortedWallNames;
		
		private static Color btnColorBadWall = Color.LightGray;
		private static Color btnColorGoodWall = Color.White;
			
		public WallMakeTab()
		{
			//
			// The InitializeComponent() call is required for Windows Forms designer support.
			//
			InitializeComponent();
			//
			numMapGroup.Value = 100;
			blackWallIndex = ThingDb.WallNames.IndexOf("BlackWall");
			// названия стен сортируем по алфавиту
			sortedWallNames = new List<string>(ThingDb.WallNames.ToArray());
			sortedWallNames.Sort();
			// кнопочки в массив
			WallSelectButtons = new Button[] { wallBtn1, wallBtn2, wallBtn3, wallBtn4, wallBtn5, wallBtn6, wallBtn7, wallBtn8, wallBtn9, wallBtn10, wallBtn11, wallBtn12, wallBtn13 };
			foreach (Button b in this.WallSelectButtons)
			{
				b.BackgroundImageLayout = ImageLayout.Center;
				b.Enabled = false;
				b.Click += new EventHandler(WallBtnClicked);
			}
			// setup modes
			buttonMode.SetStates(new EditMode[] { EditMode.WALL_PLACE, EditMode.WALL_BRUSH, EditMode.WALL_CHANGE });
		}
		
		public void SetMapView(MapView view)
		{
			mapView = view;
			// provide videobag access
			videoBag = mapView.MapRenderer.VideoBag;
			// update wall list
			comboWallSet.Items.AddRange(sortedWallNames.ToArray());
           
            comboWallSet.SelectedIndex = 0;
		}

		/// <summary>
		/// Button has been pressed, alter walltype
		/// </summary>
		void WallBtnClicked(object sender, EventArgs e)
		{
			wallFacing = Array.IndexOf(WallSelectButtons, sender);
			wallWindowed = false;
			if (wallFacing > 10)
			{
				// стены с окном
				wallFacing -= 11; 
				wallWindowed = true;
			}
			if (wallFacing < 0) wallFacing = 0;
			// update mapinterface
			//mapView.GetMapInt().WallSetData(GetSelWallTypeIndex(), (byte)MinimapGroup, (byte)numWallVari.Value, (byte)wallFacing, checkBreakableWall.Checked, wallWindowed);
		}
		
		public byte GetSelWallTypeIndex()
		{
            int selectedIndex = comboWallSet.SelectedIndex;
            
            string wallName = removeSpace(comboWallSet.Items[selectedIndex].ToString());
			int index = ThingDb.WallNames.IndexOf(wallName);
            
			if (index > 0) return (byte) index;
			return 0;
		}

		
		/// <summary>
		/// Создаем новую стену, в соответствии с тем что указал пользователь
		/// </summary>
		public Map.Wall NewWall(Point location, bool fake = false)
		{
            byte material = GetSelWallTypeIndex();
            Map.Wall.WallFacing facing = (Map.Wall.WallFacing)wallFacing;

            Map.Wall wall = new Map.Wall(location, facing, material, (byte)MinimapGroup, (byte)numWallVari.Value);
            
            //wall.Destructable = checkBreakableWall.Checked;
            wall.Window = wallWindowed;
           
            // generate random variation
            if (autovari.Checked && !fake)
            {
                if (wall.Window) return wall;
                if ((int)wall.Facing > 1) return wall;
             
                Random rnd = new Random(location.Y + location.Y + (int)DateTime.Now.Ticks);
                int randvar = rnd.Next((int)numWallVari.Value, (int)numWallVariMax.Value + 1);
                byte rndvari = Convert.ToByte(randvar);

                wall.Variation = (byte)rnd.Next((int)numWallVari.Value, (int)numWallVariMax.Value + 1);
                if (wall.Window)
                    wall.Variation = 0;

                if ((int)wall.Facing == 2) wall.Variation = 0;
            }

			return wall;
		}
		
        public string removeSpace(string spaceChar)
        {
            string temp = spaceChar.Substring(0, 1);

            if (temp.IndexOf("*") != -1)
            {

                return spaceChar.Substring(1, spaceChar.Length - 1);
            }
            else
                return spaceChar;
        }

      	public void findWallInList(string data)
      	{
			for (int i = 0; i <= comboWallSet.Items.Count; i++)
			{
			    if (removeSpace(comboWallSet.Items[i].ToString()) == data)
			    {
			        comboWallSet.SelectedIndex = i; 
			        break;
			    } 
			}
		}
       
		void UpdateBtnImages(object sender, EventArgs e)
		{
            if (videoBag == null) return;
            
            if (numWallVariMax.Value < numWallVari.Value)
                numWallVariMax.Value = numWallVari.Value;

			ThingDb.Wall wall = ThingDb.Walls[GetSelWallTypeIndex()];
			// в движке Нокса зачем-то так
			int vari = (int) numWallVari.Value * 2;
			ThingDb.Wall.WallRenderInfo[] ria;
			Bitmap bitmap; int sprite; Button wallButton;
			
            byte material = GetSelWallTypeIndex();
          
            ria = wall.RenderNormal[0];
            int hoho = ria.Length;
            hoho = (hoho / 2) - 1;
            
            numWallVariMax.Maximum = 30;

            if (numWallVariMax.Value < numWallVari.Value)
                numWallVariMax.Value = numWallVari.Value;
            
			// для каждого направления добавляем картинку
            if (WallSelectButtons != null)
			{
				for (int facing = 0; facing < 13; facing++)
				{
					//ria = wall.RenderBreakable[facing];
                  	ria = wall.RenderNormal[facing];
					
					wallButton = WallSelectButtons[facing];
					if (ria.Length > vari)
					{
						sprite = ria[vari].SpriteIndex;
						// если включена опция Fast preview (помогает понять направления стен) подменяем картинку
						//if (checkBlackWalls.Checked) sprite = ThingDb.Walls[blackWallIndex].RenderNormal[facing][0].SpriteIndex;
						// достаем картинку и включаем кнопку если такая стена существует
						bitmap = videoBag.GetBitmap(sprite);
						wallButton.BackgroundImage = bitmap;
						wallButton.Enabled = true;
						wallButton.BackColor = btnColorGoodWall;
                        numWallVari.Maximum = hoho;
                        numWallVariMax.Maximum = hoho;
                        numWallVariMax.Value = (sender.GetType().Name == "ComboBox") ? hoho : numWallVariMax.Value;
					}
					else
					{
                        if (facing == 0)
                        {
                            numWallVari.Value--;
                            numWallVari.Maximum = numWallVari.Value;
                            numWallVariMax.Maximum = numWallVari.Value;
                            return;
                        }

						// значит что стены с такими парамиетрами не существует
						wallButton.BackgroundImage = null;
						wallButton.Enabled = false;
						wallButton.BackColor = btnColorBadWall;
					}
				}
			}
		}

        private void WallMakeTab_Load(object sender, EventArgs e)
        {

        }

        private void numWallVariMax_ValueChanged(object sender, EventArgs e)
        {
            if (numWallVariMax.Value < numWallVari.Value)
                numWallVariMax.Value = numWallVari.Value;
        }

        private void WallSetFirst(object sender, MouseEventArgs e)
        {
        }
	}
}
