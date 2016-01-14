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


template<typename T>
class AutoNameMap
{
public:
	AutoNameMap()
	{
		T *t = static_cast<T *>(this);
		std::string name(t->GetName());
		
		AllocateMap();
		assert(s_Map->find(name) == s_Map->end());
		(*s_Map)[name] = t;
	}
	virtual ~AutoNameMap()
	{
		T *t = static_cast<T *>(this);
		std::string name(t->GetName());
		
		assert(s_Map != nullptr);
		assert(s_Map->erase(name) == 1);
	}
	
	static const std::map<std::string, T *>& Map()
	{
		AllocateMap();
		return *s_Map;
	}
	
private:
	static void AllocateMap()
	{
		if (s_Map == nullptr) {
			s_Map = new std::map<std::string, T *>();
		}
	}
	
	static std::map<std::string, T *> *s_Map;
};
template<typename T> std::map<std::string, T *> *AutoNameMap<T>::s_Map = nullptr;


#endif
