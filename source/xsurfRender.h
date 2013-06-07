// Copyright © 2013 Vitaly Lyaschenko (scxv86@gmail.com)
// Purpose: Basic Surface Render
//
#ifndef xsurfRender_h__
#define xsurfRender_h__

#pragma once

#include <wctype.h> // for iswspace()

#include "Font.h"
#include "FontManager.h"

#include "public/color.h"
#include "public/utils.h"
#include "public/utlvector.h"

#include "shaders/ogl.h"
#include "shaders/shader_util.h"
#include "shaders/dvbuffer.h"

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

    template<typename T>
    void PrintText(const T* text, int lenght);
    template<typename T>
    void GetTextBBox(const T* text, int lenght, BBox& bbox);
    void SetTextColor(uint8 r, uint8 g, uint8 b, uint8 a = 255);
    void SetTextPos(int posX, int posY);
    void GetTextPos(int& posX, int& posY);

    template<typename T>
    int  SetStaticText(const T* text, int lenght);
    void PrintStaticText(int idx = 0);
    void ResetStaticText(void);

    void SetBackgroundColor(Color color) const;

    int VertexPerFrame(void) const;

    // State
    void State2D_Enable(void);
    void State2D_Disable(void);

    //void State3D_Enable(void);
    //void State3D_Disable(void);

    bool IsInit(void) const;

protected:
    template<typename T>
    void GetTextArea(const T* text, xfs::BBox& bbox) const;

    template<typename T>
    int BuildStaticText(const T* text);

    template<typename T>
    void BuildTextVertices(const T* text);

    // Vertices Builder
    void PosQuad4f(const float& x0, const float& y0, const float& x1, const float& y1) const;
    void TexCoord4f(const float* texCoords) const;
    void Color4b(const unsigned char* color) const;

    struct TextRenderInfo
    {
        TextRenderInfo(void) {}
        TextRenderInfo(const int& vertOffset, const int& vertCount)
            : vertOffset_(vertOffset), vertCount_(vertCount) {}

        int vertOffset_;
        int vertCount_;
    };

    CUtlVector<TextRenderInfo> textInfo_;
    float verticesBuffer_[Config::MAX_LENGTH_STRING];

    CVertexBuffer staticVBO_;
    CVertexBuffer dynamicVBO_;
    enum ETextType
    {
        TT_STATIC,
        TT_DYNAMIC,
        TT_MAX,
    };
    ETextType currTT;

    mutable float* pBaseVertex_;
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
    : currTT(TT_DYNAMIC),
      pBaseVertex_(nullptr),
      currTextColor_(0),
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
    pBaseVertex_ = nullptr;
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

template<typename D> template<typename T>
INLINE void CSurfRender<D>::PrintText(const T* text, int lenght)
{
    assert(hFont_ > 0);
    if (!text || (lenght > Config::MAX_LENGTH_STRING))
        return;

    currTextLent_ = lenght;
    pBaseVertex_ = verticesBuffer_;
    currTT = TT_DYNAMIC;
    BuildTextVertices<T>(text);
    dynamicVBO_.PushVertexData(vertexCount_, verticesBuffer_);
    static_cast<D*>(this)->Draw2DAlphaBlend(0, vertexCount_);
}

