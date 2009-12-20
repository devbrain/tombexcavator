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

// condition resolve helpers

int getCOL(actor* ptrActor) {
	if (ptrActor->life <= 0)
		return(-1);
	else
		return(ptrActor->collision);
}

int getDISTANCE(actor* ptrActor1, actor* ptrActor2) {
	int computedDistance;

	if (!(ptrActor2->dynamicFlagsBF.bUnk0020)) {
		// clamp for major height difference
		if (abs(ptrActor2->Y - ptrActor1->Y) >= 1500) {
			computedDistance = 32000;
		} else {
			computedDistance = Distance2D(ptrActor1->X, ptrActor1->Z, ptrActor2->X, ptrActor2->Z);

			if (computedDistance > 32000) { // clamp distance to 32000
				computedDistance = 32000;
			}
		}
	} else {
		computedDistance = 32000;
	}

	return(computedDistance);
}
