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

void CheckZoneSce(actor * lactor, int actorNumber) {
	int currentX = lactor->X;
	int currentY = lactor->Z;
	int currentZ = lactor->Y;

	int var_C = 0;
	int i;
	ZONE_Box* pZone = zoneData;

	lactor->zone = -1;

	if (!actorNumber) {
		currentActorInZoneProcess = actorNumber;
	}

	for (i = 0; i < numOfZones; i++) {
		if ((currentX >= pZone->bottomLeft.X && currentX <= pZone->topRight.X) &&
				(currentZ >= pZone->bottomLeft.Y && currentZ <= pZone->topRight.Y) &&
				(currentY >= pZone->bottomLeft.Z && currentY <= pZone->topRight.Z)) { // if actor in zone
			switch (pZone->zoneType) {
			case 0: { // change room
					if (!actorNumber) { // only twinsen can change room
						if (lactor->life > 0) { // if not dead
							needChangeRoom = (char)pZone->data.ZONE_ChangeRoom.newRoomNumber;
							newTwinsenXByZone = lactor->X - pZone->bottomLeft.X + pZone->data.ZONE_ChangeRoom.positionX;
							newTwinsenZByZone = lactor->Y - pZone->bottomLeft.Y + pZone->data.ZONE_ChangeRoom.positionY;
							newTwinsenYByZone = lactor->Z - pZone->bottomLeft.Z + pZone->data.ZONE_ChangeRoom.positionZ;
							twinsenPositionModeInNewCube = 1;
						}
					}
					break;
				}
			case 1: { // opcode camera
					if (currentlyFollowedActor == actorNumber) {
						disableScreenRecenter = 1;
						if (newCameraX != pZone->data.ZONE_ForceCamera.newCameraX
								|| newCameraZ != pZone->data.ZONE_ForceCamera.newCameraY
								|| newCameraY != pZone->data.ZONE_ForceCamera.newCameraZ) {
							newCameraX = pZone->data.ZONE_ForceCamera.newCameraX;
							newCameraZ = pZone->data.ZONE_ForceCamera.newCameraY;
							newCameraY = pZone->data.ZONE_ForceCamera.newCameraZ;
							requestBackgroundRedraw = 1;
						}
					}
					break;
				}
			case 2: { // set zone
					lactor->zone = pZone->data.ZONE_SetActorZone.zoneNumber;
					break;
				}
			case 3: { // cube clip
					if (currentlyFollowedActor == actorNumber) {
						var_C = 1;
						if (useAnotherGrm != pZone->data.ZONE_PatchCube.newGrid) {
							if (pZone->data.ZONE_PatchCube.newGrid != -1) {
								createCube();
							}

							useAnotherGrm = pZone->data.ZONE_PatchCube.newGrid;
							currentGrid2 = i;
							freezeTime();
							IncrustGrm(pZone->data.ZONE_PatchCube.newGrid);
							unfreezeTime();
						}
					}
					break;
				}
			case 4: { // find object
					if (!actorNumber) { // only twinsen matter
						if (action != 0) { // if action button is pressed
							InitAnim(ANIM_activate, 1, 0, 0);
							ZoneGiveExtraBonus(pZone);
						}
					}
					break;
				}
			case 5: { // display text
					if (!actorNumber && action) {
						freezeTime();
						TestRestoreModeSVGA(1);
						TestCoulDial(pZone->data.ZONE_DisplayText.textColor);
						talkingActor = actorNumber;
						printTextFullScreen(pZone->data.ZONE_DisplayText.textIndex);
						unfreezeTime();
						fullRedraw(1);
//          waitForKey();
					}
					break;
				}
			case 6: { // ladder
					if (!actorNumber && (comportementHero != 4) && ((lactor->anim == ANIM_walk) || (lactor->anim == ANIM_reachTopOfLadder) || (lactor->anim == ANIM_climbUpLadder))) {
						Rotate(lactor->boudingBox.X.bottomLeft, lactor->boudingBox.Z.bottomLeft, lactor->angle + 0x580);
						destX += processActorX;
						destZ += processActorY;

						if (destX >= 0 && destZ >= 0 && destX <= 0x7E00 && destZ <= 0x7E00) {
							if (WorldColBrick(destX, lactor->Y + 0x100, destZ)) {
								currentActorInZoneProcess = 1;
								if (lactor->Y >= abs(pZone->bottomLeft.Y + pZone->topRight.Y) / 2) {
									InitAnim(ANIM_reachTopOfLadder, 2, 0, actorNumber);  //get out of ladder
								} else {
									InitAnim(ANIM_climbUpLadder, 0, 255, actorNumber); //go up ladder
								}
							}
						}
					}
					break;
				}
			default: {
					printf("Unsupported CheckZoneSce opcode %d for actor %d!\n", pZone->zoneType, actorNumber);
					exit(1);
				}
			}
		}
		pZone++;
	}

	if (!var_C) {
		if (actorNumber == currentlyFollowedActor) {
			if (useAnotherGrm != -1) {
				useAnotherGrm = -1;
				currentGrid2 = -1;
				createCube();
				requestBackgroundRedraw = 1;
			}
		}
	}
}

