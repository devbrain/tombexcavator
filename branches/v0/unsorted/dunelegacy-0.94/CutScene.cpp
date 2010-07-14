#include "sand.h"

#include "CutScene.h"

#include "zlib/SDL_rwops_zzip.h"



#define CS_FRAME_TIME	150



SDL_Color wwcolours[256];

bool skipIndividScene = false;

SDL_Rect cutScenePos;

SDL_Rect screenRect;

bool hasPalette = false;



int colour_red, colour_blue, colour_green, colour_purple, colour_default;



DuneCutScene::DuneCutScene()

{

	skipScene = false;

	nScenes = 0;

	curScene = 0;

	textSurface = NULL;



	colour_red = COLOUR_RED;

	colour_blue = COLOUR_BLUE;

	colour_green = COLOUR_GREEN;

	colour_purple = COLOUR_SARDAUKAR;

	colour_default = colour_red;



	/*

	for(int i=0;i<256;i++)

	{

		colours[255-i].r = palette->colors[i].r;

		colours[255-i].g = palette->colors[i].g;

		colours[255-i].b = palette->colors[i].b;

	}

*/

	hasPalette = false;

	inZip = false;

}

DuneCutScene::~DuneCutScene()

{



}



/*

 * Return the pixel value at (x, y)

 * NOTE: The surface must be locked before calling this!

 */

Uint32 getpixel(SDL_Surface *surface, int x, int y)

{

    int bpp = surface->format->BytesPerPixel;

    /* Here p is the address to the pixel we want to retrieve */

    Uint8 *p = (Uint8 *)surface->pixels + (y * surface->pitch) + (x * bpp);



    switch(bpp) {

    case 1:

        return *p;



    case 2:

        return *(Uint16 *)p;



    case 3:

        if(SDL_BYTEORDER == SDL_BIG_ENDIAN)

            return p[0] << 16 | p[1] << 8 | p[2];

        else

            return p[0] | p[1] << 8 | p[2] << 16;



    case 4:

        return *(Uint32 *)p;



    default:

        return 0;       /* shouldn't happen, but avoids warnings */

    }

}


void DuneCutScene::playCutScene()

