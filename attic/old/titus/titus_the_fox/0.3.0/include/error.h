#ifndef ERROR_H
#define ERROR_H

#define ERROR_QUIT -1
#define ERROR_NOT_ENOUGH_MEMORY -2
#define ERROR_FILE_NOT_FOUND -3
#define ERROR_INVALID_FILE -4
char lasterror[200];
int lasterrornr; //Only to be used when needed, f.ex. when return value is not int (f.ex. in function SDL_Text) (maybe this always should be used?)
void checkerror(void);

#endif
