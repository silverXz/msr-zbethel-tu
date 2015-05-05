This project is a multithreaded software rasterizer built on SDL. It was written as part of a year-long research requirement at Taylor University. Supported features include custom vertex and pixel shaders, a quad-filled pixel pipeline heavily optimized for SSE, polygon clipping, half-space rasterization, depth occlusion with z-buffering, as well as perspective correct attribute interpolation. An custom written, SSE optimized math library is included as well.

Performance scales very well to then number of physical threads on the CPU. A dual core, Hyperthreaded Intel I5 saw a 2.5x speedup from 1 thread to 4 threads. A core I7 saw a 4x+ speedup from 1 to 8 threads. SSE 4.1 is utilized, so a recent CPU is required.

To contact me with questions or comments, email me at zach.bethel AT gmail DOT com.

To read the research paper associated with this code, visit this link:

https://docs.google.com/file/d/0BwqDw_AVYMl-TUk4SEZ0MmduRFNPWWNzZ0VwZlVfQVRaX3E4/edit

To download the executable demo, visit this link (keep in mind that your CPU needs to support SSE 4.1):

http://cse.taylor.edu/~zbethel/MSR/MSR_Release.zip