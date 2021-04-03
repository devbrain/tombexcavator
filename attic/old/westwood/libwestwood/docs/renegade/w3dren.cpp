/*	Renegade W3D Renamer
	Copyright 2007 Jonathan Wilson

	This file is part of the Renegade scripts.dll
	The Renegade scripts.dll is free software; you can redistribute it and/or modify it under
	the terms of the GNU General Public License as published by the Free
	Software Foundation; either version 2, or (at your option) any later
	version. See the file COPYING for more details.
	In addition, an exemption is given to allow Run Time Dynamic Linking of this code with any closed source module that does not contain code covered by this licence.
	Only the source code to the module(s) containing the licenced code has to be released.
*/
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdio.h>
#include <wchar.h>
#include <errno.h>
#pragma warning (disable: 4100 4244)
#include "w3d_file.h"
#pragma warning (default: 4244)

char *newstr(const char *str)
{
	unsigned long len = strlen(str)+1;
	char *s = new char[len];
	memcpy(s,str,len);
	return s;
}

class FileClass {
public:
	virtual ~FileClass()
	{
	}
	virtual char *File_Name() = 0;
	virtual char *Set_Name(const char* name) = 0;
	virtual bool Create() = 0;
	virtual bool Delete() = 0;
	virtual bool Is_Available(int handle) = 0;
	virtual bool Is_Open() = 0;
	virtual int Open(const char* name, int mode) = 0;
	virtual int Open(int mode) = 0;
	virtual int Read(void* buffer, int size) = 0;
	virtual int Seek(int offset, int origin) = 0;
	virtual int Tell()
	{
		return Seek(0,1);
	}
	virtual int Size() = 0;
	virtual int Write(void* buffer, int size) = 0;
	virtual void Close() = 0;
	virtual unsigned long Get_Date_Time()
	{
		return 0;
	}
	virtual bool Set_Date_Time()
	{
		return false;
	}
	virtual void Error(int a, int b, char *c) = 0;
	virtual HANDLE Get_File_Handle()
	{
		return (HANDLE)-1;
	}
	virtual void Bias(int start, int length) = 0;
};

struct ChunkHeader {
	unsigned long ChunkType;
	unsigned long ChunkSize;
};

struct MicroChunkHeader {
	unsigned char ChunkType;
	unsigned char ChunkSize;
};

struct IOVector2Struct {
	float X;
	float Y;
};

struct IOVector4Struct {
	float X;
	float Y;
	float Z;
	float W;
};

class ChunkLoadClass {
private:
	FileClass* File;
	int StackIndex;
	unsigned long PositionStack[256];
	ChunkHeader HeaderStack[256];
	bool InMicroChunk;
	int MicroChunkPosition;
	MicroChunkHeader MCHeader;
public:
	ChunkLoadClass(FileClass *file);
	bool Open_Chunk();
	bool Peek_Next_Chunk(unsigned int *id, unsigned int *length);
	bool Close_Chunk();
	unsigned long Cur_Chunk_ID();
	unsigned long Cur_Chunk_Length();
	int Cur_Chunk_Depth();
	int Contains_Chunks();
	bool Open_Micro_Chunk();
	bool Close_Micro_Chunk();
	unsigned long Cur_Micro_Chunk_ID();
	unsigned long Cur_Micro_Chunk_Length();
	long Seek(unsigned long nbytes);
	long Read(void *buf, unsigned long nbytes);
	long Read(IOVector2Struct *v);
	long Read(IOVector3Struct *v);
	long Read(IOVector4Struct *v);
	long Read(IOQuaternionStruct *q);
};

class ChunkSaveClass {
	class FileClass* File;
	int StackIndex;
	int PositionStack[256];
	struct ChunkHeader HeaderStack[256];
	bool InMicroChunk;
	int MicroChunkPosition;
	struct MicroChunkHeader MCHeader;
public:
	ChunkSaveClass(FileClass *file);
	bool Begin_Chunk(unsigned long id);
	bool End_Chunk();
	int Cur_Chunk_Depth();
	bool Begin_Micro_Chunk(unsigned long id);
	bool End_Micro_Chunk();
	unsigned long Write(void* buf,unsigned long nbytes);
	unsigned long Write(IOVector2Struct& v);
	unsigned long Write(IOVector3Struct& v);
	unsigned long Write(IOVector4Struct& v);
	unsigned long Write(IOQuaternionStruct& q);
};

