#ifndef MENTATCLASS
#define MENTATCLASS

#include "zlib/SDL_rwops_zzip.h"
#include "zlib/zziplib.h"

typedef enum
{
	Mentat_Atreides,
	Mentat_Ordos,
	Mentat_Harkonnen,
	Mentat_Atreides_Eyes1,
	Mentat_Atreides_Eyes2,
	Mentat_Atreides_Eyes3,
	Mentat_Atreides_Eyes4,
	Mentat_Atreides_Eyes5,
	Mentat_Atreides_Mouth1,
	Mentat_Atreides_Mouth2,
	Mentat_Atreides_Mouth3,
	Mentat_Atreides_Mouth4,
	Mentat_Atreides_Mouth5,
	Mentat_Atreides_Shoulder,
	Mentat_Ordos_Eyes1,
	Mentat_Ordos_Eyes2,
	Mentat_Ordos_Eyes3,
	Mentat_Ordos_Eyes4,
	Mentat_Ordos_Eyes5,
	Mentat_Ordos_Mouth1,
	Mentat_Ordos_Mouth2,
	Mentat_Ordos_Mouth3,
	Mentat_Ordos_Mouth4,
	Mentat_Ordos_Mouth5,
	Mentat_Ordos_Shoulder,
	Mentat_Harkonnen_Eyes1,
	Mentat_Harkonnen_Eyes2,
	Mentat_Harkonnen_Eyes3,
	Mentat_Harkonnen_Eyes4,
	Mentat_Harkonnen_Eyes5,
	Mentat_Harkonnen_Mouth1,
	Mentat_Harkonnen_Mouth2,
	Mentat_Harkonnen_Mouth3,
	Mentat_Harkonnen_Mouth4,
	Mentat_Harkonnen_Mouth5,
	Mentat_Harkonnen_Shoulder,
	//Ordos Ring
	//Atreides Book
	Mentat_Unknown
} MENTAT_DATA;

#define FRAMES_CONCRETE_SLAB		1
#define FRAMES_BARRACKS			3
#define FRAMES_CARRYALL			2
#define FRAMES_CONST_YARD		1
#define FRAMES_ATRE_WORLD		30
#define FRAMES_HARK_WORLD		30
#define FRAMES_ORDOS_WORLD		30
#define FRAMES_FREMEN			2
#define FRAMES_FRIGATE			1
#define FRAMES_DEATHHAND		16
#define FRAMES_DEVASTATOR		1
#define FRAMES_HARVESTER		13
#define FRAMES_RADAR			10
#define FRAMES_HIGHTECH			2
#define FRAMES_SEIGETANK		1
#define FRAMES_HEAVYFACT		7
#define FRAMES_HEAVYINF			1
#define FRAMES_INFANTRY			1
#define FRAMES_IX			1
#define FRAMES_LIGHTFACT		14
#define FRAMES_COMBATTANK		1
#define FRAMES_MCV			1
#define FRAMES_DEVIATOR			1
#define FRAMES_ORNI			1
#define FRAMES_RAIDER			1
#define FRAMES_PALACE			17
#define FRAMES_REFINERY			6
#define FRAMES_REPAIRBAY		1
#define FRAMES_ROCKETTANK		1
#define FRAMES_ROCKETTURRET		1
#define FRAMES_SABOTEUR			1
#define FRAMES_SARDAUKAR		10
#define FRAMES_SLAB			1
#define FRAMES_SONICTANK		1
#define FRAMES_STARPORT			25
#define FRAMES_SILO			4
#define FRAMES_TRIKE			1
#define FRAMES_TURRET			1
#define FRAMES_WALL			1
#define FRAMES_WINDTRAP			12
#define FRAMES_WOR			8
#define FRAMES_WORM			1
#define FRAMES_QUAD			1

