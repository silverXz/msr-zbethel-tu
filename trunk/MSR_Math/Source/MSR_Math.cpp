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

#include "MSR_Math.h"
#include <iostream>

//
// Print statements for all data structures
//

void MSR_Vec2::Print() 
{
	std::cout << "MSR_Vec2: (" << x << "," << y << ")\n";
}

void MSR_Vec3::Print() 
{
	std::cout << "MSR_Vec3: (" << x << "," << y << "," << z << ")\n";
}

void MSR_Vec4::Print() 
{
	std::cout << "MSR_Vec4: (" << x << "," << y << "," << z << "," << w << ")\n";
}
 
void MSR_Mat4x4::Print()
{
	std::cout << "MSR_Mat4x4: (" << _11 << "," << _12 << "," << _13 << "," << _14 << ")\n";
	std::cout << "            (" << _21 << "," << _22 << "," << _23 << "," << _24 << ")\n";
	std::cout << "            (" << _31 << "," << _32 << "," << _33 << "," << _34 << ")\n";
	std::cout << "            (" << _41 << "," << _42 << "," << _43 << "," << _44 << ")\n";
}

void MSR_Mat3x3::Print()
{
	std::cout << "MSR_Mat3x3: (" << _11 << "," << _12 << "," << _13 << ")\n";
	std::cout << "	          (" << _21 << "," << _22 << "," << _23 << ")\n";
	std::cout << "            (" << _31 << "," << _32 << "," << _33 << ")\n";
}

void MSR_Plane::Print()
{
	std::cout << "MSR_Plane:" << a << "," << b << "," << c << "," << d << ") ";
}

/////////////////////////////////////////////////////////////////////////////////
// MSR_Vec2
/////////////////////////////////////////////////////////////////////////////////

//
// Constructors
//

MSR_Vec2::MSR_Vec2( const float *pf )
{
	x = pf[0];
	y = pf[1];
}

 
MSR_Vec2::MSR_Vec2( const MSR_Vec2& p )
{
	x = p.x;
	y = p.y;
}

 
MSR_Vec2::MSR_Vec2( float fx, float fy )
{
	x = fx;
	y = fy;
}

//
// Casting
//

MSR_Vec2::operator float* ()
{
	return (float *) &x;
}

 
MSR_Vec2::operator const float* () const
{
	return (const float *) &x;
}

//
// Assignment Operators
//

MSR_Vec2 &MSR_Vec2::operator += ( const MSR_Vec2& v )
{
	x += v.x;
	y += v.y;
	return *this;
}

MSR_Vec2 &MSR_Vec2::operator -= ( const MSR_Vec2& v )
{
	x -= v.x;
	y -= v.y;
	return *this;
}

MSR_Vec2 &MSR_Vec2::operator += ( float f )
{
	x += f;
	y += f;
	return *this;
}

MSR_Vec2 &MSR_Vec2::operator -= ( float f )
{
	x -= f;
	y -= f;
	return *this;
}

MSR_Vec2 &MSR_Vec2::operator *= ( float f )
{
	x *= f;
	y *= f;
	return *this;
}

MSR_Vec2 &MSR_Vec2::operator /= ( float f )
{
	x /= f;
	y /= f;
	return *this;
}

//
// Unary Operators
//

MSR_Vec2 MSR_Vec2::operator + () const
{
	return *this;
}

MSR_Vec2 MSR_Vec2::operator - () const
{
	return MSR_Vec2(-x, -y);
}

//
// Binary Operators 
//

MSR_Vec2 MSR_Vec2::operator + ( const MSR_Vec2& p ) const
{
	return MSR_Vec2(x + p.x, y + p.y);
}

MSR_Vec2 MSR_Vec2::operator - ( const MSR_Vec2& p ) const
{
	return MSR_Vec2(x - p.x, y - p.y);
}

MSR_Vec2 MSR_Vec2::operator + ( float f ) const
{	
	return MSR_Vec2(x + f, y + f);
}

MSR_Vec2 MSR_Vec2::operator - ( float f ) const
{	
	return MSR_Vec2(x - f, y - f);
}

MSR_Vec2 MSR_Vec2::operator * ( float f ) const
{
	return MSR_Vec2(x * f, y * f);
}

MSR_Vec2 MSR_Vec2::operator / ( float f ) const
{	
	return MSR_Vec2(x / f, y / f);
}

MSR_Vec2 operator * ( float f, const MSR_Vec2& p )
{
	return MSR_Vec2(f * p.x, f * p.y);
}

bool MSR_Vec2::operator == ( const MSR_Vec2& p ) const
{
	return x == p.x && y == p.y;
}

bool MSR_Vec2::operator != ( const MSR_Vec2& p ) const
{
	return x != p.x || y != p.y;
}

//
// Functions
//

void MSR_Vec2::Set(float _x, float _y)
{
	x = _x;
	y = _y;
}

float MSR_Vec2::Length() const
{
	return sqrtf(x * x + y * y);
}

float MSR_Vec2::LengthSqr() const
{
	return x * x + y * y;
}

float MSR_Vec2::Dot( const MSR_Vec2& v ) const
{
	return x * v.x + y * v.y;
}

void MSR_Vec2::Normalize()
{
	float f = sqrtf(x*x + y*y);
	x/=f; y/=f;
}

void MSR_Vec2::Lerp( const MSR_Vec2& v1, const MSR_Vec2& v2, float t )
{
	x = v1.x + t * (v2.x - v1.x);
	y = v1.y + t * (v2.y - v1.y);
}

void MSR_Vec2::Build( const MSR_Vec2& v1, const MSR_Vec2& v2 )
{
	x = v2.x - v1.x;
	y = v2.y - v1.y;
}

/////////////////////////////////////////////////////////////////////////////////
// MSR_Vec3
/////////////////////////////////////////////////////////////////////////////////

//
// Constructors
// 

MSR_Vec3::MSR_Vec3( const float *pf )
{
	x = pf[0];
	y = pf[1];
	z = pf[2];
}

 
MSR_Vec3::MSR_Vec3( const MSR_Vec2& v, float _z )
{
	x = v.x;
	y = v.y;
	z = _z;
}


MSR_Vec3::MSR_Vec3( float fx, float fy, float fz )
{
	x = fx;
	y = fy;
	z = fz;
}

//
// Casting
//
 
MSR_Vec3::operator float* ()
{
	return (float *)&x;
}

 
MSR_Vec3::operator const float* () const
{
	return (const float *)&x;
}

//
// Assignment Operators
//

MSR_Vec3 &MSR_Vec3::operator += ( const MSR_Vec3& v )
{
	x += v.x;
	y += v.y;
	z += v.z;
	return *this;
}

MSR_Vec3 &MSR_Vec3::operator -= ( const MSR_Vec3& v )
{
	x -= v.x;
	y -= v.y;
	z -= v.z;
	return *this;
}