{

	int startTicks, curTicks;



//	int frameStart, frameEnd, frameTime;

	curScene = 0;

	

	screenRect.x = 0;

	screenRect.y = 0;

	screenRect.w = screen->w;

	screenRect.h = screen->h;









/*

	if(screen->w != cutScenePos.w)

		cutScenePos.x = (screen->w - cutScenePos.w)/2;

	else

		cutScenePos.x = 0;



	if(screen->h != cutScenePos.h)

		cutScenePos.y = (screen->h - cutScenePos.h)/2;

	else

		cutScenePos.y = 0;

*/

	soundPlayer->changeMusic(MUSIC_INTRO);



	



	for(int i=0;i<this->nScenes;i++)

	{

		if(!skipScene)

		{

			for(int x=0;x<scenes[i]->loops;x++)

			{

				cutScenePos.h = scenes[i]->frames[0]->h;

				cutScenePos.w = scenes[i]->frames[0]->w;



				switch(scenes[i]->pos)

				{

				case CUTSCENE_POS_TOP_LEFT:

					cutScenePos.x = 0;

					cutScenePos.y = 0;

					break;

				case CUTSCENE_POS_TOP_CENTRE:

					cutScenePos.y = 0;

					if(screen->w != cutScenePos.w)

						cutScenePos.x = (screen->w - cutScenePos.w)/2;

					else

						cutScenePos.x = 0;

					break;

				case CUTSCENE_POS_TOP_RIGHT:

					cutScenePos.y = 0;

					if(screen->w > cutScenePos.w)

						cutScenePos.x = (screen->w - cutScenePos.w);

					else

						cutScenePos.x = 0;

					break;

				case CUTSCENE_POS_CENTRE:

					if(screen->w > cutScenePos.w)

						cutScenePos.x = (screen->w - cutScenePos.w)/2;

					else

						cutScenePos.x = 0;



					if(screen->h > cutScenePos.h)

						cutScenePos.y = (screen->h - cutScenePos.h)/2;

					else

						cutScenePos.y = 0;

					break;

				case CUTSCENE_POS_BOTTOM_LEFT:

					cutScenePos.x = 0;

					if(screen->h > cutScenePos.h)

						cutScenePos.y = screen->h - cutScenePos.h;

					else

						cutScenePos.y = 0;

					break;

				case CUTSCENE_POS_BOTTOM_CENTRE:

					if(screen->w != cutScenePos.w)

						cutScenePos.x = (screen->w - cutScenePos.w)/2;

					else

						cutScenePos.x = 0;



					if(screen->h > cutScenePos.h)

						cutScenePos.y = screen->h - cutScenePos.h;

					else

						cutScenePos.y = 0;

					break;

				case CUTSCENE_POS_BOTTOM_RIGHT:

					if(screen->h > cutScenePos.h)

						cutScenePos.y = screen->h - cutScenePos.h;

					else

						cutScenePos.y = 0;

					break;



					if(screen->w > cutScenePos.w)

						cutScenePos.x = (screen->w - cutScenePos.w);

					else

						cutScenePos.x = 0;

					break;



				}



				for(int j=0;j<scenes[i]->nFrames;j++)

				{

					if(!skipScene && !skipIndividScene)

					{

									

						scenes[i]->curFrame = j + (x * scenes[i]->nFrames);



						playVOSounds(scenes[i]);



						startTicks = SDL_GetTicks();



						SDL_FillRect(screen, &screenRect, COLOUR_BLACK);



						if(j > scenes[i]->underLayFrame && scenes[i]->hasUnderLay)

						{

							SDL_BlitSurface(scenes[i]->frames[scenes[i]->underLayFrame], NULL, screen, &cutScenePos);

						}



						SDL_BlitSurface(scenes[i]->frames[j], NULL, screen, &cutScenePos);



						if(textSurface)

							SDL_BlitSurface(textSurface, NULL, screen, &textPos);



						SDL_Flip(screen);



						



						while((curTicks = SDL_GetTicks()) - startTicks < scenes[i]->frameTime)

						{

							handleInput();

						}





					}

				}

			}

		}



		skipIndividScene = false;



		if(!skipScene)//do it like this so instantly get out, dont have to wait for the wait time if user presses escape

		{

			startTicks = SDL_GetTicks();



			while((Mix_Playing(voChan) || !allSceneSoundsPlayed(scenes[i])) && !skipScene)

			{

				playVOSounds(scenes[i]);

				handleInput();

			}

	

		}



		startTicks = SDL_GetTicks();

		while(!skipScene && (curTicks = SDL_GetTicks()) - startTicks < scenes[i]->endWaitTime)

		{

			handleInput();

		}

		

		//SDL_BlitSurface((SDL_Surface *)dataFile[UI_GameBar2].dat, NULL, screen, &gameBarPos);

	}



	//unload data

	this->unloadScenes();

}



bool DuneCutScene::allSceneSoundsPlayed(Dune_Scene *s)

{

	for(int k = 0;k < s->nVOSounds; k++)

	{

		if(s->voSounds[k]->played == false)

		{

			return false;

		}

	}



	showText(0,0,"",COLOUR_RED);

	return true;

}



void DuneCutScene::playVOSounds(Dune_Scene *s)

{

	for(int k = 0;k < s->nVOSounds; k++)

	{

		if(s->voSounds[k]->playAtFrame - 1 <= s->curFrame && !s->voSounds[k]->played)//if its supposed to play

		{

			if(s->voSounds[k]->effect)

			{

				if(!Mix_Playing(effectsChan))

				{

					s->voSounds[k]->played = true;

					//Mix_PlayChannel(effectsChan, s->voSounds[k]->sound, 0);

					Mix_PlayChannel(-1, s->voSounds[k]->sound, 0);

				}

			}

			else

			{

				if(!Mix_Playing(voChan))

				{

					s->voSounds[k]->played = true;

					Mix_PlayChannel(voChan, s->voSounds[k]->sound, 0);



					if(s->voSounds[k]->hasText)

						placeSubtitle(s->voSounds[k]->text, s->voSounds[k]->colour); //255-52 for new palette

				}

			}

		}

	}



	//show texts too

	for(int k=0;k<s->nSubtitles; k++)

	{

		if(s->subtitles[k]->playAtFrame - 1 <= s->curFrame && !s->subtitles[k]->shown)

		{

			placeSubtitle(s->subtitles[k]->text, s->subtitles[k]->colour);

			//s->subtitles[k]->shown = true;

		}

	}

}



