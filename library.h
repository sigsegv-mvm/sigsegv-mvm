#ifndef _INCLUDE_SIGSEGV_LIBRARY_H_
#define _INCLUDE_SIGSEGV_LIBRARY_H_


enum class Library : int
{
	SERVER,
	ENGINE,
};


class LibMgr
{
public:
	static void SetPtr(Library lib, void *ptr);
	static void *GetPtr(Library lib);
	
private:
	LibMgr() {}
	
	static void *s_pLibServer;
	static void *s_pLibEngine;
};


#endif
