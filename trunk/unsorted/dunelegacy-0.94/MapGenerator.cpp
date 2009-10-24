#include "sand.h"
#include "Items.h"

void clear_terrain(int tile, int type)
{
	for (int i = 0; i < map->sizeX; i++)
	{
		for (int j = 0; j < map->sizeY; j++)
		{
			map->cell[i][j].setType(type);
			map->cell[i][j].setTile(tile);
		}
	}
}

void fixWall(int xPos, int yPos)
{
	if (map->cell[xPos][yPos].hasAGroundObject() && map->cell[xPos][yPos].getGroundObject()->getItemID() == Structure_Wall)
	{
		int up, down, left, right;

		int i = xPos, j = yPos,
			maketile = Structure_w8;
			
		// Walls
		up = (!map->cellExists(i, j-1) || (map->cell[i][j-1].hasAGroundObject()
			&& (map->cell[i][j-1].getGroundObject()->getItemID() == Structure_Wall)));
				//|| (map->cell[i][j-1].getGroundObject()->getItemID() == Structure_GunTurret)
				//|| (map->cell[i][j-1].getGroundObject()->getItemID() == Structure_RocketTurret))));
		down = (!map->cellExists(i, j+1) || (map->cell[i][j+1].hasAGroundObject()
			&& (map->cell[i][j+1].getGroundObject()->getItemID() == Structure_Wall)));
		left = (!map->cellExists(i-1, j) || (map->cell[i-1][j].hasAGroundObject()
			&& (map->cell[i-1][j].getGroundObject()->getItemID() == Structure_Wall)));
		right = (!map->cellExists(i+1, j) || (map->cell[i+1][j].hasAGroundObject()
			&& (map->cell[i+1][j].getGroundObject()->getItemID() == Structure_Wall)));

			// Now perform the test
		if ((left == 1) && (right == 1) && (up == 1) && (down == 1))
				maketile = Structure_w7; //solid wall

		else if ((left == 0) && (right == 1) && (up == 1) && (down == 1))
				maketile = Structure_w2; //missing left edge

		else if ((left == 1) && (right == 0)&& (up == 1) && (down == 1))
				maketile = Structure_w3; //missing right edge

		else if ((left == 1) && (right == 1) && (up == 0) && (down == 1))
				maketile = Structure_w4; //missing top edge

		else if ((left == 1) && (right == 1) && (up == 1) && (down == 0))
				maketile = Structure_w5; //missing bottom edge

		else if ((left == 0) && (right == 1) && (up == 0) && (down == 1))
				maketile = Structure_w1; //missing top left edge

		else if ((left == 1) && (right == 0) && (up == 1) && (down == 0))
				maketile = Structure_w11; //missing bottom right edge

		else if ((left == 1) && (right == 0) && (up == 0) && (down == 1))
				maketile = Structure_w9; //missing top right edge

		else if ((left == 0) && (right == 1) && (up == 1) && (down == 0))
				maketile = Structure_w10; //missing bottom left edge

		else if ((left == 1) && (right == 0) && (up == 0) && (down == 0))
				maketile = Structure_w8; //missing above, right and below 

		else if ((left == 0) && (right == 1) && (up == 0) && (down == 0))
				maketile = Structure_w8; //missing above, left and below

		else if ((left == 0) && (right == 0) && (up == 1) && (down == 0))
				maketile = Structure_w6; //only up

		else if ((left == 0) && (right == 0) && (up == 0) && (down == 1))
				maketile = Structure_w6; //only down

		else if ((left == 1) && (right == 1) && (up == 0) && (down == 0))
				maketile = Structure_w8; //missing above and below

		else if ((left == 0) && (right == 0) && (up == 1) && (down == 1))
				maketile = Structure_w6; //missing left and right
	///////
		((WallClass*)map->cell[i][j].getGroundObject())->setTile(maketile);
	}	
}

void fixWalls(int xPos, int yPos)
{
	fixWall(xPos, yPos);

	if (map->cellExists(xPos, yPos-1))
		fixWall(xPos, yPos-1);
	if (map->cellExists(xPos, yPos+1))
		fixWall(xPos, yPos+1);
	if (map->cellExists(xPos-1, yPos))
		fixWall(xPos-1, yPos);
	if (map->cellExists(xPos+1, yPos))
		fixWall(xPos+1, yPos);
}

int on_left(int cellX, int cellY, int tile)
{
// Return 0 for NO, != 0 for YES
cellX--;
fix_cell(cellX, cellY);
if (map->cell[cellX][cellY].getType() == tile)
	return 1;

return 0;
}

int on_right(int cellX, int cellY, int tile)
{
// Return 0 for NO, != 0 for YES
cellX++;
fix_cell(cellX, cellY);
if (map->cell[cellX][cellY].getType() == tile)
	return 1;

return 0;
}

int on_up(int cellX, int cellY, int tile)
{
// Return 0 for NO, != 0 for YES
cellY--;
fix_cell(cellX, cellY);
if (map->cell[cellX][cellY].getType() == tile)
	return 1;

return 0;
}

int on_down(int cellX, int cellY, int tile)
{
// Return 0 for NO, != 0 for YES
cellY++;
fix_cell(cellX, cellY);
if (map->cell[cellX][cellY].getType() == tile)
	return 1;

return 0;
}

