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

short int var4 = 0;

void MainGameMenu(void) {
//    short int var4; // disabled to be able to quit the game without problems
	int temp;
	int temp2;
	int temp3;

	// FILE* SaveGame;

	HQ_StopSample();
//    var4 = 0; // disabled to be able to quit the game without problems
	CopyScreen(frontVideoBuffer, workVideoBuffer);

	if (var4 != 0)
		return;

	do {
		InitDial(0);
		playCDtrack(9);
		HQ_StopSample();
		GetMultiText(49, mainMenuVar1);
		temp = processMenu(mainMenuData);

		if (temp == 20) // New Game
			//  if( 1)
		{
			if (enterPlayerName(42)) {
				/*
				 * do // Cette boucle essai des noms de fichiers jusqu'a trouver un nom de fichier
				 * vide. Probleme si on a plus de 9999 sauvegarde dans le meme repertoire {
				 * saveGameFileName[0]=mainMenuVar3; saveGameFileName[1]=0; // pas dans le code
				 * original.. // random pour avoir un num entre 0000 et 9999
				 * strcat(saveGameFileName,mainMenu7(10000*rand()));
				 * strcat(saveGameFileName,".LBA"); SaveGame=checkIfFileExist(saveGameFileName);
				 * }while(!SaveGame);
				 */
				reinitAll(1);
				newGame();
				if (mainLoop()) {
					gameStaffIsDisplayed = 1;

					temp2 = shadowMode;
					temp3 = languageCD1;
					languageCD1 = 0;
					shadowMode = 0;
					reinitAll(1);
					currentRoom = 119;
					needChangeRoom = 119;
					mainLoop();
					gameStaffIsDisplayed = 0;

					languageCD1 = temp3;
					shadowMode = temp2;

					Cls();
					osystem_Flip(frontVideoBuffer);
					PlayAnimFla("The_end");
					Cls();
					osystem_Flip(frontVideoBuffer);
					osystem_setPalette(menuPalRGBA);
				}
				CopyScreen(frontVideoBuffer, workVideoBuffer);
// 				do {
// 					do {
// 						readKeyboard();
// 					} while (key1 != 0);
// 				} while (skipIntro != 0);
			}
		} else if (temp == 21) { // Continue Game
			if (chooseSave(21)) {
				reinitAll(-1);
				newGame();
				if (mainLoop()) {
					gameStaffIsDisplayed = 1;

					temp2 = shadowMode;
					temp3 = languageCD1;
					languageCD1 = 0;
					shadowMode = 0;
					reinitAll(1);
					currentRoom = 119;
					needChangeRoom = 119;
					mainLoop();
					gameStaffIsDisplayed = 0;

					languageCD1 = temp3;
					shadowMode = temp2;
					Cls();
					osystem_Flip(frontVideoBuffer);
					PlayAnimFla("The_end");

					Cls();
					osystem_Flip(frontVideoBuffer);
					osystem_setPalette(menuPalRGBA);
				}
				CopyScreen(frontVideoBuffer, workVideoBuffer);
				do {
					do {
						readKeyboard();
					} while (key1 != 0);
				} while (skipIntro != 0);
			}
		} else if (temp == 22) { // Quit
			printf("\nLBA/Relentless TwinEngine < %s / %s >\n\nOK.\n", __DATE__, __TIME__);
			var4 = 1;
			//	breakmainLoop =  true;
		} else if (temp == 23) { // Options
			CopyScreen(workVideoBuffer, frontVideoBuffer);
			osystem_Flip(frontVideoBuffer);
			soundMenuData[5] = 26;
			optionMenu();
		} else if (temp == 9999) {
			// playSubVideo();

			Load_HQR("ress.hqr", workVideoBuffer, 14);
			CopyScreen(workVideoBuffer, frontVideoBuffer);

			SetBackPal();
			osystem_Flip(frontVideoBuffer);
			FadeToPal((char *) menuPalRGBA);
		}
	} while (!var4); // once var4 is set it will imediatly quit the game
}

