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

#ifndef MSR_H
#define MSR_H

#include <MSR_Math.h>
#include <MSR_SSEMath.h>
#include <SDL/SDL.h>
#include <windows.h>
#include <winbase.h>
#include <xmmintrin.h>
#include <emmintrin.h>
#include <smmintrin.h>

#define MSRAPI extern

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

#define MSR_MAX_LIGHTS				8
#define MSR_MAX_VARYINGS			12
#define MSR_MAX_RENDER_TARGETS		16

#define MSR_DEFAULT_RENDER_TARGET   0

#define MSR_OK						0
#define MSR_ERR_NULL_TARGET			-1
#define MSR_ERR_LOW_MEMORY			-2
#define MSR_ERR_MAX_TARGETS			-3

// S T R U C T S //////////////////////////////////////////////////////

//
// Material
//

struct MSR_Material {
	MSR_Color4 ambient;
	MSR_Color4 diffuse;
	MSR_Color4 specular;
	MSR_Color4 emissive;
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
	MSR_Color4		ambient;
	MSR_Color4		diffuse;
	MSR_Color4		specular;
	MSR_Vec4		position;
	MSR_Vec4		direction;
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
	MSR_Vec4 p;	 
	
	// Color
	MSR_Color4 c;

	// Normal
	MSR_Vec4 n;
	
	// Texture coordinates
	float u, v;
};

//
// Transformed Vertex (Used by Vertex Shader)
//

MSR_SSE_ALIGNED struct MSR_TransformedVertex 
{
	// Position
	MSR_Vec4 p;

	// Arbitrary varying elements
	float varyings[MSR_MAX_VARYINGS];
};

//
// Shader Globals
//

struct MSR_ShaderGlobals 
{
	// Current texture
	SDL_Surface *tex0;

	// Transformation matrices
	MSR_Mat4x4 world, view, projection;
	MSR_Mat4x4 wvp;
	MSR_Mat4x4 viewinv;

	// Material
	MSR_Material material;

	// Lights
	MSR_Light lights[MSR_MAX_LIGHTS];
	bool lights_enabled[MSR_MAX_LIGHTS];

	// Cached lighting data
	MSR_Color4 ml_specular[MSR_MAX_LIGHTS];
	MSR_Color4 ml_diffuse[MSR_MAX_LIGHTS];
	MSR_Color4 ml_ambient[MSR_MAX_LIGHTS];
};

//
// Shader Parameters
//

struct MSR_VShaderParameters
{
	const MSR_ShaderGlobals *globals;
	MSR_Vertex *v_in;
	MSR_TransformedVertex *v_out;
};

struct MSR_FShaderParameters
{
	const MSR_ShaderGlobals *globals;
	
	MSR_SSEFloat varyings[MSR_MAX_VARYINGS];
	MSR_SSEColor3 output;
};

// F U N C T I O N   P R O T O T Y P E S //////////////////////////////

// Library initiation and shutdown 
MSRAPI int MSR_Init( SDL_Surface *screen, Uint32 flags, Uint32 num_threads );
MSRAPI int MSR_Quit();

// Render Targets
MSRAPI int MSR_CreateRenderTarget( SDL_Surface *target, Uint32 flags, Uint32 *id );
MSRAPI void MSR_SetRenderTarget( Uint32 id );
MSRAPI void MSR_GetRenderTargetDepth( Uint32 id, SDL_Surface **depth );

// Render States
MSRAPI void MSR_SetTransform( Uint32 type, const MSR_Mat4x4 &mat );
MSRAPI void MSR_SetCullMode( Uint32 cullmode );
MSRAPI void MSR_SetFillMode( Uint32 fillmode );
MSRAPI void MSR_SetFVF( Uint32 fvf );
MSRAPI void MSR_SetTexture( SDL_Surface *tex );
MSRAPI void MSR_SetMaterial( MSR_Material *mat );
MSRAPI void MSR_SetLight( MSR_Light *light, Uint32 stage );
MSRAPI void MSR_SetLightEnabled( Uint32 stage, bool on );
MSRAPI void MSR_SetLightingMode( Uint32 lightmode );
MSRAPI void MSR_SetZBufferEnabled( bool on );
MSRAPI void MSR_SetBackBufferEnabled( bool on );

// Shaders
MSRAPI void MSR_SetNumVaryings( Uint32 varyings );
MSRAPI void MSR_SetVertexShader( void (*vs)(MSR_VShaderParameters *params) );
MSRAPI void MSR_SetFragmentShader( void (*fs)(MSR_FShaderParameters *params) );

MSRAPI void MSR_DrawTriangles( MSR_Vertex *vertices, Uint32 num_vertices, Uint32 *indices, Uint32 num_indices );

// Rendering
MSRAPI void MSR_BeginScene();
MSRAPI void MSR_Clear(Uint32 flags,Uint32 color);
MSRAPI void MSR_EndScene();
MSRAPI void MSR_Present();

#endif