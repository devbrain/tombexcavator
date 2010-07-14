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
#include "math.h"

#ifdef GAME_DEBUG
extern actor* pCurrentActorRender;
#endif

static const int videoWidth = 640;
static const int videoHeight = 480;

static int baseMatrixRotationX;
static int baseMatrixRotationY;
static int baseMatrixRotationZ;

int AffObjetIso(int X, int Y, int Z, int angleX, int angleY, int angleZ, unsigned char *costumePtr) {
	unsigned char *ptr;
	short int costumeHeader;

#ifdef MACOSX
//  return 0;
#endif

	tab1 = &angleTable[0];
	tab2 = &angleTable[256];
	tab3 = &angleTable[384];

	_angleX = angleX;
	_angleY = angleY;
	_angleZ = angleZ;

	/*
	 * Those 4 vars are used to know the size of the rendered model. They are resetted to their
	 * maximum value at the begining of the renderer
	 */

	renderLeft = 32767;
	renderTop = 32767;

	renderRight = -32767;
	renderBottom = -32767;

	if (isUsingOrhoProjection == 0) {
		setSomething3sub(X, Y, Z);

		_X = destX - setSomething3Var12;
		_Y = destZ - setSomething3Var14;
		_Z = destY - setSomething3Var16;
	} else {
		_X = X;
		_Y = Y;
		_Z = Z;
	}

	_numOfPrimitives = 0; // we reset the number of primitives in the model

	renderTabEntryPtr = renderTab;  // we restart at the beginning of the renderTable

	costumeHeader = READ_LE_U16(costumePtr);

	ptr = costumePtr + 16 + READ_LE_U16(costumePtr + 14); // we jump after the header

	if (costumeHeader & 2) { // if animated
		return (renderAnimatedModel(ptr));  // That's the mostly used renderer code
	} else {
		printf("Unsupported unanimated model render!\n");
		exit(1);
	}

	return (0);
}

void configureOrthoProjection(int a, int b, int c) {
	setSomethingVar1 = a;
	setSomethingVar2 = b;
	setSomethingVar3 = c;
	isUsingOrhoProjection = 1;
}


void SetLightVector(int angleX, int angleY, int angleZ) {
	tab1 = &angleTable[0];
	tab2 = &angleTable[256];
	tab3 = &angleTable[384];

	_cameraAngleX = angleX;
	_cameraAngleY = angleY;
	_cameraAngleZ = angleZ;

	_angleX = angleX;
	_angleY = angleY;
	_angleZ = angleZ;

	RotMatIndex2(_shadeMatrix, _baseMatrix);
	TranslateGroupeS2(0, 0, 59);

	_lightX = destX;
	_lightY = destZ;
	_lightZ = destY;
}

void TranslateGroupeS2(short int ax, short int bx, short int cx) {
	int ebp;
	int ebx;
	int ecx;
	int eax;
	int edi;

	ebp = ax;
	ebx = bx;
	ecx = cx;

	edi = _shadeMatrix[0];
	eax = _shadeMatrix[1];
	edi *= ebp;
	eax *= ebx;
	edi += eax;
	eax = _shadeMatrix[2];
	eax *= ecx;
	eax += edi;
	eax >>= 14;

	destX = eax;

	edi = _shadeMatrix[3];
	eax = _shadeMatrix[4];
	edi *= ebp;
	eax *= ebx;
	edi += eax;
	eax = _shadeMatrix[5];
	eax *= ecx;
	eax += edi;
	eax >>= 14;
	destZ = eax;

	ebp *= _shadeMatrix[6];
	ebx *= _shadeMatrix[7];
	ecx *= _shadeMatrix[8];
	ebx += ebp;
	ebx += ecx;
	ebx >>= 14;
	destY = eax;
}

int renderAnimatedModel(unsigned char *costumePtr) {
	pointEntry *ptEntryPtr;
	pointTab *pointPtr;
	pointTab *pointPtrDest;
	int coX;
	int coY;
	int coZ;
	unsigned char *ptr3;
	int *ptr4;
	unsigned char *ptr6;
	int temp;

	assert(frontVideoBufferbis == frontVideoBuffer);

	_numOfPoints = READ_LE_U16(costumePtr);
	costumePtr += 2;
	_pointsPtr = costumePtr;

	costumePtr += _numOfPoints * 6;

	_numOfParts = READ_LE_U16(costumePtr);
	costumePtr += 2;
	_partsPtr = _partsPtr2 = costumePtr;

	_currentMatrixTableEntry = (unsigned char *) _matrixTable;

	loadPart(_angleX, _angleY, _angleZ, (unsigned char *) _partsPtr);

	_partsPtr += 38;

	ptEntryPtr = (pointEntry *) _partsPtr;

	if (_numOfParts - 1 != 0) {
		_numOfPrimitives = _numOfParts - 1;
		_currentMatrixTableEntry = (unsigned char *) & _matrixTable[9];

		do {
			int boneType = READ_LE_U16(((unsigned char*)ptEntryPtr) + 8);
			if (boneType == 0) {
				loadPart(ptEntryPtr->rotateX, ptEntryPtr->rotateY, ptEntryPtr->rotateZ, (unsigned char *) ptEntryPtr);  // rotation
			} else
				if (boneType == 1) {
					TranslateGroupe(ptEntryPtr->rotateX, ptEntryPtr->rotateY, ptEntryPtr->rotateZ, ptEntryPtr); // translation
				}

			_currentMatrixTableEntry += 36;
			_partsPtr += 38;
			ptEntryPtr = (pointEntry *) _partsPtr;

		} while (--_numOfPrimitives);
	}

	_numOfPrimitives = _numOfPoints;

	pointPtr = (pointTab *) _projectedPointTable;
	pointPtrDest = (pointTab *) _flattenPointTable;

	if (isUsingOrhoProjection != 0) { // use standard projection
		do {
			coX = pointPtr->x + _X;
			coY = pointPtr->y + _Y;
			coZ = -(pointPtr->z + _Z);

#ifdef USE_FLOAT
			pointPtrDest->x = (coX + coZ) * 24 / 512.f + setSomethingVar1;
			pointPtrDest->y = (((coX - coZ) * 12) - coY * 30) / 512.f + setSomethingVar2;
			pointPtrDest->z = coZ - coX - coY;
#else
			pointPtrDest->x = (coX + coZ) * 24 / 512 + setSomethingVar1;
			pointPtrDest->y = (((coX - coZ) * 12) - coY * 30) / 512 + setSomethingVar2;
			pointPtrDest->z = coZ - coX - coY;
#endif

			if (pointPtrDest->x < renderLeft)
				renderLeft = pointPtrDest->x;
			if (pointPtrDest->x > renderRight)
				renderRight = pointPtrDest->x;

			if (pointPtrDest->y < renderTop)
				renderTop = pointPtrDest->y;
			if (pointPtrDest->y > renderBottom)
				renderBottom = pointPtrDest->y;

			pointPtr++;
			pointPtrDest++;
		} while (--_numOfPrimitives);
	} else {
		do {
			coX = pointPtr->x + _X;
			coY = pointPtr->y + _Y;
			coZ = -(pointPtr->z + _Z);

			coZ += cameraVar1;

			if (coZ <= 0)
				coZ = 0x7FFFFFFF;

			{
				coX = (coX * cameraVar2) / coZ + setSomethingVar1;

				if (coX > 0xFFFF)
					coX = 0x7FFF;

				pointPtrDest->x = coX;

				if (pointPtrDest->x < renderLeft)
					renderLeft = pointPtrDest->x;

				if (pointPtrDest->x > renderRight)
					renderRight = pointPtrDest->x;
			}

			{
				coY = (-coY * cameraVar3) / coZ + setSomethingVar2;

				if (coY > 0xFFFF)
					coY = 0x7FFF;

				pointPtrDest->y = coY;

				if (pointPtrDest->y < renderTop)
					renderTop = pointPtrDest->y;
				if (pointPtrDest->y > renderBottom)
					renderBottom = pointPtrDest->y;
			}

			{
				if (coZ > 0xFFFF)
					coZ = 0x7FFF;

				pointPtrDest->z = coZ;
			}

			pointPtr++;
			pointPtrDest++;

		} while (--_numOfPrimitives);
	}

	_shadePtr = (int *) _partsPtr;

	temp = READ_LE_U16(_shadePtr);

	_shadePtr = (int *)(((unsigned char *) _shadePtr) + 2);

	if (temp) {   // process normal data
		int eax;
		int edi;

		unsigned char *_currentShadeDestination = (unsigned char *) shadeTable;
		int *renderV21 = _matrixTable;
		unsigned char *pri2Ptr2;

		_numOfPrimitives = _numOfParts;

		ptr3 = pri2Ptr2 = _partsPtr2 + 18;

		assert(frontVideoBufferbis == frontVideoBuffer);

		do {
			temp = READ_LE_U16(ptr3);
			if (temp) {
				int rs1s2v1 = temp;

				_shadeMatrix[0] = (*renderV21) * _lightX;
				_shadeMatrix[1] = (*(renderV21 + 1)) * _lightX;
				_shadeMatrix[2] = (*(renderV21 + 2)) * _lightX;

				_shadeMatrix[3] = (*(renderV21 + 3)) * _lightY;
				_shadeMatrix[4] = (*(renderV21 + 4)) * _lightY;
				_shadeMatrix[5] = (*(renderV21 + 5)) * _lightY;

				_shadeMatrix[6] = (*(renderV21 + 6)) * _lightZ;
				_shadeMatrix[7] = (*(renderV21 + 7)) * _lightZ;
				_shadeMatrix[8] = (*(renderV21 + 8)) * _lightZ;

				do { // for each normal
					short int col1;
					short int col2;
					short int col3;

					short int *colPtr;

					colPtr = (short int *) _shadePtr;

					col1 = READ_LE_S16(colPtr++);
					col2 = READ_LE_S16(colPtr++);
					col3 = READ_LE_S16(colPtr++);

					eax = _shadeMatrix[0] * col1 + _shadeMatrix[1] * col2 + _shadeMatrix[2] * col3;
					eax += _shadeMatrix[3] * col1 + _shadeMatrix[4] * col2 + _shadeMatrix[5] * col3;
					eax += _shadeMatrix[6] * col1 + _shadeMatrix[7] * col2 + _shadeMatrix[8] * col3;

					edi = 0;

					if (eax > 0) {
						eax >>= 14;
						ptr6 = (unsigned char *) _shadePtr;
						eax /= READ_LE_U16(ptr6 + 6);
						edi = (unsigned short int) eax;
					}

					WRITE_LE_U16(_currentShadeDestination, edi);
					_currentShadeDestination += 2;
					_shadePtr += 2;

				} while (--rs1s2v1);
			}
			ptr3 = pri2Ptr2 = pri2Ptr2 + 38;
			ptr4 = renderV21 = renderV21 + 9;
		} while (--_numOfPrimitives);
	}
	assert(frontVideoBufferbis == frontVideoBuffer);

	return (finishRender((unsigned char *) _shadePtr));
}

