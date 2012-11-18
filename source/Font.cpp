/****************************************************************************/
/*	Copyright (c) 2012 Vitaly Lyaschenko < scxv86@gmail.com >
/*
/*	Purpose: Implementation of the CFont class.
/*
/****************************************************************************/

#include "Font.h"
#include "FontGlobal.h"

#include "FT_Lib.h"

#include "public/common.h"

CFont::CFont(void)
: m_bBuild(false), m_bIsAsci(false)
{
	m_fontName[0] = 0;
	
	m_size = 0;

	m_iNumCharacter = 0;

	m_iNeedNumLines = 0;

	m_iNumRange = 0;
	m_pFT_Face = nullptr;
	m_pGlyphData = nullptr;

	m_iTextureWidth = 0;

	m_CurrentIndex = -1;

	// allocate memory for the two ranges
	m_UChRanges.Resize(2, 2);
}

CFont::~CFont(void)
{
	FreeCacheData();

	m_UChRanges.Clear();

	if (m_pFT_Face)
		delete m_pFT_Face;
}

/*---------------------------------------------------------------------------*/
/* Creates a new font face. Returns false if font does not exist
/*---------------------------------------------------------------------------*/
bool CFont::Create(const char* fontName, const int size)
{
	if (!fontName || ((size < 7) || (size > 128)))
		return false;

	m_size = size;

	m_pFT_Face = new ftLib::FTFace;

	if (!m_pFT_Face->CreateFace(fontName))
	{
		delete m_pFT_Face;
		m_pFT_Face = nullptr;
		return false;
	}

	m_pFT_Face->SetPixelSize(m_size);
	
	int nch;
	const char *name = util::ExtractFileName(fontName, nch);
	strncpy(m_fontName, name, nch);
	m_fontName[nch] = '\0';

	nch = util::LenghtPath(fontName);
	strncpy(m_fontPath, fontName, nch);
	m_fontPath[nch] = '\0';

	return true;
}

/*---------------------------------------------------------------------------*/
/* Adds glyphs to a font
/*---------------------------------------------------------------------------*/
bool CFont::AddCharacterRange(const int lowRange, const int upperRange)
{
	if (!m_fontName[0] || m_bBuild || m_bIsAsci)
		return false;

	const int numChars = upperRange - lowRange;

	if (numChars <= 0)
		return false;

	UCharacterRange range;
	range.lowRange = lowRange;
	range.upperRange = upperRange;
	range.chOffset = m_iNumCharacter;

	m_UChRanges.Append(range);

	++m_iNumRange;

	m_iNumCharacter += numChars;

	return true;
}

bool CFont::AllocateCacheData()
{
	if (m_iNumCharacter <= 0)
		return false;

	if (!(m_pGlyphData = new GlyphDesc_t[m_iNumCharacter]))
		return false;

	return true;
}

void CFont::FreeCacheData(void)
{
	if (m_pGlyphData)
		delete[] m_pGlyphData;
}

bool CFont::Build( void )
{
	if (!m_fontName[0] || m_bBuild)
		return false;

	if (m_iNumRange == 1)
	{
		// if we have only one range of characters, check it is ASCII range
		if (!IsRange(0, 127)) {
			return false;
		}
		pAssignCacheForChar = &CFont::AssignCacheForAsciCharSet;
		m_bIsAsci = true;
	} else {
		pAssignCacheForChar = &CFont::AssignCacheForUnicodeCharSet;
		m_bIsAsci = false;
	}

	if ( !AllocateCacheData() )
	{
		FreeCacheData();
		return false;
	}

	short offsetX = 0, heightLine = 0, numLines = 1;

	for (int nRange = 0; nRange < m_iNumRange; ++nRange)
	{
		const int lowRange = m_UChRanges[nRange].lowRange;
		const int upperRange = m_UChRanges[nRange].upperRange;
		const int charOffset = m_UChRanges[nRange].chOffset;

		for (int chId = lowRange, i = 0; chId < upperRange;  ++chId, ++i)
		{
			GlyphDesc_t &g = m_pGlyphData[ charOffset + i ];

			// gets data from the FreeType
			if (!GetGlyphDesc(chId, g))
			{
				fprintf(stderr, "\nFailed loading char: %s\n", (wchar_t)chId);
				continue;
			}

			if ((offsetX + g.bitmapWidth + 1) >= m_iTextureWidth)
			{
				offsetX = 0;
				++numLines;
			}

			offsetX += g.bitmapWidth + 1;
			heightLine = Max(heightLine, g.bitmapHeight);
		}
	}

	m_iHeight = heightLine;
	m_iNeedNumLines = numLines;

	const int m_Ascender = m_pFT_Face->GlyphAscender();
	const int m_Descender = m_pFT_Face->GlyphDescender();

	m_iAbsoluteVal = m_Ascender + m_Descender;

	return m_bBuild = true;
}

