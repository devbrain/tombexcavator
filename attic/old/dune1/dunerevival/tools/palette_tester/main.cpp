#include <allegro.h> 
#include <stdio.h>




#define OFFSET 0


#define BUFFERSIZE 10000


#define SIZE_STEP 1
#define OFS_STEP 1



int offset = OFFSET;
int width =100;
int height=100;

unsigned char* palette;
unsigned char generic_palette[768];

//int palette_offset =1;

void main_loop() {

	/*
	//generate generic palette
	for (int i=0; i<256;i++) {
		generic_palette[3*i] = i;
		generic_palette[3*i+1] = i;
		generic_palette[3*i+2] = i;
	}
*/

	BITMAP *bmp = create_bitmap(320, 200);
	unsigned char buffer[BUFFERSIZE];

	

	//LOAD DATA FROM FILE
	FILE* f;
	f=fopen("c:\\dune\\cryo.___","r");
	if (!f) {
		allegro_message("erreur fichier");
		return;
	}
	//load data
	while (fread(&buffer,sizeof(unsigned char),BUFFERSIZE,f) != 0);
	fclose(f);




	//DISPLAY DATA with user-control on width and offset
	bool quit = false;
	while (!quit) {


		//palette = buffer+palette_offset;
		palette = generic_palette;

		clear_bitmap(bmp);


		palette = &buffer[offset];

		
		for (int j=0; j<height;j++) {
			for (int i=0; i<width;i++) {
				if (width*j+i < BUFFERSIZE) {


					if (i<320 && j<200) {

						
						int value = buffer[offset+width*j+i];

						int color;
						
						/*
						if (value == 0)
							color = 0xFF00FF00;
						else if (value == 255)
							color = 0xFFFF0000;
						else {*/
							int r= palette[3*value];
							int g= palette[3*value+1];
							int b= palette[3*value+2];

							color = (255<<24)+(r<<16)+(g<<8)+b;
						//}
						putpixel(bmp, i, j, color);
					}
				}
			}
		}
		blit(bmp, screen, 0, 0, 0, 0, 320, 200);


		while(!keypressed()) {}

		int touche=readkey();
		if(touche>>8 == KEY_UP) { 
				offset+=OFS_STEP;
		}
		if(touche>>8 == KEY_DOWN) {
				offset-=OFS_STEP;
		}
		/*
		if(touche>>8 == KEY_LEFT) {
#ifdef VERTICAL
			width-=SIZE_STEP;
#else
			height-=SIZE_STEP;
#endif				
		}
		if(touche>>8 == KEY_RIGHT) {
#ifdef VERTICAL
			width+=SIZE_STEP;
#else
			height+=SIZE_STEP;
#endif
		}
		*/
		if(touche>>8 == KEY_ESC) { quit = true; }
	}


	destroy_bitmap(bmp); //Release the bitmap data

}

int main(int argc, char *argv[]) 
{
	allegro_init(); // Initialize Allegro 

	install_keyboard(); // Initialize keyboard routines
	
	
	set_color_depth(32); // Set the color depth
	set_gfx_mode(GFX_AUTODETECT, 320,200,0,0); // Change our graphics mode to 640x480
	
	main_loop();


	allegro_message("w=%d, h=%d, offs=%d", width, height, offset);


	return 0; // Exit with no errors 
}
END_OF_MAIN();
