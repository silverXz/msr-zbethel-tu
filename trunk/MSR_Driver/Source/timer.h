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

#include <windows.h>
#include <iostream>

class StopWatch;

extern StopWatch g_timer;

#define BEGIN_PROFILE() { g_timer.startTimer(); }
#define END_PROFILE(tag) { g_timer.stopTimer(); std::cout << tag << ": " << g_timer.getElapsedTime()*1000.0 << "ms " << 1.0f/(g_timer.getElapsedTime()) << "\n"; }


typedef struct {
    LARGE_INTEGER start;
    LARGE_INTEGER stop;
} stopWatch;

class StopWatch 
{
private:
    stopWatch timer;
    LARGE_INTEGER frequency;
	double time, elapsed_time;
    double LIToSecs( LARGE_INTEGER & L) ;
public:
    StopWatch() ;
    void startTimer();
	void pauseTimer();
    void stopTimer();
    double getElapsedTime();

private:
	 double _getElapsedTime();
};