MSR_Vec3 &MSR_Vec3::operator += ( float f )
{
	x += f;
	y += f;
	z += f;
	return *this;
}

MSR_Vec3 &MSR_Vec3::operator -= ( float f )
{
	x -= f;
	y -= f;
	z -= f;
	return *this;
}

MSR_Vec3 &MSR_Vec3::operator *= ( float f )
{
	x *= f;
	y *= f;
	z *= f;
	return *this;
}

MSR_Vec3 &MSR_Vec3::operator /= ( float f )
{
	x /= f;
	y /= f;
	z /= f;
	return *this;
}

//
// Unary Operators
//

MSR_Vec3 MSR_Vec3::operator + () const
{
	return *this;
}

MSR_Vec3 MSR_Vec3::operator - () const
{
	return MSR_Vec3(-x, -y, -z);
}

//
// Binary Operators
//

MSR_Vec3 MSR_Vec3::operator + ( const MSR_Vec3& v ) const
{
	return MSR_Vec3(x + v.x, y + v.y, z + v.z);
}

MSR_Vec3 MSR_Vec3::operator - ( const MSR_Vec3& v ) const
{
	return MSR_Vec3(x - v.x, y - v.y, z - v.z);
}

MSR_Vec3 MSR_Vec3::operator * ( const MSR_Vec3& v ) const
{
	return MSR_Vec3(x * v.x, y * v.y, z * v.z);
}

MSR_Vec3 MSR_Vec3::operator / ( const MSR_Vec3& v ) const
{
	return MSR_Vec3(x / v.x, y / v.y, z / v.z);
}

MSR_Vec3 MSR_Vec3::operator + ( float f ) const
{
	return MSR_Vec3(x + f, y + f, z + f);
}

MSR_Vec3 MSR_Vec3::operator - ( float f ) const
{	
	return MSR_Vec3(x - f, y - f, z - f);
}

MSR_Vec3 MSR_Vec3::operator * ( float f ) const
{
	return MSR_Vec3(x * f, y * f, z * f);
}

MSR_Vec3 MSR_Vec3::operator / ( float f ) const
{	
	return MSR_Vec3(x / f, y / f, z / f);
}

MSR_Vec3 operator * ( float f, const MSR_Vec3& v )
{
	return MSR_Vec3(f * v.x, f * v.y, f * v.z);
}

bool MSR_Vec3::operator == ( const MSR_Vec3& v ) const
{
	return x == v.x && y == v.y && z == v.z;
}

bool MSR_Vec3::operator != ( const MSR_Vec3& v ) const
{
	return x != v.x || y != v.y || z != v.z;
}

//
// Functions
//

void MSR_Vec3::Set(float _x, float _y, float _z)
{
	x = _x;
	y = _y;
	z = _z;
}

float MSR_Vec3::Length() const
{
	return sqrtf(x * x + y * y + z * z);
}

float MSR_Vec3::LengthSqr() const
{
	return x * x + y * y + z * z;
}

float MSR_Vec3::Dot( const MSR_Vec3& v ) const
{
	return x * v.x + y * v.y + z * v.z;
}

void MSR_Vec3::Normalize()
{
	float f = 1.0f/sqrtf(x*x + y*y + z*z);
	x*=f; y*=f; z*=f;
}

void MSR_Vec3::Cross( const MSR_Vec3& v1, const MSR_Vec3& v2 )
{	
	x = v1.y * v2.z - v1.z * v2.y;
	y = v1.z * v2.x - v1.x * v2.z;
	z = v1.x * v2.y - v1.y * v2.x;
}

void MSR_Vec3::Lerp( const MSR_Vec3& v1, const MSR_Vec3& v2, float t )
{
	x = v1.x + t * (v2.x - v1.x);
	y = v1.y + t * (v2.y - v1.y);
	z = v1.z + t * (v2.z - v1.z);
}

void MSR_Vec3::Build( const MSR_Vec3& v1, const MSR_Vec3& v2 )
{
	x = v2.x - v1.x;
	y = v2.y - v1.y;
	z = v2.z - v1.z;
}

/////////////////////////////////////////////////////////////////////////////////
// MSR_Vec4
/////////////////////////////////////////////////////////////////////////////////

//
// Constructors
// 

MSR_Vec4::MSR_Vec4( const float *pf )
{
	x = pf[0];
	y = pf[1];
	z = pf[2];
	w = pf[3];
}

MSR_Vec4::MSR_Vec4( const MSR_Vec3& v, float _w )
{
	x = v.x;
	y = v.y;
	z = v.z;
	w = _w;
}
 
MSR_Vec4::MSR_Vec4( float fx, float fy, float fz, float fw )
{
	x = fx;
	y = fy;
	z = fz;
	w = fw;
}

//
// Casting
//
 
MSR_Vec4::operator float* ()
{
	return (float *)&x;
}

MSR_Vec4::operator const float* () const
{
	return (const float *)&x;
}

MSR_Vec4::operator const MSR_Vec3 &() const
{
	return reinterpret_cast<const MSR_Vec3 &>(*this);
}

//
// Assignment Operators
//

MSR_Vec4 &MSR_Vec4::operator += ( const MSR_Vec4& v )
{
	*this = *(MSR_Vec4*)&_mm_add_ps(*(__m128*)this, *(__m128*)&v);
	return *this;
}

MSR_Vec4 &MSR_Vec4::operator -= ( const MSR_Vec4& v )
{
	*this = *(MSR_Vec4*)&_mm_sub_ps(*(__m128*)this, *(__m128*)&v);
	return *this;
}

MSR_Vec4 &MSR_Vec4::operator += ( float f )
{
	*this = *(MSR_Vec4*)&_mm_add_ps(*(__m128*)this, _mm_set_ps1(f));
	return *this;
}

MSR_Vec4 &MSR_Vec4::operator -= ( float f )
{
	*this = *(MSR_Vec4*)&_mm_sub_ps(*(__m128*)this, _mm_set_ps1(f));
	return *this;
}

MSR_Vec4 &MSR_Vec4::operator *= ( float f )
{
	*this = *(MSR_Vec4*)&_mm_mul_ps(*(__m128*)this, _mm_set_ps1(f));
	return *this;
}

MSR_Vec4 &MSR_Vec4::operator /= ( float f )
{
	*this = *(MSR_Vec4*)&_mm_div_ps(*(__m128*)this, _mm_set_ps1(f));
	return *this;
}

//
// Unary Operators
//

MSR_Vec4 MSR_Vec4::operator + () const
{
	return *this;
}

MSR_Vec4 MSR_Vec4::operator - () const
{
	return MSR_Vec4(-x, -y, -z);
}

//
// Binary Operators
//

