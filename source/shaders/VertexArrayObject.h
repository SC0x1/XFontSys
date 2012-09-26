/****************************************************************************/
/*	Copyright (c) 2012 Vitaly Lyaschenko < scxv86@gmail.com >
/*
/*	Purpose: A wrapper for vertex array objects
/*
/****************************************************************************/
#pragma once

#include "shaders/gl_local.h"

class CVertexArray
{
public:

	CVertexArray( void );
	~CVertexArray();

	void Init(VERTEXATTRIB pSet, VERTEXATTRIB pReset)
	{
		m_pFuncSetAttrib = pSet;
		m_pFuncResetAttrib = pReset;

		m_bInit = true;
	}

	void SetAttrib( void ) const
	{
		glBindVertexArray(m_VAO_ID);
		m_pFuncSetAttrib();
	}

	void ResetAttrib( void ) const
	{
		m_pFuncResetAttrib();
	}

	static int BufferCount(void) { return s_BufferCount; }

private:

	VERTEXATTRIB m_pFuncSetAttrib;
	VERTEXATTRIB m_pFuncResetAttrib;

	static int s_BufferCount;

	GLuint m_VAO_ID;

	bool m_bInit;
};

int CVertexArray::s_BufferCount = 0;

inline CVertexArray::CVertexArray( void )
	: m_VAO_ID(-1), m_bInit(false)
{
	++s_BufferCount;

	glGenVertexArrays( 1, &m_VAO_ID );
}

inline CVertexArray::~CVertexArray()
{
	--s_BufferCount;

	if ( m_VAO_ID ) {
		glDeleteVertexArrays(1, &m_VAO_ID);
	}
}
