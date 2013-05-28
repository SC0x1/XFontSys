// Copyright © 2013 Vitaly Lyaschenko (scxv86@gmail.com)
// Purpose: 
//
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "public/utils.h"
#include "xfontConf.h"
#include "Font.h"

using namespace xfs;

CFont::CFont()
    : bBuild_(false),
      bAsciOnly_(false),
      fontHeight_(0),
      numChars_(0),
      numNeedLines_(0), 
      pGlyphData_(nullptr),
      currIdxCache_(0)
{
    fontName_[0] = 0;
    chRanges_.Resize(2, 2);	// allocate memory for the two ranges
}

CFont::~CFont()
{
    FreeCacheData();
    chRanges_.Clear();
}
//---------------------------------------------------------------------------
// Creates a new font face. Returns false if font does not exist
//---------------------------------------------------------------------------
bool CFont::Create(const char* file, int size)
{
    if (!file || ((size < 7) || (size > 128)))
        return false;

    fontHeight_ = size;
    if (!ftFace_.Create(file))
        return false;

    ftFace_.SetPixelSize(fontHeight_);
    utils::ExtractFileName(file, fontName_);
    //utils::ExtractFilePath(file, fontPath_);
    return true;
}
//---------------------------------------------------------------------------
// Add glyphs to a font
//---------------------------------------------------------------------------
bool CFont::AddCharacterRange(int lower, int upper)
{
    if (!fontName_[0] || bBuild_ || bAsciOnly_)
        return false;

    assert((upper - lower) > 0);
    chRanges_.Append(UChRange(lower, upper, numChars_));
    numChars_ += upper - lower;
    return true;
}

bool CFont::AllocateCacheData()
{
    if (numChars_ <= 0)
        return false;

    pGlyphData_ = new GlyphDesc[numChars_];
    if (!pGlyphData_)
        return false;

    return true;
}

void CFont::FreeCacheData()
{
    if (pGlyphData_)
        delete[] pGlyphData_;
}

bool CFont::Build()
{
    if (!fontName_[0] || bBuild_)
        return false;

    if (chRanges_.Count() == 1)
    {
        // if we have only one range of characters, check it is ASCII range
        if (!IsRange(0, 127))
            return false;

        pAssignCacheForChar = &CFont::AssignCacheForAsciCharSet;
        bAsciOnly_ = true;
    }
    else
    {
        pAssignCacheForChar = &CFont::AssignCacheForUnicodeCharSet;
        bAsciOnly_ = false;
    }
    if (!AllocateCacheData())
    {
        FreeCacheData();
        return false;
    }
    short offsetX(0);
    short heightLine(0);
    short numLines(1);
    const int numRanges = chRanges_.Count();
    for (int i(0); i < numRanges; ++i)
    {
        const UChRange& r = chRanges_[i];
        for (int c(r.lower_), i(0); c < r.upper_;  ++c, ++i)
        {
            GlyphDesc& g = pGlyphData_[r.chOffset_ + i];

            // gets data from the FreeType
            if (!GetGlyph(c, g))
            {
                fprintf(stderr, "\nFailed loading char: %s\n", (wchar_t)c);
                continue;
            }
            if ((offsetX + g.bitmapWidth + 1) >= Config::currTextFontWidth)
            {
                offsetX = 0;
                ++numLines;
            }
            offsetX += g.bitmapWidth + 1;
            heightLine = utils::Max(heightLine, g.bitmapHeight);
        }
    }
    lineHeight_ = heightLine;
    numNeedLines_ = numLines;
    absoluteVal_ = ftFace_.GlyphAscender() + ftFace_.GlyphDescender();
    return bBuild_ = true;
}

bool CFont::IsFontEqual(const char* name, int fontSize) const
{
    static char buffer[256];
    const int nch = utils::ExtractFileName(name, buffer);
    if (!strncmp(buffer, fontName_, nch) && fontHeight_ == fontSize)
        return true;

    return false;
}

