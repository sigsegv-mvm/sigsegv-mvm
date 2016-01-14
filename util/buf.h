#ifndef _INCLUDE_SIGSEGV_UTIL_BUF_H_
#define _INCLUDE_SIGSEGV_UTIL_BUF_H_


#include "common.h"


class ByteBuf
{
public:
	ByteBuf(int size);
	virtual ~ByteBuf();
	
	uint8_t& operator[](int idx);
	const uint8_t& operator[](int idx) const;
	
	void SetAll(uint8_t val);
	void SetRange(int idx, int len, uint8_t val);
	
	void CopyFrom(const ByteBuf& that);
	void CopyFrom(const uint8_t *arr);
	
	int GetSize() const { return this->m_iSize; }
	
	const uint8_t *GetBufPtr() const { return this->m_Buf; }
	
private:
	const int m_iSize;
	uint8_t *m_Buf = nullptr;
};


inline ByteBuf::ByteBuf(int size) :
	m_iSize(size)
{
	this->m_Buf = new uint8_t[size];
}

inline ByteBuf::~ByteBuf()
{
	delete[] this->m_Buf;
}


inline uint8_t& ByteBuf::operator[](int idx)
{
	assert(idx >= 0 && idx < this->m_iSize);
	return this->m_Buf[idx];
}

inline const uint8_t& ByteBuf::operator[](int idx) const
{
	assert(idx >= 0 && idx < this->m_iSize);
	return this->m_Buf[idx];
}


inline void ByteBuf::SetAll(uint8_t val)
{
	memset(this->m_Buf, val, this->m_iSize);
}

inline void ByteBuf::SetRange(int idx, int len, uint8_t val)
{
	assert(idx >= 0 && idx + len <= this->m_iSize);
	
	for (int i = 0; i < len; ++i) {
		this->m_Buf[idx + i] = val;
	}
}


inline void ByteBuf::CopyFrom(const ByteBuf& that)
{
	assert(this->m_iSize == that.m_iSize);
	memcpy(this->m_Buf, that.m_Buf, this->m_iSize);
}

inline void ByteBuf::CopyFrom(const uint8_t *arr)
{
	memcpy(this->m_Buf, arr, this->m_iSize);
}


#endif
