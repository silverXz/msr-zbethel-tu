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

#ifndef MSR_SHADER_H
#define MSR_SHADER_H

#include "MSR.h"
#include "MSR_SSEMath.h"

static float4 one = _mm_set1_ps(1.0f);
static float4 zero = _mm_set1_ps(0.0f);
static float4 f255 = _mm_set1_ps(255.0f);

static inline __m128i mul_epi32(__m128i &a, __m128i &b)
{
	__m128i tmp1 = _mm_mul_epu32(a,b);
	__m128i tmp2 = _mm_mul_epu32( _mm_srli_si128(a,4), _mm_srli_si128(b,4));
	return _mm_unpacklo_epi32(_mm_shuffle_epi32(tmp1, _MM_SHUFFLE (0,0,2,0)), _mm_shuffle_epi32(tmp2, _MM_SHUFFLE (0,0,2,0)));
}

__forceinline float4 MSR_Clamp(MSR_SSEFloat &val, float4 min, float4 max)
{
	return _mm_max_ps( _mm_min_ps( *val, max ), min );
}

__forceinline float4 MSR_Wrap(MSR_SSEFloat &val)
{
	return _mm_max_ps( _mm_sub_ps( *val, _mm_floor_ps(*val) ), zero );
}

__forceinline MSR_SSEColor3 MSR_Tex2D(SDL_Surface *tex, MSR_SSEFloat &u, MSR_SSEFloat &v)
{
	MSR_SSEColor3 res;
	__m128i tU, tV;

	__m128 conv = _mm_rcp_ps( f255 );

	// Formula for computing U and V:
	// tX = (int)( min(iX/iW, 1.0f) * (tex_width - 1) );
	{
		__m128 t2 = _mm_mul_ps( MSR_Clamp(u, zero, one), _mm_set_ps1((float)(tex->clip_rect.w-1)) );
		__m128 t3 = _mm_mul_ps( MSR_Clamp(v, zero, one), _mm_set_ps1((float)(tex->clip_rect.h-1)) );

		tU = _mm_cvtps_epi32(t2);
		tV = _mm_cvtps_epi32(t3);
	}

	// tSample = tV * pitch + tU * bytesPerPixel
	__m128i tIdx = _mm_add_epi32( mul_epi32(tU, _mm_set1_epi32(tex->format->BytesPerPixel)), mul_epi32(tV, _mm_set1_epi32(tex->pitch)));

	// Since SSE doesn't support arbitrary indexing out of an array, we have to extract the indexes,
	// grab the sample, and recreate an SSE register with the new samples.
	Uint8 *ptr = (Uint8*&)tex->pixels;
	Uint8 *sample3 = &ptr[_mm_extract_epi32(tIdx, 3)];
	Uint8 *sample2 = &ptr[_mm_extract_epi32(tIdx, 2)];
	Uint8 *sample1 = &ptr[_mm_extract_epi32(tIdx, 1)];
	Uint8 *sample0 = &ptr[_mm_extract_epi32(tIdx, 0)];

	__m128i tSample = _mm_set_epi32( *(Uint32*)sample3, *(Uint32*)sample2, *(Uint32*)sample1, *(Uint32*)sample0 );

	// Finally, grab each of the channels out by shifting and masking.
 	res.r = _mm_cvtepi32_ps(_mm_srl_epi32( _mm_and_si128( tSample, _mm_set1_epi32(tex->format->Rmask) ), _mm_set_epi32(0, 0, 0, tex->format->Rshift) ) );
	res.g = _mm_cvtepi32_ps(_mm_srl_epi32( _mm_and_si128( tSample, _mm_set1_epi32(tex->format->Gmask) ), _mm_set_epi32(0, 0, 0, tex->format->Gshift) ) );
	res.b = _mm_cvtepi32_ps(_mm_srl_epi32( _mm_and_si128( tSample, _mm_set1_epi32(tex->format->Bmask) ), _mm_set_epi32(0, 0, 0, tex->format->Bshift) ) );

	*res.r = _mm_mul_ps( *res.r, conv );
	*res.g = _mm_mul_ps( *res.g, conv );
	*res.b = _mm_mul_ps( *res.b, conv );

	return res;
}


