/****************************************************************************/
/*	Copyright (c) 2012 Vitaly Lyaschenko < scxv86@gmail.com >
/*
/*	Purpose: Implement font manager
/*
/****************************************************************************/

#include "FontManager.h"
#include "FontGlobal.h"

#include "shaders/gl_local.h"
#include "Font.h"

#include "public/common.h"

#define Handle2Index( x ) (x - 1)

FontManager::FontManager()
 : m_bIsBuildAllFonts(false), m_texID(0)
{
	m_Fonts.Resize(3, 2);
}

FontManager::~FontManager()
{ }

HFont FontManager::Create_Font(const char* fontName, int size)
{
	// check whether there is already a font with the same name and size
	for (int i = 0; i < m_Fonts.Num(); ++i)
	{
		if (m_Fonts[i]->IsEqualTo(fontName, size))
			return i + 1;
	}

	const int index = m_Fonts.Append( new CFont );

	if (!m_Fonts[ index ]->Create(fontName, size))
	{
		fprintf(stderr, "\nWrong create a font %s", fontName);
		m_Fonts.RemoveIndex( index );
		delete m_Fonts[ index ];
		return INVALID_FONT;
	}

	return index + 1;
}

bool FontManager::AddGlyphSetToFont(HFont handle, int lowRange, int upperRange)
{
	int index( Handle2Index(handle) );

	if( ( Handle2Index( m_Fonts.Num() ) ) < index)
		return false;

	// if we have something to add
	if ((upperRange - lowRange) <= 0)
		return false;

	if (m_Fonts[index]->IsRange(lowRange, upperRange)) {
		return false;
	}

	// add a new range of characters
	if (!m_Fonts[index]->AddCharacterRange(lowRange, upperRange))
		return false;

	return true;
}

bool FontManager::BuildAllFonts( void )
{
	if (m_bIsBuildAllFonts)
		return false;

	const int tW = cfg::FONT_TEXTURE_WIDTH;

	int tH = 0;

	const int num_fonts = m_Fonts.Num();

	for (int i = 0; i < num_fonts; ++i)
	{
		m_Fonts[i]->SetTextureWidth(cfg::FONT_TEXTURE_WIDTH);

		if( m_Fonts[i]->Build() ) {
			tH += m_Fonts[i]->Height() * m_Fonts[i]->NeedTextureLines();
		}
		else
		{
			ClearAllFonts();
			return false;
		}
	}

	// allocate memory for texture
	unsigned char* pRawTexture = (unsigned char*)calloc(tW * tH, sizeof(unsigned char));

	if (pRawTexture == nullptr)
	{
		fprintf(stderr, "\nFailed to allocate memory for texture %dx%d", tW, tH);
		ClearAllFonts();
		return false;
	}

	int yoffset = 0;
	for (int i = 0; i < num_fonts; ++i)
	{
		m_Fonts[i]->CalculateTextureCoords(0, yoffset, tW, tH);
		m_Fonts[i]->GlyphTexSubImage(0, yoffset, tW, tH, pRawTexture);

		yoffset += m_Fonts[i]->Height() * m_Fonts[i]->NeedTextureLines();
	}

	//util::SaveAsTGA("ShowMeResult.tga", tW, tH, pRawTexture);

	UploadFontTextureToGL(tW, tH, pRawTexture, m_texID);

	free(pRawTexture);

	return m_bIsBuildAllFonts = true;
}

void FontManager::ClearAllFonts()
{
	m_Fonts.DeleteContents(true);

	m_bIsBuildAllFonts = false;

	if (m_texID != 0)
	{
		FreeGLTexture(1, &m_texID);
	}
}

bool FontManager::GetGlyphDesc(HFont handle, int wch, GlyphDesc_t &desc) const
{
	if (! m_Fonts[ Handle2Index(handle) ]->GetGlyphDesc(wch, desc))
		return false;

	return true;
}