MSR_Vec4 MSR_Vec4::operator + ( const MSR_Vec4& v ) const
{
	return *(MSR_Vec4*)&_mm_add_ps(*(__m128*)this, *(__m128*)&v);
}

MSR_Vec4 MSR_Vec4::operator - ( const MSR_Vec4& v ) const
{
	return *(MSR_Vec4*)&_mm_sub_ps(*(__m128*)this, *(__m128*)&v);
}

MSR_Vec4 MSR_Vec4::operator * ( const MSR_Vec4& v ) const
{
	return *(MSR_Vec4*)&_mm_mul_ps(*(__m128*)this, *(__m128*)&v);
}

MSR_Vec4 MSR_Vec4::operator / ( const MSR_Vec4& v ) const
{
	return *(MSR_Vec4*)&_mm_div_ps(*(__m128*)this, *(__m128*)&v);
}

MSR_Vec4 MSR_Vec4::operator + ( float f ) const
{
	return *(MSR_Vec4*)&_mm_add_ps(*(__m128*)this, _mm_set_ps1(f));
}

MSR_Vec4 MSR_Vec4::operator - ( float f ) const
{	
	return *(MSR_Vec4*)&_mm_sub_ps(*(__m128*)this, _mm_set_ps1(f));
}

MSR_Vec4 MSR_Vec4::operator * ( float f ) const
{
	return *(MSR_Vec4*)&_mm_mul_ps(*(__m128*)this, _mm_set_ps1(f));
}

MSR_Vec4 MSR_Vec4::operator / ( float f ) const
{	
	return *(MSR_Vec4*)&_mm_div_ps(*(__m128*)this, _mm_set_ps1(f));
}

MSR_Vec4 operator * ( float f, const MSR_Vec4& v )
{
	return *(MSR_Vec4*)&_mm_mul_ps(*(__m128*)&v, _mm_set_ps1(f));
}

bool MSR_Vec4::operator == ( const MSR_Vec4& v ) const
{
	return x == v.x && y == v.y && z == v.z && w == v.w;
}

bool MSR_Vec4::operator != ( const MSR_Vec4& v ) const
{
	return x != v.x || y != v.y || z != v.z || w != v.w;
}

//
// Functions
//

float MSR_Vec4::Length() const
{
	int res = _mm_extract_ps( _mm_sqrt_ss(_mm_dp_ps(*(__m128*)this, *(__m128*)this, 0xFF)), 0 );
	return *(float*)&res;
}

float MSR_Vec4::LengthSqr() const
{
	int res = _mm_extract_ps( _mm_dp_ps(*(__m128*)this, *(__m128*)this, 0xFF), 0 );
	return *(float*)&res;
}

float MSR_Vec4::Dot( const MSR_Vec4& v ) const
{
	int res = _mm_extract_ps( _mm_dp_ps(*(__m128*)this, *(__m128*)&v, 0xFF), 0 );
	return *(float*)&res;
}

void MSR_Vec4::Normalize()
{
	__m128 &self = *(__m128*)this;
	*this = *(MSR_Vec4*)&_mm_mul_ps( self, _mm_rsqrt_ps(_mm_dp_ps(self, self, 0xFF)) );
}

void MSR_Vec4::Homogenize() 
{
	*this = *(MSR_Vec4*)&_mm_div_ps(*(__m128*)&x, _mm_set_ps1(w));
}

void MSR_Vec4::Cross( const MSR_Vec4& v1, const MSR_Vec4& v2 )
{	
	x = v1.y * v2.z - v1.z * v2.y;
	y = v1.z * v2.x - v1.x * v2.z;
	z = v1.x * v2.y - v1.y * v2.x;
	w = 1.0f;
}

void MSR_Vec4::Lerp( const MSR_Vec4& v1, const MSR_Vec4& v2, float t )
{
	x = v1.x + t * (v2.x - v1.x);
	y = v1.y + t * (v2.y - v1.y);
	z = v1.z + t * (v2.z - v1.z);
	z = v1.w + t * (v2.w - v1.w);
}

void MSR_Vec4::Build( const MSR_Vec4& v1, const MSR_Vec4& v2 )
{
	x = v2.x - v1.x;
	y = v2.y - v1.y;
	z = v2.z - v1.z;
	w = v2.w - v1.w;
}

/////////////////////////////////////////////////////////////////////////////////
// MSR_Mat4x4
/////////////////////////////////////////////////////////////////////////////////

const MSR_Mat4x4 MSR_Mat4x4::Identity(1.0f,0.0f,0.0f,0.0f,
							  0.0f,1.0f,0.0f,0.0f,
							  0.0f,0.0f,1.0f,0.0f,
							  0.0f,0.0f,0.0f,1.0f);

//
// Constructors
//
 
MSR_Mat4x4::MSR_Mat4x4( const float* pf )
{
	memcpy(&_11, pf, sizeof(MSR_Mat4x4));
}

MSR_Mat4x4::MSR_Mat4x4( const MSR_Mat4x4& m )
{
	memcpy(&_11, &m, sizeof(MSR_Mat4x4));
}

MSR_Mat4x4::MSR_Mat4x4( float f11, float f12, float f13, float f14,
			    float f21, float f22, float f23, float f24, 
				float f31, float f32, float f33, float f34, 
				float f41, float f42, float f43, float f44 )
{
    _11 = f11; _12 = f12; _13 = f13; _14 = f14;
    _21 = f21; _22 = f22; _23 = f23; _24 = f24;
    _31 = f31; _32 = f32; _33 = f33; _34 = f34;
    _41 = f41; _42 = f42; _43 = f43; _44 = f44;
}

//
// Access Grants
//

float &MSR_Mat4x4::operator () ( unsigned int row, unsigned int col )
{
	return m[4*row + col];
}

float MSR_Mat4x4::operator () ( unsigned int row, unsigned int col ) const
{
	return m[4*row + col];
}

//
// Casting Operators
// 

MSR_Mat4x4::operator float* ()
{
	return (float *) &_11;
}

MSR_Mat4x4::operator const float* () const
{
	return (const float *) &_11;
}

//
// Assignment Operators
//

MSR_Mat4x4& MSR_Mat4x4::operator *= ( const MSR_Mat4x4& m )
{
	*this = *this * m;
	return *this;
}

MSR_Mat4x4& MSR_Mat4x4::operator += ( const MSR_Mat4x4& m )
{
    _11 += m._11; _12 += m._12; _13 += m._13; _14 += m._14;
    _21 += m._21; _22 += m._22; _23 += m._23; _24 += m._24;
    _31 += m._31; _32 += m._32; _33 += m._33; _34 += m._34;
    _41 += m._41; _42 += m._42; _43 += m._43; _44 += m._44;
    return *this;
}

