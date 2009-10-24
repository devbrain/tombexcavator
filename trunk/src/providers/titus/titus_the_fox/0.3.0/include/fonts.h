#ifndef FONTS_H
#define FONTS_H

#define CHAR_a 97
#define CHAR_A 65
#define CHAR_f 102
#define CHAR_F 70
#define CHAR_z 122
#define CHAR_Z 90
#define CHAR_0 48
#define CHAR_9 57
#define CHAR_EXCLAMATION 33
#define CHAR_QUESTION 63
#define CHAR_DOT 46
#define CHAR_DOLLAR 36
#define CHAR_UNDERSCORE 95

SDL_Surface * SDL_Text(char *text, int inv_back);
int SDL_Print_Text(char *text, int x, int y, int inv_back);
int viewintrotext();

#endif