template <class T> class SimpleVecClass {
protected:
	T *Vector;
	int VectorMax;
public:
	SimpleVecClass()
	{
		Vector = 0;
		VectorMax = 0;
	}
	SimpleVecClass(unsigned int size)
	{
		Vector = 0;
		VectorMax = 0;
		if (size > 0)
		{
			Resize(size);
		}
	}
	virtual bool Uninitialised_Grow(int newsize)
	{
		if ((newsize > 0) && (newsize > VectorMax))
		{
			if (Vector)
			{
				delete Vector;
			}
			Vector = new T[newsize*4];
			VectorMax = newsize;
		}
		return true;
	}
	SimpleVecClass(const SimpleVecClass<T> &cl)
	{
		VectorMax = cl.VectorMax;
		Vector = new T[VectorMax*4];
		memcpy(Vector,cl.Vector,VectorMax*4);
	}
	int Length()
	{
		return VectorMax;
	}
	T &operator[](int index)
	{
		return Vector[index];
	}
	virtual ~SimpleVecClass()
	{
		if (Vector)
		{
			delete Vector;
			Vector = 0;
			VectorMax = 0;
		}
	}
	virtual bool Resize(int newsize)
	{
		T *vec;
		if (VectorMax == newsize)
		{
			return true;
		}
		if (newsize > 0)
		{
			vec = new T[newsize*4];
			if (Vector)
			{
				int count = VectorMax;
				if (newsize < count)
				{
					count = newsize;
				}
				memcpy(vec,Vector,count*4);
				delete Vector;
				Vector = 0;
			}
			Vector = vec;
			VectorMax = newsize;
		}
		else
		{
			VectorMax = 0;
			if (Vector)
			{
				delete Vector;
				Vector = 0;
			}
		}
		return true;
	}
	SimpleVecClass &operator=(const SimpleVecClass<T> &cl)
	{
		if (Vector)
		{
			delete Vector;
		}
		VectorMax = cl.VectorMax;
		Vector = new T[VectorMax*4];
		memcpy(Vector,cl.Vector,VectorMax*4);
		return *this;
	}
};

template <class T> class SimpleDynVecClass : public SimpleVecClass<T> {
protected:
	int ActiveCount;
public:
	SimpleDynVecClass() : SimpleVecClass<T>(0)
	{
		ActiveCount = 0;
	}
	SimpleDynVecClass(int size) : SimpleVecClass<T>(size)
	{
		ActiveCount = 0;
	}
	SimpleDynVecClass(const SimpleDynVecClass<T> &cl) : SimpleVecClass<T>(cl)
	{
		ActiveCount = cl.ActiveCount;
	}
	int Count()
	{
		return ActiveCount;
	}
	virtual ~SimpleDynVecClass()
	{
		if (Vector)
		{
			delete Vector;
			Vector = 0;
		}
	}
	virtual bool Resize(int newsize)
	{
		if (SimpleVecClass<T>::Resize(newsize))
		{
			if (VectorMax < ActiveCount)
			{
				ActiveCount = VectorMax;
			}
			return true;
		}
		return false;
	}
	int Add(T& data)
	{
		for (int i = 0;i < VectorMax;i++)
		{
			if (!Vector[i])
			{
				Vector[i] = data;
				ActiveCount++;
				return i;
			}
		}
		Resize(VectorMax+1);
		Vector[VectorMax-1] = data;
		ActiveCount++;
		return VectorMax-1;
	}
	void Clear(int position)
	{
		Vector[position] = 0;
	}
	void Delete(int position)
	{
		if (position < (ActiveCount - 1))
		{
			int size = ((ActiveCount - position) * 4) - 4;
			memmove(&Vector[position],&Vector[position+1],size);
		}
		ActiveCount--;
		Vector[ActiveCount] = 0;
	}
	void Delete_All()
	{
		ActiveCount = 0;
		VectorMax = 0;
		if (Vector)
		{
			delete Vector;
			Vector = 0;
		}
	}
	SimpleDynVecClass &operator=(const SimpleDynVecClass<T> &cl)
	{
		SimpleVecClass<T>::operator =(cl);
		ActiveCount = cl.ActiveCount;
		return *this;
	}
};

