// Copyright (c) 2013 Vitaly Lyaschenko (scxv86@gmail.com)
// Purpose: Misc helper functions
//
#ifndef utils_h__
#define utils_h__

#pragma once

#define INLINE inline
#define FORCEINLINE __forceinline

namespace utils
{
	template<typename T>
	INLINE T Max(const T& a, const T& b)
	{
		return (a > b) ? a : b;
	}
	template<typename T>
	INLINE T Min(const T& a, const T& b)
	{
		return !(a > b) ? a : b;
	}

	bool TGA_Save(const char* fileName, int width, int height, unsigned char const* pData);
	bool TGA_Load(const char* fileName, unsigned char** buffer, int& w, int& h);

	int ExtractFileName(const char* path, char* buffer);
	int ExtractFilePath(const char* path, char* buffer);

	int LenghtPath(const char* path);
	void FixSlashes(char* str);

	bool IsWSpace(int w);
}

#endif // utils_h__