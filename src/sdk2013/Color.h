//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================//

#ifndef COLOR_H
#define COLOR_H

#ifdef _WIN32
#pragma once
#endif

// public/Color.h
#define _SIGSEGV_SDK2013_OVERRIDE__PUBLIC_COLOR_H 1
// sigsegv modifications [WRT: Valve SDK2013 0d8dceea 20150909 / AlliedModders 0ef5d3d4 20171105]
// - convert member variable into a union
// - make ctors constexpr
// - make member funcs constexpr

//-----------------------------------------------------------------------------
// Purpose: Basic handler for an rgb set of colors
//			This class is fully inline
//-----------------------------------------------------------------------------
class Color
{
public:
	// constructors
	constexpr Color() : _i32(0) {}
	constexpr Color(int _r, int _g, int _b, int _a = 0) : _u8{ _r, _g, _b, _a } {}
	
	// set the color
	// r - red component (0-255)
	// g - green component (0-255)
	// b - blue component (0-255)
	// a - alpha component, controls transparency (0 - transparent, 255 - opaque);
	constexpr void SetColor(int _r, int _g, int _b, int _a = 0)
	{
		_u8[0] = (unsigned char)_r;
		_u8[1] = (unsigned char)_g;
		_u8[2] = (unsigned char)_b;
		_u8[3] = (unsigned char)_a;
	}

	constexpr void GetColor(int &_r, int &_g, int &_b, int &_a) const
	{
		_r = _u8[0];
		_g = _u8[1];
		_b = _u8[2];
		_a = _u8[3];
	}

	constexpr void SetRawColor( int color32 )
	{
		_i32 = color32;
	}

	constexpr int GetRawColor() const
	{
		return _i32;
	}

	constexpr int r() const	{ return _u8[0]; }
	constexpr int g() const	{ return _u8[1]; }
	constexpr int b() const	{ return _u8[2]; }
	constexpr int a() const	{ return _u8[3]; }
	
	constexpr unsigned char &operator[](int index)
	{
		return _u8[index];
	}

	constexpr const unsigned char &operator[](int index) const
	{
		return _u8[index];
	}

	constexpr bool operator == (const Color &rhs) const
	{
		return ( GetRawColor() == rhs.GetRawColor() );
	}

	constexpr bool operator != (const Color &rhs) const
	{
		return !(operator==(rhs));
	}

	constexpr Color &operator=( const Color &rhs )
	{
		SetRawColor( rhs.GetRawColor() );
		return *this;
	}

private:
	union
	{
		int _i32;
		unsigned char _u8[4];
	};
};
static_assert(sizeof(Color) == 4);


#endif // COLOR_H