bool CFont::IsValid( void ) const
{
	if (m_fontName[0] && m_bBuild) {
		return true;
	}

	return false;
}

bool CFont::IsEqualTo(const char* fontName, int fontSize) const
{
	int nch;
	const char* name = util::ExtractFileName(fontName, nch);

	if (!strncmp(name, m_fontName, nch) && m_size == fontSize) {
		return true;
	}

	return false;
}

unsigned char const* CFont::GetGlyphBitmap(wchar_t wch) const
{
	if ( !IsValid() ) {
		return nullptr;
	}

	if (!m_pFT_Face->RenderGlyph(wch))
	{
		fprintf(stderr, "\nFailed loading char: %s\n", wch);
		return nullptr;
	}

	return m_pFT_Face->GlyphBitmap();
}

bool CFont::CalculateTextureCoords(const int xoffset, const int yoffset, const int width, const int height)
{
	if (!m_fontName[0] || !m_bBuild)
		return false;

	int iOffsetX = xoffset;
	int iOffsetY = yoffset;

	int inc = 0;

	for (int nRange = 0; nRange < m_iNumRange; ++nRange)
	{
		const int lowRange = m_UChRanges[nRange].lowRange;
		const int upperRange = m_UChRanges[nRange].upperRange;
		const int charOffset = m_UChRanges[nRange].chOffset;

		for (int chId = lowRange, i = 0; chId < upperRange; ++chId, ++i)
		{
			GlyphDesc_t &g = m_pGlyphData[ charOffset + i ];
			
			if ((iOffsetX + g.bitmapWidth) >= width)
			{
				iOffsetY += m_iHeight;
				iOffsetX = 0;
			}

			if ((iOffsetY + g.bitmapHeight) > height)
				return false;

			g.s = (float)iOffsetX / (float)width;
			g.t = (float)iOffsetY / (float)height;
			g.s2 = (float)g.bitmapWidth / (float)width;
			g.t2 = (float)g.bitmapHeight / (float)height;

			iOffsetX += g.bitmapWidth + 1;
		}
	}

	return true;
}

bool CFont::GlyphTexSubImage(const int xoffset, const int yoffset, int width, int height, unsigned char* pRawTex) const
{
	if ((!m_fontName[0]) || (!m_bBuild) || (!pRawTex))
		return false;

	assert(m_pFT_Face);

	int iOffsetX = xoffset;
	int iOffsetY = yoffset;

	for (int nRange = 0; nRange < m_iNumRange; ++nRange)
	{
		const int lowRange = m_UChRanges[nRange].lowRange;
		const int upperRange = m_UChRanges[nRange].upperRange;
		const int charOffset = m_UChRanges[nRange].chOffset;

		for (int chId = lowRange, i = 0; chId < upperRange; ++chId, ++i)
		{
			GlyphDesc_t &g = m_pGlyphData[ charOffset + i ];

			if ((iOffsetX + g.bitmapWidth) >= width)
			{
				iOffsetY += m_iHeight;
				iOffsetX = 0;
			}

			if ((iOffsetY + g.bitmapHeight) > height)
				return false;

			const unsigned char *pBitMap = this->GetGlyphBitmap((wchar_t)chId);

			for (int row = 0; row < g.bitmapHeight; ++row)
			{
				for (int pxl = 0; pxl < g.bitmapWidth; ++pxl)
				{
					int pxlIndex = (pxl + iOffsetX) + ((row  + iOffsetY) * width);

					pRawTex[pxlIndex] = pBitMap[pxl + row * g.bitmapWidth];
				}
			}

			iOffsetX += g.bitmapWidth + 1;
		}
	}

	return true;
}