MSR_Mat4x4& MSR_Mat4x4::operator -= ( const MSR_Mat4x4& m )
{
    _11 -= m._11; _12 -= m._12; _13 -= m._13; _14 -= m._14;
    _21 -= m._21; _22 -= m._22; _23 -= m._23; _24 -= m._24;
    _31 -= m._31; _32 -= m._32; _33 -= m._33; _34 -= m._34;
    _41 -= m._41; _42 -= m._42; _43 -= m._43; _44 -= m._44;
    return *this;
}

MSR_Mat4x4& MSR_Mat4x4::operator += ( float f )
{
    _11 += f; _12 += f; _13 += f; _14 += f;
    _21 += f; _22 += f; _23 += f; _24 += f;
    _31 += f; _32 += f; _33 += f; _34 += f;
    _41 += f; _42 += f; _43 += f; _44 += f;
    return *this;
}

MSR_Mat4x4& MSR_Mat4x4::operator -= ( float f )
{
    _11 -= f; _12 -= f; _13 -= f; _14 -= f;
    _21 -= f; _22 -= f; _23 -= f; _24 -= f;
    _31 -= f; _32 -= f; _33 -= f; _34 -= f;
    _41 -= f; _42 -= f; _43 -= f; _44 -= f;
    return *this;
}

MSR_Mat4x4& MSR_Mat4x4::operator *= ( float f )
{
    _11 *= f; _12 *= f; _13 *= f; _14 *= f;
    _21 *= f; _22 *= f; _23 *= f; _24 *= f;
    _31 *= f; _32 *= f; _33 *= f; _34 *= f;
    _41 *= f; _42 *= f; _43 *= f; _44 *= f;
    return *this;
}

MSR_Mat4x4& MSR_Mat4x4::operator /= ( float f )
{
    _11 /= f; _12 /= f; _13 /= f; _14 /= f;
    _21 /= f; _22 /= f; _23 /= f; _24 /= f;
    _31 /= f; _32 /= f; _33 /= f; _34 /= f;
    _41 /= f; _42 /= f; _43 /= f; _44 /= f;
    return *this;
}

//
// Unary Operators
//

MSR_Mat4x4 MSR_Mat4x4::operator + () const
{
	return *this;
}

MSR_Mat4x4 MSR_Mat4x4::operator - () const
{
	return MSR_Mat4x4(-_11, -_12, -_13, -_14,
				  -_21, -_22, -_23, -_24,
				  -_31, -_32, -_33, -_34,
				  -_41, -_42, -_43, -_44);
}

//
// Binary Operators
//

MSR_Mat4x4 MSR_Mat4x4::operator + ( const MSR_Mat4x4& m ) const
{
    return MSR_Mat4x4(_11 + m._11, _12 + m._12, _13 + m._13, _14 + m._14,
                  _21 + m._21, _22 + m._22, _23 + m._23, _24 + m._24,
                  _31 + m._31, _32 + m._32, _33 + m._33, _34 + m._34,
                  _41 + m._41, _42 + m._42, _43 + m._43, _44 + m._44);
}

MSR_Mat4x4 MSR_Mat4x4::operator - ( const MSR_Mat4x4& m ) const
{
    return MSR_Mat4x4(_11 - m._11, _12 - m._12, _13 - m._13, _14 - m._14,
                  _21 - m._21, _22 - m._22, _23 - m._23, _24 - m._24,
                  _31 - m._31, _32 - m._32, _33 - m._33, _34 - m._34,
                  _41 - m._41, _42 - m._42, _43 - m._43, _44 - m._44);
}

MSR_Vec4 MSR_Mat4x4::operator * ( const MSR_Vec4& v ) const
{
	__m128 vResult;
	__m128 vx = _mm_set_ps1(v.x);
	__m128 vy = _mm_set_ps1(v.y);
	__m128 vz = _mm_set_ps1(v.z);
	__m128 vw = _mm_set_ps1(v.w);

	vResult = _mm_add_ps( _mm_mul_ps( vx, *(__m128*)&_11 ), 
			  _mm_add_ps( _mm_mul_ps( vy, *(__m128*)&_21 ),
			  _mm_add_ps( _mm_mul_ps( vz, *(__m128*)&_31 ), 
						  _mm_mul_ps( vw, *(__m128*)&_41 ) ) ) );

	return reinterpret_cast<MSR_Vec4&>(vResult);
}

MSR_Mat4x4 MSR_Mat4x4::operator + ( float f ) const
{
    return MSR_Mat4x4(_11 + f, _12 + f, _13 + f, _14 + f,
                  _21 + f, _22 + f, _23 + f, _24 + f,
                  _31 + f, _32 + f, _33 + f, _34 + f,
                  _41 + f, _42 + f, _43 + f, _44 + f);
}

MSR_Mat4x4 MSR_Mat4x4::operator - ( float f ) const
{
    return MSR_Mat4x4(_11 - f, _12 - f, _13 - f, _14 - f,
                  _21 - f, _22 - f, _23 - f, _24 - f,
                  _31 - f, _32 - f, _33 - f, _34 - f,
                  _41 - f, _42 - f, _43 - f, _44 - f);
}

MSR_Mat4x4 MSR_Mat4x4::operator * ( float f ) const
{
    return MSR_Mat4x4(_11 * f, _12 * f, _13 * f, _14 * f,
                  _21 * f, _22 * f, _23 * f, _24 * f,
                  _31 * f, _32 * f, _33 * f, _34 * f,
                  _41 * f, _42 * f, _43 * f, _44 * f);
}

MSR_Mat4x4 MSR_Mat4x4::operator / ( float f ) const
{
	float fInv = 1.0f / f;
    return MSR_Mat4x4(_11 * fInv, _12 * fInv, _13 * fInv, _14 * fInv,
                  _21 * fInv, _22 * fInv, _23 * fInv, _24 * fInv,
                  _31 * fInv, _32 * fInv, _33 * fInv, _34 * fInv,
                  _41 * fInv, _42 * fInv, _43 * fInv, _44 * fInv);
}

MSR_Mat4x4 operator * ( float f, const MSR_Mat4x4& m ) 
{
	return MSR_Mat4x4(f * m._11, f * m._12, f * m._13, f * m._14,
				  f * m._21, f * m._22, f * m._23, f * m._24,
				  f * m._31, f * m._32, f * m._33, f * m._34,
				  f * m._41, f * m._42, f * m._43, f * m._44);
}

void MSR_Mat4x4::operator = ( const MSR_Mat4x4& m ) const
{
	memcpy((void*)this, &m, sizeof(MSR_Mat4x4));
}

MSR_Mat4x4 MSR_Mat4x4::operator * ( const MSR_Mat4x4& m ) const
{
	MSR_Mat4x4 mResult;

	float *pA = (float*)this;
	float *pB = (float*)&m;
	float *pM = (float*)&mResult;

	memset(pM, 0, sizeof(MSR_Mat4x4));

	for( unsigned char i=0; i<4; i++ )
	{
		for( unsigned char j=0; j<4; j++ )
		{
			pM[4*i+j] += pA[4*i]   * pB[j];
            pM[4*i+j] += pA[4*i+1] * pB[4+j];
            pM[4*i+j] += pA[4*i+2] * pB[8+j];
            pM[4*i+j] += pA[4*i+3] * pB[12+j];
		}
	}

	return mResult;
}

