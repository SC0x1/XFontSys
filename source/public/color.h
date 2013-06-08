// Copyright (c) 2013 Vitaly Lyaschenko (scxv86@gmail.com)
// Purpose: Basic handler for an rgb set of colors
// (fully inline)
#ifndef color_h__
#define color_h__

#pragma once

enum
{
    SHIFT_RED   = 0,
    SHIFT_GREEN = 8,
    SHIFT_BLUE  = 16,
    SHIFT_ALPHA = 24,
};

// set the color (rgba) in a range (0-255)
#define RED_(r)   ((unsigned int)(unsigned char)(r)   << SHIFT_RED)
#define GREEN_(g) ((unsigned int)((unsigned char)(g)) << SHIFT_GREEN)
#define BLUE_(b)  ((unsigned int)((unsigned char)(b)) << SHIFT_BLUE)
#define ALPHA_(a) ((unsigned int)((unsigned char)(a)) << SHIFT_ALPHA)

#define RGB_(r, g, b) (RED_(r) | GREEN_(g) | BLUE_(b))
#define RGBA_(r, g, b, a) (RED_(r) | GREEN_(g) | BLUE_(b) | ALPHA_(a))

// normalization given color component between 0..1
#define NORMALIZE_COLOR(X) ((float)X / 255.0f)

//=============================================================================
// Color
//=============================================================================
class Color
{
public:
    Color(void);
    explicit Color(unsigned int rgba);
    Color(unsigned char r, unsigned char g, unsigned char b);
    Color(unsigned char r, unsigned char g, unsigned char b, unsigned char a);

    Color& operator = (const Color& other);
    Color& operator = (unsigned int rgba);

    unsigned char& operator[](int index);
    bool operator == (Color& other) const;
    bool operator != (Color& other) const;

    void SetColor(unsigned char r, unsigned char g, unsigned char b);
    void SetColor(unsigned char r, unsigned char g, unsigned char b, unsigned char a);
    void SetValue(unsigned int rgba);

    unsigned char GetRed(void) const;
    unsigned char GetGreen(void) const;
    unsigned char GetBlue(void) const;
    unsigned char GetAlpha(void) const;
    unsigned int GetRGBA(void) const;

    // Color normalization
    float NormalizeRed(void) const;
    float NormalizeGreen(void) const;
    float NormalizeBlue(void) const;
    float NormalizeAlpha(void) const;

    unsigned char* Ptr(void);

private:
    union
    {
        struct
        {
            unsigned char r_;
            unsigned char g_;
            unsigned char b_;
            unsigned char a_;
        };
        unsigned int rgba_;
        unsigned char color_[4];
    };
};

inline Color::Color()
    : r_(0), g_(0), b_(0), a_(0) {}

inline Color::Color(unsigned int rgba)
    : rgba_(rgba) {}

inline Color::Color(unsigned char r, unsigned char g, unsigned char b)
    : r_(r), g_(g), b_(b), a_(0) {}

inline Color::Color(unsigned char r, unsigned char g, unsigned char b, unsigned char a)
    : r_(r), g_(g), b_(b), a_(a) {}

inline Color& Color::operator = (const Color& other)
{
    rgba_ = other.GetRGBA();
    return *this;
}

inline Color& Color::operator = (unsigned int rgba)
{
    SetValue(rgba);
    return *this;
}

inline unsigned char& Color::operator[](int index)
{
    return color_[index];
}

inline bool Color::operator == (Color& other) const
{
    return (r_ == other.r_ && g_ == other.g_ && b_ == other.b_ && a_ == other.a_);
}

inline bool Color::operator != (Color& other) const
{
    return !(operator==(other));
}

inline void Color::SetColor(unsigned char r, unsigned char g, unsigned char b)
{
    SetColor(r, g, b, 0);
}

inline void Color::SetColor(unsigned char r, unsigned char g, unsigned char b, unsigned char a)
{
    color_[0] = r;
    color_[1] = g;
    color_[2] = b;
    color_[3] = a;
}

inline void Color::SetValue(unsigned int rgba)
{
    rgba_ = rgba;
}

inline unsigned char Color::GetRed(void) const
{
    return r_;
}
inline unsigned char Color::GetGreen(void) const
{
    return g_;
}
inline unsigned char Color::GetBlue(void) const
{
    return b_;
}
inline unsigned char Color::GetAlpha(void) const
{
    return a_;
}
inline unsigned int Color::GetRGBA(void) const
{
    return rgba_;
}

inline float Color::NormalizeRed(void) const
{
    return NORMALIZE_COLOR(r_);
}
inline float Color::NormalizeGreen(void) const
{
    return NORMALIZE_COLOR(g_);
}
inline float Color::NormalizeBlue(void) const
{
    return NORMALIZE_COLOR(b_);
}
inline float Color::NormalizeAlpha(void) const
{
    return NORMALIZE_COLOR(a_);
}

inline unsigned char* Color::Ptr(void)
{
    return color_;
}

#endif // color_h__