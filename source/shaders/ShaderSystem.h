/*
	Copyright (c) 2012 Vitaly Lyaschenko "scxv86@gmail.com"

	Purpose: Wrapper for the GLSL shaders
	
*/
#pragma once
 
#include "shaders/gl_local.h"

#include <assert.h>

class CShaderOGL
{
	GLuint m_ProgramID;
	GLuint m_VertexShaderID;
	GLuint m_GeometryShaderID;
	GLuint m_FragmentShaderID;

	bool m_bInit;
	bool m_bIsTranspMat;

public:

	CShaderOGL( bool IsTranspMat = true );
	~CShaderOGL();

	// links and build the shader program
	bool BuildShaderProgram( const char* pNameVertexFile, const char* pNameFragmentFile );
	bool BuildShaderProgram( const char* pNameVertexFile, const char* pNameGeometryFile, const char* pNameFragmentFile );
	bool BuildShaderProgramMem( const void* pLocVertex, const void* pLocFragment);
	bool BuildShaderProgramMem( const void* pLocVertex, const void* pLocGeometry, const void* pLocFragment );

	bool DestroyShaderProgram(void);

	bool ValidateShaderProgram(void);
	
	// Vertex attribute

	int Get_AttribLocation( const char* nameAttrib )
	{
		assert( nameAttrib );

		int location(-1);

		if ( (location = glGetAttribLocation(m_ProgramID, nameAttrib)) < 0)
			return -1;

		return location;
	}

	void BindAttribLocation( const char* name, GLuint index )
	{
		assert( name );
		glBindAttribLocation(m_ProgramID, index, name);
	}

	int Get_UniformLocation( const char* nameUnif )
	{
		assert( nameUnif );

		int location(-1);

		if ( (location = glGetUniformLocation(m_ProgramID, nameUnif)) < 0)
			return -1;

		return location;
	}

	// Sets Uniform Matrix

	int Set_Matrix2fv( const float *V, const char* nameUnif, int count = 1 );
	void Set_Matrix2fv( const float *V, const int location, int count = 1 )
	{
		assert(V);
		glUniformMatrix2fv(location, count, m_bIsTranspMat, V);
	}

	int Set_Matrix3fv( const float *V, const char* nameUnif, int count = 1 );
	void Set_Matrix3fv( const float *V, const int location, int count = 1 )
	{
		assert(V);
		glUniformMatrix3fv(location, count, m_bIsTranspMat, V);
	}

	int Set_Matrix4fv( const float *V, const char* nameUnif, int count = 1 );
	void Set_Matrix4fv( const float *V, const int location, int count = 1 )
	{
		assert(V);
		glUniformMatrix4fv(location, count, m_bIsTranspMat, V);
	}

	// Sets Uniform Integers

	int Set_Integer1( int V, const char* nameUnif );
	void Set_Integer1( int V, const int location )
	{
		glUniform1i(location, V);
	}

	int Set_Integer2v( int *V, const char* nameUnif, int count = 1 );
	void Set_Integer2v( int *V, const int location, int count = 1 )
	{
		assert(V);
		glUniform2iv(location, count, V);
	}

	int Set_Integer2( int V0, int V1, const char* nameUnif );
	void Set_Integer2( int V0, int V1, const int location )
	{
		glUniform2i(location, V0, V1);
	}

	int Set_Integer3v( int* V, const char* nameUnif, int count = 1 );
	void Set_Integer3v( int* V, const int location, int count = 1 )
	{
		assert(V);
		glUniform3iv(location, count, V);
	}

	int Set_Integer3( int V0, int V1, int V2, const char* nameUnif );
	void Set_Integer3( int V0, int V1, int V2, const int location )
	{
		glUniform3i(location, V0, V1, V2);
	}

	int Set_Integer4v( int* V, const char* nameUnif, int count = 1 );
	void Set_Integer4v( int* V, const int location, int count = 1 )
	{
		assert(V);
		glUniform4iv(location, count, V);
	}

	int Set_Integer4( int V0, int V1, int V2, int V3, const char* nameUnif );
	void Set_Integer4( int V0, int V1, int V2, int V3, const int location )
	{
		glUniform4i(location, V0, V1, V2, V3);
	}

	// Sets Uniform Floats

	int Set_Float1( float V, const char* nameUnif );
	void Set_Float1( float V, const int location )
	{
		glUniform1f( location, V );
	}

	int Set_Float2v( float *V, const char* nameUnif, int count = 1 );
	void Set_Float2v( float *V, const int location, int count = 1 )
	{
		assert(V);
		glUniform2fv(location, count, V);
	}

	int Set_Float2( float V0, float V1, const char* nameUnif );
	void Set_Float2( float V0, float V1, const int location )
	{
		glUniform2f(location, V0, V1);
	}

	int Set_Float3v( float *V, const char* nameUnif, int count = 1 );
	void Set_Float3v( float *V, const int location, int count = 1 )
	{
		assert(V);
		glUniform3fv(location, count, V);
	}

	int Set_Float3( float V0, float V1, float V2, const char* nameUnif );
	void Set_Float3( float V0, float V1, float V2, const int location )
	{
		glUniform3f(location, V0, V1, V2);
	}

	int Set_Float4v( float *V, const char* nameUnif, int count = 1 );
	void Set_Float4v( float *V, const int location, int count = 1 )
	{
		assert(V);
		glUniform4fv(location, count, V);
	}

	int Set_Float4( float V0, float V1, float V2, float V3, const char* nameUnif );
	void Set_Float4( float V0, float V1, float V2, float V3, const int location )
	{
		glUniform4f(location, V0, V1, V2, V3);
	}

	int Set_Parameter( int *V, const char* nameUnif, int numParameter );
	int Set_Parameter( float *V, const char* nameUnif, int numParameter );

	// Begin/End usage the shader program

	void Begin_Use();
	void End_Use();

private:
	CShaderOGL(const CShaderOGL&);
	CShaderOGL& operator = (const CShaderOGL&);
};