void HQ_StopSample(void) {
	// todo: implement this
}

int chooseSave(int param) {
	FILE* fHandle;

	fHandle = fopen("SAVE.LBA", "rb");
	if (!fHandle) {
		return 0;
	}

	fclose(fHandle);

	/*  var_14 = 1;
	  var_0C = 50;

	  CopyScreen(workVideoBuffer, frontVideoBuffer);
	  osystem_Flip(frontVideoBuffer);
	  var_24 = alloc(1500);
	  var_28 = alloc(200);

	  var_8 = findAllSaves(var_28,var_24);

	  if(!var_8)
	      return(0);

	  InitDial(0);

	  drawSaveNameButton(320,40,GetMultiText(21,var_12C));*/


	return (1);
}

char *GetMultiText(int a, char *b) {
	if (a == mainMenu3Var1) {
		if (mainMenu3Var2 == currentTextBank) {
			strcpy(b, mainMenu3Var3);
			return (b);
		}
	}
	if (!findString(a)) {
		b[0] = 0;   // si on a pas trouve le text...
		return (0);
	}

	if ((currentTextLength - 1) > 0xFF)
		currentTextLength = 0xFF;

	copyStringToString((char *) currentTextPtr, b, currentTextLength);  // on copie dans b
	// *(currentTextPtr+currentTextLength)=0; // on ajoute un \n a la fin
	currentTextLength++;  // on augmente la taille de la string de 1
	copyStringToString(b, mainMenu3Var3, currentTextLength);  // enfin, on copie le tout dans mainMenu3Var3

	mainMenu3Var1 = a;
	mainMenu3Var2 = currentTextBank;

	return (b);
}

int processMenu(short int *menuData) {
	int localTime;
	int numEntry;
	int buttonNeedRedraw;
	int maxButton;
	short int *localData = menuData;
	short int currentButton;
	short int temp3;
	int musicChanged;
	int buttonReleased = 1;

	musicChanged = 0;

	buttonNeedRedraw = 1;

	Load_HQR("ress.hqr", bufSpeak, 51);

	numEntry = localData[1];
	currentButton = localData[0];
	localTime = lba_time;
	maxButton = numEntry - 1;

	do {
		if (localData == mainMenuData) { // si on est dans le menu principal
			if (lba_time - localTime <= 11650) {
				if (skipIntro == 46)
					if (key1 != 32)
						return (9999);
			} else {
				return (9999);
			}
		}

		if (printTextVar12 == 0) {
			buttonReleased = 1;
		}

		if (buttonReleased) {
			key = printTextVar12;

			if (((byte) key & 2)) { // on passe au bouton d'en dessous
				currentButton++;
				if (currentButton == numEntry) {
					currentButton = 0;  // on se retrouve au dessus
				}
				buttonNeedRedraw = 1;
				buttonReleased = 0;
			}

			if (((byte) key & 1)) { // on passe au bouton d'au dessus
				currentButton--;
				if (currentButton < 0) {
					currentButton = maxButton;  // on se retrouve au dessous
				}
				buttonNeedRedraw = 1;
				buttonReleased = 0;
			}

			temp3 = *(localData + currentButton * 4 + 8); // on recupere le parametre du
			// bouton
			if (temp3 <= 5) { // si c'est un bouton de volume
				switch (temp3) {
				case 1: {
					}
				case 2: {
					}
				case 3: {
					}
				case 4: {
					}
				case 5: {
					}
				}
			}
		}

		if (buttonNeedRedraw == 1) {
			*localData = currentButton;

			drawButton(localData, 0); // le bouton actuel
			do {
				readKeyboard();
				drawButton(localData, 1);
				osystem_updateImage();
			} while (printTextVar12 == 0 && key1 == 0 && skipIntro == 0);
			buttonNeedRedraw = 0;
		} else {
			if (musicChanged) {
				// todo: update des volumes...
			}

			buttonNeedRedraw = 0;
			drawButton(localData, 1);
			readKeyboard();
		}

		osystem_updateImage();
	} while (!(key1 & 2) && !(key1 & 1));

	currentButton = *(localData + 5 + currentButton * 2); // on recupere le point de sortie

	readKeyboard();

	SDL_Delay(100);

	return (currentButton);
}

