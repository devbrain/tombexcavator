/*					    
 *  Anthony Cole
 *
 */
#include "sand.h"

#include "Items.h"
#include "MentatClass.h"
#include "CommViewClass.h"
//#include "gui/gui.h"



#define TOPBAR_MASKED_AREA_HEIGHT	5
#define END_WAIT_TIMER		300


bool debug = false;
int debugCount;
int frames = 0;
int unitsCreated, structuresCreated;

PlayerClass**	player;
PlayerClass*	thisPlayer;

SoundPlayerClass* soundPlayer;

char	messageBuffer[MESSAGE_BUFFER_SIZE][MAX_LINE],
		typingMessage[MAX_MSGLENGTH - 3];
int		messageTimer[MESSAGE_BUFFER_SIZE];


MapClass		*map;
ListIterator	*selectedLists,
				*unitList,

				*selectedList,
				*structureList,
				*bulletList,

				

				*airUnitBlitList,

				*groundUnitBlitList,

				*infantryBlitList,

				*selectionBlitList,

				*undergroundUnitBlitList;


SDL_Palette		*palette;
TTF_Font		*font[MAX_FONT_SIZE];
BORDERTYPE		*dborder;

//fog surface
SDL_Surface* fogSurf;

//hidden surface
SDL_Surface* hiddenSurf;

/*fog pixelmap for alphablending 
neighbours of fogged terrain*/
Uint32 fogMAP[256][16];

/*new: commView*/
CommViewClass *commView = 0;


int houseColour[NUM_HOUSES];
 
bool	shade = true,
		fog_wanted = false;

DATAFILE* dataFile;
int lookDist[11];
int	Music_Last = Music_First - 1,
	Music_AttackFirst = Music_First,

	Music_AttackLast = Music_Last,

	Music_IntroFirst = Music_First,

	Music_IntroLast = Music_Last,

	Music_LoseFirst = Music_First,

	Music_LoseLast = Music_Last,

	Music_PeaceFirst = Music_First,

	Music_PeaceLast = Music_Last,

	Music_WinFirst = Music_First,

	Music_WinLast = Music_Last;

bool	attackMoveMode = false,
		builderSelectMode = false,
		fullscreenMode = false,
		groupCreateMode = false,
		menuMode = false,
		messageMode = false,
		moveDownMode = false,
		moveLeftMode = false,
		moveRightMode = false,
		moveUpMode = false,
		placingMode = false,
		shift = false,

		showFPS = false,

		radarMode = false,
		selectionMode = false,

		quitGame = false;



CURSORTYPE cursorFrame;

int drawnMouseX, drawnMouseY;



int	radarSizeX,
	radarSizeY;
int radarOffsetX,
	radarOffsetY;

int indicatorFrame = NONE,

	indicatorTime = 5,

	indicatorTimer,

	indicatorX = NONE,

	indicatorY = NONE;

int nextObjectID;

double fps;
int frameTimer; 
//int gameSpeed now in settings;

SDL_Rect	creditsPos,
			gameBarPos,
			powerIndicatorPos,
			selectionRect,
			spiceIndicatorPos;
SDL_Rect	topBarPos;

GAMESTATETYPE gameState;
PLAYERTYPE thisPlayerType;

SETTINGSTYPE settings;

//game interfaces
int		saveGameSpot;

Button	unitIcon;
Button	unitButton_Aggressive,
		unitButton_Defensive,
		unitButton_StandGround,
		unitButton_Scout,
		unitButton_Deploy,
		unitButton_Destruct,

		mentatButton,
		optionsButton,
		
		gameLoadSpotButton[NUM_SAVEGAMES],
		gameSaveSpotButton[NUM_SAVEGAMES],
		
		saveButton;

Entry	gameSaveNameEntry;

VBox	palaceInterface,
		unitInterface;

VBox	menu_vBox;
HBox save_hBox;

Button resumeButton;

Button gameSettingsButton;

Button restartGameButton;

Button saveGameButton;

Button loadGameButton;

Button quitButton;

VBox save_vBox;

Button gameCancelButton;

VBox load_vBox;

HBox load_hBox;

Button loadButton;

HBox	mainButtonsBox;
		
MessageBox messageBox;

Window	*menuInterface,
		menuWindow,
		loadWindow,
		saveWindow,
		settingsWindow;

Button	settingsOkButton, settingsCancelButton, 
		settingsGameSpeedPlus, settingsGameSpeedMinus,
		settingsVolumePlus, settingsVolumeMinus,
		settingsVolumeBar, settingsGameSpeedBar;

SDL_Surface *settingsOKSurf, 
			*settingsOKSurfPressed,
			*settingsCancelSurf,
			*settingsCancelSurfPressed,
			*settingsWindowSurf,
			*settingsPlus, *settingsPlusPressed,
			*settingsMinus, *settingsMinusPressed;

MentatClass *mentat = NULL;

SDL_Surface *gamebarSurface;
SDL_Surface *topBarSurface,
			*mentatSurface, *mentatSurfacePressed,
			*optionsSurface, *optionsSurfacePressed,
			*MessageBoxSurface;
			
		
SDL_Surface *aggressiveSurf, *scoutSurf, *standGroundSurf, *defensiveSurf,
			*deploySurf, *destructSurf;
SDL_Surface *aggressiveSurfActive, *scoutSurfActive, *standGroundSurfActive, *defensiveSurfActive,
			*deploySurfPressed, *destructSurfPressed;

SDL_Surface *gameMenuSurf, *resumeGameSurf, *saveGameSurf, *loadGameSurf,
			*gameSettingsSurf, *quitSurf, *restartSurf;

SDL_Surface *resumeGameSurfPressed, *saveGameSurfPressed, *loadGameSurfPressed,
			*gameSettingsSurfPressed, *quitSurfPressed, *restartSurfPressed;

SDL_Surface *saveSurf, *saveSurfPressed, *loadSurf, *loadSurfPressed, *cancelSurf, *cancelSurfPressed;

int	endTimer;
bool finishedLevel;

bool showMentat = false;

bool loadingGame;
int gameNum;
int prevHouseCol = COLOUR_HARKONNEN;

void game_mentat(void *var);
void game_options(void *var);
void init_save_load_windows();

int new_vol;
int new_gamespeed;

//a bit of a hack :/
int gameSpeedSelect = 1;
int volumeSelect = 2;
int scrollSelect = 3;

void game_settings()
{
	int width;
	int segment;
	//setup progress bars
	new_gamespeed = settings.gamespeed;
	new_vol = 100;

	segment = 156/(GAMESPEED_MAX - GAMESPEED_MIN);
	width = 156 - ((new_gamespeed - GAMESPEED_MIN) * segment);
	settingsGameSpeedBar.setWidth(width);

	segment = 156/(VOLUME_MAX - VOLUME_MIN);
	width = settings.gamespeed * segment;
	settingsVolumeBar.setWidth(width);

	menuInterface = &settingsWindow;
}

void settings_ok()
{
	//commit changes

	settings.gamespeed = new_gamespeed;

	menuInterface = &menuWindow;

}

void settings_cancel()
{
	menuInterface = &menuWindow;
}

void settings_plus(int val)
{
	int i = (int)val;
	int width;
	int segment;

	switch(i)
	{

		case 1:
			if(new_gamespeed > GAMESPEED_MIN)
				new_gamespeed -= 1;

			segment = 156/(GAMESPEED_MAX - GAMESPEED_MIN);
			width = 156 - ((new_gamespeed - GAMESPEED_MIN) * segment);
			settingsGameSpeedBar.setWidth(width);
			break;
		case 2:
			

			break;

	}
}

void settings_minus(int val)
{
	int i = (int)val;
	int width;
	int segment;

	switch(i)
	{

		case 1:
			if(new_gamespeed <= GAMESPEED_MAX)
				new_gamespeed += 1;

			segment = 156/(GAMESPEED_MAX - GAMESPEED_MIN);
			width = 156 - ((new_gamespeed - GAMESPEED_MIN) * segment);
			settingsGameSpeedBar.setWidth(width);
			break;
		case 2:
			break;

	}

}

void settings_plus_gamespeed()
{
	settings_plus(1);
}

void settings_minus_gamespeed()
{
	settings_minus(1);
}

void settings_plus_volume()
{
	settings_plus(2);
}

void settings_minus_volume()
{
	settings_minus(2);
}

void doClient()	//check for info from server
{
	char		msg[MAX_MSGLENGTH];
	int			numReady,
				numConverted,
				objectID, playerID, itemID, destObjectID,
				lx, ly, dx, dy, angle, turretAngle, forced;
	ObjectClass	*destObject, *object;

	while ((numReady = SDLNet_CheckSockets(socketSet, 0)) > 0)
	{
		if(SDLNet_SocketReady(server))	//server saying something

		{

			if (SDLNet_TCP_Recv(server, msg, MAX_MSGLENGTH)==MAX_MSGLENGTH)

			{

				numReady--;

				msg[MAX_MSGLENGTH-1] = '\0';

				//showMessage(msg);

				switch (msg[0])

				{

					case MSG_ATTACK:

						numConverted = sscanf(&msg[1], "%d,%d,%d,%d,%d,%d", &objectID, &destObjectID, &lx, &ly, &dx, &dy);

						if ((numConverted == 6) && map->cellExists(lx, ly) && map->cellExists(dx, dy) && (objectID != NONE) && (destObjectID != NONE))

						{

							destObject = NULL;

							object = map->findObjectWidthID(objectID, lx, ly);

							if (object)

							{

								COORDTYPE temp = {lx, ly};

								if (object->isAUnit())

								{

									if ((!((UnitClass*)object)->isMoving()

										&& (object->getLocation()->x == object->getDestination()->x)

										&& (object->getLocation()->y == object->getDestination()->y)) 

										|| (blockDistance(object->getLocation(), &temp) > 5))

									{

										//make sure unit is not held by something already

										if (object->hasATarget())	//if so eject them first

										{

											ObjectClass* target = object->getTarget();

											switch(target->getItemID())

											{

											case Structure_Refinery:

												if (!((RefineryClass*)target)->isFree() && (((RefineryClass*)target)->getHarvester() == object))

													((RefineryClass*)target)->deployHarvester();

												break;

											case Structure_RepairYard:

												if (!((RepairYardClass*)target)->isFree() && (((RepairYardClass*)target)->getRepairUnit() == object))

													((RepairYardClass*)target)->deployRepairUnit();

												break;

											case Unit_Carryall:

												((Carryall*)target)->deployUnit((UnitClass*)object);

												break;

											case Unit_Frigate:

												//((Unit_Frigate*)target)->deployUnit((UnitClass*)object);

												break;

											default:

												break;

											}

										}



										map->removeObjectFromMap(object);

										object->setLocation(lx, ly);

										((UnitClass*)object)->resetStartPoint();



									}

									destObject = map->findObjectWidthID(destObjectID, dx, dy);

									object->setTarget(destObject);

									object->setDestination(destObject->getLocation());

									if (object->isAUnit())

									{

										((UnitClass*)object)->clearPath();

										((UnitClass*)object)->clearNextSpotFound();

									}

								}

							}

							else

								showMessage("ERROR: MSG_ATTACK - invalid message received from server.");

						}

						else

						{

							showMessage("ERROR: MSG_ATTACK - invalid message received from server: message incorrectly formatted");

							showMessage(msg);

							fprintf(stderr, "ERROR: MSG_ATTACK - invalid message received from server: message incorrectly formatted");

							fprintf(stderr, msg);

						}

						break;

					case MSG_BUILDCOMMAND:

						numConverted = sscanf(&msg[1], "%d,%d,%d,%d,%d", &objectID, &lx, &ly, &dx, &dy);

						if ((numConverted == 5) && map->cellExists(lx, ly) && (objectID != NONE))

						{

							object = map->findObjectWidthID(objectID, lx, ly);

							if (object && object->isAStructure() && ((StructureClass*)object)->isABuilder())

								((BuilderClass*)object)->buildNum(dx ? true : false, dy);

							else

								showMessage("ERROR: MSG_BUILDCOMMAND - invalid message received from server: wrong object type");

						}

						else

							showMessage("ERROR: MSG_BUILDCOMMAND - invalid message received from server: message incorrectly formatted");

						break;

					case MSG_BUILDCOMPLETE:

						numConverted = sscanf(&msg[1], "%d,%d,%d", &objectID, &lx, &ly);

						if ((numConverted == 3) && map->cellExists(lx, ly) && (objectID != NONE))

						{

							object = map->findObjectWidthID(objectID, lx, ly);

							if (object && object->isAStructure() && ((StructureClass*)object)->isABuilder())

								((BuilderClass*)object)->setWaitingToPlace();

							else

								showMessage("ERROR: MSG_BUILDCOMPLETE - invalid message received from server.");

						}

						else

							showMessage("ERROR: MSG_BUILDCOMPLETE - invalid message received from server: message incorrectly formatted");

						break;

					case MSG_CANCELCOMMAND:

						numConverted = sscanf(&msg[1], "%d,%d,%d,%d,%d", &objectID, &lx, &ly, &dx, &dy);

						if ((numConverted == 5) && map->cellExists(lx, ly) && (objectID != NONE))

						{

							object = map->findObjectWidthID(objectID, lx, ly);

							if (object && object->isAStructure() && ((StructureClass*)object)->isABuilder())

								((BuilderClass*)object)->cancelSelection(dx ? true : false, dy);

							else

								showMessage("ERROR: MSG_CANCELCOMMAND - invalid message received from server.");

						}

						else

							showMessage("ERROR: MSG_CANCELCOMMAND - invalid message received from server: message incorrectly formatted");

						break;

					case MSG_CREATEOBJECT:

						numConverted = sscanf(&msg[1], "%d,%d,%d,%d,%d", &playerID, &itemID, &lx, &ly, &angle);

						if ((numConverted == 5) && (playerID >= 0) && (playerID < MAX_PLAYERS))

						{

							UnitClass* unit = (UnitClass*)player[playerID]->createUnit(itemID);

							if (unit)

							{

								if (map->cellExists(lx, ly))

								{

									COORDTYPE pos = {lx, ly};

									unit->deploy(&pos);

								}



								if ((angle >= 0) && (angle < NUM_ANGLES))

									unit->setAngle(angle);

							}

						}

						else

							showMessage("ERROR: MSG_CREATEOBJECT - invalid message received from server: message incorrectly formatted");

						break;

					case MSG_CONSISTANCY:

						numConverted = sscanf(&msg[1], "%d,%d,%d,%d,%d", &objectID, &lx, &ly, &angle, &turretAngle);

						if ((numConverted == 5) && map->cellExists(lx, ly) && (objectID != NONE))

						{

							object = map->findObjectWidthID(objectID, lx, ly);

							if (object)

							{

								COORDTYPE temp = {lx, ly};

								if (object->isAUnit() && (!((UnitClass*)object)->isMoving() || blockDistance(object->getLocation(), &temp) > 5))

								{

									UnitClass* unit = (UnitClass*)object;



									//make sure unit is not held by something already

									if (object->hasATarget())	//if so eject them first

									{

										ObjectClass* target = object->getTarget();

										switch(target->getItemID())

										{

										case Structure_Refinery:

											if (!((RefineryClass*)target)->isFree() && (((RefineryClass*)target)->getHarvester() == object))

												((RefineryClass*)target)->deployHarvester();

											break;

										case Structure_RepairYard:

											if (!((RepairYardClass*)target)->isFree() && (((RepairYardClass*)target)->getRepairUnit() == object))

												((RepairYardClass*)target)->deployRepairUnit();

											break;

										case Unit_Carryall:

											((Carryall*)target)->deployUnit((UnitClass*)object);

											break;

										case Unit_Frigate:

											((Frigate*)target)->deployUnit((UnitClass*)object);

											break;

										default:

											break;

										}

									}



									map->removeObjectFromMap(object);

									object->setLocation(lx, ly);

									unit->setAngle(angle);

									if (unit->isTurreted() && (turretAngle != NULL))

										((TankClass*)unit)->setTurretAngle(turretAngle);

								}

							}

							else

								showMessage("ERROR: MSG_CONSISTANCY - invalid message received from server.");

						}

						else

						{

							showMessage("ERROR: MSG_CONSISTANCY - invalid message received from server: message incorrectly formatted");

							showMessage(msg);

							fprintf(stderr, "ERROR: MSG_CONSISTANCY - invalid message received from server: message incorrectly formatted");

							fprintf(stderr, msg);

						}

						break;

					case MSG_DEVASTATE:

						numConverted = sscanf(&msg[1], "%d,%d,%d", &objectID, &lx, &ly);

						if ((numConverted == 3) && (map->cellExists(lx, ly) || (lx == NONE)) && (objectID != NONE))

						{

							object = map->findObjectWidthID(objectID, lx, ly);

							if (object && (object->getItemID() == Unit_Devastator))

								((DevastatorClass*)object)->startDevastate();

							else

								showMessage("ERROR: MSG_DEVASTATE - invalid message received from server.");

						}

						else

							showMessage("ERROR: MSG_DEVASTATE - invalid message received from server: message incorrectly formatted");

						break;

					case MSG_DEVIATE:

						numConverted = sscanf(&msg[1], "%d,%d,%d,%d", &objectID, &lx, &ly, &playerID);

						if ((numConverted == 4) && (map->cellExists(lx, ly) || (lx == NONE)) && (objectID != NONE) && ((playerID >= 0) && (playerID < MAX_PLAYERS) && (player[playerID] != NULL)))

						{

							object = map->findObjectWidthID(objectID, lx, ly);

							if (object && object->isAUnit())

								((UnitClass*)object)->deviate(player[playerID]);

							else

								showMessage("ERROR: MSG_DEVIATE - invalid message received from server.");

						}

						else

							showMessage("ERROR: MSG_DEVIATE - invalid message received from server: message incorrectly formatted");

						break;

					case MSG_MESSAGE:

					{

						char	inMessage[MAX_MSGLENGTH + MAX_NAMELENGTH + 2];



						playerID = (int)msg[1];

						if (((playerID >= 0) && (playerID < MAX_PLAYERS)) && (player[playerID] != NULL))

						{

							sprintf(inMessage, "%s: %s", settings.playerName[playerID], &msg[2]);

							showMessage(inMessage);

						}

						else

							showMessage("ERROR: MSG_MESSAGE - invalid message received from server: message incorrectly formatted");

						break;

					}

					case MSG_OBJECTCOMMAND:

						numConverted = sscanf(&msg[1], "%d,%d,%d,%d,%d,%d,%d", &objectID, &destObjectID, &lx, &ly, &dx, &dy, &forced);

						if ((numConverted == 7) && map->cellExists(lx, ly) && (objectID != NONE))

						{

							destObject = NULL;

							object = map->findObjectWidthID(objectID, lx, ly);

							if (object)

							{

								COORDTYPE temp = {lx, ly};

								//showMessage("object command received");

								if (object->isAUnit() && (!((UnitClass*)object)->isMoving() || blockDistance(object->getLocation(), &temp) > 2))

								{

									//make sure unit is not held by something already

									if (object->hasATarget())	//if so eject them first

									{

										ObjectClass* target = object->getTarget();

										switch(target->getItemID())

										{

										case Structure_Refinery:

											if (!((RefineryClass*)target)->isFree() && (((RefineryClass*)target)->getHarvester() == object))

												((RefineryClass*)target)->deployHarvester();

											break;

										case Structure_RepairYard:

											if (!((RepairYardClass*)target)->isFree() && (((RepairYardClass*)target)->getRepairUnit() == object))

												((RepairYardClass*)target)->deployRepairUnit();

											break;

										case Unit_Carryall:

											((Carryall*)target)->deployUnit((UnitClass*)object);

											break;

										case Unit_Frigate:

											((Frigate*)target)->deployUnit((UnitClass*)object);

											break;

										default:

											break;

										}

									}



									map->removeObjectFromMap(object);

									object->setLocation(lx, ly);

									((UnitClass*)object)->resetStartPoint();



								}

								if (destObjectID != NONE)

									destObject = map->findObjectWidthID(destObjectID, dx, dy);

								object->setTarget(destObject);

								if (destObject != NULL)

									object->setDestination(destObject->getLocation());

								object->setDestination(dx, dy);

									

								if (object->isAUnit())

								{

									((UnitClass*)object)->clearPath();

									if (destObject != NULL)

										((UnitClass*)object)->setGuardPoint(destObject->getLocation());

									else if (dx != NONE)

										((UnitClass*)object)->setGuardPoint(dx, dy);

									((UnitClass*)object)->clearNextSpotFound();

								}

								object->setForced(forced == 1);

							}

							else

								showMessage("ERROR: MSG_OBJECTCOMMAND - invalid message received from server.");

						}

						else

						{

							showMessage("ERROR: MSG_OBJECTCOMMAND - invalid message received from server: message incorrectly formatted");

							showMessage(msg);

							fprintf(stderr, "ERROR: MSG_OBJECTCOMMAND - invalid message received from server: message incorrectly formatted");

							fprintf(stderr, msg);

						}

						break;

					case MSG_OBJECTDESTROY:

						numConverted = sscanf(&msg[1], "%d,%d,%d", &objectID, &lx, &ly);

						if ((numConverted == 3) && map->cellExists(lx, ly) && (objectID != NONE))

						{

							object = map->findObjectWidthID(objectID, lx, ly);

							if (object)

							{

								//make sure unit is not held by something already

								if (object->isAUnit() && object->hasATarget())	//if so eject them first

								{

									ObjectClass* target = object->getTarget();

									switch(target->getItemID())

									{

									case Structure_Refinery:

										if (!((RefineryClass*)target)->isFree() && (((RefineryClass*)target)->getHarvester() == object))

											((RefineryClass*)target)->deployHarvester();

										break;

									case Structure_RepairYard:

										if (!((RepairYardClass*)target)->isFree() && (((RepairYardClass*)target)->getRepairUnit() == object))

											((RepairYardClass*)target)->deployRepairUnit();

										break;

									case Unit_Carryall:

										((Carryall*)target)->deployUnit((UnitClass*)object);

										break;

									case Unit_Frigate:

										((Frigate*)target)->deployUnit((UnitClass*)object);

										break;

									default:

										break;

									}

								}

								object->destroy();

							}

							else

								showMessage("ERROR: MSG_OBJECTDESTROY - invalid message received from server.");

						}

						else

							showMessage("ERROR: MSG_OBJECTDESTROY - invalid message received from server: message incorrectly formatted");

						break;

					case MSG_SETATTACKMODE:

						numConverted = sscanf(&msg[1], "%d,%d,%d,%d", &objectID, &lx, &ly, &angle);

						if ((numConverted == 4) && (map->cellExists(lx, ly) || (lx == NONE)) && (objectID != NONE) && ((angle >= 0) && (angle < NUM_ANGLES)))

						{

							object = map->findObjectWidthID(objectID, lx, ly);

							if (object && object->isAUnit())

								((UnitClass*)object)->setAttackMode((ATTACKTYPE)angle);

						}

						else

							showMessage("ERROR: MSG_STARPORTORDER - invalid message received from client: message incorrectly formatted");

						break;

					case MSG_PLACESTRUCTURE:

						numConverted = sscanf(&msg[1], "%d,%d,%d,%d,%d,%d,%d,%d", &playerID, &objectID, &lx, &ly, &itemID, &dx, &dy, &angle);

						if ((numConverted == 8) && map->cellExists(lx, ly) && (objectID != NONE))

						{

							object = map->findObjectWidthID(objectID, lx, ly);

							if (object

								&& ((object->isAStructure() && ((StructureClass*)object)->isABuilder())

									|| ((object->getItemID() == Unit_MCV) && (itemID == Structure_ConstructionYard))

								&& map->cellExists(lx, ly) && (playerID >= 0) && (playerID < MAX_PLAYERS)))

							{

								StructureClass* structure = (StructureClass*)player[playerID]->placeStructure(objectID, lx, ly, itemID, dx, dy);

								if (structure)

									structure->setJustPlaced();

							}

							else

								showMessage("ERROR: MSG_PLACESTRUCTURE - invalid message received from server.");

						}

						else

							showMessage("ERROR: MSG_PLACESTRUCTURE - invalid message received from server: message incorrectly formatted");

						break;

					case MSG_STARPORTORDER:

						numConverted = sscanf(&msg[1], "%d,%d,%d,%d", &playerID, &objectID, &lx, &ly);

						if ((numConverted == 4) && map->cellExists(lx, ly) && (objectID != NONE))

						{

							object = map->findObjectWidthID(objectID, lx, ly);

							if (object && object->isAStructure() && (object->getItemID() == Structure_StarPort)

								&& map->cellExists(lx, ly) && (playerID >= 0) && (playerID < MAX_PLAYERS))

								((StarPortClass*)object)->placeOrder();

							else

								showMessage("ERROR: MSG_STARPORTORDER - invalid message received from server.");

						}

						else

							showMessage("ERROR: MSG_STARPORTORDER - invalid message received from server: message incorrectly formatted");

						break;

					case MSG_STARPORTCANCEL:

						numConverted = sscanf(&msg[1], "%d,%d,%d,%d", &playerID, &objectID, &lx, &ly);

						if ((numConverted == 4) && map->cellExists(lx, ly) && (objectID != NONE))

						{

							object = map->findObjectWidthID(objectID, lx, ly);

							if (object && object->isAStructure() && (object->getItemID() == Structure_StarPort)

								&& map->cellExists(lx, ly) && (playerID >= 0) && (playerID < MAX_PLAYERS))

								((StarPortClass*)object)->cancelOrder();

							else

								showMessage("ERROR: MSG_STARPORTCANCEL - invalid message received from server.");

						}

						else

							showMessage("ERROR: MSG_STARPORTCANCEL - invalid message received from server: message incorrectly formatted");

						break;

					case MSG_PALACESPECIAL:

						numConverted = sscanf(&msg[1], "%d,%d,%d,%d,%d,%d", &playerID, &objectID, &lx, &ly, &dx, &dy);

						if ((numConverted == 6) && map->cellExists(lx, ly) && (objectID != NONE))

						{

							object = map->findObjectWidthID(objectID, lx, ly);

							if (object && object->isAStructure() && (object->getItemID() == Structure_Palace))

							{

								((PalaceClass*)object)->setDeathhandOff(dx, dy);

								((PalaceClass*)object)->doSpecial();

							}

							else

								showMessage("ERROR: MSG_PALACESPECIAL - invalid message received from server.");

						}

						else

							showMessage("ERROR: MSG_PALACESPECIAL - invalid message received from server: message incorrectly formatted");

						break;

					case MSG_PALACEENABLE:

						numConverted = sscanf(&msg[1], "%d,%d,%d,%d", &playerID, &objectID, &lx, &ly);

						if ((numConverted == 4) && map->cellExists(lx, ly) && (objectID != NONE))

						{

							object = map->findObjectWidthID(objectID, lx, ly);

							if (object && object->isAStructure() && (object->getItemID() == Structure_Palace))

								((PalaceClass*)object)->enableSpecial(true);

							else

								showMessage("ERROR: MSG_PALACEENABLE - invalid message received from server.");

						}

						else

							showMessage("ERROR: MSG_PALACEENABLE - invalid message received from server: message incorrectly formatted");

						break;

					case MSG_QUIT:
						break;
					default:
						break;
				}
			}
			else
			{
				showMessage("Disconnected from server.");

				//close the server socket

				SDLNet_TCP_DelSocket(socketSet, server);

				SDLNet_TCP_Close(server);

				settings.playerType = SINGLE;
			}
		}
	}

	if (numReady == -1)
	{//must be disconnected
		showMessage("Disconnected from server.");

		//close the server socket

		SDLNet_TCP_DelSocket(socketSet, server);

		SDLNet_TCP_Close(server);

		settings.playerType = SINGLE;
	}
}

