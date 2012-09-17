/****************************************************************************/
/*	Copyright (c) 2012 Vitaly Lyaschenko < scxv86@gmail.com >
/*
/*	Purpose: The class CFont contains the Unicode character ranges and data
/*			 for these characters ( metrics, tex. coords)
/*
/****************************************************************************/

#pragma once

#include "public/utlvector.h"
#include "FontSystem.h"

// Forward declarations
namespace ftLib {
	class FTFace;
}

class FontManager;
struct GlyphDesc_t;

class CFont
{
	friend class FontManager;

public:
	CFont( void );
	~CFont( void );

	bool Create(const char* fontName, const int size);
	bool AddCharacterRange(const int lowerRange, const int upperRange);
	bool Build( void );

	bool AssignCacheForChar(const wchar_t wch)
	{
		return (this->*pAssignCacheForChar)( wch );
	}

	// gets the glyph desc. for a character from the cache
	GlyphDesc_t const *GetGlyphDesc( void ) const;

	// gets the glyph desc. for a character from the FreeType
	bool GetGlyphDesc(wchar_t wch, GlyphDesc_t &desc) const;
	
	bool HasKerning( void );

	int GetKerningX(wchar_t wch_prev, wchar_t wch_next) const;

	int GetKerningY(wchar_t wch_prev, wchar_t wch_next) const;

	unsigned char const* GetGlyphBitmap(wchar_t wch) const;

	// returns true, if equal the font name and its size
	bool IsEqualTo(const char *fontName, int fontSize) const;
	
	// returns true, if the symbol is contained in the font
	bool IsCharInFont(wchar_t wch) const;
	
	// returns true, if this range is already contained
	bool IsRange(const int lowerRange, const int upperRange) const;

	// rturns true, if the font was Created/Built and ready for use
	bool IsValid( void ) const;

	/*
		Data accessors
	*/

	// gets the height of the line of the font, in pixels
	int Height( void ) const
	{
		return m_iHeight;
	}

	// returns the number of rows for the font in the texture
	int NeedTextureLines( void ) const
	{
		return m_iNeedNumLines;
	}

	// ascender + descender
	int GetAbsoluteValue( void ) const
	{
		return m_iAbsoluteVal;
	}

	// gets the font size
	int GetSize( void ) const
	{
		return m_size;
	}
	
	// gets the total number of characters in the font
	int GetNumCharacter( void ) const
	{
		return m_iNumCharacter;
	}

	// gets the name of this font
	const char* GetName( void ) const
	{
		return m_fontName;
	}

	// gets the full path to this font file
	const char* GetPath( void ) const
	{
		return m_fontPath;
	}

private:

	// Called from a class FontManager //

	void SetTextureWidth(const int tex_width)
	{
		m_iTextureWidth = tex_width;
	}

	bool CalculateTextureCoords(const int xoffset, const int yoffset, int width, int height);

	bool GlyphTexSubImage(const int xoffset, const int yoffset, int width, int height, unsigned char* pRawTex) const;

	bool DumpCache(const char* path) const;
	bool InitFromCache(const char* fileName);

private:

	bool (CFont::*pAssignCacheForChar)(const wchar_t wch);
	bool AssignCacheForUnicodeCharSet(const wchar_t wch);
	bool AssignCacheForAsciCharSet(const wchar_t wch);

	int FindCharInCache(wchar_t wch) const;

	bool AllocateCacheData(void);
	void FreeCacheData(void);

private:

	char m_fontPath[256];
	char m_fontName[32];
	
	int m_size;

	int m_iNumCharacter;

	int m_iHeight;

	int m_iTextureWidth;

	int m_iNeedNumLines;

	int m_iAbsoluteVal;	// ascender + descender

	// access to functionality FreeType
	ftLib::FTFace *m_pFT_Face;

	struct UCharacterRange
	{
		int lowRange;    // low range value
		int upperRange;  // high range value
		int chOffset;    // number of characters before the next range
	};

	CUtlVector<UCharacterRange>	m_UChRanges;
	int m_iNumRange;

	// current cache index for a character
	int m_CurrentIndex;

	// pointer to the cached glyphs data
	GlyphDesc_t *m_pGlyphData;

	bool m_bBuild;

	// if a font contains only ASCII characters
	bool m_bIsAsci;
};
