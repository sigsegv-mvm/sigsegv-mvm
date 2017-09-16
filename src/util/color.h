#ifndef _INCLUDE_SIGSEGV_UTIL_COLOR_H
#define _INCLUDE_SIGSEGV_UTIL_COLOR_H


class Color32
{
public:
	constexpr Color32() = default;
	
#if !defined _MSC_VER
	#warning TODO: go thru entire codebase and enable constexpr for MSVC
	constexpr
#endif
	Color32(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 0xff)
	{
		m_Bytes[0] = r;
		m_Bytes[1] = g;
		m_Bytes[2] = b;
		m_Bytes[3] = a;
	}
	
	Color32(const Color32& c1, const Color32& c2, float val, float vMin = 0.0f, float vMax = 1.0f)
	{
		R() = RemapValClamped(val, vMin, vMax, c1.R(), c2.R());
		G() = RemapValClamped(val, vMin, vMax, c1.G(), c2.G());
		B() = RemapValClamped(val, vMin, vMax, c1.B(), c2.B());
		A() = RemapValClamped(val, vMin, vMax, c1.A(), c2.A());
	}
	
	Color32(const Color32& c1, const Color32& c2, const Color32& c3, float val, float vMin = 0.0f, float vMid = 0.5f, float vMax = 1.0f)
	{
		R() = (val < vMid ? RemapValClamped(val, vMin, vMid, c1.R(), c2.R()) : RemapValClamped(val, vMid, vMax, c2.R(), c3.R()));
		G() = (val < vMid ? RemapValClamped(val, vMin, vMid, c1.G(), c2.G()) : RemapValClamped(val, vMid, vMax, c2.G(), c3.G()));
		B() = (val < vMid ? RemapValClamped(val, vMin, vMid, c1.B(), c2.B()) : RemapValClamped(val, vMid, vMax, c2.B(), c3.B()));
		A() = (val < vMid ? RemapValClamped(val, vMin, vMid, c1.A(), c2.A()) : RemapValClamped(val, vMid, vMax, c2.A(), c3.A()));
	}
	
	      uint8_t& R()       { return m_Bytes[0]; }
	const uint8_t& R() const { return m_Bytes[0]; }
	      uint8_t& G()       { return m_Bytes[1]; }
	const uint8_t& G() const { return m_Bytes[1]; }
	      uint8_t& B()       { return m_Bytes[2]; }
	const uint8_t& B() const { return m_Bytes[2]; }
	      uint8_t& A()       { return m_Bytes[3]; }
	const uint8_t& A() const { return m_Bytes[3]; }
	
	operator       Color&()       { return *reinterpret_cast<      Color *>(this); }
	operator const Color&() const { return *reinterpret_cast<const Color *>(this); }
	
private:
	uint8_t m_Bytes[4] = { 0x00, 0x00, 0x00, 0xff };
};


#ifndef C32_EXPAND
#define C32_EXPAND(col32) col32.R(), col32.G(), col32.B(), col32.A()
#endif


#endif
