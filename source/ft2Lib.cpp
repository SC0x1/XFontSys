// Copyright © 2013 Vitaly Lyaschenko (scxv86@gmail.com)
// Purpose: 
//
#include "ft2Lib.h"

namespace ft2lib
{

FT_Library g_FT2Lib = nullptr;

CFTFace::CFTFace()
    : face_(nullptr),
      bCreated_(false) {}

CFTFace::~CFTFace()
{
    if (face_)
    {
        FT_Done_Face(face_);
        --Counter();
    }
}

bool CFTFace::Create(const char* fontFile, int index)
{
    if (!g_FT2Lib || !fontFile)
        return false;

    if (FT_New_Face(g_FT2Lib, fontFile, index, &face_))
    {
        face_ = nullptr;
        return false;
    }
    ++Counter();
    return bCreated_ = true;
}

void CFTFace::ReleaseFace()
{
    if (face_)
    {
        FT_Done_Face(face_);
        --Counter();
    }
}

bool CFTFace::IsValid() const
{
    return bCreated_;
}

int CFTFace::GetCounter()
{
    return Counter();
}

bool CFTFace::SelectCharMap()
{
    return FT_Select_Charmap(face_, FT_ENCODING_UNICODE) == 0;
}

bool CFTFace::SetPixelSize(int fontSize, int horz_res, int vert_res)
{
    if (FT_Set_Char_Size(face_, fontSize << 6, fontSize << 6, horz_res, vert_res))
        return false;

    return true;
}

bool InitFT2Lib(void)
{
    if (FT_Init_FreeType(&g_FT2Lib))
        return false;

    return true;
}

void DoneFT2Lib(void)
{
    if (g_FT2Lib)
    {
        FT_Done_FreeType(g_FT2Lib);
        g_FT2Lib = nullptr;
    }
}

}; // End namespace ft2lib
