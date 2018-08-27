#include <cassert>
#include <string>
#include <bsd/string.h>


extern "C" [[gnu::visibility("default")]]
void *strcompat_alloc()
{
	std::string *str = new std::string;
	return reinterpret_cast<void *>(str);
}

extern "C" [[gnu::visibility("default")]]
void strcompat_free(void *ptr)
{
	if (ptr == nullptr) return;
	
	std::string *str = reinterpret_cast<std::string *>(ptr);
	delete str;
}


extern "C" [[gnu::visibility("default")]]
size_t strcompat_get(const void *ptr, char *dst, size_t dst_len)
{
	assert(ptr != nullptr);
	assert(dst != nullptr);
	assert(dst_len >= 1);
	
	const std::string *str = reinterpret_cast<const std::string *>(ptr);
	return strlcpy(dst, str->c_str(), dst_len);
}

extern "C" [[gnu::visibility("default")]]
void strcompat_set(void *ptr, const char *src)
{
	assert(src != nullptr);
	
	std::string *str = reinterpret_cast<std::string *>(ptr);
	str->assign(src);
}