void DuneCutScene::handleInput()

{

	SDL_Event event;

	while(SDL_PollEvent(&event))	//check for a key press
	{
		/* Look for a keypress */
		switch (event.type)
		{
			case (SDL_KEYDOWN):

				switch( event.key.keysym.sym )
				{

					case SDLK_ESCAPE:

						//showMessage("pressed escape");

						skipScene = true;

					break;



					case SDLK_SPACE:

						//skipIndividScene = true;

						break;

				}

			break;

		}

	}

}



void DuneCutScene::loadPalette(char *filename)

{

	SDL_Color tempColours[256];

	FILE *f = fopen(filename, "r");

	char *line = (char *)malloc(1024);

	int r, g, b, counter;



	r = g = b = counter = 0;



	fgets(line, 1024, f);

	fgets(line, 1024, f);

	fgets(line, 1024, f);



//	while((line = fgets(line, 1024, f)) != NULL && counter < 255)

//	{

//		sscanf(line, "%d %d %d\n", &colours[counter].r, &colours[counter].g, &colours[counter].b);

//		counter++;

//	}



	for(int i=0;i<255;i++)

	{

		line = fgets(line, 1024, f);

		sscanf(line, "%d %d %d\n", &tempColours[i].r, &tempColours[i].g, &tempColours[i].b);

		colours[255-i] = tempColours[i];

		//colours[i] = tempColours[i];

	}



	fclose(f);

/*

	f = fopen("data/intro/westwood.pal", "r");

	fgets(line, 1024, f);

	fgets(line, 1024, f);

	fgets(line, 1024, f);



	for(int i=0;i<255;i++)

	{

		line = fgets(line, 1024, f);

		sscanf(line, "%d %d %d\n", &tempColours[i].r, &tempColours[i].g, &tempColours[i].b);

		wwcolours[i] = tempColours[i];

		//colours[i] = tempColours[i];

	}



	fclose(f);*/



	//f = fopen("data/intro/temp.pal", "w");



	for(int i=0;i<255;i++)

	{

		//fprintf(f, "%d %d %d\n", colours[i].r, colours[i].g, colours[i].b);

//		wwcolours[i].r = abs(colours[i].r + wwcolours[i].r);

//		wwcolours[i].g = abs(colours[i].g + wwcolours[i].g);

//		wwcolours[i].b = abs(colours[i].b + wwcolours[i].b);

		wwcolours[i] = colours[i];

		//colours[i] = tempColours[i];

	}

	//fclose(f);



	hasPalette = true;

}



void DuneCutScene::loadScenes()