void drawSelectableLetters(void) {
	char x, y;

	for (x = 0; x < 5; x++)
		for (y = 0; y < 14; y++)
			drawSelectableLetter(x, y, 0);
}

void drawSelectableLetter(int x, int y, int arg) {
	char buffer[256];

	// char param;
	int centerX, left, top, centerY, bottom, right, right2;

	buffer[0] = allowedCharIndex[y + x * 14];

	centerX = y * 45 + 25;

	left = centerX - 20;

	right = centerX + 20;

	top = x * 56 + 200 - 25;

	buffer[1] = 0;

	centerY = x * 56 + 200;

	bottom = x * 56 + 200 + 25;

	if (arg != 0) {
		Box(left, top, right, bottom, 91);
	} else {
		blitRectangle(left, top, right, bottom, (char *) workVideoBuffer, left, top, (char *)frontVideoBuffer);
		right2 = right;
		drawBoxInsideTrans(left, top, right2, bottom, 4);
	}

	DrawCadre(left, top, right, bottom);
	right2 = right;

	CoulFont(15);

	Font(centerX - SizeFont(buffer) / 2, centerY - 18, buffer);

	osystem_CopyBlockPhys(frontVideoBuffer, left, top, right2, bottom);

}

int enterPlayerName(short int param) {
	char buffer[256];

	short int a, b, c, e;
//    char key;

	e = 1;
	a = -1;
	b = 0;
	c = 0;

	CopyScreen(workVideoBuffer, frontVideoBuffer);
	osystem_Flip(frontVideoBuffer);
	InitDial(0);
	GetMultiText(param, buffer);
	CoulFont(15);
	Font(320 - (SizeFont(buffer) / 2), 20, buffer);
	osystem_CopyBlockPhys(frontVideoBuffer, 0, 0, 639, 99);
	playerName[0] = enterPlayerNameVar1;
	// drawSmallButton(320,100,playerName,1);
	drawSelectableLetters();

	/* do
	{
	   readKeyboard();
	}
	 while (skipIntro == 0 && key1 == 0);

	 do
	{
	   key = (char) getKeyboardChar();
	}
	 while (!key);*/

	// todo: implement this

	enterPlayerNameVar2 = 0;
	CopyScreen(workVideoBuffer, frontVideoBuffer);
	osystem_Flip(frontVideoBuffer);

	return (1);

	return (c);
}

char *Itoa(int valeur) {
	// toto: implement this
	char *text;

	text = Malloc(256);
	sprintf(text, "%d", valeur);

	return (text);
	// return("test");
}

void drawButton(short int *data, int a) {
	int buttonNumber;
	int maxButton;

	// int var4;
	unsigned short int *localData = data;
	int temp;
	unsigned char temp2;
	unsigned short temp3;
	char currentButton;

	buttonNumber = *localData;
	localData += 1;
	maxButton = *localData;
	localData += 1;
	temp = *localData;
	localData += 2;

	if (temp == 0) {
		temp = 35;
	} else {
		temp = temp - (((maxButton - 1) * 6) + ((maxButton) * 50)) / 2;
	}

	if (maxButton <= 0) {
		return;

	}

	currentButton = 0;

	do {
		temp2 = (unsigned char) * localData;
		localData += 1;
		temp3 = *localData;
		localData += 1;
		if (a != 0) {

			if (currentButton == buttonNumber) {
				drawButtonGFX(320, temp, temp2, temp3, 1);

			}
		} else {
			if (currentButton == buttonNumber) {
				drawButtonGFX(320, temp, temp2, temp3, 1);
			} else {
				drawButtonGFX(320, temp, temp2, temp3, 0);
			}
		}

		currentButton++;
		temp += 56;
	} while (currentButton < maxButton);
}

