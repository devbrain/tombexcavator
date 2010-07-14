 #include "menu.h"

#include "sand.h"

//#include "gui/gui.h"

//#include "zlib/zzip.h"



#define SCREEN_BPP 8

#define VERSION "0.94"



SDL_Surface	*screen;



Window	*singleWindow;

Window	*skirmishWindow;

Window	*customWindow;

Window	*mainWindow, *multiWindow;

Window loadGameWindow;



Button loadCancelButton, gameLoadSpot[NUM_SAVEGAMES];



Button	single_buttonSkirmish;

Button	single_buttonCampaign;

Button	single_buttonCustom;

Button	single_buttonLoad;

Button	single_buttonCancel;

Button	single_buttonSkirmishStart;

Button	single_buttonSkirmishCancel;

Button	single_buttonSkirmishAtre;

Button	single_buttonSkirmishHark;

Button	single_buttonSkirmishOrdos;

Button	single_buttonSkirmishHerald;

Button	single_buttonMissionPlus;

Button	single_buttonMissionMinus;

Button 	menu_mission;

Button	menu_difficulty;



int	selectedHouse;

int selectedMission = 1;



Label	skirmishLabel;

Label	customLabel;

Label	multiLabel;

Label	*headingLabel;



//int settings.campaignLevel;



Button	main_buttonSinglePlayer,

		main_buttonMultiPlayer,

		main_buttonMapEditor,

		main_buttonOptions,

		main_buttonAbout,

		main_buttonQuit,

		

		single_buttonDifficulty,

		single_buttonHouse,



//		single_buttonCampaign,

		single_buttonRandomMap,

		single_buttonSingleMap,

//		single_buttonCancel,



		custom_buttonName[MAX_PLAYERS],

		custom_buttonColour[MAX_PLAYERS],

		custom_buttonHouse[MAX_PLAYERS],

		custom_buttonTeam[MAX_PLAYERS],

		custom_buttonStart,

		custom_buttonCancel,

		

		multi_buttonCreate,

		multi_buttonJoin,

		multi_buttonCancel,

		

		multiServer_buttonName[MAX_PLAYERS],

		multiServer_buttonColour[MAX_PLAYERS],

		multiServer_buttonHouse[MAX_PLAYERS],

		multiServer_buttonTeam[MAX_PLAYERS],

		multiServer_buttonStart,

		multiServer_buttonKick,

		multiServer_buttonCancel,

		

		multiClient_buttonName[MAX_PLAYERS],

		multiClient_buttonColour[MAX_PLAYERS],

		multiClient_buttonHouse[MAX_PLAYERS],

		multiClient_buttonTeam[MAX_PLAYERS],

		multiClient_buttonCancel,

		multiClient_buttonMessage,



		options_buttonConcrete,

		options_buttonRes,

		options_buttonFullScreen,

		options_buttonDoubleBuffered,

		options_buttonOk,

		options_buttonCancel;



Entry		multi_address,

			options_name;



HBox		multi_hbox,

			custom_hbox,

			custom_clientHbox[MAX_PLAYERS],

			multiClient_hbox,

			multiClient_clientHbox[MAX_PLAYERS],

			multiServer_hbox,

			multiServer_clientHbox[MAX_PLAYERS],

			single_hbox,

			options_hbox;

VBox		main_vbox,

			custom_vbox,

			custom_vbox2,

			custom_vbox3,

			single_vbox,

			single_vbox2,

			multi_vbox,

			multi_vbox2,

			multiClient_vbox,

			multiClient_vbox2,

			multiClient_vbox3,

			multiServer_vbox,

			multiServer_vbox2,

			multiServer_vbox3,

			options_vbox;

Widget		*currentWidget,

			*focusedWidget = NULL;

Window		about_window;





bool	gameCreated,

		createThreadValid,

		quiting,

		joinThreadValid,

		multi_buttonHit,

		multiGameStarting;

bool	inMainMenu;



int		backgroundOffsetX,

		backgroundOffsetY,



		multi_buttonRow,

		playersJoined;



TCPsocket sockets[MAX_PLAYERS];

TCPsocket server;

SDLNet_SocketSet socketSet;



SDL_Thread	*multiGame_thread = NULL;

SDL_mutex	*mutex_button = NULL,

			*mutex_currentWidget = NULL,

			*mutex_playersJoined = NULL;



SDL_Surface		*menu,

				*menuText;



void setVideoMode();

void realign_buttons();



char* houseName[9] =

{

	"Atreides",

	"Ordos",

	"Harkonnen",

	"Sardaukar",

	"Fremen",

	"Mercenary"

};



void setHeading(char *text)
{
	headingLabel->setText(text);
	headingLabel->setX((singleWindow->getWidth() - headingLabel->getWidth())/2);

	//headingLabel->setY((singleWindow->getHeight() - headingLabel->getHeight())/2);
	headingLabel->setY(((SDL_Surface *)dataFile[UI_Menu].dat)->h + 16);
	headingLabel->setVisible(true);
}



int houseNameToNum(char* house)

{

	int num = NONE;



	if (strcmp(house, "Atreides") == 0)

		num = HOUSE_ATREIDES;

	else if (strcmp(house, "Ordos") == 0)

		num = HOUSE_ORDOS;

	else if (strcmp(house, "Harkonnen") == 0)

		num = HOUSE_HARKONNEN;

	else if (strcmp(house, "Sardaukar") == 0)

		num = HOUSE_SARDAUKAR;

	else if (strcmp(house, "Fremen") == 0)

		num = HOUSE_FREMEN;

	else if (strcmp(house, "Mercenary") == 0)

		num = HOUSE_MERCENARY;



	return num;

}



char* colourName[7] =

{

	"Blue",

	"Green",

	"Red",

	"Purple",

	"Orange",

	"Grey"

};



int colourNameToNum(char* colour)

{

	int num = NONE;



	if (strcmp(colour, "Blue") == 0)

		num = 0;

	else if (strcmp(colour, "Green") == 0)

		num = 1;

	else if (strcmp(colour, "Red") == 0)

		num = 2;

	else if (strcmp(colour, "Purple") == 0)

		num = 3;

	else if (strcmp(colour, "Orange") == 0)

		num = 4;

	else if (strcmp(colour, "Grey") == 0)

		num = 5;



	return num;

}



void custom_colour(void *data);

void multiServer_colour(void *data);

void multiClient_start();



void cancel()

{

	if(currentWidget == skirmishWindow || currentWidget == &custom_vbox || currentWidget == &loadGameWindow)

	{

		currentWidget = singleWindow;

		setHeading("Single Player\n");

	}

	else

	{

		//currentWidget = &main_vbox;

		currentWidget = mainWindow;

	}

}



void start_game()

{

	multiGameStarting = false;

	inMainMenu = false;



	settings.maxPathSearch = 400;

	//check all info is valid



	do_game();

	game_loadMap();



	focusedWidget = NULL;

	inMainMenu = true;

}



void skirmish_select_atre()

{

	single_buttonSkirmishHark.setVisible(false);

	single_buttonSkirmishOrdos.setVisible(false);

	single_buttonSkirmishAtre.setVisible(true);

	selectedHouse = HOUSE_ATREIDES;

}



void skirmish_select_hark()

{

	single_buttonSkirmishHark.setVisible(true);

	single_buttonSkirmishOrdos.setVisible(false);

	single_buttonSkirmishAtre.setVisible(false);

	selectedHouse = HOUSE_HARKONNEN;

}



void skirmish_select_ordos()

{

	single_buttonSkirmishHark.setVisible(false);

	single_buttonSkirmishOrdos.setVisible(true);

	single_buttonSkirmishAtre.setVisible(false);

	selectedHouse = HOUSE_ORDOS;

}



void single_cancel()

{

	//currentWidget = &main_vbox;

	//currentWidget = mainWindow;

	cancel();

}







void single_loadGame()
{
	int		i, magicNum;
	char	charString[30],
			description[20];
	FILE	*fp;
	for(i = 0; i < NUM_SAVEGAMES; i++)

	{

		sprintf(charString, "savegame%d.dls", i);

		fp = fopen(charString, "rb");

		if (fp)

		{

			fread(&magicNum, sizeof(int), 1, fp);

			if (magicNum == SAVEMAGIC)

			{

				fread(description, sizeof(description), 1, fp);

				gameLoadSpot[i].setText(description);

				gameLoadSpot[i].setEnabled(true);

			}

			else

			{

				gameLoadSpot[i].setText("(Empty)");

				gameLoadSpot[i].setEnabled(false);

			}



			fclose(fp);

		}

		else

		{

			gameLoadSpot[i].setText("(Empty)");

			gameLoadSpot[i].setEnabled(false);

		}

	}

	currentWidget = &loadGameWindow;

}



void single_difficulty()

{

	if (single_buttonDifficulty.getProgress() == EASY)

	{

		single_buttonDifficulty.setSurface((SDL_Surface *)dataFile[UI_Dif_Medium].dat, NULL);

		single_buttonDifficulty.setProgress(MEDIUM);

	}

	else if (single_buttonDifficulty.getProgress() == MEDIUM)

	{

		single_buttonDifficulty.setSurface((SDL_Surface *)dataFile[UI_Dif_Hard].dat, NULL);

		single_buttonDifficulty.setProgress(HARD);

	}

	else if (single_buttonDifficulty.getProgress() == HARD)

	{

		single_buttonDifficulty.setSurface((SDL_Surface *)dataFile[UI_Dif_Easy].dat, NULL);

		single_buttonDifficulty.setProgress(EASY);

	}

}



void single_house()

{

	if (single_buttonHouse.textEquals("Atreides"))

		single_buttonHouse.setText("Ordos");

	else if (single_buttonHouse.textEquals("Ordos"))

		single_buttonHouse.setText("Harkonnen");

	else if (single_buttonHouse.textEquals("Harkonnen"))

		single_buttonHouse.setText("Atreides");

}



void single_mapDecrement()

{

	selectedMission--;

	if(selectedMission < 1)

		selectedMission = 22;



	/*

						//draw credits

	SDL_Rect drawLocation, source;

	char creditsBuffer[2];

	sprintf(creditsBuffer, "%d", selectedMission);
	int digits = strlen(creditsBuffer);
	SDL_Surface* surface = (SDL_Surface*)dataFile[UI_CreditsDigits].dat;
	source.w = drawLocation.w = surface->w/10;
	source.h = drawLocation.h = surface->h;
	source.y = 0;
	drawLocation.y = menu_mission.getY() + 14;
	for (int i=digits-1; i>=0; i--)
	{
		source.x = atoi(&creditsBuffer[i])*source.w;
		creditsBuffer[i] = '\0';	//so next round atoi only does one character
		drawLocation.x = menu_mission.getX() + 78 + (6 - digits + i)*2;
		//SDL_BlitSurface(surface, &source, screen, &drawLocation);



	}*/

	/*

	int mapNum = atoi(single_buttonMap.getText());

	char temp[3];



	mapNum--;

	if (mapNum < 1)

		mapNum = 22;



	sprintf(temp, "%d", mapNum);

	single_buttonMap.setText(temp);*/

}



void single_mapIncrement()

{

	selectedMission++;

	if(selectedMission > 22)

		selectedMission = 1;

	/*

	int mapNum = atoi(single_buttonMap.getText());

	char temp[3];



	mapNum++;

	if (mapNum > 22)

		mapNum = 1;



	sprintf(temp, "%d", mapNum);

	single_buttonMap.setText(temp);*/

}



void single_randomMap()

{

	settings.gameType = RANDOMMAP;

	settings.playerType = SINGLE;



	int i;

	for (i = 0; i < MAX_PLAYERS; i++)

	{

		if (single_buttonDifficulty.textEquals("Easy"))

			settings.playerDifficulty[i] = EASY;

		else if (single_buttonDifficulty.textEquals("Medium"))

			settings.playerDifficulty[i] = MEDIUM;

		else if (single_buttonDifficulty.textEquals("Hard"))

			settings.playerDifficulty[i] = HARD;

	}



	if (single_buttonHouse.textEquals("Ordos"))

		settings.playerNum = HOUSE_ORDOS;

	else if (single_buttonHouse.textEquals("Harkonnen"))

		settings.playerNum = HOUSE_HARKONNEN;

	else 

		settings.playerNum = HOUSE_ATREIDES;

	settings.playerColour[settings.playerNum] = settings.playerHouse[settings.playerNum] = settings.playerNum;



	for(i = 0; i < MAX_PLAYERS; i++)

	{

		sprintf(settings.playerName[i], "Computer %d", i);

		settings.playerHouse[i] = settings.playerColour[i] = i;

		settings.playerTeam[i] = i + 1;

		settings.isHuman[i] = false;

	}

	strncpy(settings.playerName[settings.playerNum], settings.localPlayerName, MAX_NAMELENGTH);

	settings.isHuman[settings.playerNum] = true;



	settings.techLevel = 9;



	start_game();

}







void single_campaign_start()

{

	int i;



	if(settings.playerNum != -2)

	{

		settings.playerColour[settings.playerNum] = settings.playerHouse[settings.playerNum] = settings.playerNum;



		for(i = 0; i < MAX_PLAYERS; i++)

		{

			sprintf(settings.playerName[i], "Computer %d", i);

			settings.playerHouse[i] = settings.playerColour[i] = i;

			settings.playerTeam[i] = settings.playerNum + 1;

			settings.isHuman[i] = false;

		}

		strncpy(settings.playerName[settings.playerNum], settings.localPlayerName, MAX_NAMELENGTH);

		//set player team to be different from computers

		if (settings.playerNum == 0)

			settings.playerTeam[settings.playerNum] = settings.playerNum + 2;

		else

			settings.playerTeam[settings.playerNum] = settings.playerNum;

		settings.isHuman[settings.playerNum] = true;



		if(!settings.campaignLevel)

			settings.campaignLevel = 1;

		//int mapNum = 1;

		//mapNum = atoi(single_buttonMap.getText());



		//if (settings.gameType == ORIGINAL)

		settings.playerHouse[settings.playerNum] = settings.playerNum;

		bool levelSuccessful = true;

		while(levelSuccessful && settings.campaignLevel <= 22 && settings.gameType == ORIGINAL)//22
		{

			char temp[3];

			
			settings.playerHouse[settings.playerNum] = settings.playerNum;
			settings.mapFilename[0] = '\0';
			strncpy(settings.mapFilename, "data/scenario/scen", MAX_LINE);
			switch (settings.playerHouse[settings.playerNum])
			{
				case HOUSE_ATREIDES:
					strncat(settings.mapFilename, "a0", MAX_LINE);
					break;
				case HOUSE_ORDOS:
					strncat(settings.mapFilename, "o0", MAX_LINE);
					break;
				case HOUSE_HARKONNEN:
					strncat(settings.mapFilename, "h0", MAX_LINE);
					break;
				default:
					//make_random_map(64, 64);
					break;
			}



			

			if ((settings.campaignLevel < 1) || (settings.campaignLevel > 22))

				settings.campaignLevel = 1;

			if (settings.campaignLevel < 10)

				strncat(settings.mapFilename, "0", MAX_LINE);

			sprintf(temp, "%d", settings.campaignLevel);



			strncat(settings.mapFilename, temp, MAX_LINE);

			strncat(settings.mapFilename, ".ini", MAX_LINE);

			settings.techLevel = (settings.campaignLevel + 1)/3 + 1;

			mentat->setHouse(settings.playerHouse[settings.playerNum]);
			mentat->doBriefing(settings.campaignLevel - 1);
			start_game();

			levelSuccessful = settings.won;
			settings.campaignLevel++;
		}



		if(levelSuccessful && settings.campaignLevel == 23)//23

		{

			DuneCutScene *finaleScene = new DuneCutScene();

	

			switch (settings.playerHouse[settings.playerNum])
			{
				case HOUSE_ATREIDES:
					finaleScene->parseScenesFile("data/atrefinale.scene");
					break;
				case HOUSE_ORDOS:
					finaleScene->parseScenesFile("data/ordosfinale.scene");
					break;
				case HOUSE_HARKONNEN:
					finaleScene->parseScenesFile("data/harkfinale.scene");
					break;
				default:
					//make_random_map(64, 64);
					break;
			}



			finaleScene->playCutScene();

			//play credits

		}

	}



}







