/****************************************************************************/
/*	Copyright (c) 2012 Vitaly Lyaschenko < scxv86@gmail.com >
/*
/*	Purpose: some definitions, declarations and constants
/*
/****************************************************************************/

#pragma once

//  glyph metrices
struct GlyphDesc_t
{
	// id in unicode table
	int	glyphID;

	short advanceX;
	short advanceY;

	short bitmapWidth;
	short bitmapHeight;

	short bitmapLeft;
	short bitmapTop;
};

struct UnicodeCharRange_t
{
	const char *name;
	int lowerRange;
	int upperRange;
};

// globals ...
extern	int DYNAMIC_CHARS;      // 256 define in FontSystem.cpp
const	int MIN_DYNAMIC_CHARS	= 0;
const	int MAX_DYNAMIC_CHARS	= 50000;

extern	int STATIC_CHARS;       // 4096 define in FontSystem.cpp
const	int MIN_STATIC_CHARS	= 0;
const	int MAX_STATIC_CHARS	= 50000;

extern	int FONT_TEXTURE_WIDTH; // 1024 define in FontManager.cpp

const	int MIN_TEXTURE_WIDTH	= 128;
const	int MAX_TEXTURE_WIDTH	= 4096;