void drawButtonGFX(int largeur, int posY, int c, int d, int mode) {

	int right;
	int top;
	int left;
	int bottom2;
	int bottom;

	/*
	 * int CDvolumeRemaped; int musicVolumeRemaped; int masterVolumeRemaped; int lineVolumeRemaped;
	 * int waveVolumeRemaped;
	 */

	char buttonText[256];
	int stringLength;

	left = largeur - buttonDrawVar1 / 2;
	right = largeur + buttonDrawVar1 / 2;

	top = posY - 25;
	bottom = bottom2 = posY + 25;

	if (mode != 0) {
		if (c <= 5 && c >= 1) {
			switch (c) {
			case 1: {
					// musicVolumeRemaped=RegleTrois32(left,right,255,musicVolume);
				}
			case 2: {
					// waveVolumeRemaped=RegleTrois32(left,right,255,waveVolume);

				}
			case 3: {
					// CDVolumeRemaped=RegleTrois32(left,right,255,CDVolume);
				}
			case 4: {
					// lineVolume=RegleTrois32(left,right,255,lineVolume);
				}
			case 5:

				{
					// masterVolume=RegleTrois32(left,right,255,masterVolume);
				}
			};
		} else {
			makeFireEffect(top, 64);
			if (!(rand() % 5)) {
				bufSpeak[rand() % 320 * 10 + 6400] = 255;
			}
		}

		if (c <= 5 && c >= 1) {
			// implement this
		}

	} else {
		blitRectangle(left, top, right, bottom, (char *) workVideoBuffer, left, top,
					  (char *) frontVideoBuffer);

		drawBoxInsideTrans(left, top, right, bottom2, 4);
	}

	DrawCadre(left, top, right, bottom);
	CoulFont(15);
	GetMultiText(d, buttonText);
	stringLength = SizeFont(buttonText);
	Font(largeur - (stringLength / 2), posY - 18, buttonText);

	// todo: implementer les boutons de volume...

	osystem_CopyBlockPhys(frontVideoBuffer, left, top, right, bottom);

}

void blitRectangle(int left, int top, int right, int bottom, char *source, int leftDest, int topDest, char *dest) {
	int largeur;
	int hauteur;

	char *s;
	char *d;

	int interligne;
	int temp3;
	int i;
	int j;

	assert(left >= 0);
	assert(right < 640);
	assert(top >= 0);
	assert(bottom < 480);

	s = screenLockupTable[top] + source + left;
	d = screenLockupTable[topDest] + dest + leftDest;

	largeur = right - left + 1;

	hauteur = bottom - top + 1;

	interligne = largeurEcran - largeur;
	temp3 = left;

	left >>= 2;
	temp3 &= 3;

	for (j = 0; j < hauteur; j++) {
		for (i = 0; i < largeur; i++) {
			*(d++) = *(s++);
		}

		d += interligne;
		s += interligne;
	}
}

void drawBoxInsideTrans(int left, int top, int right, int bottom, int mode) {

	byte *pos;
	int largeur;
	int hauteur;
	int hauteur2;
	int temp;
	int localMode;
	int var1;
	char color;
	char color2;

	if (left > textWindowRight)
		return;
	if (right < textWindowLeft)
		return;
	if (top > textWindowBottom)
		return;
	if (bottom < textWindowTop)
		return;

	if (left < textWindowLeft)
		left = textWindowLeft;
	if (right > textWindowRight)

		right = textWindowRight;
	if (top < textWindowTop)
		top = textWindowTop;
	if (bottom > textWindowBottom)
		bottom = textWindowBottom;

	pos = screenLockupTable[top] + frontVideoBuffer + left;
	hauteur2 = hauteur = bottom - top;
	hauteur2++;

	largeur = right - left + 1;

	temp = largeurEcran - largeur;
	localMode = mode;

	do {
		var1 = largeur;
		do {
			color2 = color = *pos;
			color2 &= 0xF0;
			color &= 0x0F;
			color -= localMode;
			if (color < 0)
				color = color2;
			else
				color += color2;
			*pos++ = color;
			var1--;
		} while (var1 > 0);
		pos += temp;
		hauteur2--;
	} while (hauteur2 > 0);
}

