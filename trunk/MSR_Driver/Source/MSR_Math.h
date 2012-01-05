///////////////////////////////////////////////////////////////////////
//
// Multithreaded Software Rasterizer
// Copyright 2010 :: Zach Bethel 
//
///////////////////////////////////////////////////////////////////////

#ifndef MSR_MATH_H
#define MSR_MATH_H

#include <cmath>
#include <cstring>
#include <iostream>

// D E F I N E S ////////////////////////////////////////////////////////////

#define MSR_PI 3.141592654f
#define MSR_PI2 1.5707963f
#define MSR_2PI 6.2831853f
#define MSR_EPSILON 0.00001f

#define MSR_ToRadian( degree ) ((degree) * (MSR_PI / 180.0f))
#define MSR_ToDegree( radian ) ((radian) * (180.0f / MSR_PI))

// F O R W A R D   D E C L A R A T I O N S //////////////////////////////////

class MSR_Point;
class MSR_Vector;
class MSR_Matrix;
class MSR_Plane;
class MSR_Color;

class MSR_Point
{
public:
	MSR_Point() {}
	MSR_Point( const float* );
	MSR_Point( const MSR_Point& );
	MSR_Point( float x, float y );

	// Functions
	float Length() const;
	float LengthSqr() const;
	float Dot( const MSR_Point& ) const;
	void  Slerp( const MSR_Point&, const MSR_Point&, float );
	void  Build( const MSR_Point&, const MSR_Point& );
	void  Normalize();
	void  Print();	

	// Assignment operators
	MSR_Point& operator += ( const MSR_Point& );
	MSR_Point& operator -= ( const MSR_Point& );
	MSR_Point& operator *= ( float );
	MSR_Point& operator /= ( float );

	// Unary operators
	MSR_Point operator + () const;
	MSR_Point operator - () const;

	// Casting operators
	operator float* ();
	operator const float* () const;

	// Binary operators
	MSR_Point operator + ( const MSR_Point& ) const;
	MSR_Point operator - ( const MSR_Point& ) const;
	MSR_Point operator * ( float ) const;
	MSR_Point operator / ( float ) const;

	friend MSR_Point operator * ( float, const MSR_Point& );

	// Equality operators
	bool operator == ( const MSR_Point& ) const;
	bool operator != ( const MSR_Point& ) const;

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

class MSR_Vector
{
public:
	MSR_Vector() {}
	MSR_Vector( const float* );
	MSR_Vector( const MSR_Vector&, float f );
	MSR_Vector( float x, float y, float z );
	MSR_Vector( float x, float y, float z, float w );

	// Functions
	float Length() const;
	float LengthSqr() const;
	float Dot( const MSR_Vector& ) const;
	void  Cross( const MSR_Vector&, const MSR_Vector& );
	void  Slerp( const MSR_Vector&, const MSR_Vector&, float );
	void  Build( const MSR_Vector&, const MSR_Vector& );
	void  Normalize();
	void  Homogenize();
	void  Print();

	// Assignment operators
	MSR_Vector& operator += ( const MSR_Vector& );
	MSR_Vector& operator -= ( const MSR_Vector& );
	MSR_Vector& operator *= ( float );
	MSR_Vector& operator /= ( float );

	// Unary operators
	MSR_Vector operator + () const;
	MSR_Vector operator - () const;

	// Casting operators
	operator float* ();
	operator const float* () const;

	// Binary operators
	MSR_Vector operator + ( const MSR_Vector& ) const;
	MSR_Vector operator - ( const MSR_Vector& ) const;
	MSR_Vector operator * ( float ) const;
	MSR_Vector operator / ( float ) const;

	// dot product operator
	float operator * ( const MSR_Vector& ) const;

	friend MSR_Vector operator * ( float, const MSR_Vector& );

	// Equality operators
	bool operator == ( const MSR_Vector& ) const;
	bool operator != ( const MSR_Vector& ) const;

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
	};
};

class MSR_Matrix
{
public:
	MSR_Matrix() {}
	MSR_Matrix( const float * );
	MSR_Matrix( const MSR_Matrix& );
	MSR_Matrix( float _11, float _12, float _13, float _14,
			    float _21, float _22, float _23, float _24,
			    float _31, float _32, float _33, float _34,
			    float _41, float _42, float _43, float _44 );

