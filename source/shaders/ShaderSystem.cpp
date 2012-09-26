/****************************************************************************/
/*	Copyright (c) 2012 Vitaly Lyaschenko < scxv86@gmail.com >
/*
/*	Purpose: Wrapper for GLSL shaders
/*
/****************************************************************************/

#include "shaders/ShaderSystem.h"
#include "public/common.h"

#define ASSERT(x) assert(x)

/*---------------------------------------------------------------------------*/
/* Purpose: check the status of compiling shaders
/* Input  : id shader
/* Output : returns true on success
/*---------------------------------------------------------------------------*/
bool Check_Shader(unsigned int checkShaderID)
{
	GLint  compiled = GL_FALSE;

	glGetShaderiv(checkShaderID, GL_COMPILE_STATUS, &compiled);

	if (!compiled)
	{
		GLint length;
		static char bufferErr[512];
		glGetShaderInfoLog(checkShaderID, sizeof(bufferErr), &length, bufferErr);
		bufferErr[length+1] = '\0';
		fprintf(stderr, "\nERROR_Shader %s\n", bufferErr);
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
inline bool Check_Program(unsigned int program, GLenum param)
{
	GLint  status = GL_FALSE;
	static GLchar buffer[512];

	glGetProgramiv(program, param, &status);

	if (!status)
	{
		GLint length;
		glGetProgramInfoLog( program, sizeof(buffer), &length, buffer );
		fprintf(stderr,"\nWrong check Program %d error: %s\n", program, buffer);
		return false;
	}

	return true;
}

inline bool ValidateProgram(GLuint program)
{
	glValidateProgram(program);

	return Check_Program(program, GL_VALIDATE_STATUS) == true;
}

inline bool CreateShader(GLenum type, const char* pSource, GLuint&  vertShaderID)
{
	ASSERT(pSource);

	char *codeVertexFile = nullptr;
	int lenghtVertexFile = strlen(pSource);

	vertShaderID = glCreateShader( type );

	glShaderSource( vertShaderID, 1, (const char**)&pSource, &lenghtVertexFile );

	delete[] codeVertexFile;

	glCompileShader( vertShaderID );

	if (!Check_Shader(vertShaderID))
	{
		glDeleteShader( vertShaderID );

		vertShaderID = 0;

		return false;
	}

	return true;
}

/*---------------------------------------------------------------------------*/
/* Purpose: removing shader
/* Input : shader - id shader
/*---------------------------------------------------------------------------*/
inline bool DestroyShader(unsigned int shader)
{
	if( glIsShader(shader) == GL_TRUE )
	{
		glDeleteShader( shader );
		shader = 0;
		return true;
	}
	return false;
}

/*---------------------------------------------------------------------------*/
/* Purpose: Create and link shader program
/* Input  : vertShaderID/fragShaderID - id vertex/fragment shader
/* Output : program - id шейдерной программы
/*---------------------------------------------------------------------------*/
inline bool CreateShaderProgram(GLuint vertShaderID, GLuint fragShaderID, GLuint& program)
{
	program = glCreateProgram();

	if (program == GL_FALSE)
		return false;

	glAttachShader(program, vertShaderID);
	glAttachShader(program, fragShaderID);

	glLinkProgram(program);

	if (!Check_Program(program, GL_LINK_STATUS))
	{
		glDetachShader(program, vertShaderID);
		glDetachShader(program, fragShaderID);
		glDeleteProgram( program );
		program = 0;
		return false;
	}

	return true;
}

inline bool CreateShaderProgram(GLuint vertShaderID, GLuint geomShaderID, GLuint fragShaderID, GLuint& program)
{
	program = glCreateProgram();

	if (program == GL_FALSE)
		return false;

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
		return false;
	}

	return true;
}

inline bool DestroyProgram(GLuint program)
{
	if ( glIsProgram(program) == GL_TRUE)
	{
		glUseProgram(0);
		glDeleteProgram(program);
		program = 0;
		return true;
	}

	return false;
}

CShaderOGL::CShaderOGL() 
:	m_VertexShaderID(0),
	m_FragmentShaderID(0),
	m_GeometryShaderID(0),
	m_ProgramID(0),
	m_VAOID(0),
	m_bInit(false)
{
	pSetAttrib = nullptr;
	pResetAttrib = nullptr;
}

CShaderOGL::~CShaderOGL()
{
	if (ValidateProgram(m_ProgramID))
	{
		
		glDetachShader(m_ProgramID, m_VertexShaderID);
		glDetachShader(m_ProgramID, m_FragmentShaderID);
	}

	DestroyShader(m_VertexShaderID);
	DestroyShader(m_FragmentShaderID);

	DestroyProgram(m_ProgramID);

	glDeleteBuffers(1, &m_VAOID);
}

bool CShaderOGL::BuildShaderProgram(const char* pNameVertex, const char* pNameFragment, VertexFormats format)
{
	ASSERT(pNameVertex && pNameFragment);
	ASSERT((m_FragmentShaderID == 0) && (m_VertexShaderID == 0));
	ASSERT((m_ProgramID == 0) && !m_bInit);


	if (!this->BuildVertexShader(pNameVertex)) {
		return false;
	}

	if (!this->BuildFragmentShader(pNameFragment)) {
		return false;
	}

	if (!CreateShaderProgram(m_VertexShaderID, m_FragmentShaderID, m_ProgramID)) {
		fprintf(stderr,"Failed create the Shader Program %d \n", m_ProgramID);
		return false;
	}

	InitVertexPointer(format);

	return true;
}

bool CShaderOGL::BuildShaderProgram(const char* pNameVertex, const char* pNameGeometry, const char* pNameFragment, VertexFormats format)
{
	ASSERT(pNameVertex && pNameFragment && pNameGeometry);
	ASSERT((m_FragmentShaderID == 0) && (m_GeometryShaderID == 0) && (m_VertexShaderID == 0));
	ASSERT((m_ProgramID == 0) && !m_bInit);

	if (!this->BuildVertexShader(pNameVertex)) {
		return false;
	}

	if (!this->BuildGeometryShader(pNameGeometry)) {
		return false;
	}

	if (!this->BuildFragmentShader(pNameFragment)) {
		return false;
	}

	if (!CreateShaderProgram(m_VertexShaderID, m_GeometryShaderID, m_FragmentShaderID, m_ProgramID)) {
		fprintf(stderr,"Failed create the Shader Program %d \n", m_ProgramID);
		return false;
	}

	InitVertexPointer(format);

	return true;
}

bool CShaderOGL::BuildShaderProgramMem(const void* pLocVertex, const void* pLocFragment, VertexFormats format)
{
	ASSERT(pLocVertex && pLocFragment);
	ASSERT((m_FragmentShaderID == 0) && (m_VertexShaderID == 0));
	ASSERT((m_ProgramID == 0) && !m_bInit);

	if (!CreateShader(GL_VERTEX_SHADER, (const char*)pLocVertex, m_VertexShaderID)) {
		return false;
	}

	if (!CreateShader(GL_FRAGMENT_SHADER, (const char*)pLocFragment, m_FragmentShaderID)) {
		return false;
	}

	if (!CreateShaderProgram(m_VertexShaderID, m_FragmentShaderID, m_ProgramID)) {
		return false;
	}

	InitVertexPointer(format);

	return true;
}

bool CShaderOGL::BuildShaderProgramMem(const void* pLocVertex, const void* pLocGeometry, const void* pLocFragment, VertexFormats format)
{
	ASSERT(pLocVertex && pLocFragment && pLocGeometry);
	ASSERT((m_FragmentShaderID == 0) && (m_GeometryShaderID == 0) && (m_VertexShaderID == 0));
	ASSERT((m_ProgramID == 0) && !m_bInit);

	if (!CreateShader(GL_VERTEX_SHADER, (const char*)pLocVertex, m_VertexShaderID)) {
		return false;
	}

	if (!CreateShader(GL_GEOMETRY_SHADER, (const char*)pLocGeometry, m_GeometryShaderID)) {
		return false;
	}

	if (!CreateShader(GL_FRAGMENT_SHADER, (const char*)pLocFragment, m_FragmentShaderID)) {
		return false;
	}

	if (!CreateShaderProgram(m_VertexShaderID, m_GeometryShaderID, m_FragmentShaderID, m_ProgramID)) {
		return false;
	}

	InitVertexPointer(format);

	return true;
}

bool CShaderOGL::BuildVertexShader(const char* pNameVertexShaderFile)
{
	const char * codeVertexShader = util::ReadWholeFileIntoString(pNameVertexShaderFile);

	if (!codeVertexShader) {
		return false;
	}

	if (!CreateShader(GL_VERTEX_SHADER, codeVertexShader, m_VertexShaderID)) {
		return false;
	}
	return true;
}

bool CShaderOGL::BuildGeometryShader(const char* pNameGeometryShaderFile)
{
	const char * codeVertexShader = util::ReadWholeFileIntoString(pNameGeometryShaderFile);

	if (!codeVertexShader) {
		return false;
	}

	if (!CreateShader(GL_GEOMETRY_SHADER, codeVertexShader, m_GeometryShaderID)) {
		return false;
	}
	return true;
}

bool CShaderOGL::BuildFragmentShader(const char* pNameFragmentShaderFile)
{
	const char * codeFragmentShader = util::ReadWholeFileIntoString(pNameFragmentShaderFile);

	if (!codeFragmentShader) {
		return false;
	}

	if (!CreateShader(GL_FRAGMENT_SHADER, codeFragmentShader, m_FragmentShaderID)) {
		return false;
	}
	return true;
}

bool CShaderOGL::ValidateShaderProgram()
{
	if (!ValidateProgram(m_ProgramID)) {
		fprintf(stderr, "\nShader Program %d Not Validate!\n", m_ProgramID);
		return false;
	}

	return true;
}

bool CShaderOGL::DestroyShaderProgram()
{
	m_bInit = false;

	if (!DestroyProgram(m_ProgramID))
		return false;

	if (m_VertexShaderID > 0)
		DestroyVertexShader();

	if (m_GeometryShaderID > 0)
		DestroyGeometryShader();

	if (m_FragmentShaderID > 0)
		DestroyFragmentShader();

	pResetAttrib();

	glDeleteVertexArrays(1, &m_VAOID);

	pSetAttrib = nullptr;
	pResetAttrib = nullptr;

	return true;
}

bool CShaderOGL::DestroyVertexShader(void)
{
	return DestroyShader(m_VertexShaderID);
}

bool CShaderOGL::DestroyGeometryShader(void)
{
	return DestroyShader(m_GeometryShaderID);
}

bool CShaderOGL::DestroyFragmentShader(void)
{
	return DestroyShader(m_FragmentShaderID);
}

bool CShaderOGL::Set_Parameter(int *V, const char* nameUnif, int numParameter, int *loc)
{
	ASSERT(V && nameUnif);
	ASSERT(numParameter < 0 && numParameter > 5);
	int location = -1;

	switch(numParameter) {
	case 1: {
		location = glGetUniformLocation(m_ProgramID, nameUnif);
		if (location < 0)
			return false;
			glUniform1iv(location, numParameter, V);
		} break;
	case 2: {
		location = glGetUniformLocation(m_ProgramID, nameUnif);
		if (location < 0)
			return false;
			glUniform2iv(location, numParameter, V);
		} break;
	case 3:	{
		location = glGetUniformLocation(m_ProgramID, nameUnif);
		if (location < 0)
			return false;
			glUniform3iv(location, numParameter, V);
		} break;
	case 4:	{
		location = glGetUniformLocation(m_ProgramID, nameUnif);
		if (location < 0)
			return false;
			glUniform4iv(location, numParameter, V);
		} break;
	default:
		return false;
	}

	if (loc) *loc = location;

	return true;
}

bool CShaderOGL::Set_Parameter(float *V, const char* nameUnif,int numParameter, int *loc)
{
	ASSERT(V && nameUnif);
	ASSERT(numParameter < 0 && numParameter > 5);
	int location = -1;

	switch(numParameter)
	{
	case 1: {
		location = glGetUniformLocation(m_ProgramID, nameUnif);
		if (location < 0)
			return false;
		glUniform1fv(location, numParameter, V);
			} break;
	case 2: {
		location = glGetUniformLocation(m_ProgramID, nameUnif);
		if (location < 0)
			return false;
		glUniform2fv(location, numParameter, V);
			} break;
	case 3: {
		location = glGetUniformLocation(m_ProgramID, nameUnif);
		if (location < 0)
			return false;
		glUniform3fv(location, numParameter, V);
			} break;
	case 4:	{
		location = glGetUniformLocation(m_ProgramID, nameUnif);
		if (location < 0)
			return false;
		glUniform4fv(location, numParameter, V);
			} break;
	default:
		return false;
	}

	if (loc) *loc = location;

	return true;
}

bool CShaderOGL::Set_Matrix2fv(const GLfloat *V, const char* nameUnif, int *loc, GLsizei count, bool transpose)
{
	ASSERT(V && nameUnif);
	int location = -1;

	location = glGetUniformLocation(m_ProgramID, nameUnif);
	if (location < 0)
		return false;
	glUniformMatrix2fv(location, count, transpose, V);

	if (loc) *loc = location;

	return true;
}

bool CShaderOGL::Set_Matrix3fv(const GLfloat *V, const char* nameUnif, int *loc, GLsizei count, bool transpose)
{
	ASSERT(V && nameUnif);
	int location = -1;

	location = glGetUniformLocation(m_ProgramID, nameUnif);
	if (location < 0)
		return false;
	glUniformMatrix3fv(location, count, transpose, V);

	if (loc) *loc = location;

	return true;
}

bool CShaderOGL::Set_Matrix4fv(const GLfloat *V, const char* nameUnif, int *loc, GLsizei count, bool transpose)
{
	ASSERT(V && nameUnif);
	int location = -1;

	location = glGetUniformLocation(m_ProgramID, nameUnif);
	if (location < 0)
		return false;
	glUniformMatrix4fv(location, count, transpose, V);

	if (loc) *loc = location;

	return true;
}

bool CShaderOGL::Set_Integer1(int V, const char* nameUnif, int *loc)
{
	ASSERT(nameUnif);
	int location = -1;

	location = glGetUniformLocation(m_ProgramID, nameUnif);
	if (location < 0)
		return false;
	glUniform1i(location, V);

	if (loc) *loc = location;

	return true;
}

bool CShaderOGL::Set_Integer2v(int* V, const char* nameUnif, int *loc, int count)
{
	ASSERT(V && nameUnif);
	int location = -1;

	location = glGetUniformLocation(m_ProgramID, nameUnif);
	if (location < 0)
		return false;
	glUniform2iv(location, count, V);

	if (loc) *loc = location;

	return true;
}

bool CShaderOGL::Set_Integer2(int V0, int V1, const char* nameUnif, int *loc)
{
	ASSERT(nameUnif);
	int location = -1;

	location = glGetUniformLocation(m_ProgramID, nameUnif);
	if (location < 0)
		return false;
	glUniform2i(location, V0, V1);

	if (loc) *loc = location;

	return true;
}

bool CShaderOGL::Set_Integer3v(int* V, const char* nameUnif, int *loc, int count)
{
	ASSERT(V && nameUnif);
	int location = -1;

	location = glGetUniformLocation(m_ProgramID, nameUnif);
	if (location < 0)
		return false;
	glUniform3iv(location, count, V);

	if (loc) *loc = location;

	return true;
}

bool CShaderOGL::Set_Integer3(int V0, int V1, int V2, const char* nameUnif, int *loc)
{
	ASSERT(nameUnif);
	int location = -1;

	location = glGetUniformLocation(m_ProgramID, nameUnif);
	if (location < 0)
		return false;
	glUniform3i(location, V0, V1, V2);

	if (loc) *loc = location;

	return true;
}

bool CShaderOGL::Set_Integer4v(int* V, const char* nameUnif, int *loc, int count)
{
	ASSERT(V && nameUnif);
	int location = -1;

	location = glGetUniformLocation(m_ProgramID, nameUnif);
	if (location < 0)
		return false;
	glUniform4iv(location, count, V);

	if (loc) *loc = location;

	return true;
}

bool CShaderOGL::Set_Integer4(int V0, int V1, int V2, int V3, const char* nameUnif, int *loc)
{
	ASSERT(nameUnif);
	int location = -1;

	location = glGetUniformLocation(m_ProgramID, nameUnif);
	if (location < 0)
		return false;
	glUniform4i(location, V0, V1, V2, V3);

	if (loc) *loc = location;

	return true;
}

bool CShaderOGL::Set_Float1(float V, const char* nameUnif, int *loc)
{
	ASSERT(nameUnif);
	int location = -1;

	location = glGetUniformLocation(m_ProgramID, nameUnif);
	if (location < 0)
		return false;
	glUniform1f( location, V );

	if (loc) *loc = location;

	return true;
}

bool CShaderOGL::Set_Float2v(float *V, const char* nameUnif, int *loc, int count)
{
	ASSERT(V && nameUnif);
	int location = -1;

	location = glGetUniformLocation(m_ProgramID, nameUnif);
	if (location < 0)
		return false;
	glUniform2fv(location, count, V);

	if (loc) *loc = location;

	return true;
}

bool CShaderOGL::Set_Float2(float V0, float V1, const char* nameUnif, int *loc)
{
	ASSERT(nameUnif);
	int location = -1;

	location = glGetUniformLocation(m_ProgramID, nameUnif);
	if (location < 0)
		return false;
	glUniform2f(location, V0, V1);

	if (loc) *loc = location;

	return true;
}

bool CShaderOGL::Set_Float3v(float *V, const char* nameUnif, int *loc, int count)
{
	ASSERT(V && nameUnif);
	int location = -1;

	location = glGetUniformLocation(m_ProgramID, nameUnif);
	if (location < 0)
		return false;
	glUniform3fv(location, count, V);

	if (loc) *loc = location;

	return true;
}

bool CShaderOGL::Set_Float3(float V0, float V1, float V2, const char* nameUnif, int *loc)
{
	ASSERT(nameUnif);
	int location = -1;

	location = glGetUniformLocation(m_ProgramID, nameUnif);
	if (location < 0)
		return false;
	glUniform3f(location, V0, V1, V2);

	if (loc) *loc = location;

	return true;
}

bool CShaderOGL::Set_Float4v(float *V, const char* nameUnif, int *loc, int count)
{
	ASSERT(V && nameUnif);
	int location = -1;

	location = glGetUniformLocation(m_ProgramID, nameUnif);
	if (location < 0)
		return false;
	glUniform4fv(location, count, V);

	if (loc) *loc = location;

	return true;
}

bool CShaderOGL::Set_Float4(float V0, float V1, float V2, float V3, const char* nameUnif, int *loc)
{
	ASSERT(nameUnif);
	int location = -1;

	location = glGetUniformLocation(m_ProgramID, nameUnif);
	if (location < 0)
		return false;
	glUniform4f(location, V0, V1, V2, V3);

	if (loc) *loc = location;

	return true;
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

void CShaderOGL::InitVertexPointer( VertexFormats format )
{
	glGenVertexArrays(1, &m_VAOID);
	glBindVertexArray(m_VAOID);

	if(format == FVF_Simple2DText)
	{
		pSetAttrib   = Set_Attrib2DText;
		pResetAttrib = Reset_Attrib2DText;
	}
	else if(format == FVF_Simple2DColoredText)
	{
		pSetAttrib   = Set_Attrib2DTextColored;
		pResetAttrib = Reset_Attrib2DTextColored;
	}

	/* ... */
}
