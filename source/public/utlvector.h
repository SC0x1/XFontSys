#pragma once

#include <assert.h>
#define BASE_SIZE 16
#define INLINE __forceinline

#define SAFE_DELETE(x) if (x) delete x; x = nullptr;
#define SAFE_DELETE_ARRAY(x) if (x) delete[] x; x = nullptr;

template<typename T>
class CUtlVector {
public:

	CUtlVector( int m_growth = BASE_SIZE );
	CUtlVector( const CUtlVector<T> &other );
	~CUtlVector( void );

	void		Clear( void );                      // удаляет все данные из вектора
	int			Num( void ) const;                  // возвращает количество элементов в векторе
	void		SetGrowth( int growth );
	int			GetGrowth( void );

	T&			AddToTail( int *id_el = nullptr );  // возвращает ссылку на новый элемент в конце вектора id_el = m_NumElements 
	int			Append( const T& obj );             // добав элемента
	int			FindIndex( const T& obj ) const;    // возвращает индекс данного элемента
	bool		RemoveIndex( int index );           // удаляет элемент по индексу
	bool		Remove( const T & obj );            // удалить элемент

	CUtlVector<T>& operator=( const CUtlVector<T> & other );
	const T&	operator[]( int index ) const;
	T&			operator[]( int index );

	void		Resize( int newsize );
	void		Resize( int newsize, int growth );

	void		DeleteContents( bool clear );

private:

	int		m_NumElements;
	int		m_size;
	int		m_growth;
	T*		m_pData;
};

template<typename T>
int CUtlVector<T>::GetGrowth(void)
{
	return m_growth;
}

template<typename T>
void CUtlVector<T>::SetGrowth( int growth )
{
	assert( growth > 0 );

	int newsize;

	m_growth = growth;

	if (m_pData)
	{
		// изменим размер до заданной велечины
		newsize = m_NumElements + growth - 1;
		newsize -= newsize % growth;
		if (newsize != m_size)
		{
			Resize(newsize);
		}
	}
}

template< typename T >
INLINE CUtlVector<T>::CUtlVector( int growth )
{
	assert( growth > 0 );

	m_pData = nullptr;
	m_growth = growth;

	m_NumElements = 0;

	Clear();
}

template<typename T>
INLINE CUtlVector<T>::CUtlVector( const CUtlVector<T> &other )
{
	m_pData = nullptr;

	m_NumElements = 0;

	*this = other;
}

template<typename T>
INLINE CUtlVector<T>::~CUtlVector( void )
{
	Clear();
}

template<typename T>
INLINE int CUtlVector<T>::FindIndex( const T& obj ) const
{
	for (int i = 0; i < m_NumElements; ++i)
	{
		if (m_pData[i] == obj) {
			return i;
		}
	}
	return -1;
}

template<typename T>
INLINE bool CUtlVector<T>::Remove( const T& obj )
{
	int id = FindIndex(obj);
	if (id >= 0) {
		return RemoveIndex(id);
	}

	return false;
}

template<typename T>
INLINE bool CUtlVector<T>::RemoveIndex( int index )
{
	assert( m_pData != nullptr );
	assert( index >= 0 );
	assert( index < m_NumElements );

	if ( (index < 0 ) || ( index >= m_NumElements) ) {
		return false;
	}
	--m_NumElements;

	for (int i = index; i < m_NumElements; ++i)	{
		m_pData[i] = m_pData[ i + 1 ];
	}

	return true;
}

template<typename T>
INLINE int CUtlVector<T>::Num( void ) const
{
	return m_NumElements;
}

template<typename T>
INLINE T& CUtlVector<T>::operator[]( int index )
{
	assert( index >= 0 );
	assert( index <= m_NumElements );

	return m_pData[ index ];
}

template<typename T>
INLINE const T& CUtlVector<T>::operator[]( int index ) const
{
	assert( index >= 0 );
	assert( index < m_NumElements );

	return m_pData[ index ];
}

template<typename T>
INLINE CUtlVector<T>& CUtlVector<T>::operator=( const CUtlVector<T> & other )
{
	Clear();

	m_NumElements = other.m_NumElements;
	m_size = other.m_size;
	m_growth = other.m_growth;

	if ( m_size )
	{
		m_pData = new T[ m_size ];
		for (int i = 0; i < m_NumElements; ++i)	{
			m_pData[ i ] = other.m_pData[ i ];
		}
	}

	return *this;
}

template<typename T>
INLINE void CUtlVector<T>::Resize( int newsize )
{
	T* temp;
	assert( newsize >= 0 );

	// очищаем вектор если нет данных
	if ( newsize <= 0 )
	{
		Clear();
		return;
	}

	if ( newsize == m_size ) {
		// если размеры совпадают, просто выходим
		return;
	}

	temp = m_pData;
	m_size = newsize;

	if ( m_size < m_NumElements ) {
		m_NumElements = m_size;
	}

	// копируем старый вектор в новый
	m_pData = new T[ m_size ];

	for (int i = 0; i < m_NumElements; ++i) {
		m_pData[ i ] = temp[ i ];
	}

	// удаляем данные старого вектора
	if ( temp )	{
		delete[] temp;
	}
}

template<typename T>
void CUtlVector<T>::Resize(int newsize, int growth)
{
	assert( newsize >= 0);
	assert( growth > 0);

	m_growth = growth;

	if ( newsize <= 0 )
	{
		Clear();
		return;
	}

	T	*temp = m_pData;
	m_size = newsize;

	if ( m_size < m_NumElements )
	{
		m_NumElements = m_size;
	}

	m_pData = new T[ m_size ];
	for (int i = 0; i < m_NumElements; ++i)
	{
		m_pData[i] = temp[i];
	}

	if ( temp )
		delete[] temp;
}

template<typename T>
INLINE void CUtlVector<T>::DeleteContents( bool clear )
{
	for (int i = 0; i < m_NumElements; ++i) {
		SAFE_DELETE(m_pData[i]);
	}

	if ( clear ) {
		Clear();
	} else {
		memset( m_pData, 0, m_size * sizeof( T ) );
	}
}

template<typename T>
T& CUtlVector<T>::AddToTail(int *id_el /*= nullptr */)
{
	if ( !m_pData ) {
		Resize( m_growth );
	}

	if ( m_NumElements == m_size ) {
		Resize( m_size + m_growth );
	}

	++m_NumElements;

	if (id_el) {
		*id_el = m_NumElements;
	}
	
	return m_pData[ m_NumElements ];
}

template<typename T>
INLINE int CUtlVector<T>::Append( const T& obj )
{
	if ( !m_pData) {
		Resize( m_growth );
	}

	if ( m_NumElements == m_size )
	{
		int newsize;

		if (m_growth == 0 )
			m_growth = 16;

		newsize = m_size + m_growth;
		Resize( newsize - newsize % m_growth );
	}

	m_pData[ m_NumElements ] = obj;
	++m_NumElements;

	return m_NumElements - 1;
}

template<typename T>
INLINE void CUtlVector<T>::Clear( void )
{
	if ( m_pData ) {
		delete[] m_pData;
	}
	m_pData = nullptr;
	m_NumElements = m_size = 0;
}
