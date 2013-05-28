// Copyright © 2013 Vitaly Lyaschenko (scxv86@gmail.com)
// Purpose:
//
#include <stdio.h>
#include <string>

#include <wchar.h>
#include <assert.h>

#include "public/utils.h"

// Targa header
#pragma pack(1)
typedef struct
{
	unsigned char identsize;       // size of ID field that follows 18 byte header (0 usually)
	unsigned char colourmaptype;   // type of colour map 0=none, 1=has palette
	unsigned char imagetype;       // type of image 0=none,1=indexed,2=rgb,3=grey,+8=rle packed

	short         colourmapstart;  // first colour map entry in palette
	short         colourmaplength; // number of colours in palette
	unsigned char colourmapbits;   // number of bits per palette entry 15,16,24,32

	short         xstart;          // image x origin
	short         ystart;          // image y origin
	short         width;           // image width in pixels
	short         height;          // image height in pixels
	unsigned char bits;            // image bits per pixel 8,16,24,32
	unsigned char descriptor;      // image descriptor bits (vh flip bits)

} TGAHeader;
#pragma pack()

namespace utils
{
	bool TGA_Save(const char* fileName, const int width, const int height, unsigned char const* pData)
	{
		if(!fileName || ((width * height) == 0) || !pData) {
			return false;
		}
		TGAHeader header;
		int bytewrite = 0;

		FILE* pFile = fopen(fileName, "wb");
		if(!pFile) {
			return false;
		}
		const size_t sizeHeader = sizeof(TGAHeader);
		memset(&header, 0, sizeHeader);
		header.imagetype = 3;
		header.width =  (short)width;
		header.height = (short)height;
		header.bits = 8;
		header.descriptor = 0x20;

		header.xstart = 0;
		header.ystart = 0;

		bytewrite = fwrite(&header, 1, sizeHeader, pFile);

		if(bytewrite != sizeHeader)
		{
			fclose(pFile);
			return false;
		}
		const int texDim = width*height;
		bytewrite = fwrite(pData, 1, texDim, pFile);
		if(bytewrite != texDim)
		{
			fclose(pFile);
			return false;
		}
		fclose(pFile);
		return true;
	}

	// TODO: implement using a file system
	bool TGA_Load(const char* fileName, unsigned char **buffer, int& w, int& h)
	{
		if(!fileName) {
			return false;
		}
		TGAHeader header;
		int byteread = 0;
		w = h = 0;

		FILE* pFile = fopen(fileName, "rb");
		if(!pFile) {
			return false;
		}
		const size_t sizeHeader = sizeof(TGAHeader);
		byteread = fread(&header, 1, sizeHeader, pFile);
		if(byteread != sizeHeader)
		{
			fclose(pFile);
			return false;
		}
		if(header.imagetype != 3)
		{
			fclose(pFile);
			return false;
		}
		w = header.width;
		h = header.height;
		byteread = fread(*buffer, 1, w * h, pFile);
		if(byteread != (w * h))
		{
			fclose(pFile);
			return false;
		}
		fclose(pFile);
		return true;
	}

	int ExtractFileName(const char* path, char* buffer)
	{
		assert(path && buffer);
		int ilen = strlen(path);
		while(ilen > 0)
		{
			if(path[ilen] == '\\' || path[ilen] == '/') {
				break;
			}
			--ilen;
		}
		const char* beg = &path[ilen] + 1;
		int chInName(0);
		// count the number of characters in a string name
		while(*beg)
		{
			if(*beg == '.') {
				break;
			}
			++chInName;
			++beg;
		}
		strncpy(buffer, &path[ilen] + 1, chInName);
		buffer[chInName] = '\0';
		return chInName;
	}

	int LenghtPath(const char* path)
	{
		assert(path);
		int ilen = strlen(path) - 1;
		while(ilen > 0)
		{
			if(path[ilen] == '\\' || path[ilen] == '/') {
				break;
			}
			--ilen;
		}
		return ilen;
	}

	void FixSlashes(char* str)
	{
		while(*str)
		{
			if(*str == '\\')
			{
				*str = '/';
			}
			str++;
		}
	}

	int ExtractFilePath(const char* path, char* buffer)
	{
		assert(path && buffer);
		const int ilen = LenghtPath(path);
		strncpy(buffer, path, ilen);
		buffer[ilen] = '\0';
		return ilen;
	}

} // End namespace utils