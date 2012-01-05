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
#include "MSR_Fragment.h"
#include <xmmintrin.h>
#include <emmintrin.h>
#include <smmintrin.h>
#include <cstdlib> 
#include <malloc.h>

// Rendering
MSR_RenderContext render_context;

// Job queue

volatile Uint32 job_queue_start_rt;
volatile Uint32 job_queue_start_ft;
volatile Uint32 job_queue_end;
volatile Uint32 num_work_threads;

// Vertex buffer
MSR_TransformedVertex **vertex_buffer;
Uint32 *vertex_buffer_size;

// Face buffer
MSR_TransformedFace **face_buffer;

// Vertex caches
static MSR_VertexCacheElement **vertex_cache;

int MSR_Init( SDL_Surface *screen, Uint32 flags, Uint32 num_threads )
{
	if( !screen ) return MSR_ERR_NULL_TARGET;

	num_work_threads = num_threads;

	int hr;
	Uint32 id;
	if( (hr = MSR_CreateRenderTarget(screen, flags, &id)) != MSR_OK ) return hr;
	MSR_SetRenderTarget(id);

	job_queue_start_rt = job_queue_start_ft = job_queue_end	= 0;

	// Allocate vertex and face buffers
	vertex_buffer_size = new Uint32[num_threads];
	vertex_buffer = new MSR_TransformedVertex*[num_threads];

	face_buffer = new MSR_TransformedFace*[num_threads];

	for( Uint32 i=0; i<num_threads; i++ )
	{
		vertex_buffer_size[i] = 0;
		vertex_buffer[i] = (MSR_TransformedVertex*)_aligned_malloc((sizeof(MSR_TransformedVertex) * MSR_VERTEX_BUFFER_SIZE_CLIP) / num_threads, 16);
		face_buffer[i] = new MSR_TransformedFace[MSR_VERTEX_BUFFER_SIZE_CLIP / (num_threads * 3)];
	}

	//
	// Setup the render context
	//

	ZeroMemory(&render_context,sizeof(MSR_RenderContext));
	render_context.cull_mode		= MSR_CULL_NONE;
	render_context.fill_mode		= MSR_FILL_SOLID;
	render_context.depth_enabled	= true;
	render_context.color_enabled	= true;
	render_context.VertexShader		= NULL;
	render_context.FragmentShader	= NULL;
	render_context.globals.world		= MSR_Mat4x4_Identity;
	render_context.globals.view			= MSR_Mat4x4_Identity;
	render_context.globals.projection	= MSR_Mat4x4_Identity;
	render_context.globals.wvp			= MSR_Mat4x4_Identity;
	render_context.globals.tex0			= NULL;

	ZeroMemory(&render_context.globals.material,sizeof(MSR_Material));
	for( Uint32 i=0; i<MSR_MAX_LIGHTS; i++ ) {
		ZeroMemory(&render_context.globals.lights[i],sizeof(MSR_Light));
		render_context.globals.lights_enabled[i] = false;
	}

	// Clear out the rendering cache
	vertex_cache = new MSR_VertexCacheElement*[num_work_threads];
	for( Uint32 i=0; i<num_work_threads; i++ ) {
		vertex_cache[i] = (MSR_VertexCacheElement*)_aligned_malloc(sizeof(MSR_VertexCacheElement) * MSR_VERTEX_CACHE_SIZE, 16);
		ZeroMemory(vertex_cache[i],sizeof(MSR_VertexCacheElement)*MSR_VERTEX_CACHE_SIZE);
		for( int j=0; j<MSR_VERTEX_CACHE_SIZE; j++ ) vertex_cache[i][j].tag = UINT_MAX;
	}

	// Create the worker threads
	MSR_InitWorkerThreads();

	return MSR_OK;
}

