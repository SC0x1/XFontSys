// Copyright © 2013 Vitaly Lyaschenko (scxv86@gmail.com)
// Purpose: XFS Settings
//
#ifndef xfontConf_h__
#define xfontConf_h__

#pragma once

//#define XFS_RENDER_OGL_3_2
#define XFS_RENDER_OGL_2_0

struct Config
{
    enum { MAX_LENGTH_STRING = 1024 };

    static int noCurrStaticChars;
    enum { MIN_STATIC_CHARS = 0 };
    enum { MAX_STATIC_CHARS = 50000 };

    static int currTextFontWidth;
    enum { MIN_FONT_TEXTURE_WIDTH = 128 };
    enum { MAX_FONT_TEXTURE_WIDTH = 4096 };

    static bool SetFontTextureWidth(int width);
    static bool SetMaxStaticChars(int count);

    static bool bKerning;
};

#endif // xfontConf_h__