{

	showMessage("loading scenes...");

	SDL_Flip(screen);

	int rChannels = Mix_ReserveChannels(2);



	if(rChannels >= 2)

	{

		voChan = rChannels - 1;

		effectsChan = rChannels;

	}

	else

	{

		voChan = effectsChan = -1;

	}



	//temp intro scene

	this->nScenes = 2;







	this->loadPalette("data/intro/intro.pal");



	scenes = (Dune_Scene **)malloc(sizeof(Dune_Scene *) * nScenes);



	scenes[0] = (Dune_Scene *)malloc(sizeof(Dune_Scene));

	scenes[0]->basePath = "data/intro/intro1";

	scenes[0]->nFrames = 50;

	scenes[0]->curFrame = 0;

	scenes[0]->endWaitTime = 2000;



	scenes[0]->nVOSounds = 3;

	scenes[0]->voSounds = (Dune_Scene_VO **)malloc(sizeof(Dune_Scene_VO *) * scenes[0]->nVOSounds);

	scenes[0]->voSounds[0] = (Dune_Scene_VO *)malloc(sizeof(Dune_Scene_VO));



	scenes[0]->voSounds[0]->sound = Mix_LoadWAV("data/intro/introvoc/resampled/dune.ogg");

	scenes[0]->voSounds[0]->played = false;

	scenes[0]->voSounds[0]->playAtFrame = 50;

	

	scenes[0]->voSounds[1] = (Dune_Scene_VO *)malloc(sizeof(Dune_Scene_VO));

	scenes[0]->voSounds[1]->sound = Mix_LoadWAV("data/intro/introvoc/resampled/blding.ogg");

	scenes[0]->voSounds[1]->played = false;

	scenes[0]->voSounds[1]->playAtFrame = 50;



	scenes[0]->voSounds[2] = (Dune_Scene_VO *)malloc(sizeof(Dune_Scene_VO));

	scenes[0]->voSounds[2]->sound = Mix_LoadWAV("data/intro/introvoc/resampled/dynasty.ogg");

	scenes[0]->voSounds[2]->played = false;

	scenes[0]->voSounds[2]->playAtFrame = 50;



	loadSceneImages(scenes[0]);



	scenes[1] = (Dune_Scene *)malloc(sizeof(Dune_Scene));

	scenes[1]->basePath = "data/intro/intro2";

	scenes[1]->nFrames = 50;

	scenes[1]->curFrame = 0;

	loadSceneImages(scenes[1]);

}



void DuneCutScene::parseScenesFile(char *filename)

