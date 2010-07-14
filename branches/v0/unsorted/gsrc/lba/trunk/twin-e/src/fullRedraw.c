/*
Copyright (C) 2002-2004 The TwinE team

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/

#include "lba.h"

extern unsigned char* brickTable[];
extern unsigned char* brickMaskTable[];

#ifdef GAME_DEBUG
actor* pCurrentActorRender;
#endif

static void DrawOverBrick3(int X, int Z, int Y);

short int overlay3dObect = 0;

#ifdef GAME_DEBUG
void draw2dBox(int x1, int y1, int x2, int y2, int color) {
	drawLine(x1, y1, x1, y2, color);
	drawLine(x2, y1, x2, y2, color);
	drawLine(x1, y1, x2, y1, color);
	drawLine(x1, y2, x2, y2, color);
}
#endif

void fullRedraw(int param) {
	short int temp1;
	short int temp2;

	// short int temp4;
	int counter;
	int counter2;

	int i;

	int arg_1A;
	int a12;
	int arg_46;
	int arg_42;
	int a0E;
	int arg_1E;
	actor *lactor;
	int temp3;
	int spriteWidth;
	int spriteHeight;

	assert(frontVideoBufferbis == frontVideoBuffer);

	temp1 = fullRedrawVar1;
	temp2 = fullRedrawVar2;

	UnSetClip();

#ifdef USE_GL
	param = 1;
	osystem_startFrame();
#endif

	if (!param) {
		blitBackgroundOnDirtyBoxes(); // blit background on dirty boxes
	} else {   // fully redraw the background
		freezeTime();
		Cls();
		redrawCube(); // draw cube background to frontVideoBuffer
		updateOverlayObjectsPosition(temp1, temp2, fullRedrawVar1, fullRedrawVar2);
		CopyScreen(frontVideoBuffer, workVideoBuffer);
	}

	osystem_startDisplayList();

	arg_1A = 0;
	a12 = 0;
	arg_46 = arg_1A + 0x1000;
	arg_42 = arg_1A + 0x0C00;

	// loop1

	for (arg_1A = 0; arg_1A < numActorInRoom; arg_1A++, arg_46++, arg_42++) { // process actors (and draw shadow if applicable)
		lactor = &actors[arg_1A];
		lactor->dynamicFlagsBF.wasDrawn = 0;  // recheck -> reinit the draw flags for the current objects

		if ((useAnotherGrm == -1) || (lactor->Y <= READ_LE_S16((currentGrid2) * 24 + (char*)zoneData + 8))) { // eject characters hidden when using another GRM

#ifndef USE_GL
			if (lactor->staticFlagsBF.bIsBackgrounded && param == 0) {  // background actor, no redraw required
				// calculate the actor position on screen
				projectPositionOnScreen(lactor->X - cameraX, lactor->Y - cameraZ, lactor->Z - cameraY);

				// is the actor in the viewable screen ?
				if (projectedPositionX > -50 && projectedPositionX < 680 && projectedPositionY > -30 && projectedPositionY < 580) {
					lactor->dynamicFlagsBF.wasDrawn = 1; // flag ?
				}
			} else
#endif
			{
				if (lactor->costumeIndex != -1  && !(lactor->staticFlagsBF.bNoDisplay)) { // 0x200 -> visible
					// calculate the actor position on screen
					projectPositionOnScreen(lactor->X - cameraX, lactor->Y - cameraZ, lactor->Z - cameraY);

					if (((lactor->staticFlagsBF.bIsUsingClipping) && projectedPositionX > -112 && projectedPositionX < 752 && projectedPositionY > -50 && projectedPositionY < 651)
							|| ((!(lactor->staticFlagsBF.bIsUsingClipping)) && projectedPositionX > -50 && projectedPositionX < 680 && projectedPositionY > -30 && projectedPositionY < 580)) {
						temp3 = lactor->Z + lactor->X - cameraX - cameraY;

						if (lactor->standOn != -1) { // if actor is on another actor
							temp3 = actors[lactor->standOn].X - cameraX + actors[lactor->standOn].Z - cameraY + 2;
						}

						if (lactor->staticFlagsBF.bIsSpriteActor) {
							drawList[a12].field_2 = arg_46; // 0x1000
							if (lactor->staticFlagsBF.bIsUsingClipping) {
								temp3 = lactor->lastX - cameraX + lactor->lastY - cameraY;
							}
						} else {
							drawList[a12].field_2 = arg_1A;
						}

						drawList[a12].field_0 = temp3;

						a12++;

						if (shadowMode != 0 && !(lactor->staticFlagsBF.bDoesntCastShadow)) { // 0x1000 -> no shadow cast
							if (lactor->standOn != -1) { // quick shadow calc if on another actor
								shadowX = lactor->X;
								shadowZ = lactor->Y - 1;
								shadowY = lactor->Z;
							} else {
								GetShadow(lactor->X, lactor->Y, lactor->Z);
							}

							temp3--;
							drawList[a12].field_0 = temp3;  // save the shadow entry in the drawList
							//drawList[a12].field_2 = arg_42; // 0xC00
							drawList[a12].field_2 = 0xC00; // 0xC00
							drawList[a12].X = shadowX;
							drawList[a12].Z = shadowZ;
							drawList[a12].Y = shadowY;
							drawList[a12].field_A = 2;
							a12++;
						}

						if (drawInGameTransBox) {
							if (currentlyFollowedActor != arg_1A) {
								/*
								 * arg_16=projectedPositionX; arg_12=projectedPositionY;
								 */
							}
						}
					}
				}
			}
		}
	}

#ifdef GAME_DEBUG
	if (bShowFlags) {
		int i;
		for (i = 0;i < numFlags;i++) {
			flagDataStruct* pCurrentFlagData = &flagData[i];

			projectPositionOnScreen(pCurrentFlagData->x - cameraX, pCurrentFlagData->y - cameraZ, pCurrentFlagData->z - cameraY);

			if (projectedPositionX > -50 && projectedPositionX < 680 && projectedPositionY > -30 && projectedPositionY < 580) {
				temp3 = pCurrentFlagData->z + pCurrentFlagData->x - cameraX - cameraY;

				drawList[a12].field_0 = pCurrentFlagData->z + pCurrentFlagData->x - cameraX - cameraY;
				drawList[a12].field_2 = 0x2000;

				drawList[a12].X = pCurrentFlagData->x;
				drawList[a12].Y = pCurrentFlagData->y;
				drawList[a12].Z = pCurrentFlagData->z;

				a12++;

				GetShadow(pCurrentFlagData->x, pCurrentFlagData->y, pCurrentFlagData->z);
				drawList[a12].field_0 = pCurrentFlagData->x - cameraX + pCurrentFlagData->z - cameraY - 1;
				drawList[a12].field_2 = 0xC00;
				drawList[a12].X = shadowX;
				drawList[a12].Z = shadowZ;
				drawList[a12].Y = shadowY;

				a12++;

			}
		}
	}
