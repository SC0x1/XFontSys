#ifndef utlvector_h__
#define utlvector_h__

#pragma once

#define BASE_SIZE 16
#define INLINE __forceinline

#define SAFE_DELETE(x) if (x) { delete x; x = nullptr; }
#define SAFE_DELETE_ARRAY(x) if (x) { delete[] x; x = nullptr; }

template<typename T>
class CUtlVector
{
public:

    explicit CUtlVector(int m_growth = BASE_SIZE);
    CUtlVector(const CUtlVector<T>& other);
    ~CUtlVector();

    void Clear();
    int  Count() const;
    void SetGrowth(int growth);
    int  GetGrowth();

    int  AddToTail(void);
    int  Append(const T& obj);
    int  FindIdx(const T& obj) const;
    bool RemoveIdx(int index);
    bool Remove(const T& obj);

    T&   Last() const;
    T&   First() const;

    CUtlVector<T>& operator=(const CUtlVector<T>& other);
    const T& operator[](int index) const;
    T&   operator[](int index);

    void Resize(int newsize);
    void Resize(int newsize, int growth);

    void DeleteContents(bool clear);

private:
    int numElements_;
    int allocSize_;
    int growthSize_;
    T* pData_;
};
template<typename T>
int CUtlVector<T>::GetGrowth(void)
{
    return growthSize_;
}
template<typename T>
void CUtlVector<T>::SetGrowth(int growth)
{
    assert(growth > 0);
    int newsize(0);
    growthSize_ = growth;
    if (pData_)
    {
        // изменим размер до заданной велечины
        newsize = numElements_ + growth - 1;
        newsize -= newsize % growth;

        if (newsize != allocSize_)
        {
            Resize(newsize);
        }
    }
}
template<typename T>
INLINE CUtlVector<T>::CUtlVector(int growth)
{
    assert(growth > 0);
    pData_ = nullptr;
    growthSize_ = growth;
    numElements_ = 0;
    Clear();
}

template<typename T>
INLINE CUtlVector<T>::CUtlVector(const CUtlVector<T>& other)
{
    pData_ = nullptr;
    numElements_ = 0;
    *this = other;
}

template<typename T>
INLINE CUtlVector<T>::~CUtlVector()
{
    Clear();
}

template<typename T>
INLINE int CUtlVector<T>::FindIdx(const T& obj) const
{
    for (int i = 0; i < numElements_; ++i)
    {
        if (pData_[i] == obj)
            return i;
    }
    return -1;
}

template<typename T>
INLINE bool CUtlVector<T>::Remove(const T& obj)
{
    const int id(FindIdx(obj));
    if (id >= 0)
        return RemoveIdx(id);

    return false;
}

template<typename T>
INLINE bool CUtlVector<T>::RemoveIdx(int index)
{
    assert(pData_ != nullptr);
    assert(index >= 0);
    assert(index < numElements_);
    if ((index < 0 ) || ( index >= numElements_))
        return false;

    --numElements_;
    for (int i = index; i < numElements_; ++i)
        pData_[i] = pData_[i + 1];

    return true;
}

template<typename T>
INLINE int CUtlVector<T>::Count() const
{
    return numElements_;
}

template<typename T>
INLINE T& CUtlVector<T>::operator[](int index)
{
    assert(index >= 0);
    assert(index <= numElements_);
    return pData_[index];
}

template<typename T>
INLINE const T& CUtlVector<T>::operator[](int index) const
{
    assert(index >= 0);
    assert(index < numElements_);
    return pData_[index];
}

template<typename T>
INLINE CUtlVector<T>& CUtlVector<T>::operator=(const CUtlVector<T>& other)
{
    Clear();
    numElements_ = other.m_NumElements;
    allocSize_ = other.m_size;
    growthSize_ = other.m_growth;
    if (allocSize_)
    {
        pData_ = new T[allocSize_];
        for (int i = 0; i < numElements_; ++i)
            pData_[i] = other.m_pData[i];
    }
    return *this;
}

template<typename T>
INLINE void CUtlVector<T>::Resize(int newsize)
{
    T* temp;
    assert(newsize >= 0);

    if (newsize <= 0)
    {
        Clear();
        return;
    }

    if (newsize == allocSize_)
        return;

    temp = pData_;
    allocSize_ = newsize;
    if (allocSize_ < numElements_)
        numElements_ = allocSize_;

    pData_ = new T[allocSize_];
    for (int i(0); i < numElements_; ++i)
        pData_[i] = temp[i];

    if (temp)
        delete[] temp;
}

template<typename T>
void CUtlVector<T>::Resize(int newsize, int growth)
{
    assert(newsize >= 0);
    assert(growth > 0);

    growthSize_ = growth;

    if (newsize <= 0)
    {
        Clear();
        return;
    }

    T* temp = pData_;
    allocSize_ = newsize;

    if (allocSize_ < numElements_)
    {
        numElements_ = allocSize_;
    }

    pData_ = new T[allocSize_];
    for (int i = 0; i < numElements_; ++i)
    {
        pData_[i] = temp[i];
    }

    if (temp)
        delete[] temp;
}

template<typename T>
INLINE void CUtlVector<T>::DeleteContents(bool clear)
{
    for (int i = 0; i < numElements_; ++i)
        SAFE_DELETE(pData_[i]);

    if (clear)
    {
        this->Clear();
    }
    else
    {
        memset(pData_, 0, allocSize_ * sizeof(T));
    }
}

template<typename T>
int CUtlVector<T>::AddToTail()
{
    if (!pData_)
    {
        Resize(growthSize_);
    }
    if (numElements_ == allocSize_)
    {
        Resize(allocSize_ + growthSize_);
    }
    ++numElements_;
}

template<typename T>
INLINE int CUtlVector<T>::Append(const T& obj)
{
    if (!pData_)
        Resize(growthSize_);

    if (numElements_ == allocSize_)
    {
        if (growthSize_ == 0)
            growthSize_ = 16;

        int newsize = allocSize_ + growthSize_;
        Resize(newsize - newsize % growthSize_);
    }
    pData_[numElements_] = obj;
    ++numElements_;
    return numElements_ - 1;
}

template<typename T>
INLINE void CUtlVector<T>::Clear()
{
    if (pData_)
        delete[] pData_;

    pData_ = nullptr;
    numElements_ = allocSize_ = 0;
}

template<typename T>
INLINE T& CUtlVector<T>::Last() const
{
    assert(numElements_ > 0);
    return pData_[numElements_ - 1];
}

template<typename T>
INLINE T& CUtlVector<T>::First() const
{
    assert(numElements_ > 0);
    assert(pData_[0]);
    return pData_[0];
}

#endif // utlvector_h__