void single_campaign()

{

	/*need to keep going through levels until finished*/

	/*perhaps showing mentat in between*/



	settings.gameType = ORIGINAL;

	settings.playerType = SINGLE;



	int i;

	for (i = 0; i < MAX_PLAYERS; i++)

	{

		if (single_buttonDifficulty.textEquals("Easy"))

			settings.playerDifficulty[i] = EASY;

		else if (single_buttonDifficulty.textEquals("Medium"))

			settings.playerDifficulty[i] = MEDIUM;

		else if (single_buttonDifficulty.textEquals("Hard"))

			settings.playerDifficulty[i] = HARD;

		//else 

		//	settings.playerDifficulty[i] = IMPOSSIBLE;

	}



	/*

	if (single_buttonHouse.textEquals("Ordos"))

		settings.playerNum = HOUSE_ORDOS;

	else if (single_buttonHouse.textEquals("Harkonnen"))

		settings.playerNum = HOUSE_HARKONNEN;

	else 

		settings.playerNum = HOUSE_ATREIDES;

		*/

	settings.playerNum = mentat->doHouseChoice();

	settings.campaignLevel = 0;

	single_campaign_start();

}



void single_loadGameEntry(void *val)

{

	currentWidget = mainWindow;

	settings.gameType = SKIRMISH;

	settings.playerType = SINGLE;





	for (int i = 0; i < MAX_PLAYERS; i++)

	{

		settings.playerDifficulty[i] = (DIFFICULTYTYPE)single_buttonDifficulty.getProgress();

	}

	settings.playerNum = selectedHouse;

		

	settings.playerColour[settings.playerNum] = settings.playerHouse[settings.playerNum] = settings.playerNum;



	for(int i = 0; i < MAX_PLAYERS; i++)

	{

		sprintf(settings.playerName[i], "Computer %d", i);

		settings.playerHouse[i] = settings.playerColour[i] = i;

		settings.playerTeam[i] = settings.playerNum + 1;

		settings.isHuman[i] = false;

	}

	

	strncpy(settings.playerName[settings.playerNum], settings.localPlayerName, MAX_NAMELENGTH);

	//set player team to be different from computers

	if (settings.playerNum == 0)

		settings.playerTeam[settings.playerNum] = settings.playerNum + 2;

	else

		settings.playerTeam[settings.playerNum] = settings.playerNum;

	settings.isHuman[settings.playerNum] = true;



	settings.playerHouse[settings.playerNum] = settings.playerNum;

	char temp[3];

			
	settings.mapFilename[0] = '\0';
	strncpy(settings.mapFilename, "data/scenario/scen", MAX_LINE);
	switch (settings.playerHouse[settings.playerNum])
	{
	case HOUSE_ATREIDES:
		strncat(settings.mapFilename, "a0", MAX_LINE);
		break;
	case HOUSE_ORDOS:
		strncat(settings.mapFilename, "o0", MAX_LINE);
		break;
	case HOUSE_HARKONNEN:
		strncat(settings.mapFilename, "h0", MAX_LINE);
		break;
	default:
		//make_random_map(64, 64);
		break;
	}



			

	if ((selectedMission < 1) || (selectedMission > 22))

		selectedMission = 1;

	if (selectedMission < 10)

		strncat(settings.mapFilename, "0", MAX_LINE);



	sprintf(temp, "%d", selectedMission);

	strncat(settings.mapFilename, temp, MAX_LINE);

	strncat(settings.mapFilename, ".ini", MAX_LINE);

	settings.techLevel = (selectedMission + 1)/3 + 1;

	do_game();
	loadingGame = true;
	game_loadEntryCallback((int)val);
	do_game();
	loadingGame = false;
	game_loop();


	if(settings.gameType == ORIGINAL)//Someone has loaded a campaign map after starting a skirmish
	{
		if(settings.won)
		{
			settings.campaignLevel++;
			single_campaign_start();
		}
	}

}



void custom_name(void *data)

{

	char temp[30];

	int i = (int)data;

	if ((i >= 0) && (i < MAX_PLAYERS))

	{

		if (custom_buttonName[i].textEquals(""))

		{

			custom_clientHbox[i].setEnabled(true);

			custom_clientHbox[i].setVisible(true);

			sprintf(temp, "Computer %d (Easy)", i);

			custom_buttonName[i].setText(temp);

			custom_buttonHouse[i].setText(houseName[getRandomInt(0, NUM_HOUSES-1)]);

			custom_colour((void*)i);

			settings.isHuman[i] = false;

			//custom_buttonHouse[i].setText("Atreides");

			//custom_buttonHouse[i].setEnabled(true);

			//custom_buttonTeam[i].setText("1");

			//custom_buttonTeam[i].setEnabled(true);

			//custom_buttonColour[i].setText("Blue");

			//custom_buttonColour[i].setEnabled(true);



			playersJoined++;

		}

		else if (strstr(custom_buttonName[i].getText(), "Easy") > 0)

		{

			sprintf(temp, "Computer %d (Medium)", i);

			custom_buttonName[i].setText(temp);

		}

		else if (strstr(custom_buttonName[i].getText(), "Medium") > 0)

		{

			sprintf(temp, "Computer %d (Hard)", i);

			custom_buttonName[i].setText(temp);

		}

		else if (strstr(custom_buttonName[i].getText(), "Hard") > 0)

		/*{

			sprintf(temp, "Computer %d (Impossible)", i);

			custom_buttonName[i].setText(temp);

		}

		else if (strstr(custom_buttonName[i].getText(), "Impossible") > 0)*/

		{

			custom_clientHbox[i].setEnabled(false);

			custom_clientHbox[i].setVisible(false);

			custom_buttonName[i].setText("");

			//custom_buttonHouse[i].setText("");

			//custom_buttonHouse[i].setEnabled(false);

			//custom_buttonTeam[i].setText("");

			//custom_buttonTeam[i].setEnabled(false);

			//custom_buttonColour[i].setText("");

			//custom_buttonColour[i].setEnabled(false);



			playersJoined--;

		}

	}

}



void custom_house(void *data)

{

	int i = (int)data;

	if ((i >= 0) && (i < MAX_PLAYERS))

	{

		int j = houseNameToNum(custom_buttonHouse[i].getText()) + 1;

		if (j >= NUM_HOUSES)

			j = 0;

		custom_buttonHouse[i].setText(houseName[j]);

	}

}



void custom_team(void *data)

{

	int i = (int)data;

	if ((i >= 0) && (i < settings.maxPlayers))

	{

		int team = atoi(custom_buttonTeam[i].getText());

		char temp[10];



		if (++team > settings.maxPlayers)

			team = 1;

		sprintf(temp, "%d", team);

		custom_buttonTeam[i].setText(temp);

	}

}



void custom_colour(void *data)

{

	int i = (int)data;

	if ((i >= 0) && (i < MAX_PLAYERS))

	{

		bool alreadyUsed;

		int j;

		do

		{

			j = colourNameToNum(custom_buttonColour[i].getText()) + 1;

			if (j >= NUM_HOUSES)

				j = 0;

			custom_buttonColour[i].setText(colourName[j]);



			alreadyUsed = false;

			for(j = 0; j < settings.maxPlayers; j++)

			if ((j != i) && custom_clientHbox[j].isEnabled() && (strcmp(custom_buttonColour[j].getText(), custom_buttonColour[i].getText()) == 0))

				alreadyUsed = true;

		} while (alreadyUsed);

	}

}





void custom_start()

{

	int	i, j,

		count, numbers[MAX_PLAYERS];



	for(i = 0; i < settings.maxPlayers; i++)

		settings.playerMapNumber[i] = NONE;



	for(i = 0; i < settings.maxPlayers; i++)

	{

		strcpy(settings.playerName[i], custom_buttonName[i].getText());

		settings.playerHouse[i] = houseNameToNum(custom_buttonHouse[i].getText());		

		settings.playerTeam[i] = atoi(custom_buttonTeam[i].getText());

		settings.playerColour[i] = colourNameToNum(custom_buttonColour[i].getText());		

		

		if (strlen(settings.playerName[i]))

		{
			readMapPlayerNumbers(settings.mapFilename, numbers);
			do
			{
				count = 0;
				settings.playerMapNumber[i] = numbers[getRandomInt(0, settings.maxPlayers-1)];
				for (j = 0; j < settings.maxPlayers; j++)	//check no other players have same number
				if ((i != j) && (settings.playerMapNumber[i] == settings.playerMapNumber[j]))
					count++;
			} while (count > 0);



			if (strstr(custom_buttonName[i].getText(), "Easy"))

				settings.playerDifficulty[i] = EASY;

			else if (strstr(custom_buttonName[i].getText(), "Medium"))

				settings.playerDifficulty[i] = MEDIUM;

			else if (strstr(custom_buttonName[i].getText(), "Hard"))

				settings.playerDifficulty[i] = HARD;

			/*else if (strstr(custom_buttonName[i].getText(), "Impossible"))

				settings.playerDifficulty[i] = IMPOSSIBLE;*/

		}

	}



	settings.techLevel = 9;



	start_game();



	if(settings.gameType = ORIGINAL)

	{

		if(settings.won)

		{

			settings.campaignLevel++;

			single_campaign_start();

		}

	}

	

}



void custom_cancel()

{

	//currentWidget = &single_hbox;

	currentWidget = singleWindow;

	setHeading("Single Player\n");

}



void single_custom()

{

	settings.gameType = CUSTOM;

	settings.playerType = SINGLE;

	settings.playerNum = 0;

	strcpy(settings.mapFilename, "map.dlm");

	settings.maxPlayers = readMapMaxPlayers(settings.mapFilename);

	

	if ((settings.maxPlayers >= 1) && (settings.maxPlayers <= MAX_PLAYERS))

	{

		int i;



		custom_vbox.dropChildren();

		custom_vbox2.dropChildren();

		for(i = 0; i < settings.maxPlayers; i++)

		{

			custom_vbox.addChild(&custom_buttonName[i]);

			custom_vbox2.addChild(&custom_clientHbox[i]);

		}



		for(i = 0; i < MAX_PLAYERS; i++)

		{

			custom_buttonName[i].setText("");

			settings.playerName[i][0] = '\0';

			custom_buttonHouse[i].setText(houseName[getRandomInt(0, NUM_HOUSES-1)]);

			custom_clientHbox[i].setEnabled(false);

			custom_clientHbox[i].setVisible(false);

		}



		strncpy(settings.playerName[0], settings.localPlayerName, MAX_NAMELENGTH);

		custom_clientHbox[0].setEnabled(true);

		custom_clientHbox[0].setVisible(true);

		custom_buttonName[0].setText(settings.localPlayerName);

		custom_buttonName[0].setEnabled(false);

		settings.isHuman[0] = true;



		currentWidget = &custom_hbox;

	}

	else

		fprintf(stderr, "ERROR: map format error.\n");

}



void singlePlayer()

{

	//currentWidget = &single_hbox;

	setHeading("Single Player\n");

	currentWidget = singleWindow;

}



void skirmish()

{

	//currentWidget = &single_hbox;

	setHeading("Single Player\nSkirmish\n");

	currentWidget = skirmishWindow;

}



void skirmishStart()

{

	/*need to keep going through levels until finished*/

	/*perhaps showing mentat in between*/



	settings.gameType = SKIRMISH;

	settings.playerType = SINGLE;



	/*

	int i;

	for (i = 0; i < MAX_PLAYERS; i++)

	{

		if (single_buttonDifficulty.textEquals("Easy"))

			settings.playerDifficulty[i] = EASY;

		else if (single_buttonDifficulty.textEquals("Medium"))

			settings.playerDifficulty[i] = MEDIUM;

		else if (single_buttonDifficulty.textEquals("Hard"))

			settings.playerDifficulty[i] = HARD;

		//else 

		//	settings.playerDifficulty[i] = IMPOSSIBLE;

	}





	if (single_buttonHouse.textEquals("Ordos"))

		settings.playerNum = HOUSE_ORDOS;

	else if (single_buttonHouse.textEquals("Harkonnen"))

		settings.playerNum = HOUSE_HARKONNEN;

	else 

		settings.playerNum = HOUSE_ATREIDES;

*/



	for (int i = 0; i < MAX_PLAYERS; i++)

	{

		settings.playerDifficulty[i] = (DIFFICULTYTYPE)single_buttonDifficulty.getProgress();

	}

	settings.playerNum = selectedHouse;

		

	settings.playerColour[settings.playerNum] = settings.playerHouse[settings.playerNum] = settings.playerNum;



	for(int i = 0; i < MAX_PLAYERS; i++)

	{

		sprintf(settings.playerName[i], "Computer %d", i);

		settings.playerHouse[i] = settings.playerColour[i] = i;

		settings.playerTeam[i] = settings.playerNum + 1;

		settings.isHuman[i] = false;

	}

	

	strncpy(settings.playerName[settings.playerNum], settings.localPlayerName, MAX_NAMELENGTH);

	//set player team to be different from computers

	if (settings.playerNum == 0)

		settings.playerTeam[settings.playerNum] = settings.playerNum + 2;

	else

		settings.playerTeam[settings.playerNum] = settings.playerNum;

	settings.isHuman[settings.playerNum] = true;



//	int mapNum = selectedMission;

//	mapNum = atoi(single_buttonMap.getText());



	//if (settings.gameType == ORIGINAL)

	settings.playerHouse[settings.playerNum] = settings.playerNum;

	char temp[3];

			
	settings.mapFilename[0] = '\0';
	strncpy(settings.mapFilename, "data/scenario/scen", MAX_LINE);
	switch (settings.playerHouse[settings.playerNum])
	{
	case HOUSE_ATREIDES:
		strncat(settings.mapFilename, "a0", MAX_LINE);
		break;
	case HOUSE_ORDOS:
		strncat(settings.mapFilename, "o0", MAX_LINE);
		break;
	case HOUSE_HARKONNEN:
		strncat(settings.mapFilename, "h0", MAX_LINE);
		break;
	default:
		//make_random_map(64, 64);
		break;
	}



			

	if ((selectedMission < 1) || (selectedMission > 22))

		selectedMission = 1;

	if (selectedMission < 10)

		strncat(settings.mapFilename, "0", MAX_LINE);



	sprintf(temp, "%d", selectedMission);

	strncat(settings.mapFilename, temp, MAX_LINE);

	strncat(settings.mapFilename, ".ini", MAX_LINE);

	settings.techLevel = (selectedMission + 1)/3 + 1;

	start_game();

	if(settings.gameType == ORIGINAL)//Someone has loaded a campaign map after starting a skirmish
	{
		if(settings.won)
		{
			settings.campaignLevel++;
			single_campaign_start();
		}
	}
}







int multi_createThread(void *data)

