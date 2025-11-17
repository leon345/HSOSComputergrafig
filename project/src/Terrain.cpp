#include "Terrain.h"
#include "rgbimage.h"
#include "Terrainshader.h"

#ifdef WIN32
#define ASSET_DIRECTORY "../../assets/"
#else
#define ASSET_DIRECTORY "../assets/"
#endif

Terrain::Terrain(const char* HeightMap, const char* DetailMap1, const char* DetailMap2, const char* DetailMap3, const char* DetailMap4) : Size(0.5f,1,0.5f)
{
    if(HeightMap && DetailMap1 && DetailMap2)
    {
        bool loaded = load( HeightMap, DetailMap1, DetailMap2, DetailMap3, DetailMap4);
        if(!loaded)
            throw std::exception();
    }
}

Terrain::~Terrain()
{
    
}

bool Terrain::load( const char* HeightMap, const char* DetailMap1, const char* DetailMap2, const char* DetailMap3, const char* DetailMap4)
{
    if( !HeightTex.load(HeightMap) )
        return false;
    if( !DetailTex[0].load(DetailMap1) )
        return false;
    if( !DetailTex[1].load(DetailMap2) )
        return false;
    if (!DetailTex[2].load(DetailMap3))
        return false;
    if (!DetailTex[3].load(DetailMap4))
        return false;
    
    // Normale Texturen laden oder Standard verwenden
    for(int i = 0; i < 4; ++i) {
        NoramlTex[i] = Texture();  // Standard-Normalmap
    }
    
    //std::cout << "NORMAL PATHHHH"<<path << std::endl;

    if( !NoramlTex[0].load(ASSET_DIRECTORY"aerial_rocks_02_2k/textures/aerial_rocks_02_nor_gl_2k.jpg"))
        return false;
    if( !NoramlTex[1].load(ASSET_DIRECTORY"rocks_ground_06_2k/rocks_ground_06_nor_gl_2k.jpg"))
        return false;
    
    if (!NoramlTex[2].load(ASSET_DIRECTORY"rocks_ground_06_2k/rocks_ground_06_nor_gl_2k.jpg"))
        return false;
    if (!NoramlTex[3].load(ASSET_DIRECTORY"snow_02_2k/textures/snow_02_nor_gl_2k.jpg"))
        return false;
  
    
    // Gauss
	RGBImage src = *HeightTex.getRGBImage();
	RGBImage filtered(src.width(), src.height());
	RGBImage::GaussFilter(filtered, src, 1.0f);

    // Wenn man neue mixmap braucht, dann uncomment
/*    
    RGBImage mix(src.width(), src.height());
    RGBImage::SobelFilter(mix, src, 1.0f);
    mix.saveToDisk(ASSET_DIRECTORY "mixtexture.bmp");
*/
    MixTex.load(ASSET_DIRECTORY "mixtexture.bmp");

    int width = filtered.width();
    int depth = filtered.height();
    m_Width = width;
    m_Depth = depth;

    //std::cout << "Heightmap width: " << width << " and depth: " << depth << "\n";

    // Verticies zwischenspeichern
    Vector** vertices = new Vector* [depth + 2];
    for (int i = 0; i < depth + 2; ++i) {
        vertices[i] = new Vector[width + 2];
    }
    
    // 1. Vertex Buffer bef�llen
    for (int z = 0; z < depth; ++z) {
        for (int x = 0; x < width; ++x) {
            Color color = filtered.getPixelColor(x, z);
            
            float gray = (0.299f * color.R + 0.587f * color.G + 0.114f * color.B);
            //std::cout << gray << "\n";
            float xCoord = this->width() * ((float)x - (float)width / 2);
            float zCoord = this->depth() * ((float)z - (float)depth / 2);
            Vector pos = Vector(xCoord, gray, zCoord);
            
            vertices[z + 1][x + 1] = Vector(xCoord, gray, zCoord);
        }
    }
    
    //std::cout << "Heightmap width: " << width << " and depth: " << depth << "\n";
    
    //Tagente und Bitangente
    int numVertices = width * depth;

    Vector* tangents = new Vector[numVertices];
    Vector* bitangents = new Vector[numVertices];
    int* tangentCounts = new int[numVertices];

    for (int i = 0; i < numVertices; ++i) {
        tangents[i] = Vector(0.0f, 0.0f, 0.0f);
        bitangents[i] = Vector(0.0f, 0.0f, 0.0f);
        tangentCounts[i] = 0;
    }

    for (int z = 0; z < depth - 1; ++z) {
        for (int x = 0; x < width - 1; ++x) {
            int v0 = z * width + x;
            int v1 = z * width + (x + 1);
            int v2 = (z + 1) * width + (x + 1);
            int v3 = (z + 1) * width + x;
            
            calculateTangentsForTriangle(vertices, v0, v2, v1, tangents, bitangents, 
                                       tangentCounts, width, depth);
            calculateTangentsForTriangle(vertices, v0, v3, v2, tangents, bitangents, 
                                       tangentCounts, width, depth);
        }
    }
    
    for (int i = 0; i < numVertices; ++i) {
        if (tangentCounts[i] > 0) {
            tangents[i] = tangents[i] * (1.0f / tangentCounts[i]);
            bitangents[i] = bitangents[i] * (1.0f / tangentCounts[i]);
            tangents[i].normalize();
            bitangents[i].normalize();
        }
    }
    
    
    
    VB.begin();
    
    for (int i = 0; i < depth; ++i) {
        for (int j = 0; j < width; ++j) {
            int x = i + 1;
            int y = j + 1;
            int index = i * width + j;
            Vector normal(0, 0, 0);

            Vector edge1, edge2;

            edge1 = vertices[x - 1][y] - vertices[x][y];
            edge2 = vertices[x - 1][y - 1] - vertices[x][y];
            normal += edge1.cross(edge2);

            edge1 = vertices[x - 1][y - 1] - vertices[x][y];
            edge2 = vertices[x][y - 1] - vertices[x][y];
            normal += edge1.cross(edge2);

            edge1 = vertices[x][y - 1] - vertices[x][y];
            edge2 = vertices[x + 1][y] - vertices[x][y];
            normal += edge1.cross(edge2);

            edge1 = vertices[x + 1][y] - vertices[x][y];
            edge2 = vertices[x + 1][y + 1] - vertices[x][y];
            normal += edge1.cross(edge2);

            edge1 = vertices[x + 1][y + 1] - vertices[x][y];
            edge2 = vertices[x][y + 1] - vertices[x][y];
            normal += edge1.cross(edge2);

            edge1 = vertices[x][y + 1] - vertices[x][y];
            edge2 = vertices[x - 1][y] - vertices[x][y];
            normal += edge1.cross(edge2);

            normal = normal.normalize();
   
            //std::cout << "Vector " << index << ": " << normal.X << " " << normal.Y << " " << normal.Z << "\n";
            float u = (float)j / depth;
            float v = (float)i / width;
            //std::cout << "u: " << u << " v: " << v << "\n";
            VB.addNormal(normal);

            VB.addTexcoord0(u, v);
            VB.addTexcoord1(u  , v );
            VB.addTexcoord2(tangents[index].X, tangents[index].Y, tangents[index].Z);
            VB.addTexcoord3(bitangents[index].X, bitangents[index].Y, bitangents[index].Z);
            
            VB.addVertex(vertices[x][y]);
        }
    }
    VB.end();

    for (int i = 0; i < depth + 2 ; ++i) {
        delete[](vertices[i]);
    }
    delete[]vertices;

    IB.begin();
    // 2. Indizes f�r Triangles:
    for (int z = 0; z < depth - 1; ++z) {
        for (int x = 0; x < width - 1; ++x) {

            int v0 = z * width + x;
            int v1 = z * width + (x + 1);
            int v2 = (z + 1) * width + (x + 1);
            int v3 = (z + 1) * width + x;

            // Dreieck 1
            IB.addIndex(v0);
            IB.addIndex(v2);
            IB.addIndex(v1);

            // Dreieck 2
            IB.addIndex(v0);
            IB.addIndex(v3);
            IB.addIndex(v2);
        }
    }
    IB.end();

    // terrain AABB
    
    if (VB.vertexCount() > 0)
    {
        const std::vector<Vector>& verts = VB.vertices();
        m_AABB.fromPoints(verts.data(), verts.size());
    } else
    {
        m_AABB = AABB::unitBox();
    }
    
    
    
    return true;
}

