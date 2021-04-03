/*
 * MapEditor
 * Пользователь: AngryKirC
 * Copyleft - Public Domain
 * Дата: 14.10.2014
 */
using System;
using System.Drawing;
using System.Collections.Generic;
using NoxShared;
using NoxShared.ObjDataXfer;
using MapEditor.MapInt;
using System.Reflection;

using MapView = MapEditor.MapView;
using ImgState = NoxShared.ThingDb.Sprite.State;
using TextRenderer = System.Windows.Forms.TextRenderer;
using Sequence = NoxShared.ThingDb.Sprite.Sequence;


namespace MapEditor.render
{
	/// <summary>
	/// Responsible for drawing map objects
	/// </summary>
	internal class ObjectRenderer
	{
		private readonly Font objectExtentFont;
		private readonly Font objectScriptNameFont;
		private readonly MapViewRenderer mapRenderer;
		private const int objectSelectionRadius = MapView.objectSelectionRadius;
		const string FORMAT_OBJECT_TEAM = "Team: {0}";
		private int drawOffsetX = 0;
		private int drawOffsetY = 0;
		// Canvas
		private readonly List<Map.Object> sortedObjectList = new List<Map.Object>();
        private readonly List<Map.Object> belowObjectList = new List<Map.Object>();
		// Pens/Brushes
		private readonly Pen objMoveablePen = new Pen(Color.OrangeRed, 1F);
		private readonly Pen extentPen = new Pen(Color.Green, 1F);
		private readonly Pen doorPen = new Pen(Color.Brown, 2F);
		private readonly Pen triggerPen = new Pen(Color.MediumOrchid, 2F);
		private readonly Pen sentryPen = new Pen(Color.DarkViolet, 2F);
		
		public ObjectRenderer(MapViewRenderer mapRenderer)
		{
			this.objectExtentFont = new Font("Arial", 9F, FontStyle.Bold);
			this.objectScriptNameFont = new Font("Arial", 11F);
			this.mapRenderer = mapRenderer;
		}
		
		/// <summary>
		/// Returns color index for specified item parameter. (MATERIAL for ex)
		/// </summary>
		private static int GetItemColorSlot(ModifierDb.Mod mod, string name)
        {
			// Get string pointer (aka color name)
       		FieldInfo colptr = typeof(ModifierDb.Mod).GetField(name);
       		string colname = (string) colptr.GetValue(mod);
       		if (colname == null) return -1;
       		// Fetch digit on the end of the string (COLOR#)
       		return int.Parse(colname.Substring(colname.Length - 1)) - 1;
        }
		
		/// <summary>
		/// Attempts to find valid Sequence for specified parameters. Returns null on failure.
		/// </summary>
		private static Sequence FindPlayerSequence(string name, string spec)
		{
			ThingDb.Thing tt = ThingDb.Things["NewPlayer"];
			for (int i = 0; i < tt.SpriteStates.Count; i++)
			{
				ImgState tmp = tt.SpriteStates[i];
				if (tmp.Name == name)
				{
					List<Sequence> seq = tmp.Animation.Sequences;
					for (int ii = 0; ii < seq.Count; ii++)
					{
						if (seq[ii].Name == spec)
						{
							return seq[ii];
						}
					}
					break;
				}
			}
			// Not found
			return null;
		}
		
