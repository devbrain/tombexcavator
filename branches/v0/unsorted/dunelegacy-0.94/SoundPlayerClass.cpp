#include "sand.h"

SoundPlayerClass::SoundPlayerClass(/*SETTINGSTYPE* settings*/)
{
	sfxVolume = 50;

	musicVolume = 75;
	responseVolume = 100;
	voVolume = 128;

	Mix_Volume(-1, sfxVolume);

	Mix_VolumeMusic(musicVolume);

	music = NULL;
	thisMusicID = NONE;

	voChannel = Mix_ReserveChannels(1);//Reserve a channel for voice over

	soundOn = true;
	musicOn = true;
	if (Music_Last < Music_First)	//if we have a song to play
		musicOn = false;
	else
		changeMusic(MUSIC_INTRO);
}
SoundPlayerClass::~SoundPlayerClass()
{
}
void SoundPlayerClass::changeMusic(MUSICTYPE musicType)
{
	if (musicOn && (Music_Last >= Music_First))
	{

		int musicID = NONE;



		switch(musicType)
		{
			case MUSIC_ATTACK:
				if ((thisMusicID < Music_AttackFirst) || (thisMusicID > Music_AttackLast))
				{
					if (Music_AttackLast >= Music_AttackFirst) //only if we have more then one song
						musicID = getRandomInt(Music_AttackFirst, Music_AttackLast);
					else
						musicID = getRandomInt(Music_First, Music_Last);
				}
				break;
			case MUSIC_INTRO:
				if (Music_IntroLast >= Music_IntroFirst) //only if we have more then one song
					musicID = getRandomInt(Music_IntroFirst, Music_IntroLast);
				else
					musicID = getRandomInt(Music_First, Music_Last);
				break;
			case MUSIC_LOSE:
				if (Music_LoseLast >= Music_LoseFirst) //only if we have more then one song
					musicID = getRandomInt(Music_LoseFirst, Music_LoseLast);
				else
					musicID = getRandomInt(Music_First, Music_Last);
				break;
			case MUSIC_PEACE:
				if (Music_PeaceLast >= Music_PeaceFirst) //only if we have more then one song
					musicID = getRandomInt(Music_PeaceFirst, Music_PeaceLast);
				else
					musicID = getRandomInt(Music_First, Music_Last);
				break;
			case MUSIC_WIN:
				if (Music_WinLast >= Music_WinFirst) //only if we have more then one song
					musicID = getRandomInt(Music_WinFirst, Music_WinLast);
				else
					musicID = getRandomInt(Music_First, Music_Last);
				break;
			case MUSIC_RANDOM:default:
				if (Music_Last > Music_First) //only if we have more then one song
				{
					do		//make sure we don't play the same song in a row
						musicID = getRandomInt(Music_First, Music_Last);
					while (musicID == thisMusicID);
				}
				else
					musicID = Music_First;
				break;
		}

		if (musicID != NONE)
		{
			Mix_HaltMusic();
			DATAFILE* file = &dataFile[musicID];	//grab the music file from the datafle
			thisMusicID = musicID;		//record which midi we are playing so don't play in a row
			music = (Mix_Music*)file->dat;
			Mix_PlayMusic(music, 0);
		}
	}
}
void SoundPlayerClass::musicCheck()
{
	if (musicOn)
	{
		if ( ! Mix_PlayingMusic() )
			changeMusic(MUSIC_PEACE);
	}
}

void SoundPlayerClass::playSoundAt(int soundID, COORDTYPE* location)
{
	if (soundOn)
	{
		int	volume = sfxVolume,
			panning = 128,
			xOffset = 0,
			yOffset = 0;

		if (location->x > dborder->maxX/BLOCKSIZE)
		{
			xOffset = location->x - dborder->maxX/BLOCKSIZE;
			panning += xOffset;
		}
		else if (location->x < dborder->minX/BLOCKSIZE)
		{	
			xOffset = dborder->minX/BLOCKSIZE - location->x;
			panning -= xOffset;
		}

		if (location->y > dborder->maxY/BLOCKSIZE)
			yOffset = location->y - dborder->maxY/BLOCKSIZE;
		else if (location->y < dborder->minY/BLOCKSIZE)
			yOffset = dborder->minY/BLOCKSIZE - location->y;
		
		volume -= (int)(10*(sqrt((double)(xOffset*xOffset + yOffset*yOffset))));
		if (volume < 15) volume = 15;

		playSound(soundID, volume);
	}
}