#endif

	arg_1A = 0;
	counter = arg_1A + 0x1800;
	counter2 = 0;

	// loop2


	do { // process extras (like key, hearts,...)
		if (extraList[counter2].field_0 != -1) {
			if (extraList[counter2].field_14 & 0x400) {
				if (lba_time - extraList[counter2].time > 35) {
					extraList[counter2].time = lba_time;
					extraList[counter2].field_14 &= 0xFBFF;
					HQ_3D_MixSample(11, 0x1000, 1, extraList[counter2].X, extraList[counter2].Z, extraList[counter2].Y);
				}
			} else {
				if ((extraList[counter2].field_14 & 1) || (extraList[counter2].field_14 & 0x40) || (extraList[counter2].field_1C + extraList[counter2].time - 150 < lba_time) || (!((lba_time + extraList[counter2].time) & 8))) {
					projectPositionOnScreen(extraList[counter2].X - cameraX, extraList[counter2].Z - cameraZ, extraList[counter2].Y - cameraY);

					if (projectedPositionX > -50 && projectedPositionX < 680 && projectedPositionY > -30 && projectedPositionY < 580) {
						int extraType;

						drawList[a12].field_0 = extraList[counter2].X - cameraX + extraList[counter2].Y - cameraY;
						drawList[a12].field_2 = counter;
						a12++;

						// to not show shadows for stars and explode clouds
						extraType = extraList[counter2].field_0 & 0x7FFF; // 0 - damage star | 1 - explode cloud

						if (shadowMode == 2 && (extraList[counter2].field_0 & 0x8000) && (extraType != 0) && (extraType != 1)) { // cast shadow
							GetShadow(extraList[counter2].X, extraList[counter2].Z, extraList[counter2].Y);
							drawList[a12].field_0 = extraList[counter2].X - cameraX + extraList[counter2].Y - cameraY - 1;
							drawList[a12].field_2 = 0xC00;
							drawList[a12].X = shadowX;
							drawList[a12].Z = shadowZ;
							drawList[a12].Y = shadowY;

							a12++;
						}
					}
				}
			}
		}
		arg_1A++;
		counter++;
		counter2++;
	} while (arg_1A < 50);

	SmallSort(drawList, a12, 12);


	/*  if(twinsen->costumeIndex != -1 && !(twinsen->staticFlagsBF.bNoDisplay))
	  {
	      int arg_32;
	      int arg_36;
	      int arg_52;
	      int arg_4A;
	      int arg_1A;
	      int result;
	      int result2;

	      arg_32=twinsen->X+twinsen->boudingBox.X.bottomLeft;
	      arg_36=twinsen->Y+twinsen->boudingBox.Z.bottomLeft;
	      arg_52=twinsen->X+twinsen->boudingBox.X.topRight;
	      arg_4A=twinsen->Y+twinsen->boudingBox.Z.topRight;
	      arg_1A=0;
	      result=-1;

	    //loop3
	      while(arg_1A<a12)
	      {
	          if(drawList[arg_1A].field_2 == 0)
	          {
	              result=arg_1A;
	              result2=drawList[arg_1A].field_0;
	              break;
	          }
	          arg_1A++;
	      }

	      if(result!=-1)
	      {
	          printf("Special draw in fullRedraw!\n");
	          exit(1);
	          arg_1A=0; // loop 4
	          while(arg_1A<a12)
	          {
	          }
	      }

	  } */


	a0E = 0;
	fullRedrawVar8 = 0;
	arg_1A = 0;
	arg_1E = 0;

	// loop6 -> final drawing code
	if (a12 > 0) {
		unsigned int flags;
		int actorNumber;
#ifdef GAME_DEBUG
		int positionInDebugBox = 0;
#endif

		do {
			actorNumber = drawList[arg_1E].field_2 & 0x3FF;
			lactor = &actors[actorNumber];
			flags = ((unsigned int) drawList[arg_1E].field_2) & 0xFC00;

			if (flags < 0xC00) { // actor
				if (!flags) {
					if (!actorNumber) {
					}

					if (lactor->previousAnimIndex != -1) // TODO: this shouldn't be required but crash when drawing the soldier getting out of the yellow moto. To investigate...
						SetInterAnimObjet2(lactor->animPosition, (char*)HQR_Get(HQR_Anims, lactor->previousAnimIndex), (char*)bodyPtrTab[lactor->costumeIndex], &lactor->animTimerData);

#ifdef GAME_DEBUG
					if (bShowBoundingBoxes)
						MDL_DrawBoundingBoxHiddenPart(lactor);

					pCurrentActorRender = lactor;
#endif
					if (!AffObjetIso(lactor->X - cameraX, lactor->Y - cameraZ, lactor->Z - cameraY, 0, lactor->angle, 0, bodyPtrTab[lactor->costumeIndex])) {
#ifdef GAME_DEBUG
						if (bShowBoundingBoxes)
							MDL_DrawBoundingBoxShownPart(lactor);
#endif

						if (renderLeft < 0)
							renderLeft = 0;

						if (renderTop < 0)
							renderTop = 0;

						if (renderRight >= 640)
							renderRight = 639;

						if (renderBottom >= 480)
							renderBottom = 479;

						SetClip(renderLeft, renderTop, renderRight, renderBottom);

						if (textWindowLeft <= textWindowRight && textWindowTop <= textWindowBottom) {
							int tempX;
							int tempZ;
							int tempY;

							lactor->dynamicFlagsBF.wasDrawn = 1;

							tempX = (lactor->X + 0x100) >> 9;
							tempZ = lactor->Y >> 8;

							if (lactor->field_3 & 0x7F)
								tempZ++;

							tempY = (lactor->Z + 0x100) >> 9;

							DrawOverBrick(tempX, tempZ, tempY);

							AddPhysBox(textWindowLeft, textWindowTop, renderRight, renderBottom);

							if ((lactor->staticFlagsBF.bIsBackgrounded) && param == 1) { // the actor is in background. Copy it to the back buffer
								//  blitRectangle(textWindowLeft, textWindowTop, textWindowRight, textWindowBottom, (char *) frontVideoBuffer, textWindowLeft, textWindowTop, (char *) workVideoBuffer);
							}
						}
					}

#ifdef GAME_DEBUG
					actorBox[positionInDebugBox].actorNum = actorNumber;
					actorBox[positionInDebugBox].left = renderLeft;
					actorBox[positionInDebugBox].right = renderRight;
					actorBox[positionInDebugBox].top = renderTop;
					actorBox[positionInDebugBox].bottom = renderBottom;

					positionInDebugBox++;

					if (bShowActorNumbers) {
						char stringTemp[256];

						sprintf(stringTemp, "%d", actorNumber);
						CoulFont(57);

						projectPositionOnScreen(lactor->X - cameraX, lactor->Y - cameraZ, lactor->Z - cameraY);

						if (projectedPositionX > 40 && projectedPositionX < 600
								&& projectedPositionY > 40 && projectedPositionY < 440)
							Font(projectedPositionX, projectedPositionY, stringTemp);

						AddPhysBox(projectedPositionX, projectedPositionY, projectedPositionX + 50, projectedPositionY + 50);
					}

#endif
				}
			} else if (flags == 0xC00) { // shadows
				if (!(drawList[arg_1E].field_2&0x3FF)) {
					//  arg_0E=1;
				}

				projectPositionOnScreen(drawList[arg_1E].X - cameraX, drawList[arg_1E].Z - cameraZ, drawList[arg_1E].Y - cameraY);
				GetDxDyGraph(drawList[arg_1E].field_A, &spriteWidth, &spriteHeight, (char*)shadowSprite);

				renderLeft = projectedPositionX - (spriteWidth - (drawList[arg_1E].field_2 & 0x3FF)) / 2;
				renderTop = projectedPositionY - (spriteHeight - (drawList[arg_1E].field_2 & 0x3FF)) / 2;

				renderBottom = projectedPositionY + (spriteHeight - (drawList[arg_1E].field_2 & 0x3FF)) / 2;
				renderRight = projectedPositionX + (spriteWidth - (drawList[arg_1E].field_2 & 0x3FF)) / 2;

				SetClip(renderLeft, renderTop, renderRight, renderBottom);

				if (textWindowLeft <= textWindowRight && textWindowTop <= textWindowBottom) {
					AffGraph(drawList[arg_1E].field_A, renderLeft, renderTop, shadowSprite);
				}

				DrawOverBrick((drawList[arg_1E].X + 0x100) >> 9, drawList[arg_1E].Z >> 8 , (drawList[arg_1E].Y + 0x100) >> 9);

				AddPhysBox(textWindowLeft, textWindowTop, renderRight, renderBottom);
			} else if (flags < 0x1000) {
				printf("Draw actor <0x1000 %d\n",
					   drawList[arg_1E].field_2 & 0x3FF);
			} else if (flags == 0x1000) { // sprite actor
				projectPositionOnScreen(lactor->X - cameraX, lactor->Y - cameraZ, lactor->Z - cameraY);
				GetDxDyGraph(0, &spriteWidth, &spriteHeight, (char *) HQR_Get(HQR_Sprites, lactor->costumeIndex));

				renderLeft = projectedPositionX + *(short int *)(spriteActorData + lactor->costumeIndex * 16);  // calculate center
				renderTop = projectedPositionY + *(short int *)(spriteActorData + lactor->costumeIndex * 16 + 2);

				renderRight = renderLeft + spriteWidth;
				renderBottom = renderTop + spriteHeight;

				if (lactor->staticFlagsBF.bIsUsingClipping) {
					// if sprite actor use croping
					SetClip(fullRedrawVar1 + lactor->cropLeft, fullRedrawVar2 + lactor->cropTop, fullRedrawVar1 + lactor->cropRight, fullRedrawVar2 + lactor->cropBottom);
				} else {
					SetClip(renderLeft, renderTop, renderRight, renderBottom);
				}

				if (textWindowLeft <= textWindowRight && textWindowTop <= textWindowBottom) {
#ifdef GAME_DEBUG
					if (bShowBoundingBoxes) {
						MDL_DrawBoundingBoxHiddenPart(lactor);
					}
#endif
					AffGraph(0, renderLeft, renderTop, HQR_Get(HQR_Sprites, lactor->costumeIndex));
#ifdef GAME_DEBUG
					if (bShowBoundingBoxes) {
						MDL_DrawBoundingBoxShownPart(lactor);
					}
#endif

					lactor->dynamicFlagsBF.wasDrawn = 1;

					if (lactor->staticFlagsBF.bIsUsingClipping) {
						DrawOverBrick3((lactor->lastX + 0x100) >> 9, lactor->lastZ >> 8, (lactor->lastY + 0x100) >> 9);
					} else {
						int tempX;
						int tempZ;
						int tempY;

						tempX = (lactor->X + lactor->boudingBox.X.topRight + 0x100) >> 9;
						tempZ = lactor->Y >> 8;
						if (lactor->field_3 & 0x7F)
							tempZ++;
						tempY = (lactor->Z + lactor->boudingBox.Z.topRight + 0x100) >> 9;

						DrawOverBrick3(tempX, tempZ, tempY);
					}

#ifndef USE_GL
					AddPhysBox(textWindowLeft, textWindowTop, textWindowRight, textWindowBottom);

					if (lactor->staticFlagsBF.bIsBackgrounded && param == 1) {
						//    blitRectangle(textWindowLeft, textWindowTop,textWindowRight, textWindowBottom,(char *) frontVideoBuffer, textWindowLeft,textWindowTop, (char *) workVideoBuffer);
					}
#endif
				}

#ifdef GAME_DEBUG
				{
					// code to add the sprite actor box to the debugger handler
					actorBox[positionInDebugBox].actorNum = actorNumber;
					actorBox[positionInDebugBox].left = renderLeft;
					actorBox[positionInDebugBox].right = renderRight;
					actorBox[positionInDebugBox].top = renderTop;
					actorBox[positionInDebugBox].bottom = renderBottom;

					positionInDebugBox++;
				}

				if (bShowActorNumbers) { // print the actor number
					char stringTemp[256];

					sprintf(stringTemp, "%d", actorNumber);
					CoulFont(157);

					projectPositionOnScreen(lactor->X - cameraX, lactor->Y - cameraZ, lactor->Z - cameraY);

					if (projectedPositionX > 40 && projectedPositionX < 600 && projectedPositionY > 40 && projectedPositionY < 440)
						Font(projectedPositionX, projectedPositionY, stringTemp);

					AddPhysBox(projectedPositionX, projectedPositionY, projectedPositionX + 50, projectedPositionY + 50);
				}
#endif
			} else if (flags == 0x1800) { // extras
				int extraFlag;
				extraListStruct* extraEntry = &extraList[actorNumber];

				projectPositionOnScreen(extraEntry->X - cameraX, extraEntry->Z - cameraZ, extraEntry->Y - cameraY);

				extraFlag = extraEntry->field_0;

				if (extraFlag & 0x8000) {
					AffSpecial(actorNumber, projectedPositionX, projectedPositionY);
				} else {
					GetDxDyGraph(0, &spriteWidth, &spriteHeight, (char*)HQR_Get(HQR_Sprites, extraEntry->field_0));

					renderLeft = (*(short int*)(((extraEntry->field_0 * 8) * 2) + spriteActorData)) + projectedPositionX;
					renderTop = (*(short int*)(((extraEntry->field_0 * 8) * 2) + spriteActorData) + 2) + projectedPositionY;

					renderRight = renderLeft + spriteWidth;
					renderBottom = renderTop + spriteHeight;

					AffGraph(0, renderLeft, renderTop, HQR_Get(HQR_Sprites, extraEntry->field_0));
				}

				SetClip(renderLeft, renderTop, renderRight, renderBottom);

				if ((textWindowLeft <= textWindowRight) && (textWindowTop <= textWindowBottom)) {
					DrawOverBrick((drawList[arg_1E].X + 0x100) >> 9, drawList[arg_1E].Z >> 8 , (drawList[arg_1E].Y + 0x100) >> 9);
					AddPhysBox(textWindowLeft, textWindowTop, renderRight, renderBottom);
				}
			}
#ifdef GAME_DEBUG
			else if (flags == 0x2000) { // flag
				int flagIdx = -1;
				char tempString[40];

				for (i = 0;i < numFlags;i++) {
					if (drawList[arg_1E].X == flagData[i].x && drawList[arg_1E].Y == flagData[i].y && drawList[arg_1E].Z == flagData[i].z) {
						flagIdx = i;
					}
				}

				assert(flagIdx != -1);

				AffObjetIso(drawList[arg_1E].X - cameraX, drawList[arg_1E].Y - cameraZ, drawList[arg_1E].Z - cameraY, 0, 0, 0, flagModelPtr);

				if (renderLeft < 0)
					renderLeft = 0;

				if (renderTop < 0)
					renderTop = 0;

				if (renderRight >= 640)
					renderRight = 639;

				if (renderBottom >= 480)
					renderBottom = 479;

				SetClip(renderLeft, renderTop, renderRight, renderBottom);

				if (textWindowLeft <= textWindowRight && textWindowTop <= textWindowBottom) {
					int tempX;
					int tempZ;
					int tempY;

					lactor->dynamicFlagsBF.wasDrawn;

					tempX = (drawList[arg_1E].X) >> 9;
					tempZ = drawList[arg_1E].Y >> 8;
					tempY = (drawList[arg_1E].Z) >> 9;

					DrawOverBrick(tempX, tempZ, tempY);

					AddPhysBox(textWindowLeft, textWindowTop, renderRight, renderBottom);

					if ((lactor->staticFlagsBF.bIsBackgrounded) && param == 1) { // the actor is in background. Copy it to the back buffer
						//  blitRectangle(textWindowLeft, textWindowTop, textWindowRight, textWindowBottom, (char *) frontVideoBuffer, textWindowLeft, textWindowTop, (char *) workVideoBuffer);
					}
				}

				sprintf(tempString, "%d", flagIdx);
				CoulFont(255);

				projectPositionOnScreen(flagData[flagIdx].x - cameraX, flagData[flagIdx].y - cameraZ, flagData[flagIdx].z - cameraY);

				spriteWidth = SizeFont(tempString);
				spriteHeight = 48;

				renderLeft = projectedPositionX - (spriteWidth / 2);
				renderRight = projectedPositionX + (spriteWidth / 2);
				renderTop = projectedPositionY - 24;
				renderBottom = projectedPositionY + spriteHeight;

				if (renderLeft < 0)
					renderLeft = 0;

				if (renderTop < 0)
					renderTop = 0;

				if (renderRight > 639)
					renderRight = 639;

				if (renderBottom > 479)
					renderBottom = 479;

				SetClip(renderLeft, renderTop, renderRight, renderBottom);

				Font(renderLeft, renderTop, tempString);

				if ((textWindowLeft <= textWindowRight) && (textWindowTop <= textWindowBottom))
					AddPhysBox(textWindowLeft, textWindowTop, renderRight, renderBottom);

			}
#endif

			arg_1A++;
			UnSetClip();
			arg_1E++;
		} while (arg_1A < a12);

#ifdef GAME_DEBUG
		debugger_numOfActorOnScreen = positionInDebugBox;
#endif

		// unknownLoop

		// loop5
	}

