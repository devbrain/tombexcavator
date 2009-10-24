#ifndef DUNE_CUTSCENE_F
#define DUNE_CUTSCENE_F

#define CUTSCENE_POS_TOP_LEFT 0
#define CUTSCENE_POS_TOP_CENTRE 1
#define CUTSCENE_POS_TOP_RIGHT 2
#define CUTSCENE_POS_BOTTOM_LEFT 3
#define CUTSCENE_POS_BOTTOM_CENTRE 4
#define CUTSCENE_POS_BOTTOM_RIGHT 5
#define CUTSCENE_POS_CENTRE 6


typedef struct Dune_Scene_VO
{
	Mix_Chunk	*sound;
	int			playAtFrame;
	bool		played;
	bool		effect;//is an effect, not a vouice over
	bool		hasText;
	int			colour;//colour of the text
	char		*text;//if you want the underneath text
} DuneSceneVO;

typedef struct Dune_Scene_Text
{
	int			playAtFrame;
	char		*text;//if you want the underneath text
	int			colour;//colour of the text
	bool		shown;
	int			pos;
} DuneSceneText;

typedef struct Dune_Scene
{
	SDL_Surface	**frames;//Frames

	int hasUnderLay;
	int underLayFrame;

	Dune_Scene_VO **voSounds;//Voice over sound ID's
	Dune_Scene_Text **subtitles;
	int nSubtitles;
	int nVOSounds;
	int voChannel, effectsChannel;//Sound channels, VO and Effects

	int pos;

	char *basePath;

	int nFrames;
	int curFrame;
	int frameTime;

	bool hasSets;
	int nSets;
	int *setFrames;
	int setMod;

	int loops;
	int endWaitTime;


} DuneScene;



class DuneCutScene
{
public:
	DuneCutScene();
	~DuneCutScene();

	
	void loadScenes();
	void parseScenesFile(char *filename);
	void playCutScene();
	
	
protected:
	void loadSceneImages(Dune_Scene* scene);
	char *getFramePath(char *basePath, int frame);
	char *getFramePathAlpha(char *basePath, int frame, int set);
	void loadPalette(char *filename);
	void handleInput();
	void unloadScenes();
	void playVOSounds(Dune_Scene *s);
	bool allSceneSoundsPlayed(Dune_Scene *s);
	void showText(int x, int y, char *text, int colour);
	void placeSubtitle(char *text, int colour);


	int nScenes;
	SDL_Color colours[256];
	
	int curScene;
	Dune_Scene **scenes;
	SDL_Surface *textSurface;
	SDL_Rect textPos;

	bool skipScene;

	int voChan, effectsChan;

	char *zipPath;
	bool inZip;

};

#endif

