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

typedef struct TextInfo_s
{
	GLuint vaoID;

	GLuint textureID;

	unsigned short offsetVerts;
	unsigned short countVerts;				// number of vertices
	
} TextInfo_t;

enum VertexFormats 
{
	FVF_Simple2DText,
	FVF_Simple2DColoredText,

	NUM_FVF,
};

typedef void(*VERTEXATTRIB)(void);

void DrawSimple2DText(const TextInfo_s & txtInfo);

void UploadFontTextureToGL( int w, int h, unsigned char* pRawTex, GLuint & idTex );

inline void FreeGLTexture( GLsizei num, GLuint const * idTex )
{
	glDeleteTextures(num, idTex);
}

inline void Set_Attrib2DText( void )
{
	// pointers
	glEnableVertexAttribArray(ATTRIB_POSITION);
	glVertexAttribPointer(ATTRIB_POSITION, 4, GL_FLOAT, GL_FALSE, 36, (GLvoid*)0);

	glEnableVertexAttribArray(ATTRIB_TEXCOORD0);
	glVertexAttribPointer(ATTRIB_TEXCOORD0, 4, GL_FLOAT, GL_FALSE, 36, (GLvoid*)16/*4*sizeof(float)*/);
}

inline void Reset_Attrib2DText( void )
{
	glDisableVertexAttribArray(ATTRIB_POSITION);
	glDisableVertexAttribArray(ATTRIB_TEXCOORD0);
}

inline void Set_Attrib2DTextColored( void )
{
	// pointers
	glEnableVertexAttribArray(ATTRIB_POSITION);
	glVertexAttribPointer(ATTRIB_POSITION, 4, GL_FLOAT, GL_FALSE, 36, (GLvoid*)0);

	glEnableVertexAttribArray(ATTRIB_TEXCOORD0);
	glVertexAttribPointer(ATTRIB_TEXCOORD0, 4, GL_FLOAT, GL_FALSE, 36, (GLvoid*)16/*4*sizeof(float)*/);

	glEnableVertexAttribArray(ATTRIB_COLOR);
	glVertexAttribPointer(ATTRIB_COLOR, 4, GL_UNSIGNED_BYTE, GL_FALSE, 36, (GLvoid*)32/*8*sizeof(float)*/);
}

inline void Reset_Attrib2DTextColored( void )
{
	glDisableVertexAttribArray(ATTRIB_POSITION);
	glDisableVertexAttribArray(ATTRIB_TEXCOORD0);
	glEnableVertexAttribArray(ATTRIB_COLOR);
}