void doServer()	//check for info from clients
{

	char	msg[MAX_MSGLENGTH+50];

	int		i,
			forced,
			numReady,
			numConverted,
			objectID, playerID, itemID, destObjectID, lx, ly, dx, dy, angle;
	ObjectClass	*destObject, *object;
	COORDTYPE	temp;
	TCPsocket	newClient;

	while ((numReady = SDLNet_CheckSockets(socketSet, 0) > 0))
	{
		if(SDLNet_SocketReady(server))	//someone is trying to join in game

		{

			numReady--;

			newClient = SDLNet_TCP_Accept(server);
			showMessage("player trying to join game in progress.\n");
			SDLNet_TCP_Close(newClient);	//they can't join a game in progress
		}

		for (i = 1; numReady && (i < MAX_PLAYERS); i++)

		{

			if (SDLNet_SocketReady(sockets[i]))

			{

				numReady--;

				if (SDLNet_TCP_Recv(sockets[i], msg, MAX_MSGLENGTH)==MAX_MSGLENGTH)

				{

					msg[MAX_MSGLENGTH-1] = '\0';

					//showMessage(msg);

					switch (msg[0])

					{

						case MSG_BUILDCOMMAND:

							numConverted = sscanf(&msg[1], "%d,%d,%d,%d,%d", &objectID, &lx, &ly, &dx, &dy);

							if ((numConverted == 5) && map->cellExists(lx, ly) && (objectID != NONE))

							{

								object = map->findObjectWidthID(objectID, lx, ly);

								if (object && (object->getOwner()->getPlayerNumber() == i) && object->isAStructure() && ((StructureClass*)object)->isABuilder())

								{

									if (((BuilderClass*)object)->buildNum(dx ? true : false, dy))

									{

										temp.x = lx; temp.y = ly;

										netServerBuildMessage(objectID, &temp, dx ? true : false, dy);

									}

								}

								else

									showMessage("ERROR: invalid message received from client.");

							}

							else

								showMessage("ERROR: invalid message received from client.");

							break;

						case MSG_CANCELCOMMAND:

							numConverted = sscanf(&msg[1], "%d,%d,%d,%d,%d", &objectID, &lx, &ly, &dx, &dy);

							if ((numConverted == 5) && map->cellExists(lx, ly) && (objectID != NONE))

							{

								object = map->findObjectWidthID(objectID, lx, ly);

								if (object && (object->getOwner()->getPlayerNumber() == i) && object->isAStructure() && ((StructureClass*)object)->isABuilder())

									((BuilderClass*)object)->netDoCancelCommand(dx ? true : false, dy);

								else

									showMessage("ERROR: invalid message received from client.");

							}

							else

								showMessage("ERROR: invalid message received from client.");

							break;

						case MSG_DEVASTATE:

							numConverted = sscanf(&msg[1], "%d,%d,%d", &objectID, &lx, &ly);

							if ((numConverted == 3) && (map->cellExists(lx, ly) || (lx == NONE)) && (objectID != NONE))

							{

								object = map->findObjectWidthID(objectID, lx, ly);

								if (object && (object->getOwner()->getPlayerNumber() == i) && (object->getItemID() == Unit_Devastator))

									((DevastatorClass*)object)->netDevastate();

								else

									showMessage("ERROR: MSG_DEVASTATE - invalid message received from client.");

							}

							else

								showMessage("ERROR: MSG_DEVASTATE - invalid message received from client: message incorrectly formatted");

							break;

						case MSG_MESSAGE:

						{

							char	inMessage[MAX_MSGLENGTH + MAX_NAMELENGTH + 2];



							playerID = (int)msg[1];

							if (((playerID >= 0) && (playerID < MAX_PLAYERS)) && (player[playerID] != NULL))

							{

								sprintf(inMessage, "%s: %s", settings.playerName[playerID], &msg[2]);

								showMessage(inMessage);



								netServerMessage(playerID, &msg[2]);

							}

							else

								showMessage("ERROR: MSG_MESSAGE - invalid message received from server: message incorrectly formatted");

							break;

						}

						case MSG_OBJECTCOMMAND:

							numConverted = sscanf(&msg[1], "%d,%d,%d,%d,%d,%d,%d", &objectID, &destObjectID, &lx, &ly, &dx, &dy, &forced);

							if ((numConverted == 7) && map->cellExists(lx, ly) && (objectID != NONE))

							{

								destObject = NULL;

								object = map->findObjectWidthID(objectID, lx, ly);

								if (object && (object->getOwner()->getPlayerNumber() == i))

								{

									if (object->isRespondable())

									{

										if (destObjectID != NONE)

											destObject = map->findObjectWidthID(destObjectID, dx, dy);

										object->setTarget(destObject);

										object->setDestination(dx, dy);



										if (object->isAUnit())

										{

											((UnitClass*)object)->clearPath();

											if (destObject != NULL)

												((UnitClass*)object)->setGuardPoint(destObject->getLocation());

											else

												((UnitClass*)object)->setGuardPoint(dx, dy);

											((UnitClass*)object)->clearNextSpotFound();

										}

										object->setForced(forced == 1);



										//forward the message to all clients

										netServerObjectMessage(objectID, destObjectID, object->getX(), object->getY(), dx, dy, forced == 1);

									}

								}

								else

									showMessage("ERROR: invalid message received from client.");

							}

							else

								showMessage("ERROR: invalid message received from client.");

							break;

						case MSG_OBJECTDESTROY:

							numConverted = sscanf(&msg[1], "%d,%d,%d", &objectID, &lx, &ly);

							if ((numConverted == 3) && map->cellExists(lx, ly) && (objectID != NONE))

							{

								object = map->findObjectWidthID(objectID, lx, ly);

								if (object && (object->getOwner()->getPlayerNumber() == i))

								{

									object->destroy();

									netServerDestroyObject(objectID, lx, ly);

								}

								else

									showMessage("ERROR: invalid message received from client.");

							}

							else

								showMessage("ERROR: invalid message received from client.");

							break;

						case MSG_PALACESPECIAL:

							numConverted = sscanf(&msg[1], "%d,%d,%d,%d", &playerID, &objectID, &lx, &ly);

							if ((numConverted == 4) && map->cellExists(lx, ly) && (objectID != NONE))

							{

								object = map->findObjectWidthID(objectID, lx, ly);

								if (object && object->isAStructure() && (object->getItemID() == Structure_Palace) && (object->getOwner()->getPlayerNumber() == i))

								{

									if (((PalaceClass*)object)->specialReady())

									{

										((PalaceClass*)object)->doSpecial();

										netServerPalaceSpecial(playerID, objectID, lx, ly, ((PalaceClass*)object)->getDeathOffX(), ((PalaceClass*)object)->getDeathOffY());

									}

								}

								else

									showMessage("ERROR: MSG_PALACESPECIAL - invalid message received from server.");

							}

							else

								showMessage("ERROR: MSG_PALACESPECIAL - invalid message received from server: message incorrectly formatted");

							break;

						case MSG_PLACESTRUCTURE:

							numConverted = sscanf(&msg[1], "%d,%d,%d,%d,%d,%d,%d,%d", &playerID, &objectID, &lx, &ly, &itemID, &dx, &dy, &angle);

							if ((numConverted == 8) && map->cellExists(lx, ly) && (objectID != NONE))

							{

								object = map->findObjectWidthID(objectID, lx, ly);

								if (object && (object->getOwner()->getPlayerNumber() == i)

									&& ((object->isAStructure() && ((StructureClass*)object)->isABuilder()) && ((BuilderClass*)object)->isTypeWaitingToPlace(itemID))

										|| ((object->getItemID() == Unit_MCV) && (itemID == Structure_ConstructionYard))

									&& map->cellExists(lx, ly) && (playerID >= 0) && (playerID <= MAX_PLAYERS))

								{

									player[playerID]->netPlaceStructure(objectID, lx, ly, itemID, dx, dy);

									if (object->getItemID() == Unit_MCV)

										object->netDestroy();

								}

								else

									showMessage("ERROR: invalid message received from client.");

							}

							else

								showMessage("ERROR: invalid message received from client.");

							break;

						case MSG_SETATTACKMODE:

							numConverted = sscanf(&msg[1], "%d,%d,%d,%d", &objectID, &lx, &ly, &angle);

							if ((numConverted == 4) && (map->cellExists(lx, ly) || (lx == NONE)) && (objectID != NONE) && ((angle >= 0) && (angle < NUM_ANGLES)))

							{

								object = map->findObjectWidthID(objectID, lx, ly);

								if (object && (object->getOwner()->getPlayerNumber() == i) && object->isAUnit())

									((UnitClass*)object)->netSetAttackMode((ATTACKTYPE)angle);

							}

							else

								showMessage("ERROR: MSG_STARPORTORDER - invalid message received from client: message incorrectly formatted");

							break;

						case MSG_STARPORTORDER:

							numConverted = sscanf(&msg[1], "%d,%d,%d,%d", &playerID, &objectID, &lx, &ly);

							if ((numConverted == 4) && map->cellExists(lx, ly) && (objectID != NONE))

							{

								object = map->findObjectWidthID(objectID, lx, ly);

								if (object && (object->getOwner()->getPlayerNumber() == i)

									&& object->isAStructure() && (object->getItemID() == Structure_StarPort)

									&& map->cellExists(lx, ly) && (playerID == i))

								{

									((StarPortClass*)object)->placeOrder();

									netServerPlaceOrder(playerID, objectID, lx, ly);

								}

								else

									showMessage("ERROR: MSG_STARPORTORDER - invalid message received from client.");

							}

							else

								showMessage("ERROR: MSG_STARPORTORDER - invalid message received from client: message incorrectly formatted");

							break;

						case MSG_STARPORTCANCEL:

							numConverted = sscanf(&msg[1], "%d,%d,%d,%d", &playerID, &objectID, &lx, &ly);

							if ((numConverted == 4) && map->cellExists(lx, ly) && (objectID != NONE))

							{

								object = map->findObjectWidthID(objectID, lx, ly);

								if (object && (object->getOwner()->getPlayerNumber() == i)

									&& object->isAStructure() && (object->getItemID() == Structure_StarPort) && (playerID == i))

								{

									((StarPortClass*)object)->cancelOrder();

									netServerCancelOrder(playerID, objectID, lx, ly);

								}

								else

									showMessage("ERROR: MSG_STARPORTCANCEL - invalid message received from server.");

							}

							else

								showMessage("ERROR: MSG_STARPORTCANCEL - invalid message received from server: message incorrectly formatted");

							break;

						case MSG_QUIT:
							break;
						default:
							break;
					}
				}
				else
				{
					sprintf(msg, "%s has left the game.", settings.playerName[i]);

					showMessage(msg);

					settings.playerName[i][0] = '\0';

					SDLNet_TCP_DelSocket(socketSet, sockets[i]);

					SDLNet_TCP_Close(sockets[i]);

					sockets[i] = NULL;
				}
			}
		}

		if (numReady == -1)
		{//must be disconnected
			showMessage("Disconnected from clients.\n");

			//close all the sockets

			for(i = 1; i < MAX_PLAYERS; i++)

			if (sockets[i] != NULL)
			{
				settings.playerName[i][0] = '\0';
				SDLNet_TCP_DelSocket(socketSet, sockets[i]);

				SDLNet_TCP_Close(sockets[i]);
			}
			SDLNet_TCP_DelSocket(socketSet, server);

			SDLNet_TCP_Close(server);

			settings.playerType = SINGLE;
		}
	}
}

void netUpdate()
{
	if (settings.playerType == CLIENT)
		doClient();
	else if (settings.playerType == SERVER)
		doServer();
}

void checkSelectionInterface()
{
	unitButton_Deploy.setEnabled(false);
	unitButton_Deploy.setVisible(false);
	unitButton_Destruct.setEnabled(false);
	unitButton_Destruct.setVisible(false);
	//unitIcon.setVisible(false);

	unitButton_Defensive.setSurface(defensiveSurf, NULL);
	unitButton_Aggressive.setSurface(aggressiveSurf, NULL);
	unitButton_Scout.setSurface(scoutSurf, NULL);
	unitButton_StandGround.setSurface(standGroundSurf, NULL);
	
	if (!selectedList->isEmpty())
	{
		/*
		int different_units = 0;
		int lastUnitID = -1;
		int picID;*/

		ObjectClass *tempObject;
		UnitClass* tempUnit = NULL;
		selectedList->reset();

		while(selectedList->currentNotNull())
		{
			tempObject = (ObjectClass*)selectedList->getCurrent();
			if (tempObject->isAUnit() && tempObject->getOwner() == thisPlayer)
			{
				tempUnit = (UnitClass*)tempObject;
				if (tempUnit->getItemID() == Unit_MCV)
				{
					unitButton_Deploy.setEnabled(true);
					unitButton_Deploy.setVisible(true);
				}
				else if (tempUnit->getItemID() == Unit_Devastator)
				{
					unitButton_Destruct.setEnabled(true);
					unitButton_Destruct.setVisible(true);
				}

				if(tempUnit->getAttackMode() == AGGRESSIVE)
				{			
					unitButton_Aggressive.setSurface(aggressiveSurfActive, NULL);
				}
				else if(tempUnit->getAttackMode() == DEFENSIVE)
				{
					unitButton_Defensive.setSurface(defensiveSurfActive, NULL);
				}
				else if(tempUnit->getAttackMode() == STANDGROUND)
				{
					unitButton_StandGround.setSurface(standGroundSurfActive, NULL);
				}
				else if(tempUnit->getAttackMode() == SCOUT)
				{
					unitButton_Scout.setSurface(scoutSurfActive, NULL);
				}

					
			}
		
			/*
			if(tempObject->getItemID() != lastUnitID && lastUnitID != -1)
			{
				different_units = 1;
			}

			lastUnitID = tempObject->getItemID();*/

			selectedList->nextLink();
		}

		/*
		picID = resolveItemPicture(lastUnitID);
		if(picID != -1)
		{
			unitIcon.setVisible(true);
			if(different_units)
			{
				SDL_Surface *surf = (SDL_Surface*)(dataFile[picID].dat);
				unitIcon.setWidth(surf->w);
				unitIcon.setHeight(surf->h);
				unitIcon.setX(unitIcon.getX() + 10);
				unitIcon.setSurface(surf, NULL);
			}
			else
			{
				SDL_Surface *surf = (SDL_Surface*)(dataFile[resolveItemPicture(lastUnitID)].dat);
				unitIcon.setWidth(surf->w + 1);
				unitIcon.setHeight(surf->h + 1);
				unitIcon.setX(unitInterface.getX() + (unitInterface.getWidth() - surf->w)/2);
				unitIcon.setSurface(surf, NULL);
			}
		}
		*/
	}
}

void doUnitIcon()
{
	unitIcon.setVisible(false);
	
	if (!selectedList->isEmpty())
	{
		int different_units = 0;
		int lastUnitID = -1;
		int picID;

		ObjectClass *tempObject;
		selectedList->reset();

		while(selectedList->currentNotNull())
		{
			tempObject = (ObjectClass*)selectedList->getCurrent();
					
			if(tempObject->getItemID() != lastUnitID && lastUnitID != -1)
			{
				different_units = 1;
			}

			lastUnitID = tempObject->getItemID();

			selectedList->nextLink();
		}

		picID = resolveItemPicture(lastUnitID);
		if(picID != -1)
		{
			unitIcon.setVisible(true);
			if(different_units)
			{
				SDL_Surface *surf = (SDL_Surface*)(dataFile[picID].dat);
				unitIcon.setWidth(surf->w);
				unitIcon.setHeight(surf->h);
				unitIcon.setX(unitInterface.getX() + (unitInterface.getWidth() - surf->w)/2);
				unitIcon.setSurface(surf, NULL);
			}
			else
			{
				SDL_Surface *surf = (SDL_Surface*)(dataFile[resolveItemPicture(lastUnitID)].dat);
				unitIcon.setWidth(surf->w + 1);
				unitIcon.setHeight(surf->h + 1);
				unitIcon.setX(unitInterface.getX() + (unitInterface.getWidth() - surf->w)/2);
				unitIcon.setSurface(surf, NULL);
			}
		}
	}
}

void mcvDeploy()
{
	if (!selectedList->isEmpty())
	{
		ObjectClass *tempObject;
		selectedList->reset();
		while(selectedList->currentNotNull())
		{
			tempObject = (ObjectClass*)selectedList->getCurrent();
			if (tempObject->getItemID() == Unit_MCV)
				((MCVClass*)tempObject)->netDeploy();
			if (selectedList->getCurrent() == tempObject)
				selectedList->nextLink();
		}
	}
}

void devastatorDestruct()
{
	if (!selectedList->isEmpty())
	{
		ObjectClass *tempObject;
		selectedList->reset();
		while(selectedList->currentNotNull())
		{
			tempObject = (ObjectClass*)selectedList->getCurrent();
			if (tempObject->getItemID() == Unit_Devastator)
				((DevastatorClass*)tempObject)->netDevastate();
			if (selectedList->getCurrent() == tempObject)
				selectedList->nextLink();
		}
	}
}

void setAttackMode(void* data)
{
	if (!selectedList->isEmpty())
	{
		ATTACKTYPE newAttackMode = (ATTACKTYPE)((int)data);

		ObjectClass *tempObject;
		UnitClass* tempUnit = NULL;
		selectedList->reset();
		while(selectedList->currentNotNull())
		{
			tempObject = (ObjectClass*)selectedList->getCurrent();
			if (tempObject->isAUnit())
			{
				tempUnit = (UnitClass*)tempObject;
				tempUnit->netSetAttackMode(newAttackMode);
			}

			selectedList->nextLink();
		}
		if (tempUnit)
		{
			tempUnit->playConfirmSound();

			unitButton_Defensive.setSurface(defensiveSurf, NULL);
			unitButton_Aggressive.setSurface(aggressiveSurf, NULL);
			unitButton_Scout.setSurface(scoutSurf, NULL);
			unitButton_StandGround.setSurface(standGroundSurf, NULL);

			if(tempUnit->getAttackMode() == AGGRESSIVE)
			{			
					unitButton_Aggressive.setSurface(aggressiveSurfActive, NULL);
			}
			else if(tempUnit->getAttackMode() == DEFENSIVE)
			{
				unitButton_Defensive.setSurface(defensiveSurfActive, NULL);
			}
			else if(tempUnit->getAttackMode() == STANDGROUND)
			{
				unitButton_StandGround.setSurface(standGroundSurfActive, NULL);
			}
			else if(tempUnit->getAttackMode() == SCOUT)
			{
				unitButton_Scout.setSurface(scoutSurfActive, NULL);
			}
		}
	}
}

