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

#include "MSR_Render.h"

// Face Buffer
extern MSR_TransformedFace **face_buffer;
extern volatile Uint32 num_work_threads;
extern volatile Uint32 curr_threads_working;

#define SYNC_THREADS() { while( curr_threads_working ) Sleep(0); }

void (*RenderFragments)(MSR_FragmentBuffer *fb);

// Render Targets
Uint32 num_render_targets = 0;
MSR_RenderTarget *set_render_target = NULL;
MSR_RenderTarget render_targets[MSR_MAX_RENDER_TARGETS];

//
// Render State Settings
// 

void MSR_SetTransform(Uint32 type, const MSR_Mat4x4 &mat) 
{
	SYNC_THREADS();

	MSR_ShaderGlobals &globals = render_context.globals;

	if( type == MSR_TRANSFORM_WORLD ) {
		globals.world = mat;
	} else if( type == MSR_TRANSFORM_VIEW ) {
		globals.view = mat;
		globals.viewinv.SetInverseOf(mat);
	} else if( type == MSR_TRANSFORM_PROJECTION ) {
		globals.projection = mat;
	} 
}

void MSR_SetCullMode( Uint32 cullmode )
{
	SYNC_THREADS();

	render_context.cull_mode = (Uint8)cullmode;
}

void MSR_SetFillMode( Uint32 fillmode )
{
	SYNC_THREADS();

	render_context.fill_mode = (Uint8)fillmode;
}

void MSR_SetTexture( SDL_Surface *tex ) 
{
	SYNC_THREADS();

	render_context.globals.tex0 = tex;
}

void MSR_SetMaterial( MSR_Material *mat ) 
{
	SYNC_THREADS();

	memcpy(&render_context.globals.material,mat,sizeof(MSR_Material));
}

void MSR_SetLight( MSR_Light *light, Uint32 stage ) 
{
	SYNC_THREADS();

	if( stage < MSR_MAX_LIGHTS ) 
		memcpy(&render_context.globals.lights[stage],light,sizeof(MSR_Light));
}

void MSR_SetLightEnabled( Uint32 stage, bool on )
{
	SYNC_THREADS();

	if( stage < MSR_MAX_LIGHTS )
		render_context.globals.lights_enabled[stage] = on;
}

void MSR_SetZBufferEnabled( bool on )
{
	SYNC_THREADS();

	if( on && !set_render_target->z_buffer ) return;

	render_context.depth_enabled = on;
}

void MSR_SetBackBufferEnabled( bool on )
{
	SYNC_THREADS();

	render_context.color_enabled = on;
}

void MSR_SetNumVaryings( Uint32 varyings )
{
	SYNC_THREADS();

	render_context.num_varyings = varyings;
}

void MSR_SetVertexShader( void (*vs)(MSR_VShaderParameters *params) )
{
	SYNC_THREADS();

	render_context.VertexShader = vs;
}

void MSR_SetFragmentShader( void (*fs)(MSR_FShaderParameters *params) )
{
	SYNC_THREADS();

	render_context.FragmentShader = fs;
}

void MSR_BeginScene() 
{
	SYNC_THREADS();

	SDL_LockSurface(set_render_target->back_buffer);
	if( set_render_target->z_buffer )
		SDL_LockSurface(set_render_target->z_buffer);
}

void MSR_Clear(Uint32 flags, Uint32 color) 
{
	SYNC_THREADS();

	if( flags & MSR_CLEAR_TARGET )
		SDL_FillRect(set_render_target->back_buffer,NULL,color);
	if( flags & MSR_CLEAR_ZBUFFER ) 
		SDL_FillRect(set_render_target->z_buffer,NULL,0);
}

void MSR_EndScene() 
{
	SYNC_THREADS();

	SDL_UnlockSurface(set_render_target->back_buffer);
	if( set_render_target->z_buffer )
		SDL_UnlockSurface(set_render_target->z_buffer);
}

void MSR_Present() 
{
	// If we're still waiting on a thread, then sleep
	SYNC_THREADS();

	SDL_Flip(set_render_target->back_buffer);
}

