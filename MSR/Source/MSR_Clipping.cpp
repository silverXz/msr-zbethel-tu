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

#include "MSR_Internal.h"
#include "MSR_Render.h"

enum {
	CLIP_POS_X_BIT = 0x01,
	CLIP_NEG_X_BIT = 0x02,
	CLIP_POS_Y_BIT = 0x04,
	CLIP_NEG_Y_BIT = 0x08,
	CLIP_POS_Z_BIT = 0x10,
	CLIP_NEG_Z_BIT = 0x20
};

static inline void AddInterpVertex(float t, int out, int in, MSR_TransformedVertex *v, int nverts )
{
	#define LINTERP(T, OUT, IN) (OUT) + ((IN) - (OUT)) * (T)

	MSR_TransformedVertex &vout = v[nverts];
	MSR_TransformedVertex &a = v[out];
	MSR_TransformedVertex &b = v[in];

	vout.p.x = LINTERP(t, a.p.x, b.p.x);
	vout.p.y = LINTERP(t, a.p.y, b.p.y);
	vout.p.z = LINTERP(t, a.p.z, b.p.z);
	vout.p.w = LINTERP(t, a.p.w, b.p.w);

	for( int i=0, n=render_context.num_varyings; i<n; i++ )
		vout.varyings[i] = LINTERP(t, a.varyings[i], b.varyings[i]);

	#undef LINTERP
}

#define DIFFERENT_SIGNS(x, y) ((x <= 0 && y > 0) || (x > 0 && y <= 0))

#define CLIP_DOTPROD(I, A, B, C, D) (v[I].p.x * A + v[I].p.y * B + v[I].p.z * C + v[I].p.w * D)

static inline int CalcClipMask( MSR_TransformedVertex &v )
{
	int cmask = 0;
	if( v.p.w - v.p.x < 0.0f ) cmask |= CLIP_POS_X_BIT;
	if( v.p.x + v.p.w < 0.0f ) cmask |= CLIP_NEG_X_BIT;
	if( v.p.w - v.p.y < 0.0f ) cmask |= CLIP_POS_Y_BIT;
	if( v.p.y + v.p.w < 0.0f ) cmask |= CLIP_NEG_Y_BIT;
	if( v.p.w - v.p.z < 0.0f ) cmask |= CLIP_POS_Z_BIT;
	if( v.p.z + v.p.w < 0.0f ) cmask |= CLIP_NEG_Z_BIT;
	return cmask;
}

#include <iostream>
using namespace std;

void ClipTriangle( MSR_TransformedVertex *v, Uint32 thread_id )
{
	int cmask = 0;
	cmask |= CalcClipMask(v[0]);
	cmask |= CalcClipMask(v[1]);
	cmask |= CalcClipMask(v[2]);

	if( cmask == 0 )
	{
		PostProcessVertex(&v[0]);
		PostProcessVertex(&v[1]);
		PostProcessVertex(&v[2]);
		InsertTransformedTriangle(&v[0], &v[1], &v[2], thread_id);
	}
	else 
	{
		int vlist[2][CLIP_BUFFER_SIZE];
		int *inlist = vlist[0], *outlist = vlist[1];
		int n = 3;
		int nverts = 3;

		// Start out with the original vertices
		inlist[0] = 0; inlist[1] = 1; inlist[2] = 2;

		float planes[6][4] = { { 1,  0,  0,  1 },
							   {-1,  0,  0,  1 },
							   { 0,  1,  0,  1 },
						       { 0, -1,  0,  1 },
						  	   { 0,  0,  1,  1 },
					  	       { 0,  0, -1,  1 } };

		int plane_bits[] = { CLIP_NEG_X_BIT, CLIP_POS_X_BIT, CLIP_NEG_Y_BIT, 
							 CLIP_POS_Y_BIT, CLIP_NEG_Z_BIT, CLIP_POS_Z_BIT };

		for( int p = 0; p < 6; p++ )
		{
			// To make things easier to read
			float a = planes[p][0];
			float b = planes[p][1];
			float c = planes[p][2];
			float d = planes[p][3];

			if( cmask & plane_bits[p] ) 
			{ 
				int idxPrev = inlist[0]; 
				float dpPrev = CLIP_DOTPROD(idxPrev, a, b, c, d); 
				int outcount = 0;

				inlist[n] = inlist[0]; 
				for( int i = 1; i<=n; i++ ) 
				{ 
					int idx = inlist[i]; 
					float dp = CLIP_DOTPROD(idx, a, b, c, d); 
					if( dpPrev >= 0.0f ) outlist[outcount++] = idxPrev; 

					if( DIFFERENT_SIGNS( dp, dpPrev ) ) 
					{ 
						if( dp < 0.0f ) 
						{ 
							float t = dp / ( dp - dpPrev ); 
							AddInterpVertex(t, idx, idxPrev, v, nverts); 
						} 
						else 
						{ 
							float t = dpPrev / ( dpPrev - dp ); 
							AddInterpVertex(t, idxPrev, idx, v, nverts); 
						} 
						outlist[outcount++] = nverts; 
						nverts++;
					} 

					idxPrev = idx; 
					dpPrev = dp; 
				} 

				if( outcount < 3 ) return; 

				int *tmp = inlist; 
				inlist = outlist; 
				outlist = tmp; 
				n = outcount;
			}
		}
		
		PostProcessVertex(&v[inlist[0]]);
		PostProcessVertex(&v[inlist[1]]);

		for( int i = 2; i < n; i++ )
		{
			PostProcessVertex(&v[inlist[i]]);
			InsertTransformedTriangle(&v[inlist[0]], &v[inlist[i-1]], &v[inlist[i]], thread_id);	
		}
	}
}