#include "sand.h"
#include "Items.h"

MapClass::MapClass(int xSize, int ySize)
{
	sizeX = xSize;
	sizeY = ySize;



	lastSinglySelectedObject = NULL;
	
	int i, j, k;
	cell = new TerrainClass*[xSize];
	for (i = 0; i < xSize; i++)
		cell[i] = new TerrainClass[ySize];

	depthCheckCount = new short[std::max(xSize, ySize)];
	memset(depthCheckCount, 0, sizeof(short)*std::max(xSize, ySize));
	depthCheckMax = new short**[xSize];
	for (i=0; i<xSize; i++)
	{
		depthCheckMax[i] = new short*[ySize];
		for (j=0; j<ySize; j++)
		{
			cell[i][j].location.x = i;
			cell[i][j].location.y = j;
			depthCheckMax[i][j] = new short[std::max(xSize, ySize)+1];
		}
	}

	//arrays used by astar algorithm
	//now fill in the maximum number of cells in a square shape
	//u could look at without success around a destination i,j
	//with a specific k distance before knowing that it is
	//imposible to get to the destination.  Each time the astar
	//algorithm pushes a node with a max diff of k, 
	//depthcheckcount(k) is incremented, if it reaches the
	//value in depthcheckmax(i,j,k), we know have done a full
	//square around target, and thus it is impossible to reach
	//the target, so we should try and get closer if possible,
	//but otherwise stop
	int x, y, maxSquareDepth,
		end;
	for (i=0; i<xSize; i++)	//x position
	for (j=0; j<ySize; j++)	//y position
	{
		//must account for edges of map
		depthCheckMax[i][j][0] = 0;
		maxSquareDepth = std::max(std::max((xSize-1) - i, i), std::max((ySize-1) - j, j));
		for (k=1; k<=maxSquareDepth; k++)	//manually count number of cells
		{									//bad
			depthCheckMax[i][j][k] = 0;

			y = j - k;
			if ((y >= 0) && (y < ySize))
			{
				x = i - k;
				if (x < 0)
					x = 0;

				if ((i + k) < xSize)
					end = i + k;
				else
					end = xSize - 1;

				depthCheckMax[i][j][k] += end - x + 1;
			}
			x = i + k;
			if ((x >= 0) && (x < xSize))
			{
				if (++y < 0)
					y = 0;

				if ((j + k) < ySize)
					end = j + k;
				else
					end = ySize - 1;

				depthCheckMax[i][j][k] += end - y + 1;
			}
			y = j + k;
			if ((y >= 0) && (y < ySize))
			{
				if (--x >= xSize)
					x = xSize - 1;

				if ((i - k) >= 0)
					end = i - k;
				else
					end = 0;

				depthCheckMax[i][j][k] += x - end + 1;
			}
			x = i - k;
			if ((x >= 0) && (x < xSize))
			{
				if (--y >= ySize)
					y = ySize - 1;
			
				if ((j - k + 1) >= 0)
					end = j - k + 1;
				else
					end = 0;

				depthCheckMax[i][j][k] += y - end + 1;
			}
			//fprintf(stdout, "depthCheckMax[%d][%d][%d] is %d.\n", i, j, k, depthCheckMax[i][j][k]);
		}
	}
}

MapClass::~MapClass()
{	
	int i, j;

	if(depthCheckMax)
	{
		for (i = 0; i < sizeX; i++)
		{
			if(depthCheckMax[i])
			{
				for (j = 0; j < sizeY; j++)
				{
					if(depthCheckMax[i][j])
						delete[] depthCheckMax[i][j];
				}
			}
		}
	}

	for (i = 0; i < sizeX; i++)
	{
		delete[] cell[i];
		delete[] depthCheckMax[i];
	}

	delete[] cell;
	delete[] depthCheckCount;
	delete[] depthCheckMax;
}



void MapClass::createSandRegions()