class RawFileClass : public FileClass {
private:
	int Rights;
	int BiasStart;
	int BiasLength;
	void* Handle;
	char* Filename;
	unsigned short Date;
	unsigned short Time;
	bool allocated;
public:
	RawFileClass()
	{
		BiasLength = -1;
		Handle = (HANDLE)-1;
		Rights = 0;
		BiasStart = 0;
		Date = 0;
		Time = 0;
		allocated = false;
		Filename = 0;
	}
	RawFileClass(char *filename)
	{
		BiasLength = -1;
		Handle = (HANDLE)-1;
		Rights = 0;
		BiasStart = 0;
		Date = 0;
		Time = 0;
		allocated = false;
		Filename = filename;
	}
	int Transfer_Block_Size()
	{
		return -11;
	}
	void Reset()
	{
		Close();
		if ((allocated) && (Filename))
		{
			delete Filename;
		}
		allocated = false;
		Filename = 0;
	}
	~RawFileClass()
	{
		Reset();
	}
	char *File_Name()
	{
		return Filename;
	}
	char *Set_Name(const char* name)
	{
		Reset();
		if (name)
		{
			Bias(0,-1);
			char *c = newstr(name);
			if (!c)
			{
				Error(0x0C,(int)c,(char *)name);
			}
			Filename = c;
			allocated = true;
		}
		return 0;
	}
	bool Create()
	{
		Close();
		if (!Open(2))
		{
			return false;
		}
		if (BiasLength != -1)
		{
			Seek(0,0);
		}
		Close();
		return true;
	}
	bool Delete()
	{
		Close();
		if (!Filename)
		{
			Error(2,0,0);
		}
		if (!Is_Available(0))
		{
			return false;
		}
		if (!DeleteFile(Filename))
		{
			Error(GetLastError(),0,Filename);
			return false;
		}
		return true;
	}
	bool Is_Available(int handle)
	{
		if (!Filename)
		{
			return false;
		}
		if (!Is_Open())
		{
			if (Handle)
			{
				Open(1);
				Close();
			}
			Handle = CreateFile(Filename,GENERIC_READ,FILE_SHARE_READ,0,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,0);
			if (!Handle)
			{
				return false;
			}
			if (!CloseHandle(Handle))
			{
				Error(GetLastError(),0,Filename);
			}
			Handle = (HANDLE)-1;
		}
		return true;
	}
	bool Is_Open()
	{
		if (Handle != (HANDLE)-1)
		{
			return true;
		}
		return false;
	}
	int Open(const char* name, int mode)
	{
		Set_Name(name);
		return Open(mode);
	}
	int Open(int mode)
	{
		Close();
		if (!Filename)
		{
			Error(2,0,0);
		}
		Rights = mode;
		switch (Rights)
		{
		case 2:
			Handle = CreateFile(Filename,GENERIC_WRITE,0,0,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,0);
			break;
		case 1:
			Handle = CreateFile(Filename,GENERIC_READ,FILE_SHARE_READ,0,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,0);
			break;
		case 0:
			Handle = CreateFile(Filename,GENERIC_READ|GENERIC_WRITE,0,0,OPEN_ALWAYS,FILE_ATTRIBUTE_NORMAL,0);
			break;
		default:
			errno = EINVAL;
		}
		if ((BiasStart) && (BiasLength != -1))
		{
			Seek(0,0);
		}
		if (Handle != (HANDLE)-1)
		{
			return true;
		}
		return false;
	}
	int Read(void* buffer, int size)
	{
		bool opened = false;
		long bytesread = 0;
		long bytesread2 = 0;
		if (!Is_Open())
		{
			if (!Open(2))
			{
				return 0;
			}
			opened = true;
		}
		if (BiasLength != -1)
		{
			int x = Seek(0,1) + BiasLength;
			if (size >= x)
			{
				size = x;
			}
		}
		while (size > 0)
		{
			if (!ReadFile(Handle,buffer,size,(LPDWORD)&bytesread,0))
			{
				Error(GetLastError(),1,Filename);
				bytesread2 += bytesread;
			}
			else
			{
				bytesread2 += bytesread;
				if (!bytesread)
				{
					break;
				}
			}
			size -= bytesread;
		}
		if (opened)
		{
			Close();
		}
		return bytesread;
	}
	int Seek(int pos, int dir)
	{
		if (BiasLength != -1)
		{
			switch (dir)
			{
			case 0:
				if (pos > BiasLength)
				{
					pos = BiasLength;
				}
				pos += BiasStart;
				break;
			case 2:
				pos += (BiasStart + BiasLength);
				dir = 0;
				break;
			}
			int seek = Raw_Seek(pos,dir) - BiasStart;
			if (seek < 0)
			{
				seek = Raw_Seek(BiasStart,0) - BiasStart;
			}
			if (seek > BiasLength)
			{
				seek = Raw_Seek(BiasStart + BiasLength,0) - BiasStart;
			}
			return seek;
		}
		else
		{
			return Raw_Seek(pos,dir);
		}
	}
	int Raw_Seek(int pos, int dir)
	{
		if (!Is_Open())
		{
			Error(9,0,Filename);
		}
		int seek = SetFilePointer(Handle,pos,0,dir);
		if (seek == -1)
		{
			Error(GetLastError(),0,Filename);
		}
		return seek;
	}
	int Size()
	{
		int size = 0;
		if (BiasLength == -1)
		{
			if (Is_Open())
			{
				size = GetFileSize(Handle,0);
				if (size == -1)
				{
					Error(GetLastError(),0,Filename);
				}
			}
			else
			{
				if (Open(1))
				{
					size = Size();
					Close();
				}
			}
			BiasLength = size - BiasStart;
			return BiasLength;
		}
		else
		{
			return BiasLength;
		}
	}
	int Write(void* buffer, int size)
	{
		bool opened = false;
		long byteswritten = 0;
		if (!Is_Open())
		{
			if (!Open(2))
			{
				return 0;
			}
			opened = true;
		}
		if (!WriteFile(Handle,buffer,size,(LPDWORD)&byteswritten,0))
		{
			Error(GetLastError(),0,Filename);
		}
		if (BiasLength != -1)
		{
			if (Raw_Seek(0,1) > BiasStart + BiasLength)
			{
				BiasLength = Raw_Seek(0,1) - BiasStart;
			}
		}
		if (opened)
		{
			Close();
		}
		return byteswritten;
	}
	void Close()
	{
		if (Is_Open())
		{
			if (!CloseHandle(Handle))
			{
				Error(GetLastError(),0,Filename);
			}
			Handle = (HANDLE)-1;
		}
	}
	unsigned long Get_Date_Time()
	{
		BY_HANDLE_FILE_INFORMATION info;
		unsigned short dostime;
		unsigned short dosdate;
		if (GetFileInformationByHandle(Handle,&info))
		{
			FileTimeToDosDateTime(&info.ftLastWriteTime,&dosdate,&dostime);
			return dosdate << 0x10 | dostime;
		}
		return 0;
	}
	bool Set_Date_Time(unsigned long datetime)
	{
		BY_HANDLE_FILE_INFORMATION info;
		FILETIME filetime;
		if (Is_Open())
		{
			if (GetFileInformationByHandle(Handle,&info))
			{
				if (DosDateTimeToFileTime((WORD)datetime,(WORD)(datetime >> 0x10),&filetime))
				{
					if (SetFileTime(Handle,&info.ftCreationTime,&filetime,&filetime))
					{
						return true;
					}
				}
			}
		}
		return false;
	}
	void Error(int a, int b, char *c)
	{
	}
	HANDLE Get_File_Handle()
	{
		return Handle;
	}
	void Bias(int start, int length)
	{
		if (!start)
		{
			BiasStart = 0;
			BiasLength = -1;
		}
		else
		{
			BiasLength = Size();
			BiasStart += start;
			if (length != -1)
			{
				if (BiasLength > length)
				{
					BiasLength = length;
				}
				int b = (BiasLength <= 0 ? 1 : 0);
				b--;
				BiasLength &= b;
			}
		}
	}
	void Attach(HANDLE handle,int rights)
	{
		Reset();
		Handle = handle;
		Rights = rights;
		BiasStart = 0;
		BiasLength = -1;
		Date = 0;
		Time = 0;
		allocated = false;
	}
	void Detatch()
	{
		Rights = 0;
		Handle = (HANDLE)-1;
		BiasStart = 0;
		BiasLength = -1;
		Date = 0;
		Time = 0;
		allocated = false;
	}
};