void ZoneGiveExtraBonus(ZONE_Box* pZone) {
	char* esi = (char*)pZone;
	int i;
	int di = 0;
	char var_8[8];
	char currentBonus;
	int index;
	int angle;

	if (!*(short int*)(esi + 0x14)) { // bonus not yet used
		for (i = 0;i < 5; i++) {
			if (*(short int*)(esi + 0x10)&(1 << (i + 4))) {
				var_8[di++] = i;
			}
		}

		if (di) {
			currentBonus = var_8[rand()%di];

			if (!magicLevel && currentBonus == 2) // if bonus is magic and magicLevel 1 not reached
				currentBonus = 1; //bonus is life

			angle = GetAngle(abs(pZone->topRight.X + pZone->bottomLeft.X) / 2, abs(pZone->topRight.Z + pZone->bottomLeft.Z) / 2, twinsen->X, twinsen->Z);
			index = ExtraBonus(abs(pZone->topRight.X + pZone->bottomLeft.X) / 2, pZone->topRight.Y, abs(pZone->topRight.Z + pZone->bottomLeft.Z) / 2, 180, angle, currentBonus + 3, *(short int*)(esi + 0x12));

			if (index != -1) {
				extraList[index].field_14 |= 0x400;
				*(short int*)(esi + 0x14) = 1;
			}
		}
	}
}

