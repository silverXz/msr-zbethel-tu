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

#ifndef MSR_FRAGMENT_H
#define MSR_FRAGMENT_H

#include "MSR.h"

// D E F I N E S //////////////////////////////////////////////////////

// The maximum number of fragments that a single polygon can output on a given tile
#define MSR_FRAGMENT_POLYGON_MAX		512

// The size of each segment of the fragment buffer, and the maximum number of segments possible.
// Since there is the potential for all of the polygons in a tile to output fragments covering 
// the entire tile, the amount of data we would need to allocate for this worst case scenario for 
// all tiles is prohibitive. Instead, we will split it into segments and allocate segments of the 
// queue as needed, thereby allowing the memory usage to reflect the needs of the application better.
#define MSR_FRAGMENT_SEGMENT_SIZE		2048
#define MSR_FRAGMENT_SEGMENTS			12

#define MSR_FRAGMENT_STATE_TILE			0
#define MSR_FRAGMENT_STATE_BLOCK		1
#define MSR_FRAGMENT_STATE_BLOCK_MASK	2

// S T R U C T S //////////////////////////////////////////////////////

struct MSR_Fragment 
{
	Uint8  state;
	Uint8  thread_id;
	Uint32 face_idx;
	Uint32 x, y;
	Uint8  mask;
};

struct MSR_FragmentBuffer
{
	MSR_Fragment *buffer[MSR_FRAGMENT_SEGMENTS];
	int segments, elements;
	int cutoff;
};

inline void MSR_FragmentBufferResize( MSR_FragmentBuffer *fb )
{
	if( fb->elements >= fb->cutoff ) {
		
		// Allocate another segment if we have space
		if( fb->segments != MSR_FRAGMENT_SEGMENTS ) {
			fb->buffer[fb->segments++] = new MSR_Fragment[MSR_FRAGMENT_SEGMENT_SIZE];
			fb->cutoff += fb->segments * MSR_FRAGMENT_SEGMENT_SIZE;
		}
	}
}

inline MSR_Fragment *MSR_FragmentBufferGet( MSR_FragmentBuffer *fb, int idx )
{
	return &fb->buffer[idx / MSR_FRAGMENT_SEGMENT_SIZE][idx % MSR_FRAGMENT_SEGMENT_SIZE];
}

inline MSR_Fragment *MSR_FragmentBufferGetNext( MSR_FragmentBuffer *fb )
{
	MSR_Fragment *frag = &fb->buffer[fb->elements / MSR_FRAGMENT_SEGMENT_SIZE][fb->elements % MSR_FRAGMENT_SEGMENT_SIZE];
	fb->elements++;
	return frag;
}

inline void MSR_FragmentBufferInit( MSR_FragmentBuffer *fb )
{
	// Initialize the fragment buffer. We set the cutoff so that we will immediately resize
	// the buffer. The cutoff before resizing is an entire tile's worth of fragments. Since
	// we resize between polygons, this covers all cases.
	fb->cutoff = -MSR_FRAGMENT_POLYGON_MAX;
	fb->elements = fb->segments = 0;

	for( int i=0; i<MSR_FRAGMENT_SEGMENTS; i++ ) {
		fb->buffer[i] = NULL;
	}

	// Do the initial allocation
	MSR_FragmentBufferResize( fb );
}

inline void MSR_FragmentBufferDestroy( MSR_FragmentBuffer *fb )
{
	fb->elements = fb->cutoff = fb->segments = 0;

	for( int i=0; i<MSR_FRAGMENT_SEGMENTS; i++ ) {
		if( fb->buffer[i] ) {
			delete [] fb->buffer[i];
			fb->buffer[i] = NULL;
		}
	}
}

inline void MSR_FragmentBufferClear( MSR_FragmentBuffer *fb )
{
	fb->elements = 0;
}

#endif