int side4(int cellX, int cellY, int tile)
{
	// Check at 4 sides for 'tile'
	int flag = 0;

	if (on_left(cellX, cellY, tile))
		flag++; 

	if (on_right(cellX, cellY, tile))
		flag++;

	if (on_up(cellX, cellY, tile))
		flag++;

	if (on_down(cellX, cellY, tile))
		flag++;

	return flag;
}

void smooth_spot(int i, int j)
{
int left = 0;
int right = 0;
int up = 0;
int down = 0;
int maketile;

	if (map->cell[i][j].getType() == Terrain_Dunes)
	{
        maketile = Terrain_DunesFull;                   // solid Dunes

        up = on_up(i, j, Terrain_Dunes);
        down = on_down(i, j, Terrain_Dunes);
        left = on_left(i, j, Terrain_Dunes);
        right = on_right(i, j, Terrain_Dunes);

		// Now perform the test
		if ((left == 1) && (right == 1) && (up == 1) && (down == 1))
				maketile = Terrain_DunesFull; //solid rock

		else if ((left == 0) && (right == 1) && (up == 1) && (down == 1))
				maketile = Terrain_DunesNotLeft; //missing left edge

		else if ((left == 1) && (right == 0)&& (up == 1) && (down == 1))
				maketile = Terrain_DunesNotRight; //missing right edge

		else if ((left == 1) && (right == 1) && (up == 0) && (down == 1))
				maketile = Terrain_DunesNotUp; //missing top edge

		else if ((left == 1) && (right == 1) && (up == 1) && (down == 0))
				maketile = Terrain_DunesNotDown; //missing bottom edge

		else if ((left == 0) && (right == 1) && (up == 0) && (down == 1))
				maketile = Terrain_DunesDownRight; //missing top left edge

		else if ((left == 1) && (right == 0) && (up == 1) && (down == 0))
				maketile = Terrain_DunesUpLeft; //missing bottom right edge

		else if ((left == 1) && (right == 0) && (up == 0) && (down == 1))
				maketile = Terrain_DunesDownLeft; //missing top right edge

		else if ((left == 0) && (right == 1) && (up == 1) && (down == 0))
				maketile = Terrain_DunesUpRight; //missing bottom left edge

		else if ((left == 1) && (right == 1) && (up == 0) && (down == 0))
				maketile = Terrain_DunesLeftRight; //missing above and below

		else if ((left == 1) && (right == 0) && (up == 0) && (down == 0))
				maketile = Terrain_DunesLeft; //missing above, right and below 

		else if ((left == 0) && (right == 1) && (up == 0) && (down == 0))
				maketile = Terrain_DunesRight; //missing above, left and below

		else if ((left == 0) && (right == 0) && (up == 1) && (down == 0))
				maketile = Terrain_DunesUp; //

		else if ((left == 0) && (right == 0) && (up == 0) && (down == 1))
				maketile = Terrain_DunesDown; //

		else if ((left == 0) && (right == 0) && (up == 1) && (down == 1))
				maketile = Terrain_DunesUpDown; //missing left and right

		else if ((left == 0) && (right == 0) && (up == 0) && (down == 0))
				maketile = Terrain_DunesIsland; //missing all edges 
///////
		map->cell[i][j].setTile(maketile);
	}

	else if (map->cell[i][j].getType() == Terrain_Mountain)
	{
        maketile = Terrain_MountainFull;                   // solid mountain

        up = on_up(i, j, Terrain_Mountain);
        down = on_down(i, j, Terrain_Mountain);
        left = on_left(i, j, Terrain_Mountain);
        right = on_right(i, j, Terrain_Mountain);

		// Now perform the test
		if ((left == 1) && (right == 1) && (up == 1) && (down == 1))
				maketile = Terrain_MountainFull; //solid rock

		else if ((left == 0) && (right == 1) && (up == 1) && (down == 1))
				maketile = Terrain_MountainNotLeft; //missing left edge

		else if ((left == 1) && (right == 0)&& (up == 1) && (down == 1))
				maketile = Terrain_MountainNotRight; //missing right edge

		else if ((left == 1) && (right == 1) && (up == 0) && (down == 1))
				maketile = Terrain_MountainNotUp; //missing top edge

		else if ((left == 1) && (right == 1) && (up == 1) && (down == 0))
				maketile = Terrain_MountainNotDown; //missing bottom edge

		else if ((left == 0) && (right == 1) && (up == 0) && (down == 1))
				maketile = Terrain_MountainDownRight; //missing top left edge

		else if ((left == 1) && (right == 0) && (up == 1) && (down == 0))
				maketile = Terrain_MountainUpLeft; //missing bottom right edge

		else if ((left == 1) && (right == 0) && (up == 0) && (down == 1))
				maketile = Terrain_MountainDownLeft; //missing top right edge

		else if ((left == 0) && (right == 1) && (up == 1) && (down == 0))
				maketile = Terrain_MountainUpRight; //missing bottom left edge

		else if ((left == 1) && (right == 1) && (up == 0) && (down == 0))
				maketile = Terrain_MountainLeftRight; //missing above and below

		else if ((left == 1) && (right == 0) && (up == 0) && (down == 0))
				maketile = Terrain_MountainLeft; //missing above, right and below 

		else if ((left == 0) && (right == 1) && (up == 0) && (down == 0))
				maketile = Terrain_MountainRight; //missing above, left and below

		else if ((left == 0) && (right == 0) && (up == 1) && (down == 0))
				maketile = Terrain_MountainUp; //

		else if ((left == 0) && (right == 0) && (up == 0) && (down == 1))
				maketile = Terrain_MountainDown; //

		else if ((left == 0) && (right == 0) && (up == 1) && (down == 1))
				maketile = Terrain_MountainUpDown; //missing left and right

		else if ((left == 0) && (right == 0) && (up == 0) && (down == 0))
				maketile = Terrain_MountainIsland; //missing all edges 
///////
		map->cell[i][j].setTile(maketile);
	}

	else if (map->cell[i][j].getType() == Terrain_Rock)
	{
		maketile = Terrain_t1;                   // Standard we have a solid rock plateau

		up = on_up(i, j, Terrain_Rock) || on_up(i, j, Structure_Slab1) || on_up(i, j, Structure_Wall) || on_up(i, j, Terrain_Mountain); 
		down = on_down(i, j, Terrain_Rock) || on_down(i, j, Structure_Slab1) || on_down(i, j, Structure_Wall) || on_down(i, j, Terrain_Mountain); 
		left = on_left(i, j, Terrain_Rock) || on_left(i, j, Structure_Slab1) || on_left(i, j, Structure_Wall) || on_left(i, j, Terrain_Mountain); 
		right = on_right(i, j, Terrain_Rock) || on_right(i, j, Structure_Slab1) || on_right(i, j, Structure_Wall) || on_right(i, j, Terrain_Mountain); 

	// Now perform the test
		if ((left == 1) && (right == 1) && (up == 1) && (down == 1))
				maketile = Terrain_t1; //solid rock

		else if ((left == 0) && (right == 1) && (up == 1) && (down == 1))
				maketile = Terrain_t2; //missing left edge

		else if ((left == 1) && (right == 0)&& (up == 1) && (down == 1))
				maketile = Terrain_t3; //missing right edge

		else if ((left == 1) && (right == 1) && (up == 0) && (down == 1))
				maketile = Terrain_t4; //missing top edge

		else if ((left == 1) && (right == 1) && (up == 1) && (down == 0))
				maketile = Terrain_t5; //missing bottom edge

		else if ((left == 0) && (right == 1) && (up == 0) && (down == 1))
				maketile = Terrain_t6; //missing top left edge

		else if ((left == 1) && (right == 0) && (up == 1) && (down == 0))
				maketile = Terrain_t7; //missing bottom right edge

		else if ((left == 1) && (right == 0) && (up == 0) && (down == 1))
				maketile = Terrain_t8; //missing top right edge

		else if ((left == 0) && (right == 1) && (up == 1) && (down == 0))
				maketile = Terrain_t9; //missing bottom left edge

		else if ((left == 1) && (right == 1) && (up == 0) && (down == 0))
				maketile = Terrain_t11; //missing above and below

		else if ((left == 1) && (right == 0) && (up == 0) && (down == 0))
				maketile = Terrain_t12; //missing above, right and below 

		else if ((left == 0) && (right == 1) && (up == 0) && (down == 0))
				maketile = Terrain_t13; //missing above, left and below

		else if ((left == 0) && (right == 0) && (up == 1) && (down == 0))
				maketile = Terrain_t14; //sand not up

		else if ((left == 0) && (right == 0) && (up == 0) && (down == 1))
				maketile = Terrain_t15; //sand not down

		else if ((left == 0) && (right == 0) && (up == 1) && (down == 1))
				maketile = Terrain_t16; //missing left and right

		else if ((left == 0) && (right == 0) && (up == 0) && (down == 0))
				maketile = Terrain_t10; //missing all edges 
	///////
				map->cell[i][j].setTile(maketile);
	}

	else if (map->cell[i][j].getType() == Terrain_Spice)
	{
		maketile = Terrain_s1;                  // Standard we have a solid spice

		up = on_up(i, j, Terrain_Spice) || on_up(i, j, Terrain_ThickSpice); 
		down = on_down(i, j, Terrain_Spice) || on_down(i, j, Terrain_ThickSpice);
		left = on_left(i, j, Terrain_Spice) || on_left(i, j, Terrain_ThickSpice);
		right = on_right(i, j, Terrain_Spice) || on_right(i, j, Terrain_ThickSpice);

		// Now perform the test
		if ((left == 1) && (right == 1) && (up == 1) && (down == 1))
				maketile = Terrain_s1; //solid spice

		else if ((left == 0) && (right == 1) && (up == 1) && (down == 1))
				maketile = Terrain_s2; //missing left edge

		else if ((left == 1) && (right == 0)&& (up == 1) && (down == 1))
				maketile = Terrain_s3; //missing right edge

		else if ((left == 1) && (right == 1) && (up == 0) && (down == 1))
				maketile = Terrain_s4; //missing top edge

		else if ((left == 1) && (right == 1) && (up == 1) && (down == 0))
				maketile = Terrain_s5; //missing bottom edge

		else if ((left == 0) && (right == 1) && (up == 0) && (down == 1))
				maketile = Terrain_s6; //missing top left edge

		else if ((left == 1) && (right == 0) && (up == 1) && (down == 0))
				maketile = Terrain_s7; //missing bottom right edge

		else if ((left == 1) && (right == 0) && (up == 0) && (down == 1))
				maketile = Terrain_s8; //missing top right edge

		else if ((left == 0) && (right == 1) && (up == 1) && (down == 0))
				maketile = Terrain_s9; //missing bottom left edge

		else if ((left == 1) && (right == 1) && (up == 0) && (down == 0))
				maketile = Terrain_s11; //missing above and below

		else if ((left == 1) && (right == 0) && (up == 0) && (down == 0))
				maketile = Terrain_s12; //missing above, right and below 

		else if ((left == 0) && (right == 1) && (up == 0) && (down == 0))
				maketile = Terrain_s13; //missing above, left and below

		else if ((left == 0) && (right == 0) && (up == 1) && (down == 0))
				maketile = Terrain_s14; //sand not up

		else if ((left == 0) && (right == 0) && (up == 0) && (down == 1))
				maketile = Terrain_s15; //sand not down

		else if ((left == 0) && (right == 0) && (up == 1) && (down == 1))
				maketile = Terrain_s16; //missing left and right

		else if ((left == 0) && (right == 0) && (up == 0) && (down == 0))
				maketile = Terrain_s10; //no edges 
		///////
				map->cell[i][j].setTile(maketile);
	}

	else if (map->cell[i][j].getType() == Terrain_ThickSpice)
	{
        maketile = Terrain_ThickSpiceFull;                   // solid ThickSpice

        up = on_up(i, j, Terrain_ThickSpice);
        down = on_down(i, j, Terrain_ThickSpice);
        left = on_left(i, j, Terrain_ThickSpice);
        right = on_right(i, j, Terrain_ThickSpice);

		// Now perform the test
		if ((left == 1) && (right == 1) && (up == 1) && (down == 1))
				maketile = Terrain_ThickSpiceFull; //solid rock

		else if ((left == 0) && (right == 1) && (up == 1) && (down == 1))
				maketile = Terrain_ThickSpiceNotLeft; //missing left edge

		else if ((left == 1) && (right == 0)&& (up == 1) && (down == 1))
				maketile = Terrain_ThickSpiceNotRight; //missing right edge

		else if ((left == 1) && (right == 1) && (up == 0) && (down == 1))
				maketile = Terrain_ThickSpiceNotUp; //missing top edge

		else if ((left == 1) && (right == 1) && (up == 1) && (down == 0))
				maketile = Terrain_ThickSpiceNotDown; //missing bottom edge

		else if ((left == 0) && (right == 1) && (up == 0) && (down == 1))
				maketile = Terrain_ThickSpiceDownRight; //missing top left edge

		else if ((left == 1) && (right == 0) && (up == 1) && (down == 0))
				maketile = Terrain_ThickSpiceUpLeft; //missing bottom right edge

		else if ((left == 1) && (right == 0) && (up == 0) && (down == 1))
				maketile = Terrain_ThickSpiceDownLeft; //missing top right edge

		else if ((left == 0) && (right == 1) && (up == 1) && (down == 0))
				maketile = Terrain_ThickSpiceUpRight; //missing bottom left edge

		else if ((left == 1) && (right == 1) && (up == 0) && (down == 0))
				maketile = Terrain_ThickSpiceLeftRight; //missing above and below

		else if ((left == 1) && (right == 0) && (up == 0) && (down == 0))
				maketile = Terrain_ThickSpiceLeft; //missing above, right and below 

		else if ((left == 0) && (right == 1) && (up == 0) && (down == 0))
				maketile = Terrain_ThickSpiceRight; //missing above, left and below

		else if ((left == 0) && (right == 0) && (up == 1) && (down == 0))
				maketile = Terrain_ThickSpiceUp; //

		else if ((left == 0) && (right == 0) && (up == 0) && (down == 1))
				maketile = Terrain_ThickSpiceDown; //

		else if ((left == 0) && (right == 0) && (up == 1) && (down == 1))
				maketile = Terrain_ThickSpiceUpDown; //missing left and right

		else if ((left == 0) && (right == 0) && (up == 0) && (down == 0))
				maketile = Terrain_ThickSpiceIsland; //missing all edges 
///////
		map->cell[i][j].setTile(maketile);
	}
 }