//
// Functions
// 

bool MSR_Mat4x4::IsIdentity()
{
	return _11 == 1.0f && _12 == 0.0f && _13 == 0.0f && _14 == 0.0f &&
		   _21 == 0.0f && _22 == 1.0f && _23 == 0.0f && _24 == 0.0f &&
		   _31 == 0.0f && _32 == 0.0f && _33 == 1.0f && _34 == 0.0f &&
		   _41 == 0.0f && _42 == 0.0f && _43 == 0.0f && _44 == 1.0f;
}

void MSR_Mat4x4::SetIdentity()
{
	_12 = _13 = _14 = _21 = _23 = _24 = 
	_31 = _32 = _34 = _41 = _42 = _43 = 0.0f;

	_11 = _22 = _33 = _44 = 1.0f;
}

void MSR_Mat4x4::Translate( float x, float y, float z )
{
	_41 = x;
	_42 = y;
	_43 = z;
}

void MSR_Mat4x4::Scale( float x, float y, float z )
{
	_11 *= x;
	_22 *= y;
	_33 *= z;
}

void MSR_Mat4x4::SetRotateX( float x )
{
	float fCos = cosf(x);
	float fSin = sinf(x);

	_22 =  fCos;
	_23 =  -fSin;
	_32 =  fSin;
	_33 =  fCos;

	_11 = _44 = 1.0f;
    _12 = _13 = _14 = _21 = _24 = _31 = _34 = _41 = _42 = _43 = 0.0f;
}

void MSR_Mat4x4::SetRotateY( float y )
{
	float fCos = cosf(y);
	float fSin = sinf(y);

	_11 =  fCos;
	_13 =  fSin;
	_31 = -fSin;
	_33 =  fCos;

	_22 = _44 = 1.0f;
	_12 = _23 = _14 = _21 = _24 = _32 = _34 = _41 = _42 = _43 = 0.0f;
}

void MSR_Mat4x4::SetRotateZ( float z )
{
	float fCos = cosf(z);
	float fSin = sinf(z);

	_11 =  fCos;
	_12 = -fSin;
	_21 =  fSin;
	_22 =  fCos;

	_33 = _44 = 1.0f;
	_13 = _14 = _23 = _24 = _31 = _32 = _34 = _41 = _42 = _43 = 0.0f;
}
	
void MSR_Mat4x4::SetRotateAxis( const MSR_Vec3& _axis, float d )
{
	MSR_Vec3 axis = -_axis;

	float fCos = cosf(d);
	float fSin = sinf(d);
	float fSum = 1.0f - fCos;

	_11 = (axis.x * axis.x) * fSum + fCos;
	_12 = (axis.x * axis.y) * fSum - (axis.z * fSin);
	_13 = (axis.x * axis.z) * fSum + (axis.y * fSin);

	_21 = (axis.y * axis.x) * fSum + (axis.z * fSin);
	_22 = (axis.y * axis.y) * fSum + fCos;
	_23 = (axis.y * axis.z) * fSum - (axis.x * fSin);

	_31 = (axis.z * axis.x) * fSum - (axis.y * fSin);
	_32 = (axis.z * axis.y) * fSum + (axis.x * fSin);
	_33 = (axis.z * axis.z) * fSum + fCos;

	_14 = _24 = _34 = _41 = _42 = _43 = 0.0f;
	_44 = 1.0f;	
}

void MSR_Mat4x4::SetTransposeOf( const MSR_Mat4x4 &m )
{
	MSR_Mat4x4 temp = m;

	_11 = temp._11;
	_21 = temp._12;
	_31 = temp._13;
	_41 = temp._14;

	_12 = temp._21;
	_22 = temp._22;
	_32 = temp._23;
	_42 = temp._24;

	_13 = temp._31;
	_23 = temp._32;
	_33 = temp._33;
	_43 = temp._34;

	_14 = temp._41;
	_24 = temp._42;
	_34 = temp._43;
	_44 = temp._44;
}

float MSR_Mat4x4::GetDeterminant() const
{
	float a0 = m[ 0]*m[ 5] - m[ 1]*m[ 4];
    float a1 = m[ 0]*m[ 6] - m[ 2]*m[ 4];
    float a2 = m[ 0]*m[ 7] - m[ 3]*m[ 4];
    float a3 = m[ 1]*m[ 6] - m[ 2]*m[ 5];
    float a4 = m[ 1]*m[ 7] - m[ 3]*m[ 5];
    float a5 = m[ 2]*m[ 7] - m[ 3]*m[ 6];
    float b0 = m[ 8]*m[13] - m[ 9]*m[12];
    float b1 = m[ 8]*m[14] - m[10]*m[12];
    float b2 = m[ 8]*m[15] - m[11]*m[12];
    float b3 = m[ 9]*m[14] - m[10]*m[13];
    float b4 = m[ 9]*m[15] - m[11]*m[13];
    float b5 = m[10]*m[15] - m[11]*m[14];
    return a0*b5 - a1*b4 + a2*b3 + a3*b2 - a4*b1 + a5*b0;
}