		/// <summary>
		/// Responsible for drawing complex object such as NPCs
		/// </summary>
		private Bitmap GetObjectImageSpecial(Map.Object obj, ThingDb.Thing tt)
		{
			Bitmap result = null;
			int index = -1, monsterSprite = 0;
			int[] icols; Color[] ncols;
			Sequence playerSeq = null;
			
			// Get monster sprite from direction
			if (tt.SpriteStates.Count > 0 && tt.Xfer == "MonsterXfer")
			{
				int framesPerDir = tt.SpriteStates[0].Animation.Frames.Count / 8;
				monsterSprite = framesPerDir * obj.GetExtraData<MonsterXfer>().DirectionId;
			}
			// Get NPC sprite from direction
			if (tt.Xfer == "NPCXfer")
			{
				playerSeq = FindPlayerSequence("IDLE", "NAKED");
				
				if (playerSeq != null)
				{
					int framesPerDir = playerSeq.Frames.Length / 8;
					monsterSprite = framesPerDir * obj.GetExtraData<NPCXfer>().DirectionId;
				}
			}
			
			switch (tt.DrawType)
			{
				case "MonsterDraw":
					index = tt.SpriteStates[0].Animation.Frames[monsterSprite];    
					result = mapRenderer.VideoBag.GetBitmap(index);
					break;
				case "MaidenDraw":
					index = tt.SpriteStates[0].Animation.Frames[monsterSprite];    
					
					// Convert colors
					ncols = obj.GetExtraData<MonsterXfer>().MaidenBodyColors;
					icols = new int[6];
					for (int i = 0; i < 6; i++) icols[i] = ncols[i].ToArgb();
					// Get dyncolor
					result = mapRenderer.VideoBag.GetBitmapDynamic(index, icols);
					break;
				case "NPCDraw":
					index = playerSeq.Frames[monsterSprite];
					
					// Convert colors
					ncols = obj.GetExtraData<NPCXfer>().NPCColors;
					icols = new int[6];
					for (int i = 0; i < 6; i++) icols[i] = ncols[i].ToArgb();
					// Get dyncolor
					result = mapRenderer.VideoBag.GetBitmapDynamic(index, icols);
					break;
				case "ArmorDraw":
				case "WeaponDraw":
					if (ModifierDb.Mods.ContainsKey(tt.Name) && tt.SpriteAnimFrames.Count > 0)
					{
						ModifierDb.Mod mod = ModifierDb.Mods[tt.Name];
						// build color array
						icols = new int[] {
							mod.COLOR1.ToArgb(),
							mod.COLOR2.ToArgb(),
							mod.COLOR3.ToArgb(),
							mod.COLOR4.ToArgb(),
							mod.COLOR5.ToArgb(),
							mod.COLOR6.ToArgb(),
						};
						
						// retrieve enchantments
						string[] enchs = null;
						switch (tt.Xfer)
						{
							case "ArmorXfer":
								enchs = obj.GetExtraData<ArmorXfer>().Enchantments;
								break;
							case "AmmoXfer":
								enchs = obj.GetExtraData<AmmoXfer>().Enchantments;
								break;
							case "WeaponXfer":
								enchs = obj.GetExtraData<WeaponXfer>().Enchantments;
								break;
						}
						// replace enchantment colors
						if (enchs != null)
						{
							// find color slots (-1 = none)
							int[] eslot = { GetItemColorSlot(mod, "EFFECTIVENESS"), 
								GetItemColorSlot(mod, "MATERIAL"), 
								GetItemColorSlot(mod, "PRIMARYENCHANTMENT"), 
								GetItemColorSlot(mod, "SECONDARYENCHANTMENT") };

							// DO IT
							for (int i = 0; i < 4; i++)
							{
								int slot = eslot[i];
								string name = enchs[i];
								if (ModifierDb.Mods.ContainsKey(name) && slot >= 0)
									icols[slot] = ModifierDb.Mods[name].COLOR.ToArgb();
							}
						}
						
						// Get dyncolor
						index = tt.SpriteAnimFrames[0];
						result = mapRenderer.VideoBag.GetBitmapDynamic(index, icols);
					}
					else
					{
						index = tt.SpriteMenuIcon;
						result = mapRenderer.VideoBag.GetBitmap(index);
					}
					break;
			}
			
			if (index >= 0)
			{
				drawOffsetX = mapRenderer.VideoBag.DrawOffsets[index][0];
				drawOffsetY = mapRenderer.VideoBag.DrawOffsets[index][1];
			}
			return result;
		}
		