FileClass *Get_Data_File(char *file)
{
	return new RawFileClass(file);
}

void Close_Data_File(FileClass *file)
{
	file->Close();
}

ChunkLoadClass::ChunkLoadClass(FileClass *file)
{
	File = file;
	StackIndex = 0;
	memset(HeaderStack,0,sizeof(HeaderStack));
	memset(PositionStack,0,sizeof(PositionStack));
	InMicroChunk = false;
	MicroChunkPosition = 0;
	MCHeader.ChunkType = 0;
}

bool ChunkLoadClass::Open_Chunk()
{
	if ((StackIndex <= 0) || (PositionStack[StackIndex-1] != (HeaderStack[StackIndex-1].ChunkSize & 0x7FFFFFFF)))
	{
		if (File->Read(&HeaderStack[StackIndex],8) == 8)
		{
			PositionStack[StackIndex] = 0;
			StackIndex++;
			return true;
		}
	}
	return false;
}

bool ChunkLoadClass::Peek_Next_Chunk(unsigned int *id, unsigned int *length)
{
	ChunkHeader h;
	if ((StackIndex <= 0) || (PositionStack[StackIndex-1] != (HeaderStack[StackIndex-1].ChunkSize & 0x7FFFFFFF)))
	{
		if (File->Read(&h,8) == 8)
		{
			File->Seek(-8,1);
			if (length)
			{
				*length = h.ChunkSize;
			}
			if (id)
			{
				*id = h.ChunkType & 0x7FFFFFFF;
			}
			return true;
		}
	}
	return false;
}