void smooth_terrain()
{
int i, j;
// Smooth the darn terrain

 for (i = 0; i < map->sizeX; i++)
 for (j = 0; j < map->sizeY; j++)
	 smooth_spot(i, j);
}

void thick_spots(int tpe)  //removes holes in rock and spice
{
 for (int i = 0; i < map->sizeX; i++)
 for (int j = 0; j < map->sizeY; j++)
  {
         if (map->cell[i][j].getType() != tpe)                  // Found something else than what thickining 
	 {
           if (side4(i, j, tpe) >= 3)
                map->cell[i][j].setType(tpe);                // Seems enough of the type around it so make this rock

           if (side4(i, j, tpe) == 2)                // Gamble, fifty fifty... rock or not?
		{
		 if (getRandomInt(0,1) == 1) 
			map->cell[i][j].setType(tpe);
		}		
	 }
  }
}

void make_spot(int cellX, int cellY, int tpe)
{
int dir;		// Direction
int j;			// Loop control
 for (j = 0; j < 1000; j++)
  {
   dir = getRandomInt(0,3);	// Random Dir
	switch(dir)
	 {
          case 0 : cellX--; break;
          case 1 : cellX++; break;
          case 2 : cellY--; break;
          case 3 : cellY++; break;
	 }
        fix_cell(cellX, cellY);
   if (tpe == Terrain_Spice)
	{
        if (map->cell[cellX][cellY].getType() == Terrain_Rock)
		continue;		// Do not place the spice spot, priority is ROCK!
	}
   map->cell[cellX][cellY].setTile(1);
	map->cell[cellX][cellY].setType(tpe);
  }
}