void game_resume()
{
	menuMode = false;
	settings.update = true;
}

void game_quit()
{
	quitGame = true;
	gameState = END;
}

void doInput()
{
	int localNumElements;
	ObjectClass* tempObject;
	SDL_Event event;
	while(SDL_PollEvent(&event))	//check for a key press
	{
		/* Look for a keypress */
		switch (event.type)
		{
		case (SDL_KEYDOWN):
			if (menuMode && focusedWidget && focusedWidget->isAnEntry())
				((Entry*)focusedWidget)->handleKeyPress(&event.key);
			else if (messageMode)
			{
				if (event.key.keysym.sym == SDLK_ESCAPE)

					messageMode = false;

				else if (event.key.keysym.sym == SDLK_RETURN)

				{

					if (strlen(typingMessage) > 0)

					{

						if (settings.playerType == SERVER)

							netServerMessage(thisPlayer->getPlayerNumber(), typingMessage);

						else if (settings.playerType == CLIENT)

							netClientMessage(thisPlayer->getPlayerNumber(), typingMessage);

					}



					messageMode = false;

				}

				else if (event.key.keysym.sym == SDLK_BACKSPACE)

				{

					if (strlen(typingMessage) > 0)

						typingMessage[strlen(typingMessage) - 1] = '\0';

				}

				else if (strlen(typingMessage) < (MAX_MSGLENGTH - 3))

				{

					if ((event.key.keysym.unicode < 0x80) && (event.key.keysym.unicode > 0))
					{
						typingMessage[strlen(typingMessage) + 1] = '\0';
						typingMessage[strlen(typingMessage)] = (char)event.key.keysym.unicode;
					}
				}
			}
			else
			{
				switch( event.key.keysym.sym )
				{
					case SDLK_0:	//if ctrl and 0 remove selected units from all groups
						localNumElements = selectedList->getNumElements();
						if (groupCreateMode)
						{
							ObjectClass* object;
							while (!selectedList->isEmpty())
							{
								object = (ObjectClass*)selectedList->removeFirstElement();
								object->setSelected(false);
								object->removeFromSelectionLists();
							}
							placingMode = false;
						}
						else
						{
							while (!selectedList->isEmpty())
								((ObjectClass*)selectedList->removeFirstElement())->setSelected(false);
							placingMode = false;
						}
						break;	//for SDLK_1 to SDLK_9 select group with that number, if ctrl create group from selected obj
					case SDLK_1: case SDLK_2: case SDLK_3: case SDLK_4: case SDLK_5: case SDLK_6: case SDLK_7: case SDLK_8: case SDLK_9:
						if (groupCreateMode)
						{
							selectedLists[event.key.keysym.sym - SDLK_1].clearList();
							copyList(selectedList, &selectedLists[event.key.keysym.sym - SDLK_1]);

							checkSelectionInterface();
						}
						else
						{
							if (!shift)
							{
								unselectAll(selectedList);
								selectedList->clearList();
							}
							
							copyList(&selectedLists[event.key.keysym.sym - SDLK_1], selectedList);
							selectAll(selectedList);

							checkSelectionInterface();
						}
						placingMode = false;
						break;
					case SDLK_MINUS:
					{
						//if (settings.playerType != CLIENT)
						{
							if (++settings.gamespeed > 100)
								settings.gamespeed = 100;
							char temp[20];
							sprintf(temp, "GameSpeed:%d", settings.gamespeed);
							showMessage(temp);
							break;
						}
					}
					case SDLK_EQUALS://PLUS
					{
						//if (settings.playerType != CLIENT)
						{
							if (--settings.gamespeed <= 0)
								settings.gamespeed = 1;
							char temp[20];
							sprintf(temp, "GameSpeed:%d", settings.gamespeed);
							showMessage(temp);
							break;
						}
					}
					case SDLK_a:	//set object on attack move
						if (!selectedList->isEmpty())
						{
							selectedList->reset();
							while(!attackMoveMode && selectedList->currentNotNull())
							{
								tempObject = (ObjectClass*)selectedList->getCurrent();

								if (tempObject->isAUnit()

									&& (tempObject->getOwner() == thisPlayer)

									&& tempObject->isRespondable()

									&& tempObject->canAttack())

								{

									attackMoveMode = true;

									cursorFrame = CURSOR_TARGET;

								}

								selectedList->nextLink();
							}
						}
						break;
					case SDLK_d:
						mcvDeploy();
						break;
					case SDLK_f:
						/*switch fog of war*/
						fog_wanted = !fog_wanted;
						break;
					case SDLK_DOWN:
						moveDownMode = true;;
						break;
					case SDLK_DELETE:
					{
						if (!selectedList->isEmpty())
						{
							selectedList->reset();
							if (shift)
							{
								while(selectedList->currentNotNull())
								{
									tempObject = (ObjectClass*)selectedList->getCurrent();

									if (debug || (tempObject->isRespondable()

													&& (tempObject->isAStructure()

														&& (tempObject->getOwner() == thisPlayer))

													|| (tempObject->isAUnit()

														&& (((UnitClass*)tempObject)->getRealOwner() == thisPlayer))))

									{

										selectedList->removeCurrent();

										tempObject->setSelected(false);

										tempObject->netDestroy();
									}
									else
										selectedList->nextLink();
								}
							}
							else
							{
								tempObject = (ObjectClass*)selectedList->getCurrent();

								if (debug || (tempObject->isRespondable()

									&& (tempObject->isAStructure()

										&& (tempObject->getOwner() == thisPlayer))

									|| (tempObject->isAUnit()

										&& (((UnitClass*)tempObject)->getRealOwner() == thisPlayer))))

								{

									selectedList->removeCurrent();

									tempObject->setSelected(false);

									tempObject->netDestroy();
								}
							}
						}
						break;
					}
					case SDLK_ESCAPE:
						if (menuMode)
						{
							if (menuInterface == &menuWindow)
								game_resume();
							else
								menuInterface = &menuWindow;
						}
						else if(showMentat)
						{
							game_mentat_exited();
						}
						else
						{
							menuMode = true;
							menuInterface = &menuWindow;
							if (settings.playerType == SINGLE)
								settings.update = false;
						}
						break;
#ifdef _DEBUG
					case SDLK_F1:

						if (settings.gameType != MULTIPLAYER)
							debug = !debug;
						break;

					case SDLK_F2:

						if (settings.gameType != MULTIPLAYER)
						{
							settings.won = true;
							settings.finished = true;
						}
						break;
#endif
					/*case SDLK_F2:

						shade = !shade;

						break;*/
					case SDLK_F11:
						showFPS = !showFPS;
						break;
					case SDLK_F12:
						soundPlayer->toggleSound();
						break;
					case SDLK_LCTRL:
						groupCreateMode = true;
						break;
					case SDLK_LEFT:
						moveLeftMode = true;
						break;
					case SDLK_LSHIFT:
						shift = true;
						break;
					case SDLK_m:
						soundPlayer->changeMusic(MUSIC_RANDOM);
						break;

					case SDLK_PRINT: case SDLK_SYSREQ:

						SDL_SaveBMP(screen, "screenshot.bmp");

						showMessage("screenshot.bmp saved");

						break;
					case SDLK_r:
						if (!selectedList->isEmpty())
						{
							ObjectClass *tempObject;
							selectedList->reset();
							while(selectedList->currentNotNull())
							{
								tempObject = (ObjectClass*)selectedList->getCurrent();
								tempObject->repair();
								selectedList->nextLink();
							}
						}
						break;
					case SDLK_RETURN:
						typingMessage[0] = '\0';
						messageMode = true;
						break;
					case SDLK_RIGHT:
						moveRightMode = true;
						break;
					case SDLK_SPACE:
						//if not multiplayer
						if ((settings.playerType == SINGLE) && !menuMode)
							settings.update = !settings.update;
						break;
					case SDLK_TAB:
						if (fullscreenMode)
						{
							fullscreenMode = false;
							dborder->maxX -= gameBarPos.w;

							//altered
							
							dborder->minY -= topBarPos.h - TOPBAR_MASKED_AREA_HEIGHT;
							
						}
						else
						{
							fullscreenMode = true;
							dborder->maxX += gameBarPos.w;

							if (dborder->maxX >= map->sizeX*BLOCKSIZE)
							{
								dborder->maxX = map->sizeX*BLOCKSIZE-1;
								dborder->minX = map->sizeX*BLOCKSIZE - screen->w;
							}

							//altered
							dborder->minY -= topBarPos.h;
							if (dborder->minY < 0)
							{
								dborder->maxY = map->sizeY*BLOCKSIZE - screen->h;
								dborder->minY = 0;// map->sizeY*BLOCKSIZE-1;
							}

							radarMode = false;	//once press tab we can no longer be stuck in radar square
						}
						break;
					case SDLK_UP:
						moveUpMode = true;
						break;
					default:
						break;
				}
				break;
			}
		case (SDL_KEYUP):
			switch( event.key.keysym.sym )
			{
				case SDLK_DOWN:
					moveDownMode = false;
					break;
				case SDLK_LCTRL:
					groupCreateMode = false;
					break;
				case SDLK_LEFT:
					moveLeftMode = false;
					break;
				case SDLK_LSHIFT:
					shift = false;
					break;
				case SDLK_RIGHT:
					moveRightMode = false;
					break;
				case SDLK_UP:
					moveUpMode = false;
					break;
				default:
					 break;
			}
			break;
		case SDL_MOUSEBUTTONDOWN:
		{
			SDL_MouseButtonEvent* mouse = &event.button;

			//TEMP TEMP TEMP
			if(!menuMode && !showMentat && !fullscreenMode)
			{
				if ((mouse->button == SDL_BUTTON_LEFT) || (mouse->button == SDL_BUTTON_RIGHT))
					mainButtonsBox.handlePress(mouse->x, mouse->y);
			}
			
			if (menuMode)
			{
				if ((mouse->button == SDL_BUTTON_LEFT) || (mouse->button == SDL_BUTTON_RIGHT))
					menuInterface->handlePress(mouse->x, mouse->y);
			}
			else if(showMentat)
			{
				if ((mouse->button == SDL_BUTTON_LEFT) || (mouse->button == SDL_BUTTON_RIGHT))
					mentat->handlePress(mouse->x, mouse->y);
			}
			else switch (mouse->button)
			{
				case (SDL_BUTTON_LEFT):
				{
					int xPos = (mouse->x + dborder->minX)/BLOCKSIZE,
						yPos = (mouse->y + dborder->minY)/BLOCKSIZE;

					if (placingMode)
					{
						if (fullscreenMode || (mouse->x < gameBarPos.x))	//if mouse is not over game bar
						{
							BuilderClass* builder = static_cast<BuilderClass*>(selectedList->getFirstElement());
							BuildItemClass* placeItem = static_cast<BuildItemClass*>(builder->getPlaceItem());

							if (placeItem->getItemID() == Structure_Slab4)
							{
								if ((map->isWithinBuildRange(xPos, yPos, builder->getOwner()) || map->isWithinBuildRange(xPos+1, yPos, builder->getOwner()) || map->isWithinBuildRange(xPos+1, yPos+1, builder->getOwner()) || map->isWithinBuildRange(xPos, yPos+1, builder->getOwner()))
									&& (map->okayToPlaceStructure(xPos, yPos, 1, 1, false, builder->getOwner()) || map->okayToPlaceStructure(xPos+1, yPos, 1, 1, false, builder->getOwner()) || map->okayToPlaceStructure(xPos+1, yPos+1, 1, 1, false, builder->getOwner()) || map->okayToPlaceStructure(xPos, yPos, 1, 1+1, false, builder->getOwner())))
									builder->getOwner()->netPlaceStructure(builder->getObjectID(), builder->getLocation()->x, builder->getLocation()->y, placeItem->getItemID(), xPos, yPos);
							}
							else if ((placeItem->getItemID() == Structure_Slab1) && map->okayToPlaceStructure(xPos, yPos, placeItem->getStructureSizeX(), placeItem->getStructureSizeY(), false, builder->getOwner()))
								builder->getOwner()->netPlaceStructure(builder->getObjectID(), builder->getLocation()->x, builder->getLocation()->y, placeItem->getItemID(), xPos, yPos);
							else if (map->okayToPlaceStructure(xPos, yPos, placeItem->getStructureSizeX(), placeItem->getStructureSizeY(), false, builder->getOwner()))
								builder->getOwner()->netPlaceStructure(builder->getObjectID(), builder->getLocation()->x, builder->getLocation()->y, placeItem->getItemID(), xPos, yPos);

							if (settings.playerType != CLIENT)
							{
								if (placingMode)	//the user has tried to place but clicked on 
								{					//impossible point
									showMessage("You cannot place it here.");
									soundPlayer->playSfxSound(Sound_cannot);	//can't place noise
								}
								else				//the user has tried to place and has been successful
									soundPlayer->playSfxSound(Sound_placeStructure);
							}
						}
					}
					else if (attackMoveMode)
					{
						if (!selectedList->isEmpty())
						{
							UnitClass* responder = NULL;
							selectedList->reset();
							while(selectedList->currentNotNull())
							{
								tempObject = (ObjectClass*)selectedList->getCurrent();

								if (tempObject->isAUnit()

									&& (tempObject->getOwner() == thisPlayer)

									&& tempObject->isRespondable())

								{

									COORDTYPE temp = {xPos, yPos};

									tempObject->netDoCommand(NULL, &temp, false);

									responder = (UnitClass*)tempObject;
								}

								selectedList->nextLink();
							}

							if (responder)
								responder->playConfirmSound();
						}
						attackMoveMode = false;
						cursorFrame = CURSOR_NORMAL;
					}
					else	//if not placingMode
					{
						if (fullscreenMode || (mouse->x < gameBarPos.x)) // it isn't on the gamebar
						{
							if (!selectionMode)	//if we have started the selection rectangle
							{
								selectionRect.x = mouse->x + dborder->minX;		//the starting point of the selection rectangele
								selectionRect.y = mouse->y + dborder->minY;
							}
							selectionMode = true;
						}
						else if (!selectionMode)
						{
							int barMousePosX = mouse->x - gameBarPos.x,
								barMousePosY = mouse->y;
								
							if ((barMousePosX >= radarOffsetX) && (barMousePosY >= radarOffsetY) && (barMousePosX < (radarOffsetX + radarSizeX)) && (barMousePosY < (radarOffsetY + radarSizeY)))  
								//if on the radar
							{
																
																
								dborder->minX = (((barMousePosX - radarOffsetX)*BLOCKSIZE - gameBarPos.x/2 + dborder->radarMinX*BLOCKSIZE)/BLOCKSIZE)*BLOCKSIZE;
								dborder->maxX = dborder->minX + gameBarPos.x - 1 + dborder->radarMinX*BLOCKSIZE; //adds half the number of blocks that you would see to the middle
								dborder->minY = (((barMousePosY - radarOffsetY)*BLOCKSIZE - screen->h/2 + dborder->radarMinY*BLOCKSIZE)/BLOCKSIZE)*BLOCKSIZE;
								dborder->maxY = dborder->minY + screen->h-1;

								int over;
								if (dborder->minX < 0)
								{
									over = dborder->minX;
									dborder->minX = 0;
									dborder->maxX -= over;
								}
								else if (dborder->maxX >= (map->sizeX*BLOCKSIZE))
								{
									over = dborder->maxX - (map->sizeX*BLOCKSIZE-1);
									dborder->maxX = map->sizeX*BLOCKSIZE-1;
									dborder->minX -= over;
								}
								/*
								if (dborder->minY < 0)
								{
									over = dborder->minY;

									//altered
									//dborder->minY = 0;
									dborder->maxY -= over;
								}*/
								if(dborder->minY < 0)
								{
									if (dborder->minY < (0 - topBarPos.h + TOPBAR_MASKED_AREA_HEIGHT) && !fullscreenMode)
									{
										over = dborder->minY;
										dborder->minY = (0 - topBarPos.h + TOPBAR_MASKED_AREA_HEIGHT);
										dborder->maxY -= over;
									}
									else
									{
										over = dborder->minY;
										dborder->minY = 0;
										dborder->maxY -= over;
									}
								}
								else if (dborder->maxY >= (map->sizeY*BLOCKSIZE))
								{
									over = dborder->maxY - (map->sizeY*BLOCKSIZE-1);
									dborder->maxY = map->sizeY*BLOCKSIZE-1;
									dborder->minY -= over;
								}

								if (dborder->minX/BLOCKSIZE < dborder->radarMinX)
								{
									dborder->radarMinX = dborder->minX/BLOCKSIZE;
									dborder->radarMaxX = dborder->radarMinX + radarSizeX;
								}
								else if (dborder->maxX/BLOCKSIZE >= dborder->radarMaxX)
								{
									dborder->radarMaxX = dborder->maxX/BLOCKSIZE+1;
									dborder->radarMinX = dborder->radarMaxX - radarSizeX;
								}
								if (dborder->minY/BLOCKSIZE < dborder->radarMinY)
								{
								//	dborder->radarMinY = dborder->minY/BLOCKSIZE;
								//	dborder->radarMaxY = dborder->radarMinY + radarSizeY;

									if(dborder->minY >= 0)
										dborder->radarMinY = dborder->minY/BLOCKSIZE;
									else
										dborder->radarMinY = 0;
								}
								else if (dborder->maxY/BLOCKSIZE >= dborder->radarMaxY)
								{
									dborder->radarMaxY = dborder->maxY/BLOCKSIZE+1;
									dborder->radarMinY = dborder->radarMaxY - radarSizeY;
								}

																								
								radarMode = true;	//stay within the box
								
							}
							else if (!selectedList->isEmpty() && ((ObjectClass*)selectedList->getFirstElement())->hasInterface() && (((((ObjectClass*)selectedList->getFirstElement()))->getOwner() == thisPlayer)/* || debug*/))
								((ObjectClass*)selectedList->getFirstElement())->getInterface()->handlePress(mouse->x, mouse->y);
						}
					}
					break;
				}	//end of SDL_BUTTON_LEFT
				case (SDL_BUTTON_MIDDLE):
				{
					//if (!radarMode && !attackMoveMode && !selectedList->isEmpty() && ((ObjectClass*)selectedList->getFirstElement())->hasInterface() && (((((ObjectClass*)selectedList->getFirstElement()))->getOwner() == thisPlayer)
					//	((ObjectClass*)selectedList->getFirstElement())->getInterface()->isWaitingToPlace();
				}
				case (SDL_BUTTON_RIGHT):	//if the right mouse button is pressed
				{
					int barMousePosX = mouse->x - gameBarPos.x, //screen height because the bar's xPos is at screen height
						barMousePosY = mouse->y;

					if (attackMoveMode)
					{	//cancel attackmove
						attackMoveMode = false;
						cursorFrame = CURSOR_NORMAL;
					}
					else if ((!selectedList->isEmpty()	//if user has a controlable unit selected
						&& (((static_cast<ObjectClass*>(selectedList->getFirstElement()))->getOwner() == thisPlayer)/* || debug*/))
						&& (fullscreenMode		//and if mouse is on the game map
							|| (mouse->x < gameBarPos.x)
							|| ((barMousePosX >= radarOffsetX)	//or on the radar
								&& (barMousePosY >= radarOffsetY)
								&& (barMousePosX < (radarOffsetX + radarSizeX))
								&& (barMousePosY < (radarOffsetY + radarSizeY)))))
					{
						int	xPos,  //coordinate on map
							yPos;

						if (fullscreenMode || (mouse->x < gameBarPos.x)) //if mouse isn't on the gamebar
						{
							xPos = (mouse->x + dborder->minX)/BLOCKSIZE; //xPos is calculated normally
							yPos = (mouse->y + dborder->minY)/BLOCKSIZE;



							indicatorX = mouse->x + dborder->minX,

							indicatorY = mouse->y + dborder->minY;
						}
						else if ((barMousePosX >= radarOffsetX) && (barMousePosY >= radarOffsetY) && (barMousePosX < (radarOffsetX + radarSizeX)) && (barMousePosY < (radarOffsetY + radarSizeY)))  //if on the radar
						{
							xPos = barMousePosX - radarOffsetX  + dborder->radarMinX, //screen height because the bar's x is at screen height
							yPos = barMousePosY - radarOffsetY  + dborder->radarMinY; //we are calculating coord from on map

							indicatorX = xPos*BLOCKSIZE + BLOCKSIZE/2;

							indicatorY = yPos*BLOCKSIZE + BLOCKSIZE/2;
						}

						indicatorFrame = 0;

						/*if (debug)
						{	//show click pos
							char tempL[50];
							sprintf(tempL, "%d, %d", xPos, yPos);
							showMessage(tempL);
						}*/
			
						//let unit handle right click on map or target
						double speedCap = NONE;
						ObjectClass	*responder = NULL,
									*tempObject;
						selectedList->reset();
						while(selectedList->currentNotNull())
						{
							tempObject = (ObjectClass*)selectedList->getCurrent();
							tempObject->handleActionCommand(xPos, yPos);

							//if this object obey the command
							if ((responder == NULL) && tempObject->isRespondable())
								responder = tempObject;

							//find slowest speed of all units
							if (shift && tempObject->isAUnit() && ((speedCap < 0) || (((UnitClass*)tempObject)->getMaxSpeed() < speedCap)))
								speedCap = ((UnitClass*)tempObject)->getMaxSpeed();

							selectedList->nextLink();
						}
						if (responder)
							responder->playConfirmSound();

						selectedList->reset();
						while(selectedList->currentNotNull())
						{
							tempObject = (ObjectClass*)selectedList->getCurrent();
							if (tempObject->isAUnit())
								((UnitClass*)tempObject)->setSpeedCap(speedCap);

							selectedList->nextLink();
						}
					}
					else if (!selectedList->isEmpty() && ((ObjectClass*)selectedList->getFirstElement())->hasInterface() && (((((ObjectClass*)selectedList->getFirstElement()))->getOwner() == thisPlayer)/* || debug*/))
						((ObjectClass*)selectedList->getFirstElement())->getInterface()->handleCancelPress(mouse->x, mouse->y);
					placingMode = false;
					break;
				}	//end of SDL_BUTTON_RIGHT
				case (4):	//if the mouse wheel is rolled up
				{
					if ((selectedList->getNumElements() == 1)
						&& ((((ObjectClass*)selectedList->getFirstElement())->getOwner() == thisPlayer)/* || debug*/)
						&& (static_cast<ObjectClass*>(selectedList->getFirstElement()))->isAStructure()
						&& ((static_cast<StructureClass*>(selectedList->getFirstElement()))->isABuilder()))
					{
						BuilderClass* builder = static_cast<BuilderClass*>(selectedList->getFirstElement());
						BuilderClass::buttonUp(builder);
						soundPlayer->playSfxSound(Sound_buttonClick);

					}
					break;
				}
				case (5):	//if the mouse wheel is rolled down
				{
					if (((selectedList->getNumElements() == 1) 
						&& ((((ObjectClass*)selectedList->getFirstElement())->getOwner() == thisPlayer)/* || debug*/)
						&& (static_cast<ObjectClass*>(selectedList->getFirstElement()))->isAStructure() 
						&& ((static_cast<StructureClass*>(selectedList->getFirstElement()))->isABuilder())))
					{
						BuilderClass* builder = static_cast<BuilderClass*>(selectedList->getFirstElement());
						BuilderClass::buttonDown(builder);
						soundPlayer->playSfxSound(Sound_buttonClick);
					}					
					break;
				}
				break;
			}
		}
		case SDL_MOUSEMOTION:
		{
			SDL_MouseMotionEvent* mouse = &event.motion;
			if (menuMode)
				menuInterface->handleMotion(mouse->x, mouse->y);
			else
			{
				moveDownMode =  (mouse->y >= screen->h-1);	//if user is trying to move view down
				moveLeftMode = (mouse->x <= 0);	//if user is trying to move view left
				moveRightMode = (mouse->x >= screen->w-1);	//if user is trying to move view right
				moveUpMode = (mouse->y <= 0);	//if user is trying to move view up



				if (radarMode)

				{

					int barMousePosX = mouse->x - gameBarPos.x, //screen height because the bar's xPos is at screen height
						barMousePosY = mouse->y;

					if (barMousePosX < radarOffsetX)
						barMousePosX = radarOffsetX;
					if (barMousePosY < radarOffsetY)
						barMousePosY = radarOffsetY;
								
					dborder->minX = (((barMousePosX - radarOffsetX)*BLOCKSIZE - gameBarPos.x/2 + dborder->radarMinX*BLOCKSIZE)/BLOCKSIZE)*BLOCKSIZE;
					dborder->maxX = dborder->minX + gameBarPos.x - 1 + dborder->radarMinX*BLOCKSIZE; //adds half the number of blocks that you would see to the middle
					dborder->minY = (((barMousePosY - radarOffsetY)*BLOCKSIZE - screen->h/2 + dborder->radarMinY*BLOCKSIZE)/BLOCKSIZE)*BLOCKSIZE;
					dborder->maxY = dborder->minY + screen->h-1;

					int over;
					if (dborder->minX < 0)
					{
						over = dborder->minX;
						dborder->minX = 0;
						dborder->maxX -= over;
					}
					else if (dborder->maxX >= (map->sizeX*BLOCKSIZE))
					{
						over = dborder->maxX - (map->sizeX*BLOCKSIZE-1);
						dborder->maxX = map->sizeX*BLOCKSIZE-1;
						dborder->minX -= over;
					}
					//if (dborder->minY < 0)
					if(dborder->minY < 0)
					{
						if (dborder->minY < (0 - topBarPos.h + TOPBAR_MASKED_AREA_HEIGHT) && !fullscreenMode)
						{
							over = dborder->minY;
							dborder->minY = (0 - topBarPos.h + TOPBAR_MASKED_AREA_HEIGHT);
							dborder->maxY -= over;
						}
						else
						{
							over = dborder->minY;
							dborder->minY = 0;
							dborder->maxY -= over;
						}
					}
					else if (dborder->maxY >= (map->sizeY*BLOCKSIZE))
					{
						over = dborder->maxY - (map->sizeY*BLOCKSIZE-1);
						dborder->maxY = map->sizeY*BLOCKSIZE-1;
						dborder->minY -= over;
					}

					if (dborder->minX/BLOCKSIZE < dborder->radarMinX)
					{
						dborder->radarMinX = dborder->minX/BLOCKSIZE;
						dborder->radarMaxX = dborder->radarMinX + radarSizeX;
					}
					else if (dborder->maxX/BLOCKSIZE >= dborder->radarMaxX)
					{
						dborder->radarMaxX = dborder->maxX/BLOCKSIZE+1;
						dborder->radarMinX = dborder->radarMaxX - radarSizeX;
					}
					if (dborder->minY/BLOCKSIZE < dborder->radarMinY)
					{
						if(dborder->minY >= 0)
							dborder->radarMinY = dborder->minY/BLOCKSIZE;
						else
							dborder->radarMinY = 0;
				
						dborder->radarMaxY = dborder->radarMinY + radarSizeY;
					}
					else if (dborder->maxY/BLOCKSIZE >= dborder->radarMaxY)
					{
						dborder->radarMaxY = dborder->maxY/BLOCKSIZE+1;
						dborder->radarMinY = dborder->radarMaxY - radarSizeY;
					}

				}
				if (!selectedList->isEmpty() && ((ObjectClass*)selectedList->getFirstElement())->hasInterface() && (((((ObjectClass*)selectedList->getFirstElement()))->getOwner() == thisPlayer)/* || debug*/))
					((ObjectClass*)selectedList->getFirstElement())->getInterface()->handleMotion(mouse->x, mouse->y);
			}
			
			drawnMouseX = mouse->x;
			drawnMouseY = mouse->y;
			break;
		}
		case SDL_MOUSEBUTTONUP:
		{
			SDL_MouseButtonEvent* mouse = &event.button;
			if ((mouse->button == SDL_BUTTON_LEFT) || (mouse->button == SDL_BUTTON_RIGHT))
			{
				//TEMP TEMP
				if(!menuMode && !showMentat)
				{
					if (mouse->button == SDL_BUTTON_LEFT)
							mainButtonsBox.handleRelease(mouse->x, mouse->y);
					else if (mouse->button == SDL_BUTTON_RIGHT)
							mainButtonsBox.handleCancelRelease(mouse->x, mouse->y);
				}

				if (menuMode)
				{
					if (mouse->button == SDL_BUTTON_LEFT)
						menuInterface->handleRelease(mouse->x, mouse->y);
					else if (mouse->button == SDL_BUTTON_RIGHT)
						menuInterface->handleCancelRelease(mouse->x, mouse->y);
				}
				else if(showMentat)
				{
					if (mouse->button == SDL_BUTTON_LEFT)
						mentat->handleRelease(mouse->x, mouse->y);
					else if (mouse->button == SDL_BUTTON_RIGHT)
						mentat->handleCancelRelease(mouse->x, mouse->y);
				}
				else if (selectionMode && (mouse->button == SDL_BUTTON_LEFT))
				{
					int finalMouseX = mouse->x,	//this keeps the box on the map, and not over game bar

						finalMouseY = mouse->y;
					if (!fullscreenMode && (finalMouseX >= gameBarPos.x))
						finalMouseX = gameBarPos.x-1;



					int rectFinishX = (finalMouseX + dborder->minX)/BLOCKSIZE;
					if (rectFinishX > (map->sizeX-1))
						rectFinishX = map->sizeX-1;
					int rectFinishY = (mouse->y + dborder->minY)/BLOCKSIZE;
					int rectStartX = selectionRect.x/BLOCKSIZE,
						rectStartY = selectionRect.y/BLOCKSIZE;


					map->selectObjects(thisPlayer->getPlayerNumber(), rectStartX, rectStartY, rectFinishX, rectFinishY, finalMouseX + dborder->minX, finalMouseY + dborder->minY, shift);

					

					

					checkSelectionInterface();

				}

				else if (!selectedList->isEmpty() && ((ObjectClass*)selectedList->getFirstElement())->hasInterface() && (((((ObjectClass*)selectedList->getFirstElement()))->getOwner() == thisPlayer)/* || debug*/))
				{
					if (mouse->button == SDL_BUTTON_LEFT)
						((ObjectClass*)selectedList->getFirstElement())->getInterface()->handleRelease(mouse->x, mouse->y);
					else if (mouse->button == SDL_BUTTON_RIGHT)
						((ObjectClass*)selectedList->getFirstElement())->getInterface()->handleCancelRelease(mouse->x, mouse->y);
				}


				radarMode = selectionMode = false;

			}

			break;
		}
		case (SDL_QUIT):
			quitGame = true;
			break;
		default:
			break;
    	}
	}

	if (moveLeftMode)
	{
		if (dborder->minX > 0)
		{
			dborder->minX--;
			dborder->maxX--;
			if ((dborder->minX < dborder->radarMinX*BLOCKSIZE) && (dborder->radarMinX > 0))
			{
				dborder->radarMinX--;
				dborder->radarMaxX--;
			}

			cursorFrame = CURSOR_LEFT;
		}
		else
			moveLeftMode = false;
	}
	else if (moveRightMode)
	{
		if (dborder->maxX < map->sizeX*BLOCKSIZE-1)
		{
			dborder->minX++;
			dborder->maxX++;
			if ((dborder->maxX > dborder->radarMaxX*BLOCKSIZE) && (dborder->radarMaxX < map->sizeX))
			{
				dborder->radarMinX++;
				dborder->radarMaxX++;
			}

			cursorFrame = CURSOR_RIGHT;
		}
		else
			moveRightMode = false;
	}

	if (moveDownMode)
	{
		if (dborder->maxY < map->sizeY*BLOCKSIZE-1)
		{
			dborder->minY++;
			dborder->maxY++;
			if ((dborder->maxY > dborder->radarMaxY*BLOCKSIZE) && (dborder->radarMaxY < map->sizeY))
			{
				dborder->radarMinY++;
				dborder->radarMaxY++;
			}

			cursorFrame = CURSOR_DOWN;
		}
		else
			moveDownMode = false;
	}
	else if (moveUpMode)
	{
		//if (dborder->minY > 0)
		if (dborder->minY > (0 - topBarPos.h + TOPBAR_MASKED_AREA_HEIGHT) && !fullscreenMode)
		{
			dborder->minY--;
			dborder->maxY--;
			
			if ((dborder->minY < dborder->radarMinY*BLOCKSIZE) && (dborder->radarMinY > 0))
			{
				dborder->radarMinY--;
				dborder->radarMaxY--;
			}

			cursorFrame = CURSOR_UP;
		}
		else if (dborder->minY > 0)
		{
			dborder->minY--;
			dborder->maxY--;
			if ((dborder->minY < dborder->radarMinY*BLOCKSIZE) && (dborder->radarMinY > 0))
			{
				dborder->radarMinY--;
				dborder->radarMaxY--;
			}

			cursorFrame = CURSOR_UP;
		}
		else
			moveUpMode = false;
	}

	if (!moveLeftMode && !moveRightMode && !moveUpMode && !moveDownMode)
	{
		if (attackMoveMode)
			cursorFrame = CURSOR_TARGET;
		else
			cursorFrame = CURSOR_NORMAL;
	}
}