{

	char			msg[MAX_MSGLENGTH];

	int				i, j, numReady;

	IPaddress		ip;

	TCPsocket		newClient;



	srand(time(NULL));

	multiGameStarting = false;

	settings.playerType = SERVER;



	multiServer_vbox.dropChildren();

	multiServer_vbox2.dropChildren();

	for(i = 0; i < settings.maxPlayers; i++)

	{

		multiServer_vbox.addChild(&multiServer_buttonName[i]);

		multiServer_vbox2.addChild(&multiServer_clientHbox[i]);

	}



	SDL_LockMutex(mutex_playersJoined);

	playersJoined = 1;

	SDL_UnlockMutex(mutex_playersJoined);



	SDL_LockMutex(mutex_currentWidget);

	currentWidget = &multiServer_hbox;

	SDL_UnlockMutex(mutex_currentWidget);



	printf("Hosting a multiplayer game...\n");

	SDL_LockMutex(mutex_button);

	for(i = 0; i < MAX_PLAYERS; i++)

	{

		multiServer_buttonName[i].setText("");

		multiServer_buttonName[0].setEnabled(true);

		settings.playerName[i][0] = '\0';

		sockets[i] = NULL;

		multiServer_clientHbox[i].setEnabled(false);

		multiServer_clientHbox[i].setVisible(false);

		multiServer_buttonHouse[i].setText(houseName[getRandomInt(0, NUM_HOUSES-1)]);

		//multiServer_buttonColour[0].setText(houseName[getRandomInt(0, NUM_HOUSES-1)]);

	}

	strncpy(settings.playerName[0], settings.localPlayerName, MAX_NAMELENGTH);

	multiServer_clientHbox[0].setEnabled(true);

	multiServer_clientHbox[0].setVisible(true);

	multiServer_buttonName[0].setText(settings.localPlayerName);

	multiServer_buttonName[0].setEnabled(false);

	settings.isHuman[0] = true;

	SDL_UnlockMutex(mutex_button);



	if(SDLNet_ResolveHost(&ip, NULL, settings.serverIp.port)==-1)

	{

		//fprintf(stderr, "SDLNet_ResolveHost: %s\n", SDLNet_GetError());

	}

	else

	{

		createThreadValid = true;

		sockets[0] = SDLNet_TCP_Open(&ip);

		server = sockets[0];

		SDLNet_TCP_AddSocket(socketSet, server);



		while (createThreadValid)

		{

			msg[0] = '\0';

			numReady = SDLNet_CheckSockets(socketSet, 10);

			if (numReady == -1)

				createThreadValid = false;

			else

			{

				if (numReady > 0) 

				{

					if(SDLNet_SocketReady(server))	//someone is trying to join

					{

						numReady--;

						newClient = SDLNet_TCP_Accept(server);

						if (newClient)

						{

							int players;

							SDL_LockMutex(mutex_playersJoined);

							players = playersJoined;

							SDL_UnlockMutex(mutex_playersJoined);



							if (!SDLNet_TCP_Recv(newClient, msg, MAX_MSGLENGTH))

							{

								//fprintf(stderr, "ERROR: %s\n", msg);

							}

							else if ((players < settings.maxPlayers) && (msg[0] == MSG_JOIN))

							{

								bool done = false;

								for(i = 1; !done && (i < settings.maxPlayers); i++)

								{

									if (strlen(multiServer_buttonName[i].getText()) == 0)

									{

										sockets[i] = newClient;

										SDLNet_TCP_AddSocket(socketSet, newClient);

										done = true;

									}

								}



								SDL_LockMutex(mutex_playersJoined);

								playersJoined++;

								SDL_UnlockMutex(mutex_playersJoined);

								i--;

								SDL_LockMutex(mutex_button);

								strncpy(settings.playerName[i], &msg[1], MAX_NAMELENGTH);

								multiServer_clientHbox[i].setEnabled(false);

								multiServer_clientHbox[i].setVisible(true);

								multiServer_buttonName[i].setText(&msg[1]);

								multiServer_buttonName[i].setEnabled(false);

								multiServer_colour((void*)i);

								settings.isHuman[i] = true;

								SDL_UnlockMutex(mutex_button);



								//tell client they have been accepted and their player number

								msg[0] = MSG_ACCEPT;

								sprintf(&msg[1], "%d,", i);

								SDLNet_TCP_Send(newClient, msg, MAX_MSGLENGTH);



								//tell newClient about all already joined players

								for(j = 0; j < settings.maxPlayers; j++)

								if ((strlen(multiServer_buttonName[j].getText()) != 0) && (sockets[j] != newClient))

								{

									msg[0] = MSG_JOIN;

									sprintf(&msg[1], "%d,", j);	//print position number

									strncat(msg, multiServer_buttonName[j].getText(), MAX_MSGLENGTH);

									SDLNet_TCP_Send(newClient, msg, MAX_MSGLENGTH);



									msg[0] = MSG_BUTTONHIT;	//send their setup details

									sprintf(&msg[1], "%d,%d,%d,%d", j, houseNameToNum(multiServer_buttonHouse[j].getText()), atoi(multiServer_buttonTeam[j].getText()), colourNameToNum(multiServer_buttonColour[j].getText()));

									SDLNet_TCP_Send(newClient, msg, MAX_MSGLENGTH);

								}



								msg[0] = MSG_JOIN;

								//tell all clients about the newClient(including newClient)

								sprintf(&msg[1], "%d,", i);	//print position number

								strncat(msg, multiServer_buttonName[i].getText(), MAX_MSGLENGTH);

								for(j = 1; j < settings.maxPlayers; j++)

								if (sockets[j] != NULL)

									SDLNet_TCP_Send(sockets[j], msg, MAX_MSGLENGTH);



								msg[0] = MSG_BUTTONHIT;	//send new player starting settings to all clients

								sprintf(&msg[1], "%d,%d,%d,%d", i, houseNameToNum(multiServer_buttonHouse[i].getText()), atoi(multiServer_buttonTeam[i].getText()), colourNameToNum(multiServer_buttonColour[i].getText()));

								for(j = 1; j < settings.maxPlayers; j++)

								if (sockets[j] != NULL)

									SDLNet_TCP_Send(sockets[j], msg, MAX_MSGLENGTH);

							}

							else	//too many players

							{

								printf("too many players for %s to join.\n", &msg[1]);

								SDLNet_TCP_Close(newClient);

							}

						}

					}



					//fprintf(create, "trying to check sockets with numReady %d\n", numReady);

					for (i = 1; numReady && (i < settings.maxPlayers); i++)

					{

						//fprintf(create, "checking socket \"%d\"\n", i);

						if (SDLNet_SocketReady(sockets[i]))

						{

							//fprintf(create, "socket is ready \"%d\"\n", i);

							if (SDLNet_TCP_Recv(sockets[i], msg, MAX_MSGLENGTH))

							{

								numReady--;

								//fprintf(create, "got a message \"%s\"\n", msg);

								switch (msg[0])

								{

									case MSG_BUTTONHIT:

									{

										int row, house, team, colour;



										msg[MAX_MSGLENGTH-1] = '\0';	//eliminate possible crash

										sscanf(&msg[1], "%d,%d,%d,%d", &row, &house, &team, &colour);

										if ((row != i)	//clients can only change their own settings)

											|| ((house < 0) || (house >= NUM_HOUSES))

											|| ((team < 1) || (team > settings.maxPlayers))

											|| ((colour < 0) || (colour >= NUM_HOUSES)))

											createThreadValid = false;

										else

										{

											char temp[10];

											sprintf(temp, "%d", team);

											SDL_LockMutex(mutex_button);

											multiServer_buttonHouse[row].setText(houseName[house]);

											multiServer_buttonTeam[row].setText(temp);

											multiServer_buttonColour[row].setText(colourName[colour]);

											SDL_UnlockMutex(mutex_button);



											//forward the message to all clients

											for(j = 1; j < settings.maxPlayers; j++)

											if (sockets[j] != NULL)

												SDLNet_TCP_Send(sockets[j], msg, MAX_MSGLENGTH);

										}

										break;

									}

									case MSG_QUIT:

									{

										if (strcmp(&msg[1], settings.playerName[i]) == 0)	//server has quit

										{

											printf("%s has quit\n", &msg[1]);

											SDL_LockMutex(mutex_button);

											multiServer_clientHbox[i].setVisible(false);

											multiServer_buttonName[i].setText("");

											multiServer_buttonName[i].setEnabled(true);

											multiServer_buttonHouse[i].setEnabled(false);

											multiServer_buttonTeam[i].setEnabled(false);

											multiServer_buttonColour[i].setEnabled(false);

											settings.playerName[i][0] = '\0';

											SDLNet_TCP_DelSocket(socketSet, sockets[i]);

											SDLNet_TCP_Close(sockets[i]);

											sockets[i] = NULL;



											SDL_LockMutex(mutex_playersJoined);

											playersJoined--;

											SDL_UnlockMutex(mutex_playersJoined);

											SDL_UnlockMutex(mutex_button);



											//tell all clients that a player has quit

											//fprintf(create, "telling everyone that %s has quit\n", &msg[1]);

											for(j = 1; j < settings.maxPlayers; j++)

											if (sockets[j] != NULL)

											{

												//fprintf(create, "told client %d that %s has quit\n", j, &msg[1]);

												SDLNet_TCP_Send(sockets[j], msg, MAX_MSGLENGTH);

											}

										}

										break;

									}

									default:

										//fprintf(stderr, "ERROR: invalid MSG type, message ignored\n");

										break;

								}

							}

							else

							{

								SDL_LockMutex(mutex_button);

								multiServer_clientHbox[i].setVisible(false);

								multiServer_clientHbox[i].setEnabled(false);

								multiServer_buttonName[i].setText("");

								multiServer_buttonName[i].setEnabled(true);



								SDLNet_TCP_DelSocket(socketSet, sockets[i]);

								SDLNet_TCP_Close(sockets[i]);

								sockets[i] = NULL;



								SDL_LockMutex(mutex_playersJoined);

								playersJoined--;

								SDL_UnlockMutex(mutex_playersJoined);

								SDL_UnlockMutex(mutex_button);



								//tell all clients that player has quit

								//fprintf(create, "telling everyone that %s has quit\n", &msg[1]);

								for(j = 1; j < settings.maxPlayers; j++)

								if (sockets[j] != NULL)

								{

									//fprintf(create, "told client %d that %s has quit\n", j, &msg[1]);

									SDLNet_TCP_Send(sockets[j], msg, MAX_MSGLENGTH);

								}

								//createThreadValid = false;

							}

						}

					}

				}



				//check for button presses

				SDL_LockMutex(mutex_button);

				if (multi_buttonHit)

				{

					msg[0] = MSG_BUTTONHIT;	//say a button was used

					sprintf(&msg[1], "%d,%d,%d,%d", multi_buttonRow, houseNameToNum(multiServer_buttonHouse[multi_buttonRow].getText()), atoi(multiServer_buttonTeam[multi_buttonRow].getText()), colourNameToNum(multiServer_buttonColour[multi_buttonRow].getText()));

					for(j = 1; j < settings.maxPlayers; j++)

					if (sockets[j] != NULL)

						SDLNet_TCP_Send(sockets[j], msg, MAX_MSGLENGTH);



					multi_buttonHit = false;

				}

				SDL_UnlockMutex(mutex_button);

			}

		}

	}

	//fflush(create);

	SDL_LockMutex(mutex_button);

	if (multiGameStarting)

	{	//let all clients know to start the game

		int	i, j,

			count, numbers[MAX_PLAYERS];



		for(i = 0; i < MAX_PLAYERS; i++)

			settings.playerMapNumber[i] = NONE;



		if (readMapPlayerNumbers(settings.mapFilename, numbers) == settings.maxPlayers)

		{

			for(i = 0; i < MAX_PLAYERS; i++)

			{

				strcpy(settings.playerName[i], multiServer_buttonName[i].getText());

				settings.playerHouse[i] = houseNameToNum(multiServer_buttonHouse[i].getText());		

				settings.playerTeam[i] = atoi(multiServer_buttonTeam[i].getText());

				settings.playerColour[i] = colourNameToNum(multiServer_buttonColour[i].getText());	

				

				if (strlen(settings.playerName[i]))

				{
					do
					{
						count = 0;
						settings.playerMapNumber[i] = numbers[getRandomInt(0, settings.maxPlayers-1)];
						for (j = 0; j < settings.maxPlayers; j++)	//check no other players have same number
						if ((i != j) && (settings.playerMapNumber[i] == settings.playerMapNumber[j]))
							count++;
					} while (count > 0);



					if (strstr(multiServer_buttonName[i].getText(), "Easy"))

						settings.playerDifficulty[i] = EASY;

					else if (strstr(multiServer_buttonName[i].getText(), "Medium"))

						settings.playerDifficulty[i] = MEDIUM;

					else if (strstr(multiServer_buttonName[i].getText(), "Hard"))

						settings.playerDifficulty[i] = HARD;

					/*else if (strstr(multiServer_buttonName[i].getText(), "Impossible"))

						settings.playerDifficulty[i] = IMPOSSIBLE;*/

				}

				else

					settings.playerMapNumber[i] = NONE;

			}



			msg[0] = MSG_START;

			sprintf(&msg[1], "%d,%d,%d,%d,%d,%d", settings.playerMapNumber[0], settings.playerMapNumber[1], settings.playerMapNumber[2], settings.playerMapNumber[3], settings.playerMapNumber[4], settings.playerMapNumber[5]);

			for(j = 1; j < settings.maxPlayers; j++)

			if (sockets[j] != NULL)

				SDLNet_TCP_Send(sockets[j], msg, MAX_MSGLENGTH);

		}

		else

		{

			multiGameStarting = false;

			showMessage("ERROR: map file corrupted");

		}



	}

	else

	{

		//tell all clients i have cancelled the game

		msg[0] = MSG_QUIT;

		strncpy(&msg[1], settings.localPlayerName, MAX_MSGLENGTH-1);

		//fprintf(create, "telling everyone game is cancelled %s\n", settings.playerName[i]);

		SDL_LockMutex(mutex_button);

		for(j = 1; j < MAX_PLAYERS; j++)

		if (sockets[j] != NULL)

		{

			SDLNet_TCP_Send(sockets[j], msg, MAX_MSGLENGTH);

			SDLNet_TCP_DelSocket(socketSet, sockets[j]);

			SDLNet_TCP_Close(sockets[j]);

		}

		SDLNet_TCP_DelSocket(socketSet, server);

		SDLNet_TCP_Close(server);

	}



	//fprintf(create, "create thread quit\n");

	//fclose(create);



	multi_buttonCreate.setEnabled(true);

	SDL_LockMutex(mutex_currentWidget);

	currentWidget = &multi_hbox;

	SDL_UnlockMutex(mutex_currentWidget);

	SDL_UnlockMutex(mutex_button);



	return 1;

}



void multi_cancel()

{

	strcpy(settings.lastAddress, multi_address.getText());

	cancel();

}



void multi_create()

{

	strcpy(settings.mapFilename, "map.dlm");

	settings.maxPlayers = readMapMaxPlayers(settings.mapFilename);
	settings.playerNum = 0;

	if ((settings.maxPlayers >= 1) && (settings.maxPlayers <= MAX_PLAYERS))

	{

		multi_buttonCreate.setEnabled(false);

		multiGame_thread = SDL_CreateThread(multi_createThread, NULL);

	}

	else

		fprintf(stderr, "ERROR: map format error.\n");

}



int multi_joinThread(void *data)