#ifdef GAME_DEBUG
	if (bShowSpriteClip) {
		int i;

		for (i = 0;i < a12;i++) {
			int actorNumber = drawList[i].field_2 & 0x3FF;
			int flags;

			lactor = &actors[actorNumber];
			flags = ((unsigned int) drawList[i].field_2) & 0xFC00;

			if (flags == 0x1000 && lactor->staticFlagsBF.bIsUsingClipping);
			{
#define clipColor 100
				//            drawLine(fullRedrawVar1 + lactor->cropLeft, fullRedrawVar2 + lactor->cropTop, fullRedrawVar1 + lactor->cropRight, fullRedrawVar2 + lactor->cropBottom,clipColor);

				draw2dBox(fullRedrawVar1 + lactor->cropLeft, fullRedrawVar2 + lactor->cropTop, fullRedrawVar1 + lactor->cropRight, fullRedrawVar2 + lactor->cropBottom, clipColor);
				draw2dBox(fullRedrawVar1 + lactor->cropLeft + 1, fullRedrawVar2 + lactor->cropTop + 1, fullRedrawVar1 + lactor->cropRight + 1, fullRedrawVar2 + lactor->cropBottom + 1, clipColor);
			}
		}
	}
#endif

#ifdef GAME_DEBUG
	/* //if(bShowSpriteClip)
	  for(i=0;i<debugger_numOfActorOnScreen;i++)
	  {
	#define actorBoxColor 120
	    draw2dBox(actorBox[i].left, actorBox[i].top, actorBox[i].right, actorBox[i].bottom,actorBoxColor);
	  }*/
