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

#include <MSR.h>
#include <string>
#include <vector>

#ifndef MSR_MESH_3DS_H
#define MSR_MESH_3DS_H

#define MSR_OBJ_REVERSE_WINDING 0x1
#define MSR_OBJ_CALC_NORMALS 0x2

struct MSR_MeshMaterial
{
	Uint32 start_idx, end_idx;
	std::string name;
	MSR_Material mat;
	SDL_Surface *texture;
};

struct MSR_MeshObj 
{
	std::string name;

	Uint32 num_vertices;
	Uint32 num_indices;

	MSR_Vertex	*vertices;
	Uint32		*indices;

	std::vector<MSR_MeshMaterial*> materials;
};

struct MSR_Mesh
{
	std::string path;
	std::string name;

	std::vector<MSR_MeshObj> objects;
	std::vector<SDL_Surface*> textures;
};

extern MSR_Mesh *MSR_LoadMeshObj(const std::string &path, const std::string &filename, Uint32 flags=MSR_OBJ_REVERSE_WINDING);
extern void MSR_ReleaseMeshObj(MSR_Mesh *&mesh);

#endif