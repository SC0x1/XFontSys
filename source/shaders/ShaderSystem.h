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
	bool BuildShaderProgram( const char* pSrcVertex, const char* pSrcFragment);
	bool BuildShaderProgram( const char* pSrcVertex, const char* pSrcGeometry, const char* pSrcFragment );

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

	void BindAttribLocation( const char* nameAttrib, GLuint index )
	{
		assert( nameAttrib );
		glBindAttribLocation(m_ProgramID, index, nameAttrib);
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

	int Set_Matrix2fv( const char* nameUnif, const float *V, int count = 1 );
	void Set_Matrix2fv( const int location, const float *V, int count = 1 )
	{
		assert(V);
		glUniformMatrix2fv(location, count, m_bIsTranspMat, V);
	}

	int Set_Matrix3fv( const char* nameUnif, const float *V, int count = 1 );
	void Set_Matrix3fv( const int location, const float *V, int count = 1 )
	{
		assert(V);
		glUniformMatrix3fv(location, count, m_bIsTranspMat, V);
	}

	int Set_Matrix4fv( const char* nameUnif, const float *V, int count = 1 );
	void Set_Matrix4fv( const int location, const float *V, int count = 1 )
	{
		assert(V);
		glUniformMatrix4fv(location, count, m_bIsTranspMat, V);
	}

	// Sets Uniform Integers

	int Set_Integer1( const char* nameUnif, int V );
	void Set_Integer1( const int location, int V )
	{
		glUniform1i(location, V);
	}

	int Set_Integer2v( const char* nameUnif, int *V, int count = 1 );
	void Set_Integer2v( const int location, int *V, int count = 1 )
	{
		assert(V);
		glUniform2iv(location, count, V);
	}

	int Set_Integer2( const char* nameUnif, int V0, int V1 );
	void Set_Integer2( const int location, int V0, int V1 )
	{
		glUniform2i(location, V0, V1);
	}

	int Set_Integer3v( const char* nameUnif, int* V, int count = 1 );
	void Set_Integer3v( const int location, int* V, int count = 1 )
	{
		assert(V);
		glUniform3iv(location, count, V);
	}

	int Set_Integer3( const char* nameUnif, int V0, int V1, int V2 );
	void Set_Integer3( const int location, int V0, int V1, int V2 )
	{
		glUniform3i(location, V0, V1, V2);
	}

	int Set_Integer4v( const char* nameUnif, int* V, int count = 1 );
	void Set_Integer4v( const int location, int* V, int count = 1 )
	{
		assert(V);
		glUniform4iv(location, count, V);
	}

	int Set_Integer4( const char* nameUnif, int V0, int V1, int V2, int V3 );
	void Set_Integer4( const int location, int V0, int V1, int V2, int V3 )
	{
		glUniform4i(location, V0, V1, V2, V3);
	}

	// Sets Uniform Floats

	int Set_Float1( const char* nameUnif, float V );
	void Set_Float1( const int location, float V )
	{
		glUniform1f( location, V );
	}

	int Set_Float2v( const char* nameUnif, float *V,  int count = 1 );
	void Set_Float2v( const int location, float *V,  int count = 1 )
	{
		assert(V);
		glUniform2fv(location, count, V);
	}

	int Set_Float2( const char* nameUnif, float V0, float V1 );
	void Set_Float2( const int location, float V0, float V1 )
	{
		glUniform2f(location, V0, V1);
	}

	int Set_Float3v( const char* nameUnif, float *V,  int count = 1 );
	void Set_Float3v( const int location, float *V,  int count = 1 )
	{
		assert(V);
		glUniform3fv(location, count, V);
	}

	int Set_Float3( const char* nameUnif, float V0, float V1, float V2 );
	void Set_Float3( const int location, float V0, float V1, float V2 )
	{
		glUniform3f(location, V0, V1, V2);
	}

	int Set_Float4v( const char* nameUnif, float *V, int count = 1 );
	void Set_Float4v( const int location, float *V,  int count = 1 )
	{
		assert(V);
		glUniform4fv(location, count, V);
	}

	int Set_Float4( const char* nameUnif, float V0, float V1, float V2, float V3 );
	void Set_Float4( const int location, float V0, float V1, float V2, float V3 )
	{
		glUniform4f(location, V0, V1, V2, V3);
	}

	int Set_Parameter( const char* nameUnif, int *V,  int numParameter );
	int Set_Parameter( const char* nameUnif, float *V,  int numParameter );

	// Begin/End usage the shader program

	void Begin_Use();
	void End_Use();

private:
	CShaderOGL(const CShaderOGL&);
	CShaderOGL& operator = (const CShaderOGL&);
};