bool ChunkLoadClass::Close_Chunk()
{
	unsigned int x = HeaderStack[StackIndex-1].ChunkSize & 0x7FFFFFFF;
	if (PositionStack[StackIndex-1] < (x))
	{
		File->Seek((x) - PositionStack[StackIndex-1],1);
	}
	StackIndex--;
	if (StackIndex > 0)
	{
		PositionStack[StackIndex-1] += (x);
		PositionStack[StackIndex-1] += 8;
	}
	return true;
}

unsigned long ChunkLoadClass::Cur_Chunk_ID()
{
	return HeaderStack[StackIndex-1].ChunkType;
}

unsigned long ChunkLoadClass::Cur_Chunk_Length()
{
	return HeaderStack[StackIndex-1].ChunkSize & 0x7FFFFFFF;
}

int ChunkLoadClass::Cur_Chunk_Depth()
{
	return StackIndex;
}

int ChunkLoadClass::Contains_Chunks()
{
	return HeaderStack[StackIndex-1].ChunkSize & 0x80000000;
}

bool ChunkLoadClass::Open_Micro_Chunk()
{
	int bytesread = Read(&MCHeader,2);
	if (bytesread == 2)
	{
		MicroChunkPosition = 0;
		InMicroChunk = true;
		return true;
	}
	return false;
}

bool ChunkLoadClass::Close_Micro_Chunk()
{
	InMicroChunk = false;
	if (MicroChunkPosition < 0)
	{
		File->Seek(MCHeader.ChunkSize - MicroChunkPosition,1);
		if (StackIndex > 0)
		{
			PositionStack[StackIndex-1] += MCHeader.ChunkSize - MicroChunkPosition;
		}
	}
	return true;
}

unsigned long ChunkLoadClass::Cur_Micro_Chunk_ID()
{
	return MCHeader.ChunkType;
}

unsigned long ChunkLoadClass::Cur_Micro_Chunk_Length()
{
	return MCHeader.ChunkSize;
}

long ChunkLoadClass::Seek(unsigned long nbytes)
{
	if ((PositionStack[StackIndex-1] + nbytes) <= (HeaderStack[StackIndex-1].ChunkSize & 0x7FFFFFFF))
	{
		if ((!InMicroChunk) || ((MicroChunkPosition + nbytes) <= MCHeader.ChunkSize))
		{
			int pos = File->Tell();
			int seek = File->Seek(nbytes,1);
			if ((seek - pos) == (int)nbytes)
			{
				PositionStack[StackIndex-1] += nbytes;
				if (InMicroChunk)
				{
					MicroChunkPosition += nbytes;
				}
				return nbytes;
			}
		}
	}
	return 0;
}