int MSR_Quit()
{
	// Clean up worker threads
	MSR_DestroyWorkerThreads();

	// Destroy all the render targets
	for( Uint32 i=0; i<num_render_targets; i++ ) 
		MSR_DestroyRenderTarget( i );

	// Vertex and face buffers
	for( Uint32 i=0; i<num_work_threads; i++ )
	{
		_aligned_free(vertex_buffer[i]);
		SAFE_DELETE_ARRAY(face_buffer[i]);
	}

	SAFE_DELETE_ARRAY(vertex_buffer_size);
	
	// Clean up vertex cache
	for( Uint32 i=0; i<num_work_threads; i++ ) 
		_aligned_free(vertex_cache[i]);

	SAFE_DELETE_ARRAY( vertex_cache );

	return 0;
}

inline void CopyVertex( MSR_TransformedVertex *dst, MSR_TransformedVertex *src )
{
#define COPY( v0, v1 ) ( reinterpret_cast<__m128&>(v0) = reinterpret_cast<__m128&>(v1) )

	COPY( dst->p, src->p ); 
	COPY( dst->varyings[0], src->varyings[0] ); 
	COPY( dst->varyings[4], src->varyings[4] ); 
	COPY( dst->varyings[8], src->varyings[8] ); 

#undef COPY
}

void GetTransformedVertex(Uint32 thread_id,MSR_Vertex *vertices, Uint32 idx, MSR_TransformedVertex *v_trans) 
{
	_mm_prefetch((const char*)&vertices[idx], _MM_HINT_T0);

	MSR_VertexCacheElement &cache_item = vertex_cache[thread_id][idx & (MSR_VERTEX_CACHE_SIZE-1)];
	if( cache_item.tag == idx ) {
		CopyVertex(v_trans, &cache_item.v);
	} else {

		// Process the vertex and put it in the cache
		cache_item.tag = idx;

		MSR_VShaderParameters params;
		params.globals = &render_context.globals;
		params.v_in = &vertices[idx];
		params.v_out = v_trans;

		render_context.VertexShader(&params);
		CopyVertex(&cache_item.v, params.v_out);
	}
}

static inline void compute_gradient( float C, 
									 float di21, float di31, 
									 float dx21, float dx31,
									 float dy21, float dy31,
									 float &dx, float &dy ) 
{
	float A = di31 * dy21 - di21 * dy31;
	float B = dx31 * di21 - dx21 * di31;

	dx = -A/C;
	dy = -B/C;
}

