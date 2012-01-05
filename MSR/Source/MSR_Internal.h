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

#ifndef MSR_INTERNAL_H
#define MSR_INTERNAL_H

#include "MSR.h"
#include "MSR_Threads.h"
#include "MSR_Render.h"
#include <limits.h>

// D E F I N E S //////////////////////////////////////////////////////

#define CLIP_BUFFER_SIZE 2*6+1

MSRAPI void ProcessTrianglesV( Uint32 thread_id );
MSRAPI void ProcessTrianglesR( Uint32 thread_id );
MSRAPI void ProcessFragments( Uint32 thread_id );
MSRAPI void ClipTriangle( MSR_TransformedVertex *v, Uint32 thread_id );
MSRAPI void InsertTransformedTriangle(MSR_TransformedVertex *v0, MSR_TransformedVertex *v1, MSR_TransformedVertex *v2, Uint32 thread_id);

// G L O B A L S //////////////////////////////////////////////////////

// Max threads working
extern volatile Uint32 num_work_threads;

// Vertex buffer
extern MSR_TransformedVertex **vertex_buffer;
extern Uint32 *vertex_buffer_size;

// Face buffer
extern MSR_TransformedFace **face_buffer;

#endif