/////////////////////process Objects
void processObjects()
{
	int itemID;
	try
	{
		if (!structureList->isEmpty())
		{
			StructureClass *tempStructure;

			structureList->reset();
			while(structureList->currentNotNull())
			{
				tempStructure = (StructureClass*)structureList->getCurrent();
				itemID = tempStructure->getItemID();

				if(tempStructure->isABuilder())
				{
					BuilderClass *tempBuilder = (BuilderClass *)tempStructure;
					tempBuilder->update();
				}
				else
				{
					tempStructure->update();
				}

				if (tempStructure->outOfRange())
					delete ((StructureClass*)structureList->removeCurrent());
				else
					structureList->nextLink();
			}
		}
	}
	catch (...)
	{
		fprintf(stderr, "Argghhh. Exception caught: structure update loop, type %d\n", itemID);
	}

	if (placingMode && selectedList->isEmpty())
		placingMode = false;

	try
	{
		if (!unitList->isEmpty())
		{
			UnitClass *tempUnit;

			unitList->reset();
			while(unitList->currentNotNull())
			{
				tempUnit = (UnitClass*)unitList->getCurrent();
				itemID = tempUnit->getItemID();
				tempUnit->update();

				if (tempUnit->outOfRange())
					delete ((UnitClass*)unitList->removeCurrent());
				else
					unitList->nextLink();
			}
		}
	}
	catch (...)
	{
		fprintf(stderr, "Argghhh. Exception caught: unit update loop, type %d\n", itemID);
	}

	try
	{
		if (!bulletList->isEmpty())
		{
			BulletClass *tempBullet;

			bulletList->reset();
			while(bulletList->currentNotNull())
			{
				tempBullet = (BulletClass*)bulletList->getCurrent();
				itemID = tempBullet->getItemID();
				tempBullet->updatePosition();

				if (tempBullet->outOfRange())
					delete ((BulletClass*)bulletList->removeCurrent());
				else
					bulletList->nextLink();
			}
		}
	}
	catch (...)
	{
		fprintf(stderr, "Argghhh. Exception caught: bullet update loop, type %d\n", itemID);
	}
}

void drawCursor()
{
	SDL_Rect dest, src;

	SDL_Surface* surface = (SDL_Surface*)dataFile[UI_Cursor].dat;

	

	dest.x = drawnMouseX;

	dest.y = drawnMouseY;

	dest.w = src.w = surface->w/NUM_CURSORS;

	dest.h = src.h = surface->h;

	src.x = src.w * cursorFrame;

	src.y = 0;



	//reposition image so pointing on right spot

	if (cursorFrame == CURSOR_RIGHT)

		dest.x -= dest.w/2;

	else if (cursorFrame == CURSOR_DOWN)

		dest.y -= dest.h/2;

	if ((cursorFrame == CURSOR_TARGET) || (cursorFrame == CURSOR_SELECTION))

	{

		dest.x -= dest.w/2;

		dest.y -= dest.h/2;

	}



	SDL_BlitSurface(surface, &src, screen, &dest);
}