		public Bitmap GetObjectImage(Map.Object obj)
		{
			Bitmap result = null;
			int index = -1;
			ThingDb.Thing tt = ThingDb.Things[obj.Name];
			// достаем картинку соответственно типу обьекта и его состоянию
			switch (tt.DrawType)
			{
				case "StaticDraw":
				case "BaseDraw":
				case "AnimateDraw":
				case "SphericalShieldDraw":
				case "WeaponAnimateDraw":
				case "FlagDraw":
				case "SummonEffectDraw":
				case "ReleasedSoulDraw":
				case "GlyphDraw":
				case "BoulderDraw":
				case "StaticRandomDraw":
				case "ArrowDraw":
				case "HarpoonDraw":
				case "WeakArrowDraw":
				case "VectorAnimateDraw":
				case "SlaveDraw": // obelisk, elevator
					if (tt.Subclass[(int) ThingDb.Thing.SubclassBitIndex.VISIBLE_OBELISK])
						index = tt.SpriteAnimFrames[tt.SpriteAnimFrames.Count - 1];
					else 
						index = tt.SpriteAnimFrames[0];
					break;
				case "DoorDraw": // doors
					index = tt.SpriteAnimFrames[(int) obj.GetExtraData<DoorXfer>().Direction];
					break;
				case "AnimateStateDraw": // pits
					if (tt.HasClassFlag(ThingDb.Thing.ClassFlags.HOLE))
					{
						if (obj.HasFlag(ThingDb.Thing.FlagsFlags.ENABLED) || obj.Terminator == 0)
							index = tt.SpriteStates[2].Animation.Frames[0]; // Open (broken)
						else
							index = tt.SpriteStates[0].Animation.Frames[0]; // Closed
					}
					else if (tt.SpriteStates.Count > 0) index = tt.SpriteStates[0].Animation.Frames[0];
					break;
				case "MonsterDraw":
				case "MaidenDraw":
				case "ArmorDraw":
				case "WeaponDraw":
				case "NPCDraw":
					if (!EditorSettings.Default.Draw_ComplexPreview)
					{
						// User disabled this feature
						index = tt.SpriteMenuIcon;
					}
					else
					{
						// Complex drawing routine
						return GetObjectImageSpecial(obj, tt);
					}
					break;
				default:
					if (tt.SpriteMenuIcon > 0) index = tt.SpriteMenuIcon;
					break;
			}
			if (index >= 0)
			{
				result = mapRenderer.VideoBag.GetBitmap(index);
				drawOffsetX = mapRenderer.VideoBag.DrawOffsets[index][0];
				drawOffsetY = mapRenderer.VideoBag.DrawOffsets[index][1];
			}
			return result;
		}
		
		/// <summary>
		/// Draws an object's extent
		/// </summary>
		public void DrawObjectExtent(Graphics g, Map.Object obj, bool draw3D)
		{
			ThingDb.Thing tt = ThingDb.Things[obj.Name];
			PointF center = obj.Location;
			
			if (tt.ExtentType == "CIRCLE")
			{
				PointF t = new PointF(center.X - tt.ExtentX, center.Y - tt.ExtentX);
				PointF p = new PointF((center.X) - tt.ExtentX, (center.Y - tt.ZSizeY) - tt.ExtentX);

				Pen rotatePen;

				if ((tt.Class & ThingDb.Thing.ClassFlags.IMMOBILE) != ThingDb.Thing.ClassFlags.IMMOBILE)
					rotatePen = objMoveablePen;
				else
					rotatePen = extentPen;

				g.DrawEllipse(rotatePen, new RectangleF(t, new Size(2 * tt.ExtentX, 2 * tt.ExtentX)));
				
				if (draw3D)
				{
					PointF point1 = new PointF(t.X, t.Y);
					point1.Y += tt.ExtentX;
					PointF point2 = new PointF(p.X, p.Y);
					point2.Y += tt.ExtentX;

					g.DrawLine(rotatePen, point1, point2);

					point1.X += tt.ExtentX * 2;
					point2.X += tt.ExtentX * 2;

					g.DrawLine(rotatePen, point1, point2);

					g.DrawEllipse(rotatePen, new RectangleF(p, new Size(2 * tt.ExtentX, 2 * tt.ExtentX)));
				}
			}
			if (tt.ExtentType == "BOX")
			{
				Point t = new Point((int)(center.X - (tt.ExtentX / 2)), (int)(center.Y - (tt.ExtentY / 2)));
				Point p = new Point((int)((center.X - (tt.ZSizeY / 2)) - (tt.ExtentX / 2)), (int)((center.Y - (tt.ZSizeY / 2)) - (tt.ExtentY / 2)));

				using (var m = new System.Drawing.Drawing2D.Matrix())
				{
					m.RotateAt(45, center);
					g.Transform = m;
					Pen rotatePen = new Pen(Color.Green, 1);
					g.DrawRectangle(rotatePen, new Rectangle(t, new Size(tt.ExtentX, tt.ExtentY)));
					
					if (draw3D)
					{
						PointF point1 = new PointF(t.X, t.Y);
						PointF point2 = new PointF(p.X, p.Y);
						g.DrawLine(rotatePen, point1, point2);

						point1 = new PointF(t.X, t.Y);
						point2 = new PointF(p.X, p.Y);
						point1.Y += tt.ExtentY;
						point2.Y += tt.ExtentY;
						g.DrawLine(rotatePen, point1, point2);

						point1 = new PointF(t.X, t.Y);
						point2 = new PointF(p.X, p.Y);
						point1.X += tt.ExtentX;
						point2.X += tt.ExtentX;
						g.DrawLine(rotatePen, point1, point2);

						point1 = new PointF(t.X, t.Y);
						point2 = new PointF(p.X, p.Y);
						point1.X += tt.ExtentX;
						point2.X += tt.ExtentX;
						point1.Y += tt.ExtentY;
						point2.Y += tt.ExtentY;
						g.DrawLine(rotatePen, point1, point2);
						
						g.DrawRectangle(rotatePen, new Rectangle(p, new Size(tt.ExtentX, tt.ExtentY)));
					}
					g.ResetTransform();
				}
			}
		}
		
