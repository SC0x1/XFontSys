/****************************************************************************/
/*	Copyright (c) 2012 Vitaly Lyaschenko < scxv86@gmail.com >
/*
/*	Purpose: Wrapper for GLSL shaders
/*
/****************************************************************************/
#pragma once

#include "shaders/gl_local.h"

#include <assert.h>

class CShaderOGL
{
	GLuint m_ProgramID;
	GLuint m_VertexShaderID;
	GLuint m_GeometryShaderID;
	GLuint m_FragmentShaderID;

public:

	CShaderOGL();
	~CShaderOGL();

	// links and build the shader program
	bool BuildShaderProgram(const char* pNameVertex, const char* pNameFragment);
	bool BuildShaderProgram(const char* pNameVertex, const char* pNameGeometry, const char* pNameFragment);
	bool BuildShaderProgramMem(const void* pLocVertex, const void* pLocFragment);
	bool BuildShaderProgramMem(const void* pLocVertex, const void* pLocGeometry, const void* pLocFragment);
	bool BuildVertexShader(const char* pNameVertex);
	bool BuildGeometryShader(const char* pNameGeometry);
	bool BuildFragmentShader(const char* pNameFragment);

	bool DestroyShaderProgram(void);
	bool DestroyVertexShader(void);
	bool DestroyGeometryShader(void);
	bool DestroyFragmentShader(void);

	bool ValidateShaderProgram(void);
	
	// Vertex attribute

	void Get_AttribLocation(const char* name, int *location)
	{
		*location = glGetAttribLocation(m_ProgramID, name);
	}

	void BindAttribLocation(const char* name, unsigned int	index)
	{
		glBindAttribLocation(m_ProgramID, index, name);
	}

	void Get_UniformLocation(const char* name, int *location)
	{
		*location = glGetUniformLocation(m_ProgramID, name);
	}

	// Sets Uniform Matrix

	bool Set_Matrix2fv(const GLfloat *V, const char* nameUnif, int *location=nullptr, int count=1, bool transpose=1);
	void Set_Matrix2fv(const float *V, const int location, int count=1, bool transpose=1)
	{
		assert(V);
		glUniformMatrix2fv(location, count, transpose, V);
	}

	bool Set_Matrix3fv(const GLfloat *V, const char* nameUnif, int *location=nullptr, int count=1, bool transpose=1);
	void Set_Matrix3fv(const float *V, const int location, int count=1, bool transpose=1)
	{
		assert(V);
		glUniformMatrix3fv(location, count, transpose, V);
	}

	bool Set_Matrix4fv(const GLfloat *V, const char* nameUnif, int *location=nullptr, int count=1, bool transpose=1);
	void Set_Matrix4fv(const float *V, const int location, int count=1, bool transpose=1)
	{
		assert(V);
		glUniformMatrix4fv(location, count, transpose, V);
	}

	// Sets Uniform Integers

	bool Set_Integer1(int V, const char* nameUnif, int *location=nullptr);
	void Set_Integer1(int V, const int location)
	{
		glUniform1i(location, V);
	}

	bool Set_Integer2v(int *V, const char* nameUnif, int *location=nullptr, int count=1);
	void Set_Integer2v(int *V, const int location, int count=1)
	{
		assert(V);
		glUniform2iv(location, count, V);
	}

	bool Set_Integer2(int V0, int V1, const char* nameUnif, int *location=nullptr);
	void Set_Integer2(int V0, int V1, const int location)
	{
		glUniform2i(location, V0, V1);
	}

	bool Set_Integer3v(int* V, const char* nameUnif, int *location=nullptr, int count=1);
	void Set_Integer3v(int* V, const int location, int count=1)
	{
		assert(V);
		glUniform3iv(location, count, V);
	}

	bool Set_Integer3(int V0, int V1, int V2, const char* nameUnif, int *location=nullptr);
	void Set_Integer3(int V0, int V1, int V2, const int location)
	{
		glUniform3i(location, V0, V1, V2);
	}

	bool Set_Integer4v(int* V, const char* nameUnif, int *location=nullptr, int count=1);
	void Set_Integer4v(int* V, const int location, int count=1)
	{
		assert(V);
		glUniform4iv(location, count, V);
	}

	bool Set_Integer4(int V0, int V1, int V2, int V3, const char* nameUnif, int *location=nullptr);
	void Set_Integer4(int V0, int V1, int V2, int V3, const int location)
	{
		glUniform4i(location, V0, V1, V2, V3);
	}

	// Sets Uniform Floats

	bool Set_Float1(float V, const char* nameUnif, int *location=nullptr);
	void Set_Float1(float V, const int location)
	{
		glUniform1f( location, V );
	}

	bool Set_Float2v(float *V, const char* nameUnif, int *location=nullptr, int count=1);
	void Set_Float2v(float *V, const int location, int count=1)
	{
		assert(V);
		glUniform2fv(location, count, V);
	}

	bool Set_Float2(float V0, float V1, const char* nameUnif, int *locatio = nullptr);
	void Set_Float2(float V0, float V1, const int location)
	{
		glUniform2f(location, V0, V1);
	}

	bool Set_Float3v(float *V, const char* nameUnif, int *location=nullptr, int count=1);
	void Set_Float3v(float *V, const int location, int count=1)
	{
		assert(V);
		glUniform3fv(location, count, V);
	}

	bool Set_Float3(float V0, float V1, float V2, const char* nameUnif, int *location=nullptr);
	void Set_Float3(float V0, float V1, float V2, const int location)
	{
		glUniform3f(location, V0, V1, V2);
	}

	bool Set_Float4v(float *V, const char* nameUnif, int *location=nullptr, int count=1);
	void Set_Float4v(float *V, const int location, int count=1)
	{
		assert(V);
		glUniform4fv(location, count, V);
	}

	bool Set_Float4(float V0, float V1, float V2, float V3, const char* nameUnif, int *location=nullptr);
	void Set_Float4(float V0, float V1, float V2, float V3, const int location)
	{
		glUniform4f(location, V0, V1, V2, V3);
	}

	bool Set_Parameter(int *V, const char* nameUnif, int numParameter, int *location);
	bool Set_Parameter(float *V, const char* nameUnif, int numParameter, int *location);

	// Begin/End usage the shader program

	void Begin_Use();
	void End_Use();

private:
	CShaderOGL(const CShaderOGL&);
	CShaderOGL& operator = (const CShaderOGL&);
};