void DrawCadre(int left, int top, int right, int bottom) {
	Line(left, top, right, top, 79);  // ligne du haut
	Line(left, top, left, bottom, 79);  // ligne de gauche
	Line(right, ++top, right, bottom, 73);  // ligne de droite
	Line(++left, bottom, right, bottom, 73);  // ligne du bas
}

void LineLBA(int a, int b, int c, int d, int e) {
	drawLine(a, b, c, d, e);
}

// a=x1
// b=y1
// c=x2
// d=y2;
/*void drawLine(int X1, int Y1, int X2, int Y2, int color)
{
    int temp;
    short int flag;
    int flag2;
    unsigned char *out;
    short int var2;
    short int xchg;

    osystem_drawLine(X1,Y1,X2,Y2,color,palette);
    return;

    currentLineColor = color;

    if (X1 > X2)      // pour toujours dessiner de gauche à droite
  {
      temp = X2;
      X2 = X1;
      X1 = temp;

      temp = Y2;
      Y2 = Y1;
      Y1 = temp;

  }

    do
    {
        flag = 0;

        if (X1 < textWindowLeft)
      {
          flag |= 1;
      }
        else
      {
          if (X1 > textWindowRight)
        return; // line completly out of screen
      }
        if (Y1 < textWindowTop)
      {
          flag |= 8;
      }
        else
      {
          if (Y1 > textWindowBottom)
        flag |= 4;
      }

        flag <<= 8;

        if (X2 < textWindowLeft)
      return; // line out of screen

        if (X2 > textWindowRight)
      flag |= 2;

        if (Y2 < textWindowTop)
      {
          flag |= 8;
      }
        else
      {
          if (Y2 > textWindowBottom)
        flag |= 4;
      }

        flag2 = flag >> 8;

        if (flag & flag2)
      return;

        flag2 |= flag;

        if(flag2)
        {
            int step1=Y1-X1;
            int step2=Y2-X2;

            if(flag & 0x100) // crop left
            {
                return;
                X1=-(X1-textWindowLeft);
                X1*=step2;
                X1/=step1;
                X2+=X1;
                X1=textWindowLeft;
            }
            else
            if(flag & 0x800)
            {
                return;
            }
            else
            if(flag & 0x400)
            {
                return;
            }
            else
            if(flag & 0x2) // crop right
            {
                Y1=X1;
                X1=textWindowRight;
                X1-=Y1;
                X1*=step2;
                X1/=step1;
                Y2=X1;
                Y2+=X2;
                X1=Y1;
                Y1=textWindowRight;
            }
            else
            if(flag & 0x8)
            {
                return;
            }
            else
            if(flag & 0x4)
            {
                return;
            }
        }
        else
        {
            break;
        }
    }
    while(1);

   // implementer la suite

    flag2 = 640;    // esi
    X2 -= X1;
    Y2 -= Y1;
    if (Y2 < 0)
  {
      flag2 = -flag2;
      Y2 = -Y2;
  }

    out = frontVideoBuffer + screenLockupTable[Y1] + X1;

    if (X2 < Y2)      // pente importante
  {
      xchg = X2;
      X2 = Y2;
      Y2 = xchg;
      var2 = X2;
      var2 <<= 1;
      Y1 = X2;
      Y2 <<= 1;
      X1++;
      do
    {
        *out = (unsigned char) color;
        Y1 -= Y2;
        if (Y1 > 0)
      {
          out += flag2;
      }
        else
      {
          Y1 += var2;
          out += flag2 + 1;
      }
    }
      while (--X2);
  }
    else      // pente reduite
  {
      var2 = X2;
      var2 <<= 1;
      Y1 = X2;
      Y2 <<= 1;
      X1++;
      do
    {
        *out = (unsigned char) color;
        out++;
        Y1 -= Y2;
        if (Y1 < 0)
      {
          Y1 += var2;
          out += flag2;
      }
    }
      while (--X2);
  }
}*/