		/// <summary>
		/// Renders extent for triggers and pressureplates
		/// </summary>
		private void DrawTriggerExtent(Graphics g, Map.Object obj)
		{
			ThingDb.Thing tt = ThingDb.Things[obj.Name];
			if (!tt.HasClassFlag(ThingDb.Thing.ClassFlags.TRIGGER)) return;
			
			TriggerXfer trigger = obj.GetExtraData<TriggerXfer>();
			
			int x = (int) obj.Location.X;
			int y = (int) obj.Location.Y;
			Rectangle triggerRect = new Rectangle(x - trigger.SizeX / 2, y - trigger.SizeY / 2, trigger.SizeX, trigger.SizeY);
			
			using (var m = new System.Drawing.Drawing2D.Matrix())
			{
				m.RotateAt(45, obj.Location);
				g.Transform = m;
				
				switch (tt.DrawType)
				{
					case "TriggerDraw":
						g.DrawRectangle(triggerPen, triggerRect);
						break;
					case "PressurePlateDraw":
						Rectangle shadeRect = new Rectangle(triggerRect.X + 1, triggerRect.Y + 1, triggerRect.Width, triggerRect.Height);
						Pen pen = new Pen(trigger.BackColor, 1F);
						g.DrawRectangle(pen, shadeRect);
						pen = new Pen(trigger.EdgeColor, 1F);
						g.DrawRectangle(pen, triggerRect);
						break;
				}
				g.ResetTransform();
			}
		}
		
		public void UpdateCanvas(Rectangle clip)
		{
			sortedObjectList.Clear();
			belowObjectList.Clear();
			// sort objects
           	foreach (Map.Object obj in mapRenderer.Map.Objects)
           	{
           		if (clip.Contains((int) obj.Location.X, (int) obj.Location.Y))
           		{
           			if (ThingDb.Things[obj.Name].HasObjectFlag(ThingDb.Thing.FlagsFlags.BELOW))
           				belowObjectList.Add(obj);
           			else
           				sortedObjectList.Add(obj);
           		}
           	}
           	sortedObjectList.Sort(new ObjectZComparer());
           	belowObjectList.Sort(new ObjectZComparer());
		}
		
		public void RenderBelow(Graphics g)
		{
			DrawObjects(g, belowObjectList);
		}
		
		public void RenderNormal(Graphics g)
		{
			DrawObjects(g, sortedObjectList);
		}
		