void add_rock_bits(int amount)
{
	int spotX, spotY;
	int done = 0;
	while (done < amount)
	{
		spotX = getRandomInt(0, map->sizeX-1);
		spotY = getRandomInt(0, map->sizeY-1);
		if (map->cell[spotX][spotY].getType() == Terrain_Sand)
		{
			map->cell[spotX][spotY].setTile(Terrain_t10);      // Rock bit
			map->cell[spotX][spotY].setType(Terrain_Rock);
		}

		done++;
	}
}

void add_blooms(int amount)
{
int spotX, spotY;
int done = 0;
while (done < amount)
{
	spotX = getRandomInt(0, map->sizeX-1);
	spotY = getRandomInt(0, map->sizeY-1);
	if (map->cell[spotX][spotY].getType() == Terrain_Sand && map->cell[spotX][spotY].getTile() == Terrain_a1)
		map->cell[spotX][spotY].setTile(getRandomInt(Terrain_a2,Terrain_a3));      // Spice bloom
	done++;
   
} // WHILE

}
void make_random_map(int sizeX, int sizeY)
{
int i, count;
int spots = ROCKFIELDS;
int fields = SPICEFIELDS;
int spotX, spotY;

	map = new MapClass(sizeX, sizeY);

clear_terrain(Terrain_a1, Terrain_Sand);

 for (i = 0; i < spots; i++)
  {
   spotX = getRandomInt(0, map->sizeX-1);
   spotY = getRandomInt(0, map->sizeY-1);

   make_spot(spotX, spotY, Terrain_Rock);
  }

// Spice fields
 for (i = 0; i < fields; i++)
  {
   spotX = getRandomInt(0, map->sizeX-1);
   spotY = getRandomInt(0, map->sizeY-1);

   make_spot(spotX, spotY, Terrain_Spice);
  }

 for (count = 0; count < ROCKFILLER; count++)
	thick_spots(Terrain_Rock); //SPOT ROCK

 for (count = 0; count < SPICEFILLER; count++)
	thick_spots(Terrain_Spice);

 	add_rock_bits(getRandomInt(0,9));
	add_blooms(getRandomInt(0,9));

	smooth_terrain();
}

