#ifndef _INCLUDE_LIBSTRCOMPAT_
#define _INCLUDE_LIBSTRCOMPAT_


#include <cstddef>


extern "C"
{
	void *strcompat_alloc(void);
	void  strcompat_free (void *ptr);
	
	size_t strcompat_get(const void *ptr, char *dst, size_t dst_len);
	void   strcompat_set(void *ptr, const char *src);
}


// The purpose of this library is to make it possible to interoperate with
// std::string objects between binaries built with modern GCC versions
// (e.g. 8.2.0) and binaries build with ancient horrible crusty GCC versions
// (e.g. 4.8.x).
// 
// FOR EXAMPLE:
// Imagine you have a Valve game built with GCC 4.8.x and a mod library built
// with GCC 8.2.0 which links to that game dynamically at runtime, and that you
// would like to interact with std::string objects between the mod library and
// the game.
// Also suppose that you've swapped out the old libstdc++.so.6.0.19 that the
// game uses by default and replaced it with the newer libstdc++.so.6.0.25.
// And then, additionally, suppose that you've also compiled the mod library
// with -D_GLIBCXX_USE_CXX11_ABI=0 so that GCC will make it use the
// copy-on-write, pre-GCC-5.1-compatible implementation of std::string.
// 
// Despite the TOTALLY 100% GUARANTEED COMPLETE AWESOME BACKWARD COMPATIBILITY
// of libstdc++, and telling the newer compiler to use the legacy std::string
// implementation, and even passing any imaginable combination of -fabi-version
// and/or -fabi-compat-version to the newer compiler, the scenario described
// above will cause crashes at runtime (such as double-free's) due to some sort
// of ABI incompatibility. This is fucking idiotic.
// 
// Given the above situation, the obvious solution is to invent a bullshit
// stupid "isolation layer" library which allows the new-compiler code to do
// std::string interop with the old-compiler code, without ever actually letting
// the new-compiler code directly touch the std::string's themselves. And so
// that's what this pile-of-shit library is.
// 
// The idea is that you build this library with the old GCC toolchain (4.8.x);
// and then whenever you feel compelled to do some sort of nice, syntactically
// clean thing involving a std::string from the new-compiler code that will
// touch the old-compiler code, you instead replace the simplistic, syntatically
// clean direct thing you would have done with the std::string with probably
// several ugly calls to the functions in this library, so that only the library
// functions built on the old compiler actually directly touch the std::string,
// and the new-compiler code can just touch C strings.
// 
// This is completely stupid and shouldn't even be necessary, but it does work.
// 
// C++ ABI compatibility is already a dumpster fire and everyone knows it. But
// what's even worse is when you're told "oh yeah don't worry, if you just do
// XYZ, then we assure you that everything will be A-OK between these two
// versions", only to then discover after hours of debugging that despite
// following the guidelines laid forth, the assurance you were given is actually
// complete bullshit in practice.
// 
// GCC people (specifically the libsupc++/libstdc++ maintainers):
// Get your shit together. Please.


#endif
