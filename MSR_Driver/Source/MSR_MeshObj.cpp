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

#include "MSR_MeshObj.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <map>
#include <malloc.h>

using namespace std;

struct Vec2 {
	float x, y;
};

struct Vec3 {
	float x, y, z;
};

struct Vertex {
	Vec3 *pos;
	Vec3 *normal;
	Vec2 *tex;
};

struct FaceIdx {
	int idx[3];
};

struct Material
{
	string name;
	float ka[3];
	float kd[3];
	float ks[3];
	string map_kd;
};

struct FaceIdxCompare
{
	bool operator ()(const FaceIdx f1, const FaceIdx f2)
	{
		if( f1.idx[0] == f2.idx[0] ) {
			if( f1.idx[1] == f2.idx[1] ) {
				return f1.idx[2] < f2.idx[2];
			}
			return f1.idx[1] < f2.idx[1];
		}
		return f1.idx[0] < f2.idx[0];
	}
};

static vector<Vec3> positions;
static vector<Vec2> tex_coords;
static vector<Vec3> normals;
static vector<Vertex> vertices;
static vector<Uint32> indices;
static map<FaceIdx, Uint32, FaceIdxCompare> vert_map;
static map<string, Material> mat_map;
static map<string, SDL_Surface*> tex_map;

void AddMaterial(MSR_Mesh *mesh, MSR_MeshObj &obj, const string &path, const string &name)
{
	if( obj.materials.size() )
	{
		obj.materials[ obj.materials.size() - 1 ]->end_idx = indices.size() - 1;
	}

	MSR_MeshMaterial *mat = new MSR_MeshMaterial;
	memset( mat, 0, sizeof(MSR_MeshMaterial));
	mat->start_idx = indices.size();
	mat->name = name;
	
	map<string, Material>::iterator it = mat_map.find(name);
	if( it != mat_map.end() )
	{
		Material &libmat = it->second;
		mat->mat.ambient.r = libmat.ka[0];
		mat->mat.ambient.g = libmat.ka[1];
		mat->mat.ambient.b = libmat.ka[2];
		mat->mat.ambient.a = 1.0f;

		mat->mat.diffuse.r = libmat.kd[0];
		mat->mat.diffuse.g = libmat.kd[1];
		mat->mat.diffuse.b = libmat.kd[2];
		mat->mat.diffuse.a = 1.0f;

		mat->mat.specular.r = libmat.ks[0];
		mat->mat.specular.g = libmat.ks[1];
		mat->mat.specular.b = libmat.ks[2];
		mat->mat.specular.a = 1.0f;

		mat->mat.power = 1.0f;
		mat->mat.emissive = MSR_Color4(0.0f, 0.0f, 0.0f, 1.0f);
		
		map<string, SDL_Surface*>::iterator it2 = tex_map.find(libmat.map_kd);
		if( it2 != tex_map.end() )
		{
			mat->texture = it2->second;
		}
		else if( libmat.map_kd.size() )
		{
			mat->texture = SDL_LoadBMP( (path + libmat.map_kd).c_str() );
			mesh->textures.push_back(mat->texture);
			tex_map[libmat.map_kd] = mat->texture;
		}
	}

	obj.materials.push_back(mat);
}

void LoadMaterials(const string &filename)
{
	fstream file;
	file.open(filename, fstream::in);

	Material *cur_mat = NULL;

	while( true )
	{
		string line, token;
		stringstream ss(stringstream::in | stringstream::out);

		// Load in a line and tokenize
		if( !getline(file, line) ) break;
		ss << line;
		ss >> token;

		if( token == "newmtl" )
		{
			Material mat;
			ss >> mat.name;
			mat_map[mat.name] = mat;
			cur_mat = &mat_map[mat.name];
		}
		else if( token == "Ka" )
		{
			ss >> cur_mat->ka[0] >> cur_mat->ka[1] >> cur_mat->ka[2];
		}
		else if( token == "Kd" )
		{
			ss >> cur_mat->kd[0] >> cur_mat->kd[1] >> cur_mat->kd[2];
		}
		else if( token == "Ks" )
		{
			ss >> cur_mat->ks[0] >> cur_mat->ks[1] >> cur_mat->ks[2];
		}
		else if( token == "map_Kd" )
		{
			ss >> cur_mat->map_kd;
		}
	}

	file.close();
};

void ParseVertex(string &token)
{
	FaceIdx face;
	stringstream ss(token);
	string elem;
	for( int i=0; i<3; i++ ) 
		if( getline( ss, elem, '/' ) ) 
			face.idx[i] = atoi(elem.c_str());

	map<FaceIdx, Uint32, FaceIdxCompare>::iterator it = vert_map.find(face);
	if( it == vert_map.end() )
	{
		Vertex vert;
		vert.pos = &positions[face.idx[0]];
		vert.tex = &tex_coords[face.idx[1]];
		vert.normal = &normals[face.idx[2]];

		vert_map[face] = vertices.size();
		indices.push_back(vertices.size());

		vertices.push_back(vert);
	}
	else 
	{
		indices.push_back(it->second);
	}
}