		private void DrawObjects(Graphics g, List<Map.Object> objects)
		{
			Pen pen; PointF center, ptf, topLeft; 
			ThingDb.Thing tt;
			Map.Object underCursor = MainWindow.Instance.mapView.GetObjectUnderCursor();
			bool drawExtents3D = EditorSettings.Default.Draw_Extents_3D;
			
			if (EditorSettings.Default.Draw_Objects)
			{
				foreach (Map.Object oe in objects)
				{
					ptf = oe.Location;
					float x = ptf.X, y = ptf.Y;
					tt = ThingDb.Things[oe.Name];
					
					center = new PointF(x, y);
					topLeft = new PointF(center.X - objectSelectionRadius, center.Y - objectSelectionRadius);
					pen = mapRenderer.ColorLayout.Objects;

					if (EditorSettings.Default.Edit_PreviewMode) // Visual Preview
					{
						// invisible triggers and pressure plates
						if (tt.DrawType == "TriggerDraw" || tt.DrawType == "PressurePlateDraw")
						{
							DrawTriggerExtent(g, oe);
							continue;
						}
						// black powder
						if (tt.DrawType == "BlackPowderDraw")
						{
							Rectangle bp = new Rectangle((int) x - 2, (int) y - 2, 4, 4);
							g.FillRectangle(new SolidBrush(Color.Gray), bp);
							continue;
						}
						
						Bitmap image = GetObjectImage(oe);
						if (image == null || tt.DrawType == "NoDraw")
						{
							// in case of failure draw only the extent
							DrawObjectExtent(g, oe, drawExtents3D);
						}
						else
						{
							int sizeX = tt.SizeX / 2;
							int sizeY = tt.SizeY / 2;
							x -= (sizeX - drawOffsetX);
							y -= (sizeY - drawOffsetY) + tt.Z;
							// no blurring
							int ix = Convert.ToInt32(x);
							int iy = Convert.ToInt32(y);
							
							// recolor in case it is being selected
							bool isSelected = mapRenderer.SelectedObjects.Items.Contains(oe);
							bool isUnderCursor = false;
							if (underCursor != null) isUnderCursor = underCursor.Equals(oe);
							// draw the image
							if (isSelected || isUnderCursor)
							{
								// highlight selection
								var shader = new BitmapShader(image);
								shader.LockBitmap();
								
								var hltColor = mapRenderer.ColorLayout.Selection;
								
								if (isSelected)
								{
									shader.ColorShade(hltColor, 0.5F);
								}
								else if (isUnderCursor)
								{
									if (MapInterface.CurrentMode == EditMode.OBJECT_PLACE)
										hltColor = mapRenderer.ColorLayout.Removing;
									
									shader.ColorGradWaves(hltColor, 7F, Environment.TickCount);
								}
								image = shader.UnlockBitmap();
								g.DrawImage(image, ix, iy, image.Width, image.Height);
								image.Dispose();
							}
							else
								g.DrawImage(image, ix, iy, image.Width, image.Height);
						}
					}
					else
					{
						// Object facing helper
						if (EditorSettings.Default.Draw_ObjectFacing)
						{
							float deg = -1F;
							if (tt.Xfer == "MonsterXfer")
							{
								MonsterXfer xfer = oe.GetExtraData<MonsterXfer>();
								deg = (float)MonsterXfer.NOX_DIRECT_VALS[xfer.DirectionId] / 256F * 360F;
							}
							if (tt.Xfer == "NPCXfer")
							{
								NPCXfer xfer = oe.GetExtraData<NPCXfer>();
								deg = (float)MonsterXfer.NOX_DIRECT_VALS[xfer.DirectionId] / 256F * 360F;
							}
							if (deg >= 0F)
							{
								using (var m = new System.Drawing.Drawing2D.Matrix())
								{
									m.RotateAt(deg, center);
									g.Transform = m;
									
									g.DrawLine(objMoveablePen, center.X, center.Y, center.X + 20, center.Y);
									g.ResetTransform();
								}
							}
							// Sentry ray
							if (tt.Xfer == "SentryXfer")
							{
								SentryXfer sentry = oe.GetExtraData<SentryXfer>();
								float targX = x + ((float) Math.Cos(sentry.BasePosRadian) * 80F);
								float targY = y + ((float) Math.Sin(sentry.BasePosRadian) * 80F);
								// show sentry ray direction
								g.DrawLine(sentryPen, x, y, targX, targY);
							}
						}
						
						if (mapRenderer.SelectedObjects.Items.Contains(oe)) pen = Pens.Green;
						
						g.DrawEllipse(pen, new RectangleF(topLeft, new Size(2 * objectSelectionRadius, 2 * objectSelectionRadius)));
						
						// If is a door
						if ((tt.Class & ThingDb.Thing.ClassFlags.DOOR) == ThingDb.Thing.ClassFlags.DOOR)
						{
							DoorXfer door = oe.GetExtraData<DoorXfer>();
							if (door.Direction == DoorXfer.DOORS_DIR.South)
							{
								g.DrawLine(doorPen, new Point((int)center.X, (int)center.Y), new Point((int)center.X - 20, (int)center.Y - 20));

								if (drawExtents3D)
								{
									g.DrawLine(doorPen, new Point((int)center.X, (int)center.Y - 40), new Point((int)center.X - 20, (int)center.Y - 60));
									g.DrawLine(doorPen, new Point((int)center.X, (int)center.Y), new Point((int)center.X - 20, (int)center.Y - 60));

									g.DrawLine(doorPen, new Point((int)center.X - 20, (int)center.Y - 20), new Point((int)center.X - 20, (int)center.Y - 60));
								}
							}
							else if (door.Direction == DoorXfer.DOORS_DIR.West)
							{
								g.DrawLine(doorPen, new Point((int)center.X, (int)center.Y), new Point((int)center.X + 20, (int)center.Y - 20));

								if (drawExtents3D)
								{
									g.DrawLine(doorPen, new Point((int)center.X, (int)center.Y - 40), new Point((int)center.X + 20, (int)center.Y - 60));
									g.DrawLine(doorPen, new Point((int)center.X, (int)center.Y), new Point((int)center.X + 20, (int)center.Y - 60));

									g.DrawLine(doorPen, new Point((int)center.X + 20, (int)center.Y - 20), new Point((int)center.X + 20, (int)center.Y - 60));
								}
							}
							else if (door.Direction == DoorXfer.DOORS_DIR.North)
							{
								g.DrawLine(doorPen, new Point((int)center.X, (int)center.Y), new Point((int)center.X + 20, (int)center.Y + 20));

								if (drawExtents3D)
								{
									g.DrawLine(doorPen, new Point((int)center.X, (int)center.Y - 40), new Point((int)center.X + 20, (int)center.Y - 20));
									g.DrawLine(doorPen, new Point((int)center.X, (int)center.Y), new Point((int)center.X + 20, (int)center.Y - 20));

									g.DrawLine(doorPen, new Point((int)center.X + 20, (int)center.Y + 20), new Point((int)center.X + 20, (int)center.Y - 20));
								}
							}
							else if (door.Direction == DoorXfer.DOORS_DIR.East)
							{
								g.DrawLine(doorPen, new Point((int)center.X, (int)center.Y), new Point((int)center.X - 20, (int)center.Y + 20));

								if (drawExtents3D)
								{
									g.DrawLine(doorPen, new Point((int)center.X, (int)center.Y - 40), new Point((int)center.X - 20, (int)center.Y - 20));
									g.DrawLine(doorPen, new Point((int)center.X, (int)center.Y), new Point((int)center.X - 20, (int)center.Y - 20));

									g.DrawLine(doorPen, new Point((int)center.X - 20, (int)center.Y + 20), new Point((int)center.X - 20, (int)center.Y - 20));
								}
							}
						}
					}

					if (EditorSettings.Default.Draw_Extents)
					{
						if (!(!EditorSettings.Default.Draw_AllExtents && oe.HasFlag(ThingDb.Thing.FlagsFlags.NO_COLLIDE)))
						{
							DrawObjectExtent(g, oe, drawExtents3D);
						}
					}
				}
				
				// Draw labels on the separate cycle to prevent layer glitching
				if (EditorSettings.Default.Draw_AllText)
				{
					foreach (Map.Object oe in objects)
					{
						Point textLocaton = new Point(Convert.ToInt32(oe.Location.X), Convert.ToInt32(oe.Location.Y));
                        textLocaton.X -= objectSelectionRadius; textLocaton.Y -= objectSelectionRadius;
						
                        if (EditorSettings.Default.Draw_ObjCustomLabels && oe.Team > 0)
                        {
                        	// Draw team
                        	Point loc = new Point(textLocaton.X, textLocaton.Y - 12);
                        	TextRenderer.DrawText(g, String.Format(FORMAT_OBJECT_TEAM, oe.Team), objectExtentFont, loc, Color.LightPink);
                        }
                        if (EditorSettings.Default.Draw_ObjCustomLabels && oe.Scr_Name.Length > 0)
						{
                        	// Draw custom label
							Size size = TextRenderer.MeasureText(oe.ScrNameShort, objectExtentFont);
							textLocaton.X -= size.Width / 3;
							TextRenderer.DrawText(g, oe.ScrNameShort, objectExtentFont, textLocaton, Color.Cyan);
						}
						else if (EditorSettings.Default.Draw_ObjThingNames)
						{
							// Draw thing name
							Size size = TextRenderer.MeasureText(oe.Name, objectExtentFont);
							textLocaton.X -= size.Width / 3;
							TextRenderer.DrawText(g, oe.Name, objectExtentFont, textLocaton, Color.Green);
						}
						else if (!EditorSettings.Default.Edit_PreviewMode && !(EditorSettings.Default.Draw_Extents || EditorSettings.Default.Draw_AllExtents))
							TextRenderer.DrawText(g, oe.Extent.ToString(), objectExtentFont, textLocaton, Color.Purple);
					}
				}
			}
		}
	}
}