void loadPart(int edx, int ecx, int ebx, unsigned char * ptr) {
	int *ebp;
	short int var;

// int* ptr1;

	int rs1v1 = READ_LE_S16(ptr);
	int rs1v2 = READ_LE_S16(ptr + 2);

	_angleX = ebx;
	_angleZ = ecx;
	_angleY = edx;

	if (rs1v1 % 6) {
		printf("Error: rs1V1\n");
		exit(1);
	}

	var = READ_LE_U16(ptr + 6);

	if (var == -1) {  // si c'est le premier point
		ebp = _baseMatrix;

		destX = 0;
		destY = 0;
		destZ = 0;
	} else {
		int pointIdx = (READ_LE_U16(ptr + 4)) / 6;
		ebp = (int *)((unsigned char *) _matrixTable + var);

		destX = _projectedPointTable[pointIdx].x;
		destY = _projectedPointTable[pointIdx].y;
		destZ = _projectedPointTable[pointIdx].z;
	}

	RotMatIndex2((int *) _currentMatrixTableEntry, ebp);  // copie dans renderTab2 + application de la rotation

	if (!rs1v2) {
		printf("No points !\n");
	}

	RotList(_pointsPtr + rs1v1, rs1v2, &_projectedPointTable[rs1v1 / 6], (int *) _currentMatrixTableEntry); // rotation des elements du model
}

void RotList(unsigned char *esi, int ecx, pointTab * dest, int *eax) {
	short int param1;
	short int param2;
	short int param3;

	short int *tempPtr;

	int rs1s2v1 = ecx;
	unsigned char *rs1s2v2;

	do {
		rs1s2v2 = esi;
		tempPtr = (short int *)(esi);

		param1 = tempPtr[0];
		param2 = tempPtr[1];
		param3 = tempPtr[2];

		dest->x = ((eax[0] * param1 + eax[1] * param2 + eax[2] * param3) >> 14) + destX;
		dest->y = ((eax[3] * param1 + eax[4] * param2 + eax[5] * param3) >> 14) + destY;
		dest->z = ((eax[6] * param1 + eax[7] * param2 + eax[8] * param3) >> 14) + destZ;

		dest++;
		esi = rs1s2v2 + 6;
	} while (--rs1s2v1);
}

void RotMatIndex2(int *eax, int *ebp) {
	int angle;
	int angleVar1;    // esi
	int angleVar2;    // ecx

	int matrix1[9];
	int matrix2[9];

	if (_angleX) {  // rotation par vers l'avant
		angle = _angleX;
		angleVar2 = tab1[angle & 0x3FF];
		angle += 0x100;
		angleVar1 = tab1[angle & 0x3FF];

		matrix1[0] = ebp[0];
		matrix1[3] = ebp[3];
		matrix1[6] = ebp[6];

		matrix1[1] = (ebp[2] * angleVar2 + ebp[1] * angleVar1) >> 14;
		matrix1[2] = (ebp[2] * angleVar1 - ebp[1] * angleVar2) >> 14;
		matrix1[4] = (ebp[5] * angleVar2 + ebp[4] * angleVar1) >> 14;
		matrix1[5] = (ebp[5] * angleVar1 - ebp[4] * angleVar2) >> 14;
		matrix1[7] = (ebp[8] * angleVar2 + ebp[7] * angleVar1) >> 14;
		matrix1[8] = (ebp[8] * angleVar1 - ebp[7] * angleVar2) >> 14;

	} else {
		matrix1[0] = ebp[0];
		matrix1[1] = ebp[1];
		matrix1[2] = ebp[2];
		matrix1[3] = ebp[3];
		matrix1[4] = ebp[4];
		matrix1[5] = ebp[5];
		matrix1[6] = ebp[6];
		matrix1[7] = ebp[7];
		matrix1[8] = ebp[8];
	}

	if (_angleY) {
		angle = _angleY;
		angleVar2 = tab1[angle & 0x3FF];
		angle += 0x100;
		angleVar1 = tab1[angle & 0x3FF];

		matrix2[2] = matrix1[2];
		matrix2[5] = matrix1[5];
		matrix2[8] = matrix1[8];

		matrix2[0] = (matrix1[1] * angleVar2 + matrix1[0] * angleVar1) >> 14;
		matrix2[1] = (matrix1[1] * angleVar1 - matrix1[0] * angleVar2) >> 14;
		matrix2[3] = (matrix1[4] * angleVar2 + matrix1[3] * angleVar1) >> 14;
		matrix2[4] = (matrix1[4] * angleVar1 - matrix1[3] * angleVar2) >> 14;
		matrix2[6] = (matrix1[7] * angleVar2 + matrix1[6] * angleVar1) >> 14;
		matrix2[7] = (matrix1[7] * angleVar1 - matrix1[6] * angleVar2) >> 14;
	} else {
		matrix2[0] = matrix1[0];
		matrix2[1] = matrix1[1];
		matrix2[2] = matrix1[2];
		matrix2[3] = matrix1[3];
		matrix2[4] = matrix1[4];
		matrix2[5] = matrix1[5];
		matrix2[6] = matrix1[6];
		matrix2[7] = matrix1[7];
		matrix2[8] = matrix1[8];
	}

	if (_angleZ) {  // rotation de coté (la plus courante)
		angle = _angleZ;
		angleVar2 = tab1[angle & 0x3FF];  // esi
		angle += 0x100;
		angleVar1 = tab1[angle & 0x3FF];  // ecx

		eax[1] = matrix2[1];
		eax[4] = matrix2[4];
		eax[7] = matrix2[7];

		eax[0] = (matrix2[0] * angleVar1 - matrix2[2] * angleVar2) >> 14;
		eax[2] = (matrix2[0] * angleVar2 + matrix2[2] * angleVar1) >> 14;
		eax[3] = (matrix2[3] * angleVar1 - matrix2[5] * angleVar2) >> 14;
		eax[5] = (matrix2[3] * angleVar2 + matrix2[5] * angleVar1) >> 14;

		eax[6] = (matrix2[6] * angleVar1 - matrix2[8] * angleVar2) >> 14;
		eax[8] = (matrix2[6] * angleVar2 + matrix2[8] * angleVar1) >> 14;
	} else {
		eax[0] = matrix2[0];
		eax[1] = matrix2[1];
		eax[2] = matrix2[2];
		eax[3] = matrix2[3];
		eax[4] = matrix2[4];
		eax[5] = matrix2[5];
		eax[6] = matrix2[6];
		eax[7] = matrix2[7];
		eax[8] = matrix2[8];
	}
}