template<typename D> template<typename T>
INLINE void CSurfRender<D>::GetTextBBox(const T* text, int lenght, BBox& bbox)
{
    currTextLent_ = lenght;
    return GetTextArea<T>(text, bbox);
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

template<typename D> template<typename T>
INLINE int CSurfRender<D>::SetStaticText(const T* text, int lenght)
{
    assert(text);
    if (lenght > Config::noCurrStaticChars)
        return -1;

    // since we use a type GL_POINTS so textLenght == vertex count
    if (!staticVBO_.HasEnoughSpace(currTextLent_)) // TODO: review for OGL 2
        return -1;

    currTextLent_ = lenght;
    return BuildStaticText<T>(text);
}

template<typename D>
INLINE void CSurfRender<D>::PrintStaticText(int idx /*= 0*/)
{
    assert(textInfo_.Count() > 0);
    currTT = TT_STATIC;
    const TextRenderInfo& info = textInfo_[idx - 1];
    static_cast<D*>(this)->Draw2DAlphaBlend(info.vertOffset_, info.vertCount_);
    vertexPerFrame_ += info.vertCount_;
}

template<typename D>
INLINE void CSurfRender<D>::ResetStaticText(void)
{
    if (noMaxChars_ != Config::noCurrStaticChars)
    {
        staticVBO_.PushVertexData(Config::noCurrStaticChars, nullptr);
        noMaxChars_ = Config::noCurrStaticChars;
    }
    staticVBO_.BorderToBeginVB();
    textInfo_.Clear();
}

template<typename D>
INLINE void CSurfRender<D>::SetBackgroundColor(Color color) const
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
INLINE void CSurfRender<D>::GetTextArea(const T* text, BBox& bbox) const
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

template<typename D> template<typename T>
INLINE int CSurfRender<D>::BuildStaticText(const T* text)
{
    int baseVertex(0);
    void* ptrVM = staticVBO_.Lock(currTextLent_, baseVertex);
    if (!ptrVM)
    {
        fprintf(stderr, "\nError Lock Vertex Buffer\n");
        return -1;
    }
    // 4float(pos) + 4float(texCoords) + 1float(4 bytes per color) = 9
    pBaseVertex_ = (float*)ptrVM + (baseVertex * 9);

    BuildTextVertices<T>(text);
    staticVBO_.Unlock(vertexCount_);

    textInfo_.Append(TextRenderInfo(baseVertex, vertexCount_));
    return textInfo_.Count();
}
template<typename D> template<typename T>
INLINE void CSurfRender<D>::BuildTextVertices(const T* text)
{
    CFont& font = *CFontManager::Spec().GetFontByHandle(hFont_);
    const int height(font.Height());
    int posX(currTextPos_[0]);
    int posY(currTextPos_[1]);
    vertexPerFrame_ += vertexCount_;
    vertexCount_ = 0;
    for (int i(0); i < currTextLent_; ++i)
    {
        const T& ch = text[i];
        const GlyphDesc* g = font.GetGlyphFromCache(ch);
        if (!g)
        {
            continue;
        }
        const int x = posX + g->bitmapLeft;
        const int y = (posY + height) - g->bitmapTop;
        Config::bKerning ?
            posX += g->advanceX + font.GetKerningX(text[i], text[i+1]) :
        posX += g->advanceX;
        //posY += g.advanceY;
        if (iswspace(ch))
        {
            if (ch == '\n')
            {
                posX = currTextPos_[0];
                posY += height;
            }
            continue;
        }
#ifdef XFS_RENDER_OGL_3_2
        // writes quad positions
        PosQuad4f(x, y, x + g->bitmapWidth, y + g->bitmapHeight);
        TexCoord4f(g->texCoord);
        Color4b(currTextColor_.Ptr());
#endif
        ++vertexCount_;
    }
}

// Vertices Builder
template<typename D>
INLINE void CSurfRender<D>::PosQuad4f(const float& x0, const float& y0, const float& x1, const float& y1) const
{
    float* p = pBaseVertex_;
    *p++ = x0;
    *p++ = y0;
    *p++ = x1;
    *p   = y1;
    pBaseVertex_ += 4;
}
template<typename D>
INLINE void CSurfRender<D>::TexCoord4f(const float* texCoords) const
{
    float* p = pBaseVertex_;
    *p++ = texCoords[0];
    *p++ = texCoords[1];
    *p++ = texCoords[2];
    *p   = texCoords[3];
    pBaseVertex_ += 4;
}
template<typename D>
INLINE void CSurfRender<D>::Color4b(const unsigned char* color) const
{
    unsigned char* p = reinterpret_cast<unsigned char*>(pBaseVertex_);
    *p++ = *color;
    *p++ = *(color + 1);
    *p++ = *(color + 2);
    *p   = *(color + 3);
    pBaseVertex_ += 1;
}
} // End namespace xfs

#endif // xsurfRender_h__