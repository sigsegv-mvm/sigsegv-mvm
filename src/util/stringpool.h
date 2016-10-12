#ifndef _INCLUDE_SIGSEGV_UTIL_STRINGPOOL_H_
#define _INCLUDE_SIGSEGV_UTIL_STRINGPOOL_H_


class StringPool
{
public:
	StringPool(bool persist) :
		m_pStrings(new std::unordered_set<std::string>()), m_bPersist(persist) {}
	
	~StringPool()
	{
		/* intentionally leak the string pool if persistence was requested */
		if (!this->m_bPersist) {
			delete this->m_pStrings;
		}
	}
	
	const char *operator()(const char *c_str)
	{
		std::string str(c_str);
		
		auto result = this->m_pStrings->insert(str);
		
		auto it    = result.first;
		bool added = result.second;
		
		if (added) {
			DevMsg("StringPool: added new string \"%s\"\n", c_str);
		}
		
		return (*it).c_str();
	}
	
private:
	std::unordered_set<std::string> *m_pStrings;
	bool m_bPersist;
};


#endif
