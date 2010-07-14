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

void Inventory(void) {
	int di = 1;
	int previouslySelectedObject;

	int localReinitVar1 = reinitVar1;
	int localReinitVar2 = reinitVar2;

	int oldLanguageCD1;
	int bx;

	CopyScreen(frontVideoBuffer, workVideoBuffer);

	SetLightVector(896, 950, 0);

	inventorySelectedColor = 68;

	if (numClover > 0)
		vars[27] = 1;

	DrawListInventory();

	oldLanguageCD1 = languageCD1;
	languageCD1 = 0;

	InitDial(2);

	bx = 3;

	TestCoulDial(4);
	InitDialWindow();

	while (skipIntro != 1) {
		readKeyboard();

		previouslySelectedObject = currentSelectedObjectInInventory;

		if (!di) {
			key = printTextVar12;
			mainLoopVar5 = key1;
			mainLoopVar7 = skipIntro;

			if (key != 0 || key1 != 0) {
				di = 1;
			}
		} else {
			mainLoopVar7 = 0;
			key = 0;
			mainLoopVar5 = 0;
			if (!printTextVar12 && !key1) {
				di = 0;
			}
		}

		if (mainLoopVar7 == 1 || mainLoopVar5 & 0x20)
			break;

		if (key & 2) {
			currentSelectedObjectInInventory++;

			if (currentSelectedObjectInInventory >= 28)
				currentSelectedObjectInInventory = 0;

			DrawOneInventory(previouslySelectedObject);

			bx = 3;
		}

		if (key & 1) {
			currentSelectedObjectInInventory--;

			if (currentSelectedObjectInInventory < 0)
				currentSelectedObjectInInventory = 27;

			DrawOneInventory(previouslySelectedObject);

			bx = 3;
		}

		if (key & 4) {
			currentSelectedObjectInInventory -= 4;

			if (currentSelectedObjectInInventory < 0)
				currentSelectedObjectInInventory += 28;

			DrawOneInventory(previouslySelectedObject);

			bx = 3;
		}

		if (key & 8) {
			currentSelectedObjectInInventory += 4;

			if (currentSelectedObjectInInventory >= 28)
				currentSelectedObjectInInventory -= 28;

			DrawOneInventory(previouslySelectedObject);

			bx = 3;
		}

		if (bx == 3) {
			SecondInitDialWindow();

			if (vars[(unsigned char)currentSelectedObjectInInventory] == 1 && !vars[70] && currentSelectedObjectInInventory <= 27) {
				OpenDialNoWindow(currentSelectedObjectInInventory + 100);
			} else {
				OpenDialNoWindow(128);
			}

			bx = 0;
		}

		if (bx != 2) {
			bx = printText10();
		}

		if (mainLoopVar5&1) {
			if (bx == 2) {
				SecondInitDialWindow();
				bx = 0;
			} else {
				if (vars[(unsigned char)currentSelectedObjectInInventory] == 1 && !vars[70] && currentSelectedObjectInInventory <= 27) {
					SecondInitDialWindow();
					OpenDialNoWindow(currentSelectedObjectInInventory + 100);
				}
			}
		}

		waitRetrace();

		DrawOneInventory(currentSelectedObjectInInventory);

		if ((mainLoopVar5 & 2) && vars[(unsigned char)currentSelectedObjectInInventory] == 1 && !vars[70] && currentSelectedObjectInInventory <= 27) {
			mainLoopVar9 = currentSelectedObjectInInventory;
			inventorySelectedColor = 91;
			DrawOneInventory(currentSelectedObjectInInventory);
			break;
		}
	}

	CloseDial();

	reinitVar1 = localReinitVar1;
	reinitVar2 = localReinitVar2;

	reinitAll1();

	languageCD1 = oldLanguageCD1;

	InitDial(currentTextBank + 3);

	while (skipIntro != 0 && key1 != 0) {
		readKeyboard();
	}

}

void SecondInitDialWindow(void) {
	blitRectangle(dialogueBoxLeft, dialogueBoxTop, dialogueBoxRight, dialogueBoxBottom, (char*)workVideoBuffer, dialogueBoxLeft, dialogueBoxTop, (char*)frontVideoBuffer);
	osystem_CopyBlockPhys(frontVideoBuffer, dialogueBoxLeft, dialogueBoxTop, dialogueBoxRight, dialogueBoxBottom);
	printText8Var3 = 0;
}

void CloseDial(void) {
	printTextVar13 = 0;
}

void OpenDialNoWindow(int itemNumber) {
	initText(itemNumber);
}

void DrawListInventory() {
	int object;

	drawBoxInsideTrans(17, 10, 622, 320, 4);
	DrawCadre(17, 10, 622, 320);
	Rect(110, 18, 188, 311, 75);
	osystem_CopyBlockPhys(frontVideoBuffer, 17, 10, 622, 320);

	for (object = 0;object < 28;object++) {
		DrawOneInventory(object);
	}

}

void DrawOneInventory(int objectNumber) {
	int objectX = (objectNumber / 4) * 85 + 64;
	int objectY = (objectNumber & 3) * 75 + 52;

	int left = objectX - 37;
	int right = objectX + 37;
	int top = objectY - 32;
	int bottom = objectY + 32;

	if (currentSelectedObjectInInventory == objectNumber)
		Box(left, top, right, bottom, inventorySelectedColor);
	else
		Box(left, top, right, bottom, 0);

	if (vars[objectNumber] && !vars[70] && objectNumber <= 27) {
		char* objectPtr;

		objectPtr = (char*)HQR_Get(HQR_Inventory, objectNumber);

		if (HQR_Flag)
			loadGfxSub((unsigned char*)objectPtr);

		objectRotation[objectNumber] += 8;

		Draw3dObject(objectX, objectY, objectPtr, objectRotation[objectNumber], 15000);

		if (objectNumber == 15) { // fuel object
			CoulFont(15);
			Font(left + 3, top + 32, Itoa(fuel));
		}
	}

	DrawCadre(left, top, right, bottom);
	osystem_CopyBlockPhys(frontVideoBuffer, left, top, right, bottom);
}

void Rect(int bottom, int right, int top, int left, int param) {
	Line(bottom, right, top, right, param);
	Line(bottom, left, top, left, param);
	Line(bottom, right, bottom, left, param);
	Line(top, right, top, left, param);
}