#endif

#ifdef GAME_DEBUG
	ZONE_DrawZones();
#endif

	counter2 = 0;

	for (i = 0;i < 10;i++) {
		if (overlayObjectList[i].field_0 != -1) {
			switch (overlayObjectList[i].positionType) {
			case 0: { // normal
					if (lba_time >= overlayObjectList[i].timeToDie) {
						overlayObjectList[i].field_0 = -1;
						continue;
					}
					break;
				}
			case 1: { // follow actor
					projectPositionOnScreen(actors[overlayObjectList[i].followedActor].X - cameraX, actors[overlayObjectList[i].followedActor].boudingBox.Y.topRight - cameraZ, actors[overlayObjectList[i].followedActor].Z - cameraY);

					overlayObjectList[i].X = projectedPositionX;
					overlayObjectList[i].Y = projectedPositionY;

					if (lba_time >= overlayObjectList[i].timeToDie) {
						overlayObjectList[i].field_0 = -1;
						continue;
					}

					break;
				}
			}

			switch (overlayObjectList[i].type) {
			case 0: { // sprite
					char* pSprite;

					pSprite = HQR_Get(HQR_Sprites, overlayObjectList[i].field_0);

					GetDxDyGraph(0, &spriteWidth, &spriteHeight, pSprite);

					renderLeft = (*(short int*)(((overlayObjectList[i].field_0 * 8) * 2) + spriteActorData)) + overlayObjectList[i].X;
					renderTop = (*(short int*)(((overlayObjectList[i].field_0 * 8) * 2) + spriteActorData) + 2) + overlayObjectList[i].Y;

					renderRight = renderLeft + spriteWidth;
					renderBottom = renderTop + spriteHeight;

					AffGraph(0, renderLeft, renderTop, pSprite);

					if ((textWindowLeft <= textWindowRight) && (textWindowTop <= textWindowBottom))
						AddPhysBox(textWindowLeft, textWindowTop, renderRight, renderBottom);

					break;
				}
			case 1: { // number
					char tempString[10];

					sprintf(tempString, "%d", overlayObjectList[i].field_0);

					spriteWidth = SizeFont(tempString);
					spriteHeight = 48;

					renderLeft = overlayObjectList[i].X - (spriteWidth / 2);
					renderRight = overlayObjectList[i].X + (spriteWidth / 2);
					renderTop = overlayObjectList[i].Y - 24;
					renderBottom = overlayObjectList[i].Y + spriteHeight;

					SetClip(renderLeft, renderTop, renderRight, renderBottom);

					CoulFont(overlayObjectList[i].followedActor);

					Font(renderLeft, renderTop, tempString);

					if ((textWindowLeft <= textWindowRight) && (textWindowTop <= textWindowBottom))
						AddPhysBox(textWindowLeft, textWindowTop, renderRight, renderBottom);

					break;
				}
			case 2: { // number range
					char tempString[10];

					sprintf(tempString, "%d", BoundRegleTrois(overlayObjectList[i].followedActor, overlayObjectList[i].field_0, 100, (overlayObjectList[i].timeToDie - lba_time - 50)));

					spriteWidth = SizeFont(tempString);
					spriteHeight = 48;

					renderLeft = overlayObjectList[i].X - (spriteWidth / 2);
					renderRight = overlayObjectList[i].X + (spriteWidth / 2);
					renderTop = overlayObjectList[i].Y - 24;
					renderBottom = overlayObjectList[i].Y + spriteHeight;

					SetClip(renderLeft, renderTop, renderRight, renderBottom);

					CoulFont(155);

					Font(renderLeft, renderTop, tempString);

					if ((textWindowLeft <= textWindowRight) && (textWindowTop <= textWindowBottom))
						AddPhysBox(textWindowLeft, textWindowTop, renderRight, renderBottom);

					break;
				}
			case 3: { // 3d object
					char* pObject;

					Box(10, 10, 69, 69, 0);
					SetClip(10, 10, 69, 69);

					pObject = HQR_Get(HQR_Inventory, overlayObjectList[i].field_0);

					if (HQR_Flag) {
						loadGfxSub(pObject);
					}

					setCameraPosition(40, 40, 128, 200, 200);
					setCameraAngle(0, 0, 0, 60, 0, 0, 16000);

					overlay3dObect += 8;

					AffObjetIso(0, 0, 0, 0, overlay3dObect, 0, pObject);

					if (renderLeft < 0)
						renderLeft = 0;

					if (renderTop < 0)
						renderTop = 0;

					if (renderRight >= 640)
						renderRight = 639;

					if (renderBottom >= 480)
						renderBottom = 479;

					DrawCadre(10, 10, 69, 69);
					AddPhysBox(10, 10, 69, 69);

					reinitAll1();
					break;
				}
			case 4: { // text
					char tempString[256];
					GetMultiText(overlayObjectList[i].field_0, tempString);

					spriteWidth = SizeFont(tempString);
					spriteHeight = 48;

					renderLeft = overlayObjectList[i].X - (spriteWidth / 2);
					renderRight = overlayObjectList[i].X + (spriteWidth / 2);
					renderTop = overlayObjectList[i].Y - 24;
					renderBottom = overlayObjectList[i].Y + spriteHeight;

					if (renderLeft < 0)
						renderLeft = 0;

					if (renderTop < 0)
						renderTop = 0;

					if (renderRight > 639)
						renderRight = 639;

					if (renderBottom > 479)
						renderBottom = 479;

					SetClip(renderLeft, renderTop, renderRight, renderBottom);

					CoulFont(actors[overlayObjectList[i].followedActor].talkColor);

					Font(renderLeft, renderTop, tempString);

					if ((textWindowLeft <= textWindowRight) && (textWindowTop <= textWindowBottom))
						AddPhysBox(textWindowLeft, textWindowTop, renderRight, renderBottom);

					break;
				}
			}
		}
	}

	UnSetClip();

	// if(waitVSync!=0)
	// waitRetrace();

	if (drawInGameTransBox != 0) {
		// fullRedrawSub10(arg_16,arg_12,arg_E,arg_A);
		fullRedrawSub5();
		if (param != 0)
			unfreezeTime();
	} else {
		if (param == 0) {
			FlipBoxes();  // update dirtyBox to screen and move all the nextDirtyBox to dirtyBox
		} else {
			if (!lockPalette)
				osystem_Flip(frontVideoBuffer);
			fullRedrawSub5(); // move all the nextDirtyBox to dirtyBox
			unfreezeTime();
		}
	}