{

	int	angle,

		i, j,

		region = 0;

	COORDTYPE pos;

	ListIterator queue;

	TerrainClass* terr;

	

	for (i = 0; i < sizeX; i++)

	for (j = 0; j < sizeY; j++)

	{

		cell[i][j].setSandRegion(NONE);

		cell[i][j].visited = false;

	}



	for (i = 0; i < sizeX; i++)

	for (j = 0; j < sizeY; j++)

	{

		if (!cell[i][j].isRock() && !cell[i][j].visited)

		{

			queue.insertFirst(&cell[i][j]);

			while(!queue.isEmpty())

			{

				terr = (TerrainClass*)queue.removeFirstElement();

				terr->setSandRegion(region);

				for (angle = 0; angle < NUM_ANGLES; angle++)

				{

					getMapPos(angle, &terr->location, &pos);

					if (cellExists(&pos) && !getCell(&pos)->isRock() && !getCell(&pos)->visited)

					{

						queue.insertFirst(getCell(&pos));

						getCell(&pos)->visited = true;

					}

				}

			}

			region++;

		}

	}



	for (i = 0; i < sizeX; i++)

	for (j = 0; j < sizeY; j++)

		cell[i][j].visited = false;

}



void MapClass::damage(ObjectClass* damager, PlayerClass* damagerOwner, COORDTYPE* realPos, int itemID, int damage, int damagePiercing, int damageRadius, bool air)

{

	int i, j;

	COORDTYPE centreCell = { realPos->x/BLOCKSIZE, realPos->y/BLOCKSIZE };



	for (i = centreCell.x-2; i <= centreCell.x+2; i++)

	for (j = centreCell.y-2; j <= centreCell.y+2; j++)

		if (cellExists(i, j))

			getCell(i, j)->damageCell(damager, damagerOwner, realPos, itemID, damage, damagePiercing, damageRadius, air);

}



void MapClass::setWinFlags(int newWinFlags)

{

	if (newWinFlags == 6 || newWinFlags == 7)

		settings.winFlags = winFlags = newWinFlags;

	else if (newWinFlags == 23)

		settings.winFlags = winFlags = 7;

	else

		settings.winFlags = winFlags = 3;

}

bool MapClass::okayToPlaceStructure(int x, int y, int buildingSizeX, int buildingSizeY, bool tilesRequired, PlayerClass* aPlayer)
{
	bool withinBuildRange = false;

	for (int i = x; i < x + buildingSizeX; i++)
	{
		for (int j = y; j < y + buildingSizeY; j++)
		{
			if (!map->cellExists(i,j) || !map->getCell(i,j)->isRock() || (tilesRequired && !map->getCell(i,j)->isConcrete()) || map->getCell(i,j)->isMountain() || map->getCell(i,j)->hasAGroundObject())
				return false;

			if ((aPlayer == NULL) || isWithinBuildRange(i, j, aPlayer))
				withinBuildRange = true;
		}
	}

	return withinBuildRange;
}

bool MapClass::isWithinBuildRange(int x, int y, PlayerClass* aPlayer)
{
	bool withinBuildRange = false;

	for (int i = x - BUILDRANGE; i <= x + BUILDRANGE; i++)
	for (int j = y - BUILDRANGE; j <= y + BUILDRANGE; j++)
		if (cellExists(i, j) && (cell[i][j].getOwner() == aPlayer->getPlayerNumber()))
			withinBuildRange = true;

	return withinBuildRange;
}
bool MapClass::okayToBuildExclusive(int x, int y, int buildingSizeX, int buildingSizeY)
{
	for (int i = 0; i < buildingSizeX; i++)
	{
		for (int j = 0; j < buildingSizeY; j++)
		{
			if (((cellBlocked(x+i, y+j) == NOTBLOCKED) || ((i == 0) && (j == 0))))
			{
				if (!cell[x+i][y+j].isRock())
				{
					//printf("%d, %d is not rock\n", x+i, y+j);
					return false;
				}
			}
			else
			{
				//printf("%d, %d is blocked\n", x+i, y+j);
				return false;
			}
		}
	}

	return true;
}
int MapClass::getPosAngle(COORDTYPE* source, COORDTYPE* pos)
{
	int angle;

	if (pos->x > source->x)
	{
		if (pos->y > source->y)
			angle = RIGHTDOWN;
		else if (pos->y < source->y)
			angle = RIGHTUP;
		else
			angle = RIGHT;
	}
	else if (pos->x < source->x)
	{
		if (pos->y > source->y)
			angle = LEFTDOWN;
		else if (pos->y < source->y)
			angle = LEFTUP;
		else
			angle = LEFT;
	}
	else
	{
		if (pos->y > source->y)
			angle = DOWN;
		else if (pos->y < source->y)
			angle = UP;
		else
			angle = NONE;
	}

	return angle;
}

