/*					    
 *  Anthony Cole
 *
 */

#include "sand.h"
#include "Items.h"
//#include "gui/gui.h"

typedef enum
{
	BUTTON_SAND,
	BUTTON_BLOOM,
	BUTTON_DUNES,
	BUTTON_SPICE,
	BUTTON_THICKSPICE,
	BUTTON_ROCK,
	BUTTON_MOUNTAIN
} BUTTON_TYPE;

typedef enum
{
	PLACING_STRUCTURE,
	PLACING_TERRAIN,
	PLACING_UNIT
} PLACING_TYPE;

Button		*floodButtonP,
			*sizeButtonsP;

Container	*editorContainer,
			*editorContainer2,
			*structureContainerP,
			*terrainContainerP,
			*unitContainerP;

bool	floodFill;
int		placing;
int		placeSize,
		terrainType,
		terrainTile,
		unitType;

BuildItemClass buildItem;

PLACING_TYPE placingType;

COORDTYPE lastPlacedSpot = {NONE, NONE};

/*set this if editor is used
and unset when leaving*/
bool edit_mode;

void saveMap(char* filename)
{
	bool found[MAX_PLAYERS];
	char temp[256];
	int i, j, x, y, magicNum = MAPMAGIC, maxPlayers, numSlabs;
	FILE* fp = fopen(filename, "wb");
	StructureClass* structure;
	UnitClass* unit;

	//find out how many players in map
	for (i = 0; i < MAX_PLAYERS; i++)
		found[i] = false;
	maxPlayers = 0;
	unitList->reset();
	while(unitList->currentNotNull())
	{
		j = ((UnitClass*)unitList->getCurrent())->getOwner()->getPlayerNumber();
		if (!found[j])
		{
			found[j] = true;
			maxPlayers++;
		}
		unitList->nextLink();
	}//
	structureList->reset();
	while(structureList->currentNotNull())
	{
		j = ((StructureClass*)structureList->getCurrent())->getOwner()->getPlayerNumber();
		if (!found[j])
		{
			found[j] = true;
			maxPlayers++;
		}
		structureList->nextLink();
	}//

	if (fp)
	{
		//write the magic number
		fwrite(&magicNum, sizeof(int), 1, fp);

		if (maxPlayers <= 0)
			showMessage("WARNING: no players added to map, it will not be playable.");
		//write the number of players
		fwrite(&maxPlayers, sizeof(int), 1, fp);

		//write the players details
		for (i=0; i<maxPlayers; i++)
		if (found[i])
			fwrite(&i, sizeof(int), 1, fp);

		numSlabs = 0;
		//write the terrain info
		fwrite(&map->sizeX, sizeof(short), 1, fp);
		fwrite(&map->sizeY, sizeof(short), 1, fp);
		for (i=0; i<map->sizeX; i++)
		for (j=0; j<map->sizeY; j++)
		{
			x = map->getCell(i, j)->getType();
			if (x == Structure_Slab1)
			{
				numSlabs++;
				x = Terrain_Rock;	//record slabs as structures later
			}
			y = map->getCell(i, j)->getTile();
			fwrite(&x, sizeof(int), 1, fp);
			fwrite(&y, sizeof(int), 1, fp);
		}

		//write the structure info
		structureList->reset();
		x = structureList->getNumElements() + numSlabs;
		fwrite(&x, sizeof(int), 1, fp);
		while(structureList->currentNotNull())
		{
			structure = (StructureClass*)structureList->getCurrent();
			x = structure->getOwner()->getPlayerNumber();
			fwrite(&x, sizeof(int), 1, fp);
			x = structure->getItemID();
			fwrite(&x, sizeof(int), 1, fp);
			x = structure->getLocation()->x;
			y = structure->getLocation()->y;
			fwrite(&x, sizeof(int), 1, fp);
			fwrite(&y, sizeof(int), 1, fp);
			if ((structure->getItemID() == Structure_GunTurret)
				|| (structure->getItemID() == Structure_RocketTurret))
			{
				x = ((GunTurretClass*)structure)->getTurretAngle();
				fwrite(&x, sizeof(int), 1, fp);	
			}

			structureList->nextLink();
		}

		//write slabs as structures
		for (i=0; i<map->sizeX; i++)
		for (j=0; j<map->sizeY; j++)
		if (map->getCell(i, j)->getType() == Structure_Slab1)
		{
			x = map->getCell(i, j)->getOwner();
			fwrite(&x, sizeof(int), 1, fp);
			x = Structure_Slab1;
			fwrite(&x, sizeof(int), 1, fp);
			fwrite(&i, sizeof(int), 1, fp);	//write location			
			fwrite(&j, sizeof(int), 1, fp);
		}

		//write the unit info
		unitList->reset();
		x = unitList->getNumElements();
		fwrite(&x, sizeof(int), 1, fp);
		while(unitList->currentNotNull())
		{
			unit = (UnitClass*)unitList->getCurrent();
			x = unit->getOwner()->getPlayerNumber();
			fwrite(&x, sizeof(int), 1, fp);
			x = unit->getItemID();
			fwrite(&x, sizeof(int), 1, fp);
			x = unit->getLocation()->x;
			y = unit->getLocation()->y;
			fwrite(&x, sizeof(int), 1, fp);
			fwrite(&y, sizeof(int), 1, fp);
			x = unit->getAngle();
			fwrite(&x, sizeof(int), 1, fp);
			if (unit->isTurreted())
			{
				x = ((TankClass*)unit)->getTurretAngle();
				fwrite(&x, sizeof(int), 1, fp);				
			}

			unitList->nextLink();
		}

		fclose(fp);
		sprintf(temp, "map saved to \"%s\".", filename);
		showMessage(temp);
	}
	else
	{
		sprintf(temp, "Could not save map to \"%s\".", filename);
		showMessage(temp);
	}
}

void setHouse(void* data)
{
	thisPlayer = (PlayerClass*)data;
	mapImageHouseColour((SDL_Surface*)dataFile[UI_GameBar].dat, thisPlayer->getColour());
}

void setPlaceSize(void* data)
{
	placeSize = (int)data;
	floodFill = false;
	floodButtonP->setToggled(false);

	for(int i=0; i<5; i++)
	if (i != (placeSize - 1)/2)
		sizeButtonsP[i].setToggled(false);
	else
		sizeButtonsP[i].setToggled(true);
}

void setPlacingType(void* data)
{
	PLACING_TYPE type = (PLACING_TYPE)((int)data);

	switch(type)
	{
	case PLACING_STRUCTURE:
		editorContainer2 = structureContainerP;
		break;
	case PLACING_TERRAIN:
		editorContainer2 = terrainContainerP;
		break;
	case PLACING_UNIT:
		editorContainer2 = unitContainerP;
		break;
	}
}

void setStructureType(void* data)
{
	buildItem = BuildItemClass((int)data);

	placingType = PLACING_STRUCTURE;
	placingMode = true;
}

void setTerrainTile(void* data)
{
	BUTTON_TYPE	number = (BUTTON_TYPE)((int)data);

	switch(number)
	{
	case BUTTON_SAND:
		terrainType = Terrain_Sand;
		terrainTile = Terrain_a1;
		break;
	case BUTTON_BLOOM:
		if ((terrainType == Terrain_Sand) && (terrainTile == Terrain_a2))
			terrainTile = Terrain_a3;
		else
			terrainTile = Terrain_a2;
		terrainType = Terrain_Sand;

		break;
	case BUTTON_DUNES:
		terrainType = Terrain_Dunes;
		terrainTile = Terrain_DunesFull;
		break;
	case BUTTON_SPICE:
		terrainType = Terrain_Spice;
		terrainTile = Terrain_s1;
		break;
	case BUTTON_THICKSPICE:
		terrainType = Terrain_ThickSpice;
		terrainTile = Terrain_ThickSpiceFull;
		break;
	case BUTTON_ROCK:
		terrainType = Terrain_Rock;
		terrainTile = Terrain_t1;
		break;
	case BUTTON_MOUNTAIN:
		terrainType = Terrain_Mountain;
		terrainTile = Terrain_MountainFull;
		break;
	default:
		terrainType = Terrain_Sand;
		terrainTile = Terrain_a1;
		break;
	}

	placingType = PLACING_TERRAIN;
	placingMode = true;
}