void TranslateGroupe(int edx, int ecx, int ebx, pointEntry * esi) {
	int *dest;
	int *source;

	_angleZ = ebx;
	_angleY = ecx;
	_angleX = edx;

	if (esi->param == -1) { // base point
		destX = 0;
		destZ = 0;
		destY = 0;

		dest = (int *) _currentMatrixTableEntry;

		dest[0] = _baseMatrix[0];
		dest[1] = _baseMatrix[1];
		dest[2] = _baseMatrix[2];
		dest[3] = _baseMatrix[3];
		dest[4] = _baseMatrix[4];
		dest[5] = _baseMatrix[5];
		dest[6] = _baseMatrix[6];
		dest[7] = _baseMatrix[7];
		dest[8] = _baseMatrix[8];
	} else {   // dependent
		destX = _projectedPointTable[(esi->data3) / 6].x; // todo:
		destZ = _projectedPointTable[(esi->data3) / 6].z; // inverse because pointab and dest entry are not in the same order....
		destY = _projectedPointTable[(esi->data3) / 6].y;

		source = (int *)((unsigned char *) _matrixTable + esi->param);
		dest = (int *) _currentMatrixTableEntry;

		dest[0] = source[0];
		dest[1] = source[1];
		dest[2] = source[2];
		dest[3] = source[3];
		dest[4] = source[4];
		dest[5] = source[5];
		dest[6] = source[6];
		dest[7] = source[7];
		dest[8] = source[8];
	}

	TransRotList(_pointsPtr + esi->data1, esi->data2, &_projectedPointTable[esi->data1 / 6], (int *) _currentMatrixTableEntry);
}

void TransRotList(unsigned char *esi, int ecx, pointTab * dest, int *eax) {
//  int i;
	short int param1;
	short int param2;
	short int param3;
	short int *tempPtr;

	int rs1s2v1 = ecx;
	unsigned char *rs1s2v2;

	do {
		rs1s2v2 = esi;

		tempPtr = (short int *)(esi);

		param1 = tempPtr[0] + _angleZ;
		param2 = tempPtr[1] + _angleY;
		param3 = tempPtr[2] + _angleX;

		dest->x = ((eax[0] * param1 + eax[1] * param2 + eax[2] * param3) >> 14) + destX;
		dest->y = ((eax[3] * param1 + eax[4] * param2 + eax[5] * param3) >> 14) + destY;
		dest->z = ((eax[6] * param1 + eax[7] * param2 + eax[8] * param3) >> 14) + destZ;

		dest++;
		esi = rs1s2v2 + 6;
	} while (--rs1s2v1);
}

struct polyHeader {
	unsigned char FillVertic_AType;
	unsigned char numOfVertex;
	short int colorIndex;
};

typedef struct polyHeader polyHeader;

struct polyVertexHeader {
	short int shadeEntry;
	short int dataOffset;
};

typedef struct polyVertexHeader polyVertexHeader;


struct computedVertex {
	short int shadeValue;
	short int x;
	short int y;
};

typedef struct computedVertex computedVertex;

void circle_fill(int x, int y, int radius, byte color) {
	int currentLine;

	radius += 1;

	for (currentLine = -radius; currentLine <= radius; currentLine++) {
		double width;

		if (abs(currentLine) != radius) {
			width = sin(acos((float)currentLine / (float)radius));
		} else {
			width = 0;
		}

		width *= radius;

		if (width < 0)
			width = - width;

		drawLine(x - width, currentLine + y, x + width, currentLine + y, color);
	}
}