BLOCKEDTYPE MapClass::cellBlocked(int xPos, int yPos)
{
        BLOCKEDTYPE blocked = COMBLOCKED;
        if (cellExists(xPos, yPos))
		{
			if (cell[xPos][yPos].getType() == Terrain_Mountain)
				blocked = MOUNTAIN;
			else if (cell[xPos][yPos].hasAnObject())
			{
				if (cell[xPos][yPos].getObject()->isInfantry())
					blocked = INFANTRY;
				else
					blocked = COMBLOCKED;
			}
			else
				blocked = NOTBLOCKED;
		}

        return blocked;
}

void MapClass::getMapPos(int angle, COORDTYPE* source, COORDTYPE* pos)
{
	switch (angle)
	{
		case (RIGHT): pos->x = source->x + 1; pos->y = source->y; break;
		case (RIGHTUP): pos->x = source->x + 1; pos->y = source->y - 1; break;
		case (UP): pos->x = source->x; pos->y = source->y - 1; break;
		case (LEFTUP): pos->x = source->x - 1; pos->y = source->y - 1; break;
		case (LEFT): pos->x = source->x - 1; pos->y = source->y; break;
		case (LEFTDOWN): pos->x = source->x - 1; pos->y = source->y + 1; break;
		case (DOWN): pos->x = source->x; pos->y = source->y + 1; break;
		case (RIGHTDOWN): pos->x = source->x + 1; pos->y = source->y + 1; break;
		default: pos->x = source->x; pos->y = source->y; break;
	}
}

COORDTYPE MapClass::findDeploySpot(COORDTYPE* origin, COORDTYPE* gatherPoint, COORDTYPE* buildingSize, ObjectClass* object)//building size is num squares
{
	double		closestDistance = 1000000000.0;
	COORDTYPE	closestPoint,

				size;

	UnitClass*	unit = (UnitClass*)object;

	bool	found = false,
			foundClosest = false;
	
	int	counter = 0,
		depth = 0,
		edge,
		x = origin->x,
		y = origin->y,
		ranX,
		ranY;



	if ((gatherPoint == NULL) || (gatherPoint->x == NONE) || (gatherPoint->y == NONE))

		gatherPoint = NULL;

	if (buildingSize == NULL)

		size.x = size.y = 0;

	else

	{

		size.x = buildingSize->x;

		size.y = buildingSize->y;

	}



	//y += size.y;

	ranX = x; ranY = y;

	do
	{
		edge = getRandomInt(0, 3);
		switch(edge)
		{
		case 0:	//right edge
			ranX = x + size.x + depth;
			ranY = getRandomInt(y-depth, y + size.y+depth);
			break;
		case 1:	//top edge
            ranX = getRandomInt(x-depth, x + size.x+depth);
			ranY = y - depth - 1;
			break;
		case 2:	//left edge
            ranX = x - depth - 1;
			ranY = getRandomInt(y-depth, y + size.y+depth);
			break;
		case 3: //bottom edge
            ranX = getRandomInt(x-depth, x + size.x+depth);
			ranY = y + size.y + depth;
			break;
		default:
			break;
		}

		if (unit->canPass(ranX, ranY))
		{
			if (gatherPoint == NULL)
			{
				closestPoint.x = ranX;
				closestPoint.y = ranY;
				found = true;
			}
			else
			{
				COORDTYPE temp = {ranX, ranY};
				if (blockDistance(&temp, gatherPoint) < closestDistance)
				{
					closestDistance = blockDistance(&temp, gatherPoint);
					closestPoint.x = ranX;
					closestPoint.y = ranY;
					foundClosest = true;
				}
			}
		}

		if (counter++ >= 100)		//if hasn't found a spot on tempObject layer in 100 tries, goto next
		{
			counter = 0;
			if (++depth > (std::max(map->sizeX, map->sizeY)))
			{
				closestPoint.x = NONE;
				closestPoint.y = NONE;
				found = true;
				fprintf(stderr, "Map full\n"); fflush(stderr);
			}
		}
	} while (!found && (!foundClosest || (counter > 0)));

	return closestPoint;
}



