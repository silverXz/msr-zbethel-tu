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

#include <iostream>
#include <MSR.h>
#include <MSR_Math.h>
#include <MSR_Shader.h>
#include "MSR_MeshOBJ.h"
#include "timer.h"
#include "shaders.h"
#include "AO.h"

#pragma comment(lib, "../Release/MSR.lib")
#pragma comment(lib, "../Release/MSR_Math.lib")

#define MEDIA_PATH "Media/"

SDL_Surface *shadow_map, *shadow_map_depth;
bool quitting = false;

using namespace std;

float bias_amt = 0.0015f;
float near_dist = 0.1f;
float far_dist = 100.0f;
float cam_radius = 10.0f;
float cam_theta = 0.0f;
float cam_phi = MSR_PI/8.0f;
float cam_theta_speed = MSR_PI/4.0f;
float cam_phi_speed = MSR_PI/4.0f;
float cam_at_height = 3.0f;

float light_theta = -MSR_PI/2.0f;

bool world_dirty = true;
bool draw_grid = false;

MSR_Vec3 mesh_scale;
Uint32 window_width, window_height;
Uint32 shadow_map_size = 1024;

MSR_Mesh *mesh;

MSR_Light l;

MSR_Mat4x4 mLightMVP, mLightView, mLightProj;

void setup_camera()
{
	MSR_Mat4x4 matProj;
	matProj.SetPerspectiveFovLH(MSR_PI/4.0f,(float)window_width/window_height,near_dist,far_dist);
	MSR_SetTransform(MSR_TRANSFORM_PROJECTION,matProj);
}

int setup_scene(const char *filename)
{
	mesh = MSR_LoadMeshObj(MEDIA_PATH, filename, MSR_OBJ_REVERSE_WINDING|MSR_OBJ_CALC_NORMALS);
	if( !mesh ) return 1;

	if( LoadAmbientOcclusion(mesh) != 0 )
	{
		CalcAmbientOcclusion(mesh, 1);
	}

	memset(&l, 0, sizeof(MSR_Light));
	l.ambient = MSR_Color4(0.8f,0.8f,0.8f,0.0f);
	l.diffuse = MSR_Color4(0.8f,0.8f,0.8f,0.0f);
	l.specular = MSR_Color4(1.0f,1.0f,1.0f,0.0f);
	l.type = MSR_LIGHT_TYPE_DIRECTIONAL;
	l.direction = MSR_Vec4(-1.0f,1.0f,0.0f);
	l.direction.Normalize();

	MSR_SetZBufferEnabled(true);
	MSR_SetCullMode(MSR_CULL_CCW);

	return 0;
}

void destroy_scene()
{
	MSR_ReleaseMeshObj(mesh);
}

void build_light_matrix( const MSR_Mat4x4 &mWorld )
{
	MSR_Vec3 cam_eye(-10.0f * sinf(light_theta), 18.0f, 10.0f * cosf(light_theta));

	l.direction = cam_eye;
	l.direction.Normalize();

	mLightView.SetLookAtLH(cam_eye,MSR_Vec3(0.0f,0.0f,0.0f),MSR_Vec3(0.0f,1.0f,0.0f));

	MSR_Mat4x4 mBias;
	mBias.SetIdentity();
	mBias._11 = mBias._22 = mBias._33 = mBias._41 = mBias._42 = mBias._43 = 0.5f;

	mLightProj.SetPerspectiveFovLH(MSR_PI/4.0f,1.0f,3.0f,30.0f);
	mLightMVP = mWorld * mLightView * mLightProj * mBias;
}