typedef enum
{
	Mentat_Concrete_Slab,
	Mentat_Barracks,
	Mentat_Carryall,
	Mentat_Construction_Yard,
	Mentat_Atreides_World,
	Mentat_Harkonnen_World,
	Mentat_Ordos_World,
	Mentat_Fremen,
	Mentat_Frigate,
	Mentat_Deathhand,
	Mentat_Devastator,
	Mentat_Harvester,
	Mentat_Radar,
	Mentat_HighTech,
	Mentat_SeigeTank,
	Mentat_HeavyFactory,
	Mentat_HeavyInfantry,
	Mentat_Infantry,
	Mentat_IX,
	Mentat_LightFactory,
	Mentat_CombatTank,
	Mentat_MCV,
	Mentat_Deviator,
	Mentat_Ornithopter,
	Mentat_Raider,
	Mentat_Palace,
	Mentat_Quad,
	Mentat_Refinery,
	Mentat_RepairBay,
	Mentat_RocketTank,
	Mentat_RocketTurret,
	Mentat_Saboteur,
	Mentat_Sardaukar,
	Mentat_Slab,
	Mentat_SonicTank,
	Mentat_Starport,
	Mentat_Silo,
	Mentat_Trike,
	Mentat_Turret,
	Mentat_Wall,
	Mentat_Windtrap,
	Mentat_WOR,
	Mentat_Worm,
	Mentat_Entry_Unknown
} MENTAT_ENTRIES;

typedef struct Mentat_Entry
{
	SDL_Surface *staticpic;
	SDL_Surface	**pic;
	char *text;
	char *fullText[512];//needed?
	char *basePath;

	int frames;
	int curFrame;

	int picID;
	bool loaded;
} MentatEntry;



class MentatClass
{

public:
        MentatClass();
		~MentatClass();
		void showMentat();
		bool doExit();
		void draw();
		void handlePress(int x, int y);
		void handleCancelPress(int x, int y);
		void handleRelease(int x, int y);
		void handleCancelRelease(int x, int y);
		void LoadData();
		void LoadMentatEntry(int entry, int frames, char *basePath, char *text);
		void UnLoadMentatEntry(int entry);
		void setMentatView(int entry);
		char *getFramePath(char *basePath, int frame);
		void createList();
		void checkMinMaxSelection();

		void doBriefing(int level);
		int doHouseChoice();
		bool doHouseInfo(int h);

		void makeTextSurface(int num);

		static void handleUp(void *val);
		static void handleDown(void *val);
		static void listButtonCallBack(void *val);

		static void yesCallback(void *val);
		static void noCallback(void *val);
		static void houseChoiceCallbackA(void *val);
		static void houseChoiceCallbackO(void *val);
		static void houseChoiceCallbackH(void *val);
		inline void setHouseChoiceExitValue(int b) { houseChoiceExitValue = b; }
		inline void setHouseChoiceValue(int b) { houseChoiceValue = b; }

		void handleInput();
		static void briefingButtonCallback(void *val);
		inline void setBriefingRunning(bool b) { briefingRunning = b; }
		

		void doUp();
		void doDown();

		void updateTimers();
		void setHouse(int h);

		void UnloadTemporaryData();

		SDL_Surface *loadImageFromZip(ZZIP_DIR* zzipdir, char *path);

private:
	Window *mentatWindow;
	int house;
	int currentViewID;
	int frameTimer, mouthTimer, eyesTimer;
	int eyesFrame, mouthFrame;
	int talkLength;
	int minSelection;
	int maxSelection;
	SDL_Surface *mentatSurface;
	SDL_Surface *mentatExitSurf;
	SDL_Surface *mentatExitSurfPressed;
	//SDL_Surface *textSurface;
	SDL_Surface *shoulderSurface;

	SDL_Surface *eyesFrames[5];
	SDL_Surface *mouthFrames[5];
	SDL_Rect mentatPos, graphicPos, textPos, mouthPos, eyesPos, shoulderPos;
	HBox mentatButtons;
	VBox listBox;
	Button buttonExit, buttonOK, listBt1, listBt2, listBt3, listBt4, listBt5, listBt6, listUp, listDown;

	BUILD_ITEM_DATA	data1, data2, data3, data4, data5, data6;//holds a void * and a number, perfect for what we need

	ListIterator	*selectionList;

	int loadedBefore;

	Mentat_Entry levelEntry;
	bool inBriefing;
	bool briefingRunning;

	bool	inHouseChoice;
	bool	inInfoScreen;

	int houseChoiceExitValue;
	int houseChoiceValue;

	Button houseAtre, houseOrdos, houseHark;
	HBox houseButtons;
	Window *houseChoiceWindow;
	SDL_Surface *heraldSurf;

	Button yesButton, noButton;//buttons for yes and no
	HBox yesNoButtons;
	Window *houseInfoWindow;
	Label	textLabel;

	char *zipPath;
};

#endif

