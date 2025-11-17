//
//  BaseModel.hpp
//  ogl4
//
//  Created by Philipp Lensing on 19.09.16.
//  Copyright © 2016 Philipp Lensing. All rights reserved.
//

#ifndef BaseModel_hpp
#define BaseModel_hpp

#include <stdio.h>
#include <vector>

#include "camera.h"
#include "matrix.h"
#include "baseshader.h"
#include "Aabb.h"
#include <array>
#include <vector>

class BaseModel
{
public:
    BaseModel();
    virtual ~BaseModel();
    virtual void draw(const BaseCamera& Cam);
    const Matrix& transform() const { return Transform; }
    virtual void transform( const Matrix& m) { Transform = m; onTransformChanged();}
    virtual void shader( BaseShader* shader, bool deleteOnDestruction=false );
    virtual BaseShader* shader() const { return pShader; }
    virtual const AABB& boundingBox() const { return box; }
	bool shadowCaster() const { return ShadowCaster; }
	void shadowCaster(bool sc) { ShadowCaster = sc; }
	void setIsCollidable(bool c) { isCollidable = c; }
	bool getIsColldeable() { return isCollidable; }
	void setIsCollectable(bool c) { isCollectable = c; }
	bool getIsCollectable() { return isCollectable; }
	void setIsActive( bool a) { isActive = a; }
	bool getIsActive() { return isActive; }
	
	virtual void getTrianglesInAABB(const AABB& query, std::vector<std::array<Vector, 3>>& outTriangles) const {}
protected:
    Matrix Transform;
    BaseShader* pShader;
    bool DeleteShader;
	bool ShadowCaster;
	bool isCollidable = true;
	bool isCollectable = false;
	bool isActive = true;
	AABB box;

	virtual void onTransformChanged(){ box = box.transform(Transform); }
        
};


#endif /* BaseModel_hpp */
