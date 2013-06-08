// Copyright (c) 2013 Vitaly Lyaschenko (scxv86@gmail.com)
// Purpose: Functional for creation and manipulation of fonts
//
#ifndef FontManager_h__
#define FontManager_h__

#pragma once

#include "public/utlvector.h"
#include "public/singleton.h"
#include "xfontTypes.h"

//===========================================================================
// CFontManager
//===========================================================================
class CFontManager : public Singleton<CFontManager>
{
    friend class Singleton<CFontManager>;
public:
    CFontManager(void);
    ~CFontManager();

    xfs::HFont Create_Font(const char* fontName, int size);
    bool AddGlyphSetToFont(xfs::HFont hdl, int lower, int upper);
    bool BuildFonts(void);
    void ReleaseAllFonts(void);

    int GetHeightFontTexture(void) const;
    bool MakeFontTexture(int texWidth, int texHeight, unsigned char** ppTex) const;

    bool GetGlyph(xfs::HFont hdl, int wch, xfs::GlyphDesc& desc) const;
    const xfs::GlyphDesc* GetGlyphFromCache(xfs::HFont hdl, int ch) const;

    int GetFontHeight(xfs::HFont hdl) const;
    bool HasKerning(xfs::HFont hdl) const;

    bool IsCharInFont(xfs::HFont hdl, wchar_t ch) const;
    CFont* GetFontByHandle(xfs::HFont hdl) const;

    int NumFonts(void) const;

private:
    CUtlVector<CFont*> fonts_;
    bool bIsBuildFonts_;
};

inline void CFontManager::ReleaseAllFonts(void)
{
    fonts_.DeleteContents(true);
    bIsBuildFonts_ = false;
}

inline int CFontManager::NumFonts(void) const
{
    return fonts_.Count();
}

#endif // FontManager_h__