int readMapPlayerNumbers(char* filename, int numbers[MAX_PLAYERS])
{
	int maxPlayers = NONE;
	FILE *file = fopen(filename, "rb");
	if (!file)
		fprintf(stderr, "Could not open map file %s.\n", filename);
	else
	{
		int magicNum;

		fread(&magicNum, sizeof(int), 1, file);
		if (magicNum == MAPMAGIC)
		{
			fread(&maxPlayers, sizeof(int), 1, file);
			if ((maxPlayers >= 0) && (maxPlayers <= MAX_PLAYERS))
			{
				//read the players details
				for (int i = 0; i < maxPlayers; i++)
					fread(&numbers[i], sizeof(int), 1, file);
			}
		}

		fclose(file);
	}

	return maxPlayers;
}

int readMapMaxPlayers(char* filename)
{
	int maxPlayers = 0;
	FILE *file = fopen(filename, "rb");
	if (!file)
		fprintf(stderr, "Could not open map file %s.\n", filename);
	else
	{
		int		magicNum;

		fread(&magicNum, sizeof(int), 1, file);
		if (magicNum == MAPMAGIC)
		{
			fread(&maxPlayers, sizeof(int), 1, file);
			if ((maxPlayers <= 0) || (maxPlayers > MAX_PLAYERS))
				maxPlayers = 0;
		}

		fclose(file);
	}

	return maxPlayers;
}