void setUnitType(void* data)
{
	unitType = (int)data;

	placingType = PLACING_UNIT;
	placingMode = true;
}

void toogleFlood()
{
	floodFill = true;
	placeSize = 1;
	floodButtonP->setToggled(true);
	for(int i=0; i<5; i++)
		sizeButtonsP[i].setToggled(false);
}

void setTerrain(int x1, int y1, int x2, int y2)
{
	int i,j, x,y;

	for (i = x1; i<=x2; i++)
	for (j = y1; j<=y2; j++)
	if (map->cellExists(i, j))
	{
		map->getCell(i, j)->setType(terrainType);
		map->getCell(i, j)->setTile(terrainTile);
	}

	if (terrainType == Terrain_Mountain)
	{
		for (i = x1-1; i<=x2+1; i++)
		for (j = y1-1; j<=y2+1; j++)
			if (map->cellExists(i, j) && !map->getCell(i, j)->isRock())
				map->getCell(i, j)->setType(Terrain_Rock);
	}
	else if (terrainType == Terrain_ThickSpice)
	{
		for (i = x1-1; i<=x2+1; i++)
		for (j = y1-1; j<=y2+1; j++)
			if (map->cellExists(i, j) && !map->getCell(i, j)->isSpice())
				map->getCell(i, j)->setType(Terrain_Spice);
	}

	//if a terrain is put down, make sure that mountains all sill have rock around them, and thick spice has normal spice around it
	//otherwise convert them to rock if mountain or normal spice if thick spice.
	for (x = x1-2; x<=x2+2; x++)
	for (y = y1-2; y<=y2+2; y++)
	if (map->cellExists(x, y))
	{
		if  (!map->getCell(x, y)->isRock())
			for (i = x-1; i<=x+1; i++)
			for (j = y-1; j<=y+1; j++)
				if (map->cellExists(i, j) && map->getCell(i, j)->isMountain())
					map->getCell(i, j)->setType(Terrain_Rock);

		if (!map->getCell(x, y)->isSpice())
			for (i = x-1; i<=x+1; i++)
			for (j = y-1; j<=y+1; j++)
				if (map->cellExists(i, j) && map->getCell(i, j)->isThickSpice())
					map->getCell(i, j)->setType(Terrain_Spice);
	}

	for (i = x1-3; i<=x2+3; i++)
	for (j = y1-3; j<=y2+3; j++)
		if (map->cellExists(i, j))
			smooth_spot(i, j);

	placing = true;
}

Uint32 EditorTimeLeft()
{
    static Uint32 next_time = 0;
    Uint32 now;

    now = SDL_GetTicks();
    if ( next_time <= now ) {
        next_time = now+TICK_INTERVAL;
        return(0);
    }
    return(next_time-now);
}