{

	FILE *f = fopen(filename, "r");

	int rChannels = Mix_ReserveChannels(2);

	char *line = (char *)malloc(1024);



	sprintf(line, "loading scene %s...", filename);



	SDL_FillRect(screen, &screenRect, COLOUR_BLACK);

	showText( 0, 0, line, COLOUR_RED);

	if(textSurface)

		SDL_BlitSurface(textSurface, NULL, screen, &textPos);

	

	SDL_Flip(screen);



	if(rChannels >= 2)

	{

		voChan = rChannels - 1;

		effectsChan = rChannels;

	}

	else

	{

		voChan = effectsChan = -1;

	}



	int sNum;

	int arg1;

	int vosNum;

	char path[1024];



	if(f)

	{

		while((line = fgets(line, 1024, f)) != NULL)

		{

			memset(path, 0, 1024);//clear the path



			if(line[0] != '#')

			{

				if(strstr(line, "SCENES"))

				{

					sscanf(line, "SCENES %d\n", &arg1);

					nScenes = arg1;

										

					scenes = (Dune_Scene **)malloc(sizeof(Dune_Scene *) * nScenes);

					for(int i=0;i<nScenes;i++)

					{

						scenes[i] = (Dune_Scene *)malloc(sizeof(Dune_Scene));

						scenes[i]->hasUnderLay = false;

						scenes[i]->underLayFrame = 0;

						scenes[i]->basePath = 0;

						scenes[i]->frames = 0;

						scenes[i]->endWaitTime = 0;

						scenes[i]->nVOSounds = 0;



						scenes[i]->setMod = 0;

						scenes[i]->nSets = 0;

						scenes[i]->hasSets = false;

						scenes[i]->loops = 1;



						scenes[i]->pos = CUTSCENE_POS_CENTRE;

						scenes[i]->frameTime = CS_FRAME_TIME;



						

					}

				}

				else if(strstr(line, "PALETTE"))

				{

					sscanf(line, "PALETTE %s\n", path);

					this->loadPalette(path);

					

				}

				else if(strstr(line, "FRAMETIME"))

				{

					sscanf(line, "%d FRAMETIME %d\n", &sNum, &vosNum);

					scenes[sNum]->frameTime = vosNum;

					

				}

				else if(strstr(line, "POSITION") && !strstr(line, "TEXT"))

				{

					sscanf(line, "%d POSITION %d\n", &sNum, &arg1);

					scenes[sNum]->pos = arg1;

					

				}

				else if(strstr(line, "FRAMES") && strstr(line, "SETFRAMES") == NULL)

				{

					sscanf(line, "%d FRAMES %d\n", &sNum, &arg1);

					

					scenes[sNum]->nFrames = arg1;

				}

				else if(strstr(line, "LOOPS"))

				{

					sscanf(line, "%d LOOPS %d\n", &sNum, &arg1);

					scenes[sNum]->loops = arg1;

				}

				else if(strstr(line, "SETMODIFIER"))

				{

					sscanf(line, "%d SETMODIFIER %d\n", &sNum, &arg1);

					scenes[sNum]->setMod = arg1;

				}

				else if(strstr(line, "UNDERLAYFRAME"))

				{

					sscanf(line, "%d UNDERLAYFRAME %d\n", &sNum, &arg1);

					

					scenes[sNum]->underLayFrame = arg1;

					scenes[sNum]->hasUnderLay = true;

				}

				else if(strstr(line, "ENDWAIT"))

				{

					sscanf(line, "%d ENDWAIT %d\n", &sNum, &arg1);

					

					scenes[sNum]->endWaitTime = arg1;

				}

				else if(strstr(line, "DEFCOL"))

				{

					sscanf(line, "DEFCOL %d\n", &arg1);

					

					colour_default = arg1;

				}

				else if(strstr(line, "BASEPATH"))

				{

					sscanf(line, "%d BASEPATH %s\n", &sNum, path);

					scenes[sNum]->basePath = (char *)malloc(strlen(path));

					scenes[sNum]->basePath = strcpy(scenes[sNum]->basePath, path);

				}

				else if(strstr(line, "ZIPFILE"))

				{

					sscanf(line, "ZIPFILE %s\n", path);

					zipPath = (char *)malloc(strlen(path));

					zipPath = strcpy(zipPath, path);

					inZip = true;

				}

				else if(strstr(line, "VOSOUNDS"))

				{

					sscanf(line, "%d VOSOUNDS %d\n", &sNum, &arg1);



					scenes[sNum]->nVOSounds = arg1;

					scenes[sNum]->voSounds = (Dune_Scene_VO **)malloc(sizeof(Dune_Scene_VO *) * scenes[sNum]->nVOSounds);

					

					for(int i=0;i<scenes[sNum]->nVOSounds;i++)

					{

						scenes[sNum]->voSounds[i] = (Dune_Scene_VO *)malloc(sizeof(Dune_Scene_VO));

						scenes[sNum]->voSounds[i]->played = false;

						scenes[sNum]->voSounds[i]->effect = false;

						scenes[sNum]->voSounds[i]->hasText = false;

						scenes[sNum]->voSounds[i]->colour = colour_default;

					}

				}

				else if(strstr(line, "TEXTS"))

				{

					sscanf(line, "%d TEXTS %d\n", &sNum, &arg1);

					scenes[sNum]->nSubtitles = arg1;



					scenes[sNum]->subtitles = (Dune_Scene_Text **)malloc(sizeof(Dune_Scene_Text *) * scenes[sNum]->nSubtitles);



					for(int i=0;i<scenes[sNum]->nSubtitles;i++)

					{

						scenes[sNum]->subtitles[i] = (Dune_Scene_Text *)malloc(sizeof(Dune_Scene_Text));

						scenes[sNum]->subtitles[i]->playAtFrame = 0;

						scenes[sNum]->subtitles[i]->shown = false;

						scenes[sNum]->subtitles[i]->colour = colour_red;

					}

				}

				else if(strstr(line, "TEXT") && strstr(line, "TEXTS") == NULL && strstr(line, "VOSOUND") == NULL)

				{

					if(strstr(line, "PLAYAT"))

					{

						sscanf(line, "%d TEXT %d PLAYAT %d\n", &sNum, &vosNum, &arg1);

						scenes[sNum]->subtitles[vosNum]->playAtFrame = arg1;

					}

					else if(strstr(line, "COLOUR"))

					{

						sscanf(line, "%d TEXT %d COLOUR %d\n", &sNum, &vosNum, &arg1);

						scenes[sNum]->subtitles[vosNum]->colour = arg1;

					}

					else if(strstr(line, "POSITION"))

					{

						sscanf(line, "%d TEXT %d POSITION %d\n", &sNum, &vosNum, &arg1);

						scenes[sNum]->subtitles[vosNum]->pos = arg1;

					}

					else

					{

						sscanf(line, "%d TEXT %d %[^\n]\n", &sNum, &vosNum, path);

						scenes[sNum]->subtitles[vosNum]->text = (char *)malloc(strlen(path));

						scenes[sNum]->subtitles[vosNum]->text  = strcpy(scenes[sNum]->subtitles[vosNum]->text, path);

					}

				}

				else if(strstr(line, "VOSOUND") && strstr(line, "VOSOUNDS") == NULL)

				{

					if(strstr(line, "PLAYAT"))

					{

						sscanf(line, "%d VOSOUND %d PLAYAT %d\n", &sNum, &vosNum, &arg1);

						scenes[sNum]->voSounds[vosNum]->playAtFrame = arg1;



					}

					else if(strstr(line, "PATH"))

					{

						sscanf(line, "%d VOSOUND %d PATH %s\n", &sNum, &vosNum, path);

						scenes[sNum]->voSounds[vosNum]->sound = Mix_LoadWAV(path);

		

					}

					else if(strstr(line, "TEXT"))

					{

						sscanf(line, "%d VOSOUND %d TEXT %[^\n]\n", &sNum, &vosNum, path);

					

						scenes[sNum]->voSounds[vosNum]->hasText = true;

						scenes[sNum]->voSounds[vosNum]->text = (char *)malloc(strlen(path));

						scenes[sNum]->voSounds[vosNum]->text = strcpy(scenes[sNum]->voSounds[vosNum]->text, path);

					}

					if(strstr(line, "EFFECT"))

					{

						sscanf(line, "%d VOSOUND %d EFFECT\n", &sNum, &vosNum);

						scenes[sNum]->voSounds[vosNum]->effect = true;

					}

					if(strstr(line, "COLOUR"))

					{

						sscanf(line, "%d VOSOUND %d COLOUR %d\n", &sNum, &vosNum, &arg1);

						scenes[sNum]->voSounds[vosNum]->colour = arg1;

					}

				}

				else if(strstr(line, "SETS"))

				{

					sscanf(line, "%d SETS %d\n", &sNum, &arg1);

					scenes[sNum]->hasSets = true;

					scenes[sNum]->nSets = arg1;



					scenes[sNum]->setFrames = (int *)malloc(sizeof(int) * arg1);

					scenes[sNum]->setMod = scenes[sNum]->setMod;



				}

				else if(strstr(line, "SETFRAMES"))

				{

					sscanf(line, "%d SETFRAMES %d %d\n", &sNum, &arg1, &vosNum);

					scenes[sNum]->setFrames[arg1] = vosNum;

				}

			}

		}



		for(int i=0;i<nScenes;i++)

		{

			SDL_Rect progress;

			loadSceneImages(scenes[i]);

			

			progress.x = textPos.x;

			progress.y = textPos.y + textPos.h + 20;

			progress.w = (int)((screen->w / nScenes) * i);

			progress.h = 10;



			SDL_FillRect(screen, &screenRect, COLOUR_BLACK);

			SDL_FillRect(screen, &progress, COLOUR_RED);

			if(textSurface)

				SDL_BlitSurface(textSurface, NULL, screen, &textPos);

			

			SDL_Flip(screen);

		}

	}

	else

	{

		sprintf(line, "loading scene %s failed", filename);

		showText( 0, 0, line, 52);

		if(textSurface)

			SDL_BlitSurface(textSurface, NULL, screen, &textPos);

		SDL_Flip(screen);

	}



	if(f)

		fclose(f);



	showText( 0, 0, "", 52);

	free(line);



}