long ChunkLoadClass::Read(void *buf, unsigned long nbytes)
{
	if ((PositionStack[StackIndex-1] + nbytes) <= (HeaderStack[StackIndex-1].ChunkSize & 0x7FFFFFFF))
	{
		if ((!InMicroChunk) || ((MicroChunkPosition + nbytes) <= MCHeader.ChunkSize))
		{
			int read = File->Read(buf,nbytes);
			if (read == (int)nbytes)
			{
				PositionStack[StackIndex-1] += nbytes;
				if (InMicroChunk)
				{
					MicroChunkPosition += nbytes;
				}
				return nbytes;
			}
		}
	}
	return 0;
}

long ChunkLoadClass::Read(IOVector2Struct *v)
{
	return Read(v,4);
}

long ChunkLoadClass::Read(IOVector3Struct *v)
{
	return Read(v,4);
}

long ChunkLoadClass::Read(IOVector4Struct *v)
{
	return Read(v,4);
}

long ChunkLoadClass::Read(IOQuaternionStruct *q)
{
	return Read(q,4);
}

ChunkSaveClass::ChunkSaveClass(FileClass *file)
{
	File = file;
	StackIndex = 0;
	memset(HeaderStack,0,sizeof(HeaderStack));
	memset(PositionStack,0,sizeof(PositionStack));
	InMicroChunk = false;
	MicroChunkPosition = 0;
	MCHeader.ChunkType = 0;
}

bool ChunkSaveClass::Begin_Chunk(unsigned long id)
{
	ChunkHeader chunkh;
	chunkh.ChunkSize = 0;
	chunkh.ChunkType = 0;
	if (StackIndex > 0)
	{
		HeaderStack[StackIndex-1].ChunkSize |= 0x80000000;
	}
	chunkh.ChunkType = id;
	chunkh.ChunkSize &= 0x80000000;
	PositionStack[StackIndex] = File->Seek(0,1);
	HeaderStack[StackIndex].ChunkType = chunkh.ChunkType;
	HeaderStack[StackIndex].ChunkSize = chunkh.ChunkSize;
	StackIndex++;
	if (File->Write(&chunkh,8) == 8)
	{
		return true;
	}
	return false;
}

bool ChunkSaveClass::End_Chunk()
{
	ChunkHeader chunkh;
	int temp = File->Seek(0,1);
	StackIndex--;
	chunkh.ChunkType = HeaderStack[StackIndex].ChunkType;
	chunkh.ChunkSize = HeaderStack[StackIndex].ChunkSize;
	File->Seek(PositionStack[StackIndex],0);
	if (File->Write(&chunkh,8) == 8)
	{
		if (StackIndex)
		{
			int temp = (HeaderStack[StackIndex-1].ChunkSize & 0x7FFFFFFF) + (chunkh.ChunkSize & 0x7FFFFFFF) + 8;
			if (HeaderStack[StackIndex-1].ChunkSize & 0x80000000)
			{
				temp |= 0x80000000;
			}
			HeaderStack[StackIndex-1].ChunkSize = temp;
		}
		File->Seek(temp,0);
		return true;
	}
	return false;
}

int ChunkSaveClass::Cur_Chunk_Depth()
{
	return StackIndex;
}

bool ChunkSaveClass::Begin_Micro_Chunk(unsigned long id)
{
	MCHeader.ChunkType = (unsigned char)id;
	MicroChunkPosition = File->Seek(0,1);
	MCHeader.ChunkSize = 0;
	if (Write(&MCHeader,2) == 2)
	{
		InMicroChunk = true;
		return true;
	}
	return false;
}

bool ChunkSaveClass::End_Micro_Chunk()
{
	int temp = File->Seek(0,1);
	File->Seek(MicroChunkPosition,0);
	if (File->Write(&MCHeader,2) == 2)
	{
		File->Seek(temp,0);
		InMicroChunk = false;
		return true;
	}
	return false;
}

