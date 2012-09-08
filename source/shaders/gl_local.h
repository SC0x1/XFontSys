/****************************************************************************/
/*	Copyright (c) 2012 Vitaly Lyaschenko < scxv86@gmail.com >
/*
/*	Purpose:
/*
/****************************************************************************/
#pragma once

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__)

#define GLEW_STATIC
#include <GL/glew.h>
#include <GL/wglew.h>

#else
#include <GL/glew.h>
#endif
#

/*
	location vertex attributes in the shader program
*/
#define ATTRIB_POSITION		0
#define ATTRIB_NORMAL		1
#define ATTRIB_COLOR		2
#define ATTRIB_TANGENT		3
#define ATTRIB_BITANGENT	4
#define ATTRIB_TEXCOORD0	5
#define ATTRIB_TEXCOORD1	6
#define ATTRIB_TEXCOORD2	7
#define ATTRIB_TEXCOORD3	8
#define ATTRIB_TEXCOORD4	9
#define ATTRIB_TEXCOORD5	10
#define ATTRIB_TEXCOORD6	11
#define ATTRIB_TEXCOORD7	12
