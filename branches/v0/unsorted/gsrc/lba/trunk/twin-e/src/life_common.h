#ifndef _LIFE_COMMON_H_
#define _LIFE_COMMON_H_

int getCOL(actor* ptrActor);
int getDISTANCE(actor* ptrActor1, actor* ptrActor2);

int resolveTrackLineFromLabel(actor* actorPtr, int trackPosition);
void getLine(char* script, char* buffer, int line);

#endif
