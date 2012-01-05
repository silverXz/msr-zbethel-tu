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

#include "AO.h"
#include <MSR_Math.h>
#include <fstream>
#include <iostream>
#include <ctime>
using namespace std;

int LoadAmbientOcclusion( MSR_Mesh *mesh )
{
	fstream file;
	file.open(mesh->path + mesh->name + ".ao", fstream::in);
	if( !file.is_open() ) return 1;

	int obj_idx = -1;
	int vert_idx = 0;

	string token;
	while( file >> token )
	{
		if( token[0] == 'o' )
		{
			obj_idx++;
		}	
		else
		{
			float a = atof( token.c_str() );
			mesh->objects[obj_idx].vertices[vert_idx].c = MSR_Vec4(a, a, a, a);
			vert_idx++;
		}
	}

	file.close();

	return 0;
}

float ElementShadow( MSR_Vec3 &v, float rSquared, MSR_Vec3 &receiverNormal,
					 MSR_Vec3 &emitterNormal, float emitterArea )
{
	return ( 1.0f - (1.0f / sqrtf( emitterArea /  rSquared + 1.0f ) ) ) * 
		min( 1.0f, max( 0.0f, emitterNormal.Dot(v) ) ) *
		min( 1.0f, max( 0.0f, 4.0f * receiverNormal.Dot(v) ) );
}

int CalcAmbientOcclusion( MSR_Mesh *mesh, Uint32 num_passes )
{
	fstream file;
	file.open(mesh->path + mesh->name + ".ao", fstream::out);
	if( !file.is_open() ) return 1;

	Uint32 pass = 0;
	cout << "Calculating ambient occlusion...\n";

	for( Uint32 i=0; i<mesh->objects.size(); i++ )
	{
		MSR_MeshObj &obj = mesh->objects[i];
		file << "\no";
		cout << "\tObject Name: " << obj.name << "\n";

		// Allocate space for vertex areas
		float *vArea = new float[obj.num_vertices];
		float *vAO = new float[obj.num_vertices];
		for( Uint32 j=0; j<obj.num_vertices; j++ )
		{
			vArea[j] = 0;
			vAO[j] = 1.0f;
		}

		// Loop through triangles and compute area
		for( Uint32 j=0; j<obj.num_indices; j+=3 )
		{
			MSR_Vec3 &v0 = reinterpret_cast<MSR_Vec3&>(obj.vertices[ obj.indices[j+0] ]);
			MSR_Vec3 &v1 = reinterpret_cast<MSR_Vec3&>(obj.vertices[ obj.indices[j+1] ]);
			MSR_Vec3 &v2 = reinterpret_cast<MSR_Vec3&>(obj.vertices[ obj.indices[j+2] ]);

			float a, b, c, s, area;
			a = (v1 - v0).Length(); 
			b = (v2 - v0).Length();
			c = (v2 - v1).Length();
			s = (a + b + c) / 2.0f;
		
			area = sqrtf( s*(s - a)*(s - b)*(s - c) ) / (3.0f);
			vArea[ obj.indices[j+0] ] += area;
			vArea[ obj.indices[j+1] ] += area;
			vArea[ obj.indices[j+2] ] += area;
		}

		for( Uint32 pass = 0; pass < num_passes; pass++ )
		{	
			cout << "\tComputing per vertex ambient terms... pass " << pass << "\n";

			// Loop through all vertices
			for( Uint32 j=0; j<obj.num_vertices; j++ )
			{
				float shadowTerm = 0.0f;

				// Loop again through all vertices
				for( Uint32 k=0; k<obj.num_vertices; k++ )
				{
					// Skip processing the vertex with itself
					if( j == k ) continue;

					MSR_Vec3 v = obj.vertices[k].p - obj.vertices[j].p;
					float rs = v.LengthSqr();
					if( rs < 0.00001f ) continue;
					v.Normalize();

					shadowTerm += ElementShadow(v, rs, 
												reinterpret_cast<MSR_Vec3&>(obj.vertices[j].n),
												reinterpret_cast<MSR_Vec3&>(obj.vertices[k].n),
												vArea[k] / MSR_PI) * obj.vertices[k].c.r;
				}

				vAO[j] = min( 1.0f, max( 0.0f, 1.0f - shadowTerm ) );

				if( pass == num_passes - 1 )
				{
					if( j % 3 == 0 ) file << "\n";
					file << vAO[j] << " ";
				}
			}

			// Loop through all vertices
			for( Uint32 j=0; j<obj.num_vertices; j++ )
			{
				obj.vertices[j].c = MSR_Color4(vAO[j], vAO[j], vAO[j], 1.0f);
			}
		}

		delete [] vArea;
		delete [] vAO;
	}
	

	file.close();

	return 0;
}