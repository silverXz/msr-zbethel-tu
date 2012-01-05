///////////////////////////////////////////////////////////////////////
//
// Multithreaded Software Rasterizer
// Copyright 2010 :: Zach Bethel 
//
///////////////////////////////////////////////////////////////////////

#ifndef MSR_H
#define MSR_H

#include "MSR_Math.h"
#include <SDL/SDL.h>
#include <windows.h>
#include <winbase.h>

#ifndef MSRAPI
#define MSRAPI extern
#endif

// D E F I N E S //////////////////////////////////////////////////////

#define MSR_INIT_ZBUFFER			0x1

#define MSR_TRANSFORM_WORLD			0
#define MSR_TRANSFORM_VIEW			1
#define MSR_TRANSFORM_PROJECTION	2

#define MSR_CULL_NONE				0
#define MSR_CULL_CCW				1
#define MSR_CULL_CW					2

#define MSR_CLEAR_ZBUFFER			0x1
#define MSR_CLEAR_TARGET			0x2

#define MSR_FILL_WIRE				0
#define MSR_FILL_SOLID				1

#define MSR_LIGHTING_NONE			0
#define MSR_LIGHTING_FLAT			1
#define MSR_LIGHTING_GOURAUD		2

#define MSR_FVF_COLOR				0x1
#define MSR_FVF_NORMAL				0x2
#define MSR_FVF_TEX_UV				0x4

#define MSR_MAX_LIGHTS				8

#define MSR_OK						0
#define MSR_ERR_NULL_TARGET			-1
#define MSR_ERR_LOW_MEMORY			-2

// S T R U C T S //////////////////////////////////////////////////////

//
// Material
//

struct MSR_Material {
	MSR_Color ambient;
	MSR_Color diffuse;
	MSR_Color specular;
	MSR_Color emissive;
	float	  power;
};

//
// Light
//

enum MSR_LIGHT_TYPE {
	MSR_LIGHT_TYPE_AMBIENT=0,
	MSR_LIGHT_TYPE_POINT,
	MSR_LIGHT_TYPE_DIRECTIONAL,
	MSR_LIGHT_TYPE_SPOT
};

struct MSR_Light {
	MSR_LIGHT_TYPE	type;
	MSR_Color		ambient;
	MSR_Color		diffuse;
	MSR_Color		specular;
	MSR_Vector		position;
	MSR_Vector		direction;
	float			range;
	float			falloff;
	float			attenuation0;
	float			attenuation1;
	float			attenuation2;
	float			theta;
	float			phi;
};

//
// Vertex
//

struct MSR_Vertex {
	
	// Position
	MSR_Vector p;	 
	
	// Color
	MSR_Color c;

	// Normal
	MSR_Vector n;
	
	// Texture coordinates
	float u, v;
};

// F U N C T I O N   P R O T O T Y P E S //////////////////////////////

// Library initiation and shutdown 
MSRAPI int MSR_Init( SDL_Surface *screen, Uint32 flags, Uint32 num_threads );
MSRAPI int MSR_Quit();

// Render States
MSRAPI void MSR_SetTransform( Uint32 type, MSR_Matrix &mat );
MSRAPI void MSR_SetCullMode( Uint32 cullmode );
MSRAPI void MSR_SetFillMode( Uint32 fillmode );
MSRAPI void MSR_SetFVF( Uint32 fvf );
MSRAPI void MSR_SetTexture( SDL_Surface *tex );
MSRAPI void MSR_SetMaterial( MSR_Material *mat );
MSRAPI void MSR_SetLight( MSR_Light *light, Uint32 stage );
MSRAPI void MSR_SetLightEnabled( Uint32 stage, bool on );
MSRAPI void MSR_SetLightingMode( Uint32 lightmode );
MSRAPI void MSR_SetZBufferEnabled( bool on );

MSRAPI void MSR_DrawTriangles( MSR_Vertex *vertices, Uint32 num_vertices, Uint16 *indices, Uint16 num_indices );

// Rendering
MSRAPI void MSR_BeginScene();
MSRAPI void MSR_Clear(Uint32 flags,Uint32 color);
MSRAPI void MSR_EndScene();
MSRAPI void MSR_Present();

#endif