	// Helper functions
	bool IsIdentity();
	void Identity();
	void Translate( float x, float y, float z );
	void Scale( float x, float y, float z);
	void RotateX( float x );
	void RotateY( float y );
	void RotateZ( float z );
	void RotateArbitrary( const MSR_Vector& axis, float d );
	void TransposeOf( const MSR_Matrix& m );
	int  InverseOf( const MSR_Matrix& m );
	void LookAt( const MSR_Vector& pos, const MSR_Vector& lookAt, const MSR_Vector& worldUp );
	void PerspectiveFovLH( float fov, float aspect, float zn, float zf );
	MSR_Vector TransformNormal( const MSR_Vector& v );
	void Print();

	// Access grants
	float& operator () ( unsigned int row, unsigned int col );
	float  operator () ( unsigned int row, unsigned int col ) const;

	// Casting operators
	operator float* ();
	operator const float* () const;

	// Assignment operators
	void operator = ( const MSR_Matrix& ) const;
	MSR_Matrix& operator *= ( const MSR_Matrix& );
	MSR_Matrix& operator += ( const MSR_Matrix& );
	MSR_Matrix& operator -= ( const MSR_Matrix& );
	MSR_Matrix& operator *= ( float );
	MSR_Matrix& operator /= ( float );

	// Unary operators
	MSR_Matrix operator + () const;
	MSR_Matrix operator - () const;

	// Binary operators
	MSR_Matrix operator * ( const MSR_Matrix& ) const;
	MSR_Vector operator * ( const MSR_Vector& ) const;
	MSR_Matrix operator + ( const MSR_Matrix& ) const;
	MSR_Matrix operator - ( const MSR_Matrix& ) const;
	MSR_Matrix operator * ( float ) const;
	MSR_Matrix operator / ( float ) const;

	friend MSR_Matrix operator * ( float, const MSR_Matrix& );

public:

	union 
	{
		// Array indexed storage
		float m[4][4];

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

class MSR_Plane
{
public:
	MSR_Plane() {}
	MSR_Plane( const MSR_Vector&, const MSR_Vector& );
	MSR_Plane( float px, float py, float pz, float vx, float vy, float vz );

	void Print();

	// Assignment operators
	MSR_Plane& operator *= ( float );
	MSR_Plane& operator /= ( float );

	// Unary operators
	MSR_Plane operator + () const;
	MSR_Plane operator - () const;

	// Binary operators
	MSR_Plane operator * ( float ) const;
	MSR_Plane operator / ( float ) const;

	friend MSR_Plane operator * ( float, const MSR_Plane& );

	bool operator == ( const MSR_Plane& ) const;
	bool operator != ( const MSR_Plane& ) const;

public:
	MSR_Vector p;
	MSR_Vector n;
};

class MSR_Color 
{
public:
	MSR_Color() {}
	MSR_Color(float, float, float, float);
	MSR_Color(const MSR_Color &);
	MSR_Color(const float *);

	void Print();

	// Assignment operators
	MSR_Color& operator *= (float);
	MSR_Color& operator /= (float);
	MSR_Color& operator -= (float);
	MSR_Color& operator += (float);
	MSR_Color& operator *= (const MSR_Color&);
	MSR_Color& operator /= (const MSR_Color&);
	MSR_Color& operator -= (const MSR_Color&);
	MSR_Color& operator += (const MSR_Color&);

	// Binary operators
	MSR_Color operator * (float) const;
	MSR_Color operator / (float) const;
	MSR_Color operator - (float) const;
	MSR_Color operator + (float) const;
	MSR_Color operator * (const MSR_Color&) const;
	MSR_Color operator / (const MSR_Color&) const;
	MSR_Color operator - (const MSR_Color&) const;
	MSR_Color operator + (const MSR_Color&) const;

	// Equality operators
	bool operator == (const MSR_Color&) const;
	bool operator != (const MSR_Color&) const;

public:
	union {

		float m[4];

		struct {
			float r, g, b, a;
		};
	};
};

// G L O B A L S ////////////////////////////////////////////////////////////

const MSR_Matrix MSR_Matrix_Identity = MSR_Matrix(1.0f,0.0f,0.0f,0.0f,
												 0.0f,1.0f,0.0f,0.0f,
												 0.0f,0.0f,1.0f,0.0f,
												 0.0f,0.0f,0.0f,1.0f);

// F U N C T I O N S //////////////////////////////////////////////////

int iround(float x);

#endif
