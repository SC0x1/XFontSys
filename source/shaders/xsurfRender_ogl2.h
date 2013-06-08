// Copyright (c) 2013 Vitaly Lyaschenko (scxv86@gmail.com)
// Purpose: 
//
#ifndef xsurfRender_ogl2_h__
#define xsurfRender_ogl2_h__

#pragma once

#include "xsurfRender.h"

// 4float(x,y,s,t) * sizeof(float)
enum { VERTEX_SIZE = 16 };

namespace xfs
{

//===========================================================================
// Surface Render OGL2
//===========================================================================
class CSurfRender_OGL2 : public CSurfRender<CSurfRender_OGL2>, public Singleton<CSurfRender_OGL2>
{
public:
    CSurfRender_OGL2(void);

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
    void Draw2DAlphaBlend(const TextRenderInfo& info) const;

protected:
    CShaderOGL shader_;
    GLuint loc_color;
    GLuint texID_;
};
typedef CSurfRender_OGL2 SurfaceRender;

INLINE CSurfRender_OGL2::CSurfRender_OGL2(void)
    : texID_(0) {}

INLINE bool CSurfRender_OGL2::Initialize_Impl(void)
{

#include "shaders_gl2.inl"

    if (!shader_.BuildProgram(vertexShader, fragmentShader))
        return false;

    shader_.DestroyCompiledShaders();

    vbo_[TT_Dynamic].CreateVB(VERTEX_SIZE, 0, nullptr, true);
    vbo_[TT_Satic].CreateVB(VERTEX_SIZE, Config::noCurrStaticChars * VERTEX_PER_CHAR);

    shader_.Use_Begin();
    shader_.BindAttribLocation("aVT", VA_POSITION);
    loc_color = shader_.Get_UniformLocation("uColor");

    return true;
}

INLINE void CSurfRender_OGL2::Release_Impl(void)
{
    vbo_[TT_Satic].ReleaseVB();
    vbo_[TT_Dynamic].ReleaseVB();

    if (texID_ > 0)
    {
        glDeleteTextures(1, &texID_);
        texID_ = 0;
    }

    shader_.DestroyProgram();
}

INLINE bool CSurfRender_OGL2::SetScreenSize_Impl(int width, int height) const
{
    shader_.Use_Begin();
    return (shader_.Set_Float2("uScale",
        2.0f / (float)width,
        2.0f / (float)height) != -1);
}

INLINE void CSurfRender_OGL2::SetBackgroundColor_Impl(Color color)
{
    if (color != currColor_)
    {
        glClearColor(
            color.NormalizeRed(),
            color.NormalizeGreen(),
            color.NormalizeBlue(),
            color.NormalizeAlpha());
        currColor_ = color;
    }
}

// 3D
//
INLINE void CSurfRender_OGL2::State3D_Enable_Impl(void) const
{
    shader_.Use_Begin();
}

INLINE void CSurfRender_OGL2::State3D_Disable_Impl(void) const
{
    shader_.Use_End();
}

INLINE void CSurfRender_OGL2::State2D_Enable_Impl(void) const
{
    glDisable(GL_CULL_FACE);
    glDisable(GL_DEPTH_TEST);
    shader_.Use_Begin();
}

INLINE void CSurfRender_OGL2::State2D_Disable_Impl(void) const
{
    shader_.Use_End();
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
}

INLINE void CSurfRender_OGL2::Draw2DAlphaBlend(const TextRenderInfo& info) const
{
    glBindTexture(GL_TEXTURE_2D, texID_);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glEnableVertexAttribArray(VA_POSITION);

    vbo_[currTT].Bind();
    glVertexAttribPointer(VA_POSITION, 4, GL_FLOAT, GL_FALSE, VERTEX_SIZE, 0);

    shader_.Set_Float4v(loc_color, info.color_);
    glDrawArrays(GL_TRIANGLES, info.vertOffset_, info.vertCount_);
    vbo_[currTT].UnBind();
    glDisableVertexAttribArray(VA_POSITION);

    glDisable(GL_BLEND);
}

INLINE void CSurfRender_OGL2::Push2DTexture(int w, int h, unsigned char* pTex)
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

#endif // xsurfRender_ogl2_h__