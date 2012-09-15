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

void UploadFontTextureToGL(int w, int h, unsigned char* pRawTex, GLuint &idTex);

FontManager::FontManager()
 : m_bIsBuildAllFonts(false)
{
	m_Fonts.Resize(4, 2);

	m_texID = 0;
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

	HFont h = 0;
	m_Fonts.AddToTail( &h ); // h == number of elements in the vector

	const int index = h - 1;

	m_Fonts[ index ] = new CFont;

	if (!m_Fonts[ index ]->Create(fontName, size))
	{
		fprintf(stderr, "\nWrong create a font %s", fontName);
		m_Fonts.RemoveIndex( index );
		delete m_Fonts[ index ];
		return INVALID_FONT;
	}

	return h;
}

bool FontManager::AddGlyphSetToFont(HFont handle, int lowRange, int upperRange)
{
	int index = handle - 1;
	if( (m_Fonts.Num() - 1) < index)
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

	const int tW = FONT_TEXTURE_WIDTH;

	int tH = 0;

	const int num_fonts = m_Fonts.Num();

	for (int i = 0; i < num_fonts; ++i)
	{
		m_Fonts[i]->SetTextureWidth(FONT_TEXTURE_WIDTH);

		if( m_Fonts[i]->Build() ) {
			tH += m_Fonts[i]->Height() * m_Fonts[i]->NeedTextureLines();
		}
		else
		{
			ClearAllFonts();
			return false;
		}
	}

	// tex. dimens.
	const size_t size_tex = tW * tH;
	// allocate memory for texture
	unsigned char* pRawTexture = (unsigned char*)malloc(size_tex);

	memset(pRawTexture, 0, sizeof(unsigned char) * size_tex);

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
		glDeleteTextures(1, &m_texID);
	}
}

bool FontManager::GetGlyphDesc(HFont handle, int wch, GlyphDesc_t &desc) const
{
	if (! m_Fonts[ handle - 1 ]->GetGlyphDesc(wch, desc))
		return false;

	return true;
}

bool FontManager::AssignCacheForChar(HFont handle, int wch)
{
	if (!m_Fonts[ handle - 1]->AssignCacheForChar(wch))
		return false;

	return true;
}

GlyphDesc_t const * FontManager::GetGlyphDescFromCache(HFont handle, int wch) const
{
	return m_Fonts[ handle - 1 ]->GetGlyphDesc();
}

bool FontManager::IsCharInFont(HFont handle, wchar_t wch) const
{
	assert( m_Fonts[ handle-1 ]->IsValid() );
	return m_Fonts[ handle-1 ]->IsCharInFont( wch );
}

CFont* FontManager::GetFontByID(HFont fnt) const
{
	assert(m_Fonts[ fnt - 1 ]);

	return m_Fonts[ fnt - 1 ];
}

int FontManager::GetFontHeight(HFont handle) const
{
	return m_Fonts[ handle - 1 ]->Height();
}

bool FontManager::HasKerning(HFont handle) const
{
	return m_Fonts[ handle - 1 ]->HasKerning();
}

float const * FontManager::GetTexCoordsFromCache(HFont handle, int wch) const
{
	return m_Fonts[ handle - 1 ]->GetTexCoords();
}

bool FontManager::DumpFontCache(HFont handle, const char* path) const
{
	const int index = handle - 1;

	const int num_fonts = m_Fonts.Num();

	if ( (num_fonts - 1) < index )
	{
		return false;
	}

	return m_Fonts[ index ]->DumpCache(path);
}

HFont FontManager::LoadFontCache(const char* filename)
{
	if (!filename)
		return INVALID_FONT;

	HFont h = INVALID_FONT;

	m_Fonts.AddToTail( &h );
	
	const int index = h - 1;

	m_Fonts[ index ] = new CFont;

	if ( !m_Fonts[ index ]->InitFromCache(filename) )
	{
		m_Fonts.RemoveIndex( index );
		delete m_Fonts[ index ];
		return INVALID_FONT;
	}

	return h;
}

bool FontManager::BuildCacheFonts( void )
{
	if (m_bIsBuildAllFonts)
		return false;

	int tH = 0;
	const int tW = FONT_TEXTURE_WIDTH;
	char gName[256];

	const int num_fonts = m_Fonts.Num();

	if ( num_fonts <= 0 )
	{
		return false;
	}

	for (int i = 0; i < num_fonts; ++i)
	{
		if ( !m_Fonts[i]->IsValid() )
		{
			return false;
		}

		m_Fonts[i]->SetTextureWidth(FONT_TEXTURE_WIDTH);
		tH += m_Fonts[i]->Height() * m_Fonts[i]->NeedTextureLines();
	}

	const int size_tex = tW * tH;
	unsigned char* pTexBase = (unsigned char*)malloc(size_tex);

	memset(pTexBase, 0, size_tex);

	int texDimY = 0;

	for (int i = 0; i < num_fonts; ++i)
	{
		CFont &font = *m_Fonts[i];

		font.CalculateTextureCoords(0, texDimY, tW, tH);

		sprintf(gName, "%s/%s_%d.tga", font.GetPath(), font.GetName(), font.GetSize());

		unsigned char * pTexOffset = pTexBase + ( texDimY * tW );

		int w, h;
		if ( !util::LoadTGA(gName, &pTexOffset, w, h) )
		{
			// just release the memory and exit
			free(pTexBase);
			return m_bIsBuildAllFonts = false;
		}

		assert(w == tW);

		texDimY += font.Height() * font.NeedTextureLines();
	}

	//util::SaveAsTGA("ShowMeResult.tga", tW, tH, pTexBase);

	UploadFontTextureToGL(tW, tH, pTexBase, m_texID);

	free(pTexBase);

	return m_bIsBuildAllFonts = true;
}

// Singleton
static FontManager s_fntMng;
extern FontManager& g_pFontManager = s_fntMng;

void UploadFontTextureToGL(int w, int h, unsigned char* pRawTex, GLuint &idTex)
{
	if (!pRawTex)
		return;

	glGenTextures(1, &idTex);

	glActiveTexture(GL_TEXTURE0);

	glBindTexture(GL_TEXTURE_2D, idTex);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, w, h, 0, GL_RED, GL_UNSIGNED_BYTE, pRawTex);

	glBindTexture(GL_TEXTURE_2D, 0);
}