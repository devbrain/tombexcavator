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

struct ZONE_BoxPoint {
	short int X;
	short int Y;
	short int Z;
};

typedef struct ZONE_BoxPoint ZONE_BoxPoint;

struct ZONE_Box {
	ZONE_BoxPoint bottomLeft;
	ZONE_BoxPoint topRight;

	short int zoneType;

	union {
		struct {
			short int newRoomNumber;
			short int positionX;
			short int positionY;
			short int positionZ;
		} ZONE_ChangeRoom;
		struct {
			short int dummy;
			short int newCameraX;
			short int newCameraY;
			short int newCameraZ;
		} ZONE_ForceCamera;
		struct {
			short int zoneNumber;
		} ZONE_SetActorZone;
		struct {
			short int newGrid;
		} ZONE_PatchCube;
		struct {
			short int textIndex;
			short int textColor;
		} ZONE_DisplayText;
		struct {
			short int data1;
			short int data2;
			short int data3;
			short int data4;
		} generic;
	}data;
	short int dummy;
};

typedef struct ZONE_Box ZONE_Box;