#ifdef USE_GL
	osystem_stopFrame();
#endif

	osystem_updateImage();

	if (!lockPalette)
		return;

	Load_HQR("ress.hqr", (byte *) & palette, 0);
	convertPalToRGBA(palette, paletteRGBA);

	/*
	 * if(useAlternatePalette) FadeToPal((char*)menuPalRGBA); else FadeToPal((char*)paletteRGBA);
	 */

	if (useAlternatePalette) {
		osystem_crossFade((char *) frontVideoBuffer, (char *) menuPalRGBA);
	} else {
		osystem_crossFade((char *) frontVideoBuffer, (char *) paletteRGBA);
	}

	osystem_setPalette((byte *) & paletteRGBA);

	readKeyboard();

	lockPalette = 0;

	osystem_updateImage();

	return;
}

void blitBackgroundOnDirtyBoxes(void) {
	int i;
	currentDirtyBoxListStruct* currentBox;

	currentBox = currentDirtyBoxList;

	for (i = 0; i < numOfRedrawBox; i++) {
		blitRectangle(currentBox->left, currentBox->top, currentBox->right, currentBox->bottom, (char *) workVideoBuffer, currentBox->left, currentBox->top, (char *) frontVideoBuffer);
		currentBox++;
	}
}

void fullRedrawSub5(void) {
	int i;

	numOfRedrawBox = 0;

	for (i = 0; i < fullRedrawVar8; i++) {
		addToRedrawBox(nextDirtyBoxList[i].left, nextDirtyBoxList[i].top, nextDirtyBoxList[i].right, nextDirtyBoxList[i].bottom);
	}
}

void addToRedrawBox(short int arg_0, short int arg_4, short int arg_8, short int arg_C) {
	int var1;
	int i = 0;
	int var_C;
	int var_8;
	int var_10;
	int var_4;

#ifdef USE_GL
	return;
#endif

	var1 = (arg_8 - arg_0) * (arg_C - arg_4);

	while (i < numOfRedrawBox) {
		if (currentDirtyBoxList[i].left >= arg_0)
			var_C = arg_0;
		else
			var_C = currentDirtyBoxList[i].left;

		if (currentDirtyBoxList[i].right <= arg_8)
			var_8 = arg_8;
		else
			var_8 = currentDirtyBoxList[i].right;

		if (currentDirtyBoxList[i].top >= arg_4)
			var_10 = arg_4;
		else
			var_10 = currentDirtyBoxList[i].top;

		if (currentDirtyBoxList[i].bottom <= arg_C)
			var_4 = arg_C;
		else
			var_4 = currentDirtyBoxList[i].bottom;

		if ((var_8 - var_C) * (var_4 - var_10) < ((currentDirtyBoxList[i].bottom - currentDirtyBoxList[i].top) * (currentDirtyBoxList[i].right - currentDirtyBoxList[i].left) + var1)) {
			currentDirtyBoxList[i].left = var_C;
			currentDirtyBoxList[i].top = var_10;
			currentDirtyBoxList[i].right = var_8;
			currentDirtyBoxList[i].bottom = var_4;

			if (currentDirtyBoxList[i].bottom >= 480)
				currentDirtyBoxList[i].bottom = 479;
			return;
		}

		i++;
	};

	currentDirtyBoxList[i].left = arg_0;
	currentDirtyBoxList[i].top = arg_4;
	currentDirtyBoxList[i].right = arg_8;
	currentDirtyBoxList[i].bottom = arg_C;

	if (currentDirtyBoxList[i].bottom >= 480)
		currentDirtyBoxList[i].bottom = 479;

	numOfRedrawBox++;
}

struct cubeEntry {
	unsigned char brickType1;
	unsigned char brickType2;
};
typedef struct cubeEntry cubeType[64][64][25];

void redrawCube(void) {

	int i;
	int x, y, z;
	// unsigned char *localBufCube = bufCube;
	unsigned char val;
	cubeType* cube = (cubeType*)bufCube;

	cameraX = newCameraX << 9;
	cameraZ = newCameraZ << 8;
	cameraY = newCameraY << 9;

	projectPositionOnScreen(-cameraX, -cameraZ, -cameraY);

	fullRedrawVar1 = projectedPositionX;
	fullRedrawVar2 = projectedPositionY;

	for (i = 0; i < 28; i++) {
		zbufferTab[i] = 0;
	}

	if (changeRoomVar10 == 0)
		return;

#ifdef GAME_DEBUG
	if (debugger_cubeClipEnabled) {
		for (z = 0; z < debugger_cubeClipZ; z++) {
			for (x = 0; x < debugger_cubeClipX; x++) {
				for (y = 0; y < debugger_cubeClipY; y++) {
					val = (*cube)[z][x][y].brickType1;
					if (val) {
						zbuffer(val - 1, (*cube)[z][x][y].brickType2, x, y, z);
					}
				}
			}
		}
	} else
#endif
	{
		for (z = 0; z < 64; z++) {
			for (x = 0; x < 64; x++) {
				for (y = 0; y < 25; y++) {
					val = (*cube)[z][x][y].brickType1;
					if (val) {
						zbuffer(val - 1, (*cube)[z][x][y].brickType2, x, y, z);
					}
				}
			}
		}
	}
}

void zbuffer(int var1, int var2, int x, int z, int y) {
	unsigned char *ptr;
	unsigned short int bx;
	int zbufferIndex;
	zbufferDataStruct *currentZbufferData;

	ptr = zbufferSub1(var1) + 3 + var2 * 4 + 2;

	bx = READ_LE_U16(ptr);

	if (!bx)
		return;

	zbufferSub2(x - newCameraX, z - newCameraZ, y - newCameraY);

	if (zbufferVar1 < -24)
		return;
	if (zbufferVar1 >= 640)
		return;
	if (zbufferVar2 < -38)
		return;
	if (zbufferVar2 >= 480)
		return;

#ifdef USE_GL
	osystem_drawBrick(bx -1, x - newCameraX, z - newCameraZ, y - newCameraY);
#else
	// draw the background brick
	AffGraph(bx - 1, zbufferVar1, zbufferVar2, brickTable);
#endif

	zbufferIndex = (zbufferVar1 + 24) / 24;

	if (zbufferTab[zbufferIndex] >= 150) {
		printf("Arg MAX_BRICK Z BUFFER atteint\n");
		exit(1);
	}

	currentZbufferData = &zbufferData[zbufferIndex][zbufferTab[zbufferIndex]];

	currentZbufferData->y = y;
	currentZbufferData->z = z;
	currentZbufferData->x = x;
	currentZbufferData->drawX = zbufferVar1;
	currentZbufferData->drawY = zbufferVar2;
	currentZbufferData->spriteNum = bx - 1;

	zbufferTab[zbufferIndex]++;
}