COORDTYPE MapClass::findClosestEdgePoint(COORDTYPE* origin, COORDTYPE* buildingSize)

{

	int closestDistance = NONE;

	COORDTYPE closestPoint;



	if (origin->x < (sizeX - (origin->x + buildingSize->x)))

	{

		closestPoint.x = 0;

		closestDistance = origin->x;

	}

	else

	{

		closestPoint.x = sizeX - 1;

		closestDistance = sizeX - (origin->x + buildingSize->x);

	}

	closestPoint.y = origin->y;



	if (origin->y < closestDistance)

	{

		closestPoint.x = origin->x;

		closestPoint.y = 0;

		closestDistance = origin->y;

	}



	if ((sizeY - (origin->y + buildingSize->y)) < closestDistance)

	{

		closestPoint.x = origin->x;

		closestPoint.y = sizeY - 1;

		closestDistance = origin->y;

	}



	return closestPoint;

}


void MapClass::findClosestFreeSpot(COORDTYPE* origin, COORDTYPE* location, COORDTYPE* closestPoint)
{
	bool	found = false;
	
	int	counter = 0,
		depth = 1,
		x = origin->x,
		y = origin->y;

	COORDTYPE check;
	
	do
	{
		do
		{
			check.x = getRandomInt(x-depth, x + depth);
			check.y = getRandomInt(y-depth, y + depth);
		} while (((check.x >= (x+1 - depth)) && (check.x < (x + depth))) && ((check.y >= (y+1 - depth)) && (check.y < (y + depth))));

		if ((cellBlocked(check.x, check.y) == NOTBLOCKED) || (check.x == location->x && check.y == location->y))
		{
			found = true;
			if ((blockDistance(origin, &check) < blockDistance(origin, location)) && (blockDistance(origin, &check) < blockDistance(origin, closestPoint)))
			{
				closestPoint->x = check.x;
				closestPoint->y = check.y;
			}
		}

		counter++;
		if (counter >= 10)		//if hasn't found a spot on tempObject layer in 100 tries, goto next
		{
			counter = 0;
			depth++;
		}
	} while (!found);
}
void MapClass::removeObjectFromMap(ObjectClass* tempObject)
{
	if (tempObject->isAStructure())
	{
		int tileStart = Terrain_tc1x1;
		StructureClass* tempStructure = static_cast<StructureClass*>(tempObject);

		if (tempStructure->getStructureSizeX() >= 2)
		{
			if (tempStructure->getStructureSizeX() == 2)
				tileStart = Terrain_tc2x2TopLeft;
			else if (tempStructure->getStructureSizeY() == 3)
				tileStart = Terrain_tc3x3TopLeft;
			else
				tileStart = Terrain_tc3x3MiddleLeft;
		}

		for (int j = tempStructure->getY(), tileCount = tileStart; j < tempStructure->getY() + tempStructure->getStructureSizeY(); j++)
		for (int i = tempStructure->getX(); i < tempStructure->getX() + tempStructure->getStructureSizeX(); i++)
			if (cellExists(i, j))
			{
				cell[i][j].unassignNonInfantryGroundObject(tempStructure);
				cell[i][j].setType(Terrain_Rock);
				cell[i][j].setOwner(NONE);
				cell[i][j].setTile(tileCount++);
			}
	}
	else
	{
		COORDTYPE location = *tempObject->getLocation();
		if (cellExists(location.x, location.y))
			cell[location.x][location.y].unassignObject(tempObject);
		if (cellExists(location.x+1, location.y))
			cell[location.x+1][location.y].unassignObject(tempObject);
		if (cellExists(location.x+1, location.y-1))
			cell[location.x+1][location.y-1].unassignObject(tempObject);
		if (cellExists(location.x, location.y-1))
			cell[location.x][location.y-1].unassignObject(tempObject);
		if (cellExists(location.x-1, location.y-1))
			cell[location.x-1][location.y-1].unassignObject(tempObject);
		if (cellExists(location.x-1, location.y))
			cell[location.x-1][location.y].unassignObject(tempObject);
		if (cellExists(location.x-1, location.y+1))
			cell[location.x-1][location.y+1].unassignObject(tempObject);
		if (cellExists(location.x, location.y+1))
			cell[location.x][location.y+1].unassignObject(tempObject);
		if (cellExists(location.x+1, location.y+1))
			cell[location.x+1][location.y+1].unassignObject(tempObject);
	}
}