int finishRender(unsigned char *esi) {
	unsigned char *edi;
	short int temp;
	int eax, ecx;

	// unsigned char temp2;
	short int counter;
	short int type;
	short int color;

	// float tempFloat;
	// short int ebx;
	// short int edx;
	lineData *lineDataPtr;
	lineCoordinates *lineCoordinatesPtr;

	int point1;

	int point2;

	int depth;
	int bestDepth;
	int currentDepth;
	short int bestZ;
	int j;
	int bestPoly = 0;
	short int shadeEntry;
	short int shadeValue;

	short int ax, bx, cx;

	// short int borrom;

	unsigned char *destPtr;
	int i;

	unsigned char *render23;
	unsigned char *render24;
	int render25;

	polyVertexHeader *currentPolyVertex;
	polyHeader *currentPolyHeader;
	polyHeader *destinationHeader;
	computedVertex *currentComputedVertex;
	pointTab *currentVertex;
	pointTab *destinationVertex;

	edi = renderTab7;   // renderTab7 c'est le buffer de coordonnées
	temp = READ_LE_S16(esi);  // we read the number of polygones
	esi += 2;

	assert(frontVideoBufferbis == frontVideoBuffer);

	if (temp) {   // if there is polygones
		primitiveCounter = temp;  // the number of primitives = the number of polygones

		do {    // loop that load all the polygones
			render23 = edi;
			currentPolyHeader = (polyHeader *) esi;
			ecx = READ_LE_S32(esi);
			esi += 2;
			FillVertic_AType = currentPolyHeader->FillVertic_AType;

			assert(FillVertic_AType <= 10);

			if (FillVertic_AType >= 9) {
				destinationHeader = (polyHeader *) edi;

				destinationHeader->FillVertic_AType = currentPolyHeader->FillVertic_AType - 2;
				destinationHeader->numOfVertex = currentPolyHeader->numOfVertex;
				destinationHeader->colorIndex = currentPolyHeader->colorIndex;

				esi += 2;
				edi += 4;

				counter = destinationHeader->numOfVertex;

				bestDepth = -32000;
				renderV19 = edi;
				osystem_startPoly();

				do {
					currentPolyVertex = (polyVertexHeader *) esi;

					shadeValue = currentPolyHeader->colorIndex + shadeTable[currentPolyVertex->shadeEntry];

					currentComputedVertex = (computedVertex *) edi;

					currentComputedVertex->shadeValue = shadeValue;

					currentVertex = &_flattenPointTable[currentPolyVertex->dataOffset / 6];
					destinationVertex = (pointTab *)(edi + 2);

					destinationVertex->x = currentVertex->x;
					destinationVertex->y = currentVertex->y;

					edi += 6;
					esi += 4;

					currentDepth = currentVertex->z;

					osystem_addPointColor(destinationVertex->x, destinationVertex->y, currentDepth, (unsigned char)shadeValue);

					if (currentDepth > bestDepth)
						bestDepth = currentDepth;
				} while (--counter);

				osystem_stopPoly();
			} else if (FillVertic_AType >= 7) { // only 1 shade value is used
				destinationHeader = (polyHeader *) edi;

				destinationHeader->FillVertic_AType = currentPolyHeader->FillVertic_AType - 7;
				destinationHeader->numOfVertex = currentPolyHeader->numOfVertex;

				color = currentPolyHeader->colorIndex;

				shadeEntry = READ_LE_S16(esi + 2);

				esi += 4;

				WRITE_LE_S16(edi + 2, color + shadeTable[shadeEntry]);

				edi += 4;
				renderV19 = edi;
				bestDepth = -32000;
				counter = destinationHeader->numOfVertex;

				osystem_startPoly();

				do {
					eax = READ_LE_S16(esi);
					esi += 2;

					currentVertex = &_flattenPointTable[eax / 6];

					destinationVertex = (pointTab *)(edi + 2);

					destinationVertex->x = currentVertex->x;
					destinationVertex->y = currentVertex->y;

					edi += 6;

					currentDepth = currentVertex->z;

					osystem_addPointColor(destinationVertex->x, destinationVertex->y, currentDepth, color + shadeTable[shadeEntry]);

					if (currentDepth > bestDepth)
						bestDepth = currentDepth;
				} while (--counter);

				osystem_stopPoly();
			} else { // no shade is used
				destinationHeader = (polyHeader *) edi;

				destinationHeader->FillVertic_AType = currentPolyHeader->FillVertic_AType;
				destinationHeader->numOfVertex = currentPolyHeader->numOfVertex;
				destinationHeader->colorIndex = currentPolyHeader->colorIndex;

				esi += 2;
				edi += 4;

				bestDepth = -32000;
				renderV19 = edi;
				eax = 0;
				counter = currentPolyHeader->numOfVertex;

				osystem_startPoly();
				do {
					eax = READ_LE_S16(esi);
					esi += 2;

					currentVertex = &_flattenPointTable[eax / 6];

					destinationVertex = (pointTab *)(edi + 2);

					destinationVertex->x = currentVertex->x;
					destinationVertex->y = currentVertex->y;

					edi += 6;

					currentDepth = currentVertex->z;

					osystem_addPointColor(destinationVertex->x, destinationVertex->y, currentDepth, (unsigned char)destinationHeader->colorIndex);

					if (currentDepth > bestDepth)
						bestDepth = currentDepth;
				} while (--(counter));
				osystem_stopPoly();
			}

			render24 = edi;
			edi = renderV19;

			render25 = bestDepth;

			ax = READ_LE_S16(edi + 4);
			bx = READ_LE_S16(edi + 8);

			ax -= READ_LE_S16(edi + 16);
			bx -= READ_LE_S16(edi + 2);

			ax *= bx;

			bestDepth = ax;
			bx = currentDepth;

			ax = READ_LE_S16(edi + 2);
			cx = READ_LE_S16(edi + 10);

			ax -= READ_LE_S16(edi + 14);
			cx -= READ_LE_S16(edi + 4);

			ax *= cx;

			ax -= bestDepth;
			currentDepth -= (bx) - 1; // peut-etre une erreur la

			if (currentDepth < 0) {
				edi = render23;
			} else {
				_numOfPrimitives++;

				renderTabEntryPtr->depth = render25;
				renderTabEntryPtr->renderType = 1;
				renderTabEntryPtr->dataPtr = render23;
				renderTabEntryPtr++;

				edi = render24;
			}
		} while (--primitiveCounter);
	}

	temp = READ_LE_S16(esi);
	esi += 2;
	if (temp) {   // pour les lignes (0)
		_numOfPrimitives += temp;
		do {
			int param;
			lineDataPtr = (lineData *) esi;
			lineCoordinatesPtr = (lineCoordinates *) edi;

			if (READ_LE_S16(&lineDataPtr->p1) % 6 != 0 || READ_LE_S16(&lineDataPtr->p2) % 6 != 0) {
				printf("lineDataPtr reference is malformed !\n");
				exit(1);
			}

			point1 = READ_LE_S16(&lineDataPtr->p1) / 6;
			point2 = READ_LE_S16(&lineDataPtr->p2) / 6;
			param = READ_LE_S32(&lineDataPtr->data);
			WRITE_LE_S32(&lineCoordinatesPtr->data, param);
			WRITE_LE_S16(&lineCoordinatesPtr->x1, _flattenPointTable[point1].x);
			WRITE_LE_S16(&lineCoordinatesPtr->y1, _flattenPointTable[point1].y);
			WRITE_LE_S16(&lineCoordinatesPtr->x2, _flattenPointTable[point2].x);
			WRITE_LE_S16(&lineCoordinatesPtr->y2, _flattenPointTable[point2].y);
			bestDepth = _flattenPointTable[point1].z;
			depth = _flattenPointTable[point2].z;

			osystem_addLine(_flattenPointTable[point1].x, _flattenPointTable[point1].y, _flattenPointTable[point1].z, _flattenPointTable[point2].x, _flattenPointTable[point2].y, _flattenPointTable[point2].z, (param & 0xFF00) >> 8);

			if (depth >= bestDepth)
				bestDepth = depth;

			renderTabEntryPtr->depth = bestDepth;
			renderTabEntryPtr->renderType = 0;
			renderTabEntryPtr->dataPtr = edi;
			renderTabEntryPtr++;

			esi += 8;
			edi += 12;
		} while (--temp);
	}

	temp = READ_LE_S16(esi);
	esi += 2;
	if (temp) {   // pour les cercles (2)
		_numOfPrimitives += temp;
		do {
			unsigned char color = *(esi + 1);
			short int center = READ_LE_U16(esi + 6);
			short int size = READ_LE_U16(esi + 4);

			osystem_addSphere(_flattenPointTable[center/6].x, _flattenPointTable[center/6].y, _flattenPointTable[center/6].z, size, color);

			*(unsigned char*)edi = color;
			WRITE_LE_S16(edi + 1, _flattenPointTable[center/6].x);
			WRITE_LE_S16(edi + 3, _flattenPointTable[center/6].y);
			WRITE_LE_S16(edi + 5, size);

			renderTabEntryPtr->depth = _flattenPointTable[center/6].z;
			renderTabEntryPtr->renderType = 2;
			renderTabEntryPtr->dataPtr = edi;
			renderTabEntryPtr++;

			esi += 8;
			edi += 7;
		} while (--temp);
	}

#ifdef USE_GL
	return(0);
#endif

	renderTabEntryPtr2 = renderTab;

	assert(frontVideoBufferbis == frontVideoBuffer);

	renderTabSortedPtr = renderTabSorted;
	for (i = 0; i < _numOfPrimitives; i++) { // then we sort the polygones (FIXME: very slow !)
		renderTabEntryPtr2 = renderTab;
		bestZ = -0x7FFF;
		for (j = 0; j < _numOfPrimitives; j++) {
			if (renderTabEntryPtr2->depth > bestZ) {
				bestZ = renderTabEntryPtr2->depth;
				bestPoly = j;
			}
			renderTabEntryPtr2++;
		}
		renderTabSortedPtr->depth = renderTab[bestPoly].depth;
		renderTabSortedPtr->renderType = renderTab[bestPoly].renderType;
		renderTabSortedPtr->dataPtr = renderTab[bestPoly].dataPtr;
		renderTabSortedPtr++;
		renderTab[bestPoly].depth = -0x7FFF;
	}
	renderTabEntryPtr2 = renderTabSorted;

	/*    _numOfPrimitives = 1;
	    renderTabEntryPtr2++;*/

	assert(frontVideoBufferbis == frontVideoBuffer);

	if (_numOfPrimitives) {
		primitiveCounter = _numOfPrimitives;
		renderV19 = esi;

		do {
			type = renderTabEntryPtr2->renderType;
			esi = renderTabEntryPtr2->dataPtr;
			renderV19 += 8;

			switch (type) {
			case 0: { // draw a line
					unsigned int x1;
					unsigned int y1;
					unsigned int x2;
					unsigned int y2;

#ifndef PCLIKE
					break;
#endif
					lineCoordinatesPtr = (lineCoordinates *) esi;
					color = (READ_LE_S32(&lineCoordinatesPtr->data) & 0xFF00) >> 8;

					x1 = READ_LE_S16((unsigned short int*) & lineCoordinatesPtr->x1);
					y1 = READ_LE_S16((unsigned short int*) & lineCoordinatesPtr->y1);
					x2 = READ_LE_S16((unsigned short int*) & lineCoordinatesPtr->x2);
					y2 = READ_LE_S16((unsigned short int*) & lineCoordinatesPtr->y2);

					assert(frontVideoBufferbis == frontVideoBuffer);

					drawLine(x1, y1, x2, y2, color);

					assert(frontVideoBufferbis == frontVideoBuffer);
					break;
				}
#ifndef MACOSX
			case 1: { // draw a polygon
					eax = READ_LE_S32(esi);
					esi += 4;

					FillVertic_AType = eax & 0xFF;
					numOfVertex = (eax & 0xFF00) >> 8;
					color = (eax & 0xFF0000) >> 16;

					assert(FillVertic_AType <= 10);

					destPtr = (unsigned char *) vertexCoordinates;

					for (i = 0; i < (numOfVertex * 3); i++) {
						WRITE_LE_S16(destPtr, READ_LE_S16(esi));
						destPtr += 2;
						esi += 2;
					}

					assert(frontVideoBufferbis == frontVideoBuffer);

					if (ComputePoly_A() != 2) {
						assert(frontVideoBufferbis == frontVideoBuffer);
						FillVertic_A(FillVertic_AType, color);
					}

					assert(frontVideoBufferbis == frontVideoBuffer);

					break;
				}
#endif
			case 2: { // draw a circle
					//  int circleSize;
					//  char circleColor;

					int circleParam1;
					int circleParam2;
					int circleParam3;
					int circleParam4;
					int circleParam5;

					eax = *(int*) esi;

					circleParam1 = *(unsigned char*)esi;
					circleParam4 = READ_LE_S16(esi + 1);
					circleParam5 = READ_LE_S16(esi + 3);
					circleParam3 = READ_LE_S16(esi + 5);

					/* circleParam1 = eax & 0xFF;
					 circleParam2 = (eax & 0xFFFF00) >> 8;
					 esi += 4;
					 eax = *(int*) esi;
					 circleParam3 = eax & 0xFFFF;
					 circleParam4 = (eax & 0xFFFF0000) >> 16;
					 esi += 4;
					 circleParam5 = *(short int*)esi;
					 esi += 2; */

					if (!isUsingOrhoProjection) {
						circleParam3 = (circleParam3 * cameraVar2) / (cameraVar1 + *(short int*)esi);
					} else {
						circleParam3 = (circleParam3 * 34) >> 9;
					}

					circleParam3 += 3;

					if (circleParam4 + circleParam3 > renderRight)
						renderRight = circleParam4 + circleParam3;

					if (circleParam4 - circleParam3 < renderLeft)
						renderLeft = circleParam4 - circleParam3;

					if (circleParam5 + circleParam3 > renderBottom)
						renderBottom = circleParam5 + circleParam3;

					if (circleParam5 - circleParam3 < renderTop)
						renderTop = circleParam5 - circleParam3;

					circleParam3 -= 3;

					assert(frontVideoBufferbis == frontVideoBuffer);
					circle_fill(circleParam4, circleParam5, circleParam3, circleParam1);
					assert(frontVideoBufferbis == frontVideoBuffer);
					/*prepareCircle(circleParam3*/

				}
			default: {
					break;
				}
			}

			esi = renderV19;
			renderTabEntryPtr2++;
		} while (--primitiveCounter);
	} else {
		renderRight = -1;
		renderBottom = -1;
		renderLeft = -1;
		renderTop = -1;
		return (-1);
	}

	return (0);
}