unsigned char *zbufferSub1(int var) {
	return (currentBll + READ_LE_U32(currentBll + 4 * var));
}

void zbufferSub2(int x, int y, int z) {
	zbufferVar1 = (x - z) * 24 + 288; // x pos
	zbufferVar2 = ((x + z) * 12) - (y * 15) + 215;  // y pos
}
/*
void AffGraph(int num, int var1, int var2, unsigned char *localBufferBrick)
{
  unsigned char *ptr;
  int top;
  int bottom;
  int left;
  int right;
  unsigned char *outPtr;
  unsigned char *outPtr2;
  int offset;
  int c1;
  int c2;
  int vc3;

  int temp;
  int iteration;
  int i;

  assert( textWindowLeft >= 0 );
  assert( textWindowRight <= 639 );
  assert( textWindowTop >= 0 );
  assert( textWindowBottom <= 479 );

  ptr = localBufferBrick + READ_LE_U32(localBufferBrick + num * 4);

  left = var1 + *(ptr + 2);
  top = var2 + *(ptr + 3);
  right = *ptr + left - 1;
  bottom = *(ptr + 1) + top - 1;

  ptr += 4;

  if (left >= textWindowLeft && top >= textWindowTop && right <= textWindowRight && bottom <= textWindowBottom)
  {     // no crop
    right++;
    bottom++;

    outPtr = frontVideoBuffer + screenLockupTable[top] + left;

    offset = -((right - left) - largeurEcran);

    for (c1 = 0; c1 < bottom - top; c1++)
    {
      vc3 = *(ptr++);
      for (c2 = 0; c2 < vc3; c2++)
      {
        temp = *(ptr++);
        iteration = temp & 0x3F;
        if (temp & 0xC0)
        {
          iteration++;
          if (!(temp & 0x40))
          {
            temp = *(ptr++);
            for (i = 0; i < iteration; i++)
              *(outPtr++) = temp;
          }
          else
          {
            for (i = 0; i < iteration; i++)
              *(outPtr++) = *(ptr++);
          }
        }
        else
        {
          outPtr += iteration + 1;
        }
      }
      outPtr += offset;
    }
  }
  else
  {
    if (left <= textWindowRight && top <= textWindowBottom && right >= textWindowLeft && bottom >= textWindowTop)
    {
      if (top < textWindowTop)  // si il manque un bout du haut de la brique
      {

        for (c1 = 0; c1 < (textWindowTop - top); c1++)
        {
          vc3 = *(ptr++);
          for (c2 = 0; c2 < vc3; c2++)
          {
            temp = *(ptr++);
            if (temp & 0xC0)
            {
              iteration = 0;
              if (temp & 0x40)
              {
                iteration = temp & 0x3F;
              }
              ptr += iteration + 1;
            }
          }
        }
        top = textWindowTop;
      }

      if (bottom > textWindowBottom)
        bottom = textWindowBottom;

      if (left < textWindowLeft)
        cropLeft = textWindowLeft - left;
      else
        cropLeft = 0;

      offset = -(left + cropLeft - 1 - right);

      right -= textWindowRight;
      if (right > 0)
        offset -= right;

      outPtr2 = screenLockupTable[top] + frontVideoBuffer;
      if (left >= 0)
        outPtr2 += left;
      outPtr = outBuffer;

      for (c1 = 0; c1 <= (bottom - top); c1++)
      {
        outPtr = outBuffer;

        for (i = 0; i < 512; i++)
        outBuffer[i] = 0;

        vc3 = *(ptr++);
        for (c2 = 0; c2 < vc3; c2++)
        {
          temp = *(ptr++);
          iteration = temp & 0x3F;
          if (temp & 0xC0)
          {
            iteration++;
            if (!(temp & 0x40))
            {
              temp = *(ptr++);
              for (i = 0; i < iteration; i++)
                *(outPtr++) = temp;
            }
            else
            {
              for (i = 0; i < iteration; i++)
                *(outPtr++) = *(ptr++);
            }
          }
          else
          {
            outPtr += iteration + 1;
          }
        }

        for (i = 0; i < offset; i++)
        {
          if (outBuffer[cropLeft + i] != 0)
            *(outPtr2) = outBuffer[cropLeft + i];
          outPtr2++;
        }
        outPtr2 += 640 - offset;
      }
    }
  }
}
*/

void AffGraph(int num, int var1, int var2, unsigned char *localBufferBrick) {
	unsigned char *ptr;
	int top;
	int bottom;
	int left;
	int right;
	unsigned char *outPtr;
	int offset;
	int c1;
	int c2;
	int vc3;

	int temp;
	int iteration;
	int i;

	int x;
	int y;

	assert(textWindowLeft >= 0);
	assert(textWindowRight <= 639);
	assert(textWindowTop >= 0);
	assert(textWindowBottom <= 479);

	if (localBufferBrick != brickTable) {
		ptr = localBufferBrick + READ_LE_U32(localBufferBrick + num * 4);
	} else {
		ptr = brickTable[num];
	}

	left = var1 + *(ptr + 2);
	top = var2 + *(ptr + 3);
	right = *ptr + left - 1;
	bottom = *(ptr + 1) + top - 1;

	ptr += 4;

	x = left;
	y = top;

// if (left >= textWindowLeft && top >= textWindowTop && right <= textWindowRight && bottom <= textWindowBottom)
	{     // no crop
		right++;
		bottom++;

		outPtr = frontVideoBuffer + screenLockupTable[top] + left;

		offset = -((right - left) - largeurEcran);

		for (c1 = 0; c1 < bottom - top; c1++) {
			vc3 = *(ptr++);
			for (c2 = 0; c2 < vc3; c2++) {
				temp = *(ptr++);
				iteration = temp & 0x3F;
				if (temp & 0xC0) {
					iteration++;
					if (!(temp & 0x40)) {
						temp = *(ptr++);
						for (i = 0; i < iteration; i++) {
							if (x >= textWindowLeft && x<textWindowRight && y >= textWindowTop && y < textWindowBottom)
								frontVideoBuffer[y*640+x] = temp;

							x++;
							outPtr++;
						}
					} else {
						for (i = 0; i < iteration; i++) {
							if (x >= textWindowLeft && x<textWindowRight && y >= textWindowTop && y < textWindowBottom)
								frontVideoBuffer[y*640+x] = *ptr;

							x++;
							ptr++;
							outPtr++;
						}
					}
				} else {
					outPtr += iteration + 1;
					x += iteration + 1;
				}
			}
			outPtr += offset;
			x = left;
			y++;
		}
	}
}


