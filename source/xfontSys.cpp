// Copyright © 2013 Vitaly Lyaschenko (scxv86@gmail.com)
// Purpose: 
//
#include <assert.h>

#include "ft2Lib.h"
#include "Font.h"
#include "FontManager.h"

#include "xfontConf.h"
#include "shaders/xsurfRender_ogl3.h"
#include "xfontSys.h"

namespace xfs
{

static CSurfRender_OGL3 surfBase;

bool Initialize(void)
{
    if (surfBase.IsInit())
        return false;

    if (!surfBase.Initialize())
        return false;

    if (!ft2lib::InitFT2Lib())
        return false;

    return true;
}

bool SetScreenSize(int width, int height)
{
    return surfBase.SetScreenSize(width, height);
}

void Release(void)
{
    surfBase.Release();
}

HFont Create_Font(const char* file, int size)
{
    return surfBase.Create_Font(file, size);
}

bool AddGlyphSetToFont(HFont hdl, EUChSets set)
{
    return surfBase.AddGlyphSetToFont(hdl, set);
}

bool AddGlyphSetToFont(HFont hdl, int lower, int upper)
{
    return surfBase.AddGlyphSetToFont(hdl, lower, upper);
}

bool BuildAllFonts(void)
{
    return surfBase.BuildFonts();
}

void BindFont(HFont hdl)
{
    surfBase.BindFont(hdl);
}

void GetTextBBox(const char* text, int lenght, BBox& bbox)
{
    surfBase.GetTextBBox(text, lenght, bbox);
}

void GetWTextBBox(const wchar_t* text, int lenght, BBox& bbox)
{
    surfBase.GetWTextBBox(text, lenght, bbox);
}

void SetTextColor(uint8 r, uint8 g, uint8 b, uint8 a)
{
    surfBase.SetTextColor(r, g, b, a);
}

void SetTextPos(int posX, int posY)
{
    surfBase.SetTextPos(posX, posY);
}

void GetTextPos(int& posX, int& posY)
{
    surfBase.GetTextPos(posX, posY);
}

int SetStaticText(const char* text, int lenght)
{
    return surfBase.SetStaticText(text, lenght);
}

int SetStaticWText(const wchar_t* text, int lenght)
{
    return surfBase.SetStaticWText(text, lenght);
}

void ResetStaticText(void)
{
    surfBase.ResetStaticText();
}

void PrintStaticText(int idx)
{
    surfBase.PrintStaticText(idx);
}

void PrintText(const char* text, int lenght)
{
    surfBase.PrintText(text, lenght);
}

void PrintWText(const wchar_t* text, int lenght)
{
    surfBase.PrintWText(text, lenght);
}

bool HasKerning(HFont hdl)
{
    return CFontManager::Spec().HasKerning(hdl);
}

void UseKerning(bool kern)
{
    Config::bKerning = kern;
}

void Draw2D_Begin(void)
{
    surfBase.State2D_Enable();
}

void Draw2D_End(void)
{
    surfBase.State2D_Disable();
}

int VertexPerFrame(void)
{
    return surfBase.VertexPerFrame();
}

bool SetFontTextureWidth(int width)
{
    return Config::SetFontTextureWidth(width);
}

bool SetMaxStaticChars(int count)
{
    return Config::SetMaxStaticChars(count);
}

}; // End namespace xfs