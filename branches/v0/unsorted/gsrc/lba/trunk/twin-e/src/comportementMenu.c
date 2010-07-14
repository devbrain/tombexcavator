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

animTimerDataStruct animDataTab[4];

void DrawComportement(int lcomportement, int arg, int arg2) {
	unsigned char *currentAnim;
	int currentAnimSate;

	int box_left;
	int box_right;
	int box_top;
	int box_bottom;
	int var_10;

	box_left = lcomportement * 110 + 110;
	box_right = box_left + 99;
	box_top = 110;
	box_bottom = 229;

	currentAnim = HQR_Get(HQR_Anims, TCOS[lcomportement]);

	currentAnimSate = winTab[lcomportement];

	if (SetInterAnimObjet(currentAnimSate, (char *) currentAnim, (char *) menuCostumeIndex, &animDataTab[lcomportement])) {
		currentAnimSate++;  // keyFrame
		if (currentAnimSate == GetNbFramesAnim((char *) currentAnim)) { // max for anim
			currentAnimSate = GetBouclageAnim((char *) currentAnim);  // start of anim
		}
		winTab[lcomportement] = currentAnimSate;
	}

	if (arg2 == 0) {
		DrawCadre(box_left - 1, box_top - 1, box_right + 1, box_bottom + 1);
	}

	saveTextWindow();
	UnSetClip();

	if (lcomportement != comportementHero) {
		Box(box_left, box_top, box_right, box_bottom, 0); // color of unselected
	} else {
		Box(box_left, box_top, box_right, box_bottom, 68);  // color of selected

		Box(110, 239, 540, 279, 0); // comportement name box
		DrawCadre(110, 239, 540, 279);

		CoulFont(15);

		if (comportementHero == 2 && autoAgressivity == 1) {
			GetMultiText(4, dataString);  // pour afficher le "auto" ou non
		} else {
			GetMultiText(comportementHero, dataString);
		}

		Font((650 - SizeFont(dataString)) / 2, 240, dataString);  // ecrit le nom du comportement dans lequel on se trouve
	}

	var_10 = box_top;

	DrawObj3D(box_left, box_top, box_right, box_bottom, -600, arg, menuCostumeIndex); // dessine le model

	osystem_CopyBlockPhys(frontVideoBuffer, box_left, var_10, box_right, box_bottom);
	osystem_CopyBlockPhys(frontVideoBuffer, 110, 239, 540, 279);

	loadSavedTextWindow();
}

void DrawInfoMenu(short int arg_0, short int arg_4) {
	int var_10;
	int var_4;
	int var_8;
	int var_C;

	int var_14;

	int temp;
	int i;

	UnSetClip();
	DrawCadre(arg_0, arg_4, arg_0 + 450, arg_4 + 80);
	Box(arg_0 + 1, arg_4 + 1, arg_0 + 449, arg_4 + 79, 0);
	var_14 = arg_0 + 9;

	AffGraph(0, var_14, arg_4 + 13, HQR_Get(HQR_Sprites, 4));

	var_10 = arg_0 + 325;
	temp = arg_0 + 25;
	var_4 = RegleTrois32(temp, var_10, 50, twinsen->life);

	var_8 = arg_4 + 10;
	var_C = arg_4 + 25;
	Box(temp, var_8, var_4, var_C, 91);
	Box(var_4, var_8, var_10, var_C, 0);

	DrawCadre(arg_0 + 25, arg_4 + 10, arg_0 + 325, arg_4 + 10 + 15);

	if (!vars[70] && vars[4]) { // ca doit etre le mana
		AffGraph(0, var_14, arg_4 + 36, HQR_Get(HQR_Sprites, 5));
		if (magicLevel > 0) {
			Box(temp, arg_4 + 35, RegleTrois32(temp, var_10, 80, magicPoint), arg_4 + 50, 75);
		}
	}

	var_4 = arg_0 + 340;

	AffGraph(0, var_4, arg_4 + 15, HQR_Get(HQR_Sprites, 3));  // draw coins
	CoulFont(155);
	Font(arg_0 + 370, arg_4 + 5, Itoa(numCoin));  // amount of coins

	AffGraph(0, var_4, arg_4 + 55, HQR_Get(HQR_Sprites, 6));  // draw key
	CoulFont(155);
	Font(arg_0 + 370, arg_4 + 40, Itoa(numKey));

	for (i = 0; i < numCloverBox; i++) { // boites à trefles
		AffGraph(0, RegleTrois32(arg_0 + 25, arg_0 + 325, 10, i), arg_4 + 58, HQR_Get(HQR_Sprites, 41));
	}

	for (i = 0; i < numClover; i++) { // trefles
		AffGraph(0, RegleTrois32(arg_0 + 25, arg_0 + 325, 10, i) + 2, arg_4 + 60, HQR_Get(HQR_Sprites, 7));
	}

	osystem_CopyBlockPhys(frontVideoBuffer, arg_0, arg_4, arg_0 + 450, arg_4 + 80);
}

