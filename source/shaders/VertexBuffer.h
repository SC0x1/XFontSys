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

	CVertexBuffer(VertexFormats format, int vertexSize, int vertexCount, bool dynamic);
	~CVertexBuffer();

	int ReInit(VertexFormats format, int vertexSize, int vertexCount, bool dynamic);

	int PushVertexData(int vertexCount, GLvoid  const* pData);

	void Clear( void )
	{
		m_BorderPosition = 0;
	}

	void* Lock( int vertexCount, int& baseVertexIndex );

	void Unlock( int vertexCount );

	int VertexSize() const { return m_VertexSize; }

	int VertexCount() const { return m_VertexCount; }

	int VertexArrayObjectID() const { return m_VAOID; }

	static int BufferCount() { return s_BufferCount; }

	// хватит ли места в буфере для указанного количества вершин
	bool HasEnoughSpace( int numVertices ) const
	{
		return ( numVertices + m_BorderPosition) <= m_VertexCount;
	}

private:

	void InitVertexPointer( VertexFormats format );

	VERTEXATTRIB m_pfSetAttrib;
	VERTEXATTRIB m_pfResetAttrib;

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
inline CVertexBuffer::CVertexBuffer(VertexFormats format, int vertexSize, int vertexCount, bool dynamic)
	: m_bLock(false), m_VBOID(-1), m_VAOID(-1), m_BorderPosition(0),
	m_VertexCount(vertexCount), m_VertexSize(vertexSize),
	m_bDynamic(dynamic)
{
	++s_BufferCount;

	m_pfSetAttrib = nullptr;
	m_pfResetAttrib = nullptr;

	InitVertexPointer( format );

	glGenVertexArrays( 1, &m_VAOID );

	glGenBuffers( 1, &m_VBOID );

	glBindVertexArray(m_VAOID);

	PushVertexData(m_VertexCount, nullptr);

	glBindVertexArray(0);
}

inline CVertexBuffer::~CVertexBuffer()
{
	--s_BufferCount;

	Unlock(0);

	if ( m_VBOID > 0) {
		glDeleteBuffers(1, &m_VBOID);
	}

	if (m_VAOID > 0 ) {
		glDeleteVertexArrays(1, &m_VAOID);
	}
}

inline int CVertexBuffer::ReInit(VertexFormats format, int vertexSize, int vertexCount, bool dynamic)
{
	m_VertexSize = vertexSize;
	m_VertexCount = vertexCount;
	m_bDynamic = dynamic;

	m_BorderPosition = 0;

	InitVertexPointer( format );

	glBindVertexArray(m_VAOID);

	const int bytes = PushVertexData(m_VertexCount, nullptr);

	glBindVertexArray(0);

	return bytes;
}

inline int CVertexBuffer::PushVertexData(int vertexCount, GLvoid  const* pData)
{
	glBindBuffer( GL_ARRAY_BUFFER, m_VBOID );

	const int size = vertexCount * m_VertexSize;

	if (m_bDynamic)	{
		m_BorderPosition = vertexCount;
		glBufferData( GL_ARRAY_BUFFER, size, pData, GL_DYNAMIC_DRAW );
	} else {
		glBufferData( GL_ARRAY_BUFFER, size, pData, GL_STATIC_DRAW );
	}

	m_pfSetAttrib();

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

		m_pfSetAttrib();

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

inline void CVertexBuffer::InitVertexPointer( VertexFormats format )
{
	if(format == FVF_Simple2DText)
	{
		m_pfSetAttrib   = Set_Attrib2DText;
		m_pfResetAttrib = Reset_Attrib2DText;
	}
	else if(format == FVF_Simple2DColoredText)
	{
		m_pfSetAttrib   = Set_Attrib2DTextColored;
		m_pfResetAttrib = Reset_Attrib2DTextColored;
	}

	/* ... */
}
