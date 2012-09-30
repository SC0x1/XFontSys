/****************************************************************************/
/*	Copyright (c) 2012 Vitaly Lyaschenko < scxv86@gmail.com >
/*
/*	Purpose: some definitions, declarations and constants
/*
/****************************************************************************/

#pragma once

// glyph metrices
struct GlyphDesc_t
{
	// id in unicode table
	int glyphID;

	union
	{
		struct
		{ 
			float s;	// x offset in image where glyph starts
			float t;	// y offset in image where glyph starts
			float s2;	
			float t2;
		};
		float texCoord[4];
	};

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
typedef struct FsglCfg_s 
{
	static const int MAX_LENGTH_STRING;

	static int STATIC_CHARS;
	static const int MIN_STATIC_CHARS;
	static const int MAX_STATIC_CHARS;

	static int FONT_TEXTURE_WIDTH;
	static const int MIN_TEXTURE_WIDTH;
	static const int MAX_TEXTURE_WIDTH;
} cfg;
