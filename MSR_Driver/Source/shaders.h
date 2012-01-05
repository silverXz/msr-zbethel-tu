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

#ifndef _SHADERS_H
#define _SHADERS_H

#include <SDL/SDL_image.h>
#include <MSR.h>
#include <MSR_Math.h>
#include <MSR_Shader.h>

extern SDL_Surface *shadow_map_depth;
extern float bias_amt;
extern MSR_Mat4x4 mLightMVP, mLightView, mLightProj;
extern void ShadowVertex(MSR_VShaderParameters *params);
extern void ShadowFragment(MSR_FShaderParameters *params);
extern void ColorVertex(MSR_VShaderParameters *params);
extern void ColorFragment(MSR_FShaderParameters *params);
	
#endif
