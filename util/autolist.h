#ifndef _INCLUDE_SIGSEGV_UTIL_AUTOLIST_H_
#define _INCLUDE_SIGSEGV_UTIL_AUTOLIST_H_


#include "common.h"


template<typename T>
class AutoList
{
public:
	AutoList()
	{
		AllocateList();
		s_List->push_back(static_cast<T *>(this));
	}
	virtual ~AutoList()
	{
		assert(s_List != nullptr);
		s_List->remove(static_cast<T *>(this));
	}
	
	static const std::list<T *>& List()
	{
		AllocateList();
		return *s_List;
	}
	
private:
	static void AllocateList()
	{
		if (s_List == nullptr) {
			s_List = new std::list<T *>();
		}
	}
	
	static std::list<T *> *s_List;
};
template<typename T> std::list<T *> *AutoList<T>::s_List = nullptr;


#endif
