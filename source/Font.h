// Copyright (c) 2013 Vitaly Lyaschenko (scxv86@gmail.com)
// Purpose: Creating font object and a manipulation its glyphs
//
#ifndef Font_h__
#define Font_h__

#pragma once

#include "public/utlvector.h"
#include "ft2Lib.h"
#include "xfontTypes.h"

//===========================================================================
// CFont
//===========================================================================
class CFont
{
    friend class CFontManager;
public:
    CFont(void);
    ~CFont();

    bool Create(const char* file, int size);
    bool AddCharacterRange(int lower, int upper);
    bool Build(void);
    xfs::GlyphDesc const* GetGlyphFromCache(int ch) const;  // gets the glyph desc. for a character from the cache
    bool GetGlyph(wchar_t wch, xfs::GlyphDesc& desc) const; // gets the glyph desc. for a character from the FreeType
    
    int GetKerningX(wchar_t ch_prev, wchar_t ch_next) const;
    int GetKerningY(wchar_t ch_prev, wchar_t ch_next) const;
    const unsigned char* GetGlyphBitmap(wchar_t ch) const;
    
    bool HasKerning(void);
    bool IsFontEqual(const char* fontName, int fontSize) const; // returns true, if equal the font name and its size
    bool IsCharInFont(wchar_t ch) const;      // returns true, if the symbol is contained in the font
    bool IsRange(int lower, int upper) const; // returns true, if this range is already contained
    bool IsValid(void) const;

    // Data accessors
    int Height(void) const;
    int NumNeedTextureLines(void) const;
    int GetAbsoluteValue(void) const;
    int GetSize(void) const;
    int GetNumCharacter(void) const;
    const char* GetName(void) const;

protected:
    bool CalcTexCoords(int offsetX, int offsetY, int width, int height);
    bool GlyphTexSubImage(int offsetX, int offsetY, int width, int height, unsigned char* texdata) const;

protected:
    bool (CFont::*pAssignCacheForChar)(wchar_t ch) const;
    bool AssignCacheForUnicodeCharSet(wchar_t ch) const;
    bool AssignCacheForAsciCharSet(wchar_t ch) const;
    bool AssignCacheForChar(wchar_t ch) const;

    int FindCharInCache(wchar_t ch) const;
    bool AllocateCacheData(void);
    void FreeCacheData(void);

private:
    char fontName_[32];
    int fontSize_;
    int numChars_;
    int lineHeight_;
    int numNeedLines_;
    int absoluteVal_;       // ascender + descender
    ft2lib::CFTFace ftFace_; // access to functionality FreeType 2
    struct UChRange
    {
        UChRange() {}
        UChRange(int lower, int upper, int offset)
            : lower_(lower), upper_(upper), chOffset_(offset) {}
        int lower_;    // low range value
        int upper_;    // high range value
        int chOffset_; // number of characters before the next range
    };
    CUtlVector<UChRange> chRanges_;
    mutable int currIdxCache_;   // current cache index for a character
    xfs::GlyphDesc* pGlyphData_; // pointer to the cached glyphs data
    bool bBuild_;
    bool bAsciOnly_;             // if a font contains only ASCII characters
};

inline xfs::GlyphDesc const* CFont::GetGlyphFromCache(int ch) const
{
    if (!AssignCacheForChar(ch))
        return nullptr;

    return &pGlyphData_[currIdxCache_];
}

inline bool CFont::AssignCacheForUnicodeCharSet(wchar_t ch) const
{
    if ((currIdxCache_ = FindCharInCache(ch)) != -1)
        return true;

    currIdxCache_ = 0;
    return false;
}

inline int CFont::FindCharInCache(wchar_t ch) const
{
    for (int i(0); i < chRanges_.Count(); ++i)
    {
        if ((ch >= chRanges_[i].lower_) &&
            (ch <= chRanges_[i].upper_))
        {
            // returns the index in the cache
            return chRanges_[i].chOffset_ + (ch - chRanges_[i].lower_);
        }
    }
    return -1;
}

inline bool CFont::AssignCacheForAsciCharSet(wchar_t ch) const
{
    if ((int)ch < 127)
    {
        currIdxCache_ = ch;
        return true;
    }
    currIdxCache_ = 0;
    return false;
}

inline bool CFont::AssignCacheForChar(wchar_t ch) const
{
    return ((*this).*pAssignCacheForChar)(ch);
}

// rturns true, if the font was Created/Built and ready for use
inline bool CFont::IsValid(void) const
{
    if (fontName_[0] && bBuild_)
        return true;

    return false;
}

// gets the height of the line of the font, in pixels
inline int CFont::Height(void) const
{
    return lineHeight_;
}	

// returns the number of rows for the font in the texture
inline int CFont::NumNeedTextureLines(void) const
{
    return numNeedLines_;
}

// ascender + descender
inline int CFont::GetAbsoluteValue(void) const
{
    return absoluteVal_;
}

// gets the font size
inline int CFont::GetSize(void) const
{
    return fontSize_;
}

// gets the total number of characters in the font
inline int CFont::GetNumCharacter(void) const
{
    return numChars_;
}

// gets the name of this font
inline const char* CFont::GetName(void) const
{
    return fontName_;
}

#endif // Font_h__