bool CFont::AssignCacheForUnicodeCharSet(const wchar_t wch)
{
	if ( (m_CurrentIndex = FindCharInCache( wch )) != -1) {
		return true;
	}

	m_CurrentIndex = 0;

	return false;
}

bool CFont::AssignCacheForAsciCharSet(const wchar_t wch)
{
	if ( (unsigned int)wch < 127 )
	{
		m_CurrentIndex = wch;
		return true;
	}

	m_CurrentIndex = 0;

	return false;
}

int CFont::FindCharInCache(wchar_t wch) const
{
	for (int i = 0; i < m_iNumRange; ++i)
	{
		if ((wch >= m_UChRanges[i].lowRange) && (wch <= m_UChRanges[i].upperRange))
		{
			// returns the index in the cache
			return m_UChRanges[i].chOffset + (wch - m_UChRanges[i].lowRange);
		}
	}
	return -1;
}

bool CFont::GetGlyphDesc(wchar_t wch, GlyphDesc_t &desc) const
{
	if (!m_pFT_Face || !m_pFT_Face->RenderGlyph(wch)) {
		return false;
	}

	ftLib::FTFace &f = *m_pFT_Face;

	desc.advanceX = f.GlyphAdvanceX();
	desc.advanceY = f.GlyphAdvanceY();

	desc.bitmapWidth = f.GlyphBitmapWidth();
	desc.bitmapHeight = f.GlyphBitmapHeight();
	desc.bitmapLeft = f.GlyphBitmapLeft();
	desc.bitmapTop = f.GlyphBitmapTop();

	desc.glyphID = wch;

	return true;
}

GlyphDesc_t const * CFont::GetGlyphDesc(void) const
{
	return &m_pGlyphData[m_CurrentIndex];
}

bool CFont::HasKerning(void)
{
	return m_pFT_Face->HasKerning();
}

int CFont::GetKerningX(wchar_t wch_prev, wchar_t wch_next) const
{
	if (!m_pFT_Face)
		return 0;

	return m_pFT_Face->GetKerningX(wch_prev, wch_next);
}

int CFont::GetKerningY(wchar_t wch_prev, wchar_t wch_next) const
{
	if (!m_pFT_Face)
		return 0;

	return m_pFT_Face->GetKerningY(wch_prev, wch_next);
}

struct FontInfo_s 
{
	char fontName[64];
	int size;
	int numRanges;
	int absoluteValue;
	int maxTextureWidth;
};