void FillVertic_A(int ecx, int edi) {
	unsigned char *out, *out2;
	short int *ptr1;
	short int *ptr2;
	int vsize, hsize;
	int color;
	int j;
	int currentLine;

// char borrow;
	short int start, stop;

	float varf2;
	float varf3;
	float varf4;

	/* if (vtop <= 0 || vbottom <= 0)
	   return;
	 if (vleft <= 0 || vright <= 0)
	   return;
	 if (vleft >= videoWidth)
	   return;
	 // if(vright>=videoWidth)
	   // return;
	 if (vtop >= videoHeight || vbottom >= videoHeight)
	   return;*/

	if (vtop < 0) {
		return;
	}

	if (vbottom >= videoHeight - 1) {
		return;
	}

	assert(frontVideoBufferbis == frontVideoBuffer);

	out = frontVideoBuffer + videoWidth * vtop;

	ptr1 = &polyTab[vtop];
	ptr2 = &polyTab2[vtop];

	vsize = vbottom - vtop;
	vsize++;

	color = edi;

	// osystem_Flip(frontVideoBuffer);

	switch (ecx) {
	case 0: { // flat polygon
			currentLine = vtop;
			do {
				if (currentLine >= 0 && currentLine < videoHeight) {
					stop = ptr1[videoHeight];
					start = ptr1[0];

					ptr1++;
					hsize = stop - start;

					if (hsize >= 0) {
						hsize++;
						out2 = start + out;

						for (j = start; j < hsize + start; j++) {
							assert(out + j < frontVideoBuffer + videoWidth * videoHeight);
							if (j >= 0 && j < videoWidth)
								out[j] = color;
						}
					}
				}
				out += videoWidth;
				currentLine++;
			} while (--vsize);
			break;

		}
	case 1: {   // copper
			currentLine = vtop;
			do {
				if (currentLine >= 0 && currentLine < videoHeight) {
					start = ptr1[0];
					stop = ptr1[videoHeight];

					ptr1++;
					hsize = stop - start;

					if (hsize >= 0) {
						unsigned short int mask = 0x43DB;
						unsigned short int dx;
						int startCopy;

						dx = (unsigned char)color;
						dx |= 0x300;

						hsize++;
						out2 = start + out;
						startCopy = start;

						for (j = startCopy; j < hsize + startCopy; j++) {
							start += mask;
							start = (start & 0xFF00) | ((start & 0xFF) & (unsigned char)(dx >> 8)) ;
							start = (start & 0xFF00) | ((start & 0xFF) + (dx & 0xFF));
							if (j >= 0 && j < videoWidth) {
								assert(out + j < frontVideoBuffer + videoWidth * videoHeight);
								out[j] = start & 0xFF;
							}
							mask = (mask << 2) | (mask >> 14);
							mask++;
						}
					}

				}
				out += videoWidth;
				currentLine++;
			} while (--vsize);
			break;
		}
	case 2: { // bopper ? (1 pixel sur 2) // pas implementé comme à l'origine // BUGGYYYY !
			currentLine = vtop;
			do {
				if (currentLine >= 0 && currentLine < videoHeight) {
					start = ptr1[0];
					stop = ptr1[videoHeight];
					ptr1++;
					hsize = stop - start;

					if (hsize >= 0) {
						hsize++;
						out2 = start + out;
						for (j = start; j < hsize + start; j++) {
							if ((start + (vtop % 1))&1) {
								if (j >= 0 && j < videoWidth) {
									assert(out + j < frontVideoBuffer + videoWidth * videoHeight);
									out[j] = color;
								}
							}
							out2++;
						}
					}

				}
				out += videoWidth;
				currentLine++;
			} while (--vsize);
			break;
		}
	case 6: { // trame (buggé)
//      unsigned char bl=color;
			unsigned char bh = 0;

			currentLine = vtop;
			do {
				if (currentLine >= 0 && currentLine < videoHeight) {
					start = ptr1[0];
					stop = ptr1[videoHeight];
					ptr1++;
					hsize = stop - start;

					if (hsize >= 0) {
						hsize++;
						out2 = start + out;

						hsize /= 2;
						if (hsize > 1) {
							unsigned short int ax;
							bh ^= 1;
							ax = (unsigned short int)*(int*) out2;
							ax &= 1;
							if (ax ^ bh) {
								assert(out2 < frontVideoBuffer + videoWidth * videoHeight);
								out2++;
							}

							for (j = 0; j < hsize; j++) {
								assert(out2 < frontVideoBuffer + videoWidth * videoHeight);
								*(out2) = (unsigned char)color;
								out2 += 2;
							}
						}
					}

				}
				out += videoWidth;
				currentLine++;
			} while (--vsize);
			break;
		}
	case 7: { // gouraud
			renderLoop = vsize;
			currentLine = vtop;
			do {
				if (currentLine >= 0 && currentLine < videoHeight) {
					unsigned short int startColor = ptr2[0];
					unsigned short int stopColor = ptr2[videoHeight];

					short int colorSize = stopColor - startColor;

					stop = ptr1[videoHeight];  // stop
					start = ptr1[0]; // start

					ptr1++;
					out2 = start + out;
					hsize = stop - start;

					varf2 = ptr2[videoHeight];
					varf3 = ptr2[0];

					ptr2++;

					varf4 = (float)((int)varf2 - (int)varf3);

					if (hsize == 0) {
						assert(out2 < frontVideoBuffer + videoWidth * videoHeight);
						if (start >= 0 && start < videoWidth)
							*out2 = ((startColor + stopColor) / 2) >> 8; // moyenne des 2 couleurs
					} else if (hsize > 0) {
						if (hsize == 1) {
							assert(out2 + 1 < frontVideoBuffer + videoWidth * videoHeight);
							if (start >= -1 && start < videoWidth - 1)
								*(out2 + 1) = stopColor >> 8;

							assert(out2 < frontVideoBuffer + videoWidth * videoHeight);
							if (start >= 0 && start < videoWidth)
								*(out2) = startColor >> 8;
						} else if (hsize == 2) {
							assert(out2 + 2 < frontVideoBuffer + videoWidth * videoHeight);
							if (start >= -2 && start < videoWidth - 2)
								*(out2 + 2) = stopColor >> 8;

							assert(out2 + 1 < frontVideoBuffer + videoWidth * videoHeight);
							if (start >= -1 && start < videoWidth - 1)
								*(out2 + 1) = ((startColor + stopColor) / 2) >> 8;

							assert(out2 < frontVideoBuffer + videoWidth * videoHeight);
							if (start >= 0 && start < videoWidth)
								*(out2) = startColor >> 8;
						} else {
							int currentXPos = start;
							colorSize /= hsize;
							hsize++;

							if (hsize % 2) {
								hsize /= 2;
								assert(out2 < frontVideoBuffer + videoWidth * videoHeight);
								if (currentXPos >= 0 && currentXPos < videoWidth)
									*(out2) = startColor >> 8;
								out2++;
								currentXPos++;
								startColor += colorSize;
							} else {
								hsize /= 2;
							}

							do {
								assert(out2 < frontVideoBuffer + videoWidth * videoHeight);
								if (currentXPos >= 0 && currentXPos < videoWidth)
									*(out2) = startColor >> 8;

								currentXPos++;
								startColor += colorSize;

								assert(out2 + 1 < frontVideoBuffer + videoWidth * videoHeight);
								if (currentXPos >= 0 && currentXPos < videoWidth)
									*(out2 + 1) = startColor >> 8;

								currentXPos++;
								out2 += 2;
								startColor += colorSize;
							} while (--hsize);
						}
					}
				}
				out += videoWidth;
				currentLine++;
			} while (--renderLoop);

			break;
		}
	case 8: { // dithering
			renderLoop = vsize;

			currentLine = vtop;
			do {
				if (currentLine >= 0 && currentLine < videoHeight) {
					stop = ptr1[videoHeight]; // stop
					start = ptr1[0];  // start
					ptr1++;
					hsize = stop - start;

					if (hsize >= 0) {
						unsigned short int startColor = ptr2[0];
						unsigned short int stopColor = ptr2[videoHeight];
						int currentXPos = start;

						out2 = start + out;
						ptr2++;

						if (hsize == 0) {
							assert(out2 < frontVideoBuffer + videoWidth * videoHeight);
							if (currentXPos >= 0 && currentXPos < videoWidth)
								*(out2) = (unsigned char)(((startColor + stopColor) / 2) >> 8);

							assert(frontVideoBufferbis == frontVideoBuffer);
						} else {
							short int colorSize = stopColor - startColor;
							if (hsize == 1) {
								unsigned short int currentColor = startColor;
								hsize++;
								hsize /= 2;

								currentColor &= 0xFF;
								currentColor += startColor;
								assert(out2 < frontVideoBuffer + videoWidth * videoHeight);
								if (currentXPos >= 0 && currentXPos < videoWidth)
									*(out2) = currentColor >> 8;

								assert(frontVideoBufferbis == frontVideoBuffer);
								currentColor &= 0xFF;
								startColor += colorSize;
								currentColor = ((currentColor & (0xFF00)) | ((((currentColor & 0xFF) << (hsize & 0xFF))) & 0xFF));
								currentColor += startColor;
								currentXPos++;
								assert(out2 + 1 < frontVideoBuffer + videoWidth * videoHeight);
								if (currentXPos >= 0 && currentXPos < videoWidth)
									*(out2 + 1) = currentColor >> 8;

								assert(frontVideoBufferbis == frontVideoBuffer);
							} else if (hsize == 2) {
								unsigned short int currentColor = startColor;
								hsize++;
								hsize /= 2;

								currentColor &= 0xFF;
								colorSize /= 2;
								currentColor = ((currentColor & (0xFF00)) | ((((currentColor & 0xFF) << (hsize & 0xFF))) & 0xFF));
								currentColor += startColor;
								assert(out2 < frontVideoBuffer + videoWidth * videoHeight);
								if (currentXPos >= 0 && currentXPos < videoWidth)
									*(out2) = currentColor >> 8;

								assert(frontVideoBufferbis == frontVideoBuffer);

								out2++;
								currentXPos++;
								startColor += colorSize;

								currentColor &= 0xFF;
								currentColor += startColor;

								assert(out2 < frontVideoBuffer + videoWidth * videoHeight);
								if (currentXPos >= 0 && currentXPos < videoWidth)
									*(out2) = currentColor >> 8;

								assert(frontVideoBufferbis == frontVideoBuffer);

								currentColor &= 0xFF;
								startColor += colorSize;
								currentColor = ((currentColor & (0xFF00)) | ((((currentColor & 0xFF) << (hsize & 0xFF))) & 0xFF));
								currentColor += startColor;

								currentXPos++;
								assert(out2 + 1 < frontVideoBuffer + videoWidth * videoHeight);
								if (currentXPos >= 0 && currentXPos < videoWidth)
									*(out2 + 1) = currentColor >> 8;

								assert(frontVideoBufferbis == frontVideoBuffer);
							} else {
								unsigned short int currentColor = startColor;
								colorSize /= hsize;
								hsize++;


								if (hsize % 2) {
									hsize /= 2;
									currentColor &= 0xFF;
									currentColor = ((currentColor & (0xFF00)) | ((((currentColor & 0xFF) << (hsize & 0xFF))) & 0xFF));
									currentColor += startColor;
									assert(out2 < frontVideoBuffer + videoWidth * videoHeight);
									if (currentXPos >= 0 && currentXPos < videoWidth)
										*(out2) = currentColor >> 8;

									assert(frontVideoBufferbis == frontVideoBuffer);
									out2++;
									currentXPos++;
								} else {
									hsize /= 2;
								}

								do {
									currentColor &= 0xFF;
									currentColor += startColor;
									assert(out2 < frontVideoBuffer + videoWidth * videoHeight);
									if (currentXPos >= 0 && currentXPos < videoWidth)
										*(out2) = currentColor >> 8;

									assert(frontVideoBufferbis == frontVideoBuffer);
									currentXPos++;
									currentColor &= 0xFF;
									startColor += colorSize;
									currentColor = ((currentColor & (0xFF00)) | ((((currentColor & 0xFF) << (hsize & 0xFF))) & 0xFF));
									currentColor += startColor;
									assert(out2 + 1 < frontVideoBuffer + videoWidth * videoHeight);
									if (currentXPos >= 0 && currentXPos < videoWidth)
										*(out2 + 1) = currentColor >> 8;

									assert(frontVideoBufferbis == frontVideoBuffer);
									currentXPos++;
									out2 += 2;
									startColor += colorSize;
								} while (--hsize);
							}
						}
					}
				}
				out += videoWidth;
				currentLine++;
			} while (--renderLoop);
			assert(frontVideoBufferbis == frontVideoBuffer);
			break;
		}
	default: {
#ifdef GAME_DEBUG
			printf("Unsuported render type %d\n", FillVertic_AType);
#endif
			break;
		}
	};

	assert(frontVideoBufferbis == frontVideoBuffer);
}