void InsertTransformedTriangle(MSR_TransformedVertex *v0, MSR_TransformedVertex *v1, MSR_TransformedVertex *v2, Uint32 thread_id)
{
	// Perform Back-face culling
	float d1x = v2->p.x - v0->p.x;
	float d1y = v2->p.y - v0->p.y;
	float d2x = v2->p.x - v1->p.x;
	float d2y = v2->p.y - v1->p.y;

	float value = d1x * d2y - d1y * d2x;
	if( render_context.cull_mode == MSR_CULL_CCW && value > 0 )
		return;
	else if( render_context.cull_mode == MSR_CULL_CW && value < 0 ) 
		return;
		
	if( value > 0 )
	{
		MSR_TransformedVertex *tmp = v1;
		v1 = v2;
		v2 = tmp;
	}

	Uint32 base_index = vertex_buffer_size[thread_id];
	Uint32 face_idx = base_index / 3;
	MSR_TransformedFace *face = &face_buffer[thread_id][face_idx];

	// Cache perspective correct varyings for v0
	face->v0x = v0->p.x; face->v0y = v0->p.y; face->v0w = v0->p.w;
	for( Uint32 i=0; i<render_context.num_varyings; i++ )
		face->v0v[i] = v0->p.w * v0->varyings[i];

	// Compute fixed point coordinates
	face->fp1[0] = iround(16.0f * v0->p.x);
	face->fp2[0] = iround(16.0f * v1->p.x);
	face->fp3[0] = iround(16.0f * v2->p.x);
	face->fp1[1] = iround(16.0f * v0->p.y);
	face->fp2[1] = iround(16.0f * v1->p.y);
	face->fp3[1] = iround(16.0f * v2->p.y);

	// Deltas
	const int DX12 = face->fp1[0] - face->fp2[0];
	const int DX23 = face->fp2[0] - face->fp3[0];
	const int DX31 = face->fp3[0] - face->fp1[0];
	const int DY12 = face->fp1[1] - face->fp2[1];
	const int DY23 = face->fp2[1] - face->fp3[1];
	const int DY31 = face->fp3[1] - face->fp1[1];

	// Fixed-point deltas
	const int FDX12 = DX12 << 4;
	const int FDX23 = DX23 << 4;
	const int FDX31 = DX31 << 4;
	const int FDY12 = DY12 << 4;
	const int FDY23 = DY23 << 4;
	const int FDY31 = DY31 << 4;

	// Compute interpolation data
	float FLTDX21, FLTDX31, FLTDY21, FLTDY31, INTERP_C;
	FLTDX21 = v1->p.x - v0->p.x;
	FLTDX31 = v2->p.x - v0->p.x;
	FLTDY21 = v1->p.y - v0->p.y;
	FLTDY31 = v2->p.y - v0->p.y;
	INTERP_C = FLTDX21 * FLTDY31 - FLTDX31 * FLTDY21;

	// Setup inverse W interpolate
	compute_gradient(INTERP_C, v1->p.w - v0->p.w, v2->p.w - v0->p.w, 
					 FLTDX21, FLTDX31, FLTDY21, FLTDY31, 
					 face->dw.x, face->dw.y);

	// Setup the rest of the interpolates
	for( Uint32 i=0; i<render_context.num_varyings; i++ ) 
	{
		float v0v = v0->varyings[i] * v0->p.w;
		float v1v = v1->varyings[i] * v1->p.w;
		float v2v = v2->varyings[i] * v2->p.w;
		compute_gradient(INTERP_C, v1v - v0v, v2v - v0v, 
						 FLTDX21, FLTDX31, FLTDY21, FLTDY31, 
						 face->dv[i].x, face->dv[i].y);
	}

	// Half-edge constants
	face->c1 = DY12 * face->fp1[0] - DX12 * face->fp1[1];
	face->c2 = DY23 * face->fp2[0] - DX23 * face->fp2[1];
	face->c3 = DY31 * face->fp3[0] - DX31 * face->fp3[1];

	// Correct for fill convention
	if(DY12 < 0 || (DY12 == 0 && DX12 > 0)) face->c1++;
	if(DY23 < 0 || (DY23 == 0 && DX23 > 0)) face->c2++;
	if(DY31 < 0 || (DY31 == 0 && DX31 > 0)) face->c3++;
	
	// Compute the fixed point bounding box
	face->minx = min(face->fp1[0], min(face->fp2[0], face->fp3[0]));
	face->maxx = max(face->fp1[0], max(face->fp2[0], face->fp3[0]));
	face->miny = min(face->fp1[1], min(face->fp2[1], face->fp3[1]));
	face->maxy = max(face->fp1[1], max(face->fp2[1], face->fp3[1]));

	// Loop through each tile and test the triangle against it.
	int min_index_x = max( (face->minx >> 4) >> MSR_SCREEN_TILE_SIZE_SHIFT, 0);
	int max_index_x = min( (face->maxx >> 4) >> MSR_SCREEN_TILE_SIZE_SHIFT, (int)set_render_target->num_tiles_x-1);
	int min_index_y = max( (face->miny >> 4) >> MSR_SCREEN_TILE_SIZE_SHIFT, 0);
	int max_index_y = min( (face->maxy >> 4) >> MSR_SCREEN_TILE_SIZE_SHIFT, (int)set_render_target->num_tiles_y-1);

	for( int y = min_index_y; y <= max_index_y; y++ ) 
	{	
		for( int x = min_index_x; x <= max_index_x; x++ ) 
		{
			if( max_index_x - min_index_x <= 2 || max_index_y - min_index_y <= 2 )
			{
				Uint32 tile_idx = y * set_render_target->num_tiles_x + x;
				MSR_Tile &tile = set_render_target->tiles[tile_idx];
				tile.index_buffer[ thread_id ][ tile.index_buffer_size[ thread_id ] ] = face_idx;
				tile.index_buffer_size[ thread_id ]++;
				tile.frag_tiles[ thread_id ][ face_idx ] = 0;

				if( InterlockedIncrement((Uint32*)&tile.dirty) == 1 ) {
					Uint32 job_idx = InterlockedIncrement((Uint32*)&job_queue_end) - 1;
					set_render_target->job_queue[job_idx] = tile_idx; 
				}
			}
			else
			{
				// Corners of tile
				int x0 = x << (MSR_SCREEN_TILE_SIZE_SHIFT + 4);
				int x1 = ( (x + 1) << (MSR_SCREEN_TILE_SIZE_SHIFT + 4) ) - 1;
				int y0 = y << (MSR_SCREEN_TILE_SIZE_SHIFT + 4);
				int y1 = ( (y + 1) << (MSR_SCREEN_TILE_SIZE_SHIFT + 4) ) - 1;

				// Evaluate half-space functions
				bool a00 = face->c1 + DX12 * y0 - DY12 * x0 > 0;
				bool a10 = face->c1 + DX12 * y0 - DY12 * x1 > 0;
				bool a01 = face->c1 + DX12 * y1 - DY12 * x0 > 0;
				bool a11 = face->c1 + DX12 * y1 - DY12 * x1 > 0;
				int a = (a00 << 0) | (a10 << 1) | (a01 << 2) | (a11 << 3);

				bool b00 = face->c2 + DX23 * y0 - DY23 * x0 > 0;
				bool b10 = face->c2 + DX23 * y0 - DY23 * x1 > 0;
				bool b01 = face->c2 + DX23 * y1 - DY23 * x0 > 0;
				bool b11 = face->c2 + DX23 * y1 - DY23 * x1 > 0;
				int b = (b00 << 0) | (b10 << 1) | (b01 << 2) | (b11 << 3);

				bool c00 = face->c3 + DX31 * y0 - DY31 * x0 > 0;
				bool c10 = face->c3 + DX31 * y0 - DY31 * x1 > 0;
				bool c01 = face->c3 + DX31 * y1 - DY31 * x0 > 0;
				bool c11 = face->c3 + DX31 * y1 - DY31 * x1 > 0;
				int c = (c00 << 0) | (c10 << 1) | (c01 << 2) | (c11 << 3);

				// Skip block when outside an edge
				if(a == 0x0 || b == 0x0 || c == 0x0) continue;

				Uint32 tile_idx = y * set_render_target->num_tiles_x + x;
				MSR_Tile &tile = set_render_target->tiles[tile_idx];
				tile.index_buffer[ thread_id ][ tile.index_buffer_size[ thread_id ] ] = face_idx;
				tile.index_buffer_size[ thread_id ]++;

				if( InterlockedIncrement((Uint32*)&tile.dirty) == 1 ) {
					Uint32 job_idx = InterlockedIncrement((Uint32*)&job_queue_end) - 1;
					set_render_target->job_queue[job_idx] = tile_idx; 
				}

				// Test if we can trivially accept the entire tile
				tile.frag_tiles[ thread_id ][ face_idx ] = ( a != 0xF || b != 0xF || c != 0xF ) ? 0 : 1;
			}
		}
	}	

	// Add the vertices to the vertex buffer
	CopyVertex(&vertex_buffer[thread_id][base_index], v0);
	CopyVertex(&vertex_buffer[thread_id][base_index+1], v1);
	CopyVertex(&vertex_buffer[thread_id][base_index+2], v2);

	// Set the vertex pointers
	face->v[0] = &vertex_buffer[thread_id][base_index]; 
	face->v[1] = &vertex_buffer[thread_id][base_index+1]; 
	face->v[2] = &vertex_buffer[thread_id][base_index+2];

	vertex_buffer_size[thread_id] += 3;
}

