// Copyright © 2013 Vitaly Lyaschenko (scxv86@gmail.com)
// Purpose: Basic Surface Render
//
#ifndef xsurfRender_h__
#define xsurfRender_h__

#pragma once

#include <wctype.h> // for iswspace()

#include "public/color.h"
#include "public/utils.h"

#include "Font.h"
#include "FontManager.h"

namespace xfs
{

static struct UChRanges
{
    const char* name;
    int lower;
    int upper;
};
#define X(a, b, c, d) {b, c, d},
static UChRanges g_UTRanges[] = { UNICODE_CHARACTER_SETS };
#undef X

//===========================================================================
// CSurfRender
//===========================================================================
template<typename D>
class CSurfRender
{
public:
    CSurfRender(void);

    bool Initialize(void);
    void Release(void);

    bool SetScreenSize(int width, int height);

    HFont Create_Font(const char* file, int size);

    bool AddGlyphSetToFont(HFont hdl, EUChSets set);
    bool AddGlyphSetToFont(HFont hdl, int lower, int upper);

    bool BuildFonts(void);
    void BindFont(HFont hnd);

    void PrintText(const char* wtext, int lenght);
    void PrintWText(const wchar_t* wtext, int lenght);
    void GetTextBBox(const char* text, int lenght, BBox& bbox);
    void GetWTextBBox(const wchar_t* text, int lenght, BBox& bbox);
    void SetTextColor(uint8 r, uint8 g, uint8 b, uint8 a = 255);
    void SetTextPos(int posX, int posY);
    void GetTextPos(int& posX, int& posY);

    int  SetStaticText(const char* wtext, int lenght);
    int  SetStaticWText(const wchar_t* wtext, int lenght);
    void PrintStaticText(int idx = 0);
    void ResetStaticText(void);

    void SetBackgroundColor(Color color);

    int VertexPerFrame(void) const;

    // State
    void State2D_Enable(void);
    void State2D_Disable(void);

    //void State3D_Enable(void);
    //void State3D_Disable(void);

    bool IsInit(void) const;

protected:
    template<typename T>
    void GetTextArea(const T* text, xfs::BBox& bbox);

    Color currTextColor_;	// color for the text
    int currTextPos_[2];	// current position in screen space
    int currTextLent_;		// current length of the text
    HFont hFont_;			// descriptor (id) of the current font
    int noMaxChars_;
    
