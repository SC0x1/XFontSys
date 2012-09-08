/****************************************************************************/
/*	Copyright (c) 2012 Vitaly Lyaschenko < scxv86@gmail.com >
/*
/*	Purpose: font manager
/*
/****************************************************************************/

#pragma once

#include "public/utlvector.h"
#include "FontSystem.h"

// Forward declarations
class CFont;
struct GlyphDesc_t;

class FontManager
{
public:
	FontManager();
	~FontManager();

	HFont Create_Font(const char* fontName, int size);

	bool AddGlyphSetToFont(HFont handle, const int lowRange, const int upperRange);
	bool BuildAllFonts( void );

	bool BuildCacheFonts( void );

	void ClearAllFonts( void );

	void GetGlyphDesc(HFont handle, wchar_t wch, GlyphDesc_t &desc) const;

	int GetFontHeight(HFont handle) const;
	
	bool DumpFontCache(HFont handle, const char* path) const;
	HFont LoadFontCache(const char* filename);

	// TODO: modify /~! out into a separate class CFontTextureCache
	void GetTextureID(unsigned int &id)
	{
		id = m_texID;
	}

	bool IsCharInFont(HFont handle, wchar_t wch) const;
	CFont*  GetFontByID(HFont handle) const;

private:

	CUtlVector<CFont*> m_Fonts;
	int m_Numfonts;

	uint32 m_texID;

	bool m_bIsBuildAllFonts;

};

extern FontManager& g_pFontManager;
