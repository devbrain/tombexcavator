#ifndef SETTINGS_H
#define SETTINGS_H

#define OPENTITUS_CONFIG_FILE "titus.conf"
#define OPENTITUS_WINDOW_TEXT "OpenTitus"

int readconfig(char *configfile);

int initcodes();

int initleveltitles();

char spritefile[256];
char levelfiles[16][256]; //16 levels in moktar, 15 levels in titus
char tituslogofile[256];
int tituslogoformat;
char titusintrofile[256];
int titusintroformat;
char titusmenufile[256];
int titusmenuformat;
char titusfinishfile[256];
int titusfinishformat;
char fontfile[256];
int levelcount;
int reswidth;
int resheight;
int bitdepth;
int ingamewidth;
int ingameheight;
int videomode;
int game;

char levelcode[16][5];
char leveltitle[16][41];

#endif