int MSR_Mat4x4::SetInverseOf( const MSR_Mat4x4& mat )
{
	float a0 = mat.m[ 0]*mat.m[ 5] - mat.m[ 1]*mat.m[ 4];
    float a1 = mat.m[ 0]*mat.m[ 6] - mat.m[ 2]*mat.m[ 4];
    float a2 = mat.m[ 0]*mat.m[ 7] - mat.m[ 3]*mat.m[ 4];
    float a3 = mat.m[ 1]*mat.m[ 6] - mat.m[ 2]*mat.m[ 5];
    float a4 = mat.m[ 1]*mat.m[ 7] - mat.m[ 3]*mat.m[ 5];
    float a5 = mat.m[ 2]*mat.m[ 7] - mat.m[ 3]*mat.m[ 6];
    float b0 = mat.m[ 8]*mat.m[13] - mat.m[ 9]*mat.m[12];
    float b1 = mat.m[ 8]*mat.m[14] - mat.m[10]*mat.m[12];
    float b2 = mat.m[ 8]*mat.m[15] - mat.m[11]*mat.m[12];
    float b3 = mat.m[ 9]*mat.m[14] - mat.m[10]*mat.m[13];
    float b4 = mat.m[ 9]*mat.m[15] - mat.m[11]*mat.m[13];
    float b5 = mat.m[10]*mat.m[15] - mat.m[11]*mat.m[14];

    float fDet = a0*b5 - a1*b4 + a2*b3 + a3*b2 - a4*b1 + a5*b0;
	if( fDet > MSR_EPSILON ) 
	{
		m[ 0] =  mat.m[ 5]*b5 - mat.m[ 6]*b4 + mat.m[ 7]*b3;
        m[ 4] = -mat.m[ 4]*b5 + mat.m[ 6]*b2 - mat.m[ 7]*b1;
        m[ 8] =  mat.m[ 4]*b4 - mat.m[ 5]*b2 + mat.m[ 7]*b0;
        m[12] = -mat.m[ 4]*b3 + mat.m[ 5]*b1 - mat.m[ 6]*b0;
        m[ 1] = -mat.m[ 1]*b5 + mat.m[ 2]*b4 - mat.m[ 3]*b3;
        m[ 5] =  mat.m[ 0]*b5 - mat.m[ 2]*b2 + mat.m[ 3]*b1;
        m[ 9] = -mat.m[ 0]*b4 + mat.m[ 1]*b2 - mat.m[ 3]*b0;
        m[13] =  mat.m[ 0]*b3 - mat.m[ 1]*b1 + mat.m[ 2]*b0;
        m[ 2] =  mat.m[13]*a5 - mat.m[14]*a4 + mat.m[15]*a3;
        m[ 6] = -mat.m[12]*a5 + mat.m[14]*a2 - mat.m[15]*a1;
        m[10] =  mat.m[12]*a4 - mat.m[13]*a2 + mat.m[15]*a0;
        m[14] = -mat.m[12]*a3 + mat.m[13]*a1 - mat.m[14]*a0;
        m[ 3] = -mat.m[ 9]*a5 + mat.m[10]*a4 - mat.m[11]*a3;
        m[ 7] =  mat.m[ 8]*a5 - mat.m[10]*a2 + mat.m[11]*a1;
        m[11] = -mat.m[ 8]*a4 + mat.m[ 9]*a2 - mat.m[11]*a0;
        m[15] =  mat.m[ 8]*a3 - mat.m[ 9]*a1 + mat.m[10]*a0;

		fDet = 1.0f / fDet;
		m[ 0] *= fDet;  
		m[ 1] *= fDet;  
		m[ 2] *= fDet;  
		m[ 3] *= fDet;
		m[ 4] *= fDet;  
		m[ 5] *= fDet;  
		m[ 6] *= fDet;  
		m[ 7] *= fDet;
		m[ 8] *= fDet;  
		m[ 9] *= fDet;  
		m[10] *= fDet;  
		m[11] *= fDet;
		m[12] *= fDet;  
		m[13] *= fDet;  
		m[14] *= fDet;  
		m[15] *= fDet;

		return 1;
	}

	return 0;
}

void MSR_Mat4x4::SetLookAtLH( const MSR_Vec3& eye, const MSR_Vec3& at, const MSR_Vec3& up )
{
	MSR_Vec3 zaxis = at - eye;
	zaxis.Normalize();

	MSR_Vec3 xaxis;
	xaxis.Cross(up, zaxis);
	xaxis.Normalize();
	
	MSR_Vec3 yaxis;
	yaxis.Cross(zaxis, xaxis);
	yaxis.Normalize();
	
	_11 = xaxis.x; _12 = yaxis.x; _13 = zaxis.x;
	_21 = xaxis.y; _22 = yaxis.y; _23 = zaxis.y;
	_31 = xaxis.z; _32 = yaxis.z; _33 = zaxis.z;
	
	_41 = -(xaxis.Dot(eye));
	_42 = -(yaxis.Dot(eye));
	_43 = -(zaxis.Dot(eye));

	_14 = 0.0f; 
	_24 = 0.0f; 
	_34 = 0.0f; 
	_44 = 1.0f;
}

void MSR_Mat4x4::SetLookAtRH( const MSR_Vec3& eye, const MSR_Vec3& at, const MSR_Vec3& up )
{
	MSR_Vec3 zaxis = eye - at;
	zaxis.Normalize();

	MSR_Vec3 xaxis;
	xaxis.Cross(up, zaxis);
	xaxis.Normalize();
	
	MSR_Vec3 yaxis;
	yaxis.Cross(zaxis, xaxis);
	yaxis.Normalize();
	
	_11 = xaxis.x; _12 = yaxis.x; _13 = zaxis.x;
	_21 = xaxis.y; _22 = yaxis.y; _23 = zaxis.y;
	_31 = xaxis.z; _32 = yaxis.z; _33 = zaxis.z;
	
	_41 = -(xaxis.Dot(eye));
	_42 = -(yaxis.Dot(eye));
	_43 = -(zaxis.Dot(eye));

	_14 = 0.0f; 
	_24 = 0.0f; 
	_34 = 0.0f; 
	_44 = 1.0f;
}

void MSR_Mat4x4::SetPerspectiveFovLH(float fov, float aspect, float zn, float zf) 
{
	float yscale = 1.0f / tan(fov*0.5f);
	float xscale = yscale / aspect;
	float oneOverZDelta = 1.0f/(zf - zn);

	_11 = xscale;
	_22 = yscale;
	_33 = zf*oneOverZDelta;
	_43 = (-zn*zf)*oneOverZDelta;
	_34 = 1.0f;

	_12 = _13 = _14 = 0.0f;
	_21 = _23 = _24 = 0.0f;
	_31 = _32 = 0.0f;
	_41 = _42 = _44 = 0.0f;
}

void MSR_Mat4x4::SetPerspectiveFovRH(float fov, float aspect, float zn, float zf) 
{
	float yscale = 1.0f / tan(fov*0.5f);
	float xscale = yscale / aspect;
	float oneOverZDelta = 1.0f/(zn - zf);

	_11 = xscale;
	_22 = yscale;
	_33 = zf*oneOverZDelta;
	_43 = (zn*zf)*oneOverZDelta;
	_34 = -1.0f;

	_12 = _13 = _14 = 0.0f;
	_21 = _23 = _24 = 0.0f;
	_31 = _32 = 0.0f;
	_41 = _42 = _44 = 0.0f;
}

void MSR_Mat4x4::SetOrthoLH( float w, float h, float zn, float zf )
{
	_11 = 2.0f / w;
	_22 = 2.0f / h;
	_33 = 1.0f / (zf - zn);
	_43 = zn / (zn - zf);
	_44 = 1.0f;

	_12 = _13 = _14 = _21 = _23 = _24 =
	_31 = _32 = _34 = _41 = _42 = 0.0f;
}

void MSR_Mat4x4::SetOrthoRH( float w, float h, float zn, float zf )
{
	_11 = 2.0f / w;
	_22 = 2.0f / h;
	_33 = 1.0f / (zn - zf);
	_43 = zn / (zn - zf);
	_44 = 1.0f;

	_12 = _13 = _14 = _21 = _23 = _24 =
	_31 = _32 = _34 = _41 = _42 = 0.0f;
}

