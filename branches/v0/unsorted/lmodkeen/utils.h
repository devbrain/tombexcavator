void quit(char *message, ...);
FILE *openfile(char *filename, char *access, int backup);
int fileexists(char *filename);
int get_exe_image_size(FILE *f, unsigned long *imglen, unsigned long *headerlen);
