///////////////////////////////////////////////////////////////////////
//
// Multithreaded Software Rasterizer
// Copyright 2010 - 2012 :: Zach Bethel 
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License v2
// as published by the Free Software Foundation.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
//
///////////////////////////////////////////////////////////////////////

#ifndef MSR_SSEMATH_H
#define MSR_SSEMATH_H

#include <cmath>
#include <cstring>
#include <iostream>
#include <xmmintrin.h>
#include <emmintrin.h>
#include <smmintrin.h>

#define MSR_SSE_ALIGNED __declspec(align(16))

// F O R W A R D   D E C L A R A T I O N S //////////////////////////////////

typedef __m128 float4;

class MSR_SSEFloat;
class MSR_SSEVec2;
class MSR_SSEVec3;
class MSR_SSEVec4;

typedef MSR_SSEVec2 MSR_SSEColor2;
typedef MSR_SSEVec3 MSR_SSEColor3;
typedef MSR_SSEVec4 MSR_SSEColor4;

MSR_SSE_ALIGNED class MSR_SSEFloat
{
public:
	MSR_SSEFloat() {}
	MSR_SSEFloat(const __m128 &ps);
	MSR_SSEFloat(float ss);
	MSR_SSEFloat(float A, float B, float C, float D);

	// Assignment operators
	MSR_SSEFloat operator += ( const MSR_SSEFloat& );
	MSR_SSEFloat operator -= ( const MSR_SSEFloat& );
	MSR_SSEFloat operator *= ( const MSR_SSEFloat& );
	MSR_SSEFloat operator /= ( const MSR_SSEFloat& );
	MSR_SSEFloat operator += ( float );
	MSR_SSEFloat operator -= ( float );
	MSR_SSEFloat operator *= ( float );
	MSR_SSEFloat operator /= ( float );

	// Unary operators
	MSR_SSEFloat operator + () const;
	MSR_SSEFloat operator - () const;

	// Casting operators
	float4 &operator * ();
	operator float4* ();
	operator const float4* () const;

	// Binary operators
	MSR_SSEFloat operator + ( const MSR_SSEFloat& ) const;
	MSR_SSEFloat operator - ( const MSR_SSEFloat& ) const;
	MSR_SSEFloat operator * ( const MSR_SSEFloat& ) const;
	MSR_SSEFloat operator / ( const MSR_SSEFloat& ) const;
	MSR_SSEFloat operator + ( float ) const;
	MSR_SSEFloat operator - ( float ) const;
	MSR_SSEFloat operator * ( float ) const;
	MSR_SSEFloat operator / ( float ) const;

public:

	float4 f;
};

class MSR_SSEVec2
{
public:
	MSR_SSEVec2() {}
	MSR_SSEVec2(const MSR_SSEFloat &x, const MSR_SSEFloat &y);

	// Functions
	MSR_SSEFloat Length() const;
	MSR_SSEFloat LengthSqr() const;
	MSR_SSEFloat Dot( const MSR_SSEVec2& ) const;
	void  Lerp( const MSR_SSEVec2& v1, const MSR_SSEVec2& v2, const MSR_SSEFloat& t );
	void  Build( const MSR_SSEVec2& v1, const MSR_SSEVec2& v2 );
	void  Normalize();

	// Assignment operators
	MSR_SSEVec2& operator += ( const MSR_SSEVec2& );
	MSR_SSEVec2& operator -= ( const MSR_SSEVec2& );
	MSR_SSEVec2& operator *= ( const MSR_SSEVec2& );
	MSR_SSEVec2& operator /= ( const MSR_SSEVec2& );
	MSR_SSEVec2& operator += ( const MSR_SSEFloat& );
	MSR_SSEVec2& operator -= ( const MSR_SSEFloat& );
	MSR_SSEVec2& operator *= ( const MSR_SSEFloat& );
	MSR_SSEVec2& operator /= ( const MSR_SSEFloat& );

	// Unary operators
	MSR_SSEVec2 operator + () const;
	MSR_SSEVec2 operator - () const;

	// Casting operators
	operator MSR_SSEFloat* ();
	operator const MSR_SSEFloat* () const;

	// Binary operators
	MSR_SSEVec2 operator + ( const MSR_SSEVec2& ) const;
	MSR_SSEVec2 operator - ( const MSR_SSEVec2& ) const;
	MSR_SSEVec2 operator * ( const MSR_SSEVec2& ) const;
	MSR_SSEVec2 operator / ( const MSR_SSEVec2& ) const;
	MSR_SSEVec2 operator + ( const MSR_SSEFloat& ) const;
	MSR_SSEVec2 operator - ( const MSR_SSEFloat& ) const;
	MSR_SSEVec2 operator * ( const MSR_SSEFloat& ) const;
	MSR_SSEVec2 operator / ( const MSR_SSEFloat& ) const;

	friend MSR_SSEVec2 operator * ( MSR_SSEFloat&, const MSR_SSEVec2& );

public:

	MSR_SSEFloat x, y;
};

class MSR_SSEVec3
{
public:
	MSR_SSEVec3() {}
	MSR_SSEVec3(const MSR_SSEFloat &x, const MSR_SSEFloat &y, const MSR_SSEFloat &z);