void MSR_Mat4x4::SetOrthoOffCenterLH( float l, float r, float b, float t, float zn, float zf )
{
	_11 = 2.0f / (r-l);
	_22 = 2.0f / (t-b);
	_33 = 1.0f / (zf - zn);
	_41 = -1.0f - 2.0f * l / (r - l);
	_42 =  1.0f + 2.0f * t / (b - t);
	_43 = zn / (zn - zf);
	_44 = 1.0f;

	_12 = _13 = _14 = _21 = _23 = _24 =
	_31 = _32 = _34 = 0.0f;
}

void MSR_Mat4x4::SetOrthoOffCenterRH( float l, float r, float b, float t, float zn, float zf )
{
	_11 = 2.0f / (r-l);
	_22 = 2.0f / (t-b);
	_33 = 1.0f / (zn - zf);
	_41 = -1.0f - 2.0f * l / (r - l);
	_42 =  1.0f + 2.0f * t / (b - t);
	_43 = zn / (zn - zf);
	_44 = 1.0f;

	_12 = _13 = _14 = _21 = _23 = _24 =
	_31 = _32 = _34 = 0.0f;
}

MSR_Vec4 MSR_Mat4x4::TransformNormal( const MSR_Vec4& v ) const
{
	MSR_Vec4 vResult;

	__m128 vx = _mm_set_ps1(v.x);
	__m128 vy = _mm_set_ps1(v.y);
	__m128 vz = _mm_set_ps1(v.z);

	vResult = *(MSR_Vec4*)&_mm_add_ps( _mm_mul_ps( vx, *(__m128*)&_11 ), 
						   _mm_add_ps( _mm_mul_ps( vy, *(__m128*)&_21 ), 
									   _mm_mul_ps( vz, *(__m128*)&_31 ) ) );

	vResult.w = 1.0f;
	return vResult;
}

/////////////////////////////////////////////////////////////////////////////////
// MSR_Mat3x3
/////////////////////////////////////////////////////////////////////////////////

const MSR_Mat3x3 MSR_Mat3x3::Identity(1.0f,0.0f,0.0f,
							  0.0f,1.0f,0.0f,
							  0.0f,0.0f,1.0f);

//
// Constructors
//

MSR_Mat3x3::MSR_Mat3x3( const float *pf )
{
	memcpy(&_11, pf, sizeof(MSR_Mat3x3));
}

MSR_Mat3x3::MSR_Mat3x3( const MSR_Mat3x3& m )
{
	memcpy(&_11, &m, sizeof(MSR_Mat3x3));
}

MSR_Mat3x3::MSR_Mat3x3( float f11, float f12, float f13,
			    float f21, float f22, float f23, 
				float f31, float f32, float f33 )
{
    _11 = f11; _12 = f12; _13 = f13;
    _21 = f21; _22 = f22; _23 = f23;
    _31 = f31; _32 = f32; _33 = f33;
}

//
// Access Grants
//

float &MSR_Mat3x3::operator () ( unsigned int row, unsigned int col )
{
	return m[3*row + col];
}

float MSR_Mat3x3::operator () ( unsigned int row, unsigned int col ) const
{
	return m[3*row + col];
}

//
// Casting Operators
// 

MSR_Mat3x3::operator float* ()
{
	return (float *) &_11;
}

MSR_Mat3x3::operator const float* () const
{
	return (const float *) &_11;
}

//
// Assignment Operators
//

MSR_Mat3x3& MSR_Mat3x3::operator *= ( const MSR_Mat3x3& m )
{
	*this = *this * m;
	return *this;
}

MSR_Mat3x3& MSR_Mat3x3::operator += ( const MSR_Mat3x3& m )
{
    _11 += m._11; _12 += m._12; _13 += m._13;
    _21 += m._21; _22 += m._22; _23 += m._23;
    _31 += m._31; _32 += m._32; _33 += m._33;
    return *this;
}

MSR_Mat3x3& MSR_Mat3x3::operator -= ( const MSR_Mat3x3& m )
{
    _11 -= m._11; _12 -= m._12; _13 -= m._13;
    _21 -= m._21; _22 -= m._22; _23 -= m._23;
    _31 -= m._31; _32 -= m._32; _33 -= m._33;
    return *this;
}

MSR_Mat3x3& MSR_Mat3x3::operator += ( float f )
{
    _11 += f; _12 += f; _13 += f;
    _21 += f; _22 += f; _23 += f;
    _31 += f; _32 += f; _33 += f;
    return *this;
}

MSR_Mat3x3& MSR_Mat3x3::operator -= ( float f )
{
    _11 -= f; _12 -= f; _13 -= f;
    _21 -= f; _22 -= f; _23 -= f;
    _31 -= f; _32 -= f; _33 -= f;
    return *this;
}

MSR_Mat3x3& MSR_Mat3x3::operator *= ( float f )
{
    _11 *= f; _12 *= f; _13 *= f;
    _21 *= f; _22 *= f; _23 *= f;
    _31 *= f; _32 *= f; _33 *= f;
    return *this;
}

MSR_Mat3x3& MSR_Mat3x3::operator /= ( float f )
{
    _11 /= f; _12 /= f; _13 /= f;
    _21 /= f; _22 /= f; _23 /= f;
    _31 /= f; _32 /= f; _33 /= f;
    return *this;
}

//
// Unary Operators
//

MSR_Mat3x3 MSR_Mat3x3::operator + () const
{
	return *this;
}

MSR_Mat3x3 MSR_Mat3x3::operator - () const
{
	return MSR_Mat3x3(-_11, -_12, -_13,
				  -_21, -_22, -_23,
				  -_31, -_32, -_33);
}

//
// Binary Operators
//

MSR_Mat3x3 MSR_Mat3x3::operator + ( const MSR_Mat3x3& m ) const
{
    return MSR_Mat3x3(_11 + m._11, _12 + m._12, _13 + m._13,
                  _21 + m._21, _22 + m._22, _23 + m._23,
                  _31 + m._31, _32 + m._32, _33 + m._33);
}

MSR_Mat3x3 MSR_Mat3x3::operator - ( const MSR_Mat3x3& m ) const
{
    return MSR_Mat3x3(_11 - m._11, _12 - m._12, _13 - m._13,
                  _21 - m._21, _22 - m._22, _23 - m._23,
                  _31 - m._31, _32 - m._32, _33 - m._33);
}

MSR_Vec3 MSR_Mat3x3::operator * ( const MSR_Vec3& v ) const
{
	MSR_Vec3 vResult;
	vResult.x = v.x*_11 + v.y*_21 + v.z*_31;
	vResult.y = v.x*_12 + v.y*_22 + v.z*_32;
	vResult.z = v.x*_13 + v.y*_23 + v.z*_33;
	return vResult;
}

