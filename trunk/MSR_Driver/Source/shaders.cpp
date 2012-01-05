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

#include "shaders.h"

static const MSR_SSEFloat SSE_ONE = 1.0f;
static const MSR_SSEFloat SSE_ZERO = 0.0f;

void ShadowVertex(MSR_VShaderParameters *params)
{
	params->v_out->p = params->globals->wvp * params->v_in->p;
}

void ShadowFragment(MSR_FShaderParameters *params)
{
}

void ColorVertex(MSR_VShaderParameters *params) 
{
	MSR_Vertex *v_in = params->v_in;
	MSR_TransformedVertex *v_out = params->v_out;
	const MSR_ShaderGlobals *globals = params->globals;

	v_out->p = globals->wvp * v_in->p;
	v_out->varyings[0] = v_in->u;
	v_out->varyings[1] = v_in->v;

	// Get the eye coordinates from the current view matrix. They should just be the inverted position.
	MSR_Vec4 *eye = (MSR_Vec4*)&params->globals->viewinv._41;

	// Transform position to world
	MSR_Vec4 p_world = params->globals->world * v_in->p;

	// Get the eye to point vector
	MSR_Vec4 v_eye = *eye - p_world;
	v_eye.Normalize();
	v_out->varyings[2] = v_eye.x;
	v_out->varyings[3] = v_eye.y;
	v_out->varyings[4] = v_eye.z;

	// Transform normal to world (use the 3x3 portion only)
	MSR_Vec4 n_world = globals->world.TransformNormal(*(MSR_Vec4*)&v_in->n);
	n_world.Normalize();
	v_out->varyings[5] = n_world.x;
	v_out->varyings[6] = n_world.y;
	v_out->varyings[7] = n_world.z;

	MSR_Vec4 ls_vec = mLightMVP * v_in->p;
	v_out->varyings[8] = ls_vec.x;
	v_out->varyings[9] = ls_vec.y;
	v_out->varyings[10] = ls_vec.w;

	v_out->varyings[11] = v_in->c.r;
}

void ColorFragment(MSR_FShaderParameters *params)
{
	MSR_SSEColor3 &out = params->output;
	const MSR_ShaderGlobals *globals = params->globals;

	// Get the texture
	MSR_SSEColor3 tex = MSR_Tex2D_Wrap(params->globals->tex0, params->varyings[0], params->varyings[1]);

	// Depth
	params->varyings[10] = _mm_rcp_ps( *params->varyings[10] );
	params->varyings[8] *= params->varyings[10];
	params->varyings[9] = SSE_ONE - (params->varyings[9] * params->varyings[10]);
	MSR_SSEFloat shadow = MSR_Tex2D_F32(shadow_map_depth, params->varyings[8], params->varyings[9]);

	// Assemble the light direction
	const MSR_Vec4 &ld = params->globals->lights[0].direction;
	MSR_SSEVec3 LightDir( MSR_SSEFloat(ld.x), MSR_SSEFloat(ld.y), MSR_SSEFloat(ld.z) );

	params->varyings[10] += bias_amt;
	float4 cmp = _mm_cmplt_ps(*shadow, *params->varyings[10]);

	// Get the eye vector
	MSR_SSEVec3 *Eye = (MSR_SSEVec3*)&params->varyings[2];
	Eye->Normalize();

	// Get the normal vector
	MSR_SSEVec3 *Normal = (MSR_SSEVec3*)&params->varyings[5];
	Normal->Normalize();

	// Diffuse
	MSR_SSEFloat diff = MSR_Clamp(Normal->Dot(LightDir), *SSE_ZERO, *SSE_ONE);

	// Specular
	MSR_SSEVec3 Reflect = ((diff * MSR_SSEFloat(2.0f)) * *Normal) - LightDir;
	Reflect.Normalize();

	MSR_SSEFloat spec = MSR_Clamp(Reflect.Dot(*Eye), *SSE_ZERO, *SSE_ONE);
	spec = spec * spec * spec * spec * spec * spec * spec;

	float4 cmp2 = _mm_cmpgt_ps( *diff, *SSE_ZERO );
	cmp = _mm_and_ps( cmp2, cmp );

	MSR_SSEFloat clamped_ao = MSR_Clamp( params->varyings[11], *SSE_ZERO, *SSE_ONE );
	out.r = tex.r * clamped_ao;
	out.g = tex.g * clamped_ao;
	out.b = tex.b * clamped_ao;

	MSR_SSEColor3 opt1, opt2;
	opt1.r = globals->ml_ambient[0].r;
	opt1.g = globals->ml_ambient[0].g;
	opt1.b = globals->ml_ambient[0].b;

	opt2.r = (diff * globals->ml_diffuse[0].r) + (spec * globals->ml_specular[0].r) + opt1.r;
	opt2.g = (diff * globals->ml_diffuse[0].g) + (spec * globals->ml_specular[0].g) + opt1.g;
	opt2.b = (diff * globals->ml_diffuse[0].b) + (spec * globals->ml_specular[0].b) + opt1.b;

	out.r *= _mm_or_ps( _mm_and_ps(*opt2.r,cmp), _mm_andnot_ps(cmp, *opt1.r) );
	out.g *= _mm_or_ps( _mm_and_ps(*opt2.g,cmp), _mm_andnot_ps(cmp, *opt1.g) );
	out.b *= _mm_or_ps( _mm_and_ps(*opt2.b,cmp), _mm_andnot_ps(cmp, *opt1.b) );
}