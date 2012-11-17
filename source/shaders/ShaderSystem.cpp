/*
	Copyright (c) 2012 Vitaly Lyaschenko "scxv86@gmail.com"

	Purpose: Wrapper for the GLSL shaders
	
*/

#include "shaders/ShaderSystem.h"
#include <stdio.h>

#define ASSERT(x) assert(x)

/*---------------------------------------------------------------------------*/
/* Purpose: check the status of compiling shaders
/* Input  : id shader
/* Output : returns true on success
/*---------------------------------------------------------------------------*/
bool Check_Shader(GLuint checkShaderID)
{
	GLint  compiled = GL_FALSE;

	glGetShaderiv(checkShaderID, GL_COMPILE_STATUS, &compiled);

	if (!compiled)
	{
		static char bufferErr[512];
		GLint length;
		glGetShaderInfoLog(checkShaderID, sizeof(bufferErr), &length, bufferErr);
		fprintf(stderr, "\nShader Error: %s\n", bufferErr);
		return false;
	}

	return true;
}

/*---------------------------------------------------------------------------*/
/* Purpose: check the status of shader program
/* Input  : program - id the shader program
/*			param - specifies the object parameter
/* Output : returns true on success
/*---------------------------------------------------------------------------*/
inline bool Check_Program(GLuint program, GLenum param)
{
	GLint  status = GL_FALSE;

	glGetProgramiv(program, param, &status);

	if (!status)
	{
		static GLchar bufferErr[512];
		GLint length;
		glGetProgramInfoLog( program, sizeof(bufferErr), &length, bufferErr );
		fprintf(stderr,"\nShader Program %d Error: %s\n", program, bufferErr);
		return false;
	}

	return true;
}

//---------------------------------------------------------------------------*/
// Purpose: Validation the shader program
// Input  : GLuint program	- shader program ID
// Output : return true if success
//---------------------------------------------------------------------------*/
inline bool ValidateProgram(GLuint program)
{
	glValidateProgram(program);

	return Check_Program(program, GL_VALIDATE_STATUS) == true;
}

//---------------------------------------------------------------------------*/
// Purpose: Creation and compile shader from source.
// Input  : GLenum type			- enum shader type,
//			const char* pSource	- pointer to shader source
// Output : If successful creation and compilation	- return shader ID,
//			otherwise will be returned 0;
//---------------------------------------------------------------------------*/
inline GLuint CreateShader(GLenum type, const char* pSource)
{
	ASSERT(pSource);

	GLuint shaderID = glCreateShader(type);

	if (shaderID == GL_FALSE)
		return 0;

	const int lenghtSource = strlen(pSource);

	glShaderSource(shaderID, 1, (const char**)&pSource, &lenghtSource);

	glCompileShader(shaderID);

	if (!Check_Shader(shaderID))
	{
		glDeleteShader(shaderID);
		shaderID = 0;
	}

	return shaderID;
}

/*---------------------------------------------------------------------------*/
/* Purpose: removing shader
/* Input : shader - id shader
/*---------------------------------------------------------------------------*/
inline bool DestroyShader(GLuint shaderID)
{
	if( glIsShader(shaderID) == GL_TRUE )
	{
		glDeleteShader(shaderID);
		return true;
	}
	return false;
}

/*---------------------------------------------------------------------------*/
/* Purpose: Create and link shader program
/* Input  : vertShaderID/fragShaderID - id vertex/fragment shader
/* Output : program - id шейдерной программы
/*---------------------------------------------------------------------------*/
inline GLuint CreateShaderProgram(GLuint vertShaderID, GLuint fragShaderID)
{
	GLuint program = glCreateProgram();

	if (program == GL_FALSE)
		return 0;

	glAttachShader(program, vertShaderID);
	glAttachShader(program, fragShaderID);

	glLinkProgram(program);

	if (!Check_Program(program, GL_LINK_STATUS))
	{
		glDetachShader(program, vertShaderID);
		glDetachShader(program, fragShaderID);
		glDeleteProgram( program );
		program = 0;
	}

	return program;
}