void drawSprite2(int num, int var1, int var2, unsigned char *localBufferBrick) {
	unsigned char *ptr;
	int top;
	int bottom;
	int left;
	int right;
	unsigned char *outPtr;
	unsigned char *outPtr2;
	int offset;
	int c1;
	int c2;
	int vc3;

	int temp;
	int iteration;
	int i;

	ptr = localBufferBrick + *(unsigned int *)(localBufferBrick + num * 4);

	left = var1 + *(ptr + 2);
	top = var2 + *(ptr + 3);
	right = *ptr + left - 1;
	bottom = *(ptr + 1) + top - 1;

	ptr += 4;

	// check des bords

	if (left >= textWindowLeft && top >= textWindowTop && right <= textWindowRight && bottom <= textWindowBottom) {
		right++;
		bottom++;

		outPtr = workVideoBuffer + screenLockupTable[top] + left;

		offset = -((right - left) - largeurEcran);

		for (c1 = 0; c1 < bottom - top; c1++) {
			vc3 = *(ptr++);
			for (c2 = 0; c2 < vc3; c2++) {
				temp = *(ptr++);
				iteration = temp & 0x3F;
				if (temp & 0xC0) {
					iteration++;
					if (!(temp & 0x40)) {
						temp = *(ptr++);
						for (i = 0; i < iteration; i++)
							*(outPtr++) = temp;
					} else {
						for (i = 0; i < iteration; i++)
							*(outPtr++) = *(ptr++);
					}
				} else {
					outPtr += iteration + 1;
				}
			}
			outPtr += offset;
		}
	} else {
		if (left <= textWindowRight && top <= textWindowBottom && right >= textWindowLeft && bottom >= textWindowTop) {
			if (top < textWindowTop) { // si il manque un bout du haut de la brique

				for (c1 = 0; c1 < (textWindowTop - top); c1++) {
					vc3 = *(ptr++);
					for (c2 = 0; c2 < vc3; c2++) {
						temp = *(ptr++);
						if (temp & 0xC0) {
							iteration = 0;
							if (temp & 0x40) {
								iteration = temp & 0x3F;
							}
							ptr += iteration + 1;
						}
					}
				}
				top = textWindowTop;
			}

			if (bottom > textWindowBottom)
				bottom = textWindowBottom;

			if (left < textWindowLeft)
				cropLeft = textWindowLeft - left;
			else
				cropLeft = 0;

			offset = -(left + cropLeft - 1 - right);

			right -= textWindowRight;
			if (right > 0)
				offset -= right;

			outPtr2 = screenLockupTable[top] + workVideoBuffer;
			if (left >= 0)
				outPtr2 += left;
			outPtr = outBuffer;

			for (c1 = 0; c1 <= bottom - top; c1++) {
				outPtr = outBuffer;

				for (i = 0; i < 512; i++)
					outBuffer[i] = 0;

				vc3 = *(ptr++);
				for (c2 = 0; c2 < vc3; c2++) {
					temp = *(ptr++);
					iteration = temp & 0x3F;
					if (temp & 0xC0) {
						iteration++;
						if (!(temp & 0x40)) {
							temp = *(ptr++);
							for (i = 0; i < iteration; i++)
								*(outPtr++) = temp;
						} else {
							for (i = 0; i < iteration; i++)
								*(outPtr++) = *(ptr++);
						}
					} else {
						outPtr += iteration + 1;
					}
				}

				for (i = 0; i < offset; i++) {
					if (outBuffer[cropLeft + i] != 0)
						*(outPtr2) = outBuffer[cropLeft + i];
					outPtr2++;
				}
				outPtr2 += 640 - offset;
			}
		}
	}
}

int projectPositionOnScreen(int coX, int coZ, int coY) {
	if (!isUsingOrhoProjection) {
		coX -= setSomething3Var12;
		coZ -= setSomething3Var14;
		coY -= setSomething3Var16;

		if (coY >= 0) {
			int bp = coY + cameraVar1;

			if (bp < 0)
				bp = 0x7FFF;

			projectedPositionX = (coX * cameraVar2) / bp + setSomethingVar1;
			projectedPositionY = (-coZ * cameraVar3) / bp + setSomethingVar2;
			projectedPositionZ = bp;

			return(-1);
		} else {
			projectedPositionX = 0;
			projectedPositionY = 0;
			projectedPositionZ = 0;
			return(0);
		}
	} else {
#ifdef USE_FLOAT
		projectedPositionX = (coX - coY) * 24 / 512.f + setSomethingVar1;
		projectedPositionY = (((coX + coY) * 12) - coZ * 30) / 512.f + setSomethingVar2;
		projectedPositionZ = coZ - coX - coY;
#else
		projectedPositionX = (coX - coY) * 24 / 512 + setSomethingVar1;
		projectedPositionY = (((coX + coY) * 12) - coZ * 30) / 512 + setSomethingVar2;
		projectedPositionZ = coZ - coX - coY;
#endif
		return (-1);
	}
	return (-1);
}

int HQ_3D_MixSample(int param0, int param1, int param2, int param3, int param4, int param5) {
	if (samplesLoaded != 0) {
		//   printf("sampleLoaded != 0 in HQ_3D_MixSample!\n");
		playSample(param0, 1, 1, 10, 10);
		// exit(1);
	}
	return (0);
}

void GetDxDyGraph(int arg_0, int *arg_4, int *arg_8, unsigned char *ptr) {
	ptr += READ_LE_S32(ptr + arg_0 * 4);

	*arg_4 = *ptr;
	*arg_8 = *(ptr + 1);
}

void DrawOverBrick(int X, int Z, int Y) {
	int CopyBlockPhysLeft;
	int CopyBlockPhysRight;
	int i;
	int j;
	zbufferDataStruct *currentZbufferData;

	CopyBlockPhysLeft = ((textWindowLeft + 24) / 24) - 1;
	CopyBlockPhysRight = ((textWindowRight + 24) / 24);

	for (j = CopyBlockPhysLeft; j <= CopyBlockPhysRight; j++) {
		for (i = 0; i < zbufferTab[j]; i++) {
			currentZbufferData = &zbufferData[j][i];

			if (currentZbufferData->drawY + 38 > textWindowTop && currentZbufferData->drawY <= textWindowBottom && currentZbufferData->z >= Z) {
				if (currentZbufferData->x + currentZbufferData->y > Y + X) {
					CopyMaskLBA(currentZbufferData->spriteNum, (j * 24) - 24, currentZbufferData->drawY, bufferBrick2, workVideoBuffer);
				}
			}
		}
	}
}

static void DrawOverBrick3(int X, int Z, int Y) {
	int CopyBlockPhysLeft;
	int CopyBlockPhysRight;
	int i;
	int j;
	zbufferDataStruct *currentZbufferData;

	CopyBlockPhysLeft = ((textWindowLeft + 24) / 24) - 1;
	CopyBlockPhysRight = (textWindowRight + 24) / 24;

	for (j = CopyBlockPhysLeft; j <= CopyBlockPhysRight; j++) {
		for (i = 0; i < zbufferTab[j]; i++) {
			currentZbufferData = &zbufferData[j][i];

			if (currentZbufferData->drawY + 38 > textWindowTop && currentZbufferData->drawY <= textWindowBottom && currentZbufferData->z >= Z) {
				if ((currentZbufferData->x == X) && (currentZbufferData->y == Y)) {
					CopyMaskLBA(currentZbufferData->spriteNum, (j * 24) - 24, currentZbufferData->drawY, bufferBrick2, workVideoBuffer);
				}

				if ((currentZbufferData->x > X) || (currentZbufferData->y > Y)) {
					CopyMaskLBA(currentZbufferData->spriteNum, (j * 24) - 24,  currentZbufferData->drawY, bufferBrick2, workVideoBuffer);
				}
			}
		}
	}
}