void MapClass::selectObjects(int playerNum, int x1, int y1, int x2, int y2, int realX, int realY, bool objectARGMode)

{

	ObjectClass	*lastCheckedObject = NULL,

				*lastSelectedObject = NULL;



	//if selection rectangle is checking only one cell and has shift selected we want to add/ remove that unit from the selected group of units

	if (!objectARGMode)

	{

		unselectAll(selectedList);

		selectedList->clearList();

	}



	if ((x1 == x2) && (y1 == y2) && cellExists(x1, y1)) 

	{

		lastCheckedObject = cell[x1][y1].getObjectAt(realX, realY);

		if ((lastCheckedObject != NULL) && (lastCheckedObject->getOwner()->getPlayerNumber() == playerNum))

		{

			if (lastCheckedObject == lastSinglySelectedObject)

			{

				for (int i = dborder->minX/BLOCKSIZE; i <= dborder->maxX/BLOCKSIZE; i++)

				for (int j = dborder->minY/BLOCKSIZE; j <= dborder->maxY/BLOCKSIZE; j++)

					if (cellExists(i, j) && cell[i][j].hasAnObject())

						cell[i][j].selectAllPlayersUnitsOfType(playerNum, lastSinglySelectedObject->getItemID(), &lastCheckedObject, &lastSelectedObject);

				lastSinglySelectedObject = NULL;

			}

			else if (!lastCheckedObject->isSelected())

			{

				lastCheckedObject->setSelected(true);

				selectedList->insertFirst(lastCheckedObject);

				lastSelectedObject = lastCheckedObject;

				lastSinglySelectedObject = lastSelectedObject;

			}

			else if (objectARGMode)	//holding down shift, unselect this unit

			{

				lastCheckedObject->setSelected(false);

				selectedList->removeElement(lastCheckedObject);

			}

		}

		else

			lastSinglySelectedObject = NULL;

	}

	else

	{

		lastSinglySelectedObject = NULL;



		for (int i = std::min(x1, x2); i <= std::max(x1, x2); i++)

		for (int j = std::min(y1, y2); j <= std::max(y1, y2); j++)

			if (cellExists(i, j) && cell[i][j].hasAnObject() 
				&& cell[i][j].isExplored(playerNum) 
				&& !cell[i][j].isFogged(playerNum) )

					cell[i][j].selectAllPlayersUnits(playerNum, &lastCheckedObject, &lastSelectedObject);
	}



	//select an enemy unit if none of your units found

	if (selectedList->isEmpty() && (lastCheckedObject != NULL) && !lastCheckedObject->isSelected())

	{

		lastCheckedObject->setSelected(true);

		lastSelectedObject = lastCheckedObject;

		selectedList->insertFirst(lastCheckedObject);

	}

	else if (lastSelectedObject != NULL)

		lastSelectedObject->playSelectSound();	//we only want one unit responding

/*

	if ((selectedList->getNumElements() == 1) && lastSelectedObject && lastSelectedObject->isAStructure() && ((StructureClass*)lastSelectedObject)->isABuilder())

		((BuilderClass*)lastSelectedObject)->checkSelectionList();*/

}


