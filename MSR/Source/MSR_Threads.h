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

#ifndef MSR_THREADS_H
#define MSR_THREADS_H

#include "MSR.h"
#include "MSR_Render.h"

// D E F I N E S //////////////////////////////////////////////////////

#define THREAD_STATE_VERTEX		0
#define THREAD_STATE_RASTER		1

// F U N C T I O N   P R O T O T Y P E S //////////////////////////////

MSRAPI void ProcessTrianglesV( Uint32 thread_id );
MSRAPI void ProcessTrianglesR( Uint32 thread_id );
MSRAPI void ProcessFragments( Uint32 thread_id );

MSRAPI void MSR_InitWorkerThreads();
MSRAPI void MSR_DestroyWorkerThreads();

// S T R U C T U R E S ////////////////////////////////////////////////

struct MSR_ThreadRenderPartition {
	Uint32 start_index;
	Uint32 end_index;
};

struct MSR_ThreadRenderData {
	MSR_Vertex *vertices;
	Uint32 *indices;

	volatile Uint32 state;
	MSR_ThreadRenderPartition *partitions;
};

// G L O B A L S //////////////////////////////////////////////////////

// Tile information
extern Uint32 num_tiles_x;
extern Uint32 num_tiles_y;
extern MSR_Tile *tiles;

// Job queue
extern volatile Uint32 job_queue_start_rt;
extern volatile Uint32 job_queue_start_ft;
extern volatile Uint32 job_queue_end;

// Thread render data
extern MSR_ThreadRenderData *thread_render_data;
extern volatile Uint32 num_work_threads;
extern volatile Uint32 curr_threads_working;

extern SDL_cond *start_working_cond;
extern SDL_cond *end_working_cond;

extern SDL_mutex *end_working_lock;
extern SDL_mutex *threads_working_lock;

#endif