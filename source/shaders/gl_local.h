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
	GLuint VAO;

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

inline void DrawSimple2DText(TextInfo_s& txtInfo)
{
	glBindTexture( GL_TEXTURE_2D, txtInfo.textureID );

	glBindVertexArray( txtInfo.VAO );

	glEnable( GL_BLEND );
	glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

	glDrawArrays( GL_POINTS, txtInfo.offsetVerts, txtInfo.countVerts );

	glDisable( GL_BLEND );

	glBindVertexArray(0);
}

inline void UploadFontTextureToGL(int w, int h, unsigned char* pRawTex, GLuint &idTex)
{
	if (!pRawTex)
		return;

	glGenTextures(1, &idTex);

	glActiveTexture(GL_TEXTURE0);

	glBindTexture(GL_TEXTURE_2D, idTex);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, w, h, 0, GL_RED, GL_UNSIGNED_BYTE, pRawTex);

	glBindTexture(GL_TEXTURE_2D, 0);
}