inline GLuint CreateShaderProgram(GLuint vertShaderID, GLuint geomShaderID, GLuint fragShaderID)
{
	GLuint program = glCreateProgram();

	if (program == GL_FALSE)
		return 0;

	glAttachShader(program, vertShaderID);
	glAttachShader(program, geomShaderID);
	glAttachShader(program, fragShaderID);

	glLinkProgram(program);

	if (!Check_Program(program, GL_LINK_STATUS))
	{
		glDetachShader(program, vertShaderID);
		glDetachShader(program, geomShaderID);
		glDetachShader(program, fragShaderID);
		glDeleteProgram( program );
		program = 0;
	}

	return program;
}

inline bool DestroyProgram(GLuint program)
{
	if (glIsProgram(program) == GL_TRUE)
	{
		glUseProgram(0);
		glDeleteProgram(program);
		return true;
	}

	return false;
}

CShaderOGL::CShaderOGL( bool Transpoce ) 
:	m_VertexShaderID(0),
	m_FragmentShaderID(0),
	m_GeometryShaderID(0),
	m_ProgramID(0),
	m_bIsTranspMat(Transpoce),
	m_bInit(false)
{ }

CShaderOGL::~CShaderOGL()
{
	DestroyShaderProgram();
}

bool CShaderOGL::BuildShaderProgram(const char* pSrcVertex, const char* pSrcFragment)
{
	ASSERT(pSrcVertex && pSrcFragment);
	ASSERT((m_FragmentShaderID == 0) && (m_VertexShaderID == 0));
	ASSERT((m_ProgramID == 0) && !m_bInit);

	m_VertexShaderID = CreateShader(GL_VERTEX_SHADER, pSrcVertex);
	if (m_VertexShaderID == 0)
	{
		return false;
	}

	m_FragmentShaderID = CreateShader(GL_FRAGMENT_SHADER, pSrcFragment);
	if (m_FragmentShaderID == 0)
	{
		this->DestroyShaderProgram();
		return false;
	}

	m_ProgramID = CreateShaderProgram(m_VertexShaderID, m_FragmentShaderID);
	if (m_ProgramID == 0)
	{
		this->DestroyShaderProgram();
		return false;
	}

	return true;
}

bool CShaderOGL::BuildShaderProgram(const char* pSrcVertex, const char* pSrcGeometry, const char* pSrcFragment)
{
	ASSERT(pSrcVertex && pSrcFragment && pSrcGeometry);
	ASSERT((m_FragmentShaderID == 0) && (m_GeometryShaderID == 0) && (m_VertexShaderID == 0));
	ASSERT((m_ProgramID == 0) && !m_bInit);

	m_VertexShaderID = CreateShader(GL_VERTEX_SHADER, pSrcVertex);
	if (m_VertexShaderID == 0)
	{
		return false;
	}

	m_GeometryShaderID = CreateShader(GL_GEOMETRY_SHADER, pSrcGeometry);
	if (m_GeometryShaderID == 0)
	{
		this->DestroyShaderProgram();
		return false;
	}

	m_FragmentShaderID = CreateShader(GL_FRAGMENT_SHADER, pSrcFragment);
	if (m_FragmentShaderID == 0)
	{
		this->DestroyShaderProgram();
		return false;
	}

	m_ProgramID = CreateShaderProgram(m_VertexShaderID, m_GeometryShaderID, m_FragmentShaderID);
	if (m_ProgramID == 0)
	{
		this->DestroyShaderProgram();
		return false;
	}

	return true;
}

bool CShaderOGL::ValidateShaderProgram()
{
	return ValidateProgram(m_ProgramID);
}

bool CShaderOGL::DestroyShaderProgram()
{
	if (m_VertexShaderID > 0)
		DestroyShader(m_VertexShaderID);

	if (m_GeometryShaderID > 0)
		DestroyShader(m_GeometryShaderID);

	if (m_FragmentShaderID > 0)
		DestroyShader(m_FragmentShaderID);

	if (m_ProgramID > 0 )
		DestroyProgram(m_ProgramID);

	m_bInit = false;

	return true;
}

