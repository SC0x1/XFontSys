// Copyright © 2013 Vitaly Lyaschenko (scxv86@gmail.com)
// Purpose: 
//
#include <assert.h>

#include "xfontConf.h"
#include "xsurfRender_ogl3.h"

// 16 bytes(pos) + 16 bytes(texCoords) + 4 bytes (color)
enum { VERTEX_SIZE = 36 };

//===========================================================================
// Vertices Builder
//===========================================================================
INLINE void PosQuad4f(float*& data, const float& x0, const float& y0, const float& x1, const float& y1);
INLINE void TexCoord4f(float*& data, const float* texCoords);
INLINE void Color4b(float*& data, const unsigned char* color);

namespace xfs
{

CSurfRender_OGL3::CSurfRender_OGL3(void)
    : texID_(0),
      pBaseVertex_(nullptr)
{
    vaoID_[TT_STATIC] = vaoID_[TT_DYNAMIC] = 0;
    currTT = TT_DYNAMIC;
}

inline void AdjustTextVertexAttrib(void);

bool CSurfRender_OGL3::Initialize_Impl(void)
{

#include "shaders_gl3.inl"

    if (!shader_.BuildProgram(vertexShader, geometryShader, fragmentShader))
        return false;

    dynamicVBO_.CreateVB(VERTEX_SIZE, 0, nullptr, true);
    staticVBO_.CreateVB(VERTEX_SIZE, Config::noCurrStaticChars);

    glGenVertexArrays(2, vaoID_);
    glBindVertexArray(vaoID_[TT_STATIC]);
    {
        staticVBO_.Bind();
        AdjustTextVertexAttrib();
        staticVBO_.UnBind();
    }
    glBindVertexArray(0);
    glBindVertexArray(vaoID_[TT_DYNAMIC]);
    {
        dynamicVBO_.Bind();
        AdjustTextVertexAttrib();
        dynamicVBO_.UnBind();
    }
    glBindVertexArray(0);

    pBaseVertex_ = nullptr;
    return true;
}

inline void AdjustTextVertexAttrib(void)
{
    glEnableVertexAttribArray(VA_POSITION);
    glVertexAttribPointer(VA_POSITION, 4, GL_FLOAT, GL_FALSE, VERTEX_SIZE, (GLvoid*)0);
    glEnableVertexAttribArray(VA_TEXCOORD0);
    glVertexAttribPointer(VA_TEXCOORD0, 4, GL_FLOAT, GL_FALSE, VERTEX_SIZE, (GLvoid*)16/*4*sizeof(float)*/);
    glEnableVertexAttribArray(VA_COLOR);
    glVertexAttribPointer(VA_COLOR, 4, GL_UNSIGNED_BYTE, GL_FALSE, VERTEX_SIZE, (GLvoid*)32/*8*sizeof(float)*/);
}

void CSurfRender_OGL3::Release_Impl(void)
{
    staticVBO_.ReleaseVB();
    dynamicVBO_.ReleaseVB();

    if (texID_ > 0)
        glDeleteTextures(1, &texID_);
}

bool CSurfRender_OGL3::SetScreenSize_Impl(int width, int height)
{
    shader_.Use_Begin();
    return (shader_.Set_Float2("UScale",
        2.0f / (float)width,
        2.0f / (float)height) != -1);
}

void CSurfRender_OGL3::PrintText_Impl(const char* text, int lenght)
{
    currTextLent_ = lenght;
    pBaseVertex_ = verticesBuffer_;
    currTT = TT_DYNAMIC;
    BuildTextVertices<char>(text);
    dynamicVBO_.PushVertexData(vertexCount_, verticesBuffer_);
    Draw2DAlphaBlend(TextRenderInfo(0, vertexCount_));
}

void CSurfRender_OGL3::PrintWText_Impl(const wchar_t* text, int lenght)
{
    currTextLent_ = lenght;
    pBaseVertex_ = verticesBuffer_;
    currTT = TT_DYNAMIC;
    BuildTextVertices<wchar_t>(text);
    dynamicVBO_.PushVertexData(vertexCount_, verticesBuffer_);
    Draw2DAlphaBlend(TextRenderInfo(0, vertexCount_));
}

int CSurfRender_OGL3::SetStaticText_Impl(const char* text, int lenght)
{
    // since we use a type GL_POINTS so textLenght == vertex count
    if (!staticVBO_.HasEnoughSpace(currTextLent_))
        return -1;

    currTextLent_ = lenght;
    return BuildStaticText<char>(text);
}

int CSurfRender_OGL3::SetStaticWText_Impl(const wchar_t* text, int lenght)
{
    // since we use a type GL_POINTS so textLenght == vertex count
    if (!staticVBO_.HasEnoughSpace(currTextLent_))
        return -1;

    currTextLent_ = lenght;
    return BuildStaticText<wchar_t>(text);
}

template<typename T>
int CSurfRender_OGL3::BuildStaticText(const T* text)
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

template<typename T>
void CSurfRender_OGL3::BuildTextVertices(const T* text)
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