void doEditorInput()
{
	ObjectClass* tempObject;
	SDL_Event event;
	while(SDL_PollEvent(&event))	//check for a key press
	{
		/* Look for a keypress */
		switch (event.type)
		{
		case (SDL_KEYDOWN):
			switch( event.key.keysym.sym )
			{
				case SDLK_DELETE:
					if (!selectedList->isEmpty())
					{
						selectedList->reset();
						while(selectedList->currentNotNull())
						{
							tempObject = (ObjectClass*)selectedList->getCurrent();

							

							selectedList->removeCurrent();

							tempObject->setSelected(false);

							tempObject->destroy();
						}
					}
					break;
				case SDLK_ESCAPE:
					quitGame = true;
					break;
				case SDLK_LCTRL:
					groupCreateMode = true;
					break;
				case SDLK_LSHIFT:
					shift = true;
					break;
				case SDLK_F12:
					soundPlayer->toggleSound();
					break;
				case SDLK_TAB:
					if (fullscreenMode)
					{
						fullscreenMode = false;
						dborder->maxX -= gameBarPos.w;
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
						radarMode = false;	//once press tab we can no longer be stuck in radar square
					}
					break;
				case SDLK_s:
					saveMap("map.dlm");
					break;
				case SDLK_p:
					SDL_SaveBMP(screen, "screenshot.bmp");
					break;
				default:
					break;
			}
			break;
		case (SDL_KEYUP):
			switch( event.key.keysym.sym )
			{
				case SDLK_LCTRL:
					groupCreateMode = false;
					break;
				case SDLK_LSHIFT:
					shift = false;
					break;
				default:
					 break;
			}
			break;
		case SDL_MOUSEBUTTONDOWN:
		{
			SDL_MouseButtonEvent* mouse = &event.button;
			switch (mouse->button)
			{
				case (SDL_BUTTON_LEFT):
				{
					int xPos = (mouse->x + dborder->minX)/BLOCKSIZE,
						yPos = (mouse->y + dborder->minY)/BLOCKSIZE;

					if (placingMode)
					{
						if (fullscreenMode || (mouse->x < gameBarPos.x))	//if mouse is not over game bar
						{
							if (placingType == PLACING_STRUCTURE)
							{
								BuildItemClass* placeItem = &buildItem;

								if (placeItem->getItemID() == Structure_Slab4)
								{
									int i,j;
									for(i = xPos; i < xPos + 2; i++)
									for(j = yPos; j < yPos + 2; j++)
									if (map->cellExists(i, j))
									{
										TerrainClass* cell = map->getCell(i, j);

										if (!cell->hasAnObject() && cell->isRock() && !cell->isMountain())
											thisPlayer->placeStructure(NONE, NONE, NONE, Structure_Slab1, i, j);
									}
								}
								else if (map->okayToPlaceStructure(xPos, yPos, placeItem->getStructureSizeX(), placeItem->getStructureSizeY(), false, NULL))
									thisPlayer->placeStructure(NONE, NONE, NONE, placeItem->getItemID(), xPos, yPos);

								if (placingMode)	//the user has tried to place but clicked on 
								{					//impossible point
									showMessage("You cannot place it here.");
									soundPlayer->playSfxSound(Sound_cannot);	//can't place noise
								}
								else				//the user has tried to place and has been
									soundPlayer->playSfxSound(Sound_placeStructure);

								placingMode = true;
							}
							else if (placingType == PLACING_TERRAIN)
							{
								if (map->cellExists(xPos, yPos))
								{
									int angle, i, j, fillMatch;
									COORDTYPE pos;
									TerrainClass* cell;
									LinkedList queue;
									fillMatch = map->getCell(xPos, yPos)->getType();

									for (i = xPos-placeSize/2; i<=xPos+placeSize/2; i++)
									for (j = yPos-placeSize/2; j<=yPos+placeSize/2; j++)
										if (map->cellExists(i, j))
											queue.insertFirst(map->getCell(i, j));

									while(!queue.isEmpty())
									{
										cell = (TerrainClass*)queue.removeFirst();
										cell->setType(terrainType);
										cell->setTile(terrainTile);
										if (floodFill)
										for (angle = 0; angle < NUM_ANGLES; angle+=2)
										{
											map->getMapPos(angle, &cell->location, &pos);
											if (map->cellExists(&pos) && (map->getCell(&pos)->getType() == fillMatch) && !map->getCell(&pos)->visited)
											{
												queue.insertFirst(map->getCell(&pos));
												map->getCell(&pos)->visited = true;
											}
										}
									}

									if (floodFill)
									{
										int x,y;

										if (terrainType == Terrain_Mountain)
										{
											for (x = 0; x<map->sizeX; x++)
											for (y = 0; y<map->sizeY; y++)
												if (map->cellExists(x, y) && map->getCell(x, y)->isMountain())
													for (i = x-1; i<=x+1; i++)
													for (j = y-1; j<=y+1; j++)
														if (map->cellExists(i, j) && !map->getCell(i, j)->isRock())
															map->getCell(i, j)->setType(Terrain_Rock);
										}
										else if (terrainType == Terrain_ThickSpice)
										{
											for (x = 0; x<map->sizeX; x++)
											for (y = 0; y<map->sizeY; y++)
												if (map->cellExists(x, y) && map->getCell(x, y)->isThickSpice())
													for (i = x-1; i<=x+1; i++)
													for (j = y-1; j<=y+1; j++)
														if (map->cellExists(i, j) && !map->getCell(i, j)->isSpice())
															map->getCell(i, j)->setType(Terrain_Spice);
										}

										//if a terrain is put down, make sure that mountains all sill have rock around them, and thick spice has normal spice around it
										//otherwise convert them to rock if mountain or normal spice if thick spice.
										for (x = 0; x<map->sizeX; x++)
										for (y = 0; y<map->sizeY; y++)
										{
											if  (!map->getCell(x, y)->isRock())
												for (i = x-1; i<=x+1; i++)
												for (j = y-1; j<=y+1; j++)
													if (map->cellExists(i, j) && map->getCell(i, j)->isMountain())
														map->getCell(i, j)->setType(Terrain_Rock);

											if (!map->getCell(x, y)->isSpice())
												for (i = x-1; i<=x+1; i++)
												for (j = y-1; j<=y+1; j++)
													if (map->cellExists(i, j) && map->getCell(i, j)->isThickSpice())
														map->getCell(i, j)->setType(Terrain_Spice);
										}

										for (i = 0; i<map->sizeX; i++)
										for (j = 0; j<map->sizeY; j++)
										{
											smooth_spot(i, j);
											map->getCell(i, j)->visited = false;
										}
									}
									else
										setTerrain(xPos-placeSize/2, yPos-placeSize/2, xPos+placeSize/2, yPos+placeSize/2);
								}
							}
							else if (placingType == PLACING_UNIT)
							{
								if (!thisPlayer->placeUnit(unitType, xPos, yPos))
								{
									showMessage("You cannot place it here.");
									soundPlayer->playSfxSound(Sound_cannot);	//can't place noise
								}

								lastPlacedSpot.x = xPos;	//save position of click
								lastPlacedSpot.y = yPos;
								placing = true;
							}
						}
					}
					else if (fullscreenMode || (mouse->x < gameBarPos.x)) // it isn't on the gamebar
					{
						if (!selectionMode)	//if we have started the selection rectangle
						{
							selectionRect.x = mouse->x + dborder->minX;		//the starting point of the selection rectangele
							selectionRect.y = mouse->y + dborder->minY;
						}
						selectionMode = true;
					}
						
					if (!selectionMode)
					{
						int barMousePosX = mouse->x - gameBarPos.x, //screen height because the bar's xPos is at screen height
							barMousePosY = mouse->y;
									
						if ((barMousePosX >= radarOffsetX) && (barMousePosY >= radarOffsetY) && (barMousePosX < (radarOffsetX + radarSizeX)) && (barMousePosY < (radarOffsetY + radarSizeY)))  //if on the radar
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
							if (dborder->minY < 0)
							{
								over = dborder->minY;
								dborder->minY = 0;
								dborder->maxY -= over;
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
								dborder->radarMinY = dborder->minY/BLOCKSIZE;
								dborder->radarMaxY = dborder->radarMinY + radarSizeY;
							}
							else if (dborder->maxY/BLOCKSIZE >= dborder->radarMaxY)
							{
								dborder->radarMaxY = dborder->maxY/BLOCKSIZE+1;
								dborder->radarMinY = dborder->radarMaxY - radarSizeY;
							}



							radarMode = true;	//stay within the box
						}
					}
					
					editorContainer->handlePress(mouse->x, mouse->y);
					editorContainer2->handlePress(mouse->x, mouse->y);
					break;
				}	//end of SDL_BUTTON_LEFT
				case (SDL_BUTTON_RIGHT):	//if the right mouse button is pressed
				{
					placingMode = false;
					break;
				}	//end of SDL_BUTTON_RIGHT
				break;
			}
		}
		case SDL_MOUSEMOTION:
		{
			SDL_MouseButtonEvent* mouse = &event.button;
			moveDownMode =  (mouse->y >= screen->h-1);	//if user is trying to move view down
			moveLeftMode = (mouse->x <= 0);	//if user is trying to move view left
			moveRightMode = (mouse->x >= screen->w-1);	//if user is trying to move view right
			moveUpMode = (mouse->y <= 0);	//if user is trying to move view up

			if (radarMode)

			{

				int barMousePosX = mouse->x - gameBarPos.x, //screen height because the bar's xPos is at screen height
					barMousePosY = mouse->y;
							
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
				if (dborder->minY < 0)
				{
					over = dborder->minY;
					dborder->minY = 0;
					dborder->maxY -= over;
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
					dborder->radarMinY = dborder->minY/BLOCKSIZE;
					dborder->radarMaxY = dborder->radarMinY + radarSizeY;
				}
				else if (dborder->maxY/BLOCKSIZE >= dborder->radarMaxY)
				{
					dborder->radarMaxY = dborder->maxY/BLOCKSIZE+1;
					dborder->radarMinY = dborder->radarMaxY - radarSizeY;
				}

			}

			if (placing)
			{
				int xPos = (mouse->x + dborder->minX)/BLOCKSIZE,
					yPos = (mouse->y + dborder->minY)/BLOCKSIZE;

				if ((xPos != lastPlacedSpot.x) || (yPos != lastPlacedSpot.y))
				{
					if (placingType == PLACING_TERRAIN)
					{
						if (map->cellExists(xPos, yPos))
							setTerrain(xPos-placeSize/2, yPos-placeSize/2, xPos+placeSize/2, yPos+placeSize/2);
					}
					else if (placingType == PLACING_UNIT)
					{
						if (!thisPlayer->placeUnit(unitType, xPos, yPos))
						{
							showMessage("You cannot place it here.");
							soundPlayer->playSfxSound(Sound_cannot);	//can't place noise
						}

						lastPlacedSpot.x = xPos;	//save position of click
						lastPlacedSpot.y = yPos;
						placing = true;
					}
				}
			}

			drawnMouseX = mouse->x;

			drawnMouseY = mouse->y;
			if (focusedWidget)
				focusedWidget->handleMotion(mouse->x, mouse->y);
			break;
		}
		case SDL_MOUSEBUTTONUP:
		{
			SDL_MouseButtonEvent* mouse = &event.button;
			if (selectionMode && (mouse->button == SDL_BUTTON_LEFT))
			{
				int finalMouseX = mouse->x,	//this keeps the box on the map, and not over game bar
					finalMouseY = mouse->y;
				if (!fullscreenMode && (finalMouseX >= gameBarPos.x))
					finalMouseX = gameBarPos.x-1;

				int rectFinishX = finalMouseX/BLOCKSIZE + dborder->minX/BLOCKSIZE;
				if (rectFinishX > (map->sizeX-1))
					rectFinishX = map->sizeX-1;
				int rectFinishY = mouse->y/BLOCKSIZE + dborder->minY/BLOCKSIZE;
				int rectStartX = selectionRect.x/BLOCKSIZE,
					rectStartY = selectionRect.y/BLOCKSIZE;

				map->selectObjects(thisPlayer->getPlayerNumber(), rectStartX, rectStartY, rectFinishX, rectFinishY, finalMouseX + dborder->minX, finalMouseY + dborder->minY, shift);
			}
			
			editorContainer->handleRelease(mouse->x, mouse->y);
			editorContainer2->handleRelease(mouse->x, mouse->y);

			placing = false;
			radarMode = false;
			selectionMode = false;
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
		}
		else
			moveDownMode = false;
	}
	else if (moveUpMode)
	{
		if (dborder->minY > 0)
		{
			dborder->minY--;
			dborder->maxY--;
			if ((dborder->minY < dborder->radarMinY*BLOCKSIZE) && (dborder->radarMinY > 0))
			{
				dborder->radarMinY--;
				dborder->radarMaxY--;
			}
		}
		else
			moveUpMode = false;
	}
}