bool MapClass::findSpice(COORDTYPE* destination, COORDTYPE* origin)
{
	bool found = false;
	
	int	counter = 0,
		depth = 1,
		x = origin->x,
		y = origin->y,
		ranX,
		ranY;

	do
	{
		do
		{
			ranX = getRandomInt(x-depth, x + depth);
			ranY = getRandomInt(y-depth, y + depth);
		} while (((ranX >= (x+1 - depth)) && (ranX < (x + depth))) && ((ranY >= (y+1 - depth)) && (ranY < (y + depth))));

		if (cellExists(ranX,ranY) && !cell[ranX][ranY].hasAGroundObject() && cell[ranX][ranY].hasSpice())
		{
			found = true;

			destination->x = ranX;
			destination->y = ranY;
		}

		counter++;
		if (counter >= 100)		//if hasn't found a spot on tempObject layer in 100 tries, goto next
		{
			counter = 0;
			depth++;
		}
		if (depth > std::max(sizeX, sizeY))
			return false;	//there is possibly no spice left anywhere on map
	} while (!found);

	if ((depth > 1) && (cell[origin->x][origin->y].hasSpice()))
	{
		destination->x = origin->x;
		destination->y = origin->y;
	}

	return true;
}
void MapClass::removeSpice(int xPos, int yPos)	//fixes spice tiles after spice gone to make things look smooth
{
	int i,j,
		x = xPos,
		y = yPos;
	
	if (cellExists(x, y))	//this is the centre cell
	{
		if (cell[x][y].getType() == Terrain_ThickSpice)
			cell[x][y].setType(Terrain_Spice);
		else
		{
			cell[x][y].setType(Terrain_Sand);
			
			//thickspice tiles cant handle non-(thick)spice tiles next to them, if there is after changes, make it non thick
			for(i = x-1; i <= x+1; i++)
			for(j = y-1; j <= y+1; j++)
				if (cellExists(i, j) && (((i==x) && (j!=y)) || ((i!=x) && (j==y))) && cell[i][j].isThickSpice())	//only check cell, right, up, left and down of this one
					cell[i][j].setType(Terrain_Spice);
		}
	}

	//make it look all smooth and nice, tiles will "fit" together
	for(i = x-2; i <= x+2; i++)
	for(j = y-2; j <= y+2; j++)
		if (cellExists(i, j))
			smooth_spot(i, j);
}

void MapClass::viewMap(int playerTeam, COORDTYPE* location, int maxViewRange)
{
	int			i;
	COORDTYPE	pos,
				check;
	pos.x = location->x,
	pos.y = location->y;

//makes map viewable in an area like as shown below 

//				       *****
//                   *********
//                  *****T*****
//                   *********
//                     *****


	check.x = pos.x - maxViewRange;
	if (check.x < 0)
		check.x = 0;

	while ((check.x < sizeX) && ((check.x - pos.x) <=  maxViewRange))
	{
		check.y = (pos.y - lookDist[abs(check.x - pos.x)]);
		if (check.y < 0) check.y = 0;

		while ((check.y < sizeY) && ((check.y - pos.y) <= lookDist[abs(check.x - pos.x)]))
		{
			if (distance_from(location, &check) <= maxViewRange)
			for (i = 0; i < MAX_PLAYERS; i++)
				if (player[i] && (player[i]->getTeam() == playerTeam))
					cell[check.x][check.y].setExplored(i, true);

			check.y++;
		}

		check.x++;
		check.y = pos.y;
	}

///////////////smooth the hidden shade/hide/fogged area
	if (playerTeam == thisPlayer->getTeam())
	{
		bool	up = false, upEdge = false,
				down = false, downEdge = false,
				left = false, leftEdge = false,
				right = false, rightEdge = false;
		
		bool    upFog = false,
				downFog = false,
				leftFog = false,
				rightFog = false;

		int		hideTile = 0;
		int		fogTile = 0;
		maxViewRange = 10;
		
		check.x = pos.x - maxViewRange;
		if (check.x < 0)
			check.x = 0;
		while ((check.x < sizeX) && ((check.x - pos.x) <=  maxViewRange))
		{
			check.y = (pos.y - lookDist[abs(check.x - pos.x)]);
			if (check.y < 0) check.y = 0;

			while ((check.y < sizeY) && ((check.y - pos.y) <=  lookDist[abs(check.x - pos.x)]))
			{
				if (distance_from(location, &check) <= maxViewRange)
				{
					hideTile = 0;

					upEdge = !cellExists(check.x, check.y-1);
					downEdge = !cellExists(check.x, check.y+1);
					leftEdge = !cellExists(check.x-1, check.y);
					rightEdge = !cellExists(check.x+1, check.y);

					up = !upEdge && (!cell[check.x][check.y-1].isExplored(thisPlayer->getPlayerNumber()));
					down = !downEdge && (!cell[check.x][check.y+1].isExplored(thisPlayer->getPlayerNumber()));
					left = !leftEdge && (!cell[check.x-1][check.y].isExplored(thisPlayer->getPlayerNumber()));
					right = !rightEdge && (!cell[check.x+1][check.y].isExplored(thisPlayer->getPlayerNumber()));
					
					upFog = !upEdge && (cell[check.x][check.y-1].isFogged(thisPlayer->getPlayerNumber()));
					downFog = !downEdge && (cell[check.x][check.y+1].isFogged(thisPlayer->getPlayerNumber()));
					leftFog = !leftEdge && (cell[check.x-1][check.y].isFogged(thisPlayer->getPlayerNumber()));
					rightFog = !rightEdge && (cell[check.x+1][check.y].isFogged(thisPlayer->getPlayerNumber()));

						// Now perform the test
					if (left && right && up && down)
							hideTile = Terrain_HiddenFull;

					else if (!left && right && up && down)
							hideTile = Terrain_HiddenNotLeft; //missing left edge

					else if (left && !right && up && down)
							hideTile = Terrain_HiddenNotRight; //missing right edge

					else if (left && right && !up && down)
							hideTile = Terrain_HiddenNotUp; //missing top edge

					else if (left && right && up && !down)
							hideTile = Terrain_HiddenNotDown; //missing bottom edge

					else if (!left && right && !up && down)
							hideTile = Terrain_HiddenDownRight; //missing top left edge

					else if (left && !right && up && !down)
							hideTile = Terrain_HiddenUpLeft; //missing bottom right edge

					else if (left && !right && !up && down)
							hideTile = Terrain_HiddenDownLeft; //missing top right edge

					else if (!left && right && up && !down)
							hideTile = Terrain_HiddenUpRight; //missing bottom left edge

					else if (left && right && !up && !down)
							hideTile = Terrain_HiddenLeftRight; //missing bottom up and down

					else if (left && !right && !up && !down)
							hideTile = Terrain_HiddenLeft; //missing above, right and below 

					else if (!left && right && !up && !down)
							hideTile = Terrain_HiddenRight; //missing above, left and below

					else if (!left && !right && up && !down)
							hideTile = Terrain_HiddenUp; //only up

					else if (!left && !right && !up && down)
							hideTile = Terrain_HiddenDown; //only down

					else if (!left && !right && up && down)
							hideTile = Terrain_HiddenUpDown; //missing left and right

					else if (!cell[check.x][check.y].isExplored(thisPlayer->getPlayerNumber()) && !left && !right && !up && !down)
							hideTile = Terrain_HiddenIsland; //missing left and right
				///////
					cell[check.x][check.y].setHideTile(hideTile);
					
					if(!cell[check.x][check.y].isFogged(thisPlayer->getPlayerNumber()))
					{
					// do it again with fog
					fogTile = 0;
					if (leftFog && rightFog && upFog && downFog)
							fogTile = Terrain_HiddenFull;

					else if (!leftFog && rightFog && upFog && downFog)
							fogTile = Terrain_HiddenNotLeft; //missing left edge

					else if (leftFog && !rightFog && upFog && downFog)
							fogTile = Terrain_HiddenNotRight; //missing right edge

					else if (leftFog && rightFog && !upFog && downFog)
							fogTile = Terrain_HiddenNotUp; //missing top edge

					else if (leftFog && rightFog && upFog && !downFog)
							fogTile = Terrain_HiddenNotDown; //missing bottom edge

					else if (!leftFog && rightFog && !upFog && downFog)
							fogTile = Terrain_HiddenDownRight; //missing top left edge

					else if (leftFog && !rightFog && upFog && !downFog)
							fogTile = Terrain_HiddenUpLeft; //missing bottom right edge

					else if (leftFog && !rightFog && !upFog && downFog)
							fogTile = Terrain_HiddenDownLeft; //missing top right edge

					else if (!leftFog && rightFog && upFog && !downFog)
							fogTile = Terrain_HiddenUpRight; //missing bottom left edge

					else if (leftFog && rightFog && !upFog && !downFog)
							fogTile = Terrain_HiddenLeftRight; //missing bottom up and down

					else if (leftFog && !rightFog && !upFog && !downFog)
							fogTile = Terrain_HiddenLeft; //missing above, right and below 

					else if (!leftFog && rightFog && !upFog && !downFog)
							fogTile = Terrain_HiddenRight; //missing above, left and below

					else if (!leftFog && !rightFog && upFog && !downFog)
							fogTile = Terrain_HiddenUp; //only up

					else if (!leftFog && !rightFog && !upFog && downFog)
							fogTile = Terrain_HiddenDown; //only down

					else if (!leftFog && !rightFog && upFog && downFog)
							fogTile = Terrain_HiddenUpDown; //missing left and right

					/*else if (cell[check.x][check.y].isFogged(thisPlayer->getPlayerNumber()) && leftFog && rightFog 
					       && upFog && downFog)
							fogTile = Terrain_HiddenIsland; //missing left and right*/
				///////
					cell[check.x][check.y].setFogTile(fogTile);
					}
					else
					cell[check.x][check.y].setFogTile(0);
				}

				check.y++;
			}

			check.x++;
			check.y = pos.y;
		}
	}
}
void MapClass::viewMap(int playerTeam, int x, int y, int maxViewRange)
{
	COORDTYPE	location;

	location.x = x;
	location.y = y;

	viewMap(playerTeam, &location, maxViewRange);
}