        // writes quad positions
        PosQuad4f(pBaseVertex_, x, y, x + g->bitmapWidth, y + g->bitmapHeight);
        TexCoord4f(pBaseVertex_, g->texCoord);
        Color4b(pBaseVertex_, currTextColor_.Ptr());
        ++vertexCount_;
    }
}

void CSurfRender_OGL3::PrintStaticText_Impl(int idx)
{
    assert(textInfo_.Count() > 0);
    currTT = TT_STATIC;
    vertexPerFrame_ += textInfo_[idx - 1].vertCount_;
    Draw2DAlphaBlend(textInfo_[idx - 1]);
}

void CSurfRender_OGL3::ResetStaticText_Impl(void)
{
    if (noMaxChars_ != Config::noCurrStaticChars)
    {
        staticVBO_.PushVertexData(Config::noCurrStaticChars, nullptr);
        noMaxChars_ = Config::noCurrStaticChars;
    }
    staticVBO_.BorderToBeginVB();
    textInfo_.Clear();
}

void CSurfRender_OGL3::SetBackgroundColor_Impl(Color color)
{
    if (color != currTextColor_)
    {
        glClearColor(
            color.NormalizeRed(),
            color.NormalizeGreen(),
            color.NormalizeBlue(),
            color.NormalizeAlpha());
        currTextColor_ = color;
    }
}

// 3D
//
void CSurfRender_OGL3::State3D_Enable_Impl(void)
{
    shader_.Use_Begin();
}

void CSurfRender_OGL3::State3D_Disable_Impl(void)
{
    shader_.Use_End();
}

INLINE void CSurfRender_OGL3::Draw2DAlphaBlend(const TextRenderInfo& info) const
{
    glBindTexture(GL_TEXTURE_2D, texID_);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glBindVertexArray(vaoID_[currTT]);
    glDrawArrays(GL_POINTS, info.vertOffset_, info.vertCount_);
    glBindVertexArray(0);

    glDisable(GL_BLEND);
}

void CSurfRender_OGL3::Push2DTexture_Impl(int w, int h, unsigned char* pTex)
{
    assert(pTex);
    assert(texID_ == 0);

    glGenTextures(1, &texID_);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texID_);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, w, h, 0, GL_RED, GL_UNSIGNED_BYTE, pTex);
    glBindTexture(GL_TEXTURE_2D, 0);
}

} // End namespace xfs

//===========================================================================
// Vertices Builder
//===========================================================================
INLINE void PosQuad4f(float*& data, const float& x0, const float& y0, const float& x1, const float& y1)
{
    float* p = data;
    *p++ = x0;
    *p++ = y0;
    *p++ = x1;
    *p   = y1;
    data += 4;
}
INLINE void TexCoord4f(float*& data, const float* texCoords)
{
    float* p = data;
    *p++ = texCoords[0];
    *p++ = texCoords[1];
    *p++ = texCoords[2];
    *p   = texCoords[3];
    data += 4;
}
INLINE void Color4b(float*& data, const unsigned char* color)
{
    unsigned char* p = reinterpret_cast<unsigned char*>(data);
    *p++ = *color;
    *p++ = *(color + 1);
    *p++ = *(color + 2);
    *p   = *(color + 3);
    data += 1;
}