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

#include "MSR_SSEMath.h"

/////////////////////////////////////////////////////////////////////////////////
// MSR_SSEFloat
/////////////////////////////////////////////////////////////////////////////////

MSR_SSEFloat::MSR_SSEFloat(const __m128 &ps) : f(ps)
{
}

MSR_SSEFloat::MSR_SSEFloat(float ss)
{
	f = _mm_set1_ps(ss);
}

MSR_SSEFloat::MSR_SSEFloat(float A, float B, float C, float D)
{
	f = _mm_set_ps(A, B, C, D);
}

//
// Assignment operators
//

MSR_SSEFloat MSR_SSEFloat::operator += ( const MSR_SSEFloat& _f )
{
	return MSR_SSEFloat(f = _mm_add_ps(f, _f.f));
}

MSR_SSEFloat MSR_SSEFloat::operator -= ( const MSR_SSEFloat& _f )
{
	return MSR_SSEFloat(f = _mm_sub_ps(f, _f.f));
}

MSR_SSEFloat MSR_SSEFloat::operator *= ( const MSR_SSEFloat& _f )
{
	return MSR_SSEFloat(f = _mm_mul_ps(f, _f.f));
}

MSR_SSEFloat MSR_SSEFloat::operator /= ( const MSR_SSEFloat& _f )
{
	return MSR_SSEFloat(f = _mm_div_ps(f, _f.f));
}

MSR_SSEFloat MSR_SSEFloat::operator += ( float _f )
{
	return MSR_SSEFloat(f = _mm_add_ps(f, *MSR_SSEFloat(_f)));
}

MSR_SSEFloat MSR_SSEFloat::operator -= ( float _f )
{
	return MSR_SSEFloat(f = _mm_sub_ps(f, *MSR_SSEFloat(_f)));
}

MSR_SSEFloat MSR_SSEFloat::operator *= ( float _f )
{
	return MSR_SSEFloat(f = _mm_mul_ps(f, *MSR_SSEFloat(_f)));
}

MSR_SSEFloat MSR_SSEFloat::operator /= ( float _f )
{
	return MSR_SSEFloat(f = _mm_div_ps(f, *MSR_SSEFloat(_f)));
}

//
// Unary Operators
//

MSR_SSEFloat MSR_SSEFloat::operator + () const
{
	return *this;
}

MSR_SSEFloat MSR_SSEFloat::operator - () const
{
	return _mm_xor_ps( f, _mm_set_ps1(0x80000000) );
}

//
// Casting Operators
//

float4 &MSR_SSEFloat::operator * ()
{
	return f;
}

MSR_SSEFloat::operator float4* ()
{
	return (float4*)&f;
}

MSR_SSEFloat::operator const float4* () const
{
	return (const float4*)&f;
}

//
// Binary Operators
//

MSR_SSEFloat MSR_SSEFloat::operator + ( const MSR_SSEFloat& _f ) const
{
	return MSR_SSEFloat( _mm_add_ps( f, _f.f ) );
}

MSR_SSEFloat MSR_SSEFloat::operator - ( const MSR_SSEFloat& _f ) const
{
	return MSR_SSEFloat( _mm_sub_ps( f, _f.f ) );
}

MSR_SSEFloat MSR_SSEFloat::operator * ( const MSR_SSEFloat& _f ) const
{
	return MSR_SSEFloat( _mm_mul_ps( f, _f.f ) );
}

MSR_SSEFloat MSR_SSEFloat::operator / ( const MSR_SSEFloat& _f ) const
{
	return MSR_SSEFloat( _mm_div_ps( f, _f.f ) );
}

MSR_SSEFloat MSR_SSEFloat::operator + ( float _f ) const
{
	return MSR_SSEFloat( _mm_add_ps( f, *MSR_SSEFloat(_f) ) );
}

MSR_SSEFloat MSR_SSEFloat::operator - ( float _f ) const
{
	return MSR_SSEFloat( _mm_sub_ps( f, *MSR_SSEFloat(_f) ) );
}

MSR_SSEFloat MSR_SSEFloat::operator * ( float _f ) const
{
	return MSR_SSEFloat( _mm_mul_ps( f, *MSR_SSEFloat(_f) ) );
}