int MSR_CreateRenderTarget( SDL_Surface *target, Uint32 flags, Uint32 *id )
{
	if( num_render_targets == MSR_MAX_RENDER_TARGETS ) return MSR_ERR_MAX_TARGETS;

	MSR_RenderTarget &rt = render_targets[num_render_targets];

	// Assign the internal back buffer pointer
	rt.back_buffer = target;

	//
	// Allocate tiles
	//

	// Compute number of tiles in the x and whether we need an edge tile
	rt.num_tiles_x = target->clip_rect.w / MSR_SCREEN_TILE_SIZE;
	Uint32 num_whole_tiles_x = rt.num_tiles_x;
	Uint32 extra_pixels_x = target->clip_rect.w % MSR_SCREEN_TILE_SIZE;
	if( extra_pixels_x ) rt.num_tiles_x++;

	// Compute number of tiles in the y and whether we need an edge tile
	rt.num_tiles_y = target->clip_rect.h / MSR_SCREEN_TILE_SIZE;
	Uint32 num_whole_tiles_y = rt.num_tiles_y;
	Uint32 extra_pixels_y = target->clip_rect.h % MSR_SCREEN_TILE_SIZE;
	if( extra_pixels_y ) rt.num_tiles_y++;

	rt.num_tiles = rt.num_tiles_x * rt.num_tiles_y;
	rt.tiles = new MSR_Tile[ rt.num_tiles ];

	//
	// Setup tile sizes and positions
	//

	Uint32 x,y;
	for( y=0; y<rt.num_tiles_y; y++ ) {

		// Is this the last tile in the y, and does it have less pixels?
		bool edge_y = (y==(rt.num_tiles_y-1) && extra_pixels_y);

		for( x=0; x<rt.num_tiles_x; x++ ) {

			// Is this the last tile in the x, and does it have less pixels?
			bool edge_x = (x==(rt.num_tiles_x-1) && extra_pixels_x);

			// Fill in the tile info
			MSR_Tile *t			 = &rt.tiles[y*rt.num_tiles_x+x];
			t->frag_tiles		 = new Uint8*[num_work_threads];
			t->index_buffer		 = new Uint32*[num_work_threads];
			t->index_buffer_size = new Uint32[num_work_threads];

			for( Uint32 i=0; i<num_work_threads; i++ ) {
				t->index_buffer_size[i] = 0;
				t->index_buffer[i] = new Uint32[ MSR_BIN_TRIANGLE_QUEUE_SIZE / num_work_threads ];
				t->frag_tiles[i] = new Uint8[ MSR_BIN_TRIANGLE_QUEUE_SIZE / (num_work_threads * 3) ];
			}

			t->width			 = !edge_x ? MSR_SCREEN_TILE_SIZE-1 : extra_pixels_x-1;
			t->height			 = !edge_y ? MSR_SCREEN_TILE_SIZE-1 : extra_pixels_y-1;
			t->x				 = x * MSR_SCREEN_TILE_SIZE;
			t->y				 = y * MSR_SCREEN_TILE_SIZE;
			t->dirty			 = 0;

			MSR_FragmentBufferInit( &t->frag_buffer );
		}
	}

	// Create the job queue
	rt.job_queue = new Uint32[rt.num_tiles_x*rt.num_tiles_y];

	// Create the Z-Buffer
	if( flags & MSR_INIT_ZBUFFER ) {
		rt.z_buffer = SDL_CreateRGBSurface(0,rt.back_buffer->w,rt.back_buffer->h,32,0,0,0,0);
		if( !rt.z_buffer ) 
			return MSR_ERR_LOW_MEMORY;
	} else {
		rt.z_buffer = NULL;
	}

	*id = num_render_targets++;

	return MSR_OK;
}