void render_mesh()
{
	MSR_Mat4x4 world;
	world.SetIdentity();
	world.Scale(mesh_scale.x,mesh_scale.y,mesh_scale.z);

	build_light_matrix(world);
	MSR_SetTransform(MSR_TRANSFORM_WORLD,world);

	for( Uint32 i=0; i<mesh->objects.size(); i++ )
	{
		if( mesh->objects[i].materials.size() )
		{
			for( Uint32 j=0; j<mesh->objects[i].materials.size(); j++ )
			{
				Uint32 start_idx = mesh->objects[i].materials[j]->start_idx;
				Uint32 range = mesh->objects[i].materials[j]->end_idx - mesh->objects[i].materials[j]->start_idx + 1;

				MSR_SetTexture( mesh->objects[i].materials[j]->texture );
				MSR_SetMaterial( &mesh->objects[i].materials[j]->mat );
				MSR_DrawTriangles(mesh->objects[i].vertices,mesh->objects[i].num_vertices,
								  &mesh->objects[i].indices[ start_idx ], range);
			}
		}
		else
		{
			MSR_DrawTriangles(mesh->objects[i].vertices,mesh->objects[i].num_vertices,mesh->objects[i].indices,mesh->objects[i].num_indices);
		}
	}
}

void handle_input(float elapsed_time)
{
	Uint8 *keys = SDL_GetKeyState(NULL);

	cam_theta += cam_theta_speed * 0.25f * elapsed_time;

	if( keys[SDLK_RIGHT] ) {
		cam_theta += cam_theta_speed * elapsed_time;
		if( cam_theta >  (2.0f * MSR_PI) ) cam_theta -= 2.0f * MSR_PI;
	}
	if( keys[SDLK_LEFT] ) {
		cam_theta -= cam_theta_speed * elapsed_time;
		if( cam_theta < -(2.0f * MSR_PI) ) cam_theta += 2.0f * MSR_PI;
	}
	if( keys[SDLK_UP] ) {
		cam_phi += cam_phi_speed * elapsed_time;
		cam_phi = min( cam_phi, MSR_PI/2.0f - 0.1f );
	}
	if( keys[SDLK_DOWN] ) {
		cam_phi -= cam_phi_speed * elapsed_time;
		cam_phi = max( cam_phi, -MSR_PI/2.0f + 0.1f );
	}

	if( keys[SDLK_PAGEUP] ) {
		cam_radius += 2.0f * elapsed_time;
		cam_radius = min(cam_radius, 50.0f);
	}

	if( keys[SDLK_PAGEDOWN] ) {
		cam_radius -= 2.0f * elapsed_time;
		cam_radius = max(cam_radius, 1.0f);
	}

	if( keys[SDLK_l] ) {
		world_dirty = true;
		light_theta += cam_theta_speed * elapsed_time;
		if( light_theta >  (2.0f * MSR_PI) ) light_theta -= 2.0f * MSR_PI;
	}
	if( keys[SDLK_k] ) {
		world_dirty = true;
		light_theta -= cam_theta_speed * elapsed_time;
		if( light_theta < -(2.0f * MSR_PI) ) light_theta += 2.0f * MSR_PI;
	}
	
	if( keys[SDLK_u] ) {
		bias_amt += 0.005f * elapsed_time;
	}

	if( keys[SDLK_y] ) {
		bias_amt -= 0.005f * elapsed_time;
	}

	if( keys[SDLK_a] ) {
		cam_at_height += elapsed_time;
	}

	if( keys[SDLK_z] ) {
		cam_at_height -= elapsed_time;
	}

	if( keys[SDLK_g] ) {
		draw_grid = !draw_grid;
	}

	if( keys[SDLK_ESCAPE] )
		quitting = true;
}

void update_camera() 
{
	MSR_Mat4x4 result, rotY, rotX;
	rotY.SetRotateY(cam_theta);
	rotX.SetRotateX(cam_phi);
	result = rotX * rotY;

	MSR_Vec4 cam_at(0.0f,cam_at_height,0.0f);
	MSR_Vec4 cam_eye(0.0f,0.0f,cam_radius);
	cam_eye = result * cam_eye;

	MSR_Mat4x4 matView;
	matView.SetLookAtLH(cam_eye + cam_at, cam_at, MSR_Vec4(0.0f,1.0f,0.0f));
	MSR_SetTransform(MSR_TRANSFORM_VIEW,matView);
}