MSR_SSEFloat MSR_SSEFloat::operator / ( float _f ) const
{
	return MSR_SSEFloat( _mm_div_ps( f, *MSR_SSEFloat(_f) ) );
}

/////////////////////////////////////////////////////////////////////////////////
// MSR_SSEVec2
/////////////////////////////////////////////////////////////////////////////////

//
// Constructors
//

MSR_SSEVec2::MSR_SSEVec2(const MSR_SSEFloat &_x, const MSR_SSEFloat &_y)
{
	x = _x;
	y = _y;
}

//
// Methods
//

MSR_SSEFloat MSR_SSEVec2::Length() const
{
	return MSR_SSESqrt( x*x + y*y );
}

MSR_SSEFloat MSR_SSEVec2::LengthSqr() const
{
	return x*x + y*y;
}

MSR_SSEFloat MSR_SSEVec2::Dot( const MSR_SSEVec2 &v ) const
{
	return x*v.x + y*v.y;
}

void MSR_SSEVec2::Lerp( const MSR_SSEVec2& v1, const MSR_SSEVec2& v2, const MSR_SSEFloat& t )
{
	x = v1.x + t * (v2.x - v1.x);
	y = v1.y + t * (v2.y - v1.y);
}

void MSR_SSEVec2::Normalize()
{
	MSR_SSEFloat f = MSR_SSERSqrt( LengthSqr() );
	x = x * f;
	y = y * f;
}

void MSR_SSEVec2::Build( const MSR_SSEVec2& v1, const MSR_SSEVec2& v2 )
{
	x = v2.x - v1.x;
	y = v2.y - v1.y;
}

//
// Assignment Operators
//

MSR_SSEVec2 &MSR_SSEVec2::operator += ( const MSR_SSEVec2& v )
{
	x = x + v.x;
	y = y + v.y;
	return *this;
}

MSR_SSEVec2 &MSR_SSEVec2::operator -= ( const MSR_SSEVec2& v )
{
	x = x - v.x;
	y = y - v.y;
	return *this;
}

MSR_SSEVec2 &MSR_SSEVec2::operator *= ( const MSR_SSEVec2& v )
{
	x = x * v.x;
	y = y * v.y;
	return *this;
}

MSR_SSEVec2 &MSR_SSEVec2::operator /= ( const MSR_SSEVec2& v )
{
	x = x / v.x;
	y = y / v.y;
	return *this;
}

MSR_SSEVec2 &MSR_SSEVec2::operator += ( const MSR_SSEFloat& f )
{
	x = x + f;
	y = y + f;
	return *this;
}

MSR_SSEVec2 &MSR_SSEVec2::operator -= ( const MSR_SSEFloat& f )
{
	x = x - f;
	y = y - f;
	return *this;
}

MSR_SSEVec2 &MSR_SSEVec2::operator *= ( const MSR_SSEFloat& f )
{
	x = x * f;
	y = y * f;
	return *this;
}

MSR_SSEVec2 &MSR_SSEVec2::operator /= ( const MSR_SSEFloat& f )
{
	x = x / f;
	y = y / f;
	return *this;
}

//
// Unary Operators
//

MSR_SSEVec2 MSR_SSEVec2::operator + () const
{
	return *this;
}

MSR_SSEVec2 MSR_SSEVec2::operator - () const
{
	return MSR_SSEVec2(-x, -y);
}

//
// Casting Operators
//

MSR_SSEVec2::operator MSR_SSEFloat* ()
{
	return (MSR_SSEFloat*)&x;
}

MSR_SSEVec2::operator const MSR_SSEFloat* () const
{
	return (const MSR_SSEFloat*)&x;
}

//
// Binary Operators 
//

MSR_SSEVec2 MSR_SSEVec2::operator + ( const MSR_SSEVec2& p ) const
{
	return MSR_SSEVec2(x + p.x, y + p.y);
}

MSR_SSEVec2 MSR_SSEVec2::operator - ( const MSR_SSEVec2& p ) const
{
	return MSR_SSEVec2(x - p.x, y - p.y);
}

MSR_SSEVec2 MSR_SSEVec2::operator * ( const MSR_SSEVec2& p ) const
{
	return MSR_SSEVec2(x * p.x, y * p.y);
}