	// Functions
	MSR_SSEFloat Length() const;
	MSR_SSEFloat LengthSqr() const;
	MSR_SSEFloat Dot( const MSR_SSEVec3& ) const;
	void  Lerp( const MSR_SSEVec3& v1, const MSR_SSEVec3& v2, const MSR_SSEFloat& t );
	void  Build( const MSR_SSEVec3& v1, const MSR_SSEVec3& v2 );
	void  Normalize();

	// Assignment operators
	MSR_SSEVec3& operator += ( const MSR_SSEVec3& );
	MSR_SSEVec3& operator -= ( const MSR_SSEVec3& );
	MSR_SSEVec3& operator *= ( const MSR_SSEVec3& );
	MSR_SSEVec3& operator /= ( const MSR_SSEVec3& );
	MSR_SSEVec3& operator += ( const MSR_SSEFloat& );
	MSR_SSEVec3& operator -= ( const MSR_SSEFloat& );
	MSR_SSEVec3& operator *= ( const MSR_SSEFloat& );
	MSR_SSEVec3& operator /= ( const MSR_SSEFloat& );

	// Unary operators
	MSR_SSEVec3 operator + () const;
	MSR_SSEVec3 operator - () const;

	// Casting operators
	operator MSR_SSEFloat* ();
	operator const MSR_SSEFloat* () const;

	// Binary operators
	MSR_SSEVec3 operator + ( const MSR_SSEVec3& ) const;
	MSR_SSEVec3 operator - ( const MSR_SSEVec3& ) const;
	MSR_SSEVec3 operator * ( const MSR_SSEVec3& ) const;
	MSR_SSEVec3 operator / ( const MSR_SSEVec3& ) const;
	MSR_SSEVec3 operator + ( const MSR_SSEFloat& ) const;
	MSR_SSEVec3 operator - ( const MSR_SSEFloat& ) const;
	MSR_SSEVec3 operator * ( const MSR_SSEFloat& ) const;
	MSR_SSEVec3 operator / ( const MSR_SSEFloat& ) const;

	friend MSR_SSEVec3 operator * ( const MSR_SSEFloat&, const MSR_SSEVec3& );

public:

	union 
	{
		// positions
		struct 
		{
			MSR_SSEFloat x, y, z;
		};

		// colors
		struct 
		{
			MSR_SSEFloat r, g, b;
		};
	};
};

class MSR_SSEVec4
{
public:
	MSR_SSEVec4() {}
	MSR_SSEVec4(const MSR_SSEFloat &x, const MSR_SSEFloat &y, const MSR_SSEFloat &z, const MSR_SSEFloat &w);

	// Functions
	MSR_SSEFloat Length() const;
	MSR_SSEFloat LengthSqr() const;
	MSR_SSEFloat Dot( const MSR_SSEVec4& ) const;
	void  Lerp( const MSR_SSEVec4& v1, const MSR_SSEVec4& v2, const MSR_SSEFloat& t );
	void  Build( const MSR_SSEVec4& v1, const MSR_SSEVec4& v2 );
	void  Normalize();

	// Assignment operators
	MSR_SSEVec4& operator += ( const MSR_SSEVec4& );
	MSR_SSEVec4& operator -= ( const MSR_SSEVec4& );
	MSR_SSEVec4& operator *= ( const MSR_SSEVec4& );
	MSR_SSEVec4& operator /= ( const MSR_SSEVec4& );
	MSR_SSEVec4& operator += ( const MSR_SSEFloat& );
	MSR_SSEVec4& operator -= ( const MSR_SSEFloat& );
	MSR_SSEVec4& operator *= ( const MSR_SSEFloat& );
	MSR_SSEVec4& operator /= ( const MSR_SSEFloat& );

	// Unary operators
	MSR_SSEVec4 operator + () const;
	MSR_SSEVec4 operator - () const;

	// Casting operators
	operator MSR_SSEFloat* ();
	operator const MSR_SSEFloat* () const;

	// Binary operators
	MSR_SSEVec4 operator + ( const MSR_SSEVec4& ) const;
	MSR_SSEVec4 operator - ( const MSR_SSEVec4& ) const;
	MSR_SSEVec4 operator * ( const MSR_SSEVec4& ) const;
	MSR_SSEVec4 operator / ( const MSR_SSEVec4& ) const;
	MSR_SSEVec4 operator + ( const MSR_SSEFloat& ) const;
	MSR_SSEVec4 operator - ( const MSR_SSEFloat& ) const;
	MSR_SSEVec4 operator * ( const MSR_SSEFloat& ) const;
	MSR_SSEVec4 operator / ( const MSR_SSEFloat& ) const;

	friend MSR_SSEVec4 operator * ( const MSR_SSEFloat&, const MSR_SSEVec4& );

public:

	union 
	{
		// positions
		struct 
		{
			MSR_SSEFloat x, y, z, w;
		};

		// colors
		struct 
		{
			MSR_SSEFloat r, g, b, a;
		};
	};
};

inline MSR_SSEFloat MSR_SSESqrt( const MSR_SSEFloat &f )
{
	return _mm_sqrt_ps(*f);
}

inline MSR_SSEFloat MSR_SSERSqrt( const MSR_SSEFloat &f )
{
	return _mm_rsqrt_ps(*f);
}

#endif 