void MSR_DestroyRenderTarget( Uint32 id )
{
	if( id >= MSR_MAX_RENDER_TARGETS ) return;
	MSR_RenderTarget &rt = render_targets[id];

	// Clean up tiles
	for( Uint32 t=0; t<rt.num_tiles_x*rt.num_tiles_y; t++ ) {

		for( Uint32 i=0; i<num_work_threads; i++ ) {
			SAFE_DELETE_ARRAY(rt.tiles[t].index_buffer[i]);
			SAFE_DELETE_ARRAY(rt.tiles[t].frag_tiles[i]);
		}

		SAFE_DELETE_ARRAY(rt.tiles[t].index_buffer_size);
		SAFE_DELETE_ARRAY(rt.tiles[t].frag_tiles);
		MSR_FragmentBufferDestroy(&rt.tiles[t].frag_buffer);
	}

	// Clean up job queue
	SAFE_DELETE_ARRAY(rt.tiles);
	SAFE_DELETE_ARRAY(rt.job_queue);

	// Delete Z Buffer
	if( rt.z_buffer ) SDL_FreeSurface(rt.z_buffer);
}

void MSR_SetRenderTarget( Uint32 id )
{
	SYNC_THREADS();

	if( id >= MSR_MAX_RENDER_TARGETS ) return;

	set_render_target = &render_targets[id];
}

void MSR_GetRenderTargetDepth( Uint32 id, SDL_Surface **depth )
{
	if( id >= MSR_MAX_RENDER_TARGETS ) return;

	*depth = render_targets[id].z_buffer;
}

void PostProcessVertex(MSR_TransformedVertex *v_trans) 
{
	float width = (float)set_render_target->back_buffer->clip_rect.w;
	float height = (float)set_render_target->back_buffer->clip_rect.h;

	MSR_Vec4 *pos = &v_trans->p;

	// Homogenize (set Z to 1/w)
	float invW = 1.0f/pos->w;
	pos->x *= invW;
	pos->y *= invW;
	pos->w = invW;

	// Transform to screen
	float half_width = 0.5f * width - 0.5f;
	float half_height = 0.5f * height - 0.5f; 
	pos->x = half_width + half_width*pos->x;
	pos->y = half_height - half_height*pos->y;
}

