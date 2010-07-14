#ifndef _OVERLAY_H_
#define _OVERLAY_H_

struct overlayObjectListStruct {
	short int field_0;
	short int X;
	short int Y;
	short int type;
	short int followedActor;
	short int positionType;
	int timeToDie;
};

typedef struct overlayObjectListStruct overlayObjectListStruct;

extern overlayObjectListStruct overlayObjectList[10];

void addOverlayObject(int arg1, int arg2, int arg3, int arg4, int arg5, int arg6, int lifeTime);
void updateOverlayObjectsPosition(short int arg_0, short int arg_4, short int arg_8, short int arg_C);

#endif
