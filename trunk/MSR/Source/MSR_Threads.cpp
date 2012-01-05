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

#include "MSR_Threads.h"
#include <time.h>

MSR_ThreadRenderData *thread_render_data;

extern Uint32 *vertex_buffer_size;

SDL_cond *start_working_cond;
SDL_cond *end_working_cond;

SDL_mutex *end_working_lock;
SDL_mutex *threads_working_lock;

volatile Uint32 curr_threads_working;

static SDL_Thread **threads;

//
// This is the worker function
//

int MSR_WorkFunc(void *data) 
{
	Uint32 thread_id = (Uint32)data;

	// This is the end state for all work threads (i.e. when a job is done, all threads go to sleep once they finish the pixel processing).
	// Coincidentally, all work threads are set to this mode on startup, because it is convenient to wake them up by just signaling that vertex
	// processing has begun.
	SDL_mutexP(end_working_lock);
	while( THREAD_STATE_RASTER == thread_render_data->state ) SDL_CondWait(start_working_cond,end_working_lock);
	SDL_mutexV(end_working_lock);

	while(true) {

		// We have just been signaled to begin the vertex processing stage.
		vertex_buffer_size[thread_id] = 0;
		ProcessTrianglesV( thread_id );

		// We finished vertex processing, so do an atomic decrement on the number of working threads. 
		// If we are the last one, prepare the job queue and signal to begin pixel processing.
		Uint32 new_value = InterlockedDecrement(&curr_threads_working);
		if( !new_value ) { 

			// Signal other threads to begin rasterization
			InterlockedExchange(&curr_threads_working, num_work_threads);
			thread_render_data->state = THREAD_STATE_RASTER;
			
		} else
			while( THREAD_STATE_VERTEX == thread_render_data->state ) Sleep(0);

		// Process pixel pipeline on tiles
		ProcessTrianglesR( thread_id );

		// Process our fragment data
		ProcessFragments( thread_id );

		InterlockedDecrement(&curr_threads_working);

		// We have finished. Since the job is done, go to sleep until the master thread wakes us up again for the next job.
		SDL_mutexP(end_working_lock);
		while( THREAD_STATE_RASTER == thread_render_data->state ) SDL_CondWait(start_working_cond,end_working_lock);
		SDL_mutexV(end_working_lock);
	}

	return 0;
}

void MSR_InitWorkerThreads() 
{
	thread_render_data = new MSR_ThreadRenderData;
	thread_render_data->vertices = NULL;
	thread_render_data->indices = NULL;
	thread_render_data->partitions = new MSR_ThreadRenderPartition[num_work_threads];
	thread_render_data->state = THREAD_STATE_RASTER;
	
	curr_threads_working = 0;

	ZeroMemory(thread_render_data->partitions,num_work_threads * sizeof(MSR_ThreadRenderPartition));

	if( num_work_threads != 1 ) {
	
		start_working_cond = SDL_CreateCond();
		end_working_cond = SDL_CreateCond();
		end_working_lock = SDL_CreateMutex();
		threads_working_lock = SDL_CreateMutex();

		threads = new SDL_Thread*[num_work_threads-1];
		for( Uint32 t=0;t<num_work_threads-1; t++ ) 
			threads[t] = SDL_CreateThread(MSR_WorkFunc,(void*)(t+1));
	}
}

void MSR_DestroyWorkerThreads() 
{
	if( thread_render_data ) {
		if( thread_render_data->partitions ) {
			delete [] thread_render_data->partitions;
		}
		delete thread_render_data;
	}
		
	if( num_work_threads != 1 ) {
		for( Uint32 t=0;t<num_work_threads-1; t++ )
			SDL_KillThread(threads[t]);

		if( threads ) delete [] threads;
		SDL_DestroyMutex(threads_working_lock);
		SDL_DestroyMutex(end_working_lock);
		SDL_DestroyCond(start_working_cond);
		SDL_DestroyCond(end_working_cond);
	}
}