void SoundPlayerClass::playSfxSound(int soundID)
{
	if (soundOn)
		playSound(soundID, sfxVolume);
}

void SoundPlayerClass::playResponseSound(int soundID)
{
	if (soundOn)
		playSound(soundID, responseVolume);
}


void SoundPlayerClass::playVOSoundDual(int soundID1, int soundID2)
{
	if (soundOn)
	{
		Mix_Chunk *sound1 = (Mix_Chunk*)dataFile[soundID1].dat;
		Mix_Chunk *sound2 = (Mix_Chunk*)dataFile[soundID2].dat;;

		Mix_Chunk chunk;

		chunk.allocated = sound1->allocated;
		chunk.volume = sound1->volume;
		chunk.alen = sound1->alen + sound2->alen;
		chunk.abuf = (Uint8 *)malloc(sound1->alen + sound2->alen);
		memcpy(chunk.abuf, sound1->abuf, sound1->alen);
		memcpy(chunk.abuf + sound1->alen, sound2->abuf, sound2->alen);

		int channel = Mix_PlayChannel(voChannel, &chunk, 0);
		if (channel != -1)
			Mix_Volume(channel, voVolume);
	}
}

void SoundPlayerClass::playVOSoundThree(int soundID1, int soundID2, int soundID3)
{
	if (soundOn)
	{
		Mix_Chunk *sound1 = (Mix_Chunk*)dataFile[soundID1].dat;
		Mix_Chunk *sound2 = (Mix_Chunk*)dataFile[soundID2].dat;
		Mix_Chunk *sound3 = (Mix_Chunk*)dataFile[soundID3].dat;

		Mix_Chunk chunk;

		chunk.allocated = sound1->allocated;
		chunk.volume = sound1->volume;
		chunk.alen = sound1->alen + sound2->alen + sound3->alen;
		chunk.abuf = (Uint8 *)malloc(chunk.alen);
		memcpy(chunk.abuf, sound1->abuf, sound1->alen);
		memcpy(chunk.abuf + sound1->alen, sound2->abuf, sound2->alen);
		memcpy(chunk.abuf + sound1->alen + sound2->alen, sound3->abuf, sound3->alen);

		int channel = Mix_PlayChannel(voChannel, &chunk, 0);
		if (channel != -1)
			Mix_Volume(channel, voVolume);
	}
}

void SoundPlayerClass::playVOSound(int soundID)
{
	if (soundOn)
	{
//		stop_sample(reinterpret_cast<SAMPLE*>(dataFile[VO_AConst].dat));	//vo can speak only one thinki at once
//		stop_sample(reinterpret_cast<SAMPLE*>(dataFile[soundID].dat));
		playSound(soundID, voVolume);
	}
}

void SoundPlayerClass::setMusic(bool value)
{
	musicOn = value;

	if (musicOn)
		changeMusic(MUSIC_RANDOM);
	else if (music != NULL)
		Mix_HaltMusic();
}

void SoundPlayerClass::toggleSound()
{
	if (!soundOn && !musicOn)
	{
		soundOn = true;
		showMessage("sound on, music off");
	}
	else if (soundOn && !musicOn)
	{
		musicOn = true;
		soundOn = false;
		showMessage("sound off, music on");

		changeMusic(MUSIC_PEACE);
	}
	else if (!soundOn && musicOn)
	{
		soundOn = true;
		showMessage("sound on, music on");
	}
	else if (soundOn && musicOn)
	{
		soundOn = false;
		musicOn = false;
		showMessage("sound off, music off");

		if (music != NULL)
			Mix_HaltMusic();
	}
}

void SoundPlayerClass::playSound(int soundID, int volume)
{
	if (soundOn)
	{
		int channel = Mix_PlayChannel(-1, (Mix_Chunk*)dataFile[soundID].dat, 0);
		if (channel != -1)
			Mix_Volume(channel, volume);
	}
}

