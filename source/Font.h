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

	bool	Create(const char* fontName, const int size);
	bool	AddCharacterRange(const int lowerRange, const int upperRange);
	bool	Build( void );

	bool	AssignCacheForChar(const int wch);

	float const*	GetTexCoords( void ) const
	{
		return m_pCacheItem[m_CurrentIndex].pTexcoords;
	}

	// gets the glyph desc. for a character from the cache
	GlyphDesc_t	const*	GetGlyphDesc( void ) const
	{
		return m_pCacheItem[m_CurrentIndex].pGlyph;
	}

	// gets the glyph desc. for a character from the FreeType
	bool	GetGlyphDesc(int wch_prev, int  wch_next, GlyphDesc_t &desc) const;

	unsigned char const*	GetGlyphBitmap(int wch) const;

	// returns true, if equal the font name and its size
	bool	IsEqualTo(const char *fontName, int fontSize) const;
	
	// returns true, if the symbol is contained in the font
	bool	IsCharInFont(int wch) const;
	
	// returns true, if this range is already contained
	bool	IsRange(const int lowerRange, const int upperRange) const;

	// rturns true, if the font was Created/Built and ready for use
	bool	IsValid( void ) const;

	/*
		Data accessors
	*/

	// gets the height of the line of the font, in pixels
	int		Height( void ) const
	{
		return m_iHeight;
	}

	// returns the number of rows for the font in the texture
	int		NeedTextureLines( void ) const
	{
		return m_iNeedNumLines;
	}

	// ascender + descender
	int		GetAbsoluteValue( void ) const
	{
		return m_iAbsoluteVal;
	}

	// gets the font size
	int		GetSize( void ) const
	{
		return m_size;
	}
	
	// gets the total number of characters in the font
	int		GetNumCharacter( void ) const
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

	void	SetTextureWidth(const int tex_width)
	{
		m_iTextureWidth = tex_width;
	}

	bool	CalculateTextureCoords(const int xoffset, const int yoffset, int width, int height);

	bool	GlyphTexSubImage(const int xoffset, const int yoffset, int width, int height, unsigned char* pRawTex) const;

	bool	DumpCache(const char* path) const;
	bool	InitFromCache(const char* fileName);

private:

	bool	(CFont::*pAssignCacheForChar)(const int wch);
	bool	AssignCacheForUnicodeCharSet(const int wch);
	bool	AssignCacheForAsciCharSet(const int wch);

	int		FindCharInCache(int wch) const;

	bool	AllocateCacheData(void);
	void	FreeCacheData(void);

	void	AssignPointerToCache( void );

private:

	bool		m_bBuild;

	// if a font contains only ASCII characters
	bool		m_bIsAsci;

	char		m_fontPath[256];
	char		m_fontName[32];
	
	int			m_size;

	int			m_iNumCharacter;

	int			m_iHeight;

	int			m_iTextureWidth;

	int			m_iNeedNumLines;

	int			m_iAbsoluteVal;	// ascender + descender

	// access to functionality FreeType
	ftLib::FTFace *m_pFT_Face;

	struct  UCharacterRange
	{
		int lowRange;	// low range value
		int	upperRange;	// high range value
		int chOffset;	// number of characters before the next range
	};

	CUtlVector<UCharacterRange>	m_UChRanges;
	int			m_iNumRange;

	struct  CaheItem_t	// cache group data
	{
		GlyphDesc_t *pGlyph;
		float		*pTexcoords;
	};

	CaheItem_t	*m_pCacheItem;

	// current cache index for a character
	int			m_CurrentIndex;

	// pointer to the cached glyphs data
	GlyphDesc_t *m_pGlyphData;

	// pointer to the UVs data
	float		*m_pTexCoords;
};