void drawScreen()
{

////////////draw terrain
	int xCount, yCount,

		xCount2 = 0,

		yCount2 = 0,

		xOffset = (dborder->minX/BLOCKSIZE)*BLOCKSIZE - dborder->minX,

		yOffset = (dborder->minY/BLOCKSIZE)*BLOCKSIZE - dborder->minY;

	

	SDL_Rect	source,

				drawLocation;


	//debugCount = 0;
	TerrainClass* cell;

	for (yCount = dborder->minY/BLOCKSIZE; yCount <= (dborder->maxY/BLOCKSIZE); yCount++)

	{

		for (xCount = dborder->minX/BLOCKSIZE; xCount <= (dborder->maxX/BLOCKSIZE); xCount++)
		{
			if (map->cellExists(xCount, yCount))
			{
				cell = &map->cell[xCount][yCount];
				
				if (debug || !shade || cell->isExplored(thisPlayer->getPlayerNumber()))

					cell->blitToScreen(xCount2*BLOCKSIZE + xOffset, yCount2*BLOCKSIZE + yOffset);
			}
			xCount2++;
		}
		yCount2++;
		xCount2 = 0;
	}
	/*char temp[30];
	sprintf(temp, "debugCount:%d", debugCount);
	showMessage(temp);*/

/////////////////draw units

	if (!undergroundUnitBlitList->isEmpty())
	{
		undergroundUnitBlitList->reset();
		while(undergroundUnitBlitList->currentNotNull())
			((UnitClass*)undergroundUnitBlitList->removeCurrent())->blitToScreen();
	}



	if (!infantryBlitList->isEmpty())

	{

		infantryBlitList->reset();

		while(infantryBlitList->currentNotNull())

			((ObjectClass*)infantryBlitList->removeCurrent())->blitToScreen();

	}



	if (!groundUnitBlitList->isEmpty())

	{

		groundUnitBlitList->reset();

		while(groundUnitBlitList->currentNotNull())

			((UnitClass*)groundUnitBlitList->removeCurrent())->blitToScreen();

	}



	if (!airUnitBlitList->isEmpty())

	{

		airUnitBlitList->reset();

		while(airUnitBlitList->currentNotNull())

			((UnitClass*)airUnitBlitList->removeCurrent())->blitToScreen();

	}

	if (!bulletList->isEmpty())
	{
		BulletClass* tempBullet;

		bulletList->reset();
		while(bulletList->currentNotNull())
		{
			tempBullet = (BulletClass*)bulletList->getCurrent();
			if ((((tempBullet->getRealX() + tempBullet->getImageW()/2) >= dborder->minX)
				&& ((tempBullet->getRealX() - tempBullet->getImageW()/2) <= dborder->maxX)
				&& ((tempBullet->getRealY() + tempBullet->getImageH()/2) >= dborder->minY)
				&& ((tempBullet->getRealY() - tempBullet->getImageH()/2) <= dborder->maxY))
				&& (debug || !shade 
				|| map->cell[(int)tempBullet->getRealX()/BLOCKSIZE][(int)tempBullet->getRealY()/BLOCKSIZE].isExplored(thisPlayer->getPlayerNumber()
				|| !map->cell[(int)tempBullet->getRealX()/BLOCKSIZE][(int)tempBullet->getRealY()/BLOCKSIZE].isFogged(thisPlayer->getPlayerNumber()))))
				tempBullet->blitToScreen();

			bulletList->nextLink();
		}
	}

	//draw selected things health and box etc
	if (!selectionBlitList->isEmpty())
	{
		ObjectClass* selectable;

		selectionBlitList->reset();

		while(selectionBlitList->currentNotNull())

		{

			selectable = (ObjectClass*)selectionBlitList->removeCurrent();
			if (selectable->isAStructure())
				((StructureClass*)selectable)->drawSelectRect();
			else
				((UnitClass*)selectable)->drawSelectionBox();
		}
	}

//////////////////////////////draw unexplored/shade

	if (shade && !debug)
	{
		xCount2 = 0,
		yCount2 = 0;
		source.y = 0;
		source.w = source.h = drawLocation.w = drawLocation.h = BLOCKSIZE;
		for (xCount = dborder->minX/BLOCKSIZE; xCount <= dborder->maxX/BLOCKSIZE; xCount++)
		{
			for (yCount = dborder->minY/BLOCKSIZE; yCount <= dborder->maxY/BLOCKSIZE; yCount++)
			{
				if ((xCount >= 0) && (xCount < map->sizeX) && (yCount >= 0) && (yCount < map->sizeY))
				{
					cell = &map->cell[xCount][yCount];
					if (cell->isExplored(thisPlayer->getPlayerNumber()))
					{
						if (cell->isNextToHidden())
						{
							source.x = cell->getHideTile()*BLOCKSIZE;
							drawLocation.x = xCount2*BLOCKSIZE + xOffset;
							drawLocation.y = yCount2*BLOCKSIZE + yOffset;
							SDL_BlitSurface(hiddenSurf, &source, screen, &drawLocation);			  								 
						}
						if(cell->isNextToFogged())
						{
							
							
							source.x = cell->getFogTile()*BLOCKSIZE;
							drawLocation.x = xCount2*BLOCKSIZE + xOffset;
							drawLocation.y = yCount2*BLOCKSIZE + yOffset;
							
					    	SDL_Rect mini;
							mini.x = mini.y  = 0;
							mini.h = mini.w = 1;
							SDL_Rect drawLoc = drawLocation;
							drawLoc.h = drawLoc.w = 0;
						
							
							
							for (int x=0;x<BLOCKSIZE; x++)
							 for (int y=0;y<BLOCKSIZE; y++)
							 	
								if(fogMAP[source.x+x][source.y+y] == 12)
								{
									drawLoc.x = drawLocation.x + x;
									drawLoc.y = drawLocation.y + y;
									SDL_BlitSurface(fogSurf,&mini,screen,&drawLoc);
								};
									
						}
						
					}
					else
					{
						source.x = 0;
						drawLocation.x = xCount2*BLOCKSIZE + xOffset;
						drawLocation.y = yCount2*BLOCKSIZE + yOffset;
						SDL_BlitSurface(hiddenSurf, &source, screen, &drawLocation);
					}
				}
				yCount2++;
			}
			xCount2++;
			yCount2 = 0;
		}
	}

/////////////draw placement position	
	
	int mouse_x, mouse_y;

	SDL_GetMouseState(&mouse_x, &mouse_y);



	if (placingMode)	//if user has selected to place a structure
	{

		if (fullscreenMode || (mouse_x < gameBarPos.x))	//if mouse is not over game bar
		{
			int	xPos = (mouse_x + dborder->minX)/BLOCKSIZE,
				yPos = (mouse_y + dborder->minY)/BLOCKSIZE;

			bool withinRange = false;
			int i, j;				
			BuilderClass* builder = static_cast<BuilderClass*>(selectedList->getFirstElement());
			BuildItemClass* placeItem = static_cast<BuildItemClass*>(builder->getPlaceItem());
			SDL_Surface	*image,
						*validPlace = (SDL_Surface*)(dataFile[UI_ValidPlace].dat),
						*invalidPlace = (SDL_Surface*)(dataFile[UI_InvalidPlace].dat);

			for (i = xPos; i < (xPos + placeItem->getStructureSizeX()); i++)
				for (j = yPos; j < (yPos + placeItem->getStructureSizeY()); j++)
					if (map->isWithinBuildRange(i, j, builder->getOwner()))
						withinRange = true;			//find out if the structure is close enough to other buildings

			for (i = xPos; i < (xPos + placeItem->getStructureSizeX()); i++)
				for (j = yPos; j < (yPos + placeItem->getStructureSizeY()); j++)
				{
					if (!withinRange || !map->cellExists(i,j) || !map->getCell(i,j)->isRock() || map->getCell(i,j)->isMountain() || map->getCell(i,j)->hasAGroundObject())
						image = invalidPlace;
					else
						image = validPlace;

					drawLocation.x = i*BLOCKSIZE - dborder->minX;
					drawLocation.y = j*BLOCKSIZE - dborder->minY;

					SDL_BlitSurface(image, NULL, screen, &drawLocation);
				}
		}
	}

///////////draw game selection rectangle
	if (selectionMode)
	{
		int finalMouseX = mouse_x;  //this keeps the box on the map, and not over game bar
		if (!fullscreenMode && (finalMouseX >= gameBarPos.x))
			finalMouseX = gameBarPos.x-1;
		drawrect(screen,  selectionRect.x - dborder->minX,  selectionRect.y - dborder->minY, finalMouseX, mouse_y, COLOUR_WHITE); //the mouse selection rectangle
	}



///////////draw action indciator

	if ((indicatorFrame != NONE) && (indicatorX >= dborder->minX) && (indicatorX <= dborder->maxX) && (indicatorY >= dborder->minY) && (indicatorY <= dborder->maxY))

	{

		source.w = drawLocation.w = ((SDL_Surface *)dataFile[UI_Indicator].dat)->w/3;

		source.h = drawLocation.h = ((SDL_Surface *)dataFile[UI_Indicator].dat)->h;

		source.x = indicatorFrame*source.w;

		source.y = 0;

		drawLocation.x = indicatorX - dborder->minX - source.w/2;

		drawLocation.y = indicatorY - dborder->minY - source.h/2;

		SDL_BlitSurface((SDL_Surface *)dataFile[UI_Indicator].dat, &source, screen, &drawLocation);

	}


///////////draw game bar
	SDL_Surface* surface;
	if (!fullscreenMode)
	{
		//SDL_BlitSurface((SDL_Surface *)dataFile[UI_GameBar2].dat, NULL, screen, &gameBarPos);
		SDL_BlitSurface(gamebarSurface, NULL, screen, &gameBarPos);
		
		if (!SDL_MUSTLOCK(screen) || (SDL_LockSurface(screen) == 0))
		{
			//draw power level indicator
			if (thisPlayer->getPowerRequirement() == 0)
			{
				if (thisPlayer->getPower() > 0)
					yCount2 = powerIndicatorPos.h + 1;
				else
					yCount2 = powerIndicatorPos.h/2;
			}
			else
				yCount2 = round((double)thisPlayer->getPower()/(double)thisPlayer->getPowerRequirement()*(double)powerIndicatorPos.h/2);
			if (yCount2 > powerIndicatorPos.h + 1)
				yCount2 = powerIndicatorPos.h + 1;

			for (yCount = 0; yCount < yCount2; yCount++)
			for (xCount = 1; xCount < powerIndicatorPos.w - 1; xCount++)
				putpixel(screen, xCount + powerIndicatorPos.x + gameBarPos.x, powerIndicatorPos.y + powerIndicatorPos.h - yCount, COLOUR_GREEN);

			//draw spice level indicator
			if (thisPlayer->getCapacity() == 0)
				yCount2 = 0;
			else
				yCount2 = round((double)thisPlayer->getStoredCredits()/(double)thisPlayer->getCapacity()*(double)spiceIndicatorPos.h);
			if (yCount2 > spiceIndicatorPos.h + 1)
				yCount2 = spiceIndicatorPos.h + 1;

			for (yCount = 0; yCount < yCount2; yCount++)
			for (xCount = 1; xCount < spiceIndicatorPos.w - 1; xCount++)
				putpixel(screen, xCount + spiceIndicatorPos.x + gameBarPos.x, spiceIndicatorPos.y + spiceIndicatorPos.h - yCount, COLOUR_ORANGE);

			if (SDL_MUSTLOCK(screen))
				SDL_UnlockSurface(screen);
		}

		if (!selectedList->isEmpty() && ((ObjectClass*)selectedList->getFirstElement())->hasInterface() /* || debug*/)
		{
			if((((ObjectClass*)selectedList->getFirstElement()))->getOwner() == thisPlayer)
			{
				unitButton_Aggressive.setVisible(true);
				unitButton_Defensive.setVisible(true);
				unitButton_Scout.setVisible(true);
				unitButton_StandGround.setVisible(true);
			}
			else
			{
				unitButton_Aggressive.setVisible(false);
				unitButton_Defensive.setVisible(false);
				unitButton_Scout.setVisible(false);
				unitButton_StandGround.setVisible(false);
			}

			(((ObjectClass*)selectedList->getFirstElement()))->drawInterface();
			doUnitIcon();
		}

		if ((selectedList->getNumElements() == 1) && (((((ObjectClass*)selectedList->getFirstElement()))->getOwner() == thisPlayer)/* || debug*/) && (((ObjectClass*)selectedList->getFirstElement()))->isAStructure() && (((StructureClass*)selectedList->getFirstElement()))->isABuilder())
		{
			//SDL_BlitSurface((SDL_Surface *)dataFile[UI_GameBar2].dat, NULL, screen, &gameBarPos);
			((BuilderClass*)selectedList->getFirstElement())->drawItems(gameBarPos.x + BUILDERBUTTONSMINX, BUILDERBUTTONSMINY, gameBarPos.x + BUILDERBUTTONSMAXX, gameBarPos.x + BUILDERBUTTONSMAXY, BUILDERBUTTONSSPACING);
		}

		//draw credits

		char creditsBuffer[10];

		sprintf(creditsBuffer, "%d", thisPlayer->getAmountOfCredits());
		int digits = strlen(creditsBuffer);
		surface = (SDL_Surface*)dataFile[UI_CreditsDigits].dat;
		source.w = drawLocation.w = surface->w/10;
		source.h = drawLocation.h = surface->h;
		source.y = 0;
		drawLocation.y = creditsPos.y;
		for (int i=digits-1; i>=0; i--)
		{
			source.x = atoi(&creditsBuffer[i])*source.w;
			creditsBuffer[i] = '\0';	//so next round atoi only does one character
			drawLocation.x = creditsPos.x + (6 - digits + i)*10;
			SDL_BlitSurface(surface, &source, screen, &drawLocation);

		}
	}

/*
Raals extra menu trial
*/
	if (!fullscreenMode)
	{

		drawLocation.x = topBarPos.x;
		drawLocation.y = topBarPos.y;
		drawLocation.w = topBarPos.w;
		drawLocation.h = topBarPos.h - TOPBAR_MASKED_AREA_HEIGHT;
		
		SDL_FillRect(screen, &drawLocation, COLOUR_BLACK);
		SDL_BlitSurface(topBarSurface, NULL, screen, &topBarPos);
		mainButtonsBox.setVisible(!showMentat);
		mainButtonsBox.draw();
		
	}
/*end changes*/

/////////draw radar (if not any CommView visible)
if(commView)
{
	if (!commView->isActive())
	{
		delete commView;
		commView = 0;
	}
	else commView->draw();
}
else //draw radar
	  
	if (!fullscreenMode)
	{
	    
		// Lock the screen for direct access to the pixels
		if (!SDL_MUSTLOCK(screen) || (SDL_LockSurface(screen) == 0))
		{
			xCount2 = 0;
			yCount2 = 0;
			TerrainClass* tempTerrain;
			

			for (xCount = 0; xCount <  dborder->radarMaxX; xCount++)
			{
				for (yCount = 0; yCount <  dborder->radarMaxY; yCount++)
				{
					
					tempTerrain = &map->cell[xCount][yCount];
					/*selecting the right colour is now handled in terrainClass
					 *getRadarColour method*/
					putpixel(screen, gameBarPos.x + radarOffsetX + xCount2, radarOffsetY + yCount2, 
							tempTerrain->getRadarColour(thisPlayer->getPlayerNumber(),
							thisPlayer->hasRadarOn())	);	

					yCount2++;
				}
				xCount2++;
				yCount2 = 0;
			}
			if (!SDL_MUSTLOCK(screen) || (SDL_LockSurface(screen) == 0))
			if (SDL_MUSTLOCK(screen))
				SDL_UnlockSurface(screen);
		}
		
		//draw a rectangle on radar representing the view
		drawrect(screen, 
			gameBarPos.x + radarOffsetX + dborder->minX/BLOCKSIZE - dborder->radarMinX, 
			radarOffsetY + dborder->minY/BLOCKSIZE - dborder->radarMinY, 
			gameBarPos.x + radarOffsetX + dborder->maxX/BLOCKSIZE - dborder->radarMinX, 
			radarOffsetY + dborder->maxY/BLOCKSIZE - dborder->radarMinY, 
			COLOUR_WHITE); 
	}

////////draw messages

	
	int pos = 0;
	for(int count = MESSAGE_BUFFER_SIZE-1; count >= 0; count--)
	if (messageTimer[count] > 0)
	{
		surface = TTF_RenderText_Solid(font[16], messageBuffer[count], palette->colors[COLOUR_WHITE]);
		drawLocation.x = 0;
		drawLocation.y = TTF_FontHeight(font[16])*pos++;
		SDL_BlitSurface(surface, NULL, screen, &drawLocation);
		SDL_FreeSurface(surface);
		if (--messageTimer[count] <= 0)

			messageBuffer[count][0] = '\0';
	}



	//draw message currently creating

	if (messageMode)

	{

		if (strlen(typingMessage) > 0)

		{

			surface = TTF_RenderText_Solid(font[16], typingMessage, palette->colors[COLOUR_WHITE]);
			xCount = surface->w;	//store for later
			yCount = surface->h;
			drawLocation.x = (screen->w - surface->w)/2;
			drawLocation.y = (screen->h - TTF_FontHeight(font[16]))/2 + screen->h/5;
			SDL_BlitSurface(surface, NULL, screen, &drawLocation);
			SDL_FreeSurface(surface);

		}

		else

			xCount = 0;



		surface = TTF_RenderText_Solid(font[16], "_", palette->colors[COLOUR_WHITE]);

		drawLocation.x = (screen->w + xCount)/2;
		drawLocation.y = (screen->h - TTF_FontHeight(font[16]))/2 + screen->h/5;
		SDL_BlitSurface(surface, NULL, screen, &drawLocation);
		SDL_FreeSurface(surface);

	}

//DRAW MESSAGEBOX
	messageBox.draw();

	if (showFPS)

	{

		char	temp[30];



		sprintf(temp, "fps:%d, frameTimer:%d", round(fps), frameTimer);

		surface = TTF_RenderText_Solid(font[16], temp, palette->colors[COLOUR_WHITE]);
		drawLocation.x = screen->w - surface->w;
		drawLocation.y = 0;
		SDL_BlitSurface(surface, NULL, screen, &drawLocation);
		SDL_FreeSurface(surface);

	}



	if (!settings.update || settings.finished || menuMode)	//show ingame menu
	{
		if (menuMode)

			menuInterface->draw();

		else

		{

			char	*finishMessage[] = { "You Have Completed Your Mission.", "You Have Failed Your Mission." },

					*pausedMessage = "Paused",

					*message;



			if (settings.finished)

			{

				if (settings.won)

					message = finishMessage[0];

				else

					message = finishMessage[1];



				/*end timer in here*/



				endTimer--;

				

				if(endTimer <= 0)

				{

					finishedLevel = true;

				}

			}

			else

				message = pausedMessage;



			surface = TTF_RenderText_Solid(font[25], message, palette->colors[COLOUR_WHITE]);

			drawLocation.x = screen->w/2 - surface->w/2;

			drawLocation.y = screen->h/2 - surface->h/2;

			drawLocation.w = surface->w;

			drawLocation.h = surface->h;

			SDL_BlitSurface(surface, NULL, screen, &drawLocation);

			SDL_FreeSurface(surface);
			
		}
	}

	if(showMentat)
	{
		mentat->draw();
	}

	drawCursor();
}

void setupView()
{
	int i, j, count;

	//setup the radar

	radarSizeX = radarSizeY = 128;
	radarOffsetX = 14;
	radarOffsetY = 2;
	if (map->sizeX < 128)		//allow for small maps
		radarSizeX = map->sizeX;
	if (map->sizeY < 128)
		radarSizeY = map->sizeY;
	
	if (radarSizeX < 128)
		radarOffsetX += (128 - radarSizeX)/2;
	if (radarSizeY < 128)
		radarOffsetY += (128 - radarSizeY)/2;
	

	//setup start location/view
	i = j = count = 0;
	ObjectClass* object;
	unitList->reset();
	while(unitList->currentNotNull())
	{
		object = (ObjectClass*)unitList->getCurrent();
		if (object->getOwner() == thisPlayer)
		{
			i += object->getX();
			j += object->getY();
			count++;
		}
		unitList->nextLink();
	}//
	structureList->reset();
	while(structureList->currentNotNull())
	{
		object = (ObjectClass*)structureList->getCurrent();
		if (object->getOwner() == thisPlayer)
		{
			i += object->getX();
			j += object->getY();
			count++;
		}
		structureList->nextLink();
	}//

	if (count == 0)
	{
		i = map->sizeX*BLOCKSIZE/2-1;
		j = map->sizeY*BLOCKSIZE/2-1;
	}
	else
	{
		i = i*BLOCKSIZE/count;
		j = j*BLOCKSIZE/count;
	}

	dborder->minX = i - gameBarPos.x/2-1;
	if (dborder->minX < 0)
		dborder->minX = 0;
	dborder->maxX = dborder->minX + gameBarPos.x-1;
	if (dborder->maxX >= map->sizeX*BLOCKSIZE)
	{
		dborder->maxX = map->sizeX*BLOCKSIZE - 1;
		dborder->minX = dborder->maxX - gameBarPos.x+1;
		if (dborder->minX < 0)
			dborder->minX = 0;
	}

	//altered
	dborder->minY = j - screen->h/2-1;

	//dborder->minY = j - (screen->h - topBarPos.h)/2-1;//screen->h/2 - topBarPos.h -1;
   	if (dborder->minY < 0)
		dborder->minY = 0;
	dborder->maxY = dborder->minY + screen->h;
	//dborder->maxY = dborder->minY + (screen->h - topBarPos.h) -1;

	if (dborder->maxY >= map->sizeY*BLOCKSIZE)
	{
		dborder->maxY = map->sizeY*BLOCKSIZE - 1;
		dborder->minY = dborder->maxY - screen->h+1;
//		dborder->minY = dborder->maxY - screen->h - topBarPos.h +1;

		if (dborder->minY < 0)
			dborder->minY = 0;
	}

	//scrolling maps
	dborder->radarMinX = i/BLOCKSIZE - radarSizeX/BLOCKSIZE/2;
	if (dborder->radarMinX < 0)
		dborder->radarMinX = 0;
	dborder->radarMaxX = dborder->radarMinX + radarSizeX;
	if (dborder->radarMaxX >= map->sizeX)
	{
		dborder->radarMaxX = map->sizeX;
		dborder->radarMinX = dborder->radarMaxX - radarSizeX;
		if (dborder->radarMinX < 0)
			dborder->radarMinX = 0;
	}
	dborder->radarMinY = j/BLOCKSIZE - radarSizeY/BLOCKSIZE/2;
	if (dborder->radarMinY < 0)
		dborder->radarMinY = 0;
	dborder->radarMaxY = dborder->radarMinY + radarSizeY;

	if (dborder->radarMaxY >= map->sizeY)
	{
		dborder->radarMaxY = map->sizeY;
		dborder->radarMinY = dborder->radarMaxY - radarSizeY;
		if (dborder->radarMinY < 0)
			dborder->radarMinY = 0;
	}
}

void game_cancel()
{
	menuInterface = &menuWindow;
}

void game_saveEntry()
{
	char	description[20],
			filename[30],
			temp[256];
	int		i, j, x, y;
	int		magicNum = SAVEMAGIC;
	double	tempDouble;

	FILE	*fp;
	StructureClass* structure;
	UnitClass* unit;

	if ((saveGameSpot >= 0) && (saveGameSpot < NUM_SAVEGAMES))
	{
		sprintf(filename, "savegame%d.dls", saveGameSpot);

		fp = fopen(filename, "wb");
		if (fp)
		{
			//write the magic number
			fwrite(&magicNum, sizeof(int), 1, fp);

			//write the game description
			strncpy(description, gameSaveNameEntry.getText(), sizeof(description));
			fwrite(description, sizeof(description), 1, fp);

			//write the map size
			fwrite(&map->sizeX, sizeof(short), 1, fp);
			fwrite(&map->sizeY, sizeof(short), 1, fp);
			
			//write entire settinsg struct
			fwrite(&settings, sizeof(SETTINGSTYPE), 1, fp);

			//write the player(s) info
			for (i=0; i<MAX_PLAYERS; i++)
			{
				if (player[i] != NULL)
					j = 1;
				else
					j = 0;
				fwrite(&j, sizeof(int), 1, fp);
				if (j)
					player[i]->save(fp);
			}

			//write the game flags
			i = map->getWinFlags();
			fwrite(&i, sizeof(i), 1, fp);

			
			/*
			i = settings.techLevel;
			fwrite(&i, sizeof(i), 1, fp);
			i = settings.gameType;
			fwrite(&i, sizeof(i), 1, fp);
			*/
			i = fullscreenMode ? 1 : 0;
			fwrite(&i, sizeof(i), 1, fp);
			
			//char *buf = (char *)malloc(sizeof(SETTINGSTYPE));

			

			if (debug)
				i = 1;
			else
				i = 0;
			fwrite(&i, sizeof(i), 1, fp);

			/*
			if(settings.gameType == ORIGINAL)
			{
				i = settings.campaignLevel;
				fwrite(&i, sizeof(i), 1, fp);
			}*/

			//write the terrain info
			for (i=0; i<map->sizeX; i++)
			for (j=0; j<map->sizeY; j++)
			{
				x = map->getCell(i, j)->getType();
				y = map->getCell(i, j)->getTile();
				fwrite(&x, sizeof(int), 1, fp);
				fwrite(&y, sizeof(int), 1, fp);
				x = map->getCell(i, j)->getOwner();
				fwrite(&x, sizeof(int), 1, fp);
				for (y=0; y<MAX_PLAYERS; y++)
				{
					if (map->getCell(i, j)->isExplored(y))
						x = 1;
					else
						x = 0;
					fwrite(&x, sizeof(int), 1, fp);
				}
			}

			//write the structure info
			structureList->reset();
			x = structureList->getNumElements();
			fwrite(&x, sizeof(int), 1, fp);
			while(structureList->currentNotNull())
			{
				structure = (StructureClass*)structureList->getCurrent();
				if (!structure->wasDestroyed())
					x = structure->getObjectID();
				else
					x = NONE;
				fwrite(&x, sizeof(int), 1, fp);
				if (x != NONE)
				{
					x = structure->getOwner()->getPlayerNumber();
					fwrite(&x, sizeof(int), 1, fp);
					x = structure->getItemID();
					fwrite(&x, sizeof(int), 1, fp);
					x = structure->getLocation()->x;
					y = structure->getLocation()->y;
					fwrite(&x, sizeof(int), 1, fp);
					fwrite(&y, sizeof(int), 1, fp);
					x = structure->getHealth();
					fwrite(&x, sizeof(int), 1, fp);
					if ((structure->getItemID() == Structure_GunTurret)
						|| (structure->getItemID() == Structure_RocketTurret))
					{
						x = ((GunTurretClass*)structure)->getTurretAngle();
						fwrite(&x, sizeof(int), 1, fp);	
					}
					else if (structure->getItemID() == Structure_Palace)
					{
						x = ((PalaceClass*)structure)->getSpecialTimer();
						fwrite(&x, sizeof(int), 1, fp);
					}
					else if (structure->getItemID() == Structure_StarPort)
					{
						x = ((StarPortClass*)structure)->getArrivalTimer();
						fwrite(&x, sizeof(int), 1, fp);
					}
					x = structure->getDestination()->x;
					y = structure->getDestination()->y;
					fwrite(&x, sizeof(int), 1, fp);
					fwrite(&y, sizeof(int), 1, fp);
				}

				structureList->nextLink();
			}

			//write the unit info
			unitList->reset();
			x = unitList->getNumElements();
			fwrite(&x, sizeof(int), 1, fp);
			while(unitList->currentNotNull())
			{
				unit = (UnitClass*)unitList->getCurrent();
				if (!unit->wasDestroyed())
					x = unit->getObjectID();
				else
					x = NONE;
				fwrite(&x, sizeof(int), 1, fp);
				if (x != NONE)
				{
					x = unit->getRealOwner()->getPlayerNumber();
					fwrite(&x, sizeof(int), 1, fp);
					x = unit->getItemID();
					fwrite(&x, sizeof(int), 1, fp);
					x = unit->getX();
					y = unit->getY();
					fwrite(&x, sizeof(int), 1, fp);
					fwrite(&y, sizeof(int), 1, fp);
					x = unit->getAngle();
					fwrite(&x, sizeof(int), 1, fp);
					x = unit->getHealth();
					fwrite(&x, sizeof(int), 1, fp);
					if (unit->getItemID() == Unit_Harvester)
					{
						tempDouble = ((HarvesterClass*)unit)->getAmountOfSpice();
						fwrite(&tempDouble, sizeof(tempDouble), 1, fp);				
					}
					else if (unit->getItemID() == Unit_Sandworm)
					{
						x = ((Sandworm*)unit)->getSleepTimer();
						fwrite(&x, sizeof(x), 1, fp);				
					}
					if (unit->isTurreted())
					{
						x = ((TankClass*)unit)->getTurretAngle();
						fwrite(&x, sizeof(int), 1, fp);				
					}
					if (unit->wasDeviated())
						x = unit->getDeviationTime();
					else
						x = 0;
					fwrite(&x, sizeof(int), 1, fp);
					if (x)
					{
						x = unit->getOwner()->getPlayerNumber();
						fwrite(&x, sizeof(int), 1, fp);
					}
					x = (int)unit->getAttackMode();
					fwrite(&x, sizeof(int), 1, fp);
					if (unit->wasForced())
						x = 1;
					else
						x = 0;
					fwrite(&x, sizeof(int), 1, fp);
					if (unit->isAttacking())
						x = 1;
					else
						x = 0;
					fwrite(&x, sizeof(int), 1, fp);
					x = unit->getDestination()->x;
					y = unit->getDestination()->y;
					fwrite(&x, sizeof(int), 1, fp);
					fwrite(&y, sizeof(int), 1, fp);
				}

				unitList->nextLink();
			}

			//save object extra info
			x = structureList->getNumElements() + unitList->getNumElements();
			fwrite(&x, sizeof(int), 1, fp);

			structureList->reset();
			while(structureList->currentNotNull())
			{
				structure = (StructureClass*)structureList->getCurrent();
				
				if (!structure->wasDestroyed())
					x = structure->getObjectID();
				else
					x = NONE;
				fwrite(&x, sizeof(int), 1, fp);
				if (x != NONE)
				{
					i = structure->getX();
					j = structure->getY();

					fwrite(&i, sizeof(int), 1, fp);
					fwrite(&j, sizeof(int), 1, fp);

					if (structure->hasATarget() && !structure->getTarget()->wasDestroyed())
						x = structure->getTarget()->getObjectID();
					else
						x = NONE;
					fwrite(&x, sizeof(int), 1, fp);
					if (x != NONE)
					{
						i = structure->getTarget()->getX();
						j = structure->getTarget()->getY();

						fwrite(&i, sizeof(int), 1, fp);
						fwrite(&j, sizeof(int), 1, fp);
					}

					if (structure->getItemID() == Structure_Refinery)
					{
						if (!((RefineryClass*)structure)->isFree())
							x = ((RefineryClass*)structure)->getHarvester()->getObjectID();
						else
							x = NONE;
						fwrite(&x, sizeof(int), 1, fp);
					}
					else if (structure->getItemID() == Structure_RepairYard)
					{
						if (!((RepairYardClass*)structure)->isFree())
							x = ((RepairYardClass*)structure)->getRepairUnit()->getObjectID();
						else
							x = NONE;
						fwrite(&x, sizeof(int), 1, fp);
					}
					if (structure->isABuilder())
						((BuilderClass*)structure)->saveBuildList(fp);
				}

				structureList->nextLink();
			}
			
			unitList->reset();
			while(unitList->currentNotNull())
			{
				unit = (UnitClass*)unitList->getCurrent();
			
				if (!unit->wasDestroyed())
					x = unit->getObjectID();
				else
					x = NONE;
				fwrite(&x, sizeof(int), 1, fp);
				if (x != NONE)
				{
					i = unit->getX();
					j = unit->getY();

					fwrite(&i, sizeof(int), 1, fp);
					fwrite(&j, sizeof(int), 1, fp);

					if (unit->hasATarget() && !unit->getTarget()->wasDestroyed())
						x = unit->getTarget()->getObjectID();
					else
						x = NONE;
					fwrite(&x, sizeof(int), 1, fp);
					if (x != NONE)
					{
						i = unit->getTarget()->getX();
						j = unit->getTarget()->getY();

						fwrite(&i, sizeof(int), 1, fp);
						fwrite(&j, sizeof(int), 1, fp);
					}
					if ((unit->getItemID() == Unit_Carryall) || (unit->getItemID() == Unit_Frigate))
						((Carryall*)unit)->saveCargo(fp);
				}

				unitList->nextLink();
			}

			x = bulletList->getNumElements();
			fwrite(&x, sizeof(x), 1, fp);
			bulletList->reset();
			while(bulletList->currentNotNull())
			{
				((BulletClass*)bulletList->getCurrent())->save(fp);
				bulletList->nextLink();
			}

			//save selection lists
			ObjectClass *tempObject;



			x = selectedList->getNumElements();

			fwrite(&x, sizeof(int), 1, fp);

			selectedList->reset();

			while(selectedList->currentNotNull())

			{

				tempObject = (ObjectClass*)selectedList->getCurrent();

				x = tempObject->getObjectID();

				fwrite(&x, sizeof(int), 1, fp);

				selectedList->nextLink();

			}



			ListIterator* tempList;

			for (int listCount = 0; listCount < 9; listCount++)

			{

				tempList = &selectedLists[listCount];

				x = tempList->getNumElements();

				fwrite(&x, sizeof(int), 1, fp);



				tempList->reset();

				while(tempList->currentNotNull())

				{

					tempObject = (ObjectClass*)tempList->getCurrent();

					x = tempObject->getObjectID();

					fwrite(&x, sizeof(int), 1, fp);

					tempList->nextLink();

				}

			}

			//write the map/dborder info
			fwrite(&dborder->radarMinX, sizeof(int), 1, fp);
			fwrite(&dborder->radarMaxX, sizeof(int), 1, fp);
			fwrite(&dborder->radarMinY, sizeof(int), 1, fp);
			fwrite(&dborder->radarMaxY, sizeof(int), 1, fp);
			fwrite(&dborder->minX, sizeof(int), 1, fp);
			fwrite(&dborder->maxX, sizeof(int), 1, fp);
			fwrite(&dborder->minY, sizeof(int), 1, fp);
			fwrite(&dborder->maxY, sizeof(int), 1, fp);

			fclose(fp);
			sprintf(temp, "game saved to \"%s\".", filename);
			showMessage(temp);

			game_resume();
		}
		else
		{
			sprintf(temp, "Could not save game to \"%s\".", filename);
			showMessage(temp);
		}
	}

	gameSaveNameEntry.setEnabled(false);
	gameSaveNameEntry.setVisible(false);
	saveButton.setEnabled(false);
	saveButton.setVisible(false);
}