MSR_Mat3x3 MSR_Mat3x3::operator + ( float f ) const
{
    return MSR_Mat3x3(_11 + f, _12 + f, _13 + f,
                  _21 + f, _22 + f, _23 + f,
                  _31 + f, _32 + f, _33 + f);
}

MSR_Mat3x3 MSR_Mat3x3::operator - ( float f ) const
{
    return MSR_Mat3x3(_11 - f, _12 - f, _13 - f,
                  _21 - f, _22 - f, _23 - f,
                  _31 - f, _32 - f, _33 - f);
}

MSR_Mat3x3 MSR_Mat3x3::operator * ( float f ) const
{
    return MSR_Mat3x3(_11 * f, _12 * f, _13 * f,
                  _21 * f, _22 * f, _23 * f,
                  _31 * f, _32 * f, _33 * f);
}

MSR_Mat3x3 MSR_Mat3x3::operator / ( float f ) const
{
	float fInv = 1.0f / f;
    return MSR_Mat3x3(_11 * fInv, _12 * fInv, _13 * fInv,
                  _21 * fInv, _22 * fInv, _23 * fInv,
                  _31 * fInv, _32 * fInv, _33 * fInv);
}

MSR_Mat3x3 operator * ( float f, const MSR_Mat3x3& m ) 
{
	return MSR_Mat3x3(f * m._11, f * m._12, f * m._13,
				  f * m._21, f * m._22, f * m._23,
				  f * m._31, f * m._32, f * m._33);
}

void MSR_Mat3x3::operator = ( const MSR_Mat3x3& m ) const
{
	memcpy((void*)this, &m, sizeof(MSR_Mat3x3));
}

MSR_Mat3x3 MSR_Mat3x3::operator * ( const MSR_Mat3x3& m ) const
{
	MSR_Mat3x3 mResult;

	float *pA = (float*)this;
	float *pB = (float*)&m;
	float *pM = (float*)&mResult;

	memset(pM, 0, sizeof(MSR_Mat3x3));

	for( unsigned char i=0; i<3; i++ )
	{
		for( unsigned char j=0; j<3; j++ )
		{
			pM[3*i+j] += pA[3*i]   * pB[j];
            pM[3*i+j] += pA[3*i+1] * pB[3+j];
            pM[3*i+j] += pA[3*i+2] * pB[6+j];
		}
	}

	return mResult;
}

//
// Functions
//

bool MSR_Mat3x3::IsIdentity()
{
	return _11 == 1.0f && _12 == 0.0f && _13 == 0.0f &&
		   _21 == 0.0f && _22 == 1.0f && _23 == 0.0f &&
		   _31 == 0.0f && _32 == 0.0f && _33 == 1.0f;
}

void MSR_Mat3x3::SetIdentity()
{
	_12 = _13 = _21 = _23 = 
	_31 = _32 = 0.0f;

	_11 = _22 = _33 = 1.0f;
}

void MSR_Mat3x3::Translate( float x, float y )
{
	_31 = x;
	_32 = y;
}

void MSR_Mat3x3::Scale( float x, float y )
{
	_11 *= x;
	_22 *= y;
}

void MSR_Mat3x3::SetRotate( float rot )
{
	float fCos = cosf(rot);
	float fSin = sinf(rot);

	_11 =  fCos;
	_12 = -fSin;
	_21 =  fSin;
	_22 =  fCos;

	_33 = 1.0f;
	_13 = _23 = _31 = _32 = 0.0f;
}

void MSR_Mat3x3::SetTransposeOf( const MSR_Mat3x3 &m )
{
	MSR_Mat3x3 temp = m;

	_11 = temp._11;
	_21 = temp._12;
	_31 = temp._13;

	_12 = temp._21;
	_22 = temp._22;
	_32 = temp._23;

	_13 = temp._31;
	_23 = temp._32;
	_33 = temp._33;
}

int MSR_Mat3x3::SetInverseOf( const MSR_Mat3x3& m )
{
	float det = GetDeterminant();
	if( det < MSR_EPSILON ) return 0;

	float inv_det = 1.0f/det;
	_11 =	   (_22*_33 - _32*_23) * inv_det;
	_21 = -1 * (_21*_33 - _23*_31) * inv_det;
	_31 =	   (_21*_32 - _31*_22) * inv_det;
	_12 = -1 * (_12*_33 - _13*_32) * inv_det;
	_22 =	   (_11*_33 - _13*_31) * inv_det;
	_32 = -1 * (_11*_32 - _31*_12) * inv_det;
	_13 =	   (_12*_23 - _13*_22) * inv_det;
	_23 = -1 * (_11*_23 - _21*_13) * inv_det;
	_33 =	   (_11*_22 - _21*_12) * inv_det;

	return 1;
}

MSR_Vec2 MSR_Mat3x3::Transform( const MSR_Vec2& v )
{
	MSR_Vec2 result;
	result.x = v.x*_11 + v.y*_21 + _31;
	result.y = v.x*_12 + v.y*_22 + _32;
	return result;
}

float MSR_Mat3x3::GetDeterminant() const
{
	return _11 * (_33*_22 - _32*_23) - 
		   _21 * (_33*_12 - _32*_13) +
		   _31 * (_23*_12 - _22*_13);
}

/////////////////////////////////////////////////////////////////////////////////
// MSR_Plane
/////////////////////////////////////////////////////////////////////////////////

//
// Constructors
//

MSR_Plane::MSR_Plane( float fa, float fb, float fc, float fd )
{
	a = fa;
	b = fb;
	c = fc;
	d = fd;
}

MSR_Plane::MSR_Plane( const float *pf )
{
	a = pf[0];
	b = pf[1];
	c = pf[2];
	d = pf[3];
}

//
// Methods
//

float MSR_Plane::DotCoord(const MSR_Vec3 &v) const
{
	return a*v.x + b*v.y + c*v.z + d;
}

void MSR_Plane::Normalize()
{
	float mag = sqrtf(a*a + b*b + c*c);
	a /= mag;
	b /= mag;
	c /= mag;
	d /= mag;
}

//
// Casting operators
//

MSR_Plane::operator float* ()
{
	return (float *) &a;
}

MSR_Plane::operator const float* () const
{
	return (const float *) &a;
}

//
// Unary Operators
//

MSR_Plane MSR_Plane::operator + () const
{
	return *this;
}

MSR_Plane MSR_Plane::operator - () const
{
	return MSR_Plane( -a, -b, -c, -d );
}

//
// Binary Operators
//

bool MSR_Plane::operator == ( const MSR_Plane& _p ) const
{
	return a == _p.a && b == _p.b && c == _p.c && d == _p.d;
}

bool MSR_Plane::operator != ( const MSR_Plane& _p ) const
{
	return a != _p.a || b != _p.b || c != _p.c || d != _p.d;
}