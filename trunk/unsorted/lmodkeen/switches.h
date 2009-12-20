typedef struct
{
	char InputPath[PATH_MAX];
	char OutputPath[PATH_MAX];
	int Backup;
	int Episode;
	int Export;
	int Import;
	int Extract;
} SwitchStruct;

SwitchStruct *getswitches(int argc, char *argv[]);