int CShaderOGL::Set_Parameter( const char* nameUnif, int *V, int numParameter )
{
	ASSERT(V && nameUnif);
	ASSERT(numParameter < 0 && numParameter > 5);

	int location(-1);

	switch(numParameter) {
	case 1: {
		location = glGetUniformLocation(m_ProgramID, nameUnif);
		if (location < 0)
			return -1;
			glUniform1iv(location, numParameter, V);
		} break;
	case 2: {
		location = glGetUniformLocation(m_ProgramID, nameUnif);
		if (location < 0)
			return -2;
			glUniform2iv(location, numParameter, V);
		} break;
	case 3:	{
		location = glGetUniformLocation(m_ProgramID, nameUnif);
		if (location < 0)
			return -3;
			glUniform3iv(location, numParameter, V);
		} break;
	case 4:	{
		location = glGetUniformLocation(m_ProgramID, nameUnif);
		if (location < 0)
			return -4;
			glUniform4iv(location, numParameter, V);
		} break;
	default:
		return -5;
	}

	return location;
}

int CShaderOGL::Set_Parameter( const char* nameUnif, float *V, int numParameter )
{
	ASSERT(V && nameUnif);
	ASSERT(numParameter < 0 && numParameter > 5);

	int location(-1);

	switch(numParameter)
	{
	case 1: {
		location = glGetUniformLocation(m_ProgramID, nameUnif);
		if (location < 0)
			return -1;
		glUniform1fv(location, numParameter, V);
			} break;
	case 2: {
		location = glGetUniformLocation(m_ProgramID, nameUnif);
		if (location < 0)
			return -2;
		glUniform2fv(location, numParameter, V);
			} break;
	case 3: {
		location = glGetUniformLocation(m_ProgramID, nameUnif);
		if (location < 0)
			return -3;
		glUniform3fv(location, numParameter, V);
			} break;
	case 4:	{
		location = glGetUniformLocation(m_ProgramID, nameUnif);
		if (location < 0)
			return -4;
		glUniform4fv(location, numParameter, V);
			} break;
	default:
		return -5;
	}

	return location;
}

int CShaderOGL::Set_Matrix2fv( const char* nameUnif, const float *V, int count )
{
	ASSERT(V && nameUnif);

	int location(-1);

	if ( (location = glGetUniformLocation(m_ProgramID, nameUnif) ) < 0)
		return -1;

	glUniformMatrix2fv(location, count, m_bIsTranspMat, V);

	return location;
}

int CShaderOGL::Set_Matrix3fv( const char* nameUnif, const float *V, int count )
{
	ASSERT(V && nameUnif);

	int location(-1);

	if ( (location = glGetUniformLocation(m_ProgramID, nameUnif) ) < 0)
		return -1;

	glUniformMatrix3fv(location, count, m_bIsTranspMat, V);

	return location;
}

int CShaderOGL::Set_Matrix4fv( const char* nameUnif, const float *V, int count )
{
	ASSERT(V && nameUnif);

	int location(-1);

	if ( (location = glGetUniformLocation(m_ProgramID, nameUnif)) < 0)
		return -1;

	glUniformMatrix4fv(location, count, m_bIsTranspMat, V);

	return location;
}

int CShaderOGL::Set_Integer1( const char* nameUnif, int V )
{
	ASSERT(nameUnif);

	int location(-1);

	if ( (location = glGetUniformLocation(m_ProgramID, nameUnif)) < 0)
		return -1;

	glUniform1i(location, V);

	return location;
}

int CShaderOGL::Set_Integer2v( const char* nameUnif, int* V, int count )
{
	ASSERT(V && nameUnif);

	int location(-1);

	if ( (location = glGetUniformLocation(m_ProgramID, nameUnif)) < 0)
		return -1;

	glUniform2iv(location, count, V);

	return location;
}

