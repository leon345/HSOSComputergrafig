#ifndef Terrain_hpp
#define Terrain_hpp

#include <stdio.h>
#include "basemodel.h"
#include "texture.h"
#include "vertexbuffer.h"
#include "indexbuffer.h"
#include <array>

class Terrain : public BaseModel
{
public:
    Terrain(const char* HeightMap=NULL, const char* DetailMap1=NULL, const char* DetailMap2=NULL, const char* DetailMap3 = NULL, const char* DetailMap4 = NULL);
    virtual ~Terrain();
    bool load(const char* HeightMap, const char* DetailMap1, const char* DetailMap2, const char* DetailMap3, const char* DetailMap4);
    void resizeTerrain(float width, float height, float depth);

    virtual void shader( BaseShader* shader, bool deleteOnDestruction=false );
    virtual void draw(const BaseCamera& Cam);
    
    float width() const { return Size.X; }
    float height() const { return Size.Y; }
    float depth() const { return Size.Z; }

    void width(float v) { Size.X = v; }
    void height(float v) { Size.Y = v; }
    void depth(float v) { Size.Z = v; }
    
    const Vector& size() const { return Size; }
    void size(const Vector& s) { Size = s; }

    void getTrianglesInAABB(const AABB& query, std::vector<std::array<Vector, 3>>& outTriangles) const override;
    const AABB& boundingBox() const override { return m_AABB; }
protected:
    void applyShaderParameter();
    
    VertexBuffer VB;
    IndexBuffer IB;
    Texture DetailTex[4];
    Texture NoramlTex[4];
    Texture MixTex;
    Texture HeightTex;
    Vector Size;
    //Quelle: https://de.wikipedia.org/wiki/Grauwert
    void recalculateNormals();
    
    AABB m_AABB;

    int m_Width, m_Depth = 0;
    void calculateTangentsForTriangle(Vector** vertices, int i0, int i1, int i2, Vector* tangents, Vector* bitangents, int* counts, int width, int height);
private:

    

};



#endif /* Terrain_hpp */