{

	int i, numReady;

	char	msg[MAX_MSGLENGTH];

	IPaddress	ip;



	multiGameStarting = false;

	settings.playerNum = NONE;

	settings.playerType = CLIENT;



	multiClient_vbox.dropChildren();

	multiClient_vbox2.dropChildren();

	for(i = 0; i < settings.maxPlayers; i++)

	{

		multiClient_vbox.addChild(&multiClient_buttonName[i]);

		multiClient_vbox2.addChild(&multiClient_clientHbox[i]);

	}



	SDL_LockMutex(mutex_button);

	for(i = 0; i < MAX_PLAYERS; i++)

	{

		multiClient_buttonName[i].setText("");

		settings.playerName[i][0] = '\0';

		multiClient_clientHbox[i].setEnabled(false);

		multiClient_clientHbox[i].setVisible(false);

	}

	SDL_UnlockMutex(mutex_button);





	//fprintf(join, "Trying to join multiplayer game %s\n", settings.serverString);

	if(SDLNet_ResolveHost(&ip, settings.serverString, settings.serverIp.port)==-1)

	{

		//printf("SDLNet_ResolveHost: %s\n", SDLNet_GetError());

	}

	else

	{

		server = SDLNet_TCP_Open(&ip);

		if(!server)

		{

			//fprintf(join, "SDLNet_TCP_Open: %s\nstring:\"%s\", port:%d\n", SDLNet_GetError(), settings.serverString, settings.serverIp.port);

		}

		else

		{

			SDLNet_TCP_AddSocket(socketSet, server);

			//printf("connected to server at %s\n", SDLNet_ResolveIP(&ip));

			msg[0] = MSG_JOIN;	//say i want to join your game

			strncpy(&msg[1], settings.localPlayerName, MAX_MSGLENGTH-1);	//give my name

			//fprintf(join, "sent a message \"%s\"\n", msg);

			if(SDLNet_TCP_Send(server, msg, MAX_MSGLENGTH) < MAX_MSGLENGTH)

			{

				//joinThreadValid = false;

				fprintf(stderr, "failed to connect to server\n");

			}

			else

			{

				joinThreadValid = true;

				SDL_LockMutex(mutex_currentWidget);

				currentWidget = &multiClient_hbox;

				SDL_UnlockMutex(mutex_currentWidget);



				while(joinThreadValid)

				{

					numReady = SDLNet_CheckSockets(socketSet, 10);

					if (numReady == -1)

					{

						joinThreadValid = false;

						fprintf(stderr, "failed to connect to server\n");

					}

					else

					{

						if (numReady)

						{

							if (SDLNet_TCP_Recv(server, msg, MAX_MSGLENGTH) >= MAX_MSGLENGTH)

							{

								msg[MAX_MSGLENGTH-1] = '\0';	//eliminate possible crash

								fprintf(stdout, "got a message \"%s\"\n", msg);

								switch (msg[0])

								{

									case MSG_ACCEPT:

									{

										int num;



										num = atoi(&msg[1]);

										if ((num > 0) && (num < settings.maxPlayers))

											settings.playerNum = num;

										else

										{

											joinThreadValid = false;

											fprintf(stderr, "ERROR: Accept error - Invalid start position received from server.\n");

										}

										break;

									}

									case MSG_BUTTONHIT:

									{

										int row, house, team, colour;



										sscanf(&msg[1], "%d,%d,%d,%d", &row, &house, &team, &colour);

										if (((row < 0) || (row >= settings.maxPlayers))

											|| ((house < 0) || (house >= NUM_HOUSES))

											|| ((team < 1) || (team > settings.maxPlayers))

											|| ((colour < 0) || (colour >= NUM_HOUSES)))

											joinThreadValid = false;

										else

										{

											char temp[10];

											sprintf(temp, "%d", team);



											SDL_LockMutex(mutex_button);

											multiClient_buttonHouse[row].setText(houseName[house]);

											multiClient_buttonTeam[row].setText(temp);

											multiClient_buttonColour[row].setText(colourName[colour]);

											SDL_UnlockMutex(mutex_button);

										}

										break;

									}

									case MSG_JOIN:

									{

										bool valid = true;

										char name[MAX_MSGLENGTH];

										int pos;



										sscanf(&msg[1], "%d,", &pos);

										strcpy(name, strstr(msg, ",")+1);

										if ((pos >=0) && (pos < settings.maxPlayers))

										{

											SDL_LockMutex(mutex_button);

											if (settings.playerName[pos][0] == '\0')

											{

												multiClient_buttonName[pos].setText(name);

												strncpy(settings.playerName[pos], name, MAX_NAMELENGTH);

												if (pos == settings.playerNum)

													multiClient_clientHbox[pos].setEnabled(true);	//only allow adjustment of own settings

												else

													multiClient_clientHbox[pos].setEnabled(false);

												multiClient_clientHbox[pos].setVisible(true);

												valid = false;

											}

											SDL_UnlockMutex(mutex_button);

										}

										break;

									}

									case MSG_START:

									{

										sscanf(&msg[1], "%d,%d,%d,%d,%d,%d", &settings.playerMapNumber[0], &settings.playerMapNumber[1], &settings.playerMapNumber[2], &settings.playerMapNumber[3], &settings.playerMapNumber[4], &settings.playerMapNumber[5]);

										joinThreadValid = false;

										multiGameStarting = true;

										fprintf(stderr, "game starting\n");

										break;

									}

									case MSG_QUIT:

									{

										bool found = false;

										for(i = 0; !found && (i < MAX_PLAYERS); i++)

										{

											if (strcmp(&msg[1], settings.playerName[i]) == 0)

											{

												fprintf(stderr, "%s has quit\n", &msg[1]);

												multiClient_buttonName[i].setText("");

												settings.playerName[i][0] = '\0';

												multiClient_clientHbox[i].setEnabled(false);

												multiClient_clientHbox[i].setVisible(false);

												found = true;



												if (i == 0)	//server has quit

												{

													fprintf(stderr, "server has quit\n");

													joinThreadValid = false;

												}

											}

										}

										break;

									}

									default:

										//fprintf(stderr, "ERROR: invalid MSG type %s, message ignored\n", msg);

										break;

								}

							}

							else

							{

								fprintf(stderr, "bad message %s\n", SDLNet_GetError());

								joinThreadValid = false;

							}

						}



						if (joinThreadValid)

						{

							//check for button presses

							SDL_LockMutex(mutex_button);

							if (multi_buttonHit)

							{

								msg[0] = MSG_BUTTONHIT;	//say a button was used

								sprintf(&msg[1], "%d,%d,%d,%d", multi_buttonRow, houseNameToNum(multiClient_buttonHouse[multi_buttonRow].getText()), atoi(multiClient_buttonTeam[multi_buttonRow].getText()), colourNameToNum(multiClient_buttonColour[multi_buttonRow].getText()));

								if(SDLNet_TCP_Send(server, msg, MAX_MSGLENGTH) < MAX_MSGLENGTH)

								{

									joinThreadValid = false;

									fprintf(stderr, "10 failed to connect to server\n");

								}



								multi_buttonHit = false;

							}

							SDL_UnlockMutex(mutex_button);

						}

					}

				}

			}

		}

	}



	SDL_LockMutex(mutex_button);

	if (server && !multiGameStarting)

	{

		//fprintf(join, "telling server im quiting\n");

		msg[0] = MSG_QUIT;	//say i am quiting

		strncpy(&msg[1], settings.localPlayerName, MAX_MSGLENGTH-1);	//give my name

		if (SDLNet_TCP_Send(server, msg, MAX_MSGLENGTH) < MAX_MSGLENGTH)

		{

			fprintf(stderr, "couldn't send im quiting\n");

		}

		SDLNet_TCP_DelSocket(socketSet, server);

		SDLNet_TCP_Close(server);

	}

	//fprintf(join, "join thread quit\n");

	//fclose(join);



	SDL_LockMutex(mutex_currentWidget);

	currentWidget = &multi_hbox;

	multi_buttonJoin.setEnabled(true);

	SDL_UnlockMutex(mutex_currentWidget);

	SDL_UnlockMutex(mutex_button);

	return 1;

}



void multi_join()

{

	strcpy(settings.mapFilename, "map.dlm");

	settings.maxPlayers = readMapMaxPlayers(settings.mapFilename);



	multi_buttonJoin.setEnabled(false);

	strncpy(settings.serverString, multi_address.getText(), MAX_LINE);

	multiGame_thread = SDL_CreateThread(multi_joinThread, NULL);

}



void multiServer_cancel()

{

	createThreadValid = false;

	//sleep(1);

	//SDL_KillThread(multiServer_thread);

}



void multiServer_kick()

{

	

}



void multiServer_message()

{



}



void multiServer_name(void *data)

{

	char temp[30];

	int i = (int)data;

	SDL_LockMutex(mutex_button);

	if ((i >= 0) && (i < MAX_PLAYERS))

	{

		char msg[MAX_MSGLENGTH];

		msg[MAX_MSGLENGTH-1] = '\0';

		if (multiServer_buttonName[i].textEquals(""))

		{

			multiServer_clientHbox[i].setEnabled(true);

			multiServer_clientHbox[i].setVisible(true);

			sprintf(temp, "Computer %d (Easy)", i);

			multiServer_buttonName[i].setText(temp);

			multiServer_colour((void*)i);

			settings.isHuman[i] = false;



			SDL_LockMutex(mutex_playersJoined);

			playersJoined++;

			SDL_UnlockMutex(mutex_playersJoined);

		}

		else if (strstr(multiServer_buttonName[i].getText(), "Easy") > 0)

		{

			strncpy(&msg[1], multiServer_buttonName[i].getText(), MAX_MSGLENGTH-1);



			sprintf(temp, "Computer %d (Medium)", i);

			multiServer_buttonName[i].setText(temp);

			multiServer_buttonHouse[i].setText(houseName[getRandomInt(0, NUM_HOUSES-1)]);

		}

		else if (strstr(multiServer_buttonName[i].getText(), "Medium") > 0)

		{

			strncpy(&msg[1], multiServer_buttonName[i].getText(), MAX_MSGLENGTH-1);



			sprintf(temp, "Computer %d (Hard)", i);

			multiServer_buttonName[i].setText(temp);

		}

		else if (strstr(multiServer_buttonName[i].getText(), "Hard") > 0)

		/*{

			sprintf(temp, "Computer %d (Impossible)", i);

			multiServer_buttonName[i].setText(temp);

		}

		else if (strstr(multiServer_buttonName[i].getText(), "Impossible") > 0)*/

		{

			strncpy(&msg[1], multiServer_buttonName[i].getText(), MAX_MSGLENGTH-1);



			multiServer_clientHbox[i].setEnabled(false);

			multiServer_clientHbox[i].setVisible(false);

			multiServer_buttonName[i].setText("");

			//multiServer_buttonHouse[i].setText("");

			//multiServer_buttonHouse[i].setEnabled(false);

			//multiServer_buttonTeam[i].setText("");

			//multiServer_buttonTeam[i].setEnabled(false);

			//multiServer_buttonColour[i].setText("");

			//multiServer_buttonColour[i].setEnabled(false);



			SDL_LockMutex(mutex_playersJoined);

			playersJoined--;

			SDL_UnlockMutex(mutex_playersJoined);

		}



		if (strstr(multiServer_buttonName[i].getText(), "Easy") <= 0)	//if existing computer player there before, we must destroy it first

		{	//so all but first must destroy

			//tell all clients that player has quit



			msg[0] = MSG_QUIT;

			for(int j = 1; j < settings.maxPlayers; j++)

			if (sockets[j] != NULL)

			{

				//fprintf(create, "told client %d that %s has quit\n", j, &msg[1]);

				SDLNet_TCP_Send(sockets[j], msg, MAX_MSGLENGTH);

			}

		}



		if (strlen(multiServer_buttonName[i].getText()))	//if added a computer player

		{

			int j;

			msg[0] = MSG_JOIN;

			sprintf(&msg[1], "%d,", i);

			strncat(msg, multiServer_buttonName[i].getText(), MAX_MSGLENGTH);

			for(j = 1; j < settings.maxPlayers; j++)

			if (sockets[j] != NULL)

				SDLNet_TCP_Send(sockets[j], msg, MAX_MSGLENGTH);



			msg[0] = MSG_BUTTONHIT;	//send new player starting settings

			sprintf(&msg[1], "%d,%d,%d,%d", i, houseNameToNum(multiServer_buttonHouse[i].getText()), atoi(multiServer_buttonTeam[i].getText()), colourNameToNum(multiServer_buttonColour[i].getText()));

			for(j = 1; j < settings.maxPlayers; j++)

			if (sockets[j] != NULL)

				SDLNet_TCP_Send(sockets[j], msg, MAX_MSGLENGTH);

		}



		multi_buttonHit = true;

		multi_buttonRow = i;

	}

	SDL_UnlockMutex(mutex_button);

}



void multiServer_house(void *data)

{

	int i = (int)data;

	SDL_LockMutex(mutex_button);

	if ((i >= 0) && (i < MAX_PLAYERS))

	{

		int j = houseNameToNum(multiServer_buttonHouse[i].getText()) + 1;

		if (j >= NUM_HOUSES)

			j = 0;

		multiServer_buttonHouse[i].setText(houseName[j]);



		multi_buttonHit = true;

		multi_buttonRow = i;

	}

	SDL_UnlockMutex(mutex_button);

}



void multiServer_team(void *data)

{

	int i = (int)data;

	SDL_LockMutex(mutex_button);

	if ((i >= 0) && (i < settings.maxPlayers))

	{

		int team = atoi(multiServer_buttonTeam[i].getText());

		char temp[10];



		if (++team > settings.maxPlayers)

			team = 1;

		sprintf(temp, "%d", team);

		multiServer_buttonTeam[i].setText(temp);



		multi_buttonHit = true;

		multi_buttonRow = i;

	}

	SDL_UnlockMutex(mutex_button);

}



void multiServer_colour(void *data)

{

	int i = (int)data;

	SDL_LockMutex(mutex_button);

	if ((i >= 0) && (i < MAX_PLAYERS))

	{

		bool alreadyUsed;

		int j;

		do

		{

			j = colourNameToNum(multiServer_buttonColour[i].getText()) + 1;

			if (j >= NUM_HOUSES)

				j = 0;

			multiServer_buttonColour[i].setText(colourName[j]);



			alreadyUsed = false;

			for(j = 0; j < settings.maxPlayers; j++)

			if ((j != i) && strlen(multiServer_buttonName[j].getText()) && (strcmp(multiServer_buttonColour[j].getText(), multiServer_buttonColour[i].getText()) == 0))

				alreadyUsed = true;

		} while (alreadyUsed);



		multi_buttonHit = true;

		multi_buttonRow = i;

	}

	SDL_UnlockMutex(mutex_button);

}



void multiServer_start()

{

	multiGameStarting = true;

	createThreadValid = false;

	SDL_WaitThread(multiGame_thread, NULL);



	printf("starting multiplayer game\n");fflush(stdout);

	start_game();

}



void multiClient_house(void *data)

{

	int i = (int)data;

	SDL_LockMutex(mutex_button);

	if (i == settings.playerNum)

	{

		int j = houseNameToNum(multiClient_buttonHouse[i].getText()) + 1;

		if (j >= NUM_HOUSES)

			j = 0;

		multiClient_buttonHouse[i].setText(houseName[j]);



		multi_buttonHit = true;

		multi_buttonRow = i;

	}

	SDL_UnlockMutex(mutex_button);

}



void multiClient_team(void *data)

{

	int i = (int)data;

	SDL_LockMutex(mutex_button);

	if (i == settings.playerNum)

	{

		int team = atoi(multiClient_buttonTeam[i].getText());

		char temp[10];



		if (++team > settings.maxPlayers)

			team = 1;

		sprintf(temp, "%d", team);

		multiClient_buttonTeam[i].setText(temp);



		multi_buttonHit = true;

		multi_buttonRow = i;

	}

	SDL_UnlockMutex(mutex_button);

}



void multiClient_colour(void *data)