void DuneCutScene::unloadScenes()

{

	for(int i=0;i<nScenes;i++)

	{

		for(int j=0;j<scenes[i]->nFrames;j++)

		{

			SDL_FreeSurface(scenes[i]->frames[j]);

		}



		free(scenes[i]);

	}



	if(textSurface)

	{

		SDL_FreeSurface(textSurface);

		textSurface = NULL;

	}



	int rChannels = Mix_ReserveChannels(0);



	free(scenes);

}



void DuneCutScene::loadSceneImages(Dune_Scene* scene)

{

	char *path = (char *)malloc(1024);

	SDL_RWops *rw;

	ZZIP_DIR* zzipdir;



	if(inZip)

		zzipdir = zzip_dir_open(zipPath, NULL );



	if(!zzipdir)

		return;



	if(!scene->hasSets)

	{

		scene->frames = (SDL_Surface **)malloc(sizeof(SDL_Surface *) * scene->nFrames);



		for(int i=1;i<=scene->nFrames;i++)

		{

			//fprintf(stdout, "loading %s\n", getFramePath(scene->basePath, i));

			sprintf(path, "%s", getFramePath(scene->basePath, i));



			if(inZip)

			{

				ZZIP_FILE* zfile = zzip_file_open(zzipdir, path, ZZIP_CASELESS);

				rw = SDL_RWFromZZIP_FILE(zfile);

				//rw = SDL_RWFromZZIP(path, "rb");

				scene->frames[i -1] = SDL_LoadBMP_RW(rw, 0);

				SDL_FreeRW(rw);

				zzip_file_close(zfile);

			}

			else

			{

				//rw = SDL_RWFromFile(path, "r");

				scene->frames[i -1] = SDL_LoadBMP(path);

			}





			if(hasPalette)

				SDL_SetPalette(scene->frames[i -1], SDL_LOGPAL|SDL_PHYSPAL, colours, 0, 256);//set the palette

//			else

//				SDL_SetPalette(scene->frames[i -1], SDL_LOGPAL|SDL_PHYSPAL, palette->colors, 0, 256);//set the palette

			

			//SDL_SetPalette(scene->frames[i -1], SDL_LOGPAL|SDL_PHYSPAL, revColours, 0, 256);//set the palette

			

			SDL_SetColorKey(scene->frames[i -1], SDL_SRCCOLORKEY | SDL_RLEACCEL, 255);

		}

	}

	else

	{

		int frameCounter = 0;

		scene->nFrames = 0;

		for(int i =0;i<scene->nSets;i++)

		{

			scene->nFrames += scene->setFrames[i];

		}



		scene->frames = (SDL_Surface **)malloc(sizeof(SDL_Surface *) * scene->nFrames);



		for(int i=0;i<scene->nSets;i++)

		{

			for(int j=0;j<scene->setFrames[i];j++)

			{

				sprintf(path, "%s", getFramePathAlpha(scene->basePath, j, i + scene->setMod));

				

				if(inZip)

				{

					ZZIP_FILE* zfile = zzip_file_open(zzipdir, path, ZZIP_CASELESS);

					rw = SDL_RWFromZZIP_FILE(zfile);

					//rw = SDL_RWFromZZIP(path, "rb");

					scene->frames[j + frameCounter] = SDL_LoadBMP_RW(rw, 0);

					SDL_FreeRW(rw);

				}

				else

				{

					//rw = SDL_RWFromFile(path, "r");

					scene->frames[j + frameCounter] = SDL_LoadBMP(path);

				}

				

				

				//scene->frames[j + frameCounter] = SDL_LoadBMP(path);

				

				if(hasPalette)

					SDL_SetPalette(scene->frames[j + frameCounter], SDL_LOGPAL|SDL_PHYSPAL, colours, 0, 256);//set the palette

				SDL_SetColorKey(scene->frames[j + frameCounter], SDL_SRCCOLORKEY | SDL_RLEACCEL, 255);

				

				if(i != 0)

				{

					//SDL_SetColorKey(scene->frames[j + frameCounter], SDL_SRCCOLORKEY | SDL_RLEACCEL, 255);

					//SDL_SetPalette(scene->frames[j + frameCounter], SDL_LOGPAL|SDL_PHYSPAL, wwcolours, 0, 256);//set the palette

				

					if ( !SDL_MUSTLOCK(scene->frames[scene->underLayFrame]) &&

						 !SDL_MUSTLOCK(scene->frames[j + frameCounter]) )

					{

						if( SDL_LockSurface(scene->frames[scene->underLayFrame]) >= 0 &&

							SDL_LockSurface(scene->frames[j + frameCounter]) >= 0)

						{

							for(int x=0;x < scene->frames[j + frameCounter]->w; x++)

							{

								for(int y=0;y < scene->frames[j + frameCounter]->h; y++)

								{

									int overPixel = getpixel(scene->frames[j + frameCounter], x, y);

									int underPixel = getpixel(scene->frames[scene->underLayFrame], x, y);

									//int newPixel = (overPixel + underPixel) % 255;



									int newPixel = 255 - (underPixel ^ overPixel);

									

									putpixel(scene->frames[j + frameCounter], x, y, newPixel);



									//fprintf(stdout, "old %d new %d\n", overPixel, newPixel);

								}

							}

						}

						else

						{

							fprintf(stdout, "couldnt lock the surfaces\n");

						}

					}

					else

					{

						

					}

					

						/* Unlock the surface */

					if ( !SDL_MUSTLOCK(scene->frames[scene->underLayFrame]) &&

						 !SDL_MUSTLOCK(scene->frames[j + frameCounter]) )

					{

						SDL_UnlockSurface(scene->frames[scene->underLayFrame]);

						SDL_UnlockSurface(scene->frames[j + frameCounter]);

					}

			

				}

				

				

				

			}



			frameCounter += scene->setFrames[i];

		}

	}



	if(zzipdir)

		zzip_dir_close(zzipdir);

}



