// Copyright (c) 2013 Vitaly Lyaschenko (scxv86@gmail.com)
// Purpose: A FreeType2 wrapper
//
#ifndef ft2Lib_h__
#define ft2Lib_h__

#pragma once

#include <ft2build.h>
#include FT_FREETYPE_H

// converts points to pixels
inline int fix26d6ToPixel(int p)
{
    return (p >> 6) + ((p & 0x20) >> 5);
}

namespace ft2lib
{

// Initialize/Shutdown the FreeType 2
//
bool InitFT2Lib(void);
void DoneFT2Lib(void);

//===========================================================================
// CFTFace
//===========================================================================
class CFTFace
{
public:
    CFTFace(void);
    ~CFTFace();

    bool Create(const char* fontFile, int index = 0);
    void ReleaseFace(void);

    bool SelectCharMap(void);
    bool SetPixelSize(int fontSize, int horz_res = 96, int vert_res = 96);
        
    bool LoadChar(int wch, bool render = true) const;

    int  GlyphAdvanceX(void) const;
    int  GlyphAdvanceY(void) const;

    bool HasKerning(void);
    int  GetKerningX(int wch_prev, int wch_next) const;
    int  GetKerningY(int wch_prev, int wch_next) const;
    int  GlyphHeight(void) const;
    int  GlyphWidth(void) const;
    int  GlyphBearingX(void) const;
    int  GlyphBearingY(void) const;
    int  GlyphAscender(void) const;
    int  GlyphDescender(void) const;

    // Glyph Bitmap
    //
    int  GlyphBitmapWidth(void) const;
    int  GlyphBitmapHeight(void) const;
    int  GlyphBitmapLeft(void) const;
    int  GlyphBitmapTop(void) const;
    int  GlyphBitmapPith(void) const;
    unsigned char* GlyphBitmapBuffer(void) const;

    int  MaxAvanceWidthGlyph(void) const;
    bool IsValid(void) const;
    static int GetCounter(void);

private:
    static int& Counter(void)
    {
        static int s_counter = 0;
        return s_counter;
    }
    FT_Face face_;
    bool bCreated_;
};


inline bool CFTFace::LoadChar(int wch, bool render) const
{
    if (FT_Load_Char(face_, wch, render ? FT_LOAD_RENDER : FT_LOAD_DEFAULT))
        return false;

    return true;
}

inline  bool CFTFace::HasKerning(void)
{
    return FT_HAS_KERNING(face_);
}

inline int CFTFace::GetKerningX(int ch_prev, int ch_next) const
{
    if((FT_UInt)ch_next)
    {
        FT_Vector vKern;
        FT_Get_Kerning(face_, FT_Get_Char_Index(face_, ch_prev), FT_Get_Char_Index(face_, ch_next), FT_KERNING_DEFAULT, &vKern);
        return fix26d6ToPixel(vKern.x);
    }
    return 0;
}

inline int CFTFace::GetKerningY(int ch_prev, int ch_next) const
{
    if ((FT_UInt)ch_next)
    {
        FT_Vector vKern;
        FT_Get_Kerning(face_, FT_Get_Char_Index(face_, ch_prev), FT_Get_Char_Index(face_, ch_next), FT_KERNING_DEFAULT, &vKern);
        return fix26d6ToPixel(vKern.y);
    }
    return 0;
}

inline int CFTFace::GlyphAdvanceX(void) const
{
    return fix26d6ToPixel(face_->glyph->advance.x);
}

inline int CFTFace::GlyphAdvanceY(void) const
{
    return fix26d6ToPixel(face_->glyph->advance.y);
}

inline int CFTFace::GlyphWidth(void) const
{
    return fix26d6ToPixel(face_->glyph->metrics.width);
}

inline int CFTFace::GlyphHeight(void) const
{
    return fix26d6ToPixel(face_->glyph->metrics.height);
}

inline int CFTFace::GlyphBearingX(void) const
{
    return fix26d6ToPixel(face_->glyph->metrics.horiBearingX);
}

inline int CFTFace::GlyphBearingY(void) const
{
    return fix26d6ToPixel(face_->glyph->metrics.horiBearingY);
}

inline int CFTFace::GlyphAscender(void) const
{
    return fix26d6ToPixel(face_->size->metrics.ascender);
}

inline int CFTFace::GlyphDescender(void) const
{
    return fix26d6ToPixel(face_->size->metrics.descender);
}

inline int CFTFace::GlyphBitmapWidth(void) const
{
    return face_->glyph->bitmap.width;
}

inline int CFTFace::GlyphBitmapHeight(void) const
{
    return face_->glyph->bitmap.rows;
}

inline int CFTFace::GlyphBitmapLeft(void) const
{
    return face_->glyph->bitmap_left;
}

inline int CFTFace::GlyphBitmapTop(void) const
{
    return face_->glyph->bitmap_top;
}

inline unsigned char* CFTFace::GlyphBitmapBuffer(void) const
{
    return face_->glyph->bitmap.buffer;
}

inline int CFTFace::GlyphBitmapPith(void) const
{
    return face_->glyph->bitmap.pitch;
}

inline int CFTFace::MaxAvanceWidthGlyph(void) const
{
    return fix26d6ToPixel(face_->max_advance_width);
}

} // End namespace ft2lib

#endif // ft2Lib_h__
