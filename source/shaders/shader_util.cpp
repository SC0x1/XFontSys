// Copyright © 2013 Vitaly Lyaschenko (scxv86@gmail.com)
// Purpose:
//
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "shaders/ogl.h"
#include "shaders/shader_util.h"
#define ASSERT(x) assert(x)

enum { INVALID_LOCATION = -1 };
//---------------------------------------------------------------------------
// Purpose: check the status of compiling shaders
// Input  : id shader
// Output : returns true on success
//---------------------------------------------------------------------------
bool Check_Shader(GLuint shaderID, GLenum param)
{
    GLint compiled = GL_FALSE;
    glGetShaderiv(shaderID, param, &compiled);
    if (!compiled)
    {
        static GLchar bufferErr[512];
        GLint length;
        glGetShaderInfoLog(shaderID, sizeof(bufferErr), &length, bufferErr);
        fprintf(stderr, "\nShader Error: %s\n", bufferErr);
        return false;
    }
    return true;
}
//---------------------------------------------------------------------------
// Purpose: check the status of shader program
// Input  : program - id the shader program
//          param - specifies the object parameter
// Output : returns true on success
//---------------------------------------------------------------------------
inline bool Check_Program(GLuint programID, GLenum param)
{
    GLint status(GL_FALSE);
    glGetProgramiv(programID, param, &status);
    if (!status)
    {
        static GLchar bufferErr[512];
        GLint length;
        glGetProgramInfoLog(programID, sizeof(bufferErr), &length, bufferErr);
        fprintf(stderr,"\nShader Program %d Error: %s\n", programID, bufferErr);
        return false;
    }
    return true;
}
//---------------------------------------------------------------------------
// Purpose: Validation the shader program
// Input  : GLuint program  - shader program ID
// Output : return true if success
//---------------------------------------------------------------------------
inline bool Validate_Program(GLuint program)
{
    glValidateProgram(program);
    return Check_Program(program, GL_VALIDATE_STATUS) == true;
}

//---------------------------------------------------------------------------
// Purpose: Creation and compile shader from source.
// Input  : GLenum type         - enum shader type,
//          const char* pSource - pointer to shader source
// Output : If successful creation and compilation  - return shader ID,
//          otherwise will be returned 0;
//---------------------------------------------------------------------------
inline GLuint Create_Shader(GLenum type, const GLchar* source)
{
    ASSERT(source);
    GLuint shaderID = glCreateShader(type);
    if (shaderID == GL_FALSE)
        return 0;

    const GLint lenghtSource = strlen(source);
    glShaderSource(shaderID, 1, (const GLchar**)&source, &lenghtSource);
    glCompileShader(shaderID);

    if (!Check_Shader(shaderID, GL_COMPILE_STATUS))
    {
        glDeleteShader(shaderID);
        shaderID = 0;
    }
    return shaderID;
}
//---------------------------------------------------------------------------
// Purpose: removing shader
// Input : shader - id shader
//---------------------------------------------------------------------------
inline bool Destroy_Shader(GLuint shaderID)
{
    if (glIsShader(shaderID) == GL_TRUE)
    {
        glDeleteShader(shaderID);
        return true;
    }
    return false;
}
//---------------------------------------------------------------------------
// Purpose: Create and link shader program
// Input  : vertShaderID/fragShaderID - id vertex/fragment shader
// Output : program - id шейдерной программы
//---------------------------------------------------------------------------
inline GLuint Create_Program(GLuint vertShaderID, GLuint fragShaderID)
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
inline GLuint Create_Program(GLuint vertShaderID, GLuint geomShaderID, GLuint fragShaderID)
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

        glDeleteProgram(program);
        program = 0;
    }
    return program;
}
inline bool Destroy_Program(GLuint program)
{
    if (glIsProgram(program) == GL_TRUE)
    {
        glUseProgram(0);
        glDeleteProgram(program);
        return true;
    }
    return false;
}

CShaderOGL::CShaderOGL(void)
    : vertexID_(0),
      fragmentID_(0),
      geometryID_(0),
      programID_(0),
      bIsUsable_(false) {}

bool CShaderOGL::s_bMatrixTransp = true;

CShaderOGL::~CShaderOGL()
{
    Use_End();
    DestroyProgram();
    DestroyCompiledShaders();
}

