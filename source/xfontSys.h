// Copyright © 2013 Vitaly Lyaschenko (scxv86@gmail.com)
// Purpose: XFS a main interface file
//
#ifndef xfontSys_h__
#define xfontSys_h__

#pragma once

#include "xfontTypes.h"

namespace xfs
{
    //
    // Initialize the XFS library
    bool Initialize(void);

    //
    // Release the XFS library
    void Release(void);

    //
    // Screen size for a shader uniform
    bool SetScreenSize(int width, int height);

    //
    // Creates a handle for the font face
    HFont Create_Font(const char* file, int size);

    //
    // Adds a set of glyphs in the font by its handle
    bool AddGlyphSetToFont(HFont hdl, EUChSets set);
    bool AddGlyphSetToFont(HFont hdl, int lower, int upper);

    //
    // Building all fonts and do make them ready for rendering
    bool BuildAllFonts(void);


    //
    // Set the font as the current by its handle
    void BindFont(HFont hdl);

    //
    // Returns the bounding rectangle for the given text
    void GetTextBBox(const char* text, int lenght, BBox& bbox);
    void GetWTextBBox(const wchar_t* text, int lenght, BBox& bbox);

    //
    // Sets the color of text
    void SetTextColor(uint8 r, uint8 g, uint8 b, uint8 a = 255);

    //
    // Sets/Gets the current text position
    void SetTextPos(int posX, int posY);
    void GetTextPos(int& posX, int& posY);

    //
    // Sets a static text and returns its ID in a single vb
    int SetStaticText(const char* text, int lenght);
    int SetStaticWText(const wchar_t* text, int lenght);

    //
    // Resets a video memory for the static text
    void ResetStaticText(void);

    //
    // Drawing the static text by its ID
    // At 0 will be drawing all static text
    void PrintStaticText(int idx = 0);

    //
    // Drawing a dynamic text strings
    void PrintText(const char* text, int lenght);
    void PrintWText(const wchar_t* text, int lenght);

    //
    // All operations of text rendering must be between these two functions
    void Draw2D_Begin(void);
    void Draw2D_End(void);


    //
    // Whether contains this font the kerning information
    bool HasKerning(HFont hdl);
    void UseKerning(bool flag);

    //
    // Returns a total number of vertices per frame
    int VertexPerFrame(void);


    // XFS Settings:
    bool SetFontTextureWidth(int width);
    bool SetMaxStaticChars(int count);

}; // End namespace xfs

#endif // xfontSys_h__