void CopyMaskLBA(int spriteNum, int x, int y, byte * localBufferBrick, byte * buffer) {
	unsigned char *ptr;
	int top;
	int bottom;
	int left;
	int right;
	unsigned char *outPtr;
	unsigned char *inPtr;
	int offset;
	int vc3;

	int temp;
	int j;

	int absX;
	int absY;

	int vSize;

	assert(textWindowLeft >= 0);
	assert(textWindowRight <= 639);
	assert(textWindowTop >= 0);
	assert(textWindowBottom <= 479);

	//ptr = localBufferBrick + *(unsigned int *) (localBufferBrick + spriteNum * 4);

	ptr = brickMaskTable[spriteNum];

	assert(ptr);

	left = x + *(ptr + 2);
	top = y + *(ptr + 3);
	right = *ptr + left - 1;
	bottom = *(ptr + 1) + top - 1;

	if (left > textWindowRight || right < textWindowLeft || bottom < textWindowTop || top > textWindowBottom)
		return;

	ptr += 4;

	absX = left;
	absY = top;

	vSize = (bottom - top) + 1;

	if (vSize <= 0)
		return;

	offset = -((right - left) - largeurEcran) - 1;

	right++;
	bottom++;

	// if line on top aren't in the blitting area...
	if (absY < textWindowTop) {
		int numOfLineToRemove;

		numOfLineToRemove = textWindowTop - absY;

		vSize -= numOfLineToRemove;
		if (vSize <= 0)
			return;

		absY += numOfLineToRemove;

		do {
			int lineDataSize;

			lineDataSize = *(ptr++);
			ptr += lineDataSize;
		} while (--numOfLineToRemove);
	}

	// reduce the vSize to remove lines on bottom
	if (absY + vSize - 1 > textWindowBottom) {
		vSize = textWindowBottom - absY + 1;
		if (vSize <= 0)
			return;
	}

	outPtr = frontVideoBuffer + screenLockupTable[absY] + left;
	inPtr = buffer + screenLockupTable[absY] + left;

	do {
		vc3 = *(ptr++);

		do {
			temp = *(ptr++); // skip size
			outPtr += temp;
			inPtr += temp;

			absX += temp;

			vc3--;
			if (!vc3)
				break;

			temp = *(ptr++); // copy size

			for (j = 0;j < temp;j++) {
				if (absX >= textWindowLeft && absX <= textWindowRight)
					*outPtr = *inPtr;

				absX++;
				outPtr++;
				inPtr++;
			}
		} while (--vc3);

		absX = left;

		outPtr += offset;
		inPtr += offset;
	} while (--vSize);
}


/*
void CopyMaskLBA(int spriteNum, int x, int y, byte * localBufferBrick, byte * buffer)
{
  unsigned char *ptr;
  int top;
  int bottom;
  int left;
  int right;
  unsigned char *outPtr;
  unsigned char *inPtr;
  unsigned char *outPtr2;
  int offset;
  int c1;
  int c2;
  int vc3;

  int temp;
  int iteration;
  int i;
  int j;

  return;

  UnSetClip();

  assert( textWindowLeft >= 0 );
  assert( textWindowRight <= 639 );
  assert( textWindowTop >= 0 );
  assert( textWindowBottom <= 479 );

  ptr = localBufferBrick + *(unsigned int *) (localBufferBrick + spriteNum * 4);

  left = x + *(ptr + 2);
  top = y + *(ptr + 3);
  right = *ptr + left - 1;
  bottom = *(ptr + 1) + top - 1;

  ptr += 4;

  if (left >= textWindowLeft && top >= textWindowTop && right <= textWindowRight && bottom <= textWindowBottom)
  {     // no crop
    right++;
    bottom++;

    outPtr = frontVideoBuffer + screenLockupTable[top] + left;
      inPtr = buffer + screenLockupTable[top] + left;

    offset = -((right - left) - largeurEcran);

    for (c1 = 0; c1 < bottom - top; c1++)
    {
      vc3 = *(ptr++);
      do
      {
        temp = *(ptr++); // skip size
        outPtr += temp;
        inPtr += temp;

        vc3--;
        if(!vc3)
          break;

        temp = *(ptr++); // copy size

        for(j=0;j<temp;j++)
        {
          *(outPtr++) = *(inPtr++);
        }
      }while(--vc3);
      outPtr += offset;
      inPtr += offset;
    }
  }
  else
  {
    return;
    if (left <= textWindowRight && top <= textWindowBottom && right >= textWindowLeft && bottom >= textWindowTop)
    {
      if (top < textWindowTop)  // si il manque un bout du haut de la brique
      {
        top -= textWindowTop;
        top =- top;
      }

      if (bottom > textWindowBottom)
        bottom = textWindowBottom;

      if (left < textWindowLeft)
        cropLeft = textWindowLeft - left;
      else
        cropLeft = 0;

      offset = -(left + cropLeft - 1 - right);

      right -= textWindowRight;
      if (right > 0)
        offset -= right;

      outPtr2 = screenLockupTable[top] + frontVideoBuffer;
      if (left >= 0)
        outPtr2 += left;
      outPtr = outBuffer;

      for (c1 = 0; c1 <= (bottom - top); c1++)
      {
        outPtr = outBuffer;

        for (i = 0; i < 512; i++)
          outBuffer[i] = 0;

        vc3 = *(ptr++);
        for (c2 = 0; c2 < vc3; c2++)
        {
          temp = *(ptr++);
          iteration = temp & 0x3F;
          if (temp & 0xC0)
          {
            iteration++;
            if (!(temp & 0x40))
            {
              temp = *(ptr++);
              for (i = 0; i < iteration; i++)
                *(outPtr++) = temp;
            }
            else
            {
              for (i = 0; i < iteration; i++)
                *(outPtr++) = *(ptr++);
            }
          }
          else
          {
            outPtr += iteration + 1;
          }
        }

        for (i = 0; i < offset; i++)
        {
          if (outBuffer[cropLeft + i] != 0)
            *(outPtr2) = outBuffer[cropLeft + i];
          outPtr2++;
        }
        outPtr2 += 640 - offset;
      }
    }
  }

}*/

void AddPhysBox(int left, int top, int right, int bottom) {
#ifdef USE_GL
	return;
#endif
	if (left < 0)
		left = 0;
	if (top < 0)
		top = 0;
	if (right >= 640)
		right = 639;
	if (bottom >= 480)
		bottom = 479;

	if (left > right || top > bottom)
		return;

	// DrawCadre (left, top, right, bottom);

	nextDirtyBoxList[fullRedrawVar8].left = left;
	nextDirtyBoxList[fullRedrawVar8].top = top;
	nextDirtyBoxList[fullRedrawVar8].right = right;
	nextDirtyBoxList[fullRedrawVar8].bottom = bottom;

	fullRedrawVar8++;

	addToRedrawBox(left, top, right, bottom);
}

//sort character list using bubble-sort
//TODO: make real use of stepSize to tell the scructure size
void SmallSort(drawListStruct *list, int listSize, int stepSize) {
	int i;
	int j;

	drawListStruct tempStruct;

	for (i = 0;i < listSize - 1;i++) {
		for (j = 0;j < listSize - 1 - i;j++) {
			if (list[j+1].field_0 < list[j].field_0) {
				memcpy(&tempStruct, &list[j+1], sizeof(drawListStruct));
				memcpy(&list[j+1], &list[j], sizeof(drawListStruct));
				memcpy(&list[j], &tempStruct, sizeof(drawListStruct));
			}
		}
	}
}

void FlipBoxes(void) {
	int i;
#ifdef USE_GL
	return;
#endif

	for (i = 0; i < numOfRedrawBox; i++) { //draw the dirty box on screen
		osystem_CopyBlockPhys(frontVideoBuffer, currentDirtyBoxList[i].left, currentDirtyBoxList[i].top, currentDirtyBoxList[i].right, currentDirtyBoxList[i].bottom);
	}

	numOfRedrawBox = 0;

	for (i = 0; i < fullRedrawVar8; i++) { //setup the dirty box for next display
		addToRedrawBox(nextDirtyBoxList[i].left, nextDirtyBoxList[i].top, nextDirtyBoxList[i].right, nextDirtyBoxList[i].bottom);
	}
}
