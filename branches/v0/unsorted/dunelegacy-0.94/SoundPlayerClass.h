typedef enum { MUSIC_ATTACK, MUSIC_INTRO, MUSIC_LOSE, MUSIC_PEACE, MUSIC_WIN, MUSIC_RANDOM } MUSICTYPE;

class SoundPlayerClass
{
public:
	SoundPlayerClass(/*SETTINGSTYPE* settings*/);
	~SoundPlayerClass();
	void changeMusic(MUSICTYPE musicType);
	void musicCheck();
	void playSoundAt(int soundID, COORDTYPE* location);
	void playResponseSound(int soundID);
	void playSfxSound(int soundID);
	void playVOSound(int soundID);
	void playVOSoundDual(int soundID1, int soundID2);
	void playVOSoundThree(int soundID1, int soundID2, int soundID3);
	void setMusic(bool value);
	void toggleSound();

private:
	void playSound(int soundID, int volume);

	bool	soundOn,
		musicOn;

	int	responseVolume,
		sfxVolume,
		musicVolume,
		thisMusicID,
		voVolume,
		voChannel;

        Mix_Music*      music;
};

