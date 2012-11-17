/****************************************************************************/
/*	Copyright (c) 2012 Vitaly Lyaschenko < scxv86@gmail.com >
/*
/*	Purpose: Some helper methods
/*
/****************************************************************************/

#pragma once

#include <string.h>
#include <stdlib.h>
#include <wchar.h>
#include <stdio.h>

#include <assert.h>

template<typename T>
T MaxVal(T const & a, T const & b) { return (a > b) ? a : b; }
template<typename T>
T MinVal(T const & a, T const & b) { return (a < b) ? a : b; }

namespace util
{
	bool SaveAsTGA(const char* fileName, const int width, const int height, unsigned char const* pData);
	bool LoadTGA(const char* fileName, unsigned char **buffer, int &w, int &h);

	//---------------------------------------------------------------------------*/
	// Purpose: Reads the entire file into a string. If successful, returns
	//			a pointer to memory with data
	// Output : lenFile - file size in bytes
	//---------------------------------------------------------------------------*/
	char* const ReadWholeFileIntoString(const char *fileName, int *lenFile = 0);

	//---------------------------------------------------------------------------*/
	// Purpose: Returns a pointer to the file name in the path to it and the number
	//			of characters in the file name without extension
	//			example : "/fullpath/filename.txt" -> out : filename, chInName = 8
	//---------------------------------------------------------------------------*/
	const char* ExtractFileName(const char* name, int &chInName);

	//---------------------------------------------------------------------------*/
	// Purpose: Returns the number of characters in the file path without its name
	// Output : number of characters in the file path
	//---------------------------------------------------------------------------*/
	int LenghtPath(const char* fileName);
}