void ProcessTrianglesV( Uint32 thread_id ) 
{
	MSR_Vertex *vertices = thread_render_data->vertices;
	Uint32 *indices = thread_render_data->indices;

	for( Uint32 i = thread_render_data->partitions[thread_id].start_index; i < thread_render_data->partitions[thread_id].end_index; i+=3 ) 
	{
		// The maximum amount of vertices after clipping
		MSR_TransformedVertex v[CLIP_BUFFER_SIZE];
		GetTransformedVertex(thread_id,vertices,indices[i  ],&v[0]);
		GetTransformedVertex(thread_id,vertices,indices[i+1],&v[1]);
		GetTransformedVertex(thread_id,vertices,indices[i+2],&v[2]);

		// Clip and insert triangle, and any additional triangles generated by clipping.
		ClipTriangle(v, thread_id);
	}
}

void ProcessTrianglesR( Uint32 thread_id ) 
{
	while(true) 
	{
		// Get the next item in the queue
		Uint32 start = InterlockedIncrement(&job_queue_start_rt) - 1;
		if( start >= job_queue_end ) {
			break;
		}

		// Find our tile index
		Uint32 tile_idx = set_render_target->job_queue[start];
		MSR_Tile *tiles = set_render_target->tiles;
		
		// Convert to fixed point to save the rasterizer from having to do it
		Uint32 tile_x, tile_y, tile_width, tile_height;

		if( render_context.fill_mode == MSR_FILL_SOLID ) 
		{
			tile_x = tiles[tile_idx].x << 4;
			tile_y = tiles[tile_idx].y << 4;
			tile_width = tiles[tile_idx].width << 4;
			tile_height = tiles[tile_idx].height << 4;

			// Reset the number of fragments to zero
			MSR_FragmentBufferClear(&tiles[tile_idx].frag_buffer);

			for( Uint32 thread_id=0; thread_id<num_work_threads; thread_id++ )
			{
				for( Uint32 j=0; j<tiles[tile_idx].index_buffer_size[thread_id]; j++ )
				{
					Uint32 idx = tiles[tile_idx].index_buffer[thread_id][j];

					// First test to make sure that this hasn't been trivially accepted. If it has, we're done!
					if( !tiles[tile_idx].frag_tiles[ thread_id ][ idx ] )
						RasterizeTriangleSolid(thread_id, idx, &tiles[tile_idx].frag_buffer,tile_x,tile_y,tile_width,tile_height);
					else
					{
						MSR_Fragment *frag = MSR_FragmentBufferGetNext(&tiles[tile_idx].frag_buffer);
						frag->state = MSR_FRAGMENT_STATE_TILE;
						frag->thread_id = thread_id;
						frag->face_idx = idx;
						frag->x = tiles[tile_idx].x;
						frag->y = tiles[tile_idx].y;
					}

					// Now do a size check on the fragment buffer and resize if necessary
					MSR_FragmentBufferResize(&tiles[tile_idx].frag_buffer);
				}

				tiles[tile_idx].index_buffer_size[thread_id] = 0;
			}
		}

		tiles[tile_idx].dirty = 0;
	}
}

