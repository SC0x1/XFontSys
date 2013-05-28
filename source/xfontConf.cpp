// Copyright © 2013 Vitaly Lyaschenko (scxv86@gmail.com)
// Purpose: 
//
#include "xfontConf.h"

int Config::noCurrStaticChars = 4096;
int Config::currTextFontWidth = 1024;
bool Config::bKerning = false;

bool Config::SetFontTextureWidth(int width)
{
    if ((width < MIN_FONT_TEXTURE_WIDTH) ||
        (width > MAX_FONT_TEXTURE_WIDTH))
    {
            return false;
    }
    currTextFontWidth = width;
    return true;
}

bool Config::SetMaxStaticChars(int count)
{
    if ((count < MIN_STATIC_CHARS) ||
        (count > MAX_STATIC_CHARS))
    {
            return false;
    }
    noCurrStaticChars = count;
    return true;
}