template <bool useColorBuffer, bool useZBuffer>
void RenderFragmentsGeneric(MSR_FragmentBuffer *fb) 
{
	//
	// BEGIN HELPER MACROS
	//

#define SETUP_VARYINGS(bx, by)																\
	{																						\
		/* Get any of the vertices and compute the start delta for x and y */				\
		float dxstart = bx - face->v0x;														\
		float dystart = by - face->v0y;														\
																							\
		/* Compute the inverse W for all four pixels */										\
		__m128 base = _mm_set1_ps(face->v0w + face->dw.x * dxstart + face->dw.y * dystart);	\
		__m128 dx = _mm_set1_ps(face->dw.x);												\
		WDY = _mm_set1_ps(face->dw.y);														\
		W0 = _mm_add_ps( base, _mm_mul_ps( dx, C0 ) );										\
		W1 = _mm_add_ps( W0, _mm_mul_ps( dx, C1 ) );										\
																							\
		/* Compute the varyings for all four pixels */										\
		for( Uint32 i=0; i<render_context.num_varyings; i++ )								\
		{																					\
			MSR_Vec2 &v = face->dv[i];														\
			base = _mm_set1_ps(face->v0v[i] + face->dv[i].x * dxstart + face->dv[i].y * dystart);	\
			dx = _mm_set1_ps(face->dv[i].x);												\
			VDY[i] = _mm_set1_ps(face->dv[i].y);											\
			V0[i] = _mm_add_ps( base, _mm_mul_ps( dx, C0 ) );								\
			V1[i] = _mm_add_ps( V0[i], _mm_mul_ps( dx, C1 ) );								\
		}																					\
	}

#define INTERPOLATE_Y()																		\
	{																						\
		W0 = _mm_add_ps(W0, WDY);															\
		W1 = _mm_add_ps(W1, WDY);															\
																							\
		if( useColorBuffer )																\
		{																					\
			for( Uint32 i=0; i<render_context.num_varyings; i+=2 ) {						\
				V0[i+0] = _mm_add_ps(V0[i+0], VDY[i+0]);									\
				V1[i+0] = _mm_add_ps(V1[i+0], VDY[i+0]);									\
				V0[i+1] = _mm_add_ps(V0[i+1], VDY[i+1]);									\
				V1[i+1] = _mm_add_ps(V1[i+1], VDY[i+1]);									\
			}																				\
			colorBuffer += cb_pitch;														\
		}																					\
																							\
		if( useZBuffer ) depthBuffer += db_pitch;											\
	}

#define LOAD_BUFFERS(W, x, oq, dq, cl, dl, dm, rejectlabel)									\
	{																						\
		dl = &depthBuffer[x];																\
		if( useZBuffer )																	\
		{																					\
			dq = _mm_load_ps(dl);															\
		}																					\
																							\
		if( useColorBuffer )																\
		{																					\
			cl = &colorBuffer[x];															\
			oq = _mm_load_si128((__m128i*)cl);												\
		}																					\
																							\
		if( useZBuffer )																	\
		{																					\
			dm = *(__m128i*)&_mm_cmpge_ps(W, dq);											\
			/* Early reject this pixel if we can */											\
			if( !_mm_movemask_ps(*(__m128*)&dm) )											\
				goto rejectlabel;															\
		}																					\
	}	

#define COMPUTE_PARAMS(params, W, V)														\
	{																						\
		__m128 w = _mm_rcp_ps(W);															\
		for( Uint32 i=0; i<render_context.num_varyings; i+=2 ) {							\
			params.varyings[i+0].f = _mm_mul_ps(w, V[i+0]);									\
			params.varyings[i+1].f = _mm_mul_ps(w, V[i+1]);									\
		}																					\
	}

#define SATURATE_RESULT(output, nquad)														\
	{																						\
		__m128i iR, iG, iB;																	\
																							\
		/* Convert back to 255 range */														\
		output.r = _mm_mul_ps(*output.r, fMax);												\
		output.g = _mm_mul_ps(*output.g, fMax);												\
		output.b = _mm_mul_ps(*output.b, fMax);												\
																							\
		/* Clamp the values to 255 */														\
		output.r = _mm_min_ps(*output.r, fMax);												\
		output.g = _mm_min_ps(*output.g, fMax);												\
		output.b = _mm_min_ps(*output.b, fMax);												\
																							\
		/* Convert to integer */															\
		iR = _mm_cvtps_epi32(*output.r);													\
		iG = _mm_cvtps_epi32(*output.g);													\
		iB = _mm_cvtps_epi32(*output.b);													\
																							\
		/* Logical shift the red and blue components to their correct locations */			\
		iR = _mm_slli_epi32(iR, 16);														\
		iG = _mm_slli_epi32(iG, 8);															\
																							\
		/* Or the results together */														\
		nquad = _mm_or_si128(iR, iG);														\
		nquad = _mm_or_si128(nquad, iB);													\
	}

#define STORE_RESULT(W, oq, nq, dq, cbl, dbl, m)											\
	{																						\
		if( useZBuffer )																	\
		{																					\
			/* Compute the new quad that should be store into the frame buffer.
			   To do this, take the new quad and mask out any bits that should not be written. Then,
			   we take the old quad and mask out bits that should be written. Then we logically or them
			   together and write the result. */											\
			dq = _mm_or_ps( _mm_and_ps(*(__m128*)&m, W), _mm_andnot_ps(*(__m128*)&m, dq) );	\
			_mm_store_si128((__m128i*)dbl, *(__m128i*)&dq);									\
		}																					\
																							\
		if( useColorBuffer )																\
		{																					\
			/* Store the new color into the frame buffer */									\
			nq  = _mm_or_si128( _mm_and_si128(m, nq), _mm_andnot_si128(m, oq));				\
			_mm_store_si128((__m128i*)cbl, nq);												\
		}																					\
	}

	//
	// END HELPER MACROS
	// 

	MSR_SSE_ALIGNED MSR_FShaderParameters params;
	params.globals = &render_context.globals;

	__m128 W0, W1, WDY;
	__m128 V0[MSR_MAX_VARYINGS], V1[MSR_MAX_VARYINGS], VDY[MSR_MAX_VARYINGS];

	__m128 C0	= _mm_set_ps( 3.0f, 2.0f, 1.0f, 0.0f );
	__m128 C1	= _mm_set_ps1( 4.0f );
	__m128 fMax = _mm_set_ps1( 255.0f );

	__m128i mask_mask = _mm_set_epi32(8, 4, 2, 1);

	Uint32 cb_pitch = set_render_target->back_buffer->pitch / 4;
	Uint32 db_pitch = set_render_target->z_buffer->pitch / 4;

	for( int elem=0; elem<fb->elements; elem++ )
	{
		// Get the next fragment
		MSR_Fragment *frag = MSR_FragmentBufferGet(fb, elem);
		MSR_TransformedFace *face = &face_buffer[frag->thread_id][frag->face_idx];
		
		if( frag->state == MSR_FRAGMENT_STATE_BLOCK_MASK )
		{
			__m128i frag_mask = _mm_set1_epi32(frag->mask);
			__m128i oquad, nquad, cbmask, dbmask;
			__m128 dbquad, InvW;
			Uint32 *colorBuffer;
			float *depthBuffer;

			colorBuffer = (Uint32*)set_render_target->back_buffer->pixels + frag->y * cb_pitch + frag->x;
			if( useZBuffer ) depthBuffer = (float*)set_render_target->z_buffer->pixels + frag->y * db_pitch + frag->x;

			// Get any of the vertices and compute the start delta for x and y				
			float dxstart = (float)frag->x - face->v0x;														
			float dystart = (float)frag->y - face->v0y;

			// Compute the inverse W for all four pixels */	
			__m128 dx = _mm_set1_ps(face->dw.x);
			__m128 base = _mm_set1_ps(face->v0w + face->dw.x * dxstart + face->dw.y * dystart);	
			W0 = _mm_add_ps( base, _mm_mul_ps( dx, C0 ) );	

			// Load in the quads from the depth and color buffers 
			if( useZBuffer ) dbquad = _mm_load_ps(depthBuffer);
			oquad = _mm_load_si128((__m128i*)colorBuffer);	
						
			// Compute mask and load buffers 
			cbmask = _mm_and_si128( frag_mask, mask_mask );
			cbmask = _mm_cmpgt_epi32(cbmask, _mm_setzero_si128());	

			if( useZBuffer )																	
			{																											
				dbmask = *(__m128i*)&_mm_cmpge_ps(W0, dbquad);											
				cbmask = _mm_and_si128( dbmask, cbmask );													
																		
				if( !_mm_movemask_ps(*(__m128*)&cbmask) )											
					continue;			
			}						

			// Compute the varyings for all four pixels		
			if( useColorBuffer )
			{
				InvW = _mm_rcp_ps( W0 );
				for( Uint32 i=0; i<render_context.num_varyings; i++ )								
				{																			
					dx = _mm_mul_ps( _mm_set1_ps(face->dv[i].x), C0 );					
					base = _mm_set1_ps(face->v0v[i] + face->dv[i].x * dxstart + face->dv[i].y * dystart);							
					params.varyings[i].f = _mm_mul_ps( _mm_add_ps( base, dx ), InvW );
				}

				render_context.FragmentShader(&params);
				SATURATE_RESULT(params.output, nquad);
			}

			if( useZBuffer )																	
			{																					
				/* Compute the new quad that should be store into the frame buffer.
				   To do this, take the new quad and mask out any bits that should not be written. Then,
				   we take the old quad and mask out bits that should be written. Then we logically or them
				   together and write the result. */											
				dbquad = _mm_or_ps( _mm_and_ps(*(__m128*)&cbmask, W0), _mm_andnot_ps(*(__m128*)&cbmask, dbquad) );	
				_mm_store_si128((__m128i*)depthBuffer, *(__m128i*)&dbquad);									
			}																					

			if( useColorBuffer )
			{
				// Store the new color into the frame buffer								
				nquad  = _mm_or_si128( _mm_and_si128(cbmask, nquad), _mm_andnot_si128(cbmask, oquad));					
				_mm_store_si128((__m128i*)colorBuffer, nquad);													
			}
		}
		else 
		{
			if( frag->state == MSR_FRAGMENT_STATE_BLOCK )
			{
				float *depthBuffer;
				Uint32 *colorBuffer = (Uint32*)set_render_target->back_buffer->pixels + frag->y * cb_pitch + frag->x;
				if( useZBuffer ) depthBuffer = (float*)set_render_target->z_buffer->pixels + frag->y * db_pitch + frag->x;
				SETUP_VARYINGS(frag->x, frag->y);

				for( Uint32 y=0; y<8; y++ )
				{
					__m128 dbquad;
					__m128i oquad, nquad, dbmask;
					Uint32 *cbTileLine;
					float *dbTileLine;

					LOAD_BUFFERS(W0, 0, oquad, dbquad, cbTileLine, dbTileLine, dbmask, INTERP_SB1);

					if( useColorBuffer )
					{
						COMPUTE_PARAMS(params, W0, V0);
						render_context.FragmentShader(&params);
						SATURATE_RESULT(params.output, nquad);
					}

					STORE_RESULT(W0, oquad, nquad, dbquad, cbTileLine, dbTileLine, dbmask);

				INTERP_SB1:
					LOAD_BUFFERS(W1, 4, oquad, dbquad, cbTileLine, dbTileLine, dbmask, INTERP_SB2);
					
					if( useColorBuffer )
					{
						COMPUTE_PARAMS(params, W1, V1);
						render_context.FragmentShader(&params);
						SATURATE_RESULT(params.output, nquad);
					}

					STORE_RESULT(W1, oquad, nquad, dbquad, cbTileLine, dbTileLine, dbmask);

				INTERP_SB2:
					INTERPOLATE_Y();
				}
			}
			else
			{
				for( Uint32 by=frag->y; by < frag->y + MSR_SCREEN_TILE_SIZE; by += 8 )
				{
					for( Uint32 bx=frag->x; bx < frag->x + MSR_SCREEN_TILE_SIZE; bx += 8 )
					{
						float *depthBuffer;
						Uint32 *colorBuffer = (Uint32*)set_render_target->back_buffer->pixels + by * cb_pitch + bx;
						if( useZBuffer ) depthBuffer = (float*)set_render_target->z_buffer->pixels + by * db_pitch + bx;
						SETUP_VARYINGS(bx, by);

						for( Uint32 y=0; y<8; y++ )
						{
							__m128 dbquad;
							__m128i oquad, nquad, dbmask;
							Uint32 *cbTileLine;
							float *dbTileLine;

							LOAD_BUFFERS(W0, 0, oquad, dbquad, cbTileLine, dbTileLine, dbmask, INTERP_ST1);
							
							if( useColorBuffer )
							{
								COMPUTE_PARAMS(params, W0, V0);
								render_context.FragmentShader(&params);
								SATURATE_RESULT(params.output, nquad);
							}
								
							STORE_RESULT(W0, oquad, nquad, dbquad, cbTileLine, dbTileLine, dbmask);

						INTERP_ST1:
							LOAD_BUFFERS(W1, 4, oquad, dbquad, cbTileLine, dbTileLine, dbmask, INTERP_ST2);
							
							if( useColorBuffer )
							{
								COMPUTE_PARAMS(params, W1, V1);
								render_context.FragmentShader(&params);
								SATURATE_RESULT(params.output, nquad);
							}

							STORE_RESULT(W1, oquad, nquad, dbquad, cbTileLine, dbTileLine, dbmask);

						INTERP_ST2:
							INTERPOLATE_Y();
						}
					}
				}
			}
		}
	}

	MSR_FragmentBufferClear(fb);

#undef SETUP_VARYINGS
#undef INTERPOLATE_Y
#undef LOAD_BUFFERS
#undef STORE_RESULT
}

