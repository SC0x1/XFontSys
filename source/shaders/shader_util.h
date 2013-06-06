// Copyright © 2013 Vitaly Lyaschenko (scxv86@gmail.com)
// Purpose: GLSL shaders wrapper
//
#ifndef shader_util_h__
#define shader_util_h__

#pragma once

//===========================================================================
// CShaderOGL
//===========================================================================
class CShaderOGL
{
public:
    CShaderOGL(void);
    ~CShaderOGL();

    // Linking and building the shader program
    bool BuildProgram(const GLchar* vertex, const GLchar* fragment);
    bool BuildProgram(const GLchar* vertex, const GLchar* geometry, const GLchar* fragment);

    void DestroyProgram(void);
    void DestroyCompiledShaders(void);

    bool IsUsableProgram(void) const;
    bool ValidateProgram(void);

    // Vertex attribute
    GLint Get_AttribLocation(const GLchar* attribName)  const;
    void BindAttribLocation(const GLchar* attribName, GLuint index) const;

    GLint Get_UniformLocation(const GLchar* unifName) const;

    // Returns the value of a uniform variable
    void Get_FloatV(GLint location, GLfloat* const* params) const;
    void Get_DoubleV(GLint location, GLdouble* const* params) const;
    void Get_IntegerV(GLint location, GLint* const* params) const;
    void Get_UIntegerV(GLint location, GLuint* const* params) const;
    //
    // Set Matrices
    //
    static bool& MatrixTranspose(void);
    GLint       Set_Matrix2fv(const GLchar* unifName, const GLfloat* value, GLint count = 1) const;
    static void	Set_Matrix2fv(GLint location, const GLfloat* value, GLint count = 1);
    GLint       Set_Matrix3fv(const GLchar* unifName, const GLfloat* value, GLint count = 1) const;
    static void	Set_Matrix3fv(GLint location, const GLfloat* value, GLint count = 1);
    GLint       Set_Matrix4fv(const GLchar* unifName, const GLfloat* value, GLint count = 1) const;
    static void	Set_Matrix4fv(GLint location, const GLfloat* value, GLint count = 1);
    //
    // Set Integers
    //
    GLint       Set_IntegerNv(const GLchar* unifName, const GLint* value, GLint count) const;
    GLint       Set_Integer1(const GLchar* unifName, GLint v0) const;
    static void	Set_Integer1(GLint location, GLint v0);
    GLint       Set_Integer2v(const GLchar* unifName, const GLint* value, GLint count = 1) const;
    static void	Set_Integer2v(GLint location, const GLint* value, GLint count = 1);
    GLint       Set_Integer2(const GLchar* unifName, GLint v0, GLint v1) const;
    static void	Set_Integer2(GLint location, GLint v0, GLint v1);
    GLint       Set_Integer3v(const GLchar* unifName, const GLint* value, GLint count = 1) const;
    static void Set_Integer3v(GLint location, const GLint* value, GLint count = 1);
    GLint       Set_Integer3(const GLchar* unifName, GLint v0, GLint v1, GLint v2) const;
    static void	Set_Integer3(GLint location, GLint v0, GLint v1, GLint v2);
    GLint       Set_Integer4v(const GLchar* unifName, const GLint* value, GLint count = 1) const;
    static void	Set_Integer4v(GLint location, const GLint* value, GLint count = 1);
    int         Set_Integer4(const GLchar* unifName, GLint v0, GLint v1, GLint v2, GLint v3) const;
    static void	Set_Integer4(GLint location, GLint v0, GLint v1, GLint v2, GLint v3);
    //
    // Set Floats
    //
    GLint       Set_FloatNv(const GLchar* unifName, const GLfloat* value, GLint count) const;
    GLint       Set_Float1(const GLchar* unifName, GLfloat v0) const;
    static void	Set_Float1(GLint location, GLfloat v0);
    GLint       Set_Float2v(const GLchar* unifName, const GLfloat* value, GLint count = 1) const;
    static void	Set_Float2v(GLint location, const GLfloat* value, GLint count = 1);
    GLint       Set_Float2(const GLchar* unifName, GLfloat v0, GLfloat v1) const;
    static void	Set_Float2(GLint location, GLfloat v0, GLfloat v1);
    GLint       Set_Float3v(const GLchar* unifName, const GLfloat* value, GLint count = 1) const;
    static void	Set_Float3v(GLint location, const GLfloat* value, GLint count = 1);
    GLint       Set_Float3(const GLchar* unifName, GLfloat v0, GLfloat v1, GLfloat v2) const;
    static void	Set_Float3(GLint location, GLfloat v0, GLfloat v1, GLfloat v2);
    GLint       Set_Float4v(const GLchar* unifName, const GLfloat* value, GLint count = 1) const;
    static void	Set_Float4v(GLint location, const GLfloat* value,  GLint count = 1);
    GLint       Set_Float4(const GLchar* unifName, GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3) const;
    static void	Set_Float4(GLint location, GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3);
    //
    // Usage an shader program (Begin / End)
    //
    void Use_Begin(void) const;
    void Use_End(void) const;
    //
    // Geting the Program/Shaders indices
    //
    GLuint IdxProgram(void) const;
    GLuint IdxVertexShader(void) const;
    GLuint IdxGeometryShader(void) const;
    GLuint IdxFragmentShader(void) const;

private:
    CShaderOGL(const CShaderOGL&);
    CShaderOGL& operator = (const CShaderOGL&);