void ProcessFragments( Uint32 thread_id )
{
	while(true)
	{
		// Get the next item in the queue
		Uint32 start = InterlockedIncrement(&job_queue_start_ft) - 1;
		if( start >= job_queue_end ) {
			break;
		}

		Uint32 tile_idx = set_render_target->job_queue[start];

		// Block if this is still being rasterized
		while( set_render_target->tiles[tile_idx].dirty ) Sleep(0);

		RenderFragments(&set_render_target->tiles[tile_idx].frag_buffer);
	}
}

void MSR_DrawTrianglesBatchSerial( MSR_Vertex *vertices, Uint32 num_vertices, Uint32 *indices, Uint32 num_indices ) 
{
	thread_render_data->indices = indices;

	MSR_ThreadRenderPartition *partitions = thread_render_data->partitions;
	partitions[0].start_index = 0;
	partitions[0].end_index = num_indices;
	job_queue_start_rt = job_queue_start_ft = job_queue_end = 0;
	vertex_buffer_size[0] = 0;

	//
	// Since we are the only thread, process all data at once.
	// 

	ProcessTrianglesV(0);
	ProcessTrianglesR(0);
	ProcessFragments(0);
}

void MSR_DrawTrianglesBatchParallel( MSR_Vertex *vertices, Uint32 num_vertices, Uint32 *indices, Uint32 num_indices ) 
{
	// Split up the indices evenly among threads
	Uint32 indices_per_thread = (num_indices / (3 * num_work_threads)) * 3;
	Uint32 tris_left_over = (num_indices % (3 * num_work_threads)) / 3;
	thread_render_data->indices = indices;

	// Setup the main thread partition
	MSR_ThreadRenderPartition *partitions = thread_render_data->partitions;
	partitions[0].start_index = 0;
	partitions[0].end_index = indices_per_thread;
	if( tris_left_over ) {
		partitions[0].end_index += 3;
		tris_left_over--;
	}

	// Setup the work thread partitions
	for( Uint32 curr_thread=1; curr_thread<num_work_threads; curr_thread++ ) {
		partitions[curr_thread].start_index = partitions[curr_thread-1].end_index;
		partitions[curr_thread].end_index = partitions[curr_thread].start_index + indices_per_thread;
		if( tris_left_over ) {
			partitions[curr_thread].end_index += 3;
			tris_left_over--;
		}
	}

	// Wait for the work threads to finish.
	while( curr_threads_working ) Sleep(0);
	
	job_queue_start_rt = job_queue_start_ft = job_queue_end = 0;

	// Signal all worker threads to begin processing immediately.
	SDL_mutexP(end_working_lock);
	curr_threads_working = num_work_threads;
	thread_render_data->state = THREAD_STATE_VERTEX;
	SDL_CondBroadcast(start_working_cond);
	SDL_mutexV(end_working_lock);

	// Process our vertex data.
	vertex_buffer_size[0] = 0;
	ProcessTrianglesV(0);

	// If we are the last, prepare the job queue and signal all threads to begin processing the jobs immediately.
	Uint32 new_value = InterlockedDecrement(&curr_threads_working);
	if( !new_value ) { 

		// Signal other threads to begin rasterization
		InterlockedExchange(&curr_threads_working, num_work_threads);
		thread_render_data->state = THREAD_STATE_RASTER;

	} else
		while( THREAD_STATE_VERTEX == thread_render_data->state ) Sleep(0);

	// Rasterize our triangles.
	ProcessTrianglesR(0);

	// Process our fragment data
	ProcessFragments(0);

	InterlockedDecrement(&curr_threads_working);
}

