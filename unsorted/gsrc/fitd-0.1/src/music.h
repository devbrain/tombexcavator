#ifndef _MUSIC_H_
#define _MUSIC_H_
extern unsigned int musicChrono;
int initMusicDriver(void);
void destroyMusicDriver(void);
int fadeMusic(int param1, int param2, int param3);
#endif