void game_save()
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

				gameSaveSpotButton[i].setText(description);

			}

			else

				gameSaveSpotButton[i].setText("(Empty)");



			fclose(fp);

		}

		else

			gameSaveSpotButton[i].setText("(Empty)");

	}

	gameSaveNameEntry.setEnabled(false);
	gameSaveNameEntry.setVisible(false);
	saveButton.setEnabled(false);
	saveButton.setVisible(false);
    menuInterface = &saveWindow;
}

void game_loadEntry(void* data)
{
	loadingGame = true;
	finishedLevel = true;
	gameNum = (int)data;

}

void game_loadEntryCallback(int num)
{
	char	description[20],
			filename[30],
			temp[256];
	short	mapSizeX, mapSizeY;
	int		i, j, x, y, itemID, magicNum, objectID, playerNum, colour, house;//,num;
	double	tempDouble;
	COORDTYPE		source, destination;
	FILE			*fp;
	BulletClass*	newBullet;
	UnitClass*		newUnit;
	StructureClass*	newStructure;
	ObjectClass		*newObject, *newTarget;

	//num = (int)data;
	if ((num >= 0) && (num < NUM_SAVEGAMES))
	{
		sprintf(filename, "savegame%d.dls", num);
		fp = fopen(filename, "rb");
		if (fp)
		{
			gameState = START;

			fread(&magicNum, sizeof(int), 1, fp);
			if (magicNum == SAVEMAGIC)
			{
				//read the game description
				fread(description, sizeof(description), 1, fp);

				//read map size
				fread(&mapSizeX, sizeof(short), 1, fp);
				fread(&mapSizeY, sizeof(short), 1, fp);
				if ((mapSizeX >= 0) && (mapSizeX < MAX_XSIZE) && (mapSizeY >= 0) && (mapSizeY < MAX_YSIZE))
				{
					////////clear all old data
					if(structureList)
					{
						while(!structureList->isEmpty())
							delete ((StructureClass*)structureList->removeFirstElement());
					}
					else
					{
						unitList = new ListIterator();	//holds all the units

						structureList = new ListIterator();	//all the structures
						bulletList = new ListIterator();	//all projectiles

						selectedList = new ListIterator();	//all the objects currently selected
						selectedLists = new ListIterator[NUMSELECTEDLISTS];	//nine - for each of the keys 1 - 9 on keyboard



						airUnitBlitList = new ListIterator();	//holds a list ordering the blitting of units

						groundUnitBlitList = new ListIterator();	//holds a list ordering the blitting of units

						infantryBlitList = new ListIterator();	//holds a list ordering the blitting of units

						selectionBlitList = new ListIterator();	//holds a list ordering the blitting of selection boxes

						undergroundUnitBlitList = new ListIterator();	//holds a list ordering the blitting of units

					}


					if(unitList)

					{

						while(!unitList->isEmpty())

							delete ((UnitClass*)unitList->removeFirstElement());
					}


					if(selectedList)

					{

						selectedList->clearList();

						for (i=0; i<NUMSELECTEDLISTS; i++)

							selectedLists[i].clearList();
					}

					if(bulletList)
					{
						while(!bulletList->isEmpty())
							delete ((BulletClass*)bulletList->removeFirstElement());

					}



					for (i=0; i<MAX_PLAYERS; i++)

					if (player[i] != NULL)

					{

						delete player[i];

						player[i] = NULL;

					}



					if(map)

						delete map;
					/////////////

					//create the new map
					map = new MapClass(mapSizeX, mapSizeY);

					//read settings
					fread(&settings, sizeof(SETTINGSTYPE), 1, fp);
					new_gamespeed=settings.gamespeed;
					
					//load the player(s) info
					for (i=0; i<MAX_PLAYERS; i++)
					{
						fread(&j, sizeof(int), 1, fp);
						if (j)	//player in game
						{
							fread(&house, sizeof(int), 1, fp);
							fread(&colour, sizeof(int), 1, fp);
							fread(&j, sizeof(int), 1, fp);
							if (j)	//player is ai
							{
								fread(&j, sizeof(int), 1, fp);
								player[i] = new AiPlayerClass(i, house, colour, 0, (DIFFICULTYTYPE)j);
								//settings.playerHouse[i] = house;
							}
							else
							{
								
								player[i] = new PlayerClass(i, house, colour, 0);
								settings.playerNum = i;
								settings.playerHouse[settings.playerNum] = house;

							}

							player[i]->load(fp);
						}
					}

					//load the game flags
					fread(&i, sizeof(i), 1, fp);
					map->setWinFlags(i);

					/*
					fread(&i, sizeof(i), 1, fp);
					settings.techLevel = i;
					fread(&i, sizeof(i), 1, fp);
					settings.gameType = (GAMETYPE)i;
					*/
					

					

					fread(&i, sizeof(i), 1, fp);
					fullscreenMode = (i == 1);
					fread(&i, sizeof(i), 1, fp);
					debug = (i == 1);

					/*
					if(settings.gameType == ORIGINAL)
					{
						fread(&i, sizeof(i), 1, fp);
						settings.campaignLevel = i;
					}*/

					//load terrain data
					for (i=0; i<map->sizeX; i++)
					for (j=0; j<map->sizeY; j++)
					{
						fread(&x, sizeof(int), 1, fp);
						map->getCell(i, j)->setType(x);
						fread(&x, sizeof(int), 1, fp);
						map->getCell(i, j)->setTile(x);
						fread(&x, sizeof(int), 1, fp);
						map->getCell(i, j)->setOwner(x);
						for (y=0; y<MAX_PLAYERS; y++)
						{
							fread(&x, sizeof(int), 1, fp);
							if (x)
								map->viewMap(player[y]->getTeam(), i, j, 0);	
						}
					}

					map->createSandRegions();
					smooth_terrain();

					//load the structure info
					fread(&num, sizeof(int), 1, fp);
					for (i=0; i<num; i++)
					{
						fread(&objectID, sizeof(int), 1, fp);
						if (objectID != NONE)
						{
							fread(&playerNum, sizeof(int), 1, fp);
							fread(&itemID, sizeof(int), 1, fp);
							fread(&x, sizeof(int), 1, fp);
							fread(&y, sizeof(int), 1, fp);

							newStructure = (StructureClass*)player[playerNum]->placeStructure(NONE, NONE, NONE, itemID, x, y);
							if (newStructure)
							{
								newStructure->setObjectID(objectID);
								fread(&x, sizeof(int), 1, fp);
								newStructure->setHealth(x);
								if ((newStructure->getItemID() == Structure_GunTurret)
									|| (newStructure->getItemID() == Structure_RocketTurret))
								{
									fread(&x, sizeof(int), 1, fp);
									((GunTurretClass*)newStructure)->setTurretAngle(x);
								}
								else if (newStructure->getItemID() == Structure_Palace)
								{
									fread(&x, sizeof(int), 1, fp);
									((PalaceClass*)newStructure)->setSpecialTimer(x);
								}
								else if (newStructure->getItemID() == Structure_StarPort)
								{
									fread(&x, sizeof(int), 1, fp);
									((StarPortClass*)newStructure)->setArrivalTimer(x);
								}
								fread(&x, sizeof(int), 1, fp);
								fread(&y, sizeof(int), 1, fp);
								newStructure->setDestination(x, y);
							}
						}
					}

					//load the unit info
					fread(&num, sizeof(int), 1, fp);
					for (i=0; i<num; i++)
					{
						fread(&objectID, sizeof(int), 1, fp);
						if (objectID != NONE)
						{
							fread(&playerNum, sizeof(int), 1, fp);
							fread(&itemID, sizeof(int), 1, fp);
							fread(&x, sizeof(int), 1, fp);
							fread(&y, sizeof(int), 1, fp);

							newUnit = (UnitClass*)player[playerNum]->createUnit(itemID);
							if (newUnit)
							{
								if (x != NONE)
								{
									COORDTYPE temp = { x, y };
									newUnit->deploy(&temp);
								}
								newUnit->setObjectID(objectID);
								fread(&x, sizeof(int), 1, fp);
								newUnit->setAngle(x);
								fread(&x, sizeof(int), 1, fp);
								newUnit->setHealth(x);
								if (newUnit->getItemID() == Unit_Harvester)
								{
									fread(&tempDouble, sizeof(tempDouble), 1, fp);	
									((HarvesterClass*)newUnit)->setAmountOfSpice(tempDouble);
								}
								else if (newUnit->getItemID() == Unit_Sandworm)
								{
									fread(&x, sizeof(x), 1, fp);
									((Sandworm*)newUnit)->setSleepTimer(x);				
								}
								if (newUnit->isTurreted())
								{
									fread(&x, sizeof(int), 1, fp);				
									((TankClass*)newUnit)->setTurretAngle(x);
								}
								fread(&x, sizeof(int), 1, fp);
								if (x > 0)	//was it deviated
								{
									fread(&y, sizeof(int), 1, fp);
									if ((y >= 0) && (y < MAX_PLAYERS))
									{
										newUnit->deviate(player[y]);
										newUnit->setDeviationTime(x);
									}
								}
								fread(&x, sizeof(int), 1, fp);
								newUnit->setAttackMode((ATTACKTYPE)x);
								fread(&x, sizeof(int), 1, fp);
								newUnit->setForced(x == 1);
								fread(&x, sizeof(int), 1, fp);
								newUnit->setAttacking(x == 1);
								fread(&x, sizeof(int), 1, fp);
								fread(&y, sizeof(int), 1, fp);
								newUnit->setDestination(x, y);
							}
							else
								newUnit = newUnit;
						}
					}

					map->setWinFlags(3);
					setupView();

					//load object extra info
					fread(&num, sizeof(int), 1, fp);
					for (i=0; i<num; i++)
					{
						fread(&objectID, sizeof(int), 1, fp);
						if (objectID != NONE)
						{
							fread(&x, sizeof(int), 1, fp);
							fread(&y, sizeof(int), 1, fp);
							newObject = map->findObjectWidthID(objectID, x, y);
							if (newObject == NULL)
								newObject = newObject;
							fread(&objectID, sizeof(int), 1, fp);
							if (objectID != NONE)
							{
								fread(&x, sizeof(int), 1, fp);
								fread(&y, sizeof(int), 1, fp);
								newTarget = map->findObjectWidthID(objectID, x, y);
								if (newObject)
									newObject->setTarget(newTarget);
							}
							if (newObject)
							{
								if (newObject->getItemID() == Structure_Refinery)
								{
									fread(&objectID, sizeof(int), 1, fp);
									if (objectID != NONE)
									{
										newTarget = map->findObjectWidthID(objectID, x, y);
										if (newTarget && (newTarget->getItemID() == Unit_Harvester))
										{
											newTarget->setTarget(newObject);
											((RefineryClass*)newObject)->assignHarvester((HarvesterClass*)newTarget);
										}
									}
								}
								else if (newObject->getItemID() == Structure_RepairYard)
								{
									fread(&objectID, sizeof(int), 1, fp);
									if (objectID != NONE)
									{
										newTarget = map->findObjectWidthID(objectID, x, y);
										if (newTarget && newTarget->isAUnit())
										{
											newTarget->setTarget(newObject);
											((RepairYardClass*)newObject)->assignUnit((UnitClass*)newTarget);
										}
									}
								}
								if (newObject->isAStructure() && ((StructureClass*)newObject)->isABuilder())
								((BuilderClass*)newObject)->loadBuildList(fp);
								if ((newObject->getItemID() == Unit_Carryall) || (newObject->getItemID() == Unit_Frigate))
									((Carryall*)newObject)->loadCargo(fp);
							}
						}
					}

					x = bulletList->getNumElements();
					fread(&x, sizeof(x), 1, fp);
					for(i = 0; i < x; i++)
					{
						fread(&objectID, sizeof(x), 1, fp);
						if (objectID != NONE)
							newObject = map->findObjectWidthID(objectID, NONE, NONE);
						else
							newObject = NULL;
						
						fread(&source, sizeof(source), 1, fp);
						fread(&destination, sizeof(source), 1, fp);
						fread(&itemID, sizeof(x), 1, fp);

						//air
						fread(&num, sizeof(x), 1, fp);

						newBullet = new BulletClass(newObject, &source, &destination, itemID, num == 1);
						newBullet->load(fp);
						bulletList->insertLast(newBullet);
					}

					//load selection lists
					ObjectClass *tempObject;



					fread(&x, sizeof(int), 1, fp);	//read number in selection list

					for (i = 0; i < x; i++)

					{

						fread(&y, sizeof(int), 1, fp);

						tempObject = map->findObjectWidthID(y, NONE, NONE);

						if (tempObject)

						{

							selectedList->insertLast(tempObject);

							tempObject->setSelected(true);

						}

					}



					ListIterator* tempList;

					for (int listCount = 0; listCount < 9; listCount++)

					{

						tempList = &selectedLists[listCount];

						

						fread(&x, sizeof(int), 1, fp);	//read number in selection list

						for (i = 0; i < x; i++)

						{

							fread(&y, sizeof(int), 1, fp);

							tempObject = map->findObjectWidthID(y, NONE, NONE);

							if (tempObject)

								tempList->insertLast(tempObject);

						}

					}

					//load the map/dborder info
					fread(&dborder->radarMinX, sizeof(int), 1, fp);
					fread(&dborder->radarMaxX, sizeof(int), 1, fp);
					fread(&dborder->radarMinY, sizeof(int), 1, fp);
					fread(&dborder->radarMaxY, sizeof(int), 1, fp);
					fread(&dborder->minX, sizeof(int), 1, fp);
					fread(&dborder->maxX, sizeof(int), 1, fp);
					fread(&dborder->minY, sizeof(int), 1, fp);
					fread(&dborder->maxY, sizeof(int), 1, fp);

					//check view is ok
					if (dborder->minX < 0)
						dborder->minX = 0;
					dborder->maxX = dborder->minX + gameBarPos.x-1;
					if (dborder->maxX >= map->sizeX*BLOCKSIZE)
					{
						dborder->maxX = map->sizeX*BLOCKSIZE - 1;
						dborder->minX = dborder->maxX - gameBarPos.x+1;
						if (dborder->minX < 0)
							dborder->minX = 0;
					}

   					if (dborder->minY < 0)
						dborder->minY = 0;
					dborder->maxY = dborder->minY + screen->h-1;
					if (dborder->maxY >= map->sizeY*BLOCKSIZE)
					{
						dborder->maxY = map->sizeY*BLOCKSIZE - 1;
						dborder->minY = dborder->maxY - screen->h+1;
						if (dborder->minY < 0)
							dborder->minY = 0;
					}

					game_resume();
					clearMessage();
					settings.finished = false;
				}
			}
			else
			{
				sprintf(temp, "Corrupt save game file \"%s\".", filename);
				showMessage(temp);
			}

			gameState = BEGUN;
			fclose(fp);
		}
		else
		{
			sprintf(temp, "Could not open save game file \"%s\".", filename);
			showMessage(temp);
		}
	}
}

void game_load()
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

				gameLoadSpotButton[i].setText(description);

				gameLoadSpotButton[i].setEnabled(true);

			}

			else

			{

				gameLoadSpotButton[i].setText("(Empty)");

				gameLoadSpotButton[i].setEnabled(false);

			}



			fclose(fp);

		}

		else

		{

			gameLoadSpotButton[i].setText("(Empty)");

			gameLoadSpotButton[i].setEnabled(false);

		}

	}

	menuInterface = &loadWindow;
}

void game_cancelSaveEntry()
{
	gameSaveNameEntry.setEnabled(false);
	gameSaveNameEntry.setVisible(false);
	saveButton.setEnabled(false);
	saveButton.setVisible(false);
}

void game_setSaveGameSpot(void* data)
{
	int spot = (int)data;

	if ((spot >= 0) && (spot < NUM_SAVEGAMES))
	{
		saveGameSpot = spot;

		gameSaveNameEntry.setEnabled(true);
		gameSaveNameEntry.setText(gameSaveSpotButton[spot].getText());
		gameSaveNameEntry.setX(gameSaveSpotButton[spot].getX());
		gameSaveNameEntry.setY(gameSaveSpotButton[spot].getY());
		gameSaveNameEntry.setVisible(true);
		if (gameSaveNameEntry.textEquals("(Empty)"))
			gameSaveNameEntry.setText("");
		gameSaveNameEntry.handlePress(NONE, NONE);

		saveButton.setEnabled(true);
		saveButton.setVisible(true);
	}
}

void mapHouseColours()
{
	//thisPlayer = player[settings.playerNum];
	int playerColour = thisPlayer->getColour();
	
	//TODO: should probably free these first, memory leak

	gamebarSurface = copySurface((SDL_Surface*)dataFile[UI_GameBar2].dat);
	topBarSurface = copySurface((SDL_Surface*)dataFile[UI_TopBar].dat);
	mentatSurface = copySurface((SDL_Surface*)dataFile[UI_Mentat].dat);
	mentatSurfacePressed = copySurface((SDL_Surface*)dataFile[UI_Mentat_Pressed].dat);
	MessageBoxSurface = copySurface((SDL_Surface*)dataFile[UI_MessageBox].dat);
	optionsSurface = copySurface((SDL_Surface*)dataFile[UI_Options].dat);
	optionsSurfacePressed = copySurface((SDL_Surface*)dataFile[UI_Options_Pressed].dat);

	aggressiveSurf = copySurface((SDL_Surface*)dataFile[UI_Unit_Aggr].dat);
	aggressiveSurfActive = copySurface((SDL_Surface*)dataFile[UI_Unit_Aggr_Active].dat);
	scoutSurf = copySurface((SDL_Surface*)dataFile[UI_Unit_Scout].dat);
	scoutSurfActive = copySurface((SDL_Surface*)dataFile[UI_Unit_Scout_Active].dat);
	standGroundSurf = copySurface((SDL_Surface*)dataFile[UI_Unit_StGnd].dat);
	standGroundSurfActive = copySurface((SDL_Surface*)dataFile[UI_Unit_StGnd_Active].dat);
	defensiveSurf = copySurface((SDL_Surface*)dataFile[UI_Unit_Def].dat);
	defensiveSurfActive = copySurface((SDL_Surface*)dataFile[UI_Unit_Def_Active].dat);

	deploySurf = copySurface((SDL_Surface*)dataFile[UI_Unit_Deploy].dat);
	deploySurfPressed = copySurface((SDL_Surface*)dataFile[UI_Unit_Deploy_Pressed].dat);
	
	destructSurf = copySurface((SDL_Surface*)dataFile[UI_Unit_Destruct].dat);
	destructSurfPressed = copySurface((SDL_Surface*)dataFile[UI_Unit_Destruct_Pressed].dat);

	resumeGameSurf = copySurface((SDL_Surface*)dataFile[UI_Game_Resume].dat);
	resumeGameSurfPressed = copySurface((SDL_Surface*)dataFile[UI_Game_Resume_Pressed].dat);

	saveGameSurf = copySurface((SDL_Surface*)dataFile[UI_Game_Save].dat);
	saveGameSurfPressed = copySurface((SDL_Surface*)dataFile[UI_Game_Save_Pressed].dat);

	loadGameSurf = copySurface((SDL_Surface*)dataFile[UI_Loadgame].dat);
	loadGameSurfPressed = copySurface((SDL_Surface*)dataFile[UI_Loadgame_Pressed].dat);

	gameSettingsSurf = copySurface((SDL_Surface*)dataFile[UI_Game_Settings].dat);
	gameSettingsSurfPressed = copySurface((SDL_Surface*)dataFile[UI_Game_Settings_Pressed].dat);

	quitSurf = copySurface((SDL_Surface*)dataFile[UI_Game_Quit].dat);
	quitSurfPressed = copySurface((SDL_Surface*)dataFile[UI_Game_Quit_Pressed].dat);

	restartSurf = copySurface((SDL_Surface*)dataFile[UI_Game_Restart].dat);
	restartSurfPressed = copySurface((SDL_Surface*)dataFile[UI_Game_Restart_Pressed].dat);

	cancelSurf = copySurface((SDL_Surface*)dataFile[UI_Cancel].dat);
	cancelSurfPressed = copySurface((SDL_Surface*)dataFile[UI_Cancel_Pressed].dat);

	saveSurf = copySurface((SDL_Surface*)dataFile[UI_Save].dat);
	saveSurfPressed = copySurface((SDL_Surface*)dataFile[UI_Save_Pressed].dat);

	loadSurf = copySurface((SDL_Surface*)dataFile[UI_Load].dat);
	loadSurfPressed = copySurface((SDL_Surface*)dataFile[UI_Load_Pressed].dat);

	settingsWindowSurf = copySurface((SDL_Surface*)dataFile[UI_OptionsMenu].dat);

	settingsCancelSurf = copySurface((SDL_Surface*)dataFile[UI_Cancel].dat);
	settingsCancelSurfPressed = copySurface((SDL_Surface*)dataFile[UI_Cancel_Pressed].dat);

	settingsOKSurf = copySurface((SDL_Surface*)dataFile[UI_OK].dat);
	settingsOKSurfPressed = copySurface((SDL_Surface*)dataFile[UI_OK_Pressed].dat);

	settingsPlus = copySurface((SDL_Surface*)dataFile[UI_Plus].dat);
	settingsPlusPressed = copySurface((SDL_Surface*)dataFile[UI_Plus_Pressed].dat);

	settingsMinus = copySurface((SDL_Surface*)dataFile[UI_Minus].dat);
	settingsMinusPressed = copySurface((SDL_Surface*)dataFile[UI_Minus_Pressed].dat);

	mapImageHouseColour(settingsPlus, playerColour);
	mapImageHouseColour(settingsPlusPressed, playerColour);
	mapImageHouseColour(settingsMinus, playerColour);
	mapImageHouseColour(settingsMinusPressed, playerColour);

	mapImageHouseColour(settingsWindowSurf, playerColour);
	mapImageHouseColour(settingsCancelSurf, playerColour);
	mapImageHouseColour(settingsCancelSurfPressed, playerColour);
	mapImageHouseColour(settingsOKSurf, playerColour);
	mapImageHouseColour(settingsOKSurfPressed, playerColour);
		
	mapImageHouseColour(cancelSurf, playerColour);
	mapImageHouseColour(cancelSurfPressed, playerColour);
	mapImageHouseColour(saveSurf, playerColour);
	mapImageHouseColour(saveSurfPressed, playerColour);
	mapImageHouseColour(loadSurf, playerColour);
	mapImageHouseColour(loadSurfPressed, playerColour);

	mapImageHouseColour(aggressiveSurf, playerColour);
	mapImageHouseColour(aggressiveSurfActive, playerColour);
	mapImageHouseColour(scoutSurf, playerColour);
	mapImageHouseColour(scoutSurfActive, playerColour);
	mapImageHouseColour(standGroundSurf, playerColour);
	mapImageHouseColour(standGroundSurfActive, playerColour);
	mapImageHouseColour(defensiveSurf, playerColour);
	mapImageHouseColour(defensiveSurfActive, playerColour);

	mapImageHouseColour(deploySurf, playerColour);
	mapImageHouseColour(deploySurfPressed, playerColour);
	mapImageHouseColour(destructSurf, playerColour);
	mapImageHouseColour(destructSurfPressed, playerColour);

	mapImageHouseColour(resumeGameSurf, playerColour);
	mapImageHouseColour(resumeGameSurfPressed, playerColour);

	mapImageHouseColour(saveGameSurf, playerColour);
	mapImageHouseColour(saveGameSurfPressed, playerColour);
	mapImageHouseColour(loadGameSurf, playerColour);
	mapImageHouseColour(loadGameSurfPressed, playerColour);

	mapImageHouseColour(gameSettingsSurf, playerColour);
	mapImageHouseColour(gameSettingsSurfPressed, playerColour);
	mapImageHouseColour(quitSurf, playerColour);
	mapImageHouseColour(quitSurfPressed, playerColour);
	mapImageHouseColour(restartSurf, playerColour);
	mapImageHouseColour(restartSurfPressed, playerColour);


	mapImageHouseColourBase(gamebarSurface, playerColour, prevHouseCol);
	mapImageHouseColourBase(topBarSurface, playerColour, prevHouseCol);

	mapImageHouseColourBase(mentatSurface, playerColour, prevHouseCol);
	mapImageHouseColourBase(mentatSurfacePressed, playerColour, prevHouseCol);
	mapImageHouseColourBase(optionsSurface, playerColour, prevHouseCol);
	mapImageHouseColourBase(optionsSurfacePressed, playerColour, prevHouseCol);
	
	//prevHouseCol = thisPlayer->getColour();
}