bool FontManager::AssignCacheForChar(HFont handle, int wch)
{
	if (!m_Fonts[ Handle2Index(handle) ]->AssignCacheForChar(wch))
		return false;

	return true;
}

GlyphDesc_t const * FontManager::GetGlyphDescFromCache(HFont handle, int wch) const
{
	return m_Fonts[ Handle2Index(handle) ]->GetGlyphDesc();
}

bool FontManager::IsCharInFont(HFont handle, wchar_t wch) const
{
	assert( m_Fonts[ Handle2Index(handle) ]->IsValid() );
	return m_Fonts[ Handle2Index(handle) ]->IsCharInFont( wch );
}

CFont* FontManager::GetFontByID(HFont handle) const
{
	assert(m_Fonts[ Handle2Index(handle) ]);

	return m_Fonts[ Handle2Index(handle) ];
}

int FontManager::GetFontHeight(HFont handle) const
{
	return m_Fonts[ Handle2Index(handle) ]->Height();
}

bool FontManager::HasKerning(HFont handle) const
{
	return m_Fonts[ Handle2Index(handle) ]->HasKerning();
}

bool FontManager::DumpFontCache(HFont handle, const char* path) const
{
	const int index( Handle2Index(handle) );

	if ( ( Handle2Index( m_Fonts.Num() ) ) < index ) {
		return false;
	}

	return m_Fonts[ index ]->DumpCache(path);
}

HFont FontManager::LoadFontCache(const char* filename)
{
	if (!filename)
		return INVALID_FONT;

	const int index = m_Fonts.Append( new CFont );

	if ( !m_Fonts[ index ]->InitFromCache(filename) )
	{
		m_Fonts.RemoveIndex( index );
		delete m_Fonts[ index ];
		return INVALID_FONT;
	}

	return index + 1;
}

bool FontManager::BuildCacheFonts( void )
{
	if (m_bIsBuildAllFonts)
		return false;

	int tH = 0;
	const int tW = cfg::FONT_TEXTURE_WIDTH;
	char gName[256];

	const int num_fonts = m_Fonts.Num();

	if ( num_fonts <= 0 ) {
		return false;
	}

	for (int i = 0; i < num_fonts; ++i)
	{
		if ( !m_Fonts[i]->IsValid() ) {
			return false;
		}

		m_Fonts[i]->SetTextureWidth(cfg::FONT_TEXTURE_WIDTH);
		tH += m_Fonts[i]->Height() * m_Fonts[i]->NeedTextureLines();
	}

	// allocate memory for texture
	unsigned char* pRawTexture = (unsigned char*)calloc(tW * tH, sizeof(unsigned char));

	if (pRawTexture == nullptr)
	{
		fprintf(stderr, "\nFailed to allocate memory for texture %dx%d", tW, tH);
		ClearAllFonts();
		return false;
	}

	int texDimY = 0;

	for (int i = 0; i < num_fonts; ++i)
	{
		CFont &font = *m_Fonts[i];

		font.CalculateTextureCoords(0, texDimY, tW, tH);

		sprintf(gName, "%s/%s_%d.tga", font.GetPath(), font.GetName(), font.GetSize());

		unsigned char * pTexOffset = pRawTexture + ( texDimY * tW );

		int w(0), h(0);
		if ( !util::LoadTGA(gName, &pTexOffset, w, h) )
		{
			// just release the memory and exit
			free(pRawTexture);
			return m_bIsBuildAllFonts = false;
		}

		assert(w == tW);

		texDimY += font.Height() * font.NeedTextureLines();
	}

	//util::SaveAsTGA("ShowMeResult.tga", tW, tH, pTexBase);

	UploadFontTextureToGL(tW, tH, pRawTexture, m_texID);

	free(pRawTexture);

	return m_bIsBuildAllFonts = true;
}

// Singleton
static FontManager s_fntMng;
extern FontManager& g_pFontManager = s_fntMng;
