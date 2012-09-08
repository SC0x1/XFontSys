/****************************************************************************/
/*	Copyright (c) 2012 Vitaly Lyaschenko < scxv86@gmail.com >
/*
/*	Purpose:
/*
/****************************************************************************/

#pragma once

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string>

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__)

#define GLEW_STATIC
#include <GL/glew.h>
#include <GL/wglew.h>

#else
#include <GL/glew.h>
#endif

// Important! include GLFW header after the header Wrangler Library (GLEW)
#include <GL/glfw.h>

#include "logger.h"

float GetFPS(float interval = 1.0);

void NowTime( std::string& str );

/*---------------------------------------------------------------------------*/
/* Purpose: Initialize the GLEW library
/*---------------------------------------------------------------------------*/
bool InitGLEW( void );

/*---------------------------------------------------------------------------*/
/* Gets general information about the video system
/*---------------------------------------------------------------------------*/
void PrintVideoInfo();

char* const ReadWholeFileIntoString(const char *fileName, int *lenFile = nullptr);


bool GetRelativePathToResources(std::string& out_path);

bool SetWorkingDirectory( void );