{

	int i = (int)data;

	SDL_LockMutex(mutex_button);

	if (i == settings.playerNum)

	{

		bool alreadyUsed;

		int j;



		do

		{

			j = colourNameToNum(multiClient_buttonColour[i].getText()) + 1;

			if (j >= NUM_HOUSES)

				j = 0;

			multiClient_buttonColour[i].setText(colourName[j]);



			alreadyUsed = false;

			for(j = 0; j < settings.maxPlayers; j++)

				if ((j != i) && strlen(multiClient_buttonName[j].getText()) && (strcmp(multiClient_buttonColour[j].getText(), multiClient_buttonColour[i].getText()) == 0))

				alreadyUsed = true;

		} while (alreadyUsed);



		multi_buttonHit = true;

		multi_buttonRow = i;

	}

	SDL_UnlockMutex(mutex_button);

}



void multiClient_cancel()

{

	joinThreadValid = false;

	SDL_LockMutex(mutex_currentWidget);

	currentWidget = &multi_hbox;

	SDL_UnlockMutex(mutex_currentWidget);

	SDL_WaitThread(multiGame_thread, NULL);

}



void multiClient_start()

{

	SDL_WaitThread(multiGame_thread, NULL);



	if (settings.playerNum == NONE)

		fprintf(stderr, "ERROR: playernum not received from server.\n");

	else

	{

		printf("starting multiplayer game\n");fflush(stdout);

		for(int i = 0; i < MAX_PLAYERS; i++)

		{

			strcpy(settings.playerName[i], multiClient_buttonName[i].getText());

			settings.playerHouse[i] = houseNameToNum(multiClient_buttonHouse[i].getText());		

			settings.playerTeam[i] = atoi(multiClient_buttonTeam[i].getText());

			settings.playerColour[i] = colourNameToNum(multiClient_buttonColour[i].getText());

			settings.isHuman[i] = true;

			

			if (strstr(multiClient_buttonName[i].getText(), "Easy"))

				settings.playerDifficulty[i] = EASY;

			else if (strstr(multiClient_buttonName[i].getText(), "Medium"))

				settings.playerDifficulty[i] = MEDIUM;

			else if (strstr(multiClient_buttonName[i].getText(), "Hard"))

				settings.playerDifficulty[i] = HARD;

			/*else if (strstr(multiClient_buttonName[i].getText(), "Impossible"))

				settings.playerDifficulty[i] = IMPOSSIBLE;*/

		}



		multiGameStarting = true;

		joinThreadValid = false;



		start_game();

	}

}



void multiClient_message()

{



}



void multiPlayer()

{

	settings.gameType = MULTIPLAYER;

	settings.techLevel = 9;



	multi_address.setText(settings.lastAddress);

	currentWidget = &multi_hbox;

}



void mapEditor()

{

	int i;



	settings.maxPlayers = MAX_PLAYERS;

	settings.playerNum = 0;

	for(i = 0; i < MAX_PLAYERS; i++)

	{

		sprintf(settings.playerName[i], "Computer %d", i);

		settings.playerHouse[i] = settings.playerColour[i] = i;

		settings.playerTeam[i] = 1;

		settings.isHuman[i] = false;

	}

	strncpy(settings.playerName[settings.playerNum], settings.localPlayerName, MAX_NAMELENGTH);

	settings.isHuman[settings.playerNum] = true;





	do_editor();

	soundPlayer->setMusic(true);

	focusedWidget = NULL;

}



void options()

{

	currentWidget = &options_vbox;



	if (settings.concreteRequired)

		options_buttonConcrete.setText("Concrete Required");

	else

		options_buttonConcrete.setText("Concrete Not Required");



	if (strcmp(settings.screen, "Full Screen") == 0)

		options_buttonFullScreen.setText("Full Screen");

	else

		options_buttonFullScreen.setText("Windowed");



	if (settings.doubleBuffered)

		options_buttonDoubleBuffered.setText("Double Buffered");

	else

		options_buttonDoubleBuffered.setText("Single Buffered");



	if (settings.width == 320)

		options_buttonRes.setText("320x240");

	else if (settings.width == 640)

		options_buttonRes.setText("640x480");

	else if (settings.width == 800)

		options_buttonRes.setText("800x600");

	else

		options_buttonRes.setText("1024x768");



	options_name.setText(settings.localPlayerName);

}



void options_concrete()

{

	if (options_buttonConcrete.textEquals("Concrete Required"))

		options_buttonConcrete.setText("Concrete Not Required");

	else

		options_buttonConcrete.setText("Concrete Required");

}



void options_fullScreen()

{

	if (options_buttonFullScreen.textEquals("Full Screen"))

		options_buttonFullScreen.setText("Windowed");

	else

		options_buttonFullScreen.setText("Full Screen");

}



void options_res()

{

	if (options_buttonRes.textEquals("320x240"))

		options_buttonRes.setText("640x480");

	else if (options_buttonRes.textEquals("640x480"))

		options_buttonRes.setText("800x600");

	else if (options_buttonRes.textEquals("800x600"))

		options_buttonRes.setText("1024x768");

	else

		options_buttonRes.setText("320x240");

}



void options_doubleBuffered()

{

	if (options_buttonDoubleBuffered.textEquals("Double Buffered"))

		options_buttonDoubleBuffered.setText("Single Buffered");

	else

		options_buttonDoubleBuffered.setText("Double Buffered");

}



void options_cancel()

{

	//currentWidget = &main_vbox;

	currentWidget = mainWindow;

}



void options_ok()

{

	strcpy(settings.localPlayerName, options_name.getText());



	if (options_buttonConcrete.textEquals("Concrete Required"))

		settings.concreteRequired = true;

	else

		settings.concreteRequired = false;



	if (options_buttonFullScreen.textEquals("Full Screen"))

		strcpy(settings.screen, "Full Screen");

	else

		strcpy(settings.screen, "Windowed");

	

	if (options_buttonRes.textEquals("320x240"))

	{

		settings.width = 320;

		settings.height = 240;

	}

	else if (options_buttonRes.textEquals("640x480"))

	{

		settings.width = 640;

		settings.height = 480;

	}

	else if (options_buttonRes.textEquals("800x600"))

	{

		settings.width = 800;

		settings.height = 600;

	}

	else

	{

		settings.width = 1024;

		settings.height = 768;

	}



	settings.doubleBuffered = options_buttonDoubleBuffered.textEquals("Double Buffered");

	setVideoMode();

	realign_buttons();



	//currentWidget = &main_vbox;

	currentWidget = mainWindow;

}



void about()

{

	currentWidget = &about_window;

}



void quit()

{

	quiting = true;

}

void setVideoMode()
{
	int videoFlags = 0;
	if (settings.doubleBuffered)
		videoFlags |= SDL_HWSURFACE | SDL_DOUBLEBUF;
	if (strcmp(settings.screen, "Full Screen") == 0)
		videoFlags |= SDL_FULLSCREEN;

	screen = SDL_SetVideoMode(settings.width, settings.height, SCREEN_BPP, videoFlags);
	if(screen)
	{
		SDL_Surface	*menu;

		SDL_SetColors(screen, palette->colors, 0, palette->ncolors);
		SDL_ShowCursor(SDL_DISABLE);

		menu = (SDL_Surface*)(dataFile[UI_Menu].dat);
		backgroundOffsetX = (screen->w - menu->w)/2;
		backgroundOffsetY = (screen->h - menu->h)/2;

		//resize all buttons
//		main_vbox.setX(screen->w/3);

//		main_vbox.setWidth(screen->w/3);

//		main_vbox.setY(screen->h/2 + 32);

//		main_vbox.setHeight(screen->h - main_vbox.getY() - 10);
//		main_vbox.resizeChildren();

		single_hbox.setX(10);

		single_hbox.setWidth(screen->w - 20);

		single_hbox.setY(10);

		single_hbox.setHeight(screen->h - 20);
		single_hbox.resizeChildren();

		custom_hbox.setX(10);

		custom_hbox.setWidth(screen->w - 20);

		custom_hbox.setY(10);

		custom_hbox.setHeight(screen->h - 20);
		custom_hbox.resizeChildren();

		multi_hbox.setX(10);

		multi_hbox.setWidth(screen->w - 20);

		multi_hbox.setY(10);

		multi_hbox.setHeight(screen->h - 20);
		multi_hbox.resizeChildren();

		multiServer_hbox.setX(10);

		multiServer_hbox.setWidth(screen->w - 20);

		multiServer_hbox.setY(10);

		multiServer_hbox.setHeight(screen->h - 20);
		multiServer_hbox.resizeChildren();

		multiClient_hbox.setX(10);

		multiClient_hbox.setWidth(screen->w - 20);

		multiClient_hbox.setY(10);

		multiClient_hbox.setHeight(screen->h - 20);
		multiClient_hbox.resizeChildren();

		options_vbox.setX(10);

		options_vbox.setWidth(screen->w - 20);

		options_vbox.setY(10);

		options_vbox.setHeight(screen->h - 20);
		options_vbox.resizeChildren();

		about_window.setX(10);

		about_window.setWidth(screen->w - 20);

		about_window.setY(10);

		about_window.setHeight(screen->h - 20);

		fixDisplayFormat();
    }

	else

	{
		fprintf(stderr, "ERROR: Couldn't set video mode: %s\n", SDL_GetError());
		quit();
    }
}







void realign_buttons()