unsigned char const* CFont::GetGlyphBitmap(wchar_t ch) const
{
    if (!IsValid())
        return nullptr;

    if (!ftFace_.LoadChar(ch))
    {
        fprintf(stderr, "\nFailed loading char: %s\n", ch);
        return nullptr;
    }
    return ftFace_.GlyphBitmapBuffer();
}

bool CFont::CalcTexCoords(int offsetX, int offsetY, int width, int height)
{
    if (!fontName_[0] || !bBuild_)
        return false;

    const int numRanges = chRanges_.Count();
    for (int i(0); i < numRanges; ++i)
    {
        const UChRange& rng = chRanges_[i];
        for (int chId(rng.lower_), i(0); chId < rng.upper_; ++chId, ++i)
        {
            GlyphDesc& g = pGlyphData_[rng.chOffset_ + i];
            if ((offsetX + g.bitmapWidth) >= width)
            {
                offsetY += lineHeight_;
                offsetX = 0;
            }
            if ((offsetY + g.bitmapHeight) > height)
                return false;

            g.s = (float)offsetX / (float)width;
            g.t = (float)offsetY / (float)height;
            g.p = (float)g.bitmapWidth / (float)width;
            g.q = (float)g.bitmapHeight / (float)height;
            offsetX += g.bitmapWidth + 1;
        }
    }
    return true;
}

bool CFont::GlyphTexSubImage(int offsetX, int offsetY, int width, int height, unsigned char* pRawTex) const
{
    if ((!fontName_[0]) || (!bBuild_) || (!pRawTex))
        return false;

    const int numRanges = chRanges_.Count();
    for (int i(0); i < numRanges; ++i)
    {
        const UChRange& r = chRanges_[i];
        for (int chId(r.lower_), i(0); chId < r.upper_; ++chId, ++i)
        {
            GlyphDesc& g = pGlyphData_[r.chOffset_ + i];
            if ((offsetX + g.bitmapWidth) >= width)
            {
                offsetY += lineHeight_;
                offsetX = 0;
            }
            if ((offsetY + g.bitmapHeight) > height)
                return false;

            const unsigned char *pBitMap = this->GetGlyphBitmap((wchar_t)chId);
            for (int ih(0); ih < g.bitmapHeight; ++ih)
            {
                for (int iw(0); iw < g.bitmapWidth; ++iw)
                {
                    const int pxlIdx((iw + offsetX) + ((ih  + offsetY) * width));
                    pRawTex[pxlIdx] = pBitMap[iw + ih * g.bitmapWidth];
                }
            }
            offsetX += g.bitmapWidth + 1;
        }
    }
    return true;
}

bool CFont::GetGlyph(wchar_t ch, GlyphDesc& desc) const
{
    assert(ftFace_.IsValid());
    if (!ftFace_.LoadChar(ch))
        return false;

    desc.advanceX = ftFace_.GlyphAdvanceX();
    desc.advanceY = ftFace_.GlyphAdvanceY();
    desc.bitmapWidth = ftFace_.GlyphBitmapWidth();
    desc.bitmapHeight = ftFace_.GlyphBitmapHeight();
    desc.bitmapLeft = ftFace_.GlyphBitmapLeft();
    desc.bitmapTop = ftFace_.GlyphBitmapTop();
    desc.ch = ch;
    return true;
}

bool CFont::HasKerning()
{
    return ftFace_.HasKerning();
}

int CFont::GetKerningX(wchar_t ch_prev, wchar_t ch_next) const
{
    assert(ftFace_.IsValid());
    return ftFace_.GetKerningX(ch_prev, ch_next);
}

int CFont::GetKerningY(wchar_t ch_prev, wchar_t ch_next) const
{
    assert(ftFace_.IsValid());
    return ftFace_.GetKerningY(ch_prev, ch_next);
}

bool CFont::IsRange(const int lower, const int upper) const
{
    for (int i(0); i < chRanges_.Count(); ++i)
    {
        if ((lower >= chRanges_[i].lower_) && 
            (upper <= chRanges_[i].upper_))
        {
            return true;
        }
    }
    return false;
}

bool CFont::IsCharInFont(wchar_t ch) const
{
    if (FindCharInCache(ch) != -1)
        return true;

    return false;
}