/////////////////////process Objects
void processEditorObjects()
{
	if (!structureList->isEmpty())
	{
		StructureClass *tempStructure;

		structureList->reset();
		while(structureList->currentNotNull())
		{
			tempStructure = (StructureClass*)structureList->getCurrent();

			if (tempStructure->wasDestroyed())
				delete ((StructureClass*)structureList->removeCurrent());
			else
				structureList->nextLink();
		}
	}

	if (!unitList->isEmpty())
	{
		UnitClass *tempUnit;

		unitList->reset();
		while(unitList->currentNotNull())
		{
			tempUnit = (UnitClass*)unitList->getCurrent();

			if (tempUnit->wasDestroyed())
				delete ((UnitClass*)unitList->removeCurrent());
			else
				unitList->nextLink();
		}
	}
}

void drawEditorScreen()
{
/////////////////draw structures
	if (!structureList->isEmpty())
	{
		StructureClass* tempStructure;

		structureList->reset();
		while(structureList->currentNotNull())
		{
			tempStructure = (StructureClass*)structureList->getCurrent();

			//if in view range
			if (tempStructure->isOnScreen())
				tempStructure->blitToScreen();

			structureList->nextLink();
		}
	}

////////////draw terrain and units
	int xCount, yCount,

		xCount2 = 0,

		yCount2 = 0,

		xOffset = (dborder->minX/BLOCKSIZE)*BLOCKSIZE - dborder->minX,

		yOffset = (dborder->minY/BLOCKSIZE)*BLOCKSIZE - dborder->minY;



	SDL_Rect	drawLocation;


	for (yCount = dborder->minY/BLOCKSIZE; yCount <= (dborder->maxY/BLOCKSIZE); yCount++)

	{

		for (xCount = dborder->minX/BLOCKSIZE; xCount <= (dborder->maxX/BLOCKSIZE); xCount++)
		{
			map->cell[xCount][yCount].blitToScreen(xCount2*BLOCKSIZE + xOffset, yCount2*BLOCKSIZE + yOffset);
			xCount2++;
		}
		yCount2++;
		xCount2 = 0;
	}

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

/////////draw bullets

	if (!bulletList->isEmpty())
	{
		BulletClass* tempBullet;

		bulletList->reset();
		while(bulletList->currentNotNull())
		{
			tempBullet = (BulletClass*)bulletList->getCurrent();
			if ((((tempBullet->getRealX() + tempBullet->getImageW()/2)/BLOCKSIZE >= dborder->minX)
				&& ((tempBullet->getRealX() - tempBullet->getImageW()/2)/BLOCKSIZE <= dborder->maxX)
				&& ((tempBullet->getRealY() + tempBullet->getImageH()/2)/BLOCKSIZE >= dborder->minY)
				&& ((tempBullet->getRealY() - tempBullet->getImageH()/2)/BLOCKSIZE <= dborder->maxY)))
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

			SDL_Surface	*image,
						*validPlace = (SDL_Surface*)(dataFile[UI_ValidPlace].dat),
						*invalidPlace = (SDL_Surface*)(dataFile[UI_InvalidPlace].dat);

			if (placingType == PLACING_STRUCTURE)
			{
				for (i = xPos; i < (xPos + buildItem.getStructureSizeX()); i++)
				for (j = yPos; j < (yPos + buildItem.getStructureSizeY()); j++)
				{
					if (!map->cellExists(i,j) || !map->getCell(i,j)->isRock() || map->getCell(i,j)->hasAGroundObject())
						image = invalidPlace;
					else
						image = validPlace;

					drawLocation.x = i*BLOCKSIZE - dborder->minX;
					drawLocation.y = j*BLOCKSIZE - dborder->minY;

					SDL_BlitSurface(image, NULL, screen, &drawLocation);
				}
			}
			else if (placingType == PLACING_TERRAIN)
			{
				for (i = xPos-placeSize/2; i<=xPos+placeSize/2; i++)
				for (j = yPos-placeSize/2; j<=yPos+placeSize/2; j++)
				if (map->cellExists(i, j))
				{
					drawLocation.x = i*BLOCKSIZE - dborder->minX;
					drawLocation.y = j*BLOCKSIZE - dborder->minY;
					SDL_BlitSurface(validPlace, NULL, screen, &drawLocation);
				}
			}
			else if (placingType == PLACING_UNIT)
			{
				if (map->cellExists(xPos, yPos))
				{
					TerrainClass* cell = map->getCell(xPos, yPos);
					image = validPlace;

					if (unitType == Unit_Sandworm)
					{
						if(cell->isRock() || cell->hasAnUndergroundUnit())
							image = invalidPlace;
					}
					else if ((unitType == Unit_Infantry) || (unitType == Unit_Trooper) || (unitType == Unit_Fremen) || (unitType == Unit_Sardaukar))
					{
						if (cell->hasANonInfantryGroundObject() || !cell->infantryNotFull())
							image = invalidPlace;
					}
					else if ((unitType == Unit_Carryall) || (unitType == Unit_Ornithopter))
					{
						if (cell->hasAnAirUnit())
							image = invalidPlace;
					}
					else if (cell->hasAGroundObject() || cell->isMountain())
						image = invalidPlace;

					drawLocation.x = xPos*BLOCKSIZE - dborder->minX;
					drawLocation.y = yPos*BLOCKSIZE - dborder->minY;

					SDL_BlitSurface(image, NULL, screen, &drawLocation);
				}
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

///////////draw game bar
	if (!fullscreenMode)
	{
		SDL_BlitSurface((SDL_Surface *)dataFile[UI_GameBar].dat, NULL, screen, &gameBarPos);
		editorContainer->draw();
		editorContainer2->draw();
	}

/////////draw radar

	if (!fullscreenMode)
	{
	    // Lock the screen for direct access to the pixels
		if (!SDL_MUSTLOCK(screen) || (SDL_LockSurface(screen) == 0))
		{
			xCount2 = 0;
			yCount2 = 0;
			TerrainClass* tempTerrain;
			for (xCount = dborder->radarMinX; xCount < dborder->radarMaxX; xCount++)
			{
				for (yCount = dborder->radarMinY; yCount < dborder->radarMaxY; yCount++)
				{
					tempTerrain = &map->cell[xCount][yCount];
					if (tempTerrain->hasAnObject())
					{
						ObjectClass* tempObject = tempTerrain->getObject();
						int colour;
						switch (tempObject->getOwner()->getHouse())
						{
							case(HOUSE_ATREIDES): colour = COLOUR_ATREIDES; break;
							case(HOUSE_ORDOS): colour = COLOUR_ORDOS; break;
							case(HOUSE_HARKONNEN): colour = COLOUR_HARKONNEN; break;
							case(HOUSE_SARDAUKAR): colour = COLOUR_SARDAUKAR; break;
							case(HOUSE_FREMEN): colour = COLOUR_FREMEN; break;
							case(HOUSE_MERCENARY): colour = COLOUR_MERCENARY; break;
						}

						if (tempObject->getItemID() == Unit_Sandworm)
							colour = COLOUR_WHITE;
						putpixel(screen, gameBarPos.x + radarOffsetX + xCount2, radarOffsetY + yCount2, colour);
					}
					else
					{
						int	type = tempTerrain->getType(),
							colour;

						switch (type)
						{
							case Terrain_Dunes:
								colour = COLOUR_DESERTSAND;
								break;
							case Terrain_Mountain:
								colour = COLOUR_MOUNTAIN;
								break;
							case Terrain_Rock:
								colour = COLOUR_DARKGREY;
								break;
							case Terrain_Sand:
								colour = COLOUR_DESERTSAND;
								break;
							case Terrain_Spice:
								colour = COLOUR_SPICE;
								break;
							case Terrain_ThickSpice:
								colour = COLOUR_THICKSPICE;
								break;
							default://case Structure_Slab1:
								colour = COLOUR_DARKGREY;
						}

						putpixel(screen, gameBarPos.x + radarOffsetX + xCount2, radarOffsetY + yCount2, colour);
					}
					yCount2++;
				}
				xCount2++;
				yCount2 = 0;
			}
			
			if (SDL_MUSTLOCK(screen))
				SDL_UnlockSurface(screen);
		}
		
		drawrect(screen, gameBarPos.x + radarOffsetX + dborder->minX/BLOCKSIZE - dborder->radarMinX, radarOffsetY + dborder->minY/BLOCKSIZE - dborder->radarMinY, gameBarPos.x + radarOffsetX + dborder->maxX/BLOCKSIZE - dborder->radarMinX, radarOffsetY + dborder->maxY/BLOCKSIZE - dborder->radarMinY, COLOUR_WHITE); //draw a rectangle on radar representing the view
	}

////////draw messages
	SDL_Surface* text;
	int pos = 0;
	for(int count = MESSAGE_BUFFER_SIZE-1; count >= 0; count--)
	if (messageTimer[count] > 0)
	{
		text = TTF_RenderText_Solid(font[16], messageBuffer[count], palette->colors[COLOUR_WHITE]);
		drawLocation.x = 0;
		drawLocation.y = TTF_FontHeight(font[16])*pos++;
		SDL_BlitSurface(text, NULL, screen, &drawLocation);
		SDL_FreeSurface(text);
		if (--messageTimer[count] <= 0)
			messageBuffer[count][0] = '\0';
	}

	drawCursor();
}

void do_editor()
{
	int i, j,
		frameStart, frameEnd, frameTime;
	
	edit_mode = true;
	
	quitGame = false;
	focusedWidget = NULL;

	gameState = GS_NONE;

	floodFill = false;
	placing = false;
	placeSize = 1,
	terrainType = NONE,
	terrainTile,
	unitType = NONE;

	printf("starting editor\n");fflush(stdout);
	soundPlayer->setMusic(false);
	//////////////////////////////////////////////////////////////////////////
	dborder = new BORDERTYPE;

	unitList = new ListIterator();	//holds all the units
	structureList = new ListIterator();	//all the structures
	bulletList = new ListIterator();	//all projectiles
	selectedList = new ListIterator();	//all the objects currently selected
	selectedLists = new ListIterator[NUMSELECTEDLISTS];	//nine - for each of the keys 1 - 9 on keyboard

	airUnitBlitList = new ListIterator();	//lists ordering the blitting of units
	groundUnitBlitList = new ListIterator();
	infantryBlitList = new ListIterator();
	selectionBlitList = new ListIterator();	//holds a list ordering the blitting of selection boxes
	undergroundUnitBlitList = new ListIterator();

	player = new PlayerClass*[NUM_HOUSES];
	for (i = 0; i < MAX_PLAYERS; i++)	
		player[i] = NULL;
	for (i = 0; i < NUM_HOUSES; i++)
	{
		if (settings.isHuman[i])
			player[i] = new PlayerClass(i, settings.playerHouse[i], settings.playerColour[i], DEFAULT_STARTINGCREDITS);
		else
		{
			player[i] = new AiPlayerClass(i, settings.playerHouse[i], settings.playerColour[i], ((settings.gameType == ORIGINAL) || (settings.gameType == SKIRMISH))  ? 0 : DEFAULT_STARTINGCREDITS, settings.playerDifficulty[i]);
		}
		player[i]->setTeam(settings.playerTeam[i]);
		player[i]->assignMapPlayerNum(i);
	}
	thisPlayer = player[0];
	mapImageHouseColour((SDL_Surface*)dataFile[UI_GameBar].dat, thisPlayer->getColour());
	mapImageHouseColour((SDL_Surface*)dataFile[UI_GameBar2].dat, thisPlayer->getColour());

	FILE* mapFile = fopen("map.dlm", "r");
	if (mapFile != NULL)
	{
		fclose(mapFile);
		if (!loadMap("map.dlm"))
		{
			make_random_map(128, 128);
			showMessage("error in map file \"map.dlm\", try deleting it.");
			fprintf(stderr, "error in map file \"map.dlm\", try deleting it.");
		}
	}
	else
		make_random_map(128, 128);

	gameBarPos.w = ((SDL_Surface *)dataFile[UI_GameBar].dat)->w;
	gameBarPos.h = ((SDL_Surface *)dataFile[UI_GameBar].dat)->h;
	gameBarPos.x = screen->w - gameBarPos.w;
	gameBarPos.y = 0;
	
	radarSizeX = radarSizeY = 128;
	if (map->sizeX < 128)		//allow for small maps
		radarSizeX = map->sizeX;
	if (map->sizeY < 128)
		radarSizeY = map->sizeY;
	radarOffsetX = 14;
	radarOffsetY = 2;
	if (radarSizeX < 128)
		radarOffsetX += (128 - radarSizeX)/2;
	if (radarSizeY < 128)
		radarOffsetY += (128 - radarSizeY)/2;

	i = map->sizeX*BLOCKSIZE/2-1;
	j = map->sizeY*BLOCKSIZE/2-1;

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
	dborder->minY = j - screen->h/2-1;
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

	//////////////////////////////////////////////////////////////////////////

	Button		floodButton,
				houseButtons[6],
				sizeButtons[5],
				typeButtons[3],
				structureButtons[8][4],
				terrainButtons[4][5],
				unitButtons[4][5];
	Container	structureContainer,
				terrainContainer,
				unitContainer;
	HBox		houseHBox,
				placeSizeHBox,
				structureHBoxs[8],
				terrainHBoxs[4],
				unitHBoxs[4];
	VBox		typeVBox,
				structureVBox,
				terrainVBox,
				unitVBox;

	SDL_Rect	src;
	SDL_Surface	*surface;

	floodButtonP = &floodButton;
	sizeButtonsP = sizeButtons;
	structureContainerP = &structureContainer,
	terrainContainerP = &terrainContainer,
	unitContainerP = &unitContainer;

	structureContainer.setX(gameBarPos.x + 16);
	structureContainer.setY(16 + 128 + 16);
	structureContainer.setWidth((screen->w - 16) - structureContainer.getX());
	structureContainer.setHeight((screen->h - 16) - structureContainer.getY());

	terrainContainer.setX(gameBarPos.x + 16);
	terrainContainer.setY(16 + 128 + 16);
	terrainContainer.setWidth((screen->w - 16) - terrainContainer.getX());
	terrainContainer.setHeight((screen->h - 16) - terrainContainer.getY());

	unitContainer.setX(gameBarPos.x + 16);
	unitContainer.setY(16 + 128 + 16);
	unitContainer.setWidth((screen->w - 16) - unitContainer.getX());
	unitContainer.setHeight((screen->h - 16) - unitContainer.getY());

	typeVBox.setBuffering(0);
	typeVBox.setX(gameBarPos.x + 16);
	typeVBox.setY(16 + 128 + 16);
	typeVBox.setWidth((screen->w - 16) - typeVBox.getX());
	typeVBox.setHeight(64);
	typeVBox.setVisible(true);

	typeButtons[0].setText("Structures");
	typeButtons[0].setCallBack(setPlacingType, (void*)PLACING_STRUCTURE);
	typeButtons[0].setVisible(true);
	typeVBox.addChild(&typeButtons[0]);

	typeButtons[1].setText("Terrain");
	typeButtons[1].setCallBack(setPlacingType, (void*)PLACING_TERRAIN);
	typeButtons[1].setVisible(true);
	typeVBox.addChild(&typeButtons[1]);

	typeButtons[2].setText("Units");
	typeButtons[2].setCallBack(setPlacingType, (void*)PLACING_UNIT);
	typeButtons[2].setVisible(true);
	typeVBox.addChild(&typeButtons[2]);
	editorContainer = &typeVBox;

	houseHBox.setBuffering(0);
	houseHBox.setVisible(true);

	SDL_Surface* colourSurface = SDL_DisplayFormat((SDL_Surface*)dataFile[Unknown].dat);
	src.x = src.y = 0;
	src.w = src.h = BLOCKSIZE;

	SDL_FillRect(colourSurface, NULL, SDL_MapRGB(colourSurface->format, palette->colors[COLOUR_ATREIDES].r, palette->colors[COLOUR_ATREIDES].g, palette->colors[COLOUR_ATREIDES].b));
	houseButtons[HOUSE_ATREIDES].setSurface(colourSurface, &src);
	SDL_FillRect(colourSurface, NULL, SDL_MapRGB(colourSurface->format, palette->colors[COLOUR_ORDOS].r, palette->colors[COLOUR_ORDOS].g, palette->colors[COLOUR_ORDOS].b));
	houseButtons[HOUSE_ORDOS].setSurface(colourSurface, &src);
	SDL_FillRect(colourSurface, NULL, SDL_MapRGB(colourSurface->format, palette->colors[COLOUR_HARKONNEN].r, palette->colors[COLOUR_HARKONNEN].g, palette->colors[COLOUR_HARKONNEN].b));
	houseButtons[HOUSE_HARKONNEN].setSurface(colourSurface, &src);
	SDL_FillRect(colourSurface, NULL, SDL_MapRGB(colourSurface->format, palette->colors[COLOUR_SARDAUKAR].r, palette->colors[COLOUR_SARDAUKAR].g, palette->colors[COLOUR_SARDAUKAR].b));
	houseButtons[HOUSE_SARDAUKAR].setSurface(colourSurface, &src);
	SDL_FillRect(colourSurface, NULL, SDL_MapRGB(colourSurface->format, palette->colors[COLOUR_FREMEN].r, palette->colors[COLOUR_FREMEN].g, palette->colors[COLOUR_FREMEN].b));
	houseButtons[HOUSE_FREMEN].setSurface(colourSurface, &src);
	SDL_FillRect(colourSurface, NULL, SDL_MapRGB(colourSurface->format, palette->colors[COLOUR_MERCENARY].r, palette->colors[COLOUR_MERCENARY].g, palette->colors[COLOUR_MERCENARY].b));
	houseButtons[HOUSE_MERCENARY].setSurface(colourSurface, &src);

	for(i=0; i<NUM_HOUSES; i++)
	{
		houseButtons[i].setCallBack(setHouse, (void*)player[i]);
		houseHBox.addChild(&houseButtons[i]);
		houseButtons[i].setVisible(true);
	}

	//////////// structures
	structureVBox.setBuffering(0);
	structureVBox.setX(gameBarPos.x + 16);
	structureVBox.setY(16 + 128 + 16 + 80);
	structureVBox.setWidth((screen->w - 16) - structureVBox.getX());
	structureVBox.setHeight((screen->h - 16) - structureVBox.getY());
	structureVBox.setVisible(true);
	structureContainer.addChild(&structureVBox);

	structureVBox.addChild(&houseHBox);
	for(i = 0; i < 8; i++)
		structureVBox.addChild(&structureHBoxs[i]);

	for(i = 0; i < 8; i++)
	{
		structureHBoxs[i].setBuffering(0);
		for(j = 0; j < 4; j++)
		{
			structureButtons[i][j].setVisible(true);
			structureHBoxs[i].addChild(&structureButtons[i][j]);
		}
		structureHBoxs[i].setVisible(true);
	}

	//structure buttons
	src.x = src.y = 0;
	src.w = src.h = BLOCKSIZE;
	//Structure_ConstructionYard button
	src.w = src.h = 2*BLOCKSIZE;
	structureButtons[0][0].setSurface(((SDL_Surface*)dataFile[Structure_ConstructionYard].dat), &src);
	structureButtons[0][0].setCallBack(setStructureType, (void*)Structure_ConstructionYard);
	//Structure_Slab1 button
	src.w = src.h = BLOCKSIZE;
	structureButtons[0][1].setSurface(((SDL_Surface*)dataFile[Structure_Slab1].dat), &src);
	structureButtons[0][1].setCallBack(setStructureType, (void*)Structure_Slab1);
	//Structure_Slab4 button
	src.w = src.h = 2*BLOCKSIZE;
	structureButtons[0][2].setSurface(((SDL_Surface*)dataFile[Structure_Slab4].dat), &src);
	structureButtons[0][2].setCallBack(setStructureType, (void*)Structure_Slab4);
	//Structure_Wall button
	src.w = src.h = BLOCKSIZE;
	structureButtons[0][3].setSurface(((SDL_Surface*)dataFile[Structure_Wall].dat), &src);
	structureButtons[0][3].setCallBack(setStructureType, (void*)Structure_Wall);
	//Structure_WindTrap button
	src.w = src.h = 2*BLOCKSIZE;
	structureButtons[1][0].setSurface(((SDL_Surface*)dataFile[Structure_WindTrap].dat), &src);
	structureButtons[1][0].setCallBack(setStructureType, (void*)Structure_WindTrap);

	src.w = src.h = 2*BLOCKSIZE;
	structureButtons[1][1].setSurface(((SDL_Surface*)dataFile[Structure_Silo].dat), &src);
	structureButtons[1][1].setCallBack(setStructureType, (void*)Structure_Silo);

	//Structure_Radar button
	src.w = src.h = 2*BLOCKSIZE;
	structureButtons[1][2].setSurface(((SDL_Surface*)dataFile[Structure_Refinery].dat), &src);
	structureButtons[1][2].setCallBack(setStructureType, (void*)Structure_Refinery);
	//Structure_WOR button
	src.w = src.h = 2*BLOCKSIZE;
	structureButtons[1][3].setSurface(((SDL_Surface*)dataFile[Structure_Radar].dat), &src);
	structureButtons[1][3].setCallBack(setStructureType, (void*)Structure_Radar);

	src.w = src.h = 2*BLOCKSIZE;
	structureButtons[2][0].setSurface(((SDL_Surface*)dataFile[Structure_WOR].dat), &src);
	structureButtons[2][0].setCallBack(setStructureType, (void*)Structure_WOR);

	src.w = src.h = BLOCKSIZE;
	structureButtons[2][1].setSurface(((SDL_Surface*)dataFile[Structure_GunTurret].dat), &src);
	structureButtons[2][1].setCallBack(setStructureType, (void*)Structure_GunTurret);

	src.w = src.h = BLOCKSIZE;
	structureButtons[2][2].setSurface(((SDL_Surface*)dataFile[Structure_RocketTurret].dat), &src);
	structureButtons[2][2].setCallBack(setStructureType, (void*)Structure_RocketTurret);

	src.w = src.h = 2*BLOCKSIZE;
	structureButtons[2][3].setSurface(((SDL_Surface*)dataFile[Structure_LightFactory].dat), &src);
	structureButtons[2][3].setCallBack(setStructureType, (void*)Structure_LightFactory);

	src.w = src.h = 2*BLOCKSIZE;
	structureButtons[3][0].setSurface(((SDL_Surface*)dataFile[Structure_HeavyFactory].dat), &src);
	structureButtons[3][0].setCallBack(setStructureType, (void*)Structure_HeavyFactory);

	src.w = src.h = 2*BLOCKSIZE;
	structureButtons[3][1].setSurface(((SDL_Surface*)dataFile[Structure_RepairYard].dat), &src);
	structureButtons[3][1].setCallBack(setStructureType, (void*)Structure_RepairYard);

	src.w = src.h = 2*BLOCKSIZE;
	structureButtons[3][2].setSurface(((SDL_Surface*)dataFile[Structure_HighTechFactory].dat), &src);
	structureButtons[3][2].setCallBack(setStructureType, (void*)Structure_HighTechFactory);

	src.w = src.h = 2*BLOCKSIZE;
	structureButtons[3][3].setSurface(((SDL_Surface*)dataFile[Structure_StarPort].dat), &src);
	structureButtons[3][3].setCallBack(setStructureType, (void*)Structure_StarPort);

	src.w = src.h = 2*BLOCKSIZE;
	structureButtons[4][0].setSurface(((SDL_Surface*)dataFile[Structure_IX].dat), &src);
	structureButtons[4][0].setCallBack(setStructureType, (void*)Structure_IX);

	src.w = src.h = 2*BLOCKSIZE;
	structureButtons[4][1].setSurface(((SDL_Surface*)dataFile[Structure_Palace].dat), &src);
	structureButtons[4][1].setCallBack(setStructureType, (void*)Structure_Palace);

	structureContainer.setVisible(true);

	/////////////////////// terrain

	terrainVBox.setBuffering(0);
	terrainVBox.setX(gameBarPos.x + 16);
	terrainVBox.setY(16 + 128 + 16 + 80);
	terrainVBox.setWidth((screen->w - 16) - terrainVBox.getX());
	terrainVBox.setHeight(((screen->h - 16) - terrainVBox.getY())/2);
	terrainVBox.setVisible(true);
	terrainContainer.addChild(&terrainVBox);

	placeSizeHBox.setBuffering(0);
	placeSizeHBox.setVisible(true);

	char temp[5];
	for(i=0; i<5; i++)
	{
		sizeButtons[i].setCallBack(setPlaceSize, (void*)(2*i + 1));
		sprintf(temp, "%d", 2*i + 1);
		sizeButtons[i].setText(temp);
		sizeButtons[i].setToggler(true);
		sizeButtons[i].setVisible(true);
		placeSizeHBox.addChild(&sizeButtons[i]);
	}
	floodButton.setCallBack(toogleFlood);
	floodButton.setText("Fill");
	floodButton.setToggler(true);
	floodButton.setVisible(true);
	placeSizeHBox.addChild(&floodButton);
	
	terrainVBox.addChild(&placeSizeHBox);
	for(i = 0; i < 4; i++)
		terrainVBox.addChild(&terrainHBoxs[i]);

	for(i = 0; i < 4; i++)
	{
		terrainHBoxs[i].setBuffering(0);
		for(j = 0; j < 5; j++)
		{
			terrainButtons[i][j].setVisible(true);
			terrainHBoxs[i].addChild(&terrainButtons[i][j]);
		}
		terrainHBoxs[i].setVisible(true);
	}

	//Terrain Buttons
	src.y = 0;
	src.w = src.h = BLOCKSIZE;
	//sand button
	src.x = 0;
	terrainButtons[1][0].setSurface(((SDL_Surface*)dataFile[Terrain_Sand].dat), &src);
	terrainButtons[1][0].setCallBack(setTerrainTile, (void*)BUTTON_SAND);
	//bloom button
	src.x = BLOCKSIZE;
	terrainButtons[1][1].setSurface(((SDL_Surface*)dataFile[Terrain_Sand].dat), &src);
	terrainButtons[1][1].setCallBack(setTerrainTile, (void*)BUTTON_BLOOM);
	//altsand button
	src.x = 0;
	terrainButtons[1][2].setSurface(((SDL_Surface*)dataFile[Terrain_Dunes].dat), &src);
	terrainButtons[1][2].setCallBack(setTerrainTile, (void*)BUTTON_DUNES);
	//spice button
	src.x = 0;
	terrainButtons[1][3].setSurface(((SDL_Surface*)dataFile[Terrain_Spice].dat), &src);
	terrainButtons[1][3].setCallBack(setTerrainTile, (void*)BUTTON_SPICE);
	//dense spice button
	src.x = 0;
	terrainButtons[1][4].setSurface(((SDL_Surface*)dataFile[Terrain_ThickSpice].dat), &src);
	terrainButtons[1][4].setCallBack(setTerrainTile, (void*)BUTTON_THICKSPICE);
	//rock button
	src.x = 0;
	terrainButtons[2][0].setSurface(((SDL_Surface*)dataFile[Terrain_Rock].dat), &src);
	terrainButtons[2][0].setCallBack(setTerrainTile, (void*)BUTTON_ROCK);
	//mountain button
	src.x = 0;
	terrainButtons[2][1].setSurface(((SDL_Surface*)dataFile[Terrain_Mountain].dat), &src);
	terrainButtons[2][1].setCallBack(setTerrainTile, (void*)BUTTON_MOUNTAIN);

	terrainContainer.setVisible(true);

	////////////////// units

	unitVBox.setBuffering(0);
	unitVBox.setX(gameBarPos.x + 16);
	unitVBox.setY(16 + 128 + 16 + 80);
	unitVBox.setWidth((screen->w - 16) - unitVBox.getX());
	unitVBox.setHeight(((screen->h - 16) - unitVBox.getY())/2);
	unitVBox.setVisible(true);
	unitContainer.addChild(&unitVBox);

	unitVBox.addChild(&houseHBox);
	for(i = 0; i < 4; i++)
		unitVBox.addChild(&unitHBoxs[i]);

	for(i = 0; i < 4; i++)
	{
		unitHBoxs[i].setBuffering(0);
		for(j = 0; j < 5; j++)
		{
			unitButtons[i][j].setVisible(true);
			unitHBoxs[i].addChild(&unitButtons[i][j]);
		}
		unitHBoxs[i].setVisible(true);
	}

	//unit Buttons
	src.x = 0;
	src.y = 0;
	src.w = src.h = BLOCKSIZE;
	//infantry button
	surface = (SDL_Surface*)dataFile[Unit_Infantry].dat;
	src.w = surface->w/4;
	src.h = surface->h/4;
	unitButtons[0][0].setSurface(surface, &src);
	unitButtons[0][0].setCallBack(setUnitType, (void*)Unit_Infantry);
	//trooper button
	surface = (SDL_Surface*)dataFile[Unit_Trooper].dat;
	src.w = surface->w/4;
	src.h = surface->h/4;
	unitButtons[0][1].setSurface(surface, &src);
	unitButtons[0][1].setCallBack(setUnitType, (void*)Unit_Trooper);
	//trike button
	surface = (SDL_Surface*)dataFile[Unit_Trike].dat;
	src.w = surface->w/8;
	src.h = surface->h;
	unitButtons[0][2].setSurface(surface, &src);
	unitButtons[0][2].setCallBack(setUnitType, (void*)Unit_Trike);
	//raider button
	surface = (SDL_Surface*)dataFile[Unit_Raider].dat;
	src.w = surface->w/8;
	src.h = surface->h;
	unitButtons[0][2].setSurface(surface, &src);
	unitButtons[0][2].setCallBack(setUnitType, (void*)Unit_Raider);
	//quad button
	surface = (SDL_Surface*)dataFile[Unit_Quad].dat;
	src.w = surface->w/8;
	src.h = surface->h;
	unitButtons[0][3].setSurface(surface, &src);
	unitButtons[0][3].setCallBack(setUnitType, (void*)Unit_Quad);
	//tank button
	surface = (SDL_Surface*)dataFile[Unit_TankBase].dat;
	surface = SDL_DisplayFormat(surface);
	src.w = surface->w/8;
	src.h = surface->h;
	SDL_BlitSurface((SDL_Surface*)dataFile[Unit_Tank].dat, NULL, surface, NULL);
	unitButtons[0][4].setSurface(surface, &src);
	SDL_FreeSurface(surface);
	unitButtons[0][4].setCallBack(setUnitType, (void*)Unit_Tank);
	//launcher button
	surface = (SDL_Surface*)dataFile[Unit_TankBase].dat;
	surface = SDL_DisplayFormat(surface);
	src.w = surface->w/8;
	src.h = surface->h;
	SDL_BlitSurface((SDL_Surface*)dataFile[Unit_Launcher].dat, NULL, surface, NULL);
	unitButtons[1][0].setSurface(surface, &src);
	unitButtons[1][0].setCallBack(setUnitType, (void*)Unit_Launcher);
	SDL_FreeSurface(surface);
	//siegetank button
	surface = (SDL_Surface*)dataFile[Unit_SiegeTankBase].dat;
	surface = SDL_DisplayFormat(surface);
	src.w = surface->w/8;
	src.h = surface->h;
	SDL_BlitSurface((SDL_Surface*)dataFile[Unit_SiegeTank].dat, NULL, surface, NULL);
	unitButtons[1][1].setSurface(surface, &src);
	unitButtons[1][1].setCallBack(setUnitType, (void*)Unit_SiegeTank);
	SDL_FreeSurface(surface);
	//Harvester button
	surface = (SDL_Surface*)dataFile[Unit_Harvester].dat;
	src.w = surface->w/8;
	src.h = surface->h;
	unitButtons[1][2].setSurface(surface, &src);
	unitButtons[1][2].setCallBack(setUnitType, (void*)Unit_Harvester);
	//MCV button
	surface = (SDL_Surface*)dataFile[Unit_MCV].dat;
	src.w = surface->w/8;
	src.h = surface->h;
	unitButtons[1][3].setSurface(surface, &src);
	unitButtons[1][3].setCallBack(setUnitType, (void*)Unit_MCV);
	//Carryall button
	surface = (SDL_Surface*)dataFile[Unit_Carryall].dat;
	src.w = surface->w/8;
	src.h = surface->h/2;
	unitButtons[1][4].setSurface(surface, &src);
	unitButtons[1][4].setCallBack(setUnitType, (void*)Unit_Carryall);
	//Ornithopter button
	surface = (SDL_Surface*)dataFile[Unit_Ornithopter].dat;
	src.w = surface->w/8;
	src.h = surface->h/4;
	unitButtons[2][0].setSurface(surface, &src);
	unitButtons[2][0].setCallBack(setUnitType, (void*)Unit_Ornithopter);
	//sonictank button
	surface = (SDL_Surface*)dataFile[Unit_TankBase].dat;
	surface = SDL_DisplayFormat(surface);
	src.w = surface->w/8;
	src.h = surface->h;
	SDL_BlitSurface((SDL_Surface*)dataFile[Unit_SonicTank].dat, NULL, surface, NULL);
	unitButtons[2][1].setSurface(surface, &src);
	unitButtons[2][1].setCallBack(setUnitType, (void*)Unit_SonicTank);
	SDL_FreeSurface(surface);
	//devastator button
	surface = (SDL_Surface*)dataFile[Unit_DevastatorBase].dat;
	surface = SDL_DisplayFormat(surface);
	src.w = surface->w/8;
	src.h = surface->h;
	SDL_BlitSurface((SDL_Surface*)dataFile[Unit_Devastator].dat, NULL, surface, NULL);
	unitButtons[2][2].setSurface(surface, &src);
	unitButtons[2][2].setCallBack(setUnitType, (void*)Unit_Devastator);
	SDL_FreeSurface(surface);
	//deviator button
	surface = (SDL_Surface*)dataFile[Unit_TankBase].dat;
	surface = SDL_DisplayFormat(surface);
	src.w = surface->w/8;
	src.h = surface->h;
	SDL_BlitSurface((SDL_Surface*)dataFile[Unit_Deviator].dat, NULL, surface, NULL);
	unitButtons[2][3].setSurface(surface, &src);
	unitButtons[2][3].setCallBack(setUnitType, (void*)Unit_Deviator);
	SDL_FreeSurface(surface);
	//worm button
	surface = (SDL_Surface*)dataFile[Unit_Sandworm].dat;
	src.w = surface->w;
	src.h = surface->h/5;
	unitButtons[2][4].setSurface(surface, &src);
	unitButtons[2][4].setCallBack(setUnitType, (void*)Unit_Sandworm);
	//saboteur button
	surface = (SDL_Surface*)dataFile[Unit_Saboteur].dat;
	src.w = surface->w/4;
	src.h = surface->h/4;
	unitButtons[3][0].setSurface(surface, &src);
	unitButtons[3][0].setCallBack(setUnitType, (void*)Unit_Saboteur);

	unitContainer.setVisible(true);

	editorContainer2 = &terrainContainer;
	//////////////////////////////////////////////////////////////////////////

	settings.finished = false;
	settings.update = false;
	settings.won = false;

	frameStart = SDL_GetTicks();
	//main editor loop
	do
	{
		do

		{

			drawEditorScreen();
			SDL_Flip(screen);
			frameEnd = SDL_GetTicks();
		} while (frameEnd == frameStart);



		frameTime = frameEnd - frameStart; // find difference to get frametime

		frameStart = SDL_GetTicks();



/*		if (showFPS)

		{

			fps = 1.0/(((double)frameTime)/1000.0);

			frameTimer = frameTime;

		}*/



		//frameTime /= gameSpeed;
		if (frameTime <= 0)	//so super fast computers will stil work
			frameTime = 1;
		while (frameTime--)
		{
			doEditorInput();
			processEditorObjects();
		}		
	} while (!quitGame);

	for (i = 0; i < NUM_HOUSES; i++)
		player[i]->setQuitingGame();

	//delete editorStructureVBox;

	while(!structureList->isEmpty())
		delete ((StructureClass*)structureList->removeFirstElement());

	while(!unitList->isEmpty())
		delete ((UnitClass*)unitList->removeFirstElement());

	delete structureList;
	delete unitList;

	delete selectedList;
	delete[] selectedLists;

	while(!bulletList->isEmpty())
		delete ((BulletClass*)bulletList->removeFirstElement());
	delete bulletList;

	delete map;
	delete dborder;

	for (i = 0; i < NUM_HOUSES; i++)
		delete player[i];	

	/*unset editor_mode*/
	edit_mode = false;
}