{

	/////////// create buttons ///////////////

	SDL_Surface *surf, *surfPressed;

	int i;



	surf = ((SDL_Surface *)dataFile[UI_Menu].dat);



	mainWindow = new Window();

	mainWindow->setHeight(surf->h);

	mainWindow->setWidth(surf->w);

	

	if(screen->w > surf->w)

		mainWindow->setX((screen->w - surf->w)/2);

	else

		mainWindow->setX(0);



	if(screen->h > surf->h)

		mainWindow->setY((screen->h - surf->h)/2);

	else

		mainWindow->setY(0);



	surf = ((SDL_Surface *)dataFile[UI_Single].dat);

	surfPressed = ((SDL_Surface *)dataFile[UI_Single_Pressed].dat);



	//main_buttonSinglePlayer.setText("Single-Player Game");

	main_buttonSinglePlayer.setWidth(surf->w);

	main_buttonSinglePlayer.setHeight(surf->h);

	main_buttonSinglePlayer.setSurface(surf, NULL);

	main_buttonSinglePlayer.setPressedSurface(surfPressed, NULL);

	main_buttonSinglePlayer.setCallBack(singlePlayer);

	mainWindow->addChild(&main_buttonSinglePlayer);

	main_buttonSinglePlayer.setVisible(true);

	main_buttonSinglePlayer.setX(mainWindow->getX() + 240);

	main_buttonSinglePlayer.setY(mainWindow->getY() + 310);



	

	surf = ((SDL_Surface *)dataFile[UI_Multi].dat);

	surfPressed = ((SDL_Surface *)dataFile[UI_Multi_Pressed].dat);



	main_buttonMultiPlayer.setWidth(surf->w);

	main_buttonMultiPlayer.setHeight(surf->h);

	main_buttonMultiPlayer.setSurface(surf, NULL);

	main_buttonMultiPlayer.setPressedSurface(surfPressed, NULL);

	//main_buttonMultiPlayer.setText("Multi-Player Game");

	main_buttonMultiPlayer.setCallBack(multiPlayer);

	mainWindow->addChild(&main_buttonMultiPlayer);

	main_buttonMultiPlayer.setVisible(true);

	main_buttonMultiPlayer.setX(main_buttonSinglePlayer.getX());

	main_buttonMultiPlayer.setY(main_buttonSinglePlayer.getY() + 19);



	surf = ((SDL_Surface *)dataFile[UI_MapEdit].dat);

	surfPressed = ((SDL_Surface *)dataFile[UI_MapEdit_Pressed].dat);

	main_buttonMapEditor.setWidth(surf->w);

	main_buttonMapEditor.setHeight(surf->h);

	main_buttonMapEditor.setSurface(surf, NULL);

	main_buttonMapEditor.setPressedSurface(surfPressed, NULL);

//	main_buttonMapEditor.setText("Map Editor");

	main_buttonMapEditor.setCallBack(mapEditor);

	mainWindow->addChild(&main_buttonMapEditor);

	main_buttonMapEditor.setVisible(true);

	main_buttonMapEditor.setX(main_buttonMultiPlayer.getX());

	main_buttonMapEditor.setY(main_buttonMultiPlayer.getY() + 19);



	surf = ((SDL_Surface *)dataFile[UI_OptionsMM].dat);

	surfPressed = ((SDL_Surface *)dataFile[UI_OptionsMM_Pressed].dat);

	main_buttonOptions.setWidth(surf->w);

	main_buttonOptions.setHeight(surf->h);

	main_buttonOptions.setSurface(surf, NULL);

	main_buttonOptions.setPressedSurface(surfPressed, NULL);

//	main_buttonOptions.setText("Options");

	main_buttonOptions.setCallBack(options);

	mainWindow->addChild(&main_buttonOptions);

	main_buttonOptions.setVisible(true);

	main_buttonOptions.setX(main_buttonMapEditor.getX());

	main_buttonOptions.setY(main_buttonMapEditor.getY() + 19);



	surf = ((SDL_Surface *)dataFile[UI_About].dat);

	surfPressed = ((SDL_Surface *)dataFile[UI_About_Pressed].dat);

	main_buttonAbout.setWidth(surf->w);

	main_buttonAbout.setHeight(surf->h);

	main_buttonAbout.setSurface(surf, NULL);

	main_buttonAbout.setPressedSurface(surfPressed, NULL);

//	main_buttonAbout.setText("About");

	main_buttonAbout.setCallBack(about);

	mainWindow->addChild(&main_buttonAbout);

	main_buttonAbout.setVisible(true);

	main_buttonAbout.setX(main_buttonOptions.getX());

	main_buttonAbout.setY(main_buttonOptions.getY() + 19);



	surf = ((SDL_Surface *)dataFile[UI_Quit].dat);

	surfPressed = ((SDL_Surface *)dataFile[UI_Quit_Pressed].dat);

	main_buttonQuit.setWidth(surf->w);

	main_buttonQuit.setHeight(surf->h);

	main_buttonQuit.setSurface(surf, NULL);

	main_buttonQuit.setPressedSurface(surfPressed, NULL);

//	main_buttonQuit.setText("Quit");

	main_buttonQuit.setCallBack(quit);

	mainWindow->addChild(&main_buttonQuit);

	main_buttonQuit.setVisible(true);

	main_buttonQuit.setX(main_buttonAbout.getX());

	main_buttonQuit.setY(main_buttonAbout.getY() + 19);



	mainWindow->setVisible(true);

	currentWidget = mainWindow;	//set main to be the current menu



///////////////////// single player menu



	int menuH = ((SDL_Surface *)dataFile[UI_Menu].dat)->h;

	int menuW = ((SDL_Surface *)dataFile[UI_Menu].dat)->w;



	singleWindow = new Window();

	singleWindow->setWidth(menuW);

	singleWindow->setHeight(menuH);



	if(screen->w > menuW)

		singleWindow->setX((screen->w - menuW) / 2);

	else

		singleWindow->setX(0);



	if(screen->h > menuH)

		singleWindow->setY(((screen->h - menuH) / 2));// + (menuH));

	else

		singleWindow->setY(0);





	headingLabel = new Label();

	headingLabel->setBackgroundColour(COLOUR_BLACK);

	headingLabel->setTextColour(COLOUR_RED);



/*

	surf = ((SDL_Surface *)dataFile[UI_Menu_Background].dat);

	//surfPressed = ((SDL_Surface *)dataFile[UI_Single_Campaign_Pressed].dat);



	menuBackground.setX(singleWindow->getX() + 240);

	menuBackground.setY(singleWindow->getY() + 310);

	menuBackground.setSurface(surf, NULL);

	menuBackground.setHeight(surf->h);

	menuBackground.setWidth(surf->w);

	menuBackground.setVisible(true);

	menuBackground.setEnabled(false);

*/





	surf = ((SDL_Surface *)dataFile[UI_Single_Campaign].dat);

	surfPressed = ((SDL_Surface *)dataFile[UI_Single_Campaign_Pressed].dat);



	//single_buttonCampaign.setText("Campaign");

	single_buttonCampaign.setCallBack(single_campaign);

	single_buttonCampaign.setVisible(true);

	single_buttonCampaign.setX(singleWindow->getX() + 240);

	single_buttonCampaign.setY(singleWindow->getY() + 310);

	single_buttonCampaign.setHeight(surf->h);

	single_buttonCampaign.setWidth(surf->w);

	single_buttonCampaign.setSurface(surf, NULL);

	single_buttonCampaign.setPressedSurface(surfPressed, NULL);

//	single_buttonCampaign.setY(single_buttonCustom.getY() +19);

//	single_buttonCampaign.setX(single_buttonCustom.getX());

//	single_buttonCampaign.setHeight(125);

//	single_buttonCampaign.setWidth(175);

//	single_buttonCampaign.setBackgroundColour(COLOUR_BLACK);

//	single_buttonCampaign.setBorderColour(COLOUR_RED);



	surf = ((SDL_Surface *)dataFile[UI_Single_Custom].dat);

	surfPressed = ((SDL_Surface *)dataFile[UI_Single_Custom_Pressed].dat);



	//single_buttonCustom.setText("Custom");

	single_buttonCustom.setCallBack(single_custom);

	single_buttonCustom.setVisible(true);

	single_buttonCustom.setY(single_buttonCampaign.getY() +19);

	single_buttonCustom.setX(single_buttonCampaign.getX());

	single_buttonCustom.setHeight(surf->h);

	single_buttonCustom.setWidth(surf->w);

	single_buttonCustom.setSurface(surf, NULL);

	single_buttonCustom.setPressedSurface(surfPressed, NULL);



	surf = ((SDL_Surface *)dataFile[UI_Single_Skirmish].dat);

	surfPressed = ((SDL_Surface *)dataFile[UI_Single_Skirmish_Pressed].dat);

	

	single_buttonSkirmish.setCallBack(skirmish);

	single_buttonSkirmish.setVisible(true);

	single_buttonSkirmish.setY(single_buttonCustom.getY() +19);

	single_buttonSkirmish.setX(single_buttonCustom.getX());

	single_buttonSkirmish.setHeight(surf->h);

	single_buttonSkirmish.setWidth(surf->w);

	single_buttonSkirmish.setSurface(surf, NULL);

	single_buttonSkirmish.setPressedSurface(surfPressed, NULL);

//	single_buttonSkirmish.setBackgroundColour(COLOUR_BLACK);

//	single_buttonSkirmish.setBorderColour(COLOUR_RED);



	surf = ((SDL_Surface *)dataFile[UI_Loadgame].dat);

	surfPressed = ((SDL_Surface *)dataFile[UI_Loadgame_Pressed].dat);

	

	single_buttonLoad.setCallBack(single_loadGame);

	single_buttonLoad.setVisible(true);

	single_buttonLoad.setY(single_buttonSkirmish.getY() +19);

	single_buttonLoad.setX(single_buttonSkirmish.getX());

	single_buttonLoad.setSurface(surf, NULL);

	single_buttonLoad.setPressedSurface(surfPressed, NULL);

	single_buttonLoad.setHeight(surf->h);

	single_buttonLoad.setWidth(surf->w);



	surf = ((SDL_Surface *)dataFile[UI_Single_Cancel].dat);

	surfPressed = ((SDL_Surface *)dataFile[UI_Single_Cancel_Pressed].dat);



	single_buttonCancel.setY(single_buttonLoad.getY() +19);

	single_buttonCancel.setX(single_buttonLoad.getX());

	single_buttonCancel.setVisible(true);

	single_buttonCancel.setHeight(surf->h);

	single_buttonCancel.setWidth(surf->w);

	single_buttonCancel.setSurface(surf, NULL);

	single_buttonCancel.setPressedSurface(surfPressed, NULL);

	single_buttonCancel.setCallBack(single_cancel);



	surf = ((SDL_Surface *)dataFile[UI_Difficulty].dat);

	menu_difficulty.setSurface(surf,NULL);

	//single_buttonDifficulty.setCallBack(single_difficulty);

	menu_difficulty.setVisible(true);

	menu_difficulty.setHeight(surf->h);

	menu_difficulty.setWidth(surf->w);

	menu_difficulty.setX(mainWindow->getX() + 52);

	menu_difficulty.setY(mainWindow->getY() + 304);

	menu_difficulty.setTransparent(true);

	



	surf = ((SDL_Surface *)dataFile[UI_Dif_Medium].dat);

	//single_buttonDifficulty.setText("Difficulty: Medium");

	single_buttonDifficulty.setSurface(surf,NULL);

	single_buttonDifficulty.setCallBack(single_difficulty);

	single_buttonDifficulty.setVisible(true);

	single_buttonDifficulty.setProgress(MEDIUM);

	single_buttonDifficulty.setHeight(surf->h);

	single_buttonDifficulty.setWidth(surf->w);

	single_buttonDifficulty.setX(menu_difficulty.getX() + 16);

	single_buttonDifficulty.setY(menu_difficulty.getY() + 40);



//	single_buttonDifficulty.setX(singleWindow->getX() + 240);

//	single_buttonDifficulty.setY(singleWindow->getY() + 310);

//	single_buttonDifficulty.setHeight(48);

//	single_buttonDifficulty.setWidth(200);

//	single_buttonDifficulty.setBackgroundColour(COLOUR_BLACK);

//	single_buttonDifficulty.setBorderColour(COLOUR_RED);

	





	

//	singleWindow->addChild(headingLabel);

	singleWindow->addChild(&menu_difficulty);

	singleWindow->addChild(&single_buttonLoad);

	singleWindow->addChild(&single_buttonSkirmish);

	singleWindow->addChild(&single_buttonCustom);

	singleWindow->addChild(&single_buttonCancel);

	singleWindow->addChild(&single_buttonCampaign);

	singleWindow->addChild(&single_buttonDifficulty);

	singleWindow->setVisible(true);





	skirmishWindow = new Window();



	skirmishWindow->setWidth(menuW);

	skirmishWindow->setHeight(menuH);



	if(screen->w > menuW)

		skirmishWindow->setX((screen->w - menuW) / 2);

	else

		skirmishWindow->setX(0);



	if(screen->h > menuH)

		skirmishWindow->setY(((screen->h - menuH) / 2));

	else

		skirmishWindow->setY(0);



	

/*

	Label *houseLabel = new Label();

	houseLabel->setBackgroundColour(COLOUR_BLACK);

	houseLabel->setTextColour(COLOUR_RED);

	houseLabel->setText("House: \n");

	houseLabel->setX(skirmishWindow->getX() + 16);

	houseLabel->setY(skirmishWindow->getY() + menuH + 48);

	houseLabel->setVisible(true);



	Label *levelLabel = new Label();

	levelLabel->setBackgroundColour(COLOUR_BLACK);

	levelLabel->setTextColour(COLOUR_RED);

	levelLabel->setText("Level: \n");

	levelLabel->setX(skirmishWindow->getX() + 20);

	levelLabel->setY(skirmishWindow->getY() + menuH + houseLabel->getHeight() + 10);

	levelLabel->setVisible(true);



	single_buttonHouse.setTextColour(COLOUR_RED);

	single_buttonHouse.setText("Harkonnen");

	single_buttonHouse.setCallBack(single_house);

	single_buttonHouse.setVisible(true);

	single_buttonHouse.setBackgroundColour(COLOUR_BLACK);

	single_buttonHouse.setBorderColour(COLOUR_RED);

	single_buttonHouse.setWidth(100);

	single_buttonHouse.setHeight(20);

	single_buttonHouse.setX(houseLabel->getX() + houseLabel->getWidth() + 10);

	single_buttonHouse.setY(houseLabel->getY());



	single_buttonMap.setTextColour(COLOUR_RED);

	single_buttonMap.setText("1");

	single_buttonMap.setCallBack(single_mapIncrement);

	single_buttonMap.setCallBackCancel(single_mapDecrement);

	single_buttonMap.setVisible(true);

	single_buttonMap.setBackgroundColour(COLOUR_BLACK);

	single_buttonMap.setBorderColour(COLOUR_RED);

	single_buttonMap.setWidth(20);

	single_buttonMap.setHeight(20);

	single_buttonMap.setX(levelLabel->getX() + levelLabel->getWidth() + 10);

	single_buttonMap.setY(levelLabel->getY());

*/



	

//single_buttonSkirmishStart;

//single_buttonSkirmishCancel;

	surf = ((SDL_Surface *)dataFile[UI_HouseSelect].dat);

	single_buttonSkirmishHerald.setSurface(surf, NULL);

	single_buttonSkirmishHerald.setHeight(surf->h);

	single_buttonSkirmishHerald.setWidth(surf->w);

	single_buttonSkirmishHerald.setX(skirmishWindow->getX() + 150);

	single_buttonSkirmishHerald.setY(skirmishWindow->getY() + 68);

	single_buttonSkirmishHerald.setVisible(true);

	single_buttonSkirmishHerald.setBackgroundColour(COLOUR_BLACK);

	single_buttonSkirmishHerald.setBorderColour(COLOUR_BLACK);

	//single_buttonSkirmishHerald.setTransparent(true);



	surf = ((SDL_Surface *)dataFile[UI_HeraldAtre_Coloured].dat);

	single_buttonSkirmishAtre.setSurface(surf, NULL);

	single_buttonSkirmishAtre.setHeight(surf->h);

	single_buttonSkirmishAtre.setWidth(surf->w);

	single_buttonSkirmishAtre.setX(single_buttonSkirmishHerald.getX() + 20);

	single_buttonSkirmishAtre.setY(single_buttonSkirmishHerald.getY() + 53);

	single_buttonSkirmishAtre.setEnabled(true);

	single_buttonSkirmishAtre.setTransparent(true);

	single_buttonSkirmishAtre.setCallBack(skirmish_select_atre);



	surf = ((SDL_Surface *)dataFile[UI_HeraldHark_Coloured].dat);

	single_buttonSkirmishHark.setSurface(surf, NULL);

	single_buttonSkirmishHark.setHeight(surf->h);

	single_buttonSkirmishHark.setWidth(surf->w);

	single_buttonSkirmishHark.setX(single_buttonSkirmishHerald.getX() + 214);

	single_buttonSkirmishHark.setY(single_buttonSkirmishHerald.getY() + 53);

	single_buttonSkirmishHark.setEnabled(true);

	single_buttonSkirmishHark.setTransparent(true);

	single_buttonSkirmishHark.setCallBack(skirmish_select_hark);



	surf = ((SDL_Surface *)dataFile[UI_HeraldOrd_Coloured].dat);

	single_buttonSkirmishOrdos.setSurface(surf, NULL);

	single_buttonSkirmishOrdos.setHeight(surf->h);

	single_buttonSkirmishOrdos.setWidth(surf->w);

	single_buttonSkirmishOrdos.setX(single_buttonSkirmishHerald.getX() + 116);

	single_buttonSkirmishOrdos.setY(single_buttonSkirmishHerald.getY() + 53);

	single_buttonSkirmishOrdos.setEnabled(true);

	single_buttonSkirmishOrdos.setTransparent(true);

	single_buttonSkirmishOrdos.setCallBack(skirmish_select_ordos);



	skirmish_select_atre();//make one of em selected



	surf = ((SDL_Surface *)dataFile[UI_Single_Start].dat);

	surfPressed = ((SDL_Surface *)dataFile[UI_Single_Start_Pressed].dat);

	single_buttonSkirmishStart.setCallBack(skirmishStart);

	single_buttonSkirmishStart.setVisible(true);

	single_buttonSkirmishStart.setSurface(surf, NULL);

	single_buttonSkirmishStart.setPressedSurface(surfPressed, NULL);

	single_buttonSkirmishStart.setWidth(surf->w);

	single_buttonSkirmishStart.setHeight(surf->h);

	single_buttonSkirmishStart.setX(skirmishWindow->getX() + 240);

	single_buttonSkirmishStart.setY(skirmishWindow->getY() + 310);



//	surf = ((SDL_Surface *)dataFile[UI_MissionSelect].dat);

//	single_buttonSkirmishCancel.setTextColour(COLOUR_RED);

//	single_buttonSkirmishCancel.setText("Cancel");

//	single_buttonSkirmishCancel.setCallBack(cancel);

//	single_buttonSkirmishCancel.setVisible(true);

//	single_buttonSkirmishCancel.setBackgroundColour(COLOUR_BLACK);

//	single_buttonSkirmishCancel.setBorderColour(COLOUR_RED);

//	single_buttonSkirmishCancel.setWidth(75);

//	single_buttonSkirmishCancel.setHeight(20);

//	single_buttonSkirmishCancel.setX(single_buttonSkirmishStart.getX() - single_buttonSkirmishCancel.getWidth() - 16);

//	single_buttonSkirmishCancel.setY(single_buttonSkirmishStart.getY());





	surf = ((SDL_Surface *)dataFile[UI_MissionSelect].dat);

	menu_mission.setSurface(surf, NULL);

	menu_mission.setHeight(surf->h);

	menu_mission.setWidth(surf->w);

	menu_mission.setX(skirmishWindow->getX() + 449);

	menu_mission.setY(skirmishWindow->getY() + 303);

	menu_mission.setVisible(true);

	menu_mission.setTransparent(true);



	surf = ((SDL_Surface *)dataFile[UI_Plus].dat);

	surfPressed = ((SDL_Surface *)dataFile[UI_Plus_Pressed].dat);

	single_buttonMissionPlus.setSurface(surf, NULL);

	single_buttonMissionPlus.setPressedSurface(surfPressed, NULL);

	single_buttonMissionPlus.setHeight(surf->h);

	single_buttonMissionPlus.setWidth(surf->w);

	single_buttonMissionPlus.setCallBack(single_mapIncrement);

	single_buttonMissionPlus.setVisible(true);

	single_buttonMissionPlus.setEnabled(true);

	single_buttonMissionPlus.setTransparent(true);

	single_buttonMissionPlus.setX(menu_mission.getX() + 105);

	single_buttonMissionPlus.setY(menu_mission.getY() + 5);



	surf = ((SDL_Surface *)dataFile[UI_Minus].dat);

	surfPressed = ((SDL_Surface *)dataFile[UI_Minus_Pressed].dat);

	single_buttonMissionMinus.setSurface(surf, NULL);

	single_buttonMissionMinus.setPressedSurface(surfPressed, NULL);

	single_buttonMissionMinus.setHeight(surf->h);

	single_buttonMissionMinus.setWidth(surf->w);

	single_buttonMissionMinus.setCallBack(single_mapDecrement);

	single_buttonMissionMinus.setEnabled(true);

	single_buttonMissionMinus.setVisible(true);

	single_buttonMissionMinus.setTransparent(true);

	single_buttonMissionMinus.setX(menu_mission.getX() + 105);

	single_buttonMissionMinus.setY(menu_mission.getY() + 18);



//	skirmishWindow->addChild(headingLabel);

//	skirmishWindow->addChild(houseLabel);

//	skirmishWindow->addChild(levelLabel);

//	skirmishWindow->addChild(&single_buttonMap);

	skirmishWindow->addChild(&single_buttonSkirmishHerald);

	skirmishWindow->addChild(&single_buttonSkirmishAtre);

	skirmishWindow->addChild(&single_buttonSkirmishHark);

	skirmishWindow->addChild(&single_buttonSkirmishOrdos);

	skirmishWindow->addChild(&menu_difficulty);

	skirmishWindow->addChild(&menu_mission);

	skirmishWindow->addChild(&single_buttonMissionPlus);

	skirmishWindow->addChild(&single_buttonMissionMinus);

	skirmishWindow->addChild(&single_buttonDifficulty);

	skirmishWindow->addChild(&single_buttonCancel);

	skirmishWindow->addChild(&single_buttonSkirmishStart);



	skirmishWindow->setVisible(true);





/*	single_hbox.addChild(&single_vbox);

	single_vbox.setVisible(true);



	single_hbox.addChild(&single_vbox2);

	single_vbox2.setVisible(true);



	single_buttonHouse.setText("Harkonnen");

	single_buttonHouse.setCallBack(single_house);

	single_vbox.addChild(&single_buttonHouse);

	single_buttonHouse.setVisible(true);



	single_buttonDifficulty.setText("Medium");

	single_buttonDifficulty.setCallBack(single_difficulty);

	single_vbox.addChild(&single_buttonDifficulty);

	single_buttonDifficulty.setVisible(true);



	single_buttonMap.setText("22");

	single_buttonMap.setCallBack(single_mapIncrement);

	single_buttonMap.setCallBackCancel(single_mapDecrement);

	single_vbox.addChild(&single_buttonMap);

	single_buttonMap.setVisible(true);



	single_buttonCampaign.setText("Campaign");

	single_buttonCampaign.setCallBack(single_campaign);

	single_vbox2.addChild(&single_buttonCampaign);

	single_buttonCampaign.setVisible(true);



	single_buttonRandomMap.setText("Random Map");

	single_buttonRandomMap.setCallBack(single_randomMap);

	single_vbox2.addChild(&single_buttonRandomMap);

	single_buttonRandomMap.setVisible(true);



	single_buttonSingleMap.setText("Custom");

	single_buttonSingleMap.setCallBack(single_custom);

	single_vbox2.addChild(&single_buttonSingleMap);

	single_buttonSingleMap.setVisible(true);



	single_buttonCancel.setText("Cancel");

	single_buttonCancel.setCallBack(single_cancel);

	single_vbox2.addChild(&single_buttonCancel);

	single_buttonCancel.setVisible(true);



	single_hbox.setVisible(true);

*/

///////////////////// custom menu



	custom_hbox.dropChildren();

	custom_vbox.dropChildren();

	custom_vbox2.dropChildren();

	custom_vbox3.dropChildren();



	custom_hbox.addChild(&custom_vbox);

	custom_vbox.setVisible(true);



	custom_hbox.addChild(&custom_vbox2);

	custom_vbox2.setVisible(true);



	custom_hbox.addChild(&custom_vbox3);

	custom_vbox3.setVisible(true);



	char temp[3];

	for(i = 0; i < MAX_PLAYERS; i++)

	{

		custom_clientHbox[i].dropChildren();

		custom_clientHbox[i].setBuffering(0);

		custom_clientHbox[i].setVisible(false);

		custom_clientHbox[i].setEnabled(false);

		custom_buttonName[i].setText("");

		custom_buttonName[i].setCallBack(custom_name, (void*)i);

		custom_buttonName[i].setVisible(true);

		custom_buttonHouse[i].setText("Atreides");

		custom_buttonHouse[i].setCallBack(custom_house, (void*)i);

		//custom_buttonHouse[i].setEnabled(false);

		custom_clientHbox[i].addChild(&custom_buttonHouse[i]);

		custom_buttonHouse[i].setVisible(true);

		sprintf(temp, "%d", i + 1);

		custom_buttonTeam[i].setText(temp);

		custom_buttonTeam[i].setCallBack(custom_team, (void*)i);

		//custom_buttonTeam[i].setEnabled(false);

		custom_clientHbox[i].addChild(&custom_buttonTeam[i]);

		custom_buttonTeam[i].setVisible(true);

		switch(i)

		{

		case 0:

			custom_buttonColour[i].setText("Blue");

			break;

		case 1:

			custom_buttonColour[i].setText("Green");

			break;

		case 2:

			custom_buttonColour[i].setText("Grey");

			break;

		case 3:

			custom_buttonColour[i].setText("Orange");

			break;

		case 4:

			custom_buttonColour[i].setText("Purple");

			break;

		case 5:

			custom_buttonColour[i].setText("Red");

			break;

		default:

			break;

		}

		custom_buttonColour[i].setCallBack(custom_colour, (void*)i);

		//custom_buttonColour[i].setEnabled(false);

		custom_clientHbox[i].addChild(&custom_buttonColour[i]);

		custom_buttonColour[i].setVisible(true);

	}



	custom_buttonStart.setText("Start");

	custom_buttonStart.setCallBack(custom_start);

	custom_vbox3.addChild(&custom_buttonStart);

	custom_buttonStart.setVisible(true);



	custom_buttonCancel.setText("Cancel");

	custom_buttonCancel.setCallBack(custom_cancel);

	custom_vbox3.addChild(&custom_buttonCancel);

	custom_buttonCancel.setVisible(true);



	custom_hbox.setVisible(true);



///////////////////// multi player menu

	multi_hbox.dropChildren();

	multi_vbox.dropChildren();



	multi_hbox.addChild(&multi_vbox);

	multi_vbox.setVisible(true);



	multi_hbox.addChild(&multi_vbox2);

	multi_vbox2.setVisible(true);



	multi_address.setText("192.168.1.246");

	multi_vbox.addChild(&multi_address);

	multi_address.setVisible(true);



	multi_buttonCreate.setText("Create Game");

	multi_buttonCreate.setCallBack(multi_create);

	multi_vbox2.addChild(&multi_buttonCreate);

	multi_buttonCreate.setVisible(true);



	multi_buttonJoin.setText("Join Game");

	multi_buttonJoin.setCallBack(multi_join);

	multi_vbox2.addChild(&multi_buttonJoin);

	multi_buttonJoin.setVisible(true);



	multi_buttonCancel.setText("Cancel");

	multi_buttonCancel.setCallBack(multi_cancel);

	multi_vbox2.addChild(&multi_buttonCancel);

	multi_buttonCancel.setVisible(true);



	multi_hbox.setVisible(true);



///////////////////////////

	//multiServer_vbox.setBuffering(0);

	multiServer_hbox.addChild(&multiServer_vbox);

	multiServer_vbox.setVisible(true);



	multiServer_hbox.addChild(&multiServer_vbox2);

	multiServer_vbox2.setVisible(true);



	multiServer_vbox3.dropChildren();

	multiServer_hbox.addChild(&multiServer_vbox3);

	multiServer_vbox3.setVisible(true);



	for(i = 0; i < MAX_PLAYERS; i++)

	{

		multiServer_clientHbox[i].dropChildren();

		multiServer_clientHbox[i].setBuffering(0);

		multiServer_clientHbox[i].setVisible(false);

		multiServer_clientHbox[i].setEnabled(false);

		multiServer_buttonName[i].setText("");

		multiServer_buttonName[i].setCallBack(multiServer_name, (void*)i);

		multiServer_buttonName[i].setVisible(true);

		multiServer_buttonHouse[i].setText("Atreides");

		multiServer_buttonHouse[i].setCallBack(multiServer_house, (void*)i);

		//multiServer_buttonHouse[i].setEnabled(false);

		multiServer_clientHbox[i].addChild(&multiServer_buttonHouse[i]);

		multiServer_buttonHouse[i].setVisible(true);

		sprintf(temp, "%d", i + 1);

		multiServer_buttonTeam[i].setText(temp);

		multiServer_buttonTeam[i].setCallBack(multiServer_team, (void*)i);

		//multiServer_buttonTeam[i].setEnabled(false);

		multiServer_clientHbox[i].addChild(&multiServer_buttonTeam[i]);

		multiServer_buttonTeam[i].setVisible(true);

		switch(i)

		{

		case 0:

			multiServer_buttonColour[i].setText("Blue");

			break;

		case 1:

			multiServer_buttonColour[i].setText("Green");

			break;

		case 2:

			multiServer_buttonColour[i].setText("Grey");

			break;

		case 3:

			multiServer_buttonColour[i].setText("Orange");

			break;

		case 4:

			multiServer_buttonColour[i].setText("Purple");

			break;

		case 5:

			multiServer_buttonColour[i].setText("Red");

			break;

		default:

			break;

		}

		multiServer_buttonColour[i].setCallBack(multiServer_colour, (void*)i);

		//multiServer_buttonColour[i].setEnabled(false);

		multiServer_clientHbox[i].addChild(&multiServer_buttonColour[i]);

		multiServer_buttonColour[i].setVisible(true);

	}

	

	multiServer_buttonStart.setText("Start Game");

	multiServer_buttonStart.setCallBack(multiServer_start);

	multiServer_vbox3.addChild(&multiServer_buttonStart);

	multiServer_buttonStart.setVisible(true);



	multiServer_buttonKick.setText("Kick Player");

	multiServer_buttonKick.setCallBack(multiServer_kick);

	multiServer_vbox3.addChild(&multiServer_buttonKick);

	multiServer_buttonKick.setVisible(true);



	multiServer_buttonCancel.setText("Cancel");

	multiServer_buttonCancel.setCallBack(multiServer_cancel);

	multiServer_vbox3.addChild(&multiServer_buttonCancel);

	multiServer_buttonCancel.setVisible(true);



	multiServer_hbox.setVisible(true);



///////////////////////////

	multiClient_hbox.addChild(&multiClient_vbox);

	multiClient_vbox.setEnabled(false);

	multiClient_vbox.setVisible(true);



	multiClient_hbox.addChild(&multiClient_vbox2);

	multiClient_vbox2.setVisible(true);



	multiClient_vbox3.dropChildren();

	multiClient_hbox.addChild(&multiClient_vbox3);

	multiClient_vbox3.setVisible(true);



	for(i = 0; i < MAX_PLAYERS; i++)

	{

		multiClient_clientHbox[i].dropChildren();

		multiClient_clientHbox[i].setBuffering(0);

		multiClient_clientHbox[i].setVisible(false);

		multiClient_clientHbox[i].setEnabled(false);

		multiClient_buttonName[i].setText("");

		multiClient_buttonName[i].setVisible(true);

		multiClient_buttonHouse[i].setText("Atreides");

		multiClient_buttonHouse[i].setCallBack(multiClient_house, (void*)i);

		//multiClient_buttonHouse[i].setEnabled(false);

		multiClient_clientHbox[i].addChild(&multiClient_buttonHouse[i]);

		multiClient_buttonHouse[i].setVisible(true);

		sprintf(temp, "%d", i + 1);

		multiClient_buttonTeam[i].setText(temp);

		multiClient_buttonTeam[i].setCallBack(multiClient_team, (void*)i);

		//multiClient_buttonTeam[i].setEnabled(false);

		multiClient_clientHbox[i].addChild(&multiClient_buttonTeam[i]);

		multiClient_buttonTeam[i].setVisible(true);

		switch(i)

		{

		case 0:

			multiClient_buttonColour[i].setText("Blue");

			break;

		case 1:

			multiClient_buttonColour[i].setText("Green");

			break;

		case 2:

			multiClient_buttonColour[i].setText("Grey");

			break;

		case 3:

			multiClient_buttonColour[i].setText("Orange");

			break;

		case 4:

			multiClient_buttonColour[i].setText("Purple");

			break;

		case 5:

			multiClient_buttonColour[i].setText("Red");

			break;

		default:

			break;

		}

		multiClient_buttonColour[i].setCallBack(multiClient_colour, (void*)i);

		//multiClient_buttonColour[i].setEnabled(false);

		multiClient_clientHbox[i].addChild(&multiClient_buttonColour[i]);

		multiClient_buttonColour[i].setVisible(true);

	}



	multiClient_buttonMessage.setText("Send Message");

	multiClient_buttonMessage.setCallBack(multiClient_message);

	multiClient_vbox3.addChild(&multiClient_buttonMessage);

	multiClient_buttonMessage.setVisible(true);



	multiClient_buttonCancel.setText("Cancel");

	multiClient_buttonCancel.setCallBack(multiClient_cancel);

	multiClient_vbox3.addChild(&multiClient_buttonCancel);

	multiClient_buttonCancel.setVisible(true);



	multiClient_hbox.setVisible(true);

	

////////////// map editor menu



////////////// options menu

//	options_vbox.dropChildren();

	options_vbox.addChild(&options_name);

	options_name.setVisible(true);



	options_buttonConcrete.setCallBack(options_concrete);

	options_vbox.addChild(&options_buttonConcrete);

	options_buttonConcrete.setVisible(true);



	options_buttonRes.setCallBack(options_res);

	options_vbox.addChild(&options_buttonRes);

	options_buttonRes.setVisible(true);



	options_buttonFullScreen.setCallBack(options_fullScreen);

	options_vbox.addChild(&options_buttonFullScreen);

	options_buttonFullScreen.setVisible(true);



	options_buttonDoubleBuffered.setCallBack(options_doubleBuffered);

	options_vbox.addChild(&options_buttonDoubleBuffered);

	options_buttonDoubleBuffered.setVisible(true);



	options_vbox.addChild(&options_hbox);

	options_hbox.setVisible(true);



	options_buttonCancel.setText("Cancel");

	options_buttonCancel.setCallBack(options_cancel);

	options_hbox.addChild(&options_buttonCancel);

	options_buttonCancel.setVisible(true);



	options_buttonOk.setText("Ok");

	options_buttonOk.setCallBack(options_ok);

	options_hbox.addChild(&options_buttonOk);

	options_buttonOk.setVisible(true);



	options_vbox.setVisible(true);



////////////// about menu

	about_window.setVisible(true);



/////////////////////////////////////////////

}