bool CFont::InitFromCache(const char* fileName)
{
	if ( m_fontName[0] )
		return false;

	FILE* pFile = fopen(fileName, "rb");

	if ( !pFile )
		return false;

	FontInfo_s info;

	fread(&info, 1, sizeof(FontInfo_s), pFile);

	strcpy(m_fontName, info.fontName);

	if (!SetFontTextureWidth(info.maxTextureWidth))
	{
		fclose(pFile);
		return false;
	}

	m_iTextureWidth = info.maxTextureWidth;

	int nch = util::LenghtPath(fileName);
	strncpy(m_fontPath, fileName, nch);
	m_fontPath[nch] = '\0';

	m_size = info.size;

	// if we have only one range
	if (info.numRanges == 1)
	{
		UCharacterRange rng;
		fread(&rng, 1, sizeof(UCharacterRange), pFile);

		// If this range is not ASCII
		if (rng.lowRange != 0 || rng.upperRange != 127)
		{
			// just close the file and return FAIL
			fclose(pFile);
			return false;
		}

		AddCharacterRange(0, 127);

		pAssignCacheForChar = &CFont::AssignCacheForAsciCharSet;

		m_bIsAsci = true;

	} else {

		for (int i = 0; i < info.numRanges; ++i)
		{
			UCharacterRange rng;
			fread(&rng, 1, sizeof(UCharacterRange), pFile);

			AddCharacterRange(rng.lowRange, rng.upperRange);
		}

		pAssignCacheForChar = &CFont::AssignCacheForUnicodeCharSet;
	}

	if (!AllocateCacheData())
	{
		FreeCacheData();
		return false;
	}

	fread(m_pGlyphData, 1, sizeof(GlyphDesc_t) * m_iNumCharacter, pFile);

	short offsetX = 0, heightLine = 0, numLines = 1;

	for (int nRange = 0; nRange < m_iNumRange; ++nRange)
	{
		const int lowRange = m_UChRanges[nRange].lowRange;
		const int upperRange = m_UChRanges[nRange].upperRange;
		const int charOffset = m_UChRanges[nRange].chOffset;

		for (int chId = lowRange, i = 0; chId < upperRange;  ++chId, ++i)
		{
			GlyphDesc_t &g = m_pGlyphData[charOffset + i];

			if ((offsetX + g.bitmapWidth) >= m_iTextureWidth)
			{
				offsetX = 0;
				++numLines;
			}

			offsetX += g.bitmapWidth + 1;
			heightLine = Max(heightLine, g.bitmapHeight);
		}
	}

	m_iHeight = heightLine;
	m_iNeedNumLines = numLines;

	m_iAbsoluteVal = info.absoluteValue;

	return m_bBuild = true;
}

bool CFont::DumpCache(const char* path) const
{
	if ( !path 	|| !m_fontName[0] || !m_bBuild )
		return false;

	FontInfo_s info;
	char buffer[512];

	// copy the font information
	strcpy(info.fontName, m_fontName);
	info.size = m_size;
	info.numRanges = m_iNumRange;
	info.absoluteValue = m_iAbsoluteVal;
	info.maxTextureWidth = m_iTextureWidth;

	sprintf(buffer, "%s/%s_%d.cfnt", path, m_fontName, info.size);

	FILE* pFile = fopen(buffer, "wb");

	if ( !pFile )
	{
		fprintf(stderr, "\nError open file for write: %s", buffer);
		return false;
	}

	fwrite(&info, 1, sizeof(FontInfo_s), pFile);

	for (int nr = 0; nr < info.numRanges; ++nr)
	{
		const UCharacterRange &rng = m_UChRanges[nr];

		fwrite(&rng, 1, sizeof(UCharacterRange), pFile);
	}

	fwrite(m_pGlyphData, 1, sizeof(GlyphDesc_t) * m_iNumCharacter, pFile);

	fclose(pFile);

	const int tH = m_iHeight * m_iNeedNumLines;
	const int tW = cfg::FONT_TEXTURE_WIDTH;

	const size_t size_tex = tW * tH;
	unsigned char* pRawTexture = (uint8*)malloc(size_tex);

	memset(pRawTexture, 0, sizeof(uint8) * size_tex);

	GlyphTexSubImage(0, 0, tW, tH, pRawTexture);

	sprintf(buffer, "%s/%s_%d.tga", path, m_fontName, m_size);

	if( !util::SaveAsTGA(buffer, tW, tH, pRawTexture) )
	{
		free(pRawTexture);
		return false;
	}

	free(pRawTexture);

	return true;
}

bool CFont::IsRange(const int lowerRange, const int upperRange) const
{
	for (int i = 0; i < m_iNumRange; ++i)
	{
		if ((lowerRange >= m_UChRanges[i].lowRange) && 
			(upperRange <= m_UChRanges[i].upperRange))
			return true;
	}
	return false;
}

bool CFont::IsCharInFont(wchar_t wch) const
{
	if ( FindCharInCache(wch) != -1) {
		return true;
	}
	return false;
}