void Terrain::resizeTerrain(const float width, const float height, const float depth)
{
    size(Vector(width, height, depth));

    std::vector<Vector>& verts = VB.vertices();
    for (Vector& vertex : verts)
    {
        vertex.X *= width;
        vertex.Y *= height;
        vertex.Z *= depth;
    }

    recalculateNormals();

    m_AABB.fromPoints(verts.data(), verts.size());
}

// Von ChatGPT erzeugt.
void Terrain::recalculateNormals()
{
    const std::vector<Vector>& verts = VB.vertices();
    const std::vector<unsigned int>& inds = IB.indices();

    if (verts.empty() || inds.empty() || m_Width <= 0 || m_Depth <= 0) return;

    std::vector<Vector> normals = VB.normals(); // Getter muss Referenz zurückgeben
    normals.assign(verts.size(), Vector(0,0,0));

    for (size_t t = 0; t < inds.size()/3; ++t)
    {
        unsigned int i0 = inds[t*3+0];
        unsigned int i1 = inds[t*3+1];
        unsigned int i2 = inds[t*3+2];

        const Vector& a = verts[i0];
        const Vector& b = verts[i1];
        const Vector& c = verts[i2];

        Vector n = (b-a).cross(c-a).normalize();

        normals[i0] += n;
        normals[i1] += n;
        normals[i2] += n;
    }

    for (Vector& n : normals)
        n.normalize();
}