int CShaderOGL::Set_Integer2( const char* nameUnif, int V0, int V1 )
{
	ASSERT(nameUnif);

	int location(-1);

	if ( (location = glGetUniformLocation(m_ProgramID, nameUnif)) < 0)
		return -1;

	glUniform2i(location, V0, V1);

	return location;
}

int CShaderOGL::Set_Integer3v( const char* nameUnif, int* V, int count )
{
	ASSERT(V && nameUnif);

	int location(-1);

	if ( (location = glGetUniformLocation(m_ProgramID, nameUnif)) < 0)
		return -1;

	glUniform3iv(location, count, V);

	return location;
}

int CShaderOGL::Set_Integer3( const char* nameUnif, int V0, int V1, int V2 )
{
	ASSERT(nameUnif);

	int location(-1);

	if ( (location = glGetUniformLocation(m_ProgramID, nameUnif)) < 0)
		return -1;

	glUniform3i(location, V0, V1, V2);

	return location;
}

int CShaderOGL::Set_Integer4v( const char* nameUnif, int* V, int count )
{
	ASSERT(V && nameUnif);

	int location(-1);

	if ( (location = glGetUniformLocation(m_ProgramID, nameUnif)) < 0)
		return -1;

	glUniform4iv(location, count, V);

	return location;
}

int CShaderOGL::Set_Integer4( const char* nameUnif, int V0, int V1, int V2, int V3 )
{
	ASSERT(nameUnif);

	int location(-1);

	if ( (location = glGetUniformLocation(m_ProgramID, nameUnif)) < 0)
		return -1;

	glUniform4i(location, V0, V1, V2, V3);

	return location;
}

int CShaderOGL::Set_Float1( const char* nameUnif, float V )
{
	ASSERT(nameUnif);

	int location(-1);

	if ( (location = glGetUniformLocation(m_ProgramID, nameUnif)) < 0)
		return -1;

	glUniform1f( location, V );

	return location;
}

int CShaderOGL::Set_Float2v( const char* nameUnif, float *V, int count )
{
	ASSERT(V && nameUnif);

	int location(-1);

	if ( (location = glGetUniformLocation(m_ProgramID, nameUnif)) < 0)
		return -1;

	glUniform2fv(location, count, V);

	return location;
}

int CShaderOGL::Set_Float2( const char* nameUnif, float V0, float V1 )
{
	ASSERT(nameUnif);

	int location(-1);

	if ( (location = glGetUniformLocation(m_ProgramID, nameUnif)) < 0)
		return -1;

	glUniform2f(location, V0, V1);

	return location;
}

int CShaderOGL::Set_Float3v( const char* nameUnif, float *V, int count )
{
	ASSERT(V && nameUnif);

	int location(-1);

	if ( (location = glGetUniformLocation(m_ProgramID, nameUnif)) < 0)
		return -1;

	glUniform3fv(location, count, V);

	return location;
}

int CShaderOGL::Set_Float3( const char* nameUnif, float V0, float V1, float V2 )
{
	ASSERT(nameUnif);

	int location(-1);

	if ( (location = glGetUniformLocation(m_ProgramID, nameUnif)) < 0)
		return -1;

	glUniform3f(location, V0, V1, V2);

	return location;
}

int CShaderOGL::Set_Float4v( const char* nameUnif, float *V, int count )
{
	ASSERT(V && nameUnif);

	int location(-1);

	if ( (location = glGetUniformLocation(m_ProgramID, nameUnif)) < 0)
		return -1;

	glUniform4fv(location, count, V);

	return location;
}

int CShaderOGL::Set_Float4( const char* nameUnif, float V0, float V1, float V2, float V3 )
{
	ASSERT(nameUnif);

	int location(-1);

	if ( (location = glGetUniformLocation(m_ProgramID, nameUnif)) < 0)
		return -1;

	glUniform4f(location, V0, V1, V2, V3);

	return location;
}

void CShaderOGL::Begin_Use()
{	
	glUseProgram(m_ProgramID);

	ValidateProgram(m_ProgramID);
}

void CShaderOGL::End_Use()
{
	glUseProgram(0);
}
