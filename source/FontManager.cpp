// Copyright © 2013 Vitaly Lyaschenko (scxv86@gmail.com)
// Purpose:
//
#include <assert.h>

#include "Font.h"
#include "FontManager.h"

using namespace xfs;

CFontManager::CFontManager(void)
    : bIsBuildFonts_(false)
{
    fonts_.Resize(3, 2);
}

CFontManager::~CFontManager() {}

HFont CFontManager::Create_Font(const char* fontName, int size)
{
    // check whether there is already a font with the same name and size
    for (int i = 0; i < fonts_.Count(); ++i)
    {
        if (fonts_[i]->IsFontEqual(fontName, size))
            return i + 1;
    }
    const int idx = fonts_.Append(new CFont);
    if (!fonts_[idx]->Create(fontName, size))
    {
        fprintf(stderr, "\nWrong create a font %s", fontName);
        fonts_.RemoveIdx(idx);
        delete fonts_[idx];
        return INVALID_FONT;
    }
    return idx + 1;
}

bool CFontManager::AddGlyphSetToFont(HFont hdl, int lower, int upper)
{
    const int idx(hdl-1);
    if ((fonts_.Count()-1) < idx)
        return false;

    if ((upper - lower) <= 0) // if we have something to add
        return false;

    if (fonts_[idx]->IsRange(lower, upper))
        return false;

    if (!fonts_[idx]->AddCharacterRange(lower, upper))
        return false;

    return true;
}

bool CFontManager::BuildFonts(void)
{
    if (bIsBuildFonts_)
        return false;

    for(int i = 0; i < fonts_.Count(); ++i)
    {
        if (!fonts_[i]->Build())
        {
            ReleaseAllFonts();
            return false;
        }
    }
    return bIsBuildFonts_ = true;
}

bool CFontManager::GetGlyph(HFont hdl, int ch, GlyphDesc& desc) const
{
    if (!fonts_[hdl-1]->GetGlyph(ch, desc))
        return false;

    return true;
}

const GlyphDesc* CFontManager::GetGlyphFromCache(HFont hdl, int ch) const
{
    return fonts_[hdl-1]->GetGlyphFromCache(ch);
}

bool CFontManager::IsCharInFont(HFont hdl, wchar_t ch) const
{
    assert(fonts_[hdl-1]->IsValid());
    return fonts_[hdl-1]->IsCharInFont(ch);
}

CFont* CFontManager::GetFontByHandle(HFont hdl) const
{
    assert(fonts_[hdl-1]);
    return fonts_[hdl-1];
}

int CFontManager::GetFontHeight(HFont hdl) const
{
    return fonts_[hdl-1]->Height();
}

bool CFontManager::HasKerning(HFont hdl) const
{
    return fonts_[hdl-1]->HasKerning();
}

int CFontManager::GetHeightFontTexture(void) const
{
    assert(bIsBuildFonts_);
    int texHeight(0);
    for (int i = 0; i < fonts_.Count(); ++i)
    {
        if (!fonts_[i]->IsValid())
            return -1;

        texHeight += fonts_[i]->Height() * fonts_[i]->NumNeedTextureLines();
    }

    return texHeight;
}

bool CFontManager::MakeFontTexture(int texWidth, int texHeight, unsigned char** ppTex) const
{
    assert(bIsBuildFonts_);
    assert(*ppTex);
    assert((texWidth > 0) && (texHeight > 0));

    int offsetY(0);
    for (int i = 0; i < fonts_.Count(); ++i)
    {
        if (!fonts_[i]->IsValid())
            return false;

        fonts_[i]->CalcTexCoords(0, offsetY, texWidth, texHeight);
        fonts_[i]->GlyphTexSubImage(0, offsetY, texWidth, texHeight, *ppTex);
        offsetY += fonts_[i]->Height() * fonts_[i]->NumNeedTextureLines();
    }
    return true;
}