void RasterizeTriangleSolid(Uint32 thread_id, Uint32 face_idx, MSR_FragmentBuffer *frag_buffer, int tile_x, int tile_y, int tile_width, int tile_height) 
{
	MSR_TransformedFace *face = &face_buffer[thread_id][face_idx];

	MSR_TransformedVertex *v0 = face->v[0];
	MSR_TransformedVertex *v1 = face->v[1];
	MSR_TransformedVertex *v2 = face->v[2];

	// 28.4 fixed-point coordinates
	const int X1 = face->fp1[0];
	const int X2 = face->fp2[0];
	const int X3 = face->fp3[0];
	const int Y1 = face->fp1[1];
	const int Y2 = face->fp2[1];
	const int Y3 = face->fp3[1];

	// Half edge constants
	const int C1 = face->c1;
	const int C2 = face->c2;
	const int C3 = face->c3;

	// Deltas
	const int DX12 = X1 - X2;
	const int DX23 = X2 - X3;
	const int DX31 = X3 - X1;
	const int DY12 = Y1 - Y2;
	const int DY23 = Y2 - Y3;
	const int DY31 = Y3 - Y1;

	// Fixed-point deltas
	const int FDX12 = DX12 << 4;
	const int FDX23 = DX23 << 4;
	const int FDX31 = DX31 << 4;
	const int FDY12 = DY12 << 4;
	const int FDY23 = DY23 << 4;
	const int FDY31 = DY31 << 4;

	// Pre-calculate the SSE offsets and interpolates
	__m128i iOffsetDY12 = _mm_set_epi32(FDY12 * 3, FDY12 * 2, FDY12, 0);
	__m128i iOffsetDY23 = _mm_set_epi32(FDY23 * 3, FDY23 * 2, FDY23, 0);
	__m128i iOffsetDY31 = _mm_set_epi32(FDY31 * 3, FDY31 * 2, FDY31, 0);
	__m128i iFDY12 = _mm_set1_epi32(FDY12 << 2);
	__m128i iFDY23 = _mm_set1_epi32(FDY23 << 2);
	__m128i iFDY31 = _mm_set1_epi32(FDY31 << 2);

	// Bounding rectangle
	int minx = (max(face->minx,tile_x)			   + 0xF) >> 4;
	int maxx = (min(face->maxx,tile_x+tile_width)  + 0xF) >> 4;
	int miny = (max(face->miny,tile_y)			   + 0xF) >> 4;
	int maxy = (min(face->maxy,tile_y+tile_height) + 0xF) >> 4;

	// Block size, standard 8x8 (must be power of two)
	const int q = 8;

	// Start in corner of 8x8 block
	minx &= ~(q - 1);
	miny &= ~(q - 1);

	// Loop through blocks
	for(int y = miny; y < maxy; y += q)
	{
		for(int x = minx; x < maxx; x += q)
		{
			// Corners of block
			int x0 = x << 4;
			int x1 = (x + q - 1) << 4;
			int y0 = y << 4;
			int y1 = (y + q - 1) << 4;

			// Evaluate half-space functions
			bool a00 = C1 + DX12 * y0 - DY12 * x0 > 0;
			bool a10 = C1 + DX12 * y0 - DY12 * x1 > 0;
			bool a01 = C1 + DX12 * y1 - DY12 * x0 > 0;
			bool a11 = C1 + DX12 * y1 - DY12 * x1 > 0;
			int a = (a00 << 0) | (a10 << 1) | (a01 << 2) | (a11 << 3);

			bool b00 = C2 + DX23 * y0 - DY23 * x0 > 0;
			bool b10 = C2 + DX23 * y0 - DY23 * x1 > 0;
			bool b01 = C2 + DX23 * y1 - DY23 * x0 > 0;
			bool b11 = C2 + DX23 * y1 - DY23 * x1 > 0;
			int b = (b00 << 0) | (b10 << 1) | (b01 << 2) | (b11 << 3);

			bool c00 = C3 + DX31 * y0 - DY31 * x0 > 0;
			bool c10 = C3 + DX31 * y0 - DY31 * x1 > 0;
			bool c01 = C3 + DX31 * y1 - DY31 * x0 > 0;
			bool c11 = C3 + DX31 * y1 - DY31 * x1 > 0;
			int c = (c00 << 0) | (c10 << 1) | (c01 << 2) | (c11 << 3);

			// Skip block when outside an edge
			if(a == 0x0 || b == 0x0 || c == 0x0) continue;

			// Accept whole block when totally covered
			if( a == 0xF && b == 0xF && c == 0xF )
			{
				// Generate a fragment
				MSR_Fragment *frag = MSR_FragmentBufferGetNext(frag_buffer);
				frag->state = MSR_FRAGMENT_STATE_BLOCK;
				frag->thread_id = thread_id;
				frag->face_idx = face_idx;
				frag->x = x;
				frag->y = y;
			}
			else 
			{
				int CY1 = C1 + DX12 * y0 - DY12 * x0;
				int CY2 = C2 + DX23 * y0 - DY23 * x0;
				int CY3 = C3 + DX31 * y0 - DY31 * x0;

				for(int iy = y; iy < y + q; iy++)
				{
					__m128i iCX1, iCX2, iCX3, cx1_mask, cx2_mask, cx3_mask, cx_mask_comp;

					// Generate the edge functions 128-bit masks for all four pixels
					iCX1 = _mm_sub_epi32( _mm_set1_epi32(CY1), iOffsetDY12 );
					cx1_mask = _mm_cmpgt_epi32( iCX1, _mm_setzero_si128() );
					iCX2 = _mm_sub_epi32( _mm_set1_epi32(CY2), iOffsetDY23 );
					cx2_mask = _mm_cmpgt_epi32( iCX2, _mm_setzero_si128() );
					iCX3 = _mm_sub_epi32( _mm_set1_epi32(CY3), iOffsetDY31 );
					cx3_mask = _mm_cmpgt_epi32( iCX3, _mm_setzero_si128() );			

					// Composite them together
					cx_mask_comp = _mm_and_si128( cx1_mask, _mm_and_si128( cx2_mask, cx3_mask ) );

					// Generate a 4-bit mask from the composite 128-bit mask 
					Uint32 mask = _mm_movemask_ps(*(__m128*)&_mm_and_si128(cx_mask_comp, _mm_set1_epi32(0xF0000000)));
					if( mask )
					{
						MSR_Fragment *frag = MSR_FragmentBufferGetNext(frag_buffer);
						frag->state = MSR_FRAGMENT_STATE_BLOCK_MASK;
						frag->thread_id = thread_id;
						frag->face_idx = face_idx;
						frag->x = x;
						frag->y = iy;
						frag->mask = mask;
					}
					
					// Interpolate the edge functions and generate another set of 128-bit masks for all four pixels
					iCX1 = _mm_sub_epi32( iCX1, iFDY12 );
					cx1_mask = _mm_cmpgt_epi32( iCX1, _mm_setzero_si128() );
					iCX2 = _mm_sub_epi32( iCX2, iFDY23 );
					cx2_mask = _mm_cmpgt_epi32( iCX2, _mm_setzero_si128() );
					iCX3 = _mm_sub_epi32( iCX3, iFDY31 );
					cx3_mask = _mm_cmpgt_epi32( iCX3, _mm_setzero_si128() );

					// Composite them together
					cx_mask_comp = _mm_and_si128( cx1_mask, _mm_and_si128(cx2_mask, cx3_mask) );

					// Generate a 4-bit mask from the composite 128-bit mask 
					mask = _mm_movemask_ps(*(__m128*)&_mm_and_si128(cx_mask_comp, _mm_set1_epi32(0xF0000000)));
					if( mask )
					{
						MSR_Fragment *frag = MSR_FragmentBufferGetNext(frag_buffer);
						frag->state = MSR_FRAGMENT_STATE_BLOCK_MASK;
						frag->thread_id = thread_id;
						frag->face_idx = face_idx;
						frag->x = x + 4;
						frag->y = iy;
						frag->mask = mask;
					}
		
					CY1 += FDX12;
					CY2 += FDX23;
					CY3 += FDX31;
				}
			}
		}
	}
}

void PrepareRasterizer()
{
	if( render_context.color_enabled )
	{
		if( render_context.depth_enabled )
			RenderFragments = RenderFragmentsGeneric<true, true>;
		else
			RenderFragments = RenderFragmentsGeneric<true, false>;
	}
	else
	{
		if( render_context.depth_enabled )
			RenderFragments = RenderFragmentsGeneric<false, true>;
		else
			RenderFragments = RenderFragmentsGeneric<false, false>;
	}
}