MSR_SSEVec2 MSR_SSEVec2::operator / ( const MSR_SSEVec2& p ) const
{
	return MSR_SSEVec2(x / p.x, y / p.y);
}

MSR_SSEVec2 MSR_SSEVec2::operator + ( const MSR_SSEFloat& f ) const
{	
	return MSR_SSEVec2(x + f, y + f);
}

MSR_SSEVec2 MSR_SSEVec2::operator - ( const MSR_SSEFloat& f ) const
{	
	return MSR_SSEVec2(x - f, y - f);
}

MSR_SSEVec2 MSR_SSEVec2::operator * ( const MSR_SSEFloat& f ) const
{
	return MSR_SSEVec2(x * f, y * f);
}

MSR_SSEVec2 MSR_SSEVec2::operator / ( const MSR_SSEFloat& f ) const
{	
	return MSR_SSEVec2(x / f, y / f);
}

MSR_SSEVec2 operator * ( const MSR_SSEFloat& f, const MSR_SSEVec2& p )
{
	return MSR_SSEVec2(f * p.x, f * p.y);
}

/////////////////////////////////////////////////////////////////////////////////
// MSR_SSEVec3
/////////////////////////////////////////////////////////////////////////////////

//
// Constructors
//

MSR_SSEVec3::MSR_SSEVec3(const MSR_SSEFloat &_x, const MSR_SSEFloat &_y, const MSR_SSEFloat &_z)
{
	x = _x;
	y = _y;
	z = _z;
}

MSR_SSEFloat MSR_SSEVec3::Length() const
{
	return MSR_SSESqrt( x*x + y*y + z*z );
}

MSR_SSEFloat MSR_SSEVec3::LengthSqr() const
{
	return x*x + y*y + z*z;
}

MSR_SSEFloat MSR_SSEVec3::Dot( const MSR_SSEVec3 &v ) const
{
	return x*v.x + y*v.y + z*v.z;
}

void MSR_SSEVec3::Lerp( const MSR_SSEVec3& v1, const MSR_SSEVec3& v2, const MSR_SSEFloat& t )
{
	x = v1.x + t * (v2.x - v1.x);
	y = v1.y + t * (v2.y - v1.y);
	z = v1.z + t * (v2.z - v1.z);
}

void MSR_SSEVec3::Normalize()
{
	MSR_SSEFloat f = MSR_SSERSqrt( LengthSqr() );
	x *= f;
	y *= f;
	z *= f;
}

void MSR_SSEVec3::Build( const MSR_SSEVec3& v1, const MSR_SSEVec3& v2 )
{
	x = v2.x - v1.x;
	y = v2.y - v1.y;
	z = v2.z - v1.z;
}

//
// Assignment Operators
//

MSR_SSEVec3 &MSR_SSEVec3::operator += ( const MSR_SSEVec3& v )
{
	x = x + v.x;
	y = y + v.y;
	z = z + v.z;
	return *this;
}

MSR_SSEVec3 &MSR_SSEVec3::operator -= ( const MSR_SSEVec3& v )
{
	x = x - v.x;
	y = y - v.y;
	z = z - v.z;
	return *this;
}

MSR_SSEVec3 &MSR_SSEVec3::operator *= ( const MSR_SSEVec3& v )
{
	x = x * v.x;
	y = y * v.y;
	z = z * v.z;
	return *this;
}

MSR_SSEVec3 &MSR_SSEVec3::operator /= ( const MSR_SSEVec3& v )
{
	x = x / v.x;
	y = y / v.y;
	z = z / v.z;
	return *this;
}

MSR_SSEVec3 &MSR_SSEVec3::operator += ( const MSR_SSEFloat& f )
{
	x = x + f;
	y = y + f;
	z = z + f;
	return *this;
}

MSR_SSEVec3 &MSR_SSEVec3::operator -= ( const MSR_SSEFloat& f )
{
	x = x - f;
	y = y - f;
	z = z - f;
	return *this;
}

MSR_SSEVec3 &MSR_SSEVec3::operator *= ( const MSR_SSEFloat& f )
{
	x = x * f;
	y = y * f;
	z = z * f;
	return *this;
}

MSR_SSEVec3 &MSR_SSEVec3::operator /= ( const MSR_SSEFloat& f )
{
	x = x / f;
	y = y / f;
	z = z / f;
	return *this;
}