int ComputePoly_A(void) {
	short int vertexX, vertexY;
	short int *ptr1, *ptr3;
	int i;
	short int push1, push2;
	char direction = 1;
	int echange;
	short int oldVertexX, oldVertexY;
	short int currentVertexX, currentVertexY;
	short int size;
	int temp2, temp4, temp5, temp6;
	int step;
	float vfloat, vfloat2;

	pRenderV1 = vertexCoordinates;
	pRenderV2 = pRenderV3;
	numOfVertexRemaining = numOfVertex;
	polyCropped = 0;

	vleft = vtop = 32767;
	vright = vbottom = -32768;

	ptr1 = vertexCoordinates;

	for (i = 0; i < numOfVertex; i++) {
		ptr1++;   // discarding the 1st parameter

		vertexX = *(ptr1++);

		if (vertexX < vleft)
			vleft = vertexX;
		if (vertexX > vright)
			vright = vertexX;

		vertexY = *(ptr1++);

		if (vertexY < vtop)
			vtop = vertexY;
		if (vertexY > vbottom)
			vbottom = vertexY;
	}

	ptr1[0] = pRenderV1[0];
	ptr1[1] = pRenderV1[1];
	ptr1[2] = pRenderV1[2];
	/*
	  if(vleft<0)
	    return 2;
	  if(vright>=videoWidth)
	    return 2;
	  if(vtop<0)
	    return 2;
	  if(vbottom>=videoHeight)
	    return 2;
	*/
	ptr1 = pRenderV1;   // on retourne au debut de la liste

	vertexParam1 = vertexParam2 = (*(ptr1++)) & 0xFF;
	oldVertexX = *(ptr1++);
	oldVertexY = *(ptr1++);

	do {
		oldVertexParam = vertexParam1;

		vertexParam1 = vertexParam2 = (*(ptr1++)) & 0xFF;
		currentVertexX = *(ptr1++);
		currentVertexY = *(ptr1++);

		// drawLine(oldVertexX,oldVertexY,currentVertexX,currentVertexY,255);

		if (currentVertexY == oldVertexY) { // since it's scanline based, we don't care when we are only moving along X
			oldVertexX = size = currentVertexX;
		} else {
			push1 = currentVertexX; // let's save the current coordinates since we are going to modify the values
			push2 = currentVertexY;

			if (currentVertexY < oldVertexY) { // if we are going up
				size = oldVertexY - currentVertexY;
				direction = -1;

				if (oldVertexX < currentVertexX) { // if we are going up right
					echange = oldVertexX; // we invert the vertex to draw from new to old
					oldVertexX = currentVertexX;
					currentVertexX = echange;

					echange = currentVertexY;
					currentVertexY = oldVertexY;

					oldVertexY = echange;

					echange = oldVertexParam;
					oldVertexParam = vertexParam2;
					vertexParam2 = echange;

					direction = 1;  // we will draw by going down the tab
				}

				temp2 = oldVertexY; // temp2 is the starting Y position
				oldVertexY = size;  // oldVertexY now become the number of pixel
				size = temp2 * 2;

				ptr3 = &polyTab[temp2 + videoHeight]; // ptr3 is the output ptr in the renderTab

				temp4 = ((oldVertexX - currentVertexX) << 16);  // temp4 = size in X << 16

				temp5 = temp4 / oldVertexY; // temp5 is the size of a step << 16
				temp6 = temp4 % oldVertexY; // temp6 is the remaining << 16

				vfloat = ((float)(oldVertexX - currentVertexX)) / ((float) oldVertexY);

				temp6 >>= 1;
				temp6 += 0x7FFF;

				step = (unsigned short) temp5;  // retrieve the size of a step

				// temp7 = (((unsigned short)temp6) | ((oldVertexX & 0xFFFF)<<16));
				vfloat2 = oldVertexX;

				oldVertexX = oldVertexY;  // oldVertexX is now the number of vertical pixels

				oldVertexY += 2;

				for (i = 0; i < oldVertexY; i++) {
					// *(ptr3)=((temp7&0xFFFF0000)>>16);
					if ((ptr3 - polyTab) < 960)
						*(ptr3) = (short int) vfloat2;
					ptr3 += direction;
					// temp7+=step;
					vfloat2 -= vfloat;
				}

				if (FillVertic_AType >= 7) { // we must compute the color progression
					short int* ptr3 = &polyTab2[temp2 + videoHeight];

					temp4 = (vertexParam2 - oldVertexParam); // compute the color difference

					if (temp4 >= 0) {
						union {
							struct {
								unsigned char al;
								unsigned char ah;
							} bit;
							unsigned short int temp;
						}test;

						union {
							struct {
								unsigned char al;
								unsigned char ah;
							} bit;
							unsigned short int temp;
						}reste;

						test.bit.al = oldVertexParam;
						test.bit.ah = vertexParam2;

						test.bit.ah -= test.bit.al;

						test.bit.al = 0;

						reste.temp = test.temp % oldVertexX;

						test.temp /= oldVertexX;

						reste.bit.al >>= 1;
						reste.bit.al += 0x7F;

						reste.bit.ah = oldVertexParam;

						oldVertexX += 2;

						for (i = 0; i < oldVertexX; i++) {
							if ((ptr3 - polyTab2) < 960)
								*(ptr3) = reste.temp;
							ptr3 += direction;
							reste.temp += test.temp;
						}
					} else {
						union {
							struct {
								unsigned char al;
								unsigned char ah;
							} bit;
							unsigned short int temp;
						}test;

						union {
							struct {
								unsigned char al;
								unsigned char ah;
							} bit;
							unsigned short int temp;
						}reste;

						test.bit.al = oldVertexParam;
						test.bit.ah = vertexParam2;

						test.bit.ah -= test.bit.al;
						test.bit.ah = -test.bit.ah;

						test.bit.al = 0;

						reste.temp = test.temp % (oldVertexX);

						test.temp /= oldVertexX;

						reste.bit.al >>= 1;
						reste.bit.al = -reste.bit.al;
						reste.bit.al += 0x7F;

						reste.bit.ah = oldVertexParam;

						for (i = 0; i <= oldVertexX; i++) {
							if ((ptr3 - polyTab2) < 960)
								*(ptr3) = reste.temp;
							ptr3 += direction;
							reste.temp -= test.temp;
						}
					}
				}
				direction = 1;
				oldVertexY = push2;
				oldVertexX = push1;
			} else { // if we are going down
				size = currentVertexY - oldVertexY; // size is the number of pixel we must go
				// verticaly

				if (oldVertexX > currentVertexX) { // if we are going down and to the left
					echange = oldVertexX; // in that case, we will draw the line the other
					// side (from new point to old point)
					oldVertexX = currentVertexX;
					currentVertexX = echange;

					echange = currentVertexY;
					currentVertexY = oldVertexY;
					oldVertexY = echange;

					echange = oldVertexParam;
					oldVertexParam = vertexParam2;
					vertexParam2 = echange;

					direction = -1; // since we are going backward in the screen
				}

				temp2 = oldVertexY; // temp2 is the starting Y position
				oldVertexY = size;  // oldVertexY now become the number of pixel
				size = temp2 * 2;

				ptr3 = &polyTab[temp2]; // ptr3 is the output ptr in the renderTab

				temp4 = ((currentVertexX - oldVertexX) << 16);  // temp4 = size in X << 16

				temp5 = temp4 / oldVertexY; // temp5 is the size of a step << 16
				temp6 = temp4 % oldVertexY; // temp6 is the remaining << 16

				vfloat = ((float)(currentVertexX - oldVertexX)) / ((float) oldVertexY);

				temp6 >>= 1;
				temp6 += 0x7FFF;

				step = (unsigned short) temp5;  // retrieve the size of a step

				// temp7 = (((unsigned short)temp6) | ((oldVertexX & 0xFFFF)<<16));
				vfloat2 = oldVertexX;

				oldVertexX = oldVertexY;  // oldVertexX is now the number of vertical pixels

				oldVertexY += 2;

				for (i = 0; i < oldVertexY; i++) {
					// *(ptr3)=((temp7&0xFFFF0000)>>16);
					if ((ptr3 - polyTab) < 960)
						*(ptr3) = (short int) vfloat2;
					ptr3 += direction;
					// temp7+=step;
					vfloat2 += vfloat;
				}

				if (FillVertic_AType >= 7) {
					short int* ptr3 = &polyTab2[temp2];

					temp4 = ((vertexParam2 - oldVertexParam)); // compute the color difference

					if (temp4 >= 0) {
						union {
							struct {
								unsigned char al;
								unsigned char ah;
							} bit;
							unsigned short int temp;
						}test;

						union {
							struct {
								unsigned char al;
								unsigned char ah;
							} bit;
							unsigned short int temp;
						}reste;

						test.bit.al = oldVertexParam;
						test.bit.ah = vertexParam2;

						test.bit.ah -= test.bit.al;

						test.bit.al = 0;

						reste.temp = test.temp % oldVertexX;

						test.temp /= oldVertexX;

						reste.bit.al >>= 1;
						reste.bit.al += 0x7F;

						reste.bit.ah = oldVertexParam;

						oldVertexX += 2;

						for (i = 0; i < oldVertexX; i++) {
							if ((ptr3 - polyTab2) < 960)
								*(ptr3) = reste.temp;
							ptr3 += direction;
							reste.temp += test.temp;
						}
					} else {
						union {
							struct {
								unsigned char al;
								unsigned char ah;
							} bit;
							unsigned short int temp;
						}test;

						union {
							struct {
								unsigned char al;
								unsigned char ah;
							} bit;
							unsigned short int temp;
						}reste;

						test.bit.al = oldVertexParam;
						test.bit.ah = vertexParam2;

						test.bit.ah -= test.bit.al;
						test.bit.ah = -test.bit.ah;

						test.bit.al = 0;

						reste.temp = test.temp % (oldVertexX);

						test.temp /= oldVertexX;

						reste.bit.al >>= 1;
						reste.bit.al = -reste.bit.al;
						reste.bit.al += 0x7F;

						reste.bit.ah = oldVertexParam;

						for (i = 0; i <= oldVertexX; i++) {
							if ((ptr3 - polyTab2) < 960)
								*(ptr3) = reste.temp;
							ptr3 += direction;
							reste.temp -= test.temp;
						}
					}
				}
				direction = 1;
				oldVertexY = push2;
				oldVertexX = push1;
			}
		}
	} while (--numOfVertexRemaining);

	return (1);
}

