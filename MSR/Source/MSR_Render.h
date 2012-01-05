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

#ifndef MSR_RENDER_H
#define MSR_RENDER_H

#include "MSR_Math.h"
#include "MSR_Fragment.h"

#pragma comment(lib,"MSR_Math.lib")

// D E F I N E S //////////////////////////////////////////////////////

#define SAFE_DELETE(p) { if(p) { delete[] p; (p)=NULL; } }
#define SAFE_DELETE_ARRAY(p) { if(p) { delete[] (p); (p)=NULL; } }

#define MSR_VERTEX_BUFFER_SIZE			18000
#define MSR_VERTEX_BUFFER_SIZE_CLIP		MSR_VERTEX_BUFFER_SIZE*5
#define MSR_VERTEX_CACHE_SIZE			32

#define MSR_BIN_TRIANGLE_QUEUE_SIZE		MSR_VERTEX_BUFFER_SIZE_CLIP
#define MSR_SCREEN_TILE_SIZE			64
#define MSR_SCREEN_TILE_SIZE_SHIFT		6

// S T R U C T S //////////////////////////////////////////////////////

//
// Render Context
//

struct MSR_RenderContext {

	// All the rendering attributes
	Uint8 cull_mode;	
	Uint8 fill_mode;
	bool depth_enabled;
	bool color_enabled;

	// Shader info
	Uint32 num_varyings;
	void (*VertexShader)(MSR_VShaderParameters *);
	void (*FragmentShader)(MSR_FShaderParameters *);

	MSR_ShaderGlobals globals;
};

//
// Vertex cache element
//

struct MSR_VertexCacheElement {
	Uint32 tag;
	Uint32 vb_idx;
	MSR_TransformedVertex v;
};

//
// Face
//

struct MSR_TransformedFace {
	MSR_TransformedVertex *v[3];

	// Cached start values for varyings
	float v0x, v0y, v0w;
	float v0v[MSR_MAX_VARYINGS];

	// Cached fixed point coordinates
	int fp1[2];
	int fp2[2];
	int fp3[2];

	// Cached half edge constants
	int c1, c2, c3;

	// Cache the bounding box of this face
	int minx, maxx, miny, maxy;

	// Inverse W coordinate
	MSR_Vec2 dw;

	// Varyings
	MSR_Vec2 dv[MSR_MAX_VARYINGS];
};

// 
// Tile
//

struct MSR_Tile {
	
	// Width and height of tile
	Uint16 width, height;

	// Corner of the tile
	Uint16 x, y;

	// Bin queues of elements
	Uint32 **index_buffer;
	Uint32 *index_buffer_size;
	volatile Uint32 dirty;

	// Bin queue of trivially accepted tiles
	Uint8 **frag_tiles;

	// Fragment buffer for rasterization
	MSR_FragmentBuffer frag_buffer;
};

struct MSR_RenderTarget {

	SDL_Surface *back_buffer;
	SDL_Surface *z_buffer;

	Uint32 num_tiles_x;
	Uint32 num_tiles_y;
	Uint32 num_tiles;
	MSR_Tile *tiles;

	Uint32 *job_queue;
};

// G L O B A L S ///////////////////////////////////////////////////////

// Render targets
MSRAPI MSR_RenderTarget render_targets[MSR_MAX_RENDER_TARGETS];
MSRAPI MSR_RenderTarget *set_render_target;
MSRAPI Uint32 num_render_targets;

// Render context
MSRAPI MSR_RenderContext render_context;

// Fragment rendering function
MSRAPI void (*RenderFragments)(MSR_FragmentBuffer *fb);

// F U N C T I O N S //////////////////////////////////////////////////

MSRAPI void MSR_DestroyRenderTarget( Uint32 id );
MSRAPI void PostProcessVertex(MSR_TransformedVertex *v_trans);
MSRAPI void RasterizeTriangleSolid(Uint32 thread_id, Uint32 face_idx, MSR_FragmentBuffer *frag_buffer, int tile_x, int tile_y, int tile_width, int tile_height);
MSRAPI void PrepareRasterizer();

#endif