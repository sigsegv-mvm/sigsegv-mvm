#ifndef _INCLUDE_SIGSEGV_MOD_H_
#define _INCLUDE_SIGSEGV_MOD_H_


#include "common.h"


class IMod
{
public:
	virtual const char *GetName() const final { return this->m_pszName; }
	
	virtual bool OnLoad(char *err, size_t maxlen) = 0;
	virtual void OnUnload() = 0;
	
protected:
	IMod(const char *name);
	virtual ~IMod();
	
private:
	const char *m_pszName;
};


#endif