void drawLine(int a, int b, int c, int d, int e) {
	int temp;
	short int flag;
	int flag2;
	unsigned char *out;
	short int color;
	short int var2;
	short int xchg;
	int currentLineColor = e;

	if (a > c) {    // pour toujours dessiner de gauche à droite
		temp = c;
		c = a;
		a = temp;

		temp = d;
		d = b;
		b = temp;
	}

	flag = 0;

	if (a < textWindowLeft) {
		flag |= 1;
	} else {
		if (a > textWindowRight)
			return;
	}
	if (b < textWindowTop) {
		flag |= 8;
	} else {
		if (b > textWindowBottom)
			flag |= 4;
	}

	flag <<= 8;

	if (c < textWindowLeft)
		return;
	if (c <= textWindowLeft)
		flag |= 2;

	if (d < textWindowTop) {
		flag |= 8;
	} else {
		if (d > textWindowBottom)
			flag |= 4;
	}

	flag2 = flag;

	if (flag)
		return;

// implementer la suite

	flag2 = videoWidth;    // esi
	c -= a;
	d -= b;
	if (d < 0) {
		flag2 = -flag2;
		d = -d;
	}

	assert(frontVideoBufferbis == frontVideoBuffer);
	out = frontVideoBuffer + screenLockupTable[b] + a;

	color = currentLineColor;
	if (c < d) {    // pente importante
		xchg = c;
		c = d;
		d = xchg;
		var2 = c;
		var2 <<= 1;
		b = c;
		d <<= 1;
		c++;
		do {
			*out = (unsigned char) color;
			b -= d;
			if (b > 0) {
				out += flag2;
			} else {
				b += var2;
				out += flag2 + 1;
			}
		} while (--c);
	} else {   // pente reduite
		var2 = c;
		var2 <<= 1;
		b = c;
		d <<= 1;
		c++;
		do {
			*out = (unsigned char) color;
			out++;
			b -= d;
			if (b < 0) {
				b += var2;
				out += flag2;
			}
		} while (--c);
	}
}