MSR_Mesh *MSR_LoadMeshObj(const string &path, const string &filename, Uint32 flags)
{
	fstream file;
	file.open(path + filename, fstream::in);

	MSR_Mesh *mesh = new MSR_Mesh;

	string mtllib;
	mesh->name = filename;
	mesh->path = path;

	Vec2 t; t.x = 0.0f; t.y = 0.0f;
	Vec3 v; v.x = 0.0f; v.y = 0.0f; v.z = 0.0f;
	positions.push_back(v);
	normals.push_back(v);
	tex_coords.push_back(t);

	MSR_MeshObj obj;
	mesh->objects.push_back(obj);

	while( file.good() )
	{		
		string line, token;
		stringstream ss(stringstream::in | stringstream::out);

		// Load in a line and tokenize
		bool done = getline(file, line) == 0;
		ss << line;
		ss >> token;

		if( done || token == "o" )
		{
			// Is this the not the first object?
			if( positions.size() != 1 )
			{
				MSR_MeshObj *obj;

				obj = &mesh->objects[ mesh->objects.size() - 1 ];
				obj->indices = new Uint32[indices.size()];
				obj->vertices = (MSR_Vertex*)_aligned_malloc(vertices.size() * sizeof(MSR_Vertex), 16);
				obj->num_indices = indices.size();
				obj->num_vertices = vertices.size();
				obj->materials[ obj->materials.size() - 1 ]->end_idx = obj->num_indices - 1;

				for( Uint32 i=0; i<obj->num_vertices; i++ )
				{
					obj->vertices[i].p.x = vertices[i].pos->x;
					obj->vertices[i].p.y = vertices[i].pos->y;
					obj->vertices[i].p.z = vertices[i].pos->z;
					obj->vertices[i].p.w = 1.0f;

					if( flags & MSR_OBJ_CALC_NORMALS ) {
						obj->vertices[i].n = MSR_Vec4(0.0f,0.0f,0.0f,0.0f);
					} else {
						obj->vertices[i].n.x = vertices[i].normal->x;
						obj->vertices[i].n.y = vertices[i].normal->y;
						obj->vertices[i].n.z = vertices[i].normal->z;
				 		obj->vertices[i].n.w = 1.0f;
					}

					obj->vertices[i].u = vertices[i].tex->x;
					obj->vertices[i].v = vertices[i].tex->y;
					obj->vertices[i].c = MSR_Color4(1.0f,1.0f,1.0f,1.0f);
				}

				for( Uint32 i=0; i<obj->num_indices; i+=3 ) 
				{
					obj->indices[i + 1] = indices[i + 1];

					if( flags & MSR_OBJ_REVERSE_WINDING ) {
						obj->indices[i] = indices[i + 2];
						obj->indices[i + 2] = indices[i];
					} else {
						obj->indices[i + 2] = indices[i + 2];
						obj->indices[i] = indices[i];
					}

					if( flags & MSR_OBJ_CALC_NORMALS ) 
					{
						MSR_Vertex &v0 = obj->vertices[ obj->indices[i+0] ];
						MSR_Vertex &v1 = obj->vertices[ obj->indices[i+1] ];
						MSR_Vertex &v2 = obj->vertices[ obj->indices[i+2] ];

						MSR_Vec4 n;
						n.Cross( v2.p - v0.p, 
								 v1.p - v0.p );

						v0.n += n;
						v1.n += n;
						v2.n += n;
					}
				}

				if( flags & MSR_OBJ_CALC_NORMALS )
				{
					for( Uint32 i=0; i<obj->num_vertices; i++ ) 
					{
						reinterpret_cast<MSR_Vec3&>(obj->vertices[i].n).Normalize();
						obj->vertices[i].n.w = 1.0f;
					}
				}

				if( !done ) {
					mesh->objects.push_back(MSR_MeshObj());
					vertices.clear();
					indices.clear();
					vert_map.clear();
				}
			}

			if( !done ) ss >> mesh->objects[ mesh->objects.size() - 1 ].name;
			else
				break;
		}
		else if( token == "mtllib" )
		{
			ss >> mtllib;
			LoadMaterials(path + mtllib);
		}
		else if( token == "v" )
		{
			Vec3 v;
			ss >> v.x; ss >> v.y; ss >> v.z;
			positions.push_back(v);
		}
		else if( token == "vn" )
		{
			Vec3 vn;
			ss >> vn.x; ss >> vn.y; ss >> vn.z;
			normals.push_back(vn);
		}
		else if( token == "vt" )
		{
			Vec2 vt;
			ss >> vt.x; ss >> vt.y;
			tex_coords.push_back(vt);
		}
		else if( token == "f" )
		{
			ss >> token;
			ParseVertex(token);
			ss >> token;
			ParseVertex(token);
			ss >> token;
			ParseVertex(token);
		}
		else if( token == "usemtl" ) 
		{
			string name;
			ss >> name;
			AddMaterial(mesh, mesh->objects[ mesh->objects.size() - 1 ], path, name);
		}
	}

	file.close();

	positions.clear();
	normals.clear();
	tex_coords.clear();
	vertices.clear();
	indices.clear();
	vert_map.clear();
	mat_map.clear();
	tex_map.clear();

	return mesh;
}

void MSR_ReleaseMeshObj(MSR_Mesh *&mesh)
{
	for( Uint32 i=0; i<mesh->objects.size(); i++ )
	{
		if( mesh->objects[i].indices ) delete [] mesh->objects[i].indices;
		if( mesh->objects[i].vertices ) _aligned_free(mesh->objects[i].vertices);

		for( Uint32 j=0; j<mesh->objects[i].materials.size(); j++ )
		{
			delete mesh->objects[i].materials[j];
		}

		mesh->objects[i].materials.clear();
	}

	for( Uint32 i=0; i<mesh->textures.size(); i++ )
	{	
		SDL_FreeSurface(mesh->textures[i]);
	}

	mesh->textures.clear();
}