    int vertexCount_;
    int vertexPerFrame_;	// debug info
    bool bInit_;
};

template<typename D>
INLINE CSurfRender<D>::CSurfRender(void)
    : currTextColor_(0),
      currTextLent_(0),
      hFont_(0),
      vertexCount_(0),
      vertexPerFrame_(0),
      bInit_(false)
{
    noMaxChars_ = Config::noCurrStaticChars;
    currTextPos_[0] = currTextPos_[1] = 0;
}

template<typename D>
INLINE bool CSurfRender<D>::Initialize(void)
{
    return bInit_ = static_cast<D*>(this)->Initialize_Impl();
}

template<typename D>
INLINE void CSurfRender<D>::Release(void)
{
    CFontManager::Spec().ReleaseAllFonts();
    static_cast<D*>(this)->Release_Impl();
    bInit_ = false;
}

template<typename D>
INLINE bool CSurfRender<D>::SetScreenSize(int width, int height)
{
    return static_cast<D*>(this)->SetScreenSize_Impl(width, height);
}

template<typename D>
INLINE HFont CSurfRender<D>::Create_Font(const char* file, int size)
{
    return CFontManager::Spec().Create_Font(file, size);
}

template<typename D>
INLINE bool CSurfRender<D>::AddGlyphSetToFont(HFont hdl, EUChSets chSet)
{
    if ((hdl <= 0) || (chSet >= EUChSets::MAX_CHAR_SETS))
        return false;

    BindFont(hdl);
    return CFontManager::Spec().AddGlyphSetToFont(hdl, g_UTRanges[chSet].lower, g_UTRanges[chSet].upper);
}

template<typename D>
INLINE bool CSurfRender<D>::AddGlyphSetToFont(HFont hdl, int lower, int upper)
{
    if ((hdl <= 0) || ((upper - lower) <= 0))
        return false;

    BindFont(hdl);
    return CFontManager::Spec().AddGlyphSetToFont(hdl, lower, upper);
}

template<typename D>
INLINE bool CSurfRender<D>::BuildFonts(void)
{
    if (!CFontManager::Spec().BuildFonts())
        return false;
   
    const int width = Config::currTextFontWidth;
    const int height = CFontManager::Spec().GetHeightFontTexture();
    assert(height > 0);

    // allocate memory for the texture
    uint8* pTex(nullptr);
    pTex = (uint8*)calloc(width * height, sizeof(uint8));
    if (!pTex)
    {
        fprintf(stderr, "\nFailed to allocate memory for the font texture %dx%d\n", width, height);
        CFontManager::Spec().ReleaseAllFonts();
        return false;
    }

    if (!CFontManager::Spec().MakeFontTexture(width, height, &pTex))
    {
        fprintf(stderr, "\nFailed to make texture fonts\n");
        CFontManager::Spec().ReleaseAllFonts();
        return false;
    }
    //utils::TGA_Save("ShowMeResult.tga", width, height, pTex);
    static_cast<D*>(this)->Push2DTexture_Impl(width, height, pTex);
    free(pTex);
    return true;
}

template<typename D>
INLINE void CSurfRender<D>::BindFont(HFont hdl)
{
    hFont_ = hdl;
}

template<typename D>
INLINE void CSurfRender<D>::PrintText(const char* text, int lenght)
{
    if (!text || (lenght > Config::MAX_LENGTH_STRING))
        return;

    assert(hFont_ > 0);
    static_cast<D*>(this)->PrintText_Impl(text, lenght);
}

template<typename D>
INLINE void CSurfRender<D>::PrintWText(const wchar_t* text, int lenght)
{
    if (!text || (lenght > Config::MAX_LENGTH_STRING))
        return;

    assert(hFont_ > 0);
    static_cast<D*>(this)->PrintWText_Impl(text, lenght);
}

template<typename D>
INLINE void CSurfRender<D>::GetTextBBox(const char* text, int lenght, BBox& bbox)
{
    currTextLent_ = lenght;
    return GetTextArea<char>(text, bbox);
}

template<typename D>
INLINE void CSurfRender<D>::GetWTextBBox(const wchar_t* text, int lenght, BBox& bbox)
{
    currTextLent_ = lenght;
    return GetTextArea<wchar_t>(text, bbox);
}

template<typename D>
INLINE void CSurfRender<D>::SetTextColor(uint8 r, uint8 g, uint8 b, uint8 a)
{
    currTextColor_.SetColor(r, g, b, a);
}

template<typename D>
INLINE void CSurfRender<D>::SetTextPos(int posX, int posY)
{
    currTextPos_[0] = posX;
    currTextPos_[1] = posY;
}

template<typename D>
INLINE void CSurfRender<D>::GetTextPos(int& posX, int& posY)
{
    posX = currTextPos_[0];
    posY = currTextPos_[1];
}

template<typename D>
INLINE int CSurfRender<D>::SetStaticText(const char* text, int lenght)
{
    assert(text);
    if (lenght > Config::noCurrStaticChars)
        return -1;

    return static_cast<D*>(this)->SetStaticText_Impl(text, lenght);
}

template<typename D>
INLINE int CSurfRender<D>::SetStaticWText(const wchar_t* text, int lenght)
{
    assert(text);
    if (lenght > Config::noCurrStaticChars)
        return -1;

    return static_cast<D*>(this)->SetStaticWText_Impl(text, lenght);
}

template<typename D>
INLINE void CSurfRender<D>::PrintStaticText(int idx /*= 0*/)
{
    static_cast<D*>(this)->PrintStaticText_Impl(idx);
}

template<typename D>
INLINE void CSurfRender<D>::ResetStaticText(void)
{
    static_cast<D*>(this)->ResetStaticText_Impl();
}

template<typename D>
INLINE void CSurfRender<D>::SetBackgroundColor(Color color)
{
    return static_cast<D*>(this)->SetBackgroundColor_Impl(color);
}

template<typename D>
INLINE int CSurfRender<D>::VertexPerFrame(void) const
{
    return vertexPerFrame_;
}

template<typename D>
INLINE void CSurfRender<D>::State2D_Enable(void)
{
    vertexPerFrame_ = 0;
    return static_cast<D*>(this)->State2D_Enable_Impl();
}
template<typename D>
INLINE void CSurfRender<D>::State2D_Disable(void)
{
    return static_cast<D*>(this)->State2D_Disable_Impl();
}

template<typename D>
INLINE bool CSurfRender<D>::IsInit(void) const
{
    return bInit_;
}

template<typename D> template<typename T>
INLINE void CSurfRender<D>::GetTextArea(const T* text, BBox& bbox)
{
    assert(text);
    int posX(currTextPos_[0]);
    int posY(currTextPos_[1]);
    int maxW(0);
    int maxH(0);
    const CFont& font = *CFontManager::Spec().GetFontByHandle(hFont_);
    const int lineHeight(font.Height());

    for (int i(0); i < currTextLent_; ++i)
    {
        const T& ch = text[i];
        if (iswspace(ch))
        {
            if (ch == '\n')
            {
                posY += lineHeight;
                posX = currTextPos_[0];
                continue;
            }
            if ((ch == ' ') || (ch == '\t'))
                ;
            else
            {
                continue;
            }
        }
        const GlyphDesc* g = font.GetGlyphFromCache(ch);
        if (!g)
        {
            continue;
        }
        Config::bKerning
            ? posX += g->advanceX + font.GetKerningX(text[i], text[i+1])
            : posX += g->advanceX;
        //posY += pDesc->advanceY;
        if (posX > maxW)
            maxW = posX;
    }
    maxH = posY + lineHeight + (lineHeight - font.GetAbsoluteValue());
    bbox.minX = currTextPos_[0];
    bbox.minY = currTextPos_[1];
    bbox.maxX = maxW;
    bbox.maxY = maxH;
}

} // End namespace xfs

#endif // xsurfRender_h__