__forceinline MSR_SSEColor3 MSR_Tex2D_Wrap(SDL_Surface *tex, MSR_SSEFloat &u, MSR_SSEFloat &v)
{
	MSR_SSEColor3 res;
	__m128i tU, tV;

	__m128 conv = _mm_rcp_ps( f255 );

	// Formula for computing U and V:
	// tX = (int)( min(iX/iW, 1.0f) * (tex_width - 1) );
	{
		__m128 t2 = _mm_mul_ps( MSR_Wrap(u), _mm_set_ps1((float)(tex->clip_rect.w-1)) );
		__m128 t3 = _mm_mul_ps( MSR_Wrap(v), _mm_set_ps1((float)(tex->clip_rect.h-1)) );

		tU = _mm_cvtps_epi32(t2);
		tV = _mm_cvtps_epi32(t3);
	}

	// tSample = tV * pitch + tU * bytesPerPixel
	__m128i tIdx = _mm_add_epi32( mul_epi32(tU, _mm_set1_epi32(tex->format->BytesPerPixel)), mul_epi32(tV, _mm_set1_epi32(tex->pitch)));

	// Since SSE doesn't support arbitrary indexing out of an array, we have to extract the indexes,
	// grab the sample, and recreate an SSE register with the new samples.
	Uint8 *ptr = (Uint8*&)tex->pixels;
	Uint8 *sample3 = &ptr[_mm_extract_epi32(tIdx, 3)];
	Uint8 *sample2 = &ptr[_mm_extract_epi32(tIdx, 2)];
	Uint8 *sample1 = &ptr[_mm_extract_epi32(tIdx, 1)];
	Uint8 *sample0 = &ptr[_mm_extract_epi32(tIdx, 0)];

	__m128i tSample = _mm_set_epi32( *(Uint32*)sample3, *(Uint32*)sample2, *(Uint32*)sample1, *(Uint32*)sample0 );

	// Finally, grab each of the channels out by shifting and masking.
	res.r = _mm_cvtepi32_ps(_mm_srl_epi32( _mm_and_si128( tSample, _mm_set1_epi32(tex->format->Rmask) ), _mm_set_epi32(0, 0, 0, tex->format->Rshift) ) );
	res.g = _mm_cvtepi32_ps(_mm_srl_epi32( _mm_and_si128( tSample, _mm_set1_epi32(tex->format->Gmask) ), _mm_set_epi32(0, 0, 0, tex->format->Gshift) ) );
	res.b = _mm_cvtepi32_ps(_mm_srl_epi32( _mm_and_si128( tSample, _mm_set1_epi32(tex->format->Bmask) ), _mm_set_epi32(0, 0, 0, tex->format->Bshift) ) );

	*res.r = _mm_mul_ps( *res.r, conv );
	*res.g = _mm_mul_ps( *res.g, conv );
	*res.b = _mm_mul_ps( *res.b, conv );

	return res;
}

__forceinline MSR_SSEFloat MSR_Tex2D_F32(SDL_Surface *tex, MSR_SSEFloat &u, MSR_SSEFloat &v)
{
	MSR_SSEFloat res;
	__m128i tU, tV;

	// Formula for computing U and V:
	// tX = (int)( min(iX/iW, 1.0f) * (tex_width - 1) );
	{
		__m128 t2 = _mm_mul_ps( MSR_Clamp(u, zero, one), _mm_set_ps1((float)(tex->clip_rect.w-1)) );
		__m128 t3 = _mm_mul_ps( MSR_Clamp(v, zero, one), _mm_set_ps1((float)(tex->clip_rect.h-1)) );

		tU = _mm_cvtps_epi32(t2);
		tV = _mm_cvtps_epi32(t3);
	}

	// tSample = tV * pitch + tU * bytesPerPixel
	__m128i tIdx = _mm_add_epi32( mul_epi32(tU, _mm_set1_epi32(tex->format->BytesPerPixel)), mul_epi32(tV, _mm_set1_epi32(tex->pitch)));

	// Since SSE doesn't support arbitrary indexing out of an array, we have to extract the indexes,
	// grab the sample, and recreate an SSE register with the new samples.
	Uint8 *ptr = (Uint8*&)tex->pixels;
	Uint8 *sample3 = &ptr[_mm_extract_epi32(tIdx, 3)];
	Uint8 *sample2 = &ptr[_mm_extract_epi32(tIdx, 2)];
	Uint8 *sample1 = &ptr[_mm_extract_epi32(tIdx, 1)];
	Uint8 *sample0 = &ptr[_mm_extract_epi32(tIdx, 0)];

	res = _mm_set_ps( *(float*)sample3, *(float*)sample2, *(float*)sample1, *(float*)sample0 );
	return res;
}

#endif