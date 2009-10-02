#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <SDL_image.h>
#include <map>
#include <string>
#include <SDL_TTF.h>

using namespace std;

#undef main

#define TILE_WIDTH 32
#define TILE_HEIGHT 16

unsigned char get_char(FILE *f)
{
    
    unsigned char c1 = fgetc(f);
    
    return c1;
}

short get_word(FILE *f)
{
    short key = 0;
    int c1 = fgetc(f);
    if (feof(f))
    {
	return key;
    }
    int c2 = fgetc(f);
    if (feof(f))
    {
	return key;
    }

    key = c1 * 256 + c2;

    return key;
}

class ConfigFile
{
  private:
    static bool parse_natural(const char *buf, int &out)
    {
	out = atoi(buf);
	return out > 0;
    }
    
  public:

    string tile_image_file;
    int tile_width, tile_height, map_nb_cols, map_nb_rows, index_type_size;
    int index_offset;
    
    ConfigFile(const char *cf)
    {
	FILE *f = fopen(cf,"rt");
	if (f == 0)
	{
	    fprintf(stderr,"Cannot open config file %s\n",cf);
	    exit(1);
	}
	char buf[100];

	bool error;
	
	for (int i = 0; i < 7; i++)
	{
	    fgets(buf, sizeof(buf)-2, f);

	    buf[strlen(buf)-1] = '\0';
	    
	    error = feof(f);

	    if (!error)
	    {
		switch(i)
		{
		  case 0:
		    tile_image_file = buf;
		    break;
		  case 1:
		    error = !parse_natural(buf,tile_width);
		    break;
		  case 2:
		    error = !parse_natural(buf,tile_height);
		    break;
		  case 3:
		    error = !parse_natural(buf,map_nb_cols);
		    break;
		  case 4:
		    error = !parse_natural(buf,map_nb_rows);
		    break;
		  case 5:		    
		    error = !parse_natural(buf,index_type_size);
		    if (!error)
		    {
			error = (index_type_size > 2);
		    }
		  case 6:		    
		    index_offset = atoi(buf);
		    break;
		}
	    }
	    if (error)
	    {
		fprintf(stderr,"Error while parsing file %s line %d\n",cf,i+1);
		exit(1);
	    }
	}

	fclose(f);
    }

    
};
    
int main(unsigned int argc, char **argv)
{
    
    unsigned char *map_data = NULL;
    int nb_rows,nb_cols,nb_tiles;
    SDL_Surface **tile_array;
    int i,j;
    
    if (argc < 4) { fprintf(stderr,"Usage: map_writer <config_file> <map data file> <map output file>\n"
	    "\n"
	    "Config file is as follows: \n"
	    "\n"
	    "tile_image_file\n"
	    "tile_width\n"
	    "tile_height\n"
	    "map_nb_cols\n"
	    "map_nb_rows\n"
	    "index_type_size (1 or 2)\n"
	    "index_offset (0, -1, 1, ...) \n\n"
	    "example:\n\n"
	    "tiles_demo.png\n"
	    "32\n"
	    "16\n"
	    "128\n"
	    "64\n"
	    "1\n"
	    "1\n\n");
	
	exit(1);
    }

    printf("map writer (c) 2009 JOTD\n\n");
    

    // load image

    ConfigFile cf(argv[1]);
    
    SDL_Surface *tiles = IMG_Load(cf.tile_image_file.c_str());
    if(tiles == NULL)
    {
	fprintf(stderr,"problem while opening image %s\n",cf.tile_image_file.c_str());
	exit(1);
    }

    nb_rows = tiles->h / cf.tile_height;
    nb_cols = tiles->w / cf.tile_width;

    tile_array = (SDL_Surface **)malloc(sizeof(SDL_Surface *) * nb_rows * nb_cols);

    
    SDL_Rect src_clip;
    src_clip.w = cf.tile_width;
    src_clip.h = cf.tile_height;
    src_clip.y = 0;

    nb_tiles = nb_rows * nb_cols;

    printf("loaded %d tiles, nb_cols=%d, nb_rows=%d\n",nb_tiles,nb_cols,nb_rows);
    //TTF_Init();
    
    // create tiles

    int k = 0;
    
    for (i = 0; i < nb_rows; i++)
    {
	src_clip.x = 0;
	
	for (j = 0; j < nb_cols; j++)
	{
	    tile_array[k] = SDL_CreateRGBSurface
	    (SDL_SWSURFACE, cf.tile_width, cf.tile_height, 32,
	     0xff0000, 0xff00, 0xff, 0x0);

	    SDL_BlitSurface(tiles, &src_clip, tile_array[k], 0);
	    
	    src_clip.x += TILE_WIDTH;

	    k++;

	}
	src_clip.y += TILE_HEIGHT;
    }

    //TTF_Font *fntCourier = TTF_OpenFont( (string(getenv("WINDIR")) + string("/Fonts/cour.ttf")).c_str(), 11 );

    int map_w_index = 0;
    int map_h_index = 0;
    
    FILE *f = fopen(argv[2],"rb");
    if (f != NULL)
    {
	
	short idx=0, key;
	int map_width = cf.map_nb_cols * cf.tile_width;
	int map_height = cf.map_nb_rows * cf.tile_height;
	
	int nb_values = cf.map_nb_cols * cf.map_nb_rows;
	
	printf("nb map indexes = %d\n",nb_values);
	
	SDL_Surface *output = SDL_CreateRGBSurface
	(SDL_SWSURFACE, map_width, map_height, 32,
	 0xff0000, 0xff00, 0xff, 0x0);

	map_data = new unsigned char[nb_values];

	for (int i = 0; i < nb_values; i++)
	{
	    int kindex= 0;
	    
	    if (cf.index_type_size == 1)
	    {
		kindex = (int)get_char(f);
	    }
	    else
	    {
		// 2
		kindex = (int)get_char(f) * 256 + (int)get_char(f);
	    }
	    if (feof(f))
	    {
		break;
	    }
	 

	    // blit to map
	    
	    
		
		src_clip.x = map_w_index;
		src_clip.y = map_h_index;

		kindex += cf.index_offset;
		
		if ((kindex < nb_tiles) && (kindex >= 0))
		{
		    SDL_BlitSurface(tile_array[kindex], 0, output, &src_clip);
		}
		
	
		
	    map_w_index += cf.tile_width;
	    if (map_w_index >= map_width)
	    {
		map_w_index = 0;
		map_h_index += cf.tile_height;
	    }
	    
	}
	fclose(f);
	SDL_SaveBMP(output, argv[3]);

	SDL_FreeSurface(output);
	//TTF_CloseFont( fntCourier );
    }
    else
    {
	fprintf(stderr,"Can't open %s\n",argv[2]);
	exit(1);
    }
    
    
    delete [] map_data;
    free(tile_array);

    return 0;
}
