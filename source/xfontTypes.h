// Copyright © 2013 Vitaly Lyaschenko (scxv86@gmail.com)
// Purpose: XFS Types
//
#ifndef xfontTypes_h__
#define xfontTypes_h__

#pragma once

#define XFS_VERSION 0.6

enum { INVALID_FONT = -1 };

namespace xfs
{

typedef unsigned char  uint8;
typedef unsigned short uint16;
typedef unsigned int   uint32;

// A font handle
typedef int HFont;

#define UNICODE_CHARACTER_SETS \
    X(BASIC_LATIN, "Base Latin", 0, 127) \
    X(LATIN_1_SUPPLEMENT, "Latin-1 Supplement", 128, 255) \
    X(LATIN_EXTENDED_A, "Latin Extended-A", 256, 383) \
    X(LATIN_EXTENDED_B, "Latin Extended-B", 384, 591) \
    X(IPA_EXTENSIONS, "IPA Extensions", 592, 687) \
    X(SPACING_MODIFIER_LETTERS, "Spacing Modifier Letters", 688, 767) \
    X(COMBINING_DIACRITICAL_MARKS, "Combining Diacritical Marks", 768, 879) \
    X(GREEK_AND_COPTIC, "Greek and Coptic", 880, 1023) \
    X(CYRILLIC, "Cyrillic", 1024, 1279) \
    X(CYRILLIC_SUPPLEMENTARY, "Cyrillic Supplementary", 1280, 1327) \
    X(MAX_CHAR_SETS, "", 0, 0) // add more ...

#define X(a, b, c, d) a,
enum EUChSets { UNICODE_CHARACTER_SETS };
#undef X

// glyph metrices
struct GlyphDesc
{
    // id in unicode table
    wchar_t ch;
    union
    {
        struct
        { 
            float s;	// x offset in image where glyph starts
            float t;	// y offset in image where glyph starts
            float p;	
            float q;
        };
        float texCoord[4];
    };
    short advanceX;
    short advanceY;
    short bitmapWidth;
    short bitmapHeight;
    short bitmapLeft;
    short bitmapTop;
};

typedef struct BBox_s
{
    BBox_s(void);
    BBox_s& operator = (const BBox_s& othe);
    int minX;
    int minY;
    int maxX;
    int maxY;
} BBox;

inline BBox_s::BBox_s() : minX(0), minY(0), maxX(0), maxY(0) {}

inline BBox_s& BBox_s::operator = (const BBox_s& other)
{
    if (this == &other)
        return *this;

    (*this).minX = other.minX;
    (*this).minY = other.minY;
    (*this).maxX = other.maxX;
    (*this).maxY = other.maxY;

    return *this;
}

}; // End namespace xfs

#endif // xfontTypes_h__