void setCameraPosition(int X, int Y, int param2, int param3, int param4) {
	setSomethingVar1 = X;
	setSomethingVar2 = Y;

	cameraVar1 = param2;
	cameraVar2 = param3;
	cameraVar3 = param4;

	isUsingOrhoProjection = 0;
}

void setCameraAngle(int param0, int param1, int param2, int param3, int param4, int param5, int param6) {
	setSomething2Var1 = param0;
	setSomething2Var2 = param1;
	setSomething2Var3 = param2;

	setSomething3(param3, param4, param5);

	setSomething3Var16 += param6;

	setCameraAngleSub(setSomething3Var12, setSomething3Var14, setSomething3Var16);

	setSomething2Var1 = destX;
	setSomething2Var2 = destZ;
	setSomething2Var3 = destY;
}

void setCameraAngleSub(int eax, int ebx, int ecx) {
	destX = (_baseMatrix[0] * eax + _baseMatrix[3] * ebx + _baseMatrix[6] * ecx) >> 14;
	destZ = (_baseMatrix[1] * eax + _baseMatrix[4] * ebx + _baseMatrix[7] * ecx) >> 14;
	destY = (_baseMatrix[2] * eax + _baseMatrix[5] * ebx + _baseMatrix[8] * ecx) >> 14;
}

void setSomething3sub(int eax, int ebx, int ecx) {
	destX = (_baseMatrix[0] * eax + _baseMatrix[1] * ebx + _baseMatrix[2] * ecx) >> 14;
	destZ = (_baseMatrix[3] * eax + _baseMatrix[4] * ebx + _baseMatrix[5] * ecx) >> 14;
	destY = (_baseMatrix[6] * eax + _baseMatrix[7] * ebx + _baseMatrix[8] * ecx) >> 14;
}

void setSomething2(int a, int b, int c) {
	setSomething2Var1 = a;
	setSomething2Var2 = b;
	setSomething2Var3 = c;
}

void setSomething3(int a, int b, int c) { // setupBaseMatrix
	int angleXCos;
	int angleXSin;

	int angleYCos;
	int angleYSin;

	int angleZCos;
	int angleZSin;

	int temp;

	tab1 = &angleTable[0];
	tab2 = &angleTable[256];
	tab3 = &angleTable[384];

	baseMatrixRotationX = a & 0x3FF;
	baseMatrixRotationY = b & 0x3FF;
	baseMatrixRotationZ = c & 0x3FF;

	angleXCos = tab1[baseMatrixRotationX];
	angleXSin = tab1[(baseMatrixRotationX + 256) & 0x3FF];

	angleYCos = tab1[baseMatrixRotationY];
	angleYSin = tab1[(baseMatrixRotationY + 256) & 0x3FF];

	angleZCos = tab1[baseMatrixRotationZ];
	angleZSin = tab1[(baseMatrixRotationZ + 256) & 0x3FF];

	_baseMatrix[0] = angleZSin;
	_baseMatrix[1] = -angleZCos;
	_baseMatrix[3] = (angleZCos * angleXSin) >> 14;
	_baseMatrix[4] = (angleZSin * angleXSin) >> 14;
	_baseMatrix[6] = (angleZCos * angleXCos) >> 14;
	_baseMatrix[7] = (angleZSin * angleXCos) >> 14;

	_baseMatrix[0] = (angleZSin * angleYSin) >> 14;
	_baseMatrix[2] = (angleZSin * angleYCos) >> 14;

	temp = _baseMatrix[3];

	_baseMatrix[3] = ((angleYSin * temp) + (angleYCos * angleXCos)) >> 14;
	_baseMatrix[5] = ((angleYCos * temp) - (angleYSin * angleXCos)) >> 14;

	temp = _baseMatrix[6];

	_baseMatrix[6] = ((angleYSin * temp) - (angleXSin * angleYCos)) >> 14;
	_baseMatrix[8] = ((angleYCos * temp) + (angleXSin * angleYSin)) >> 14;

	setSomething3sub(setSomething2Var1, setSomething2Var2, setSomething2Var3);

	setSomething3Var12 = destX;
	setSomething3Var14 = destZ;
	setSomething3Var16 = destY;
}
