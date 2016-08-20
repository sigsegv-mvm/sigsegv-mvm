#ifndef _INCLUDE_SIGSEGV_STUB_FONTS_H_
#define _INCLUDE_SIGSEGV_STUB_FONTS_H_


#if defined _WINDOWS

class CWin32Font
{
public:
	HFONT m_hFont;
	HDC m_hDC;
	HBITMAP m_hDIB;
	
	unsigned char *m_pBuf;
	
	CUtlSymbol m_szName;
	
	short m_iTall;
	unsigned short m_iWeight;
	unsigned short m_iFlags;
	unsigned short m_iScanLines;
	unsigned short m_iBlur;
	unsigned short m_rgiBitmapSize[2];
	bool m_bUnderlined;
	
	unsigned int m_iHeight           : 8;
	unsigned int m_iMaxCharWidth     : 8;
	unsigned int m_iAscent           : 8;
	unsigned int m_iDropShadowOffset : 1;
	unsigned int m_iOutlineSize      : 1;
	unsigned int m_bAntiAliased      : 1;
	unsigned int m_bRotary           : 1;
	unsigned int m_bAdditive         : 1;
};

#endif


class CFontManager
{
public:
	
};


CFontManager& FontManager();


#endif