    GLuint programID_;
    GLuint vertexID_;
    GLuint geometryID_;
    GLuint fragmentID_;
    bool bIsUsable_;
    static bool s_bMatrixTransp;
};

inline bool CShaderOGL::IsUsableProgram(void) const
{
    return bIsUsable_;
}

inline GLint CShaderOGL::Get_AttribLocation(const GLchar* attribName) const
{
    assert(attribName);
    return glGetAttribLocation(programID_, attribName);
}
inline void CShaderOGL::BindAttribLocation(const GLchar* attribName, GLuint index) const
{
    assert(attribName);
    glBindAttribLocation(programID_, index, attribName);
}
inline GLint CShaderOGL::Get_UniformLocation(const GLchar* unifName) const
{
    assert(unifName);
    return glGetUniformLocation(programID_, unifName);
}

// Get Uniform
//
inline void CShaderOGL::Get_FloatV(GLint location, GLfloat* const* params) const
{
    glGetUniformfv(programID_, location, *params);
}
inline void CShaderOGL::Get_DoubleV(GLint location, GLdouble* const* params) const
{
    glGetUniformdv(programID_, location, *params);
}
inline void CShaderOGL::Get_IntegerV(GLint location, GLint* const* params) const
{
    glGetUniformiv(programID_, location, *params);
}
inline void CShaderOGL::Get_UIntegerV(GLint location, GLuint* const* params) const
{
    glGetUniformuiv(programID_, location, *params);
}

// Set Matrices
//
inline bool& CShaderOGL::MatrixTranspose(void)
{
    return s_bMatrixTransp;
}

inline void CShaderOGL::Set_Matrix2fv(GLint location, const GLfloat* value, GLint count)
{
    assert(value);
    glUniformMatrix2fv(location, count, s_bMatrixTransp, value);
}
inline void CShaderOGL::Set_Matrix3fv(GLint location, const GLfloat* value, GLint count)
{
    assert(value);
    glUniformMatrix3fv(location, count, s_bMatrixTransp, value);
}
inline void CShaderOGL::Set_Matrix4fv(GLint location, const GLfloat* value, GLint count)
{
    assert(value);
    glUniformMatrix4fv(location, count, s_bMatrixTransp, value);
}

// Set Integers
//
inline void CShaderOGL::Set_Integer1(GLint location, GLint v0)
{
    glUniform1i(location, v0);
}
inline void CShaderOGL::Set_Integer2v(GLint location, const GLint* value, GLint count)
{
    assert(value);
    glUniform2iv(location, count, value);
}
inline void CShaderOGL::Set_Integer2(GLint location, GLint v0, GLint v1)
{
    glUniform2i(location, v0, v1);
}
inline void Set_Integer3v(GLint location, const GLint* value, GLint count)
{
    assert(value);
    glUniform3iv(location, count, value);
}
inline void CShaderOGL::Set_Integer3(GLint location, GLint v0, GLint v1, GLint v2)
{
    glUniform3i(location, v0, v1, v2);
}
inline void CShaderOGL::Set_Integer4v(GLint location, const GLint* value, GLint count)
{
    assert(value);
    glUniform4iv(location, count, value);
}
inline void CShaderOGL::Set_Integer4(GLint location, GLint v0, GLint v1, GLint v2, GLint v3)
{
    glUniform4i(location, v0, v1, v2, v3);
}

// Set Floats
//
inline void CShaderOGL::Set_Float1(GLint location, GLfloat v0)
{
    glUniform1f(location, v0);
}
inline void CShaderOGL::Set_Float2v(GLint location, const GLfloat* value, GLint count)
{
    assert(value);
    glUniform2fv(location, count, value);
}
inline void CShaderOGL::Set_Float2(GLint location, GLfloat v0, GLfloat v1)
{
    glUniform2f(location, v0, v1);
}
inline void CShaderOGL::Set_Float3v(GLint location, const GLfloat* value, GLint count)
{
    assert(value);
    glUniform3fv(location, count, value);
}
inline void CShaderOGL::Set_Float3(GLint location, GLfloat v0, GLfloat v1, GLfloat v2)
{
    glUniform3f(location, v0, v1, v2);
}
inline void CShaderOGL::Set_Float4v(GLint location, const GLfloat* value,  GLint count)
{
    assert(value);
    glUniform4fv(location, count, value);
}
inline void CShaderOGL::Set_Float4(GLint location, GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3)
{
    glUniform4f(location, v0, v1, v2, v3);
}

// Usage an shader program (Begin / End)
//
inline void CShaderOGL::Use_Begin(void) const
{
    glUseProgram(programID_);
}
inline void CShaderOGL::Use_End(void) const
{
    glUseProgram(0);
}

// Geting the Program/Shaders indices
//
inline GLuint CShaderOGL::IdxProgram(void) const
{
    return programID_;
}
inline GLuint CShaderOGL::IdxVertexShader(void) const
{
    return vertexID_;
}
inline GLuint CShaderOGL::IdxGeometryShader(void) const
{
    return geometryID_;
}
inline GLuint CShaderOGL::IdxFragmentShader(void) const
{
    return fragmentID_;
}

#endif // shader_util_h__