MSRAPI void MSR_DrawTriangles( MSR_Vertex *vertices, Uint32 num_vertices, Uint32 *indices, Uint32 num_indices )
{	
	// Clear out the vertex caches
	for( Uint32 i=0; i<num_work_threads; i++ )
		for( Uint32 j=0; j<MSR_VERTEX_CACHE_SIZE; j++ ) vertex_cache[i][j].tag = UINT_MAX;

	// Compute the current world * view * matrix transform 
	MSR_ShaderGlobals &globals = render_context.globals;
	globals.wvp = globals.world * globals.view * globals.projection;

	// Compute light * material colors
	for( int i=0; i<MSR_MAX_LIGHTS; i++ )
	{
		if( globals.lights_enabled[i] )
		{
			globals.ml_ambient[i] = globals.lights[i].ambient * globals.material.ambient;
			globals.ml_diffuse[i] = globals.lights[i].diffuse * globals.material.diffuse;
			globals.ml_specular[i] = globals.lights[i].specular * globals.material.specular;
		}
	}

	PrepareRasterizer();
	thread_render_data->vertices = vertices;

	// Figure the right draw call to make
	void (*DrawTrianglesPtr)(MSR_Vertex *vertices, Uint32 num_vertices, Uint32 *indices, Uint32 num_indices );
	if( num_work_threads > 1 && (num_indices / (3*num_work_threads)) ) 
		DrawTrianglesPtr = &MSR_DrawTrianglesBatchParallel;
	else 
		DrawTrianglesPtr = &MSR_DrawTrianglesBatchSerial;

	// Break this up into batches
	Uint32 curr_idx = 0;
	while( curr_idx+MSR_VERTEX_BUFFER_SIZE <= num_indices ) { 

		DrawTrianglesPtr( vertices, num_vertices, &indices[curr_idx], MSR_VERTEX_BUFFER_SIZE );
		curr_idx += MSR_VERTEX_BUFFER_SIZE;
	}

	// Handle the last set of indices, if there are any
	Uint32 last_bit = num_indices - curr_idx;
	if( last_bit ) {

		DrawTrianglesPtr( vertices, num_vertices, &indices[curr_idx], last_bit );
	}
}