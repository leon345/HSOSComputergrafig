//
//  TriangleBoxModel.cpp
//  CGXcode
//
//  Created by Philipp Lensing on 10.10.16.
//  Copyright © 2016 Philipp Lensing. All rights reserved.
//

#include "TriangleBoxModel.h"

TriangleBoxModel::TriangleBoxModel(float Width, float Height, float Depth) : Width(Width), Height(Height), Depth(Depth)
{
	float x = Width * 0.5f;
	float y = Height * 0.5f;
	float z = Depth * 0.5f;
	box = AABB(Vector(-x,-y,-z), Vector(x,y,z));
}
/*
void TriangleBoxModel::onTransformChanged()
{
	box = box.transform(Transform);
}
*/

void TriangleBoxModel::draw(const BaseCamera& Cam)
{
	if (!getIsActive())
		return;
	
    BaseModel::draw(Cam);

	float w = Width * 0.5f;
	float h = Height * 0.5f;
	float d = Depth * 0.5f;

	struct Vertex {
		float x, y, z;
		float nx, ny, nz;
		float u, v;
	};
	
	Vertex verticies[]{

		//x,  y,  z, nx, ny, nz, u, v
		 
		// Back
		{-w, -h, -d,  0,  0, -1, 0, 1},
		{-w,  h, -d,  0,  0, -1, 0, 0},
		{ w,  h, -d,  0,  0, -1, 1, 0},
		{ w, -h, -d,  0,  0, -1, 1, 1},
		// Front
		{-w, -h,  d,  0,  0,  1, 0, 1},
		{-w,  h,  d,  0,  0,  1, 0, 0},
		{ w,  h,  d,  0,  0,  1, 1, 0},
		{ w, -h,  d,  0,  0,  1, 1, 1},
		// Left
		{-w, -h, -d, -1,  0,  0, 0, 1},
		{-w, -h,  d, -1,  0,  0, 1, 1},
		{-w,  h,  d, -1,  0,  0, 1, 0},
		{-w,  h, -d, -1,  0,  0, 0, 0},
		// Right
		{ w,  h, -d,  1,  0,  0, 1, 0},
		{ w,  h,  d,  1,  0,  0, 0, 0},
		{ w, -h,  d,  1,  0,  0, 0, 1},
		{ w, -h, -d,  1,  0,  0, 1, 1},
		// Top
		{-w,  h, -d,  0,  1,  0, 0, 0},
		{-w,  h,  d,  0,  1,  0, 0, 1},
		{ w,  h, -d,  0,  1,  0, 1, 0},
		{ w,  h,  d,  0,  1,  0, 1, 1},
		// Bottom
		{-w, -h, -d,  0, -1,  0, 0, 1},
		{ w, -h, -d,  0, -1,  0, 1, 1},
		{-w, -h,  d,  0, -1,  0, 0, 0},
		{ w, -h,  d,  0, -1,  0, 1, 0}
	};
	
	struct Index {
		unsigned int a, b, c;
	};

	Index indices[]{
		// Back
		{0, 1, 2},
		{2, 3, 0},
		// Front
		{5, 4, 6},
		{6, 4, 7},
		// Left
		{8, 9, 10},
		{10, 11, 8},
		// Right
		{12, 13, 14},
		{14, 15, 12},
		// Top
		{16, 17, 18},
		{18, 17, 19},
		// Bottom
		{20, 21, 22},
		{21, 23, 22}
	};
	

	VB.begin();

	for (Vertex& v : verticies)
	{
		VB.addNormal(v.nx, v.ny, v.nz);
		VB.addTexcoord0(v.u, v.v);
		VB.addVertex(v.x, v.y, v.z);
	}

	
	VB.end();

	
	IB.begin();

	for (Index& i : indices)
	{
		IB.addIndex(i.a);
		IB.addIndex(i.b);
		IB.addIndex(i.c);
	}
	
	IB.end();
	

	VB.activate();
	IB.activate();

	glDrawElements(GL_TRIANGLES, IB.indexCount(), IB.indexFormat(), 0);

	IB.deactivate();
	VB.deactivate();
}