void Terrain::getTrianglesInAABB(const AABB& query, std::vector<std::array<Vector, 3>>& outTriangles) const
{
    const auto& verts = VB.vertices();
    const auto& inds  = IB.indices();

    if (inds.empty() || verts.empty()) return;

    if (!m_AABB.intersects(query))
        return;

    for (size_t t = 0; t < inds.size()/3; ++t)
    {
        const Vector& a = verts[inds[t*3+0]];
        const Vector& b = verts[inds[t*3+1]];
        const Vector& c = verts[inds[t*3+2]];

        Vector triMin(std::min({a.X,b.X,c.X}),
                      std::min({a.Y,b.Y,c.Y}),
                      std::min({a.Z,b.Z,c.Z}));
        Vector triMax(std::max({a.X,b.X,c.X}),
                      std::max({a.Y,b.Y,c.Y}),
                      std::max({a.Z,b.Z,c.Z}));

        if (!AABB(triMin, triMax).intersects(query))
            continue;

        outTriangles.push_back({a,b,c});
    }
}


void Terrain::shader( BaseShader* shader, bool deleteOnDestruction )
{
    BaseModel::shader(shader, deleteOnDestruction);
}

void Terrain::draw(const BaseCamera& Cam)
{
    applyShaderParameter();
    BaseModel::draw(Cam);
    // TODO: add code for drawing index and vertex buffers
    VB.activate();
    IB.activate();
    
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glDrawElements(GL_TRIANGLES, IB.indexCount(), IB.indexFormat(), 0);
    //glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    IB.deactivate();
    VB.deactivate();
}

void Terrain::applyShaderParameter()
{
    TerrainShader* Shader = dynamic_cast<TerrainShader*>(BaseModel::shader());
    if(!Shader)
        return;
    
    Shader->mixTex(&MixTex);
    for(int i=0; i<4; i++)
    {
        Shader->detailTex(i,&DetailTex[i]);
        if (NoramlTex[i].isValid())
        {
            Shader->normalTex(i, &NoramlTex[i]);    
        }
    }
        
    Shader->scaling(Size);
    
    // TODO: add additional parameters if needed..
}
//Quelle Lawonn, Kai. �Computer Graphics II  - Normal Mapping�. Uni Jena, o.�J. Zugegriffen 28. August 2025. https://vis.uni-jena.de/Lecture/ComputerGraphics2/Lec10_b_NormalMapping.pdf.
void Terrain::calculateTangentsForTriangle(Vector** vertices, int i0, int i1, int i2, Vector* tangents,
    Vector* bitangents, int* counts, int width, int height)
{
    // UV-Koordinatenn bestimmen 
    float u0 = (i0 % width) / (float)width;
    float v0 = (i0 / width) / (float)height;
    float u1 = (i1 % width) / (float)width;
    float v1 = (i1 / width) / (float)height;
    float u2 = (i2 % width) / (float)width;
    float v2 = (i2 / width) / (float)height;

    // Vertex-Positionen abrufen
    Vector pos0 = vertices[(i0 / width) + 1][(i0 % width) + 1];
    Vector pos1 = vertices[(i1 / width) + 1][(i1 % width) + 1];
    Vector pos2 = vertices[(i2 / width) + 1][(i2 % width) + 1];

    // Richtung Kante berechnen
    Vector deltaPos1 = pos1 - pos0;
    Vector deltaPos2 = pos2 - pos0;

    // UV-Differenzen berechnen
    float deltaU1 = u1 - u0;
    float deltaV1 = v1 - v0;
    float deltaU2 = u2 - u1;
    float deltaV2 = v2 - v1;

    // Inverse der UV-Matrix-Determinante
    float r = 1.0f / (deltaU1 * deltaV2 - deltaU2 * deltaV1);

    // Tangent und Bitangent berechnen
    Vector tangent = (deltaPos1 * deltaV2 - deltaPos2 * deltaV1) * r;
    Vector bitangent = (deltaPos2 * deltaU1 - deltaPos1 * deltaU2) * r;

    // Zu allen drei Vertices hinzuf�gen (akkumulieren)
    tangents[i0] = tangents[i0] + tangent;
    tangents[i1] = tangents[i1] + tangent;
    tangents[i2] = tangents[i2] + tangent;

    bitangents[i0] = bitangents[i0] + bitangent;
    bitangents[i1] = bitangents[i1] + bitangent;
    bitangents[i2] = bitangents[i2] + bitangent;

    // Z�hler f�r Mittelwertbildung erh�hen
    counts[i0]++;
    counts[i1]++;
    counts[i2]++;
}