//
// Unary Operators
//

MSR_SSEVec3 MSR_SSEVec3::operator + () const
{
	return *this;
}

MSR_SSEVec3 MSR_SSEVec3::operator - () const
{
	return MSR_SSEVec3(-x, -y, -z);
}

//
// Casting Operators
//

MSR_SSEVec3::operator MSR_SSEFloat* ()
{
	return (MSR_SSEFloat*)&x;
}

MSR_SSEVec3::operator const MSR_SSEFloat* () const
{
	return (const MSR_SSEFloat*)&x;
}

//
// Binary Operators 
//

MSR_SSEVec3 MSR_SSEVec3::operator + ( const MSR_SSEVec3& p ) const
{
	return MSR_SSEVec3(x + p.x, y + p.y, z + p.z);
}

MSR_SSEVec3 MSR_SSEVec3::operator - ( const MSR_SSEVec3& p ) const
{
	return MSR_SSEVec3(x - p.x, y - p.y, z - p.z);
}

MSR_SSEVec3 MSR_SSEVec3::operator * ( const MSR_SSEVec3& p ) const
{
	return MSR_SSEVec3(x * p.x, y * p.y, z * p.z);
}

MSR_SSEVec3 MSR_SSEVec3::operator / ( const MSR_SSEVec3& p ) const
{
	return MSR_SSEVec3(x / p.x, y / p.y, z / p.z);
}

MSR_SSEVec3 MSR_SSEVec3::operator + ( const MSR_SSEFloat& f ) const
{	
	return MSR_SSEVec3(x + f, y + f, z + f);
}

MSR_SSEVec3 MSR_SSEVec3::operator - ( const MSR_SSEFloat& f ) const
{	
	return MSR_SSEVec3(x - f, y - f, z - f);
}

MSR_SSEVec3 MSR_SSEVec3::operator * ( const MSR_SSEFloat& f ) const
{
	return MSR_SSEVec3(x * f, y * f, z * f);
}

MSR_SSEVec3 MSR_SSEVec3::operator / ( const MSR_SSEFloat& f ) const
{	
	return MSR_SSEVec3(x / f, y / f, y / f);
}

MSR_SSEVec3 operator * ( const MSR_SSEFloat& f, const MSR_SSEVec3& p )
{
	return MSR_SSEVec3(f * p.x, f * p.y, f * p.z);
}

/////////////////////////////////////////////////////////////////////////////////
// MSR_SSEVec4
/////////////////////////////////////////////////////////////////////////////////

//
// Constructors
//

MSR_SSEVec4::MSR_SSEVec4(const MSR_SSEFloat &_x, const MSR_SSEFloat &_y, const MSR_SSEFloat &_z, const MSR_SSEFloat &_w)
{
	x = _x;
	y = _y;
	z = _z;
	w = _w;
}

MSR_SSEFloat MSR_SSEVec4::Length() const
{
	return MSR_SSESqrt( x*x + y*y + z*z + w*w );
}

MSR_SSEFloat MSR_SSEVec4::LengthSqr() const
{
	return x*x + y*y + z*z + w*w;
}

MSR_SSEFloat MSR_SSEVec4::Dot( const MSR_SSEVec4 &v ) const
{
	return x*v.x + y*v.y + z*v.z = w*v.w;
}

void MSR_SSEVec4::Lerp( const MSR_SSEVec4& v1, const MSR_SSEVec4& v2, const MSR_SSEFloat& t )
{
	x = v1.x + t * (v2.x - v1.x);
	y = v1.y + t * (v2.y - v1.y);
	z = v1.z + t * (v2.z - v1.z);
	w = v1.w + t * (v2.w - v1.w);
}

void MSR_SSEVec4::Normalize()
{
	MSR_SSEFloat f = MSR_SSERSqrt( LengthSqr() );
	x *= f;
	y *= f;
	z *= f;
	w *= f;
}

void MSR_SSEVec4::Build( const MSR_SSEVec4& v1, const MSR_SSEVec4& v2 )
{
	x = v2.x - v1.x;
	y = v2.y - v1.y;
	z = v2.z - v1.z;
	w = v2.w - v1.w;
}

//
// Assignment Operators
//