unsigned long ChunkSaveClass::Write(void* buf,unsigned long nbytes)
{
	if (File->Write(buf,nbytes) == (int)nbytes)
	{
		int temp = (HeaderStack[StackIndex-1].ChunkSize & 0x7FFFFFFF) + nbytes;
		if (HeaderStack[StackIndex-1].ChunkSize & 0x80000000)
		{
			temp |= 0x80000000;
		}
		HeaderStack[StackIndex-1].ChunkSize = temp;
		if (InMicroChunk)
		{
			MCHeader.ChunkSize = MCHeader.ChunkSize + (unsigned char)nbytes;
		}
		return nbytes;
	}
	return 0;
}

unsigned long ChunkSaveClass::Write(IOVector2Struct& v)
{
	return Write(&v,4);
}

unsigned long ChunkSaveClass::Write(IOVector3Struct& v)
{
	return Write(&v,4);
}

unsigned long ChunkSaveClass::Write(IOVector4Struct& v)
{
	return Write(&v,4);
}

unsigned long ChunkSaveClass::Write(IOQuaternionStruct& q)
{
	return Write(&q,4);
}

ChunkLoadClass *cload;
ChunkSaveClass *csave;
char *oldtex;
char *newtex;
char *oldw3d;
char *neww3d;
void WriteChunk(ChunkLoadClass *cload,ChunkSaveClass *csave)
{
	while (cload->Open_Chunk())
	{
		csave->Begin_Chunk(cload->Cur_Chunk_ID());
		if (cload->Contains_Chunks())
		{
			WriteChunk(cload,csave);
		}
		else
		{
			switch (cload->Cur_Chunk_ID())
			{
			case W3D_CHUNK_TEXTURE_NAME:
				{
					unsigned int len = cload->Cur_Chunk_Length();
					char *buf = new char[len];
					cload->Read(buf,len);
					if (oldtex && !_stricmp(buf,oldtex))
					{
						delete buf;
						buf = newstr(newtex);
						len = strlen(newtex)+1;
					}
					csave->Write(buf,len);
					delete buf;
					break;
				}
			case W3D_CHUNK_HMODEL_HEADER:
				{
					W3dHModelHeaderStruct header;
					cload->Read(&header,sizeof(W3dHModelHeaderStruct));
					memset(header.Name,0,sizeof(header.Name));
					strcpy(header.Name,neww3d);
					csave->Write(&header,sizeof(W3dHModelHeaderStruct));
					break;
				}
			case W3D_CHUNK_AGGREGATE_HEADER:
				{
					W3dAggregateHeaderStruct header;
					cload->Read(&header,sizeof(W3dAggregateHeaderStruct));
					memset(header.Name,0,sizeof(header.Name));
					strcpy(header.Name,neww3d);
					csave->Write(&header,sizeof(W3dAggregateHeaderStruct));
					break;
				}
			case W3D_CHUNK_HLOD_HEADER:
				{
					W3dHLodHeaderStruct header;
					cload->Read(&header,sizeof(W3dHLodHeaderStruct));
					memset(header.Name,0,sizeof(header.Name));
					strcpy(header.Name,neww3d);
					csave->Write(&header,sizeof(W3dHLodHeaderStruct));
					break;
				}
			default:
				{
					unsigned int len = cload->Cur_Chunk_Length();
					unsigned char *buf = new unsigned char[len];
					cload->Read(buf,len);
					csave->Write(buf,len);
					delete buf;
				}
			}
		}
		csave->End_Chunk();
		cload->Close_Chunk();
	}
}

int main(int argc, char* argv[])
{
	printf("W3D Renamer - renames a W3D file and optionally a texture\n");
	if (argc < 3)
	{
		printf("usage:\nw3dren infile outfile [oldtex newtex]\ninfile is the input file\noutfile is the output file with the new name\noldtex and newtex, if specified specify the name of the old texture and the new one to rename it to");
		exit(1);
	}
	FileClass *inf = Get_Data_File(argv[1]);
	FileClass *outf = Get_Data_File(argv[2]);
	neww3d = newstr(argv[2]);
	strrchr(neww3d,'.')[0] = 0;
	_strupr(neww3d);
	if (argv[3] && argv[4])
	{
		oldtex = newstr(argv[3]);
		newtex = newstr(argv[4]);
	}
	else
	{
		oldtex = 0;
		newtex = 0;
	}
	inf->Open(1);
	outf->Open(2);
	cload = new ChunkLoadClass(inf);
	csave = new ChunkSaveClass(outf);
	WriteChunk(cload,csave);
	return 0;
}