void game_loadMap()
{
	//load map
	if (settings.gameType == RANDOMMAP)
		make_random_map(128, 128);
	else if (!loadMap(settings.mapFilename))
		make_random_map(128, 128);



	game_loop();
}

void do_game()
{
	int		i;
//	int		count,
//			frameStart, frameEnd, frameTime,

//			frameTimeTotal, numFrames,

//			tic;

	gameState = START;

	structuresCreated = unitsCreated = 0;
	fps = 0;
	debug = false;

	builderSelectMode = fullscreenMode = 

	groupCreateMode = menuMode = messageMode = moveDownMode = 

	moveLeftMode = moveRightMode = moveUpMode = 

	placingMode = shift = showFPS = radarMode = selectionMode = 
	quitGame = false;

	focusedWidget = NULL;
	nextObjectID = 0;

	for(i = 0; i < MESSAGE_BUFFER_SIZE; i++)	//clear in game messages
		messageBuffer[i][0] = '\0';
	typingMessage[0] = '\0';

	powerIndicatorPos.x = 14;
	powerIndicatorPos.y = 146;
	spiceIndicatorPos.x = 20;
	spiceIndicatorPos.y = 146;
	powerIndicatorPos.w = spiceIndicatorPos.w = 4;
	powerIndicatorPos.h = spiceIndicatorPos.h = 331;

	printf("starting game\n");fflush(stdout);

	soundPlayer->changeMusic(MUSIC_PEACE);
	//////////////////////////////////////////////////////////////////////////
	dborder = new BORDERTYPE;

	//create surface for fog overlay (full fogged square)
	SDL_FreeSurface(fogSurf);
	fogSurf = SDL_CreateRGBSurface(
			SDL_HWSURFACE,
			256,/*width*/
			16,/*height*/
			32,
			0,0,0,0);/*r,g,b,*/
	
	SDL_SetAlpha(fogSurf, SDL_SRCALPHA, 40);

	hiddenSurf = copySurface((SDL_Surface*)dataFile[Terrain_Hidden].dat);
	
	//set up pixelmap for neighbours of fogged terrain
	SDL_LockSurface(hiddenSurf);
	for(int x=0;x<256;x++)
		for(int y=0;y<16;y++)	
			fogMAP[x][y] = getpixel(hiddenSurf,x,y);
	SDL_UnlockSurface(hiddenSurf);
	
							
							
	
	if(!loadingGame)
	{

		unitList = new ListIterator();	//holds all the units

		structureList = new ListIterator();	//all the structures
		bulletList = new ListIterator();	//all projectiles

		selectedList = new ListIterator();	//all the objects currently selected
		selectedLists = new ListIterator[NUMSELECTEDLISTS];	//nine - for each of the keys 1 - 9 on keyboard



		airUnitBlitList = new ListIterator();	//holds a list ordering the blitting of units

		groundUnitBlitList = new ListIterator();	//holds a list ordering the blitting of units

		infantryBlitList = new ListIterator();	//holds a list ordering the blitting of units

		selectionBlitList = new ListIterator();	//holds a list ordering the blitting of selection boxes

		undergroundUnitBlitList = new ListIterator();	//holds a list ordering the blitting of units

	
		player = new PlayerClass*[NUM_HOUSES];
		for (i = 0; i < MAX_PLAYERS; i++)	
			player[i] = NULL;

		for (i = 0; i < MAX_PLAYERS; i++)
		{
			if (strlen(settings.playerName[i]))
			{
				if (settings.isHuman[i])
					player[i] = new PlayerClass(i, settings.playerHouse[i], settings.playerColour[i], DEFAULT_STARTINGCREDITS);
				else
					player[i] = new AiPlayerClass(i, settings.playerHouse[i], settings.playerColour[i], ((settings.gameType == ORIGINAL) || (settings.gameType == SKIRMISH)) ? 0 : DEFAULT_STARTINGCREDITS, settings.playerDifficulty[i]);
				player[i]->setTeam(settings.playerTeam[i]);
				player[i]->assignMapPlayerNum(settings.playerMapNumber[i]);
			}
		}

		thisPlayer = player[settings.playerNum];
	}
	/*
	mapImageHouseColour((SDL_Surface*)dataFile[UI_GameBar].dat, thisPlayer->getColour());
	mapImageHouseColour((SDL_Surface*)dataFile[UI_GameBar2].dat, thisPlayer->getColour());
	mapImageHouseColour((SDL_Surface*)dataFile[UI_TopBar].dat, thisPlayer->getColour());

	mapImageHouseColour((SDL_Surface*)dataFile[UI_Up].dat, thisPlayer->getColour());
	mapImageHouseColour((SDL_Surface*)dataFile[UI_Up_Pressed].dat, thisPlayer->getColour());
	mapImageHouseColour((SDL_Surface*)dataFile[UI_Down].dat, thisPlayer->getColour());
	mapImageHouseColour((SDL_Surface*)dataFile[UI_Down_Pressed].dat, thisPlayer->getColour());
	mapImageHouseColour((SDL_Surface*)dataFile[UI_Upgrade].dat, thisPlayer->getColour());
	mapImageHouseColour((SDL_Surface*)dataFile[UI_Upgrade_Pressed].dat, thisPlayer->getColour());
	mapImageHouseColour((SDL_Surface*)dataFile[UI_Mentat].dat, thisPlayer->getColour());
	mapImageHouseColour((SDL_Surface*)dataFile[UI_Mentat_Pressed].dat, thisPlayer->getColour());
	mapImageHouseColour((SDL_Surface*)dataFile[UI_Options].dat, thisPlayer->getColour());
	mapImageHouseColour((SDL_Surface*)dataFile[UI_Options_Pressed].dat, thisPlayer->getColour());
*/
	
		

	mapHouseColours();

	focusedWidget = NULL;
	gameBarPos.w = gamebarSurface->w;
	gameBarPos.h = gamebarSurface->h;
	gameBarPos.x = screen->w - gameBarPos.w;
	gameBarPos.y = 0;

	topBarPos.w = topBarSurface->w;
	topBarPos.h = topBarSurface->h;
	topBarPos.x = 0;
	topBarPos.y = 0;

	creditsPos.x = 	gameBarPos.x + 49;
	creditsPos.y = 	gameBarPos.y + 135;

	//Setup main buttons
//	SDL_Surface *mentatSurf = mentatSurface;
//	SDL_Surface *optionsSurf = (SDL_Surface *)dataFile[UI_Options].dat;

	/*set up messageBox so mainButtons can be adjusted*/
	messageBox.initialize();
	messageBox.setX(-2);
	messageBox.setY(-2);

	mainButtonsBox.dropChildren();
	mainButtonsBox.setX(messageBox.getWidth() + 10);
	mainButtonsBox.setY(0);
	mainButtonsBox.setWidth(topBarPos.w - 18 - messageBox.getWidth());
	mainButtonsBox.setHeight(topBarPos.h - 18); //The twirly bar thing is 18 pixels
	
	
	
	optionsButton.setHeight(optionsSurface->h);
	optionsButton.setWidth(optionsSurface->w);
//	optionsButton.setX(15);
//	optionsButton.setY(15);
	optionsButton.setSurface(optionsSurface, NULL);
	optionsButton.setPressedSurface(optionsSurfacePressed, NULL);
	optionsButton.setCallBack(game_options, NULL);
	optionsButton.setVisible(true);

	mentatButton.setHeight(mentatSurface->h);
	mentatButton.setWidth(mentatSurface->w);
//	mentatButton.setX(15 + optionsSurf->w + 15);
//	mentatButton.setY(15);
	mentatButton.setSurface(mentatSurface, NULL);
	mentatButton.setPressedSurface(mentatSurfacePressed, NULL);
	mentatButton.setCallBack(game_mentat, NULL);
	mentatButton.setVisible(true);

	mainButtonsBox.addChild(&optionsButton);
	mainButtonsBox.addChild(&mentatButton);
	mainButtonsBox.setVisible(true);
    
		
	//mentat = new MentatClass(thisPlayer->getHouse());
	mentat->setHouse(thisPlayer->getHouse());
	showMentat = false;

	//////////////////////////////////////////////////////////////////////////



	unitInterface.dropChildren();

	unitInterface.setX(gameBarPos.x + 33);
	unitInterface.setY(16 + 128 + 16);
	unitInterface.setWidth((screen->w - 16) - unitInterface.getX());
	unitInterface.setHeight((screen->h - 16) - unitInterface.getY());


	unitIcon.setEnabled(false);
	unitIcon.setVisible(true);
	unitIcon.setWidth(64);
	unitIcon.setHeight(48);
	unitInterface.addChild(&unitIcon);

	//unitButton_Aggressive.setText("Aggressive");
	unitButton_Aggressive.setSurface(aggressiveSurf, NULL);
	unitButton_Aggressive.setTransparent(true);
	unitButton_Aggressive.setHelpText("Unit will engage any unit within guard range");
	unitButton_Aggressive.setCallBack(setAttackMode, (void*)AGGRESSIVE);
	unitButton_Aggressive.setVisible(true);
	unitInterface.addChild(&unitButton_Aggressive);

	//unitButton_Defensive.setText("Defensive");
	unitButton_Defensive.setSurface(defensiveSurf, NULL);
	unitButton_Defensive.setTransparent(true);
	unitButton_Defensive.setHelpText("Unit will remain in approximate location");
	unitButton_Defensive.setCallBack(setAttackMode, (void*)DEFENSIVE);
	unitButton_Defensive.setVisible(true);
	unitInterface.addChild(&unitButton_Defensive);


	//unitButton_StandGround.setText("Stand Ground");
	unitButton_StandGround.setSurface(standGroundSurf, NULL);
	unitButton_StandGround.setTransparent(true);
	unitButton_StandGround.setHelpText("Unit will not move from location");
	unitButton_StandGround.setCallBack(setAttackMode, (void*)STANDGROUND);
	unitButton_StandGround.setVisible(true);
	unitInterface.addChild(&unitButton_StandGround);

	unitButton_Scout.setSurface(scoutSurf, NULL);
	unitButton_Scout.setTransparent(true);
//	unitButton_Scout.setText("Scout");
	unitButton_Scout.setHelpText("Unit will not move, nor attack");
	unitButton_Scout.setCallBack(setAttackMode, (void*)SCOUT);
	unitButton_Scout.setVisible(true);
	unitInterface.addChild(&unitButton_Scout);

//	unitButton_Deploy.setText("Deploy");
	unitButton_Deploy.setSurface(deploySurf, NULL);
	unitButton_Deploy.setPressedSurface(deploySurfPressed, NULL);
	unitButton_Deploy.setCallBack(mcvDeploy);
	unitButton_Deploy.setEnabled(false);
	unitButton_Deploy.setVisible(false);
	unitInterface.addChild(&unitButton_Deploy);


//	unitButton_Destruct.setText("Destruct");
	unitButton_Destruct.setSurface(destructSurf, NULL);
	unitButton_Destruct.setPressedSurface(destructSurfPressed, NULL);
	unitButton_Destruct.setCallBack(devastatorDestruct);
	unitButton_Destruct.setEnabled(false);
	unitButton_Destruct.setVisible(false);
	unitInterface.addChild(&unitButton_Destruct);



	unitInterface.setVisible(true);



	//------------------



	SDL_Surface *surf = (SDL_Surface *)dataFile[UI_GameMenu].dat;



	menuWindow.dropChildren();

	menuWindow.setX((screen->w - surf->w) / 2);

	menuWindow.setY((screen->h - surf->h) / 2);

	menuWindow.setWidth(surf->w);

	menuWindow.setHeight(surf->h);

	menuWindow.setSurface(surf, NULL);

/*	menuWindow.setX(screen->w/4);

	menuWindow.setY(screen->h/4);

	menuWindow.setWidth(screen->w/2);

	menuWindow.setHeight(screen->h/2);*/



	menuInterface = &menuWindow;



//	menu_vBox.dropChildren();

//	menu_vBox.setX(menuInterface->getX() + 10);

//	menu_vBox.setY(menuInterface->getY() + 10);

//	menu_vBox.setWidth(menuInterface->getWidth() - 20);

///	menu_vBox.setHeight(menuInterface->getHeight() - 20);

//	menuInterface->addChild(&menu_vBox);

//	menu_vBox.setVisible(true);





	

//	resumeButton.setText("Resume Game");
	resumeButton.setSurface(resumeGameSurf, NULL);
	resumeButton.setPressedSurface(resumeGameSurfPressed, NULL);
	resumeButton.setX(menuWindow.getX() + 22);
	resumeButton.setY(menuWindow.getY() + 35);
	resumeButton.setHeight(resumeGameSurf->h);
	resumeButton.setWidth(resumeGameSurf->w);
	resumeButton.setCallBack(game_resume);
	resumeButton.setVisible(true);
	menuWindow.addChild(&resumeButton);



	gameSettingsButton.setSurface(gameSettingsSurf, NULL);
	gameSettingsButton.setPressedSurface(gameSettingsSurfPressed, NULL);
	gameSettingsButton.setX(resumeButton.getX());
	gameSettingsButton.setY(resumeButton.getY() + 19);
	gameSettingsButton.setHeight(gameSettingsSurf->h);
	gameSettingsButton.setWidth(gameSettingsSurf->w);

//	gameSettingsButton.setText("Game Settings");
	gameSettingsButton.setCallBack(game_settings);
	gameSettingsButton.setVisible(true);
	menuWindow.addChild(&gameSettingsButton);







//	soundOptionsButton.setText("Sound Options");
//	soundOptionsButton.setCallBack(soundOptionsButton);
//	soundOptionsButton.setVisible(true);
//	menuWindow.addChild(&soundOptionsButton);





	saveGameButton.setSurface(saveGameSurf, NULL);
	saveGameButton.setPressedSurface(saveGameSurfPressed, NULL);
	saveGameButton.setX(gameSettingsButton.getX());
	saveGameButton.setY(gameSettingsButton.getY() + 19);
	saveGameButton.setHeight(saveGameSurf->h);
	saveGameButton.setWidth(saveGameSurf->w);

//	saveGameButton.setText("Save Game");
	saveGameButton.setCallBack(game_save);
	saveGameButton.setVisible(true);
	menuWindow.addChild(&saveGameButton);

	saveGameButton.setEnabled(settings.gameType != MULTIPLAYER);



	loadGameButton.setSurface(loadGameSurf, NULL);
	loadGameButton.setPressedSurface(loadGameSurfPressed, NULL);
	loadGameButton.setX(saveGameButton.getX());
	loadGameButton.setY(saveGameButton.getY() + 19);
	loadGameButton.setHeight(loadGameSurf->h);
	loadGameButton.setWidth(loadGameSurf->w);

//	loadGameButton.setText("Load Game");
	loadGameButton.setCallBack(game_load);
	loadGameButton.setVisible(true);
	menuWindow.addChild(&loadGameButton);

	loadGameButton.setEnabled(settings.gameType != MULTIPLAYER);



	quitButton.setSurface(quitSurf, NULL);
	quitButton.setPressedSurface(quitSurfPressed, NULL);
	quitButton.setX(loadGameButton.getX());
	quitButton.setY(loadGameButton.getY() + 19);
	quitButton.setHeight(quitSurf->h);
	quitButton.setWidth(quitSurf->w);

//	quitButton.setText("Quit to Main Menu");
	quitButton.setCallBack(game_quit);
	quitButton.setVisible(true);
	menuWindow.addChild(&quitButton);



	restartGameButton.setSurface(restartSurf, NULL);
	restartGameButton.setPressedSurface(restartSurfPressed, NULL);
	restartGameButton.setX(quitButton.getX());
	restartGameButton.setY(quitButton.getY() + 19);
	restartGameButton.setHeight(restartSurf->h);
	restartGameButton.setWidth(restartSurf->w);

//	quitButton.setText("Quit to Main Menu");
//	restartGameButton.setCallBack(game_quit);
	restartGameButton.setVisible(true);
	menuWindow.addChild(&restartGameButton);



	menuInterface->setVisible(true);



	//------------------

	init_save_load_windows();

/*

	surf = (SDL_Surface *)dataFile[UI_LoadSaveWindow].dat;



	saveWindow.dropChildren();

	

	saveWindow.setSurface(surf, NULL);

	saveWindow.setX(screen->w/4);

	saveWindow.setY(screen->h/4);

//	saveWindow.setWidth(screen->w/2);

//	saveWindow.setHeight(screen->h/2);

	saveWindow.setWidth(surf->w);

	saveWindow.setHeight(surf->h);

	saveWindow.setVisible(true);



//	save_vBox.dropChildren();

//	save_hBox.dropChildren();

//	save_vBox.setX(saveWindow.getX() + 6);

//	save_vBox.setY(saveWindow.getY() + 42);

//	save_vBox.setWidth(180);

//	save_vBox.setHeight(137);

//	saveWindow.addChild(&save_vBox);

//	save_vBox.setVisible(true);



	int startX = saveWindow.getX() + 6;

	int startY = saveWindow.getY() + 42;



	SDL_Surface *slotSurf = (SDL_Surface *)dataFile[UI_GameSlot].dat;



	for(i = 0; i < NUM_SAVEGAMES; i++)

	{

		gameSaveSpotButton[i].setCallBack(game_setSaveGameSpot, (void*)i);

	//	load_vBox.addChild(&gameLoadSpotButton[i]);

		gameSaveSpotButton[i].setX(startX);

		gameSaveSpotButton[i].setY(startY);

		gameSaveSpotButton[i].setSurface(slotSurf, NULL);

		gameSaveSpotButton[i].setHeight(slotSurf->h);

		gameSaveSpotButton[i].setWidth(slotSurf->w);

		gameSaveSpotButton[i].setVisible(true);

		gameSaveSpotButton[i].setTransparent(true);



		saveWindow.addChild(&gameSaveSpotButton[i]);

		startY += slotSurf->h + 2;

	}



	gameSaveNameEntry.setWidth(slotSurf->w);

	gameSaveNameEntry.setHeight(slotSurf->h);

	gameSaveNameEntry.setCallBackCancel(game_cancelSaveEntry);

	gameSaveNameEntry.setCallBackConfirm(game_saveEntry);

	saveWindow.addChild(&gameSaveNameEntry);



	

//	save_vBox.addChild(&save_hBox);

//	save_hBox.setVisible(true);



	//saveButton.setText("Save");
	saveButton.setSurface(saveSurf, NULL);
	saveButton.setPressedSurface(saveSurfPressed, NULL);
	saveButton.setHeight(saveSurf->h);
	saveButton.setWidth(saveSurf->w);
	saveButton.setX(saveWindow.getX() + 37);
	saveButton.setY(saveWindow.getY() + 187);
	saveButton.setCallBack(game_saveEntry);
	saveWindow.addChild(&saveButton);
	saveButton.setVisible(true);



	

	//gameCancelButton.setText("Cancel");
	gameCancelButton.setSurface(cancelSurf, NULL);
	gameCancelButton.setPressedSurface(cancelSurfPressed, NULL);
	gameCancelButton.setHeight(cancelSurf->h);
	gameCancelButton.setWidth(cancelSurfPressed->w);
	gameCancelButton.setX(saveWindow.getX() + 87);
	gameCancelButton.setY(saveWindow.getY() + 187);
	gameCancelButton.setCallBack(game_cancel);
	saveWindow.addChild(&gameCancelButton);
	gameCancelButton.setVisible(true);



	//------------------



	loadWindow.dropChildren();

	loadWindow.setX(screen->w/4);

	loadWindow.setY(screen->h/4);

//	loadWindow.setWidth(screen->w/2);

//	loadWindow.setHeight(screen->h/2);

	loadWindow.setSurface(surf, NULL);

	loadWindow.setWidth(surf->w);

	loadWindow.setHeight(surf->h);



	loadWindow.setVisible(true);



	load_vBox.dropChildren();

	load_hBox.dropChildren();



//	load_vBox.setX(loadWindow.getX() + 6);

//	load_vBox.setY(loadWindow.getY() + 42);

//	load_vBox.setWidth(180);

//	load_vBox.setHeight(137);

//	loadWindow.addChild(&load_vBox);

//	load_vBox.setVisible(true);



	startX = saveWindow.getX() + 6;

	startY = saveWindow.getY() + 42;



	slotSurf = (SDL_Surface *)dataFile[UI_GameSlot].dat;



	for(i = 0; i < NUM_SAVEGAMES; i++)

	{

		gameLoadSpotButton[i].setCallBack(game_loadEntry, (void*)i);

	//	load_vBox.addChild(&gameLoadSpotButton[i]);

		gameLoadSpotButton[i].setX(startX);

		gameLoadSpotButton[i].setY(startY);

		gameLoadSpotButton[i].setSurface(slotSurf, NULL);

		gameLoadSpotButton[i].setHeight(slotSurf->h);

		gameLoadSpotButton[i].setWidth(slotSurf->w);

		gameLoadSpotButton[i].setVisible(true);

		gameLoadSpotButton[i].setTransparent(true);



		loadWindow.addChild(&gameLoadSpotButton[i]);

		startY += slotSurf->h + 2;

	}



	//load_vBox.addChild(&gameSaveNameEntry);



	

//	load_vBox.addChild(&load_hBox);

//	load_hBox.setVisible(true);



	

//	loadButton.setText("Load");
	loadButton.setSurface(loadSurf, NULL);
	loadButton.setPressedSurface(loadSurfPressed, NULL);
	loadButton.setWidth(loadSurf->w);
	loadButton.setHeight(loadSurf->h);
	loadButton.setVisible(true);
	loadButton.setX(loadWindow.getX() + 37);
	loadButton.setY(loadWindow.getY() + 181);
	loadButton.setCallBack(game_loadEntry, (void*)0);
//	load_hBox.addChild(&loadButton);

	loadButton.setEnabled(false);	//don't need it

//	loadWindow.addChild(&loadButton);

	loadWindow.addChild(&gameCancelButton);


//	load_hBox.addChild(&gameCancelButton);

//	i = gameSaveNameEntry.getWidth();

//	gameSaveNameEntry.setX(gameSaveNameEntry.getX() + i/5);

//	gameSaveNameEntry.setWidth(3*i/5);

	//make the save name box smaller so it looks different



	//////////////////////////////////////////////////////////////////////////

	//game_loadMap();
	*/

	//surf = (SDL_Surface *)dataFile[UI_OptionsMenu].dat;
	surf = settingsWindowSurf;

	settingsWindow.dropChildren();
	settingsWindow.setX((screen->w - surf->w) / 2);

	settingsWindow.setY((screen->h - surf->h) / 2);

	settingsWindow.setWidth(surf->w);

	settingsWindow.setHeight(surf->h);
	settingsWindow.setSurface(surf, NULL);
	settingsWindow.setVisible(true);

//	surf = (SDL_Surface *)dataFile[UI_OptionsMenu].dat;
//	sur = (SDL_Surface *)dataFile[UI_OptionsMenu].dat;

	settingsCancelButton.setSurface(settingsCancelSurf, NULL);
	settingsCancelButton.setPressedSurface(settingsCancelSurfPressed, NULL);
	settingsCancelButton.setX(settingsWindow.getX() + 87);
	settingsCancelButton.setY(settingsWindow.getY() + 133);
	settingsCancelButton.setWidth(settingsCancelSurf->w);
	settingsCancelButton.setHeight(settingsCancelSurf->h);
	settingsCancelButton.setCallBack(settings_cancel);
	settingsCancelButton.setVisible(true);

	settingsOkButton.setSurface(settingsOKSurf, NULL);
	settingsOkButton.setPressedSurface(settingsOKSurfPressed, NULL);
	settingsOkButton.setX(settingsWindow.getX() + 49);
	settingsOkButton.setY(settingsWindow.getY() + 133);
	settingsOkButton.setWidth(settingsOKSurf->w);
	settingsOkButton.setHeight(settingsOKSurf->h);
	settingsOkButton.setCallBack(settings_ok);
	settingsOkButton.setVisible(true);

	settingsGameSpeedPlus.setSurface(settingsPlus, NULL);
	settingsGameSpeedPlus.setPressedSurface(settingsPlusPressed, NULL);
	settingsGameSpeedPlus.setX(settingsWindow.getX() + 176);
	settingsGameSpeedPlus.setY(settingsWindow.getY() + 53);
	settingsGameSpeedPlus.setWidth(settingsPlus->w);
	settingsGameSpeedPlus.setHeight(settingsPlus->h);
	settingsGameSpeedPlus.setCallBack(settings_plus_gamespeed);
	settingsGameSpeedPlus.setVisible(true);

	settingsGameSpeedMinus.setSurface(settingsMinus, NULL);
	settingsGameSpeedMinus.setPressedSurface(settingsMinusPressed, NULL);
	settingsGameSpeedMinus.setX(settingsWindow.getX() + 5);
	settingsGameSpeedMinus.setY(settingsWindow.getY() + 53);
	settingsGameSpeedMinus.setWidth(settingsMinus->w);
	settingsGameSpeedMinus.setHeight(settingsMinus->h);
	settingsGameSpeedMinus.setCallBack(settings_minus_gamespeed);
	settingsGameSpeedMinus.setVisible(true);

	settingsVolumePlus.setSurface(settingsPlus, NULL);
	settingsVolumePlus.setPressedSurface(settingsPlusPressed, NULL);
	settingsVolumePlus.setX(settingsWindow.getX() + 176);
	settingsVolumePlus.setY(settingsWindow.getY() + 85);
	settingsVolumePlus.setWidth(settingsPlus->w);
	settingsVolumePlus.setHeight(settingsPlus->h);
	settingsVolumePlus.setCallBack(settings_plus_volume);
	settingsVolumePlus.setVisible(true);

	settingsVolumeMinus.setSurface(settingsMinus, NULL);
	settingsVolumeMinus.setPressedSurface(settingsMinusPressed, NULL);
	settingsVolumeMinus.setX(settingsWindow.getX() + 5);
	settingsVolumeMinus.setY(settingsWindow.getY() + 85);
	settingsVolumeMinus.setWidth(settingsMinus->w);
	settingsVolumeMinus.setHeight(settingsMinus->h);
	settingsVolumeMinus.setCallBack(settings_minus_volume);
	settingsVolumeMinus.setVisible(true);

	settingsGameSpeedBar.setBackgroundColour(thisPlayer->getColour());
	settingsGameSpeedBar.setBorderColour(thisPlayer->getColour());
	settingsGameSpeedBar.setTextColour(thisPlayer->getColour());
	settingsGameSpeedBar.setHeight(6);
	settingsGameSpeedBar.setWidth(100);
	settingsGameSpeedBar.setX(settingsWindow.getX() + 20);
	settingsGameSpeedBar.setY(settingsWindow.getY() + 57);
	settingsGameSpeedBar.setVisible(true);

	settingsVolumeBar.setBackgroundColour(thisPlayer->getColour());
	settingsVolumeBar.setBorderColour(thisPlayer->getColour());
	settingsVolumeBar.setTextColour(thisPlayer->getColour());
	settingsVolumeBar.setHeight(5);
	settingsVolumeBar.setWidth(100);
	settingsVolumeBar.setX(settingsWindow.getX() + 20);
	settingsVolumeBar.setY(settingsWindow.getY() + 88);
	settingsVolumeBar.setVisible(true);

	settingsWindow.addChild(&settingsOkButton);
	settingsWindow.addChild(&settingsCancelButton);
	settingsWindow.addChild(&settingsGameSpeedPlus);
	settingsWindow.addChild(&settingsGameSpeedMinus);
	settingsWindow.addChild(&settingsVolumePlus);
	settingsWindow.addChild(&settingsVolumeMinus);
	settingsWindow.addChild(&settingsVolumeBar);
	settingsWindow.addChild(&settingsGameSpeedBar);

}