char *DuneCutScene::getFramePath(char *basePath, int frame)

{



	char *framePath = (char *)malloc(strlen(basePath) + 9);

	char *end = (char *)malloc(10);



	if(frame-1 >= 10)

	{

		sprintf(end, " 00%d.bmp", frame -1);

	}

	else

	{

		sprintf(end, " 000%d.bmp", frame -1);

	}



	framePath = strcpy(framePath, basePath);

	framePath = strcat(framePath, end);



	return framePath;



}



char *DuneCutScene::getFramePathAlpha(char *basePath, int frame, int set)

{



	char *framePath = (char *)malloc(strlen(basePath) + 9);

	char *end = (char *)malloc(10);

	char c;



	switch(set)

	{

		case 0:

			c = 'a';

		break;



		case 1:

			c = 'b';

		break;



		case 2:

			c = 'c';

		break;

	}



	if(frame >= 10)

	{

		sprintf(end, "%c 00%d.bmp", c, frame);

	}

	else

	{

		sprintf(end, "%c 000%d.bmp", c, frame);

	}



	framePath = strcpy(framePath, basePath);

	framePath = strcat(framePath, end);



	return framePath;



}



void DuneCutScene::showText(int x, int y, char *text, int colour)

{

	//red = 52

	if(textSurface)

	{

		SDL_FreeSurface(textSurface);

		textSurface = NULL;

	}



	if(strcmp(text, "") != 0)

	{

		if(hasPalette)

			textSurface = TTF_RenderText_Solid(font[25], text, colours[colour]);

		else

			textSurface = TTF_RenderText_Solid(font[25], text, palette->colors[colour]);

		textPos.x = x;

		textPos.y = y;

		textPos.w = textSurface->w;

		textPos.h = textSurface->h;

	}

}



void DuneCutScene::placeSubtitle(char *text, int colour)

{

	showText(0,0, text, colour);



	if(screen->w != textPos.w)

		textPos.x = (screen->w - textPos.w)/2;

	else

		textPos.x = 0;



	if(screen->h != textPos.h)

		textPos.y = cutScenePos.y + cutScenePos.h + 16;

	else

		textPos.x = 0;

}

