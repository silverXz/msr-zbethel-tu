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

#include "timer.h"

StopWatch g_timer;

double StopWatch::LIToSecs( LARGE_INTEGER & L) {
    return ((double)L.QuadPart /(double)frequency.QuadPart) ;
}

StopWatch::StopWatch(){
    timer.start.QuadPart=0;
    timer.stop.QuadPart=0; 
	time = 0.0;
	elapsed_time = 0.00001f;
    QueryPerformanceFrequency( &frequency ) ;
}

void StopWatch::startTimer( ) {
    QueryPerformanceCounter(&timer.start);
}

void StopWatch::pauseTimer()
{
	QueryPerformanceCounter(&timer.stop);
	time += _getElapsedTime();
	elapsed_time = time;
}

void StopWatch::stopTimer( ) {
    QueryPerformanceCounter(&timer.stop);
	elapsed_time = time + _getElapsedTime();
	time = 0.0;
}

double StopWatch::getElapsedTime()
{
	return elapsed_time;
}

double StopWatch::_getElapsedTime() {
    LARGE_INTEGER time;
    time.QuadPart = timer.stop.QuadPart - timer.start.QuadPart;
    return LIToSecs( time);
}