void game_loop(void)

{

	int		i, count,
			frameStart, frameEnd, frameTime,

			frameTimeTotal, numFrames,

			tic;



	setupView();

	///////////////////////////



	//clear messages

	for(count = 0; count < MESSAGE_BUFFER_SIZE; count++)

		messageTimer[count] = 0;



	settings.finished = false;

	settings.update = true;

	settings.won = false;

	gameState = BEGUN;



	//settings.gamespeed = 10;

	tic = 0;

	frameTimeTotal = numFrames = 0;

	frameStart = SDL_GetTicks();



	//setup endlevel conditions

	endTimer = END_WAIT_TIMER;

	finishedLevel = false;



	//main game loop
	try
	{
		do
		{

			do

			{

				drawScreen();
				
				SDL_Flip(screen);
				
				frameEnd = SDL_GetTicks();
			} while (frameEnd == frameStart);



			frameTime = frameEnd - frameStart; // find difference to get frametime

			frameStart = SDL_GetTicks();



			frameTimeTotal += frameTime;

			numFrames++;



			if (showFPS)

			{

				fps = 1.0/(((double)frameTime)/1000.0);

				frameTimer = frameTime;

			}


			while (frameTime--)
			{
				doInput();

				if (settings.update && (tic % settings.gamespeed == 0))
				{
					if ((settings.gameType == RANDOMMAP) && (unitList->getNumElements() < 1000))
					{
						bool valid = true;
						int	ranCount = 0,
							ranPlayer = getRandomInt(0, 1),//NUM_HOUSES-1),
							ranType,
							i, j;
						do
						{
							i = getRandomInt(0, map->sizeX-1);
							j = getRandomInt(0, map->sizeY-1);
							valid = (ranCount++ < 100);
						} while (valid && map->cell[i][j].hasAnObject());
						
						if (valid)
						{
							do
								ranType = getRandomInt(Unit_Devastator, Unit_Trooper);
							while ((ranType == Unit_DevastatorBase)
								|| (ranType == Unit_Frigate)
								|| (ranType == Unit_Harvester)
								|| (ranType == Unit_HarvesterSand)
								|| (ranType == Unit_MCV)
								|| (ranType == Unit_Saboteur)
								//|| (ranType == Unit_Sandworm)
								|| (ranType == Unit_SiegeTankBase)
								|| (ranType == Unit_TankBase));
								//ranType != Unit_Devastator && ranType != Unit_Deviator);
							player[ranPlayer]->placeUnit(ranType, i, j);

						}
					}



					netUpdate();

					for (i = 0; i < MAX_PLAYERS; i++)
					if (player[i])
						player[i]->update();

					processObjects();



					if ((indicatorFrame != NONE) && (--indicatorTimer <= 0))

					{

						indicatorTimer = indicatorTime;

						if (++indicatorFrame > 2)

							indicatorFrame = NONE;

					}

				}

				tic++;
			}
			soundPlayer->musicCheck();	//if song has finished, start playing next one
		} while (!quitGame && !finishedLevel);//not sure if we need this extra bool
	}
	catch (...)
	{
		fprintf(stderr, "Argghhh. Exception caught: main game loop\n");
	}

	if (settings.playerType == SERVER)
	{
		//close all the sockets

		for(i = 1; i < MAX_PLAYERS; i++)

		if (sockets[i] != NULL)
		{
			settings.playerName[i][0] = '\0';
			SDLNet_TCP_DelSocket(socketSet, sockets[i]);

			SDLNet_TCP_Close(sockets[i]);
		}
		SDLNet_TCP_DelSocket(socketSet, server);

		SDLNet_TCP_Close(server);
	}
	else if (settings.playerType == CLIENT)
	{
		//close server socket

		SDLNet_TCP_DelSocket(socketSet, server);

		SDLNet_TCP_Close(server);
	}


	while(!structureList->isEmpty())
		delete ((StructureClass*)structureList->removeFirstElement());


	while(!unitList->isEmpty())

		delete ((UnitClass*)unitList->removeFirstElement());




	if(loadingGame)
	{
		game_loadEntryCallback(gameNum);


		/*
		mentat->setHouse(thisPlayer->getHouse());
		showMentat = false;
		mapHouseColours();
		*/

		/*
		for (i = 0; i < MAX_PLAYERS; i++)
		{
			if (strlen(settings.playerName[i]))
			{
				player[i]->setTeam(settings.playerTeam[i]);
				player[i]->assignMapPlayerNum(settings.playerMapNumber[i]);
			}
		}*/
		
		do_game();
		loadingGame = false;
		finishedLevel = false;
		game_loop();
	}

	else

	{

		delete structureList;

		delete unitList;



		delete selectedList;

		delete[] selectedLists;

		while(!bulletList->isEmpty())
			delete ((BulletClass*)bulletList->removeFirstElement());
		delete bulletList;

		delete map;
		map = NULL;
		delete dborder;

		for (i = 0; i < NUM_HOUSES; i++)
		if (player[i])
			delete player[i];

		cursorFrame = CURSOR_NORMAL;
		focusedWidget = NULL;
	}

	printf("Average frameTime:%d", frameTimeTotal/numFrames);

}

void game_mentat(void *var)
{
	//
	if(mentat == NULL)
	{
		showMessage("Mentat failed to load");
	}
	else
	{
		showMentat = true;

		if (settings.playerType == SINGLE)
				settings.update = false;
	}
}

void game_mentat_exited()
{
	if(mentat->doExit())
	{
		showMentat = false;

		if (settings.playerType == SINGLE)
				settings.update = true;
	}
}

void game_options(void *var)
{
	if (!menuMode)
	{
		menuMode = true;
		menuInterface = &menuWindow;
		if (settings.playerType == SINGLE)
			settings.update = false;
	}
}

void setMentat(MentatClass *m)
{
	mentat = m;
}

int resolveItemPicture(int id)
{
	int newPicID = -1;

	switch(id)
	{
		case Unit_Carryall:
			newPicID = Picture_Carryall;
		break;

		case Unit_Devastator:
			newPicID = Picture_Devastator;
		break;

		case Unit_Deviator:
			newPicID = Picture_Deviator;
		break;

		case Unit_Harvester:
			newPicID = Picture_Harvester;
		break;

		case Unit_Launcher:
			newPicID = Picture_Launcher;
		break;

		case Unit_MCV:
			newPicID = Picture_MCV;
		break;

		case Unit_Ornithopter:
			newPicID = Picture_Ornithopter;
		break;

		case Unit_Quad:
			newPicID = Picture_Quad;
		break;

		case Unit_Raider:
			newPicID = Picture_Raider;
		break;

		case Unit_SiegeTank:
			newPicID = Picture_SiegeTank;
		break;

		case Unit_SonicTank:
			newPicID = Picture_SonicTank;
		break;

		case Unit_Tank:
			newPicID = Picture_Tank;
		break;

		case Unit_Trike:
			newPicID = Picture_Trike;
		break;

		case Unit_Infantry:
			newPicID = Picture_Infantry;
		break;

		case Unit_Trooper:
			newPicID = Picture_Trooper;
		break;

		case Unit_Saboteur:
			newPicID = Picture_Saboteur;
		break;

		case Unit_Fremen:
			newPicID = Picture_Fremen;
		break;

		case Unit_Sardaukar:
			newPicID = Picture_Sardaukar;
		break;
		
		}

		return newPicID;
}

void init_save_load_windows()
{
	int i;
	SDL_Surface *surf = (SDL_Surface *)dataFile[UI_LoadSaveWindow].dat;



	if(!cancelSurf)

	{

		cancelSurf = ((SDL_Surface*)dataFile[UI_Cancel].dat);
		cancelSurfPressed = ((SDL_Surface*)dataFile[UI_Cancel_Pressed].dat);

		saveSurf = ((SDL_Surface*)dataFile[UI_Save].dat);
		saveSurfPressed = ((SDL_Surface*)dataFile[UI_Save_Pressed].dat);

		loadSurf = ((SDL_Surface*)dataFile[UI_Load].dat);
		loadSurfPressed = ((SDL_Surface*)dataFile[UI_Load_Pressed].dat);

	}



	saveWindow.dropChildren();

	

	saveWindow.setSurface(surf, NULL);

	saveWindow.setX(screen->w/4);

	saveWindow.setY(screen->h/4);

//	saveWindow.setWidth(screen->w/2);

//	saveWindow.setHeight(screen->h/2);

	saveWindow.setWidth(surf->w);

	saveWindow.setHeight(surf->h);

	saveWindow.setVisible(true);



//	save_vBox.dropChildren();

//	save_hBox.dropChildren();

//	save_vBox.setX(saveWindow.getX() + 6);

//	save_vBox.setY(saveWindow.getY() + 42);

//	save_vBox.setWidth(180);

//	save_vBox.setHeight(137);

//	saveWindow.addChild(&save_vBox);

//	save_vBox.setVisible(true);



	/*

	for(i = 0; i < NUM_SAVEGAMES; i++)

	{

		gameSaveSpotButton[i].setCallBack(game_setSaveGameSpot, (void*)i);

		save_vBox.addChild(&gameSaveSpotButton[i]);

		gameSaveSpotButton[i].setVisible(true);

		gameSaveSpotButton[i].setTransparent(true);

	}*/



	int startX = saveWindow.getX() + 6;

	int startY = saveWindow.getY() + 42;



	SDL_Surface *slotSurf = (SDL_Surface *)dataFile[UI_GameSlot].dat;



	for(i = 0; i < NUM_SAVEGAMES; i++)

	{

		gameSaveSpotButton[i].setCallBack(game_setSaveGameSpot, (void*)i);

	//	load_vBox.addChild(&gameLoadSpotButton[i]);

		gameSaveSpotButton[i].setX(startX);

		gameSaveSpotButton[i].setY(startY);

		gameSaveSpotButton[i].setSurface(slotSurf, NULL);

		gameSaveSpotButton[i].setHeight(slotSurf->h);

		gameSaveSpotButton[i].setWidth(slotSurf->w);

		gameSaveSpotButton[i].setVisible(true);

		gameSaveSpotButton[i].setTransparent(true);



		saveWindow.addChild(&gameSaveSpotButton[i]);

		startY += slotSurf->h + 2;

	}



	gameSaveNameEntry.setWidth(slotSurf->w);

	gameSaveNameEntry.setHeight(slotSurf->h);

	gameSaveNameEntry.setCallBackCancel(game_cancelSaveEntry);

	gameSaveNameEntry.setCallBackConfirm(game_saveEntry);

	saveWindow.addChild(&gameSaveNameEntry);



	

//	save_vBox.addChild(&save_hBox);

//	save_hBox.setVisible(true);



	//saveButton.setText("Save");
	saveButton.setSurface(saveSurf, NULL);
	saveButton.setPressedSurface(saveSurfPressed, NULL);
	saveButton.setHeight(saveSurf->h);
	saveButton.setWidth(saveSurf->w);
	saveButton.setX(saveWindow.getX() + 37);
	saveButton.setY(saveWindow.getY() + 187);
	saveButton.setCallBack(game_saveEntry);
	saveWindow.addChild(&saveButton);
	saveButton.setVisible(true);



	

	//gameCancelButton.setText("Cancel");
	gameCancelButton.setSurface(cancelSurf, NULL);
	gameCancelButton.setPressedSurface(cancelSurfPressed, NULL);
	gameCancelButton.setHeight(cancelSurf->h);
	gameCancelButton.setWidth(cancelSurfPressed->w);
	gameCancelButton.setX(saveWindow.getX() + 87);
	gameCancelButton.setY(saveWindow.getY() + 187);
	gameCancelButton.setCallBack(game_cancel);
	saveWindow.addChild(&gameCancelButton);
	gameCancelButton.setVisible(true);



	//------------------



	loadWindow.dropChildren();

	loadWindow.setX(screen->w/4);

	loadWindow.setY(screen->h/4);

//	loadWindow.setWidth(screen->w/2);

//	loadWindow.setHeight(screen->h/2);

	loadWindow.setSurface(surf, NULL);

	loadWindow.setWidth(surf->w);

	loadWindow.setHeight(surf->h);



	loadWindow.setVisible(true);



	load_vBox.dropChildren();

	load_hBox.dropChildren();



//	load_vBox.setX(loadWindow.getX() + 6);

//	load_vBox.setY(loadWindow.getY() + 42);

//	load_vBox.setWidth(180);

//	load_vBox.setHeight(137);

//	loadWindow.addChild(&load_vBox);

//	load_vBox.setVisible(true);



	startX = saveWindow.getX() + 6;

	startY = saveWindow.getY() + 42;



	slotSurf = (SDL_Surface *)dataFile[UI_GameSlot].dat;



	for(i = 0; i < NUM_SAVEGAMES; i++)

	{

		gameLoadSpotButton[i].setCallBack(game_loadEntry, (void*)i);

	//	load_vBox.addChild(&gameLoadSpotButton[i]);

		gameLoadSpotButton[i].setX(startX);

		gameLoadSpotButton[i].setY(startY);

		gameLoadSpotButton[i].setSurface(slotSurf, NULL);

		gameLoadSpotButton[i].setHeight(slotSurf->h);

		gameLoadSpotButton[i].setWidth(slotSurf->w);

		gameLoadSpotButton[i].setVisible(true);

		gameLoadSpotButton[i].setTransparent(true);



		loadWindow.addChild(&gameLoadSpotButton[i]);

		startY += slotSurf->h + 2;

	}



	//load_vBox.addChild(&gameSaveNameEntry);



	

//	load_vBox.addChild(&load_hBox);

//	load_hBox.setVisible(true);



	

//	loadButton.setText("Load");
	loadButton.setSurface(loadSurf, NULL);
	loadButton.setPressedSurface(loadSurfPressed, NULL);
	loadButton.setWidth(loadSurf->w);
	loadButton.setHeight(loadSurf->h);
	loadButton.setVisible(true);
	loadButton.setX(loadWindow.getX() + 37);
	loadButton.setY(loadWindow.getY() + 181);
	loadButton.setCallBack(game_loadEntry, (void*)0);
//	load_hBox.addChild(&loadButton);

	loadButton.setEnabled(false);	//don't need it

//	loadWindow.addChild(&loadButton);

	loadWindow.addChild(&gameCancelButton);


//	load_hBox.addChild(&gameCancelButton);

//	i = gameSaveNameEntry.getWidth();

//	gameSaveNameEntry.setX(gameSaveNameEntry.getX() + i/5);

//	gameSaveNameEntry.setWidth(3*i/5);
}