int mainLoop()

{

	bool key_alt = false;

	SDL_Event		event;

//	SDL_Surface		*menu,

//					*menuText;

	int i;



	socketSet = SDLNet_AllocSocketSet(MAX_PLAYERS);

	cursorFrame = CURSOR_NORMAL;



	quiting = false;

	SDL_Rect	dest;



	SDL_Rect drawLocation, source;

	char creditsBuffer[3];

	/////////////do menu screen
	while (!quiting)
	{
		SDL_FillRect(screen, NULL, COLOUR_BLACK);
		//blit the menu image to the screen
		menu = (SDL_Surface*)(dataFile[UI_Menu].dat);
		
		//dest.x = backgroundOffsetX;
		//dest.y = backgroundOffsetX;
		dest.w = menu->w;
		dest.h = menu->h;


		if(screen->h > dest.h)

			dest.y = (screen->h - dest.h)/2;

		else

			dest.y = 0;



		if(screen->w > dest.w)

			dest.x = (screen->w - dest.w)/2;

		else

			dest.x = 0;

		SDL_BlitSurface(menu, NULL, screen, &dest);

		SDL_LockMutex(mutex_currentWidget);

		if (currentWidget == mainWindow)

		{

			//dest.x = backgroundOffsetX;
			//dest.y = backgroundOffsetX;
			dest.y = screen->h - menuText->h - 17;
			dest.x = screen->w - menuText->w - 25;
			dest.w = menuText->w;
			dest.h = menuText->h;

			SDL_BlitSurface(menuText, NULL, screen, &dest);
		}
		currentWidget->draw();	//draw the current menu

		if(currentWidget == skirmishWindow)
		{
					//draw credits

			sprintf(creditsBuffer, "%d", selectedMission);
			int digits = strlen(creditsBuffer);
			SDL_Surface* surface = (SDL_Surface*)dataFile[UI_CreditsDigits].dat;
			source.w = drawLocation.w = surface->w/10;
			source.h = drawLocation.h = surface->h;
			source.y = 0;
			drawLocation.y = menu_mission.getY() + 17;
			for (int i=digits-1; i>=0; i--)
			{
				source.x = atoi(&creditsBuffer[i])*source.w;
				creditsBuffer[i] = '\0';	//so next round atoi only does one character
				drawLocation.x = menu_mission.getX() + 40 + (6 - digits + i)*10;
				SDL_BlitSurface(surface, &source, screen, &drawLocation);

			}
		}

		SDL_UnlockMutex(mutex_currentWidget);
		drawCursor();

		if (multiGameStarting)	//for client who received start game message
			multiClient_start();

		while(SDL_PollEvent(&event))	//check the events
		{
			switch (event.type)
			{

				case (SDL_KEYDOWN):	// Look for a keypress

				{
					if (focusedWidget)
					{
						if (focusedWidget->isAnEntry())
							((Entry*)focusedWidget)->handleKeyPress(&event.key);
					}
					else
					{
						switch(event.key.keysym.sym)
						{
							case SDLK_LALT: case SDLK_RALT:
								key_alt = true;
								break;
							case SDLK_ESCAPE:

								if (currentWidget != mainWindow)

									cancel();

								else
									quit();
								break;

							case SDLK_RETURN:

								if (key_alt)

								{

									SDL_WM_ToggleFullScreen(screen);

									printf("switch\n");

								}

								break;

							case SDLK_PRINT: case SDLK_SYSREQ:

								SDL_SaveBMP(screen, "screenshot.bmp");

								printf("screenshot taken\n");

								break;

							default:
								break;
						}
						break;

					}

				}

				case (SDL_KEYUP):	// Look for a keyrelease

				{

					switch(event.key.keysym.sym)
					{
						case SDLK_LALT: case SDLK_RALT:
							cursorFrame = CURSOR_NORMAL;
							key_alt = false;
							break;

						default:

							break;

					}

				}

				case SDL_MOUSEBUTTONDOWN:

				{

					SDL_MouseButtonEvent* mouse = &event.button;

					SDL_LockMutex(mutex_button);

					if (!multi_buttonHit)

					{

						if ((mouse->button == SDL_BUTTON_LEFT) || (mouse->button == 5))

							currentWidget->handlePress(mouse->x, mouse->y);

						else if ((mouse->button == SDL_BUTTON_RIGHT) || (mouse->button == 4))

							currentWidget->handleCancelPress(mouse->x, mouse->y);

					}

					SDL_UnlockMutex(mutex_button);

					break;

				}

				case SDL_MOUSEMOTION:

				{

					SDL_MouseMotionEvent* mouse = &event.motion;



					drawnMouseX = mouse->x;

					drawnMouseY = mouse->y;

					//if (focusedWidget)

						currentWidget->handleMotion(mouse->x, mouse->y);

					break;

				}

				case SDL_MOUSEBUTTONUP:

				{

					SDL_MouseButtonEvent* mouse = &event.button;

					if (!multi_buttonHit)

					{

						if ((mouse->button == SDL_BUTTON_LEFT) || (mouse->button == 5))

							currentWidget->handleRelease(mouse->x, mouse->y);

						else if ((mouse->button == SDL_BUTTON_RIGHT) || (mouse->button == 4))

							currentWidget->handleCancelRelease(mouse->x, mouse->y);

					}

					break;

				}

				case (SDL_QUIT):
					quit();
					break;
				default:
					break;
    		}
		}

		SDL_Flip(screen);
	}

	delete soundPlayer;
	Mix_CloseAudio();

	SDLNet_FreeSocketSet(socketSet);

	SDLNet_Quit();

	SDL_FreeSurface(menuText);

	for (i = 1; i < MAX_FONT_SIZE; i++)

		TTF_CloseFont(font[i]);

	TTF_Quit();

	destroyDataFile();

	SDL_Quit();



	FILE *config;

	config = fopen("dunelegacy.cfg", "wb");

	if (config)

	{

		if (settings.concreteRequired)

			fprintf(config, "%s\n", "Concrete Required");

		else

			fprintf(config, "%s\n", "Concrete Not Required");

		fprintf(config, "%d,%d\n", settings.width, settings.height);

		fprintf(config, "%s\n", settings.screen);

		if (settings.doubleBuffered)

			fprintf(config, "%s\n", "Double Buffered");

		else

			fprintf(config, "%s\n", "Single Buffered");

		fprintf(config, "%s\n", settings.localPlayerName);

		fprintf(config, "%s\n", settings.lastAddress);



		fclose(config);

	}

	else

		fprintf(stderr, "ERROR: couldn't save to config file.\n");



	return 0;
}

