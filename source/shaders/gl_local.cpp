/****************************************************************************/
/*	Copyright (c) 2012 Vitaly Lyaschenko < scxv86@gmail.com >
/*
/*	Purpose:
/*
/****************************************************************************/

#include "gl_local.h"

void DrawSimple2DText(const TextInfo_s& txtInfo)
{
	glBindTexture( GL_TEXTURE_2D, txtInfo.textureID );

	glEnable( GL_BLEND );
	glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

	glBindVertexArray(txtInfo.vaoID);
	glDrawArrays( GL_POINTS, txtInfo.offsetVerts, txtInfo.countVerts );
	glBindVertexArray(0);

	glDisable( GL_BLEND );
}

void UploadFontTextureToGL(int w, int h, unsigned char* pRawTex, GLuint &idTex)
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