bool CShaderOGL::BuildProgram(const GLchar* vertex, const GLchar* fragment)
{
    ASSERT(vertex && fragment);
    ASSERT((fragmentID_ == 0) && (vertexID_ == 0));
    ASSERT((programID_ == 0) && !bIsUsable_);

    vertexID_ = Create_Shader(GL_VERTEX_SHADER, vertex);
    if (vertexID_ == 0)
        return false;

    fragmentID_ = Create_Shader(GL_FRAGMENT_SHADER, fragment);
    if (fragmentID_ == 0)
    {
        this->DestroyCompiledShaders();
        return false;
    }
    programID_ = Create_Program(vertexID_, fragmentID_);
    if (programID_ == 0)
    {
        this->DestroyCompiledShaders();
        return false;
    }
    return bIsUsable_ = true;
}

bool CShaderOGL::BuildProgram(const GLchar* vertex, const GLchar* geometry, const GLchar* fragment)
{
    ASSERT(vertex && fragment && geometry);
    ASSERT((fragmentID_ == 0) && (geometryID_ == 0) && (vertexID_ == 0));
    ASSERT((programID_ == 0) && !bIsUsable_);

    vertexID_ = Create_Shader(GL_VERTEX_SHADER, vertex);
    if (vertexID_ == 0)
        return false;

    geometryID_ = Create_Shader(GL_GEOMETRY_SHADER, geometry);
    if (geometryID_ == 0)
    {
        this->DestroyCompiledShaders();
        return false;
    }
    fragmentID_ = Create_Shader(GL_FRAGMENT_SHADER, fragment);
    if (fragmentID_ == 0)
    {
        this->DestroyCompiledShaders();
        return false;
    }
    programID_ = Create_Program(vertexID_, geometryID_, fragmentID_);
    if (programID_ == 0)
    {
        this->DestroyCompiledShaders();
        return false;
    }
    return bIsUsable_ = true;
}

bool CShaderOGL::ValidateProgram(void)
{
    return Validate_Program(programID_);
}

void CShaderOGL::DestroyProgram(void)
{
    if (programID_ > 0 )
    {
        if (Destroy_Program(programID_))
        {
            programID_ = 0;
            bIsUsable_ = false;
        }
    }
}

void CShaderOGL::DestroyCompiledShaders(void)
{
    if (vertexID_ > 0 )
        if (Destroy_Shader(vertexID_))
            vertexID_ = 0;

    if (geometryID_ > 0 )
        if (Destroy_Shader(geometryID_))
            geometryID_ = 0;

    if (fragmentID_ > 0 )
        if (Destroy_Shader(fragmentID_))
            fragmentID_ = 0;
}

GLint CShaderOGL::Set_IntegerNv(const GLchar* unifName, const GLint* value, GLint count) const
{
    ASSERT(unifName && value);
    ASSERT((count < 0) && (count > 5));
    const GLint location = glGetUniformLocation(programID_, unifName);
    if (location != INVALID_LOCATION)
    {
        switch(count)
        {
        case 1 :
            glUniform1iv(location, count, value);
            break;
        case 2 :
            glUniform2iv(location, count, value);
            break;
        case 3 :
            glUniform3iv(location, count, value);
            break;
        case 4 :
            glUniform4iv(location, count, value);
            break;
        default:
            ASSERT(0);
        }
    }
    return location;
}

GLint CShaderOGL::Set_FloatNv(const GLchar* unifName, const GLfloat* value, GLint count) const
{
    ASSERT(value && unifName);
    ASSERT(count < 0 && count > 5);
    const GLint location = glGetUniformLocation(programID_, unifName);
    if (location != INVALID_LOCATION)
    {
        switch(count)
        {
        case 1 :
            glUniform1fv(location, count, value);
            break;
        case 2 :
            glUniform2fv(location, count, value);
            break;
        case 3 :
            glUniform3fv(location, count, value);
            break;
        case 4 :
            glUniform4fv(location, count, value);
            break;
        default:
            ASSERT(0);
        }
    }
    return location;
}

GLint CShaderOGL::Set_Matrix2fv(const GLchar* unifName, const GLfloat* value, GLint count) const
{
    ASSERT(unifName && value);
    const GLint location = glGetUniformLocation(programID_, unifName);
    if (location != INVALID_LOCATION)
        glUniformMatrix2fv(location, count, s_bMatrixTransp, value);

    return location;
}

GLint CShaderOGL::Set_Matrix3fv(const GLchar* unifName, const GLfloat* value, GLint count) const
{
    ASSERT(unifName && value);
    const GLint location = glGetUniformLocation(programID_, unifName);
    if (location != INVALID_LOCATION)
        glUniformMatrix3fv(location, count, s_bMatrixTransp, value);

    return location;
}

GLint CShaderOGL::Set_Matrix4fv(const GLchar* unifName, const GLfloat* value, GLint count) const
{
    ASSERT(unifName && value);
    const GLint location = glGetUniformLocation(programID_, unifName);
    if (location != INVALID_LOCATION)
        glUniformMatrix4fv(location, count, s_bMatrixTransp, value);

    return location;
}

