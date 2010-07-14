struct BITMAP16
{
    unsigned int width;
    unsigned int height;
    unsigned long linewidth;    /* width of line in bytes (includes padding) */
    unsigned int bpp;
    unsigned int colours;
    unsigned char **lines;
    unsigned char *bits;
};
typedef struct BITMAP16 BITMAP16;

void bmp_free(BITMAP16 *bmp);
BITMAP16 *bmp_create(int width, int height, int bpp);
BITMAP16 *bmp_load(char *fname);
int bmp_save(BITMAP16 *bmp, char *fname, int backup);
int bmp_putpixel(BITMAP16 *bmp, int x, int y, int c);
int bmp_getpixel(BITMAP16 *bmp, int x, int y);
void bmp_rect(BITMAP16 *bmp, int x1, int y1, int x2, int y2, int c);
BITMAP16 *bmp_duplicate(BITMAP16 *bmp);
int bmp_split(BITMAP16 *bmp, BITMAP16 **red, BITMAP16 **green, BITMAP16 **blue, BITMAP16 **bright);
BITMAP16 *bmp_merge(BITMAP16 *red, BITMAP16 *green, BITMAP16 *blue, BITMAP16 *bright);
void bmp_blit(BITMAP16 *src, unsigned int srcx, unsigned int srcy, BITMAP16 *dest, unsigned int destx, unsigned int desty, unsigned int width, unsigned int height);



