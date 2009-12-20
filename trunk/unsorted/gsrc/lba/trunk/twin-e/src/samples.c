/*
Copyright (C) 2002-2004 The TwinE team

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/

#include "lba.h"

#ifdef USE_FMOPL_MIDI
#include <SDL_mixer.h>

struct sampleStructData {
	int sampleIdx;
};

struct sampleStructData sampleTable[16];

void playSample(int sampleNum, int freq, int repeat, int x, int y) {
	char filename[MAX_PATH];
	Mix_Chunk * sample;
	FILE* fhandle;

	sample = Mix_LoadWAV_RW(SDL_RWFromMem(HQR_Get(HQR_Samples,sampleNum), Size_HQR("SAMPLES.HQR", sampleNum)), 0);

	if (sample == NULL)
		printf("Mix_LoadWAV(\"%s\"): %s\n", filename, Mix_GetError());

	Mix_PlayChannel(0, sample, repeat - 1);
}

void playSampleFla(int sampleNum, int freq, int repeat, int x, int y) {
	char filename[MAX_PATH];
	Mix_Chunk * sample;

	sprintf(filename,"fla/flasamp/flasamp%02d.voc", sampleNum + 1);

	sample = Mix_LoadWAV(filename);
	Mix_PlayChannel(0, sample, repeat - 1);
}

void soundInit() {
	const int audio_buffers = 512;

	Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, audio_buffers);
	Mix_AllocateChannels(8);
}

static Mix_Music * sample = NULL;

// TODO: Use RWops
void playMidi(int musicNum) {
	char filename[MAX_PATH];
	FILE* fhandle;

	if (sample != NULL)
		Mix_FreeMusic(sample);

	if (musicNum == 9)
		return;

	sprintf(filename, DATA_DIR"midi/%02d.midi", musicNum);

	fhandle = OpenRead(filename);

	if (!fhandle) {
		hqr_entry* temp;

		Mkdir(DATA_DIR"midi");
		fhandle = OpenWrite(filename);

		temp = HQR_Get(HQR_Inventory, musicNum);
		fwrite(temp->ptr, temp->size1, 1, fhandle);
		free(temp);
	}

	Close(fhandle);

	sample = Mix_LoadMUS(filename);

	if (sample == NULL)
		printf("Mix_LoadMUS(\"%s\"): %s\n", filename, Mix_GetError());
	Mix_PlayMusic(sample, 0);
}

#endif