int SizeFont(char *string) {
	unsigned char caractere;
	stringLenght = 0;

	do {
		caractere = (unsigned char) * (string++);

		if (caractere == 0)
			break;
		if (caractere == 0x20) {
			stringLenght += spaceLenght;
		}

		else {
			stringLenght += interCharSpace; // espace inter lettre
			stringLenght += *(fntFont + convertWFromLE(*((short int *)(fntFont + caractere * 4))));
		}
	} while (1);

	return (stringLenght);
}

void copyStringToString(char *a, char *b, int c) {
	int i;

	for (i = 0; i < c; i++)
		*(b++) = *(a++);
}

int optionMenu(void) { // Options menu
	byte temp;
	byte temp2 = 0;

	CopyScreen(frontVideoBuffer, workVideoBuffer);
	HQ_StopSample();

	playCDtrack(9);   // warning... Truc pas trop gere avec BX là...

	do {
		temp = processMenu(soundMenuData);

		if (temp == 15 || temp == 26) { // Return to Game
			temp2 = 1;
		} else if (temp == 30) { // Volume Settings
			CopyScreen(workVideoBuffer, frontVideoBuffer);
			osystem_Flip(frontVideoBuffer);
			// volumeMenu();
		} else if (temp == 46) { // Saved Gamee Management

			CopyScreen(workVideoBuffer, frontVideoBuffer);
			osystem_Flip(frontVideoBuffer);
			// saveManipMenu();

		} else if (temp == 47) { // Advenced Options
			CopyScreen(workVideoBuffer, frontVideoBuffer);
			osystem_Flip(frontVideoBuffer);
			optionMenu2();
		}
	} while (temp2 != 1);

	CopyScreen(workVideoBuffer, frontVideoBuffer);
	osystem_Flip(frontVideoBuffer);

	return (0);
}

void optionMenu2(void) { // Advanced Options
	byte temp;
	byte quit = 0;

	CopyScreen(frontVideoBuffer, workVideoBuffer);

	do {
		temp = processMenu(subMenuData);

		if (temp == 33) { // No Scenery Zoom
			subMenuData[13] = 233;
			//  zoomMode=0;
		} else if (temp == 231) {
			subMenuData[9] = 31;
		} else if (temp == 232) { // Shadows for all animated objects
			subMenuData[11] = 32;
			shadowMode = 2;
		} else if (temp == 233) { // Scenery Zoom On
			subMenuData[13] = 33;
			//  zoomMode=1;
		} else if (temp == 131) {
			subMenuData[9] = 231;
		} else if (temp == 132) { // No Shadows
			subMenuData[11] = 232;
			shadowMode = 0;
		} else if (temp == 26) { // Back to previous menu
			quit = 1;

		} else if (temp == 31) {
			subMenuData[9] = 131;
		} else if (temp == 32) { // Character shadows
			subMenuData[11] = 132;
			shadowMode = 1;
		} else if (temp == 2) { // Aggressive Manual
			subMenuData[7] = 4;
			autoAgressivity = 0;
		} else if (temp == 4) { // Aggressive Auto
			subMenuData[7] = 2;
			autoAgressivity = 1;
		}

	} while (!quit);

	// todo: implementer le process interne

	CopyScreen(workVideoBuffer, frontVideoBuffer);
	osystem_Flip(frontVideoBuffer);
}

/*void abort(void)
{
}*/
