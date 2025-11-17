//
//  LineBoxModel.cpp
//  CGXcode
//
//  Created by Philipp Lensing on 10.10.16.
//  Copyright © 2016 Philipp Lensing. All rights reserved.
//

#include "LineBoxModel.h"

LineBoxModel::LineBoxModel( float Width, float Height, float Depth ) : Width(Width), Height(Height), Depth(Depth)
{
	// TODO: Add your code (Exercise 1)   
}

/**
* Aus Width, Height und Depth kann man die Verticies der Box ableiten.
* Da die Box im Ursprung liegt, kann man einfach alle Werte halbieren und dann die Werte je nach Verticy + oder - vergeben.
* Bsp. LineBoxModel(2, 3, 4)
* v0(-1, -1.5, -2), v1(1, -1.5, -2), v2(1, 1.5, -2) -> Triangle 1
* v0(-1, -1.5, -2), v2(1, 1.5, -2), v3(1, -1.5, -2) -> Triangle 2
* Die Verticies müssen immer im uhrzeigersinn angeordnet werden, damit die Normale immer nach außen zeigt.
*/

void LineBoxModel::draw(const BaseCamera& Cam)
{
    BaseModel::draw(Cam);

	//Vektoren
	Vector v0 = Vector(-Width / 2, -Height / 2, -Depth / 2);
	Vector v1 = Vector(-Width / 2, Height / 2, -Depth / 2);
	Vector v2 = Vector(Width / 2, Height / 2, -Depth / 2);
	Vector v3 = Vector(Width / 2, -Height / 2, -Depth / 2);
	Vector v4 = Vector(-Width / 2, -Height / 2, Depth / 2);
	Vector v5 = Vector(-Width / 2, Height / 2, Depth / 2);
	Vector v6 = Vector(Width / 2, Height / 2, Depth / 2);
	Vector v7 = Vector(Width / 2, -Height / 2, Depth / 2);

	VB.begin();
	
	VB.addVertex(v0);
	VB.addVertex(v1);

	VB.addVertex(v0);
	VB.addVertex(v3);

	VB.addVertex(v0);
	VB.addVertex(v4);

	VB.addVertex(v4);
	VB.addVertex(v5);

	VB.addVertex(v4);
	VB.addVertex(v7);

	VB.addVertex(v7);
	VB.addVertex(v3);

	VB.addVertex(v3);
	VB.addVertex(v2);

	VB.addVertex(v7);
	VB.addVertex(v6);

	VB.addVertex(v6);
	VB.addVertex(v2);

	VB.addVertex(v6);
	VB.addVertex(v5);

	VB.addVertex(v1);
	VB.addVertex(v2);

	VB.addVertex(v1);
	VB.addVertex(v5);

    VB.end();
  
    VB.activate();

    glDrawArrays(GL_LINES, 0, VB.vertexCount());

    VB.deactivate();
}