int main( int argc, char* argv[] ) 
{ 
	if( argc < 5 ) return 0;
	window_width = atoi(argv[1]);
	window_height = atoi(argv[2]);
	int num_threads = atoi(argv[3]);
	if( num_threads < 1 || window_width < 1 || window_height < 1 ) return 0;

	if( SDL_Init(SDL_INIT_VIDEO) != 0 ) return 1;

	SDL_Surface *screen = SDL_SetVideoMode(window_width,window_height,0,SDL_SWSURFACE);
	if( screen == NULL ) return 2;

	if( argc >= 9 ) shadow_map_size = atoi( argv[8] );
	shadow_map = SDL_CreateRGBSurface(SDL_SWSURFACE, shadow_map_size, shadow_map_size, 16, 0, 0, 0, 0);

	if( MSR_Init(screen, MSR_INIT_ZBUFFER, num_threads ) != 0 ) return 4;

	Uint32 shadow_map_id = 0;
	MSR_CreateRenderTarget(shadow_map, MSR_INIT_ZBUFFER, &shadow_map_id);
	if( shadow_map_id == 0 ) return 5;

	if( setup_scene(argv[4]) != 0 ) return 6;

	if( argc >= 8 ) {
		mesh_scale.x = atof(argv[5]);
		mesh_scale.y = atof(argv[6]);
		mesh_scale.z = atof(argv[7]);
	} else {
		mesh_scale = MSR_Vec3( 1.0f, 1.0f, 1.0f );
	}

	bool first_frame = true;
	float fps = 0.0f;
	float t = 0.0f;
	int frames = 0;

	StopWatch sw;
	sw.startTimer();

	setup_camera();
	MSR_SetLightEnabled(0, true);

	while(!quitting) 
	{
		sw.stopTimer();
		sw.startTimer();
		
		SDL_Event evt;
		while( SDL_PollEvent(&evt) )
		{
			if( evt.type == SDL_QUIT) {
				quitting = true;
				break;
			}
		}	

		MSR_SetLight(&l, 0);
		handle_input(sw.getElapsedTime());
		update_camera();

		if( !first_frame && world_dirty )
		{
			MSR_SetBackBufferEnabled(false);
			MSR_SetRenderTarget(shadow_map_id);
			MSR_BeginScene();

			MSR_Clear(MSR_CLEAR_ZBUFFER,0);

			MSR_SetNumVaryings(0);
			MSR_SetVertexShader(ShadowVertex);
			MSR_SetFragmentShader(ShadowFragment);
		
			MSR_SetTransform(MSR_TRANSFORM_VIEW, mLightView);
			MSR_SetTransform(MSR_TRANSFORM_PROJECTION, mLightProj);
	
			render_mesh();
			MSR_EndScene();
			MSR_SetBackBufferEnabled(true);

			world_dirty = false;
		}
		
		setup_camera();
		update_camera();

		MSR_GetRenderTargetDepth(shadow_map_id, &shadow_map_depth);

		MSR_SetRenderTarget(MSR_DEFAULT_RENDER_TARGET);
		MSR_BeginScene();
		MSR_Clear(MSR_CLEAR_TARGET|MSR_CLEAR_ZBUFFER,0x00101f);

		MSR_SetNumVaryings(12);
		MSR_SetVertexShader(ColorVertex);
		MSR_SetFragmentShader(ColorFragment);

		render_mesh();

		MSR_EndScene();
		if( !first_frame ) MSR_Present();

		frames++;
		fps += 1.0 / sw.getElapsedTime();
		t += sw.getElapsedTime();
		if( t >= 1.0f )
		{
			if( first_frame ) 
				first_frame = false;
			else 
				cout << "FPS: " << fps / frames << "\n";

			frames = 0;
			fps = 0.0f;
			t = 0.0f;
		}
	}

	destroy_scene();

	SDL_FreeSurface(shadow_map);

	MSR_Quit();
	SDL_Quit();

	return 0; 
}