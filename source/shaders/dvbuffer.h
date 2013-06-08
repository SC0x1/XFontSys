// Copyright (c) 2013 Vitaly Lyaschenko (scxv86@gmail.com)
// Purpose: VBO wrapper
//
#ifndef dvbuffer_h__
#define dvbuffer_h__

#pragma once

class CVertexBuffer
{
public:
    CVertexBuffer(void);
    CVertexBuffer(int vertexSize, int vertexCount, void* data = nullptr, bool dynamic = false);
    ~CVertexBuffer();

    bool CreateVB(int vertexSize, int vertexCount, void* data = nullptr, bool dynamic = false);
    void ReleaseVB(void);

    void* Lock(int vertexCount, int& baseVertexIndex) const;
    void Unlock(int vertexCount);

    void Replace(int vertexOffset, int vertexCount, const void* data);
    int PushVertexData(int vertexCount, const void* data);

    int VertexSize(void) const;
    int BorderPositionVB(void) const;
    void BorderToBeginVB(void);
    int Capacity(void) const;

    bool HasEnoughSpace(int no_vertices) const;

    void Bind(void) const;
    void UnBind(void) const;

    // Buffer Manipulations
    unsigned int GetID(void) const;

    int GetBuffersCount(void) const;

private:
    static int& Counter()
    {
        static int s_vertexBuffersCount = 0;
        return s_vertexBuffersCount;
    }
    unsigned int vboID_;
    unsigned int usage_;
    int vertexCapacity_; // общее количество вертексов которое влезет в буфер
    int vertexSize_;
    int border_;

    mutable bool bIsBind_;
    mutable bool bIsLocked_;
    bool bIsCreated_;
};

inline CVertexBuffer::CVertexBuffer(void)
    : bIsBind_(false),
      bIsLocked_(false),
      bIsCreated_(false),
      vboID_(0),
      border_(0),
      vertexSize_(-1),
      vertexCapacity_(-1) {}

inline CVertexBuffer::CVertexBuffer(int vertexSize, int vertexCount, void* data, bool dynamic)
    : bIsBind_(false),
      bIsLocked_(false),
      bIsCreated_(false),
      vboID_(0),
      border_(0),
      vertexSize_(vertexSize),
      vertexCapacity_(vertexCount)
{
    CreateVB(vertexSize, vertexCount, data, dynamic);
}

inline CVertexBuffer::~CVertexBuffer()
{
    ReleaseVB();
}

inline bool CVertexBuffer::CreateVB(int vertexSize, int vertexCount, void* data, bool dynamic)
{
    assert(!bIsCreated_);
    vertexSize_ = vertexSize;
    vertexCapacity_ = vertexCount;
    glGenBuffers(1, &vboID_);
    this->Bind();
    if (dynamic)
    {
        usage_ = GL_DYNAMIC_DRAW;
    }
    else
    {
        usage_ = GL_STATIC_DRAW;
    }
    glBufferData(GL_ARRAY_BUFFER, vertexCapacity_ * vertexSize_, data, usage_);
    this->UnBind();

    if(data)
    {
        border_ = vertexCapacity_;
    }
    ++Counter();
    return (bIsCreated_ = true);
}

inline void CVertexBuffer::ReleaseVB(void)
{
    if (bIsCreated_)
    {
        Unlock(0);
        if (vboID_ > 0)
        {
            glDeleteBuffers(1, &vboID_);
            vboID_ = 0;
        }

        --Counter();
        bIsCreated_ = false;
    }
}

inline void* CVertexBuffer::Lock(int vertexCount, int& baseVertexIndex) const
{	
    assert(!bIsLocked_);
    baseVertexIndex = 0;
    if (vertexCount > vertexCapacity_)
    {
        assert(0);
        return nullptr;
    }
    void* ptr = nullptr;
    if (vboID_ > 0)
    {
        this->Bind();
        ptr = (void*)glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
        baseVertexIndex = border_;
        bIsLocked_ = true;
    }
    assert(ptr);
    return ptr;
}

inline void CVertexBuffer::Unlock(int vertexCount)
{
    if (bIsLocked_ && vboID_)
    {
        border_ += vertexCount;
        glUnmapBuffer(GL_ARRAY_BUFFER);
        this->UnBind();
        bIsLocked_ = false;
    }
}

inline void CVertexBuffer::Replace(int vertexOffset, int vertexCount, const void* data)
{
    assert(HasEnoughSpace(vertexOffset + vertexCount));
    if ((vertexOffset + vertexCount) > border_) // TODO: review this!
    {
        border_ += (vertexOffset + vertexCount) - border_;
    }

    const int size = vertexCount * vertexSize_;
    const int offset = vertexOffset * vertexSize_;

    this->Bind();
    glBufferSubData(GL_ARRAY_BUFFER, offset, size, (const GLvoid *)data);
    this->UnBind();
}

inline int CVertexBuffer::PushVertexData(int vertexCount, const void* data)
{
    border_ = vertexCount;
    const int size = vertexCount * vertexSize_;
    this->Bind();
    glBufferData(GL_ARRAY_BUFFER, size, data, usage_);
    this->UnBind();
    return size;
}

inline int CVertexBuffer::VertexSize() const
{ 
    return vertexSize_;
}

inline int CVertexBuffer::BorderPositionVB(void) const
{
    return border_;
}

inline void CVertexBuffer::BorderToBeginVB(void)
{
    border_ = 0;
}

inline int CVertexBuffer::Capacity() const
{
    return vertexCapacity_;
}

inline bool CVertexBuffer::HasEnoughSpace(int noVertices) const
{
    return (noVertices + border_) <= vertexCapacity_;
}

inline unsigned int CVertexBuffer::GetID(void) const
{
    return vboID_;
}

inline void CVertexBuffer::Bind(void) const
{
    assert(!bIsBind_);
    glBindBuffer(GL_ARRAY_BUFFER, vboID_);
    bIsBind_ = true;
}

inline void CVertexBuffer::UnBind(void) const
{
    assert(bIsBind_);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    bIsBind_ = false;
}

inline int CVertexBuffer::GetBuffersCount(void) const
{
    return Counter();
}
#endif // dvbuffer_h__