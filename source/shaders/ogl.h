#ifndef ogl_h__
#define ogl_h__

#pragma once

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__)

#define GLEW_STATIC
#include <GL/glew.h>
#include <GL/wglew.h>

#else
#include <GL/glew.h>
#endif

// Location vertex attributes in the shader program
enum VertexAttrib
{
    VA_POSITION = 0,
    VA_NORMAL = 1,
    VA_COLOR = 2,
    VA_TANGENT = 3,
    VA_BITANGENT = 4,

    VA_TEXCOORD0 = 5,
    VA_TEXCOORD1 = 6,
    VA_TEXCOORD2 = 7,
    VA_TEXCOORD3 = 8,
    VA_TEXCOORD4 = 9,
    VA_TEXCOORD5 = 10,
    VA_TEXCOORD6 = 11,
    VA_TEXCOORD7 = 12,
};

#endif // ogl_h__