void drawMenuWin(short int var) {
	DrawCadre(100, 100, 550, 290);
	drawBoxInsideTrans(101, 101, 549, 289, 2);

	setAnimAtKeyFrame(winTab[0], HQR_Get(HQR_Anims, TCOS[0]), menuCostumeIndex, &animDataTab[0]);
	DrawComportement(0, var, 0);

	setAnimAtKeyFrame(winTab[1], HQR_Get(HQR_Anims, TCOS[1]), menuCostumeIndex, &animDataTab[1]);
	DrawComportement(1, var, 0);

	setAnimAtKeyFrame(winTab[2], HQR_Get(HQR_Anims, TCOS[2]), menuCostumeIndex, &animDataTab[2]);
	DrawComportement(2, var, 0);

	setAnimAtKeyFrame(winTab[3], HQR_Get(HQR_Anims, TCOS[3]), menuCostumeIndex, &animDataTab[3]);
	DrawComportement(3, var, 0);

	DrawInfoMenu(100, 300);

	osystem_CopyBlockPhys(frontVideoBuffer, 100, 100, 550, 290);
}

void processComportementMenu(void) {

	int languageCD1temp;
	int var_4;
	int savedComportement;

	freezeTime();

	if (comportementHero == 4) {
		HQ_StopSample();
		SetComportement(0);
	}

	menuCostumeIndex = bodyPtrTab[twinsen->costumeIndex];

	TCOS[0] = TCos0Init;
	TCOS[1] = TCos1Init;
	TCOS[2] = TCos2Init;
	TCOS[3] = TCos3Init;

	setActorAngleSafe(twinsen->angle, twinsen->angle - 256, 50, &timeVar);

	CopyScreen(frontVideoBuffer, workVideoBuffer);

	languageCD1temp = languageCD1;
	languageCD1 = 0;
	var_4 = currentTextBank;
	currentTextBank = -1;

	InitDial(0);

	drawMenuWin(twinsen->angle);

	savedComportement = comportementHero;

	setAnimAtKeyFrame(winTab[comportementHero], HQR_Get(HQR_Anims, TCOS[comportementHero]), menuCostumeIndex, &animDataTab[comportementHero]);

	readKeyboard();

	while (key1 & 4 || (skipIntro > 59 && skipIntro < 62)) {
		readKeyboard();

		key = printTextVar12;

		if (key & 8)
			comportementHero++;

		if (key & 4)
			comportementHero--;

		if (comportementHero < 0) // J'ai reecrit ca à ma sauce
			comportementHero = 3;

		if (comportementHero >= 4)
			comportementHero = 0;

		if (savedComportement != comportementHero) {
			DrawComportement(savedComportement, twinsen->angle, 1);
			savedComportement = comportementHero;
			setActorAngleSafe(twinsen->angle, twinsen->angle - 256, 50, &timeVar);  // rotate twinsen
			setAnimAtKeyFrame(winTab[comportementHero], HQR_Get(HQR_Anims, TCOS[comportementHero]), menuCostumeIndex, &animDataTab[comportementHero]);

			while (printTextVar12) {
				readKeyboard();
				DrawComportement(comportementHero, -1, 1);
			}
		}
		DrawComportement(comportementHero, -1, 1);
#ifndef PCLIKE
		lba_time += 3;
#endif
		osystem_updateImage();
	}

	SetComportement(comportementHero);

	reinitAll1();

	currentTextBank = var_4;
	InitDial(currentTextBank + 3);

	languageCD1 = languageCD1temp;

	unfreezeTime();
}