GLint CShaderOGL::Set_Integer1(const GLchar* unifName, GLint v0) const
{
    ASSERT(unifName);
    const GLint location = glGetUniformLocation(programID_, unifName);

    if (location != INVALID_LOCATION)
        glUniform1i(location, v0);

    return location;
}

GLint CShaderOGL::Set_Integer2v(const GLchar* unifName, const GLint* value, GLint count) const
{
    ASSERT(unifName && value);
    const GLint location = glGetUniformLocation(programID_, unifName);

    if (location != INVALID_LOCATION)
         glUniform2iv(location, count, value);

    return location;
}

GLint CShaderOGL::Set_Integer2(const GLchar* unifName, GLint v0, GLint v1) const
{
    ASSERT(unifName);
    const GLint location = glGetUniformLocation(programID_, unifName);
    if (location != INVALID_LOCATION)
        glUniform2i(location, v0, v1);

    return location;
}

GLint CShaderOGL::Set_Integer3v(const GLchar* unifName, const GLint* value, GLint count) const
{
    ASSERT(unifName && value);
    const GLint location = glGetUniformLocation(programID_, unifName);
    if (location != INVALID_LOCATION)
        glUniform3iv(location, count, value);

    return location;
}

GLint CShaderOGL::Set_Integer3(const GLchar* unifName, GLint v0, GLint v1, GLint v2) const
{
    ASSERT(unifName);
    const GLint location = glGetUniformLocation(programID_, unifName);
    if (location != INVALID_LOCATION)
        glUniform3i(location, v0, v1, v2);

    return location;
}

GLint CShaderOGL::Set_Integer4v(const GLchar* unifName, const GLint* value, GLint count) const
{
    ASSERT(unifName && value);
    const GLint location = glGetUniformLocation(programID_, unifName);
    if (location != INVALID_LOCATION)
        glUniform4iv(location, count, value);

    return location;
}

GLint CShaderOGL::Set_Integer4(const GLchar* unifName, GLint v0, GLint v1, GLint v2, GLint v3) const
{
    ASSERT(unifName);
    const GLint location = glGetUniformLocation(programID_, unifName);
    if (location != INVALID_LOCATION)
        glUniform4i(location, v0, v1, v2, v3);

    return location;
}

GLint CShaderOGL::Set_Float1(const GLchar* unifName, GLfloat v0) const
{
    ASSERT(unifName);
    const GLint location = glGetUniformLocation(programID_, unifName);
    if (location != INVALID_LOCATION)
        glUniform1f(location, v0);

    return location;
}

GLint CShaderOGL::Set_Float2v(const GLchar* unifName, const GLfloat* value, GLint count) const
{
    ASSERT(unifName && value);
    const GLint location = glGetUniformLocation(programID_, unifName);
    if (location != INVALID_LOCATION)
        glUniform2fv(location, count, value);

    return location;
}

GLint CShaderOGL::Set_Float2(const GLchar* unifName, GLfloat v0, GLfloat v1) const
{
    ASSERT(unifName);
    const GLint location = glGetUniformLocation(programID_, unifName);
    if (location != INVALID_LOCATION)
        glUniform2f(location, v0, v1);

    return location;
}

GLint CShaderOGL::Set_Float3v(const GLchar* unifName, const GLfloat* value, GLint count) const
{
    ASSERT(unifName && value);
    const GLint location = glGetUniformLocation(programID_, unifName);
    if (location != INVALID_LOCATION)
        glUniform3fv(location, count, value);

    return location;
}

GLint CShaderOGL::Set_Float3(const GLchar* unifName, GLfloat v0, GLfloat v1, GLfloat v2) const
{
    ASSERT(unifName);
    const GLint location = glGetUniformLocation(programID_, unifName);
    if (location != INVALID_LOCATION)
        glUniform3f(location, v0, v1, v2);

    return location;
}

GLint CShaderOGL::Set_Float4v(const GLchar* unifName, const GLfloat* value, GLint count) const
{
    ASSERT(unifName && value);
    const GLint location = glGetUniformLocation(programID_, unifName);
    if (location != INVALID_LOCATION)
        glUniform4fv(location, count, value);

    return location;
}

GLint CShaderOGL::Set_Float4(const GLchar* unifName, GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3) const
{
    ASSERT(unifName);
    const GLint location = glGetUniformLocation(programID_, unifName);
    if (location != INVALID_LOCATION)
        glUniform4f(location, v0, v1, v2, v3);

    return location;
}