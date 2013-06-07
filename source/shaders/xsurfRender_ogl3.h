// Copyright © 2013 Vitaly Lyaschenko (scxv86@gmail.com)
// Purpose: 
//
#ifndef xsurfRender_ogl3_h__
#define xsurfRender_ogl3_h__

#pragma once

#include "xsurfRender.h"

enum { VERTEX_SIZE = 36 };

namespace xfs
{

//===========================================================================
// Surface Render OGL3
//===========================================================================
class CSurfRender_OGL3 : public CSurfRender<CSurfRender_OGL3>
{
public:
    CSurfRender_OGL3(void);

    bool Initialize_Impl(void);
    void Release_Impl();

    bool SetScreenSize_Impl(int width, int height) const;

    void SetBackgroundColor_Impl(Color color);

    // State
    void State2D_Enable_Impl(void) const;
    void State2D_Disable_Impl(void) const;

    // 3D
    void State3D_Enable_Impl(void) const;
    void State3D_Disable_Impl(void) const;

    void Push2DTexture(int w, int h, unsigned char* pTex);
    void Draw2DAlphaBlend(int offset, int count) const;

protected:
    CShaderOGL shader_;
    GLuint vaoID_[TT_MAX];
    GLuint texID_;
};

INLINE CSurfRender_OGL3::CSurfRender_OGL3(void)
    : texID_(0)
{
    vaoID_[TT_STATIC] = vaoID_[TT_DYNAMIC] = 0;
}

inline void AdjustTextVertexAttrib(void);

INLINE bool CSurfRender_OGL3::Initialize_Impl(void)
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

    //pBaseVertex_ = nullptr;
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

INLINE void CSurfRender_OGL3::Release_Impl(void)
{
    staticVBO_.ReleaseVB();
    dynamicVBO_.ReleaseVB();

    if (texID_ > 0)
    {
        glDeleteTextures(1, &texID_);
        texID_ = 0;
    }

    shader_.DestroyCompiledShaders();
    shader_.DestroyProgram();
}

INLINE bool CSurfRender_OGL3::SetScreenSize_Impl(int width, int height) const
{
    shader_.Use_Begin();
    return (shader_.Set_Float2("UScale",
        2.0f / (float)width,
        2.0f / (float)height) != -1);
}

INLINE void CSurfRender_OGL3::SetBackgroundColor_Impl(Color color)
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
INLINE void CSurfRender_OGL3::State3D_Enable_Impl(void) const
{
    shader_.Use_Begin();
}

INLINE void CSurfRender_OGL3::State3D_Disable_Impl(void) const
{
    shader_.Use_End();
}

INLINE void CSurfRender_OGL3::State2D_Enable_Impl(void) const
{
    glDisable(GL_CULL_FACE);
    glDisable(GL_DEPTH_TEST);
    shader_.Use_Begin();
}

INLINE void CSurfRender_OGL3::State2D_Disable_Impl(void) const
{
    shader_.Use_End();
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
}

INLINE void CSurfRender_OGL3::Draw2DAlphaBlend(GLint offset, GLsizei count) const
{
    glBindTexture(GL_TEXTURE_2D, texID_);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glBindVertexArray(vaoID_[currTT]);
    glDrawArrays(GL_POINTS, offset, count);
    glBindVertexArray(0);

    glDisable(GL_BLEND);
}

INLINE void CSurfRender_OGL3::Push2DTexture(int w, int h, unsigned char* pTex)
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

#endif // xsurfRender_ogl3_h__