MSR_SSEVec4 &MSR_SSEVec4::operator += ( const MSR_SSEVec4& v )
{
	x = x + v.x;
	y = y + v.y;
	z = z + v.z;
	w = w + v.w;
	return *this;
}

MSR_SSEVec4 &MSR_SSEVec4::operator -= ( const MSR_SSEVec4& v )
{
	x = x - v.x;
	y = y - v.y;
	z = z - v.z;
	w = w - v.w;
	return *this;
}

MSR_SSEVec4 &MSR_SSEVec4::operator *= ( const MSR_SSEVec4& v )
{
	x = x * v.x;
	y = y * v.y;
	z = z * v.z;
	w = w * v.w;
	return *this;
}

MSR_SSEVec4 &MSR_SSEVec4::operator /= ( const MSR_SSEVec4& v )
{
	x = x / v.x;
	y = y / v.y;
	z = z / v.z;
	w = z / v.w;
	return *this;
}


MSR_SSEVec4 &MSR_SSEVec4::operator += ( const MSR_SSEFloat& f )
{
	x = x + f;
	y = y + f;
	z = z + f;
	w = w + f;
	return *this;
}

MSR_SSEVec4 &MSR_SSEVec4::operator -= ( const MSR_SSEFloat& f )
{
	x = x - f;
	y = y - f;
	z = z - f;
	w = w - f;
	return *this;
}

MSR_SSEVec4 &MSR_SSEVec4::operator *= ( const MSR_SSEFloat& f )
{
	x = x * f;
	y = y * f;
	z = z * f;
	w = w * f;
	return *this;
}

MSR_SSEVec4 &MSR_SSEVec4::operator /= ( const MSR_SSEFloat& f )
{
	x = x / f;
	y = y / f;
	z = z / f;
	w = w / f;
	return *this;
}

//
// Unary Operators
//

MSR_SSEVec4 MSR_SSEVec4::operator + () const
{
	return *this;
}

MSR_SSEVec4 MSR_SSEVec4::operator - () const
{
	return MSR_SSEVec4(-x, -y, -z, -w);
}

//
// Casting Operators
//

MSR_SSEVec4::operator MSR_SSEFloat* ()
{
	return (MSR_SSEFloat*)&x;
}

MSR_SSEVec4::operator const MSR_SSEFloat* () const
{
	return (const MSR_SSEFloat*)&x;
}

//
// Binary Operators 
//

MSR_SSEVec4 MSR_SSEVec4::operator + ( const MSR_SSEVec4& p ) const
{
	return MSR_SSEVec4(x + p.x, y + p.y, z + p.z, w + p.w);
}

MSR_SSEVec4 MSR_SSEVec4::operator - ( const MSR_SSEVec4& p ) const
{
	return MSR_SSEVec4(x - p.x, y - p.y, z - p.z, w - p.w);
}

MSR_SSEVec4 MSR_SSEVec4::operator * ( const MSR_SSEVec4& p ) const
{
	return MSR_SSEVec4(x * p.x, y * p.y, z * p.z, w * p.w);
}

MSR_SSEVec4 MSR_SSEVec4::operator / ( const MSR_SSEVec4& p ) const
{
	return MSR_SSEVec4(x / p.x, y / p.y, z / p.z, w / p.w);
}

MSR_SSEVec4 MSR_SSEVec4::operator + ( const MSR_SSEFloat& f ) const
{	
	return MSR_SSEVec4(x + f, y + f, z + f, w + f);
}

MSR_SSEVec4 MSR_SSEVec4::operator - ( const MSR_SSEFloat& f ) const
{	
	return MSR_SSEVec4(x - f, y - f, z - f, w - f);
}

MSR_SSEVec4 MSR_SSEVec4::operator * ( const MSR_SSEFloat& f ) const
{
	return MSR_SSEVec4(x * f, y * f, z * f, w * f);
}

MSR_SSEVec4 MSR_SSEVec4::operator / ( const MSR_SSEFloat& f ) const
{	
	return MSR_SSEVec4(x / f, y / f, y / f, w / f);
}

MSR_SSEVec4 operator * ( const MSR_SSEFloat& f, const MSR_SSEVec4& p )
{
	return MSR_SSEVec4(f * p.x, f * p.y, f * p.z, f * p.w);
}