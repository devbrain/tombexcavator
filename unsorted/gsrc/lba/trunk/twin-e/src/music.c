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

#ifdef PCLIKE
# ifdef USE_FMOPL_MIDI
#  include <SDL_mixer.h>
# endif
# include "SDL/SDL.h"
#endif

void PlayMusic(int musicNum) {
	if (musicNum == -1) {    /* stop music */
		fullStopMusic();
		currentlyPlayingCDTrack = -1;
		// playMusic2();
		playMusciVar1 = -1;
		return;
	}

	if (musicNum >= 1 && musicNum <= 9)
		playCDtrack(musicNum);
	else
		playMidi(musicNum);

	return;
}

void playCDtrack(int trackNumber) {
#ifdef PCLIKE
	freezeTime();
#ifdef USE_FMOPL_MIDI
	Mix_FadeOutMusic(500);
#endif
	currentlyPlayingMidi = -1;

	if (cdrom != NULL) {
		if (SDL_CDStatus(cdrom) == CD_PLAYING) {
			SDL_CDStop(cdrom);
		}
	}
	currentlyPlayingCDTrack = -1;

	if (cdrom != NULL) {
		if (CD_INDRIVE(SDL_CDStatus(cdrom)))
			SDL_CDPlayTracks(cdrom, trackNumber, 0, 1, 0);
	} else {
		playMidi(trackNumber);
	}

	unfreezeTime();
#endif
}

void fullStopMusic(void) {
#ifdef PCLIKE
	if (cdrom != NULL) {
//    int status = SDL_CDStatus(cdrom);
//    if(status == CD_PLAYING)
		{
			SDL_CDStop(cdrom);
		}
	}
#endif
}

void stopMusic(void) {
	fullStopMusic();
	currentlyPlayingCDTrack = -1;
}

int IsMidiPlaying(void) {
	return (0);
}

void FadeMusicMidi(short int arg_0) {
}

int getCurrentlyPlayingMusic(void) {
	if (musicPosition > musicLength)
		currentlyPlayingCDTrack = -1;

	return (currentlyPlayingCDTrack);
}