bool loadNewMap(FILE* file)
{
	bool	found[MAX_PLAYERS],
			ok = false;
	short	mapSizeX, mapSizeY;
	int		i, j, x, y, itemID, num,
			playerNum, maxPlayers, givenPlayer;
	UnitClass*		newUnit;
	StructureClass*	newStructure;

	fread(&maxPlayers, sizeof(int), 1, file);

	for (i = 0; i < maxPlayers; i++)
		found[i] = false;
	//read the players details
	for (i = 0; i < maxPlayers; i++)
	{
		fread(&j, sizeof(int), 1, file);
		if ((j >= 0) && (j < MAX_PLAYERS))
			found[j] = true;
		else
			return false;
	}

	fread(&mapSizeX, sizeof(short), 1, file);
	fread(&mapSizeY, sizeof(short), 1, file);
	if ((mapSizeX >= 0) && (mapSizeX < MAX_XSIZE) && (mapSizeY >= 0) && (mapSizeY < MAX_YSIZE))
		ok = true;
	else
		fprintf(stderr, "Level size is unsupported %d,%d.", mapSizeX, mapSizeY);

	if (ok)
	{
		map = new MapClass(mapSizeX, mapSizeY);

		//load terrain data
		for (i=0; i<map->sizeX; i++)
		for (j=0; j<map->sizeY; j++)
		{
			fread(&x, sizeof(int), 1, file);
			map->getCell(i, j)->setType(x);
			fread(&x, sizeof(int), 1, file);
			map->getCell(i, j)->setTile(x);
		}

		map->createSandRegions();
		smooth_terrain();

		//load the structure info
		fread(&num, sizeof(int), 1, file);
		for (i=0; i<num; i++)
		{
			fread(&playerNum, sizeof(int), 1, file);
			fread(&itemID, sizeof(int), 1, file);
			fread(&x, sizeof(int), 1, file);
			fread(&y, sizeof(int), 1, file);

			//find the player associated to this map player
			for (givenPlayer = 0; givenPlayer < settings.maxPlayers; givenPlayer++)
			if ((player[givenPlayer] != NULL) && (player[givenPlayer]->getMapPlayerNum() == playerNum))
				break;	//found it

			if (givenPlayer < settings.maxPlayers)
			{	//found player to give to
				newStructure = (StructureClass*)player[givenPlayer]->placeStructure(NONE, NONE, NONE, itemID, x, y);
				if ((itemID == Structure_GunTurret)
					|| (itemID == Structure_RocketTurret))
				{
					fread(&x, sizeof(int), 1, file);
					((GunTurretClass*)newStructure)->setTurretAngle(x);
				}
			}	//don't make the structure because no owner associated
			else if ((itemID == Structure_GunTurret)
				|| (itemID == Structure_RocketTurret))
				fread(&x, sizeof(int), 1, file);	//still read all of structure info
		}

		//load the unit info
		fread(&num, sizeof(int), 1, file);
		for (i=0; i<num; i++)
		{
			fread(&playerNum, sizeof(int), 1, file);
			fread(&itemID, sizeof(int), 1, file);
			fread(&x, sizeof(int), 1, file);
			fread(&y, sizeof(int), 1, file);

			//find the player associated to this map player
			for (givenPlayer = 0; givenPlayer < settings.maxPlayers; givenPlayer++)
			if ((player[givenPlayer] != NULL) && (player[givenPlayer]->getMapPlayerNum() == playerNum))
				break;	//found it

			if (givenPlayer < settings.maxPlayers)
			{	//found player to give to
				newUnit = ((UnitClass*)player[givenPlayer]->placeUnit(itemID, x, y));
				fread(&x, sizeof(int), 1, file);
				if (newUnit)
				{
					newUnit->setAngle(x);
					if (newUnit->isTurreted())
					{
						fread(&x, sizeof(int), 1, file);				
						((TankClass*)newUnit)->setTurretAngle(x);
					}
				}
			}
			else
			{
				fread(&x, sizeof(int), 1, file);	//still read all of unit info
				if ((itemID == Unit_Tank) || (itemID == Unit_SiegeTank))	//nasty hack - FIXME
					fread(&x, sizeof(int), 1, file);
			}
		}

		map->setWinFlags(3);
	}

	return ok;
}