void init_loadWindow()
{
	int i;
	SDL_Surface *surf = (SDL_Surface *)dataFile[UI_LoadSaveWindow].dat;



	SDL_Surface *cancelSurf = ((SDL_Surface*)dataFile[UI_Cancel].dat);
	SDL_Surface *cancelSurfPressed = ((SDL_Surface*)dataFile[UI_Cancel_Pressed].dat);

	SDL_Surface *saveSurf = ((SDL_Surface*)dataFile[UI_Save].dat);
	SDL_Surface *saveSurfPressed = ((SDL_Surface*)dataFile[UI_Save_Pressed].dat);

	SDL_Surface *loadSurf = ((SDL_Surface*)dataFile[UI_Load].dat);
	SDL_Surface *loadSurfPressed = ((SDL_Surface*)dataFile[UI_Load_Pressed].dat);

	







	//------------------



	loadGameWindow.dropChildren();

	loadGameWindow.setX((screen->w - surf->w)/2);

	loadGameWindow.setY((screen->h - surf->h)/2);

//	loadGameWindow.setWidth(screen->w/2);

//	loadGameWindow.setHeight(screen->h/2);

	loadGameWindow.setSurface(surf, NULL);

	loadGameWindow.setWidth(surf->w);

	loadGameWindow.setHeight(surf->h);

	loadGameWindow.setVisible(true);



	loadCancelButton.setSurface(cancelSurf, NULL);
	loadCancelButton.setPressedSurface(cancelSurfPressed, NULL);
	loadCancelButton.setHeight(cancelSurf->h);
	loadCancelButton.setWidth(cancelSurfPressed->w);
	loadCancelButton.setX(loadGameWindow.getX() + 87);
	loadCancelButton.setY(loadGameWindow.getY() + 187);
	loadCancelButton.setCallBack(cancel);
	loadCancelButton.setVisible(true);



	int startX = loadGameWindow.getX() + 6;

	int startY = loadGameWindow.getY() + 42;



	SDL_Surface *slotSurf = (SDL_Surface *)dataFile[UI_GameSlot].dat;



	for(i = 0; i < NUM_SAVEGAMES; i++)

	{

		gameLoadSpot[i].setCallBack(single_loadGameEntry, (void*)i);

		gameLoadSpot[i].setX(startX);

		gameLoadSpot[i].setY(startY);

		gameLoadSpot[i].setSurface(slotSurf, NULL);

		gameLoadSpot[i].setHeight(slotSurf->h);

		gameLoadSpot[i].setWidth(slotSurf->w);

		gameLoadSpot[i].setVisible(true);

		gameLoadSpot[i].setTransparent(true);



		loadGameWindow.addChild(&gameLoadSpot[i]);

		startY += slotSurf->h + 2;

	}



/*	loadButton.setSurface(loadSurf, NULL);
	loadButton.setPressedSurface(loadSurfPressed, NULL);
	loadButton.setWidth(loadSurf->w);
	loadButton.setHeight(loadSurf->h);
	loadButton.setVisible(true);
	loadButton.setX(loadWindow.getX() + 37);
	loadButton.setY(loadWindow.getY() + 181);
//	loadButton.setCallBack(game_loadEntry, (void*)0);


//	loadButton.setEnabled(false);	//don't need it

*/

	loadGameWindow.addChild(&loadCancelButton);


}


int main(int argc, char *argv[])
{
	bool	key_alt = false;
    int     i,

			seed = time(NULL);

	FILE	*config;

//	SDL_Event		event;



	inMainMenu = true;

	lookDist[0] = 10;lookDist[1] = 10;lookDist[2] = 9;lookDist[3] = 9;lookDist[4] = 9;lookDist[5] = 8;lookDist[6] = 8;lookDist[7] = 7;lookDist[8] = 6;lookDist[9] = 4;lookDist[10] = 1;

	settings.concreteRequired = true;
	settings.gameType = ORIGINAL;
	settings.playerType = SINGLE;
	
	settings.gamespeed = 10;
	
	settings.playerNum = HOUSE_ATREIDES;

	settings.playerHouse[settings.playerNum] = settings.playerNum;

	strcpy(settings.localPlayerName, "Player");

	settings.serverIp.port = 53301;

	settings.numPlayers = 2;
	settings.width = 640;
	settings.height = 480;
	strcpy(settings.screen, "Windowed");
	srand(seed);

	config = fopen("dunelegacy.cfg", "rb");

	if (config)

	{

		char	temp[MAX_LINE],

				*value;



		//get concrete option

		fgets(temp, MAX_LINE, config);

		temp[strlen(temp)-1] = '\0';	//remove newline

		if (strcmp(temp, "Concrete Required") == 0)

			settings.concreteRequired = true;

		else

			settings.concreteRequired = false;



		fgets(temp, MAX_LINE, config);

		value = strtok(temp, ",\n\r");

		if (strcmp(value, "320") == 0)

		{

			settings.width = 320;

			settings.height = 240;

		}

		else if (strcmp(value, "640") == 0) 

		{

			settings.width = 640;

			settings.height = 480;

		}

		else if (strcmp(value, "800") == 0)

		{

			settings.width = 800;

			settings.height = 600;

		}

		else if (strcmp(value, "1024") == 0)

		{

			settings.width = 1024;

			settings.height = 768;

		}



		fgets(temp, MAX_LINE, config);

		temp[strlen(temp)-1] = '\0';	//remove newline

		if (strcmp(value, "Full Screen") == 0)

			strcpy(settings.screen, "Full Screen");

		else if (strcmp(value, "Windowed") == 0) 

			strcpy(settings.screen, "Windowed");



		fgets(temp, MAX_LINE, config);

		temp[strlen(temp)-1] = '\0';	//remove newline

		settings.doubleBuffered = (strcmp(value, "Double Buffered") == 0);



		fgets(settings.localPlayerName, MAX_NAMELENGTH, config);

		settings.localPlayerName[strlen(settings.localPlayerName)-1] = '\0';	//remove newline



		fgets(settings.lastAddress, MAX_LINE, config);		

		settings.lastAddress[strlen(settings.lastAddress)-1] = '\0';	//remove newline



		fclose(config);

	}

	else

		fprintf(stderr, "ERROR: couldn't read config file.\n");

	if (argc > 1)	//check for overiding params
	{
		if (strcmp(argv[1], "-f") == 0)
			strcpy(settings.screen, "Full Screen");
		else if (strcmp(argv[1], "-w") == 0)
			strcpy(settings.screen, "Windowed");
	}

	if (SDL_Init(SDL_INIT_AUDIO | SDL_INIT_TIMER | SDL_INIT_VIDEO | SDL_INIT_NOPARACHUTE) < 0)
	{
		fprintf(stderr, "ERROR: Couldn't initialise SDL: %s\n", SDL_GetError());
		exit(1);
	}
	SDL_EnableUNICODE(1);

	SDL_WM_SetCaption("Dune Legacy", "Dune Legacy");
	
	fprintf(stdout, "initialising sound.....\n");fflush(stdout);
	if ( Mix_OpenAudio(/*MIX_DEFAULT_FREQUENCY*/11025, MIX_DEFAULT_FORMAT/*AUDIO_U16*/, 2, 512) < 0 )

	{

		SDL_Quit();
		fprintf(stderr,"Warning: Couldn't set 11025 Hz 16-bit audio\n- Reason: %s\n",SDL_GetError());

		exit(1);

	}

	else

		fprintf(stdout, "allocated %d channels.\n", Mix_AllocateChannels(16)); fflush(stdout);

	/////////start up networking/////////
	if (SDLNet_Init() < 0)
	{
		fprintf(stderr, "ERROR: Couldn't initialise networking: %s\n", SDLNet_GetError());

		Mix_CloseAudio();

		SDL_Quit();
		exit(1);
	}



	mutex_button = SDL_CreateMutex();

	mutex_currentWidget = SDL_CreateMutex();

	mutex_playersJoined = SDL_CreateMutex();



	for(i=0;i<MAX_PLAYERS;i++)

		sockets[i] = NULL;


	if (TTF_Init() < 0)
	{
		fprintf(stderr, "ERROR: Couldn't initialise font library: %s\n", SDL_GetError());

		SDL_Quit();

		Mix_CloseAudio();

		SDLNet_Quit();
		exit(1);
	}
	
	////////load the datafile
	fprintf(stdout, "loading data.....\n");fflush(stdout);
	loadDataFile();
	fprintf(stdout, "loading fonts.....\n");fflush(stdout);

	for(i = 1; i < MAX_FONT_SIZE; i++)
	{
		font[i] = 0;
		config = fopen("data/font.ttf", "r");	//must check for existing file because of bug
		if (config)
		{
			fclose(config);
			font[i] = TTF_OpenFont("data/font.ttf", i);
		}
		if (font[i] <= 0)
		{
			config = fopen("data/font.fon", "r");
			if (config)
			{
				fclose(config);
				font[i] = TTF_OpenFont("data/font.fon", i);
			}
		}
		if (font[i] <= 0)
		{
			fprintf(stderr, "ERROR: unable to load a font file: data/font.ttf or data/font.fon\n");
			TTF_Init();
			SDLNet_Quit();
			Mix_CloseAudio();
			SDL_Quit();
			exit(1);
		}

	}

	fprintf(stdout, "starting sound.....\n");fflush(stdout);
	soundPlayer = new SoundPlayerClass();
	fprintf(stdout, "starting main menu.......\n");fflush(stdout);
	menu = (SDL_Surface*)(dataFile[UI_Menu].dat);
	palette = new SDL_Palette;
	palette->ncolors = menu->format->palette->ncolors;
	palette->colors = new SDL_Color[palette->ncolors];
	memcpy(palette->colors, menu->format->palette->colors, sizeof(SDL_Color)*palette->ncolors);


	//get the house palettes
	houseColour[HOUSE_ATREIDES] = COLOUR_ATREIDES;

	houseColour[HOUSE_ORDOS] = COLOUR_ORDOS;

	houseColour[HOUSE_HARKONNEN] = COLOUR_HARKONNEN;

	houseColour[HOUSE_SARDAUKAR] = COLOUR_SARDAUKAR;

	houseColour[HOUSE_FREMEN] = COLOUR_FREMEN;

	houseColour[HOUSE_MERCENARY] = COLOUR_MERCENARY;



	//load mentat

	fprintf(stdout, "loading mentat.....\n");fflush(stdout);
	MentatClass *mentat = new MentatClass();
	setMentat(mentat);



	screen = NULL;
	setVideoMode();



/*play intro scene*/

	DuneCutScene *introScene = new DuneCutScene();

//	introScene->parseScenesFile("data/intro.scene");

//	introScene->parseScenesFile("data/atrefinale.scene");

//	introScene->parseScenesFile("data/ordosfinale.scene");

//	introScene->parseScenesFile("data/harkfinale.scene");

//	introScene->parseScenesFile("data/credits.scene");

	introScene->playCutScene();

	

	delete introScene;

/*end intro scene*/





	//i suspect the palette on one of the images is corrupt...so we have to redefine white

	palette->colors[COLOUR_WHITE].r = 255;

	palette->colors[COLOUR_WHITE].g = 255;

	palette->colors[COLOUR_WHITE].b = 255;


	char versionString[100];
	sprintf(versionString, "%s", VERSION);
	menuText = TTF_RenderText_Solid(font[16], versionString, palette->colors[COLOUR_BLACK]);



	init_loadWindow();

	realign_buttons();



	return mainLoop();

}

