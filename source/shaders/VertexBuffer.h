/****************************************************************************/
/*	Copyright (c) 2012 Vitaly Lyaschenko < scxv86@gmail.com >
/*
/*	Purpose: A wrapper for vertex buffer objects
/*
/****************************************************************************/
#pragma once

#include "shaders/gl_local.h"

class CVertexBuffer
{
public:

	CVertexBuffer(VERTEXATTRIB pSetAttrib, int vertexSize, int vertexCount, bool dynamic);
	~CVertexBuffer();

	int PushVertexData(int vertexCount, GLvoid  const* pData);

	int ReInit(VERTEXATTRIB pSetAttrib, int vertexSize, int vertexCount, bool dynamic);

	void Clear( void )
	{
		m_BorderPosition = 0;
	}

	void SetAttrib(void) const
	{
		glBindBuffer( GL_ARRAY_BUFFER, m_VBOID );
	}

	void ResetAttrib(void) const
	{
		glBindBuffer( GL_ARRAY_BUFFER, 0 );
	}

	void* Lock( int vertexCount, int& baseVertexIndex );

	void Unlock( int vertexCount );

	int VertexSize() const { return m_VertexSize; }

	int VertexCount() const { return m_VertexCount; }

	static int BufferCount() { return s_BufferCount; }

	// хватит ли места в буфере для указанного количества вершин
	bool HasEnoughSpace( int numVertices ) const
	{
		return ( numVertices + m_BorderPosition) <= m_VertexCount;
	}

private:

	VERTEXATTRIB m_pFuncSetAttrib;

	static int s_BufferCount;

	// id VBO в видеопамяти
	GLuint m_VBOID;

	GLuint m_VAOID;

	// общее количество вертексов которое влезет в буфер
	int m_VertexCount;

	int m_BorderPosition;
	int m_VertexSize;

	bool m_bDynamic;
	bool m_bLock;
};

int CVertexBuffer::s_BufferCount = 0;

//-----------------------------------------------------------------------------
// Purpose: конструктор для динамического VBO 
// Input  : int vertexSize - размер одной вершины в буфере ( к примеру xyz + uv = ( 3float + 2float ) * sizeof(float) =  20 байт )
//			int vertexCount - максимальное количество вершин содержащихся в буфере
//-----------------------------------------------------------------------------
inline CVertexBuffer::CVertexBuffer(VERTEXATTRIB pSetAttrib, int vertexSize, int vertexCount, bool dynamic)
	: m_bLock(false), m_VBOID(-1), m_VAOID(-1), m_BorderPosition(0),
	m_VertexCount(vertexCount), m_VertexSize(vertexSize),
	m_bDynamic(dynamic)
{
	++s_BufferCount;

	m_pFuncSetAttrib = pSetAttrib;

	glGenBuffers( 1, &m_VBOID );

	PushVertexData(m_VertexCount, nullptr);
}

inline CVertexBuffer::~CVertexBuffer()
{
	--s_BufferCount;

	Unlock(0);

	if ( m_VBOID ) {
		glDeleteBuffers(1, &m_VBOID);
	}
}

inline int CVertexBuffer::ReInit(VERTEXATTRIB pSetAttrib, int vertexSize, int vertexCount, bool dynamic)
{
	m_pFuncSetAttrib = pSetAttrib;
	m_VertexSize = vertexSize;
	m_VertexCount = vertexCount;
	m_bDynamic = dynamic;

	m_BorderPosition = 0;

	return PushVertexData(m_VertexCount, nullptr);
}

inline int CVertexBuffer::PushVertexData(int vertexCount, GLvoid  const* pData)
{
	glBindBuffer( GL_ARRAY_BUFFER, m_VBOID );

	m_pFuncSetAttrib();

	const int size = vertexCount * m_VertexSize;

	if (m_bDynamic)	{
		m_BorderPosition = vertexCount;
		glBufferData( GL_ARRAY_BUFFER, size, pData, GL_DYNAMIC_DRAW );
	} else {
		glBufferData( GL_ARRAY_BUFFER, size, pData, GL_STATIC_DRAW );
	}

	glBindBuffer( GL_ARRAY_BUFFER, 0 );

	return size;
}

inline void* CVertexBuffer::Lock( int vertexCount, int& baseVertexIndex )
{	
	baseVertexIndex = 0;

	if ( vertexCount > m_VertexCount )
		return nullptr; 

	void* ptr = nullptr;

	if (!m_bLock)
	{
		glBindBuffer( GL_ARRAY_BUFFER, m_VBOID );

		ptr = glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
		baseVertexIndex = m_BorderPosition;

		m_bLock = true;
	}
	return ptr;
}

inline void CVertexBuffer::Unlock( int vertexCount )
{
	if (m_bLock && (m_VBOID))
	{
		m_BorderPosition += vertexCount;

		glUnmapBuffer(GL_ARRAY_BUFFER);

		glBindBuffer( GL_ARRAY_BUFFER, 0 );

		m_bLock = false;
	}
}
