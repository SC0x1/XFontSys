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
#define ATTRIB_POSITION  0
#define ATTRIB_TEXCOORD0 1
#define ATTRIB_COLOR     2