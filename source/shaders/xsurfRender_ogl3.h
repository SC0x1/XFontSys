// Copyright © 2013 Vitaly Lyaschenko (scxv86@gmail.com)
// Purpose: 
//
#ifndef xsurfRender_ogl3_h__
#define xsurfRender_ogl3_h__

#pragma once

#include "public/utlvector.h"
#include "shaders/ogl.h"
#include "shaders/shader_util.h"
#include "shaders/dvbuffer.h"

#include "xsurfRender.h"

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

    bool SetScreenSize_Impl(int width, int height);

    void PrintText_Impl(const char* text, int texLen);
    void PrintWText_Impl(const wchar_t* text, int texLen);

    int SetStaticText_Impl(const char* text, int lenght);
    int SetStaticWText_Impl(const wchar_t* text, int lenght);
    void PrintStaticText_Impl(int idx = 0);
    void ResetStaticText_Impl(void);

    void SetBackgroundColor_Impl(Color color);

    // State
    void State2D_Enable_Impl(void);
    void State2D_Disable_Impl(void);
    

    // 3D
    void State3D_Enable_Impl(void);
    void State3D_Disable_Impl(void);

    void Push2DTexture_Impl(int w, int h, unsigned char* pTex);

protected:
    struct TextRenderInfo
    {
        TextRenderInfo(void) {}
        TextRenderInfo(const int& vertOffset, const int& vertCount)
            : vertOffset_(vertOffset),
              vertCount_(vertCount) {}

        int vertOffset_;
        int vertCount_;
    };

    template<typename T>
    int BuildStaticText(const T* text);

    template<typename T>
    void BuildTextVertices(const T* text);

    
    void Draw2DAlphaBlend(const TextRenderInfo& txtInfo) const;

    CUtlVector<TextRenderInfo> textInfo_;

    float verticesBuffer_[Config::MAX_LENGTH_STRING];
    mutable float* pBaseVertex_;

    CVertexBuffer staticVBO_;
    CVertexBuffer dynamicVBO_;
    CShaderOGL shader_;

    enum ETextType
    {
        TT_STATIC,
        TT_DYNAMIC,
        TT_MAX,
    };
    ETextType currTT;
    GLuint vaoID_[TT_MAX];
    GLuint texID_;
};

INLINE void CSurfRender_OGL3::State2D_Enable_Impl(void)
{
    glDisable(GL_CULL_FACE);
    glDisable(GL_DEPTH_TEST);
    shader_.Use_Begin();
}

INLINE void CSurfRender_OGL3::State2D_Disable_Impl(void)
{
    shader_.Use_End();
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
}

} // End namespace xfs

#endif // xsurfRender_ogl3_h__