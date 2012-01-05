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

#ifndef MSR_MATH_H
#define MSR_MATH_H

#include <cmath>
#include <cstring>
#include <iostream>
#include <xmmintrin.h>
#include <smmintrin.h>

#define MSR_SSE_ALIGNED __declspec(align(16))

// D E F I N E S ////////////////////////////////////////////////////////////

#define MSR_PI 3.141592654f
#define MSR_PI2 1.5707963f
#define MSR_2PI 6.2831853f
#define MSR_EPSILON 0.00001f

#define MSR_ToRadian( degree ) ((degree) * (MSR_PI / 180.0f))
#define MSR_ToDegree( radian ) ((radian) * (180.0f / MSR_PI))

// F O R W A R D   D E C L A R A T I O N S //////////////////////////////////

MSR_SSE_ALIGNED class MSR_Vec2;
MSR_SSE_ALIGNED class MSR_Vec3;
MSR_SSE_ALIGNED class MSR_Vec4;
MSR_SSE_ALIGNED class MSR_Mat4x4;
MSR_SSE_ALIGNED class MSR_Mat3x3;
MSR_SSE_ALIGNED class MSR_Plane;

typedef MSR_Vec3 MSR_Color3;
typedef MSR_Vec4 MSR_Color4;

MSR_SSE_ALIGNED class MSR_Vec2
{
public:
	MSR_Vec2() {}
	MSR_Vec2( const float* );
	MSR_Vec2( const MSR_Vec2& );
	MSR_Vec2( float x, float y );

	// Functions
	void Set(float x, float y);
	float Length() const;
	float LengthSqr() const;
	float Dot( const MSR_Vec2& ) const;
	void  Lerp( const MSR_Vec2& v1, const MSR_Vec2& v2, float t );
	void  Build( const MSR_Vec2& v1, const MSR_Vec2& v2 );
	void  Normalize();
	void  Print();	

	// Assignment operators
	MSR_Vec2& operator += ( const MSR_Vec2& );
	MSR_Vec2& operator -= ( const MSR_Vec2& );
	MSR_Vec2& operator += ( float );
	MSR_Vec2& operator -= ( float );
	MSR_Vec2& operator *= ( float );
	MSR_Vec2& operator /= ( float );

	// Unary operators
	MSR_Vec2 operator + () const;
	MSR_Vec2 operator - () const;

	// Casting operators
	operator float* ();
	operator const float* () const;

	// Binary operators
	MSR_Vec2 operator + ( const MSR_Vec2& ) const;
	MSR_Vec2 operator - ( const MSR_Vec2& ) const;
	MSR_Vec2 operator + ( float ) const;
	MSR_Vec2 operator - ( float ) const;
	MSR_Vec2 operator * ( float ) const;
	MSR_Vec2 operator / ( float ) const;

	friend MSR_Vec2 operator * ( float, const MSR_Vec2& );

	// Equality operators
	bool operator == ( const MSR_Vec2& ) const;
	bool operator != ( const MSR_Vec2& ) const;

public:

	union 
	{
		// array indexed storage
		float m[2];

		// explicit names
		struct 
		{
			float x, y;
		};
	};
};

MSR_SSE_ALIGNED class MSR_Vec3
{
public:
	MSR_Vec3() {}
	MSR_Vec3( const float* );
	MSR_Vec3( const MSR_Vec2&, float z );
	MSR_Vec3( float x, float y, float z );

	// Functions
	void Set(float x, float y, float z);
	float Length() const;
	float LengthSqr() const;
	float Dot( const MSR_Vec3& ) const;
	void  Cross( const MSR_Vec3& v1, const MSR_Vec3& v2 );
	void  Lerp( const MSR_Vec3& v1, const MSR_Vec3& v2, float t );
	void  Build( const MSR_Vec3& v1, const MSR_Vec3& v2 );
	void  Normalize();
	void  Print();

	// Assignment operators
	MSR_Vec3& operator += ( const MSR_Vec3& );
	MSR_Vec3& operator -= ( const MSR_Vec3& );
	MSR_Vec3& operator += ( float );
	MSR_Vec3& operator -= ( float );
	MSR_Vec3& operator *= ( float );
	MSR_Vec3& operator /= ( float );

	// Unary operators
	MSR_Vec3 operator + () const;
	MSR_Vec3 operator - () const;

	// Casting operators
	operator float* ();
	operator const float* () const;

	// Binary operators
	MSR_Vec3 operator + ( const MSR_Vec3& ) const;
	MSR_Vec3 operator - ( const MSR_Vec3& ) const;
	MSR_Vec3 operator * ( const MSR_Vec3& ) const;
	MSR_Vec3 operator / ( const MSR_Vec3& ) const;
	MSR_Vec3 operator + ( float ) const;
	MSR_Vec3 operator - ( float ) const;
	MSR_Vec3 operator * ( float ) const;
	MSR_Vec3 operator / ( float ) const;

	friend MSR_Vec3 operator * ( float, const MSR_Vec3& );

	// Equality operators
	bool operator == ( const MSR_Vec3& ) const;
	bool operator != ( const MSR_Vec3& ) const;

public:

	union 
	{
		// Array indexed storage
		float m[3];

		// Explicit names
		struct
		{
			float x, y, z;
		};

		// Colors
		struct
		{
			float r, g, b;
		};
	};
};

MSR_SSE_ALIGNED class MSR_Vec4
{
public:
	MSR_Vec4() {}
	MSR_Vec4( const float* );
	MSR_Vec4( const MSR_Vec3&, float w = 1.0f );
	MSR_Vec4( float x, float y, float z, float w = 1.0f );

	// Functions
	float Length() const;
	float LengthSqr() const;
	float Dot( const MSR_Vec4& ) const;
	void  Cross( const MSR_Vec4& v1, const MSR_Vec4& v2 );
	void  Lerp( const MSR_Vec4& v1, const MSR_Vec4& v2, float t );
	void  Build( const MSR_Vec4& v1, const MSR_Vec4& v2 );
	void  Normalize();
	void  Homogenize();
	void  Print();

	// Assignment operators
	MSR_Vec4& operator += ( const MSR_Vec4& );
	MSR_Vec4& operator -= ( const MSR_Vec4& );
	MSR_Vec4& operator += ( float );
	MSR_Vec4& operator -= ( float );
	MSR_Vec4& operator *= ( float );
	MSR_Vec4& operator /= ( float );

	// Unary operators
	MSR_Vec4 operator + () const;
	MSR_Vec4 operator - () const;

	// Casting operators
	operator float* ();
	operator const float* () const;
	operator const MSR_Vec3 &() const;

	// Binary operators
	MSR_Vec4 operator + ( const MSR_Vec4& ) const;
	MSR_Vec4 operator - ( const MSR_Vec4& ) const;
	MSR_Vec4 operator * ( const MSR_Vec4& ) const;
	MSR_Vec4 operator / ( const MSR_Vec4& ) const;
	MSR_Vec4 operator + ( float ) const;
	MSR_Vec4 operator - ( float ) const;
	MSR_Vec4 operator * ( float ) const;
	MSR_Vec4 operator / ( float ) const;

	friend MSR_Vec4 operator * ( float, const MSR_Vec4& );

	// Equality operators
	bool operator == ( const MSR_Vec4& ) const;
	bool operator != ( const MSR_Vec4& ) const;

public:

	union 
	{
		// Array indexed storage
		float m[4];

		// Explicit names
		struct
		{
			float x, y, z, w;
		};

		// Colors
		struct  
		{
			float r, g, b, a;
		};
	};
};

MSR_SSE_ALIGNED class MSR_Mat4x4
{
public:
	MSR_Mat4x4() {}
	MSR_Mat4x4( const float * );
	MSR_Mat4x4( const MSR_Mat4x4& );
	MSR_Mat4x4( float _11, float _12, float _13, float _14,
				float _21, float _22, float _23, float _24,
				float _31, float _32, float _33, float _34,
				float _41, float _42, float _43, float _44 );

	// Helper functions
	bool IsIdentity();
	void SetIdentity();
	void Translate( float x, float y, float z );
	void Scale( float x, float y, float z);
	void SetRotateX( float x );
	void SetRotateY( float y );
	void SetRotateZ( float z );
	void SetRotateAxis( const MSR_Vec3& axis, float d );
	void SetTransposeOf( const MSR_Mat4x4& m );
	float GetDeterminant() const;
	int  SetInverseOf( const MSR_Mat4x4& m );
	void SetLookAtLH( const MSR_Vec3& eye, const MSR_Vec3& at, const MSR_Vec3& up );
	void SetLookAtRH( const MSR_Vec3& eye, const MSR_Vec3& at, const MSR_Vec3& up );
	void SetPerspectiveFovLH( float fov, float aspect, float zn, float zf );
	void SetPerspectiveFovRH( float fov, float aspect, float zn, float zf );
	void SetOrthoLH( float w, float h, float zn, float zf );
	void SetOrthoRH( float w, float h, float zn, float zf );
	void SetOrthoOffCenterLH( float l, float r, float b, float t, float zn, float zf );
	void SetOrthoOffCenterRH( float l, float r, float b, float t, float zn, float zf );
	MSR_Vec4 TransformNormal( const MSR_Vec4& v ) const;
	void Print();

	// Access grants
	float& operator () ( unsigned int row, unsigned int col );
	float  operator () ( unsigned int row, unsigned int col ) const;

	// Casting operators
	operator float* ();
	operator const float* () const;

	// Assignment operators
	void operator = ( const MSR_Mat4x4& ) const;
	MSR_Mat4x4& operator *= ( const MSR_Mat4x4& );
	MSR_Mat4x4& operator += ( const MSR_Mat4x4& );
	MSR_Mat4x4& operator -= ( const MSR_Mat4x4& );
	MSR_Mat4x4& operator += ( float );
	MSR_Mat4x4& operator -= ( float );
	MSR_Mat4x4& operator *= ( float );
	MSR_Mat4x4& operator /= ( float );

	// Unary operators
	MSR_Mat4x4 operator + () const;
	MSR_Mat4x4 operator - () const;

	// Binary operators
	MSR_Mat4x4 operator * ( const MSR_Mat4x4& ) const;
	MSR_Vec4 operator * ( const MSR_Vec4& ) const;
	MSR_Mat4x4 operator + ( const MSR_Mat4x4& ) const;
	MSR_Mat4x4 operator - ( const MSR_Mat4x4& ) const;
	MSR_Mat4x4 operator + ( float ) const;
	MSR_Mat4x4 operator - ( float ) const;
	MSR_Mat4x4 operator * ( float ) const;
	MSR_Mat4x4 operator / ( float ) const;

	friend MSR_Mat4x4 operator * ( float, const MSR_Mat4x4& );

public:

	static const MSR_Mat4x4 Identity;

	union 
	{
		// Array indexed storage
		float m[16];

		// Explicit names
		struct
		{
			float _11, _12, _13, _14,
				  _21, _22, _23, _24,
				  _31, _32, _33, _34,
				  _41, _42, _43, _44;
		};
	};
};

MSR_SSE_ALIGNED class MSR_Mat3x3
{
public:
	MSR_Mat3x3() {}
	MSR_Mat3x3( const float * );
	MSR_Mat3x3( const MSR_Mat3x3& );
	MSR_Mat3x3( float _11, float _12, float _13,
		float _21, float _22, float _23,
		float _31, float _32, float _33 );

	// Helper functions
	bool IsIdentity();
	void SetIdentity();
	void Translate( float x, float y );
	void Scale( float x, float y );
	void SetRotate( float rot );
	void SetTransposeOf( const MSR_Mat3x3& m );
	int  SetInverseOf( const MSR_Mat3x3& m );
	MSR_Vec2 Transform( const MSR_Vec2& v );
	float GetDeterminant() const;
	void Print();

	// Access grants
	float& operator () ( unsigned int row, unsigned int col );
	float  operator () ( unsigned int row, unsigned int col ) const;

	// Casting operators
	operator float* ();
	operator const float* () const;

	// Assignment operators
	void operator = ( const MSR_Mat3x3& ) const;
	MSR_Mat3x3& operator *= ( const MSR_Mat3x3& );
	MSR_Mat3x3& operator += ( const MSR_Mat3x3& );
	MSR_Mat3x3& operator -= ( const MSR_Mat3x3& );
	MSR_Mat3x3& operator += ( float );
	MSR_Mat3x3& operator -= ( float );
	MSR_Mat3x3& operator *= ( float );
	MSR_Mat3x3& operator /= ( float );

	// Unary operators
	MSR_Mat3x3 operator + () const;
	MSR_Mat3x3 operator - () const;

	// Binary operators
	MSR_Mat3x3 operator * ( const MSR_Mat3x3& ) const;
	MSR_Vec3 operator * ( const MSR_Vec3& ) const;
	MSR_Mat3x3 operator + ( const MSR_Mat3x3& ) const;
	MSR_Mat3x3 operator - ( const MSR_Mat3x3& ) const;
	MSR_Mat3x3 operator + ( float ) const;
	MSR_Mat3x3 operator - ( float ) const;
	MSR_Mat3x3 operator * ( float ) const;
	MSR_Mat3x3 operator / ( float ) const;

	friend MSR_Mat3x3 operator * ( float, const MSR_Mat3x3& );

public:

	static const MSR_Mat3x3 Identity;

	union
	{
		// Array indexed storage
		float m[9];

		// Explicit names
		struct
		{
			float _11, _12, _13,
				  _21, _22, _23,
				  _31, _32, _33;
		};
	};
};

MSR_SSE_ALIGNED class MSR_Plane
{
public:
	MSR_Plane() {}
	MSR_Plane( float fa, float fb, float fc, float fd );
	MSR_Plane( const float * );

	void Print();
	float DotCoord(const MSR_Vec3 &v) const;
	void Normalize();

	// Casting operators
	operator float *();
	operator const float *() const;

	// Unary operators
	MSR_Plane operator + () const;
	MSR_Plane operator - () const;

	// Binary operators
	bool operator == ( const MSR_Plane& ) const;
	bool operator != ( const MSR_Plane& ) const;

public:
	float a, b, c, d;
};

// G L O B A L S ////////////////////////////////////////////////////////////

const MSR_Mat4x4 MSR_Mat4x4_Identity = MSR_Mat4x4(1.0f,0.0f,0.0f,0.0f,
												  0.0f,1.0f,0.0f,0.0f,
												  0.0f,0.0f,1.0f,0.0f,
												  0.0f,0.0f,0.0f,1.0f);

const MSR_Mat3x3 MSR_Mat3x3_Identity = MSR_Mat3x3(1.0f,0.0f,0.0f,
												  0.0f,1.0f,0.0f,
												  0.0f,0.0f,1.0f);

// F U N C T I O N S //////////////////////////////////////////////////

inline int iround(float x)
{
	return _mm_cvt_ss2si( _mm_load_ps( &x ) ); 

	/*asm {
		fld x
		fistp t
	}*/

//	return t;
}

#endif