ObjectClass* MapClass::findObjectWidthID(int objectID, int lx, int ly)

{

	int			x,y;

	ObjectClass	*object = NULL;

	

	if (cellExists(lx, ly))

		object = getCell(lx, ly)->getObjectWithID(objectID);

	if (object == NULL)	//object wasn't found in expected cell

	{	//search surrounding cells

		for(x=lx-5; x<lx+5 && !object; x++)

		for(y=ly-5; y<ly+5 && !object; y++)

		if (cellExists(x, y))

			object = getCell(x, y)->getObjectWithID(objectID);



		if (object == NULL)	//object wasn't found in surrounding cells

		{	//search lists

			if (!unitList->isEmpty())

			{

				unitList->reset();

				while(unitList->currentNotNull() && !object)

				{

					if (((UnitClass*)unitList->getCurrent())->hasObjectID(objectID))

						object = (UnitClass*)unitList->getCurrent();



					unitList->nextLink();

				}

			}



			if (object == NULL)	//object wasn't found in units

			{

				if (!structureList->isEmpty())

				{

					structureList->reset();

					while(structureList->currentNotNull() && !object)

					{

						if (((StructureClass*)structureList->getCurrent())->hasObjectID(objectID))

							object = (StructureClass*)structureList->getCurrent();



						structureList->nextLink();

					}

				}

			}

		}

	}



	return object;

}