#ifdef GAME_DEBUG
void ZONE_DrawZones(void) {
	int currentZone;
	ZONE_Box* pZone = zoneData;

	for (currentZone = 0; currentZone <= numOfZones; currentZone++) {
		pointTab frontBottomLeftPoint;
		pointTab frontBottomRightPoint;

		pointTab frontTopLeftPoint;
		pointTab frontTopRightPoint;

		pointTab backBottomLeftPoint;
		pointTab backBottomRightPoint;

		pointTab backTopLeftPoint;
		pointTab backTopRightPoint;

		point3dStruct frontBottomLeftPoint2D;
		point3dStruct frontBottomRightPoint2D;

		point3dStruct frontTopLeftPoint2D;
		point3dStruct frontTopRightPoint2D;

		point3dStruct backBottomLeftPoint2D;
		point3dStruct backBottomRightPoint2D;

		point3dStruct backTopLeftPoint2D;
		point3dStruct backTopRightPoint2D;

		unsigned char color;

		pZone = &zoneData[currentZone];

		// compute the points in 3D

		frontBottomLeftPoint.x = pZone->bottomLeft.X - cameraX;
		frontBottomLeftPoint.y = pZone->bottomLeft.Y - cameraZ;
		frontBottomLeftPoint.z = pZone->topRight.Z - cameraY;

		frontBottomRightPoint.x = pZone->topRight.X - cameraX;
		frontBottomRightPoint.y = pZone->bottomLeft.Y - cameraZ;
		frontBottomRightPoint.z = pZone->topRight.Z - cameraY;

		frontTopLeftPoint.x = pZone->bottomLeft.X - cameraX;
		frontTopLeftPoint.y = pZone->topRight.Y - cameraZ;
		frontTopLeftPoint.z = pZone->topRight.Z - cameraY;

		frontTopRightPoint.x = pZone->topRight.X - cameraX;
		frontTopRightPoint.y = pZone->topRight.Y - cameraZ;
		frontTopRightPoint.z = pZone->topRight.Z - cameraY;

		backBottomLeftPoint.x = pZone->bottomLeft.X - cameraX;
		backBottomLeftPoint.y = pZone->bottomLeft.Y - cameraZ;
		backBottomLeftPoint.z = pZone->bottomLeft.Z - cameraY;

		backBottomRightPoint.x = pZone->topRight.X - cameraX;
		backBottomRightPoint.y = pZone->bottomLeft.Y - cameraZ;
		backBottomRightPoint.z = pZone->bottomLeft.Z - cameraY;

		backTopLeftPoint.x = pZone->bottomLeft.X - cameraX;
		backTopLeftPoint.y = pZone->topRight.Y - cameraZ;
		backTopLeftPoint.z = pZone->bottomLeft.Z - cameraY;

		backTopRightPoint.x = pZone->topRight.X - cameraX;
		backTopRightPoint.y = pZone->topRight.Y - cameraZ;
		backTopRightPoint.z = pZone->bottomLeft.Z - cameraY;

		// project all points

		MDL_DrawBoundingBox_ProjectPoints(&frontBottomLeftPoint,    &frontBottomLeftPoint2D);
		MDL_DrawBoundingBox_ProjectPoints(&frontBottomRightPoint,   &frontBottomRightPoint2D);
		MDL_DrawBoundingBox_ProjectPoints(&frontTopLeftPoint,       &frontTopLeftPoint2D);
		MDL_DrawBoundingBox_ProjectPoints(&frontTopRightPoint,      &frontTopRightPoint2D);
		MDL_DrawBoundingBox_ProjectPoints(&backBottomLeftPoint,     &backBottomLeftPoint2D);
		MDL_DrawBoundingBox_ProjectPoints(&backBottomRightPoint,    &backBottomRightPoint2D);
		MDL_DrawBoundingBox_ProjectPoints(&backTopLeftPoint,        &backTopLeftPoint2D);
		MDL_DrawBoundingBox_ProjectPoints(&backTopRightPoint,       &backTopRightPoint2D);

		// draw all lines

#ifdef PCLIKE
		switch (pZone->zoneType) {
		case 0: {
				if (!bShowCubeChangeZones)
					continue;
				break;
			}
		case 1: {
				if (!bShowCameraZones)
					continue;
				break;
			}
		case 2: {
				if (!bShowScenaricZones)
					continue;
				break;
			}
		case 3: {
				if (!bShowGRMZones)
					continue;
				break;
			}
		case 4: {
				if (!bShowObjZones)
					continue;
				break;
			}
		case 5: {
				if (!bShowTextZones)
					continue;
				break;
			}
		case 6: {
				if (!bShowLadderZones)
					continue;
				break;
			}
		default: {
				continue;
				break;
			}
		}
#endif

		color = 15 * 3 + pZone->zoneType * 16;
#ifdef USE_GL
		// draw front part
		osystem_addLine(frontBottomLeftPoint2D.x, frontBottomLeftPoint2D.y, frontBottomLeftPoint2D.z, frontTopLeftPoint2D.x, frontTopLeftPoint2D.y, frontTopLeftPoint2D.z, color);
		osystem_addLine(frontTopLeftPoint2D.x, frontTopLeftPoint2D.y, frontTopLeftPoint2D.z, frontTopRightPoint2D.x, frontTopRightPoint2D.y, frontTopRightPoint2D.z, color);
		osystem_addLine(frontTopRightPoint2D.x, frontTopRightPoint2D.y, frontTopRightPoint2D.z, frontBottomRightPoint2D.x, frontBottomRightPoint2D.y, frontBottomRightPoint2D.z, color);
		osystem_addLine(frontBottomRightPoint2D.x, frontBottomRightPoint2D.y, frontBottomRightPoint2D.z, frontBottomLeftPoint2D.x, frontBottomLeftPoint2D.y, frontBottomLeftPoint2D.z, color);

		// draw top part
		osystem_addLine(frontTopLeftPoint2D.x, frontTopLeftPoint2D.y, frontTopLeftPoint2D.z, backTopLeftPoint2D.x, backTopLeftPoint2D.y, backTopLeftPoint2D.z, color);
		osystem_addLine(backTopLeftPoint2D.x, backTopLeftPoint2D.y, backTopLeftPoint2D.z, backTopRightPoint2D.x, backTopRightPoint2D.y, backTopRightPoint2D.z, color);
		osystem_addLine(backTopRightPoint2D.x, backTopRightPoint2D.y, backTopRightPoint2D.z, frontTopRightPoint2D.x, frontTopRightPoint2D.y, frontTopRightPoint2D.z, color);
		osystem_addLine(frontTopRightPoint2D.x, frontTopRightPoint2D.y, frontTopRightPoint2D.z, frontTopLeftPoint2D.x, frontTopLeftPoint2D.y, frontTopLeftPoint2D.z, color);

		// draw back part
		osystem_addLine(backBottomLeftPoint2D.x, backBottomLeftPoint2D.y, backBottomLeftPoint2D.z, backTopLeftPoint2D.x, backTopLeftPoint2D.y, backTopLeftPoint2D.z, color);
		osystem_addLine(backTopLeftPoint2D.x, backTopLeftPoint2D.y, backTopLeftPoint2D.z, backTopRightPoint2D.x, backTopRightPoint2D.y, backTopRightPoint2D.z, color);
		osystem_addLine(backTopRightPoint2D.x, backTopRightPoint2D.y, backTopRightPoint2D.z, backBottomRightPoint2D.x, backBottomRightPoint2D.y, backBottomRightPoint2D.z, color);
		osystem_addLine(backBottomRightPoint2D.x, backBottomRightPoint2D.y, backBottomRightPoint2D.z, backBottomLeftPoint2D.x, backBottomLeftPoint2D.y, backBottomLeftPoint2D.z, color);

		// draw bottom part
		osystem_addLine(frontBottomLeftPoint2D.x, frontBottomLeftPoint2D.y, frontBottomLeftPoint2D.z, backBottomLeftPoint2D.x, backBottomLeftPoint2D.y, backBottomLeftPoint2D.z, color);
		osystem_addLine(backBottomLeftPoint2D.x, backBottomLeftPoint2D.y, backBottomLeftPoint2D.z, backBottomRightPoint2D.x, backBottomRightPoint2D.y, backBottomRightPoint2D.z, color);
		osystem_addLine(backBottomRightPoint2D.x, backBottomRightPoint2D.y, backBottomRightPoint2D.z, frontBottomRightPoint2D.x, frontBottomRightPoint2D.y, frontBottomRightPoint2D.z, color);
		osystem_addLine(frontBottomRightPoint2D.x, frontBottomRightPoint2D.y, frontBottomRightPoint2D.z, frontBottomLeftPoint2D.x, frontBottomLeftPoint2D.y, frontBottomLeftPoint2D.z, color);
#else
		// draw front part
		drawLine(frontBottomLeftPoint2D.x, frontBottomLeftPoint2D.y, frontTopLeftPoint2D.x, frontTopLeftPoint2D.y, color);
		drawLine(frontTopLeftPoint2D.x, frontTopLeftPoint2D.y, frontTopRightPoint2D.x, frontTopRightPoint2D.y, color);
		drawLine(frontTopRightPoint2D.x, frontTopRightPoint2D.y, frontBottomRightPoint2D.x, frontBottomRightPoint2D.y, color);
		drawLine(frontBottomRightPoint2D.x, frontBottomRightPoint2D.y, frontBottomLeftPoint2D.x, frontBottomLeftPoint2D.y, color);

		// draw top part
		drawLine(frontTopLeftPoint2D.x, frontTopLeftPoint2D.y, backTopLeftPoint2D.x, backTopLeftPoint2D.y, color);
		drawLine(backTopLeftPoint2D.x, backTopLeftPoint2D.y, backTopRightPoint2D.x, backTopRightPoint2D.y, color);
		drawLine(backTopRightPoint2D.x, backTopRightPoint2D.y, frontTopRightPoint2D.x, frontTopRightPoint2D.y, color);
		drawLine(frontTopRightPoint2D.x, frontTopRightPoint2D.y, frontTopLeftPoint2D.x, frontTopLeftPoint2D.y, color);

		// draw back part
		drawLine(backBottomLeftPoint2D.x, backBottomLeftPoint2D.y, backTopLeftPoint2D.x, backTopLeftPoint2D.y, color);
		drawLine(backTopLeftPoint2D.x, backTopLeftPoint2D.y, backTopRightPoint2D.x, backTopRightPoint2D.y, color);
		drawLine(backTopRightPoint2D.x, backTopRightPoint2D.y, backBottomRightPoint2D.x, backBottomRightPoint2D.y, color);
		drawLine(backBottomRightPoint2D.x, backBottomRightPoint2D.y, backBottomLeftPoint2D.x, backBottomLeftPoint2D.y, color);

		// draw bottom part
		drawLine(frontBottomLeftPoint2D.x, frontBottomLeftPoint2D.y, backBottomLeftPoint2D.x, backBottomLeftPoint2D.y, color);
		drawLine(backBottomLeftPoint2D.x, backBottomLeftPoint2D.y, backBottomRightPoint2D.x, backBottomRightPoint2D.y, color);
		drawLine(backBottomRightPoint2D.x, backBottomRightPoint2D.y, frontBottomRightPoint2D.x, frontBottomRightPoint2D.y, color);
		drawLine(frontBottomRightPoint2D.x, frontBottomRightPoint2D.y, frontBottomLeftPoint2D.x, frontBottomLeftPoint2D.y, color);
#endif
	}
}

#endif