bool loadOldMap(FILE* mapFile)
{
	bool	ok = false,
			done = false;	//this will be set to false if any errors, so level won't load

	char	*crud,
			temp[MAX_LINE],
			line[MAX_LINE];

	int		i = 0,
			j = 0;

	FILE	*seedFile;

	do
		crud = fgets(line, MAX_LINE, mapFile);
	while ((crud != NULL)&& !strstr(line, "[BASIC]"));		
		
	if (crud == NULL)
		ok = false;
	else	//now find the line which states the map seed
	{
		int	x = 62, y = 62,
			winFlags = 3;
		fseek(mapFile, 0, SEEK_SET);
		while (!done && (fgets(line, MAX_LINE, mapFile) != NULL))
		{
			if (strstr(line, "MapScale="))
			{
				i = j = 0;
				done = true;
			}
		}

		done = false;
		fseek(mapFile, 0, SEEK_SET);
		while (!done && (fgets(line, MAX_LINE, mapFile) != NULL))
		{
			if (strstr(line, "WinFlags="))
			{
				i = j = 0;
				while (line[i++] != '=');
				while (line[i] != '\n' && line[i] != ' ')
					temp[j++] = line[i++];

				winFlags = atoi(temp);
				done = true;
			}
		}

		map = new MapClass(x, y);
		map->setWinFlags(winFlags);

		do
			crud = fgets(line, MAX_LINE, mapFile);
		while ((crud != NULL) && !strstr(line, "[MAP]"));

		done = false;
		char	*loc, *loc2;
		while (!done && (fgets(line, MAX_LINE, mapFile) != NULL))
		{
			loc = strstr(line, "Seed=") ;
			if (loc)
			{
				loc+=5;
				loc2 = strstr(loc, "\n");
				loc2[0] = '\0';
				
				/*i = j = 0;
				while (line[i++] != '=');
				while (line[i] != '\n' && line[i] != ' ')
					temp[j++] = line[i++];
				int z;
				for (z = 0; z < 4; z++)
					seedFileName[z] = temp[z];*/
				char path[MAX_LINE] = "data/scenario/seed/";
				strncat(path, loc, MAX_LINE);
				strncat(path, ".map", MAX_LINE);
				seedFile = fopen(path, "r");
				if (!seedFile)
				{
					fprintf(stderr, "Couldn't find seed file %s\n", path);
					delete map;
					make_random_map(64, 64);
				}
				else
				{
					for (j = 0; j < map->sizeY; j++)
					{
						if (fgets(line, MAX_LINE, seedFile) != NULL)
						{
							for (i = 0; i < map->sizeX; i++)
							{
								int type;

								if (line[i] == ')')
									type = Terrain_Sand;
								else if ((line[i] == '^') || (line[i] == '%') || (line[i] == '(') || (line[i] == '$'))
									type = Terrain_Rock;
								else if ((line[i] == 'R') || (line[i] == 'r'))
									type = Terrain_Mountain;
								else if (line[i] == '-')
									type = Terrain_Spice;
								else if (line[i] == '+')
									type = Terrain_ThickSpice;
								else
									type = Terrain_Dunes;

								map->cell[i][j].setType(type);
								map->cell[i][j].setTile(0);
							}
						}
					}
					ok = true;
					done = true;

					fclose(seedFile);
				}
			}
		}
		map->createSandRegions();
	}

	if (ok)
	{
		ok = false;
		//now load the units
		fseek(mapFile, 0, SEEK_SET);
		do
			crud = fgets(line, MAX_LINE, mapFile);
		while ((crud != NULL) && !strstr(line, "[UNITS]"));

		if (crud == NULL)
		{
			//fprintf(stderr, "This file is not a valid mission file %s. (no UNITS)\n", mapName);
			ok = false;
		}
		else	//now go and create these units
		{
			done = false;
			int	house,
				unitID;
			COORDTYPE pos;

			char	*houseString,
					*positionString,
					*typeString;
			while (!done && (fgets(line, MAX_LINE, mapFile) != NULL))
			{
				if (line[0] == '[')
					done = true;
				else if ((line[0] != ';') && strstr(line, "="))
				{
					strtok(line, "=,\n\r");
					houseString = strtok(NULL, "=,\n\r");

					if (houseString == NULL)
						house = HOUSE_ATREIDES;
					else if (strcmp(houseString, "Atreides")==0)
						house = HOUSE_ATREIDES;
					else if (strcmp(houseString, "Ordos")==0)
						house = HOUSE_ORDOS;
					else if (strcmp(houseString, "Harkonnen")==0)
						house = HOUSE_HARKONNEN;
					else if (strcmp(houseString, "Fremen")==0)
						house = HOUSE_FREMEN;
					else if (strcmp(houseString, "Sardaukar")==0)
						house = HOUSE_SARDAUKAR;
					else if (strcmp(houseString, "Mercenary")==0)
						house = HOUSE_MERCENARY;

					typeString = strtok(NULL, "=,\n\r");
					strtok(NULL, "=,\n\r");
					positionString = strtok(NULL, "=,\n\r");

					if (positionString == NULL)
						i = 0;
					else
						i = toInt(positionString);
					pos.x = i%(map->sizeX+2) - 1;
					pos.y = i/(map->sizeX+2) - 1;

					if (typeString == NULL)
						unitID = Unit_Quad;						
					else if (strcmp(typeString, "Devastator")==0)
						unitID = Unit_Devastator;
					else if (strcmp(typeString, "Deviator")==0)
						unitID = Unit_Deviator;
					else if (strcmp(typeString, "Harvester")==0)
						unitID = Unit_Harvester;
					else if (strcmp(typeString, "Infantry")==0)
					{	//make three
						unitID = Unit_Infantry;
						player[house]->placeUnit(unitID, pos.x, pos.y);
						player[house]->placeUnit(unitID, pos.x, pos.y);
					}
					else if (strcmp(typeString, "Launcher")==0)
						unitID = Unit_Launcher;
					else if (strcmp(typeString, "MCV")==0)
						unitID = Unit_MCV;
					else if (strcmp(typeString, "Quad")==0)
						unitID = Unit_Quad;
					else if (strcmp(typeString, "Sandworm")==0)
						unitID = Unit_Sandworm;
					else if (strcmp(typeString, "Siege Tank")==0)
						unitID = Unit_SiegeTank;
					else if (strcmp(typeString, "Sonic Tank")==0)
						unitID = Unit_SonicTank;
					else if (strcmp(typeString, "Soldier")==0)
						unitID = Unit_Infantry;
					else if (strcmp(typeString, "Tank")==0)
						unitID = Unit_Tank;
					else if (strcmp(typeString, "Trike")==0)
						unitID = Unit_Trike;
					else if (strcmp(typeString, "Raider")==0)
						unitID = Unit_Raider;
					else if (strcmp(typeString, "Troopers")==0)
					{	//make three
						unitID = Unit_Trooper;
						player[house]->placeUnit(unitID, pos.x, pos.y);
						player[house]->placeUnit(unitID, pos.x, pos.y);
					}
					else if (strcmp(typeString, "Trooper")==0)
						unitID = Unit_Trooper;
					else
						unitID = Unit_Quad;
						
					if (!player[house]->placeUnit(unitID, pos.x, pos.y))
						fprintf(stderr, "This file is not a valid unit entry: %s. (invalid unit position)\n", line);

					ok = true;
				}
			}
		}
	}

	if (ok)
	{
		ok = false;
		//now load the structures
		fseek(mapFile, 0, 0);
		do
			crud = fgets(line, MAX_LINE, mapFile);
		while ((crud != NULL) && !strstr(line, "[STRUCTURES]"));

		if (crud == NULL)
		{
//			fprintf(stderr, "This file is not a valid mission file %s. (no STRUCTURES)\n", mapName);
			ok = false;
		}
		else	//now go and create these structures
		{
			int house = HOUSE_ATREIDES,
				xPos, yPos;

			char	*classString,
					*houseString,
					*positionString,
					*typeString;

			StructureClass* tempStructure;

			done = false;
			while (!done && (fgets(line, MAX_LINE, mapFile) != NULL))
			{
				if (line[0] == '[')	//no structures listed under heading structures
					done = true;
				else if ((line[0] != ';') && strstr(line, "="))
				{
					classString = strtok(line, "=,\n\r");
					houseString = strtok(NULL, "=,\n\r");
					typeString = strtok(NULL, "=,\n\r");

					if (houseString == NULL)
						house = HOUSE_ATREIDES;
					else if (strcmp(houseString, "Atreides")==0)
						house = HOUSE_ATREIDES;
					else if (strcmp(houseString, "Ordos")==0)
						house = HOUSE_ORDOS;
					else if (strcmp(houseString, "Harkonnen")==0)
						house = HOUSE_HARKONNEN;
					else if (strcmp(houseString, "Fremen")==0)
						house = HOUSE_FREMEN;
					else if (strcmp(houseString, "Sardaukar")==0)
						house = HOUSE_SARDAUKAR;
					else if (strcmp(houseString, "Mercenary")==0)
						house = HOUSE_MERCENARY;

					if (strstr(classString, "GEN"))
					{
						strtok(classString, "N=,\n\r");	//we want the cell location after GEN seperate by 'N'
						positionString = strtok(NULL, "N=,\n\r");
						if (positionString == NULL)
							i = 0;
						else
							i = toInt(positionString);

						xPos = i%(map->sizeX+2) - 1;
						yPos = i/(map->sizeX+2) - 1;

						if ((xPos >= 0) && (xPos < map->sizeX) && (yPos >= 0) && (yPos < map->sizeY))
						{
							if (strcmp(typeString, "Concrete")==0)
								player[house]->placeStructure(NONE, NONE, NONE, Structure_Slab1, xPos, yPos);
							else if (strcmp(typeString, "Wall")==0)
								player[house]->placeStructure(NONE, NONE, NONE, Structure_Wall, xPos, yPos);;

							ok = true;
						}
					}
					else
					{
						strtok(NULL, "=,\n\r");
						positionString = strtok(NULL, "=,\n\r");	//we want the cell location after GEN seperate by 'N'
						if (positionString == NULL)
							i = 0;
						else
							i = toInt(positionString);

						xPos = i%(map->sizeX+2) - 1;
						yPos = i/(map->sizeX+2) - 1;

						int itemID = 0;

						if ((xPos >= 0) && (xPos < map->sizeX) && (yPos >= 0) && (yPos < map->sizeY))
						{
							tempStructure = NULL;
							if (typeString == NULL)
								itemID = 0;
							else if (strcmp(typeString, "Barracks")==0)
								itemID = Structure_WOR;//Structure_Barracks;
							else if (strcmp(typeString, "Const Yard")==0)
								itemID = Structure_ConstructionYard;
							else if (strcmp(typeString, "R-Turret")==0)
								itemID = Structure_RocketTurret;
							else if (strcmp(typeString, "Turret")==0)
								itemID = Structure_GunTurret;
							else if (strcmp(typeString, "Heavy Fctry")==0)
								itemID = Structure_HeavyFactory;
							else if (strcmp(typeString, "Hi-Tech")==0)
								itemID = Structure_HighTechFactory;
							else if (strcmp(typeString, "IX")==0)
								itemID = Structure_IX;
							else if (strcmp(typeString, "Light Fctry")==0)
								itemID = Structure_LightFactory;
							else if (strcmp(typeString, "Palace")==0)
								itemID = Structure_Palace;
							else if (strcmp(typeString, "Outpost")==0)
								itemID = Structure_Radar;
							else if (strcmp(typeString, "Refinery")==0)
								itemID = Structure_Refinery;
							else if (strcmp(typeString, "Repair")==0)
								itemID = Structure_RepairYard;
							else if (strcmp(typeString, "Spice Silo")==0)
								itemID = Structure_Silo;
							else if (strcmp(typeString, "Star Port")==0)
								itemID = Structure_StarPort;
							else if (strcmp(typeString, "Windtrap")==0)
								itemID = Structure_WindTrap;
							else if (strcmp(typeString, "WOR")==0)
								itemID = Structure_WOR;

							if (itemID)
								player[house]->placeStructure(NONE, NONE, NONE, itemID, xPos, yPos);

							ok = true;
						}
					}
				}
			}
		}

		smooth_terrain();
	}

	return ok;
}

bool loadMap(char* mapName)
{
	bool	ok = false;
	FILE	*mapFile = fopen(mapName, "rb");
	if (!mapFile)
		fprintf(stderr, "Could not open map file %s.\n", mapName);
	else
	{
		int		magicNum;

		fread(&magicNum, sizeof(int), 1, mapFile);
		if (magicNum != MAPMAGIC)
		{
			fclose(mapFile);
			mapFile = fopen(mapName, "r");
			if (mapFile)
				ok = loadOldMap(mapFile);
		}
		else
			ok = loadNewMap(mapFile);			

		fclose(mapFile);

		if (!structureList->isEmpty())
		{
			COORDTYPE* pos;
			StructureClass	*tempStructure;

			structureList->reset();
			while(structureList->currentNotNull())
			{
				tempStructure = (StructureClass*)structureList->getCurrent();
				if (tempStructure->getItemID() == Structure_Refinery)
				{
					pos = tempStructure->getLocation();
					if (gameState != GS_NONE)
						tempStructure->getOwner()->freeHarvester(pos->x, pos->y);
				}
				structureList->nextLink();
			}
		}
	}

	return ok;
}

