//
//  Model.cpp
//  ogl4
//
//  Created by Philipp Lensing on 21.09.16.
//  Copyright © 2016 Philipp Lensing. All rights reserved.
//

#include "Model.h"
#include "phongshader.h"
#include <list>
#include <__msvc_filebuf.hpp>
using namespace std;

Model::Model() : pMeshes(NULL), MeshCount(0), pMaterials(NULL), MaterialCount(0)
{
    
}
Model::Model(const char* ModelFile, bool FitSize) : pMeshes(NULL), MeshCount(0), pMaterials(NULL), MaterialCount(0)
{
    bool ret = load(ModelFile, FitSize);
    if(!ret)
        throw std::exception();
}
Model::~Model()
{
    if (pMeshes != nullptr) {
        delete[] pMeshes;
    }
    delete[] pMaterials;

    deleteNodes(&RootNode);
}

void Model::deleteNodes(Node* pNode)
{
    if(!pNode)
        return;
    for(unsigned int i=0; i<pNode->ChildCount; ++i)
        deleteNodes(&(pNode->Children[i]));
    if(pNode->ChildCount>0)
        delete [] pNode->Children;
    if(pNode->MeshCount>0)
        delete [] pNode->Meshes;
}

bool Model::load(const char* ModelFile, bool FitSize)
{
    const aiScene* pScene = aiImportFile( ModelFile,aiProcessPreset_TargetRealtime_Fast | aiProcess_TransformUVCoords );
    
    if(pScene==NULL || pScene->mNumMeshes<=0)
        return false;
    
    Filepath = ModelFile;
    Path = Filepath;
    size_t pos = Filepath.rfind('/');
    if(pos == std::string::npos)
        pos = Filepath.rfind('\\');
    if(pos !=std::string::npos)
        Path.resize(pos+1);
    
    loadMeshes(pScene, FitSize);
    loadMaterials(pScene);
    loadNodes(pScene);
    
    return true;
}

void Model::loadMeshes(const aiScene* pScene, bool FitSize)
{
    // Lösche vorhandene Mesh-Daten, falls existiert
    if (pMeshes != nullptr) {
        delete[] pMeshes;
        pMeshes = nullptr;
        MeshCount = 0;
    }
    MeshCount = pScene->mNumMeshes;
    pMeshes = new Mesh[pScene->mNumMeshes];
    
    for (size_t i = 0; i < MeshCount; ++i)
    {
        aiMesh* mesh = pScene->mMeshes[i];
        pMeshes[i].MaterialIdx = mesh->mMaterialIndex;
        
        pMeshes[i].VB.begin();
        // ob das aktuelle Mesh UV-Texturkoordinaten enthält
        if (mesh->HasTextureCoords(0)) { 
            for (size_t j = 0; j < mesh->mNumVertices; ++j) {
                const aiVector3D& uvTexturkoordinat = mesh->mTextureCoords[0][j];
                pMeshes[i].VB.addTexcoord0(uvTexturkoordinat.x, 1-uvTexturkoordinat.y);
            }
        }
        // Tangenten im Vertexbuffer reinschreiben
        if (mesh->HasTangentsAndBitangents()) {
            for (size_t j = 0; j < mesh->mNumVertices; ++j) {
                const aiVector3D& tangents = mesh->mTangents[j];
                const aiVector3D& bitangents = mesh->mBitangents[j];
                pMeshes[i].VB.addTexcoord1(tangents.x, 1 - tangents.y, tangents.z);
                pMeshes[i].VB.addTexcoord2(bitangents.x, 1 - bitangents.y, bitangents.z);
            }
        }
        // ob das aktuelle Mesh Normalen enthält
        if (mesh->HasNormals()) {
            for (size_t j = 0; j < mesh->mNumVertices; ++j) {
                const aiVector3D& normalMesh = mesh->mNormals[j];
                pMeshes[i].VB.addNormal(normalMesh.x, normalMesh.y, normalMesh.z);
            }
        }
        // ob aktuelle Mesh Positionsdaten enthält
        if (mesh->HasPositions()) {
            for (size_t j = 0; j < mesh->mNumVertices; ++j) {
                const aiVector3D& positionMesh = mesh->mVertices[j];
                size_t size = 0;
                if (FitSize){
                    size = 5;
                    pMeshes[i].VB.addVertex(positionMesh.x * size, positionMesh.y * size, positionMesh.z * size);
                }
                // skalierten Positionen des aktuellen Vertex dem Vertex-Buffer VB des aktuellen Meshes pMeshes[i] hinzugefügt.
                else {
                    pMeshes[i].VB.addVertex(positionMesh.x, positionMesh.y, positionMesh.z);
                }
            }
        }
        pMeshes[i].VB.end();
        
        pMeshes[i].IB.begin();
        for (size_t j = 0; j < mesh->mNumFaces; ++j) {
            const aiFace& faceMesh = mesh->mFaces[j];
            for (size_t x = 0; x < faceMesh.mNumIndices; ++x) {
                pMeshes[i].IB.addIndex(faceMesh.mIndices[x]);
            }
        }
        pMeshes[i].IB.end();
    }
    this->calcBoundingBox(pScene, LocalBoundingBox);
}

void Model::loadMaterials(const aiScene* pScene)
{
    if (pMaterials) {
        delete[] pMaterials;
        pMaterials = nullptr;
        MaterialCount = 0;
    }

    MaterialCount = pScene->mNumMaterials;
    pMaterials = new Material[MaterialCount];

    for (size_t materialIndex = 0; materialIndex < MaterialCount; materialIndex++) {
        aiMaterial* modelMaterial = pScene->mMaterials[materialIndex];
        
        aiColor3D color;
        modelMaterial->Get(AI_MATKEY_COLOR_SPECULAR, color);
        pMaterials[materialIndex].SpecColor = Color(color.r, color.g, color.b);

        modelMaterial->Get(AI_MATKEY_COLOR_DIFFUSE, color);
        pMaterials[materialIndex].DiffColor = Color(color.r, color.g, color.b);

        modelMaterial->Get(AI_MATKEY_COLOR_AMBIENT, color);
        pMaterials[materialIndex].AmbColor = Color(color.r, color.g, color.b);

        float shininess = 0.0f;
        modelMaterial->Get(AI_MATKEY_SHININESS, shininess);
        pMaterials[materialIndex].SpecExp = shininess;
        
        aiString diffusePath;
        if (modelMaterial->GetTextureCount(aiTextureType_DIFFUSE) > 0 &&
            modelMaterial->GetTexture(aiTextureType_DIFFUSE, 0, &diffusePath) == AI_SUCCESS) 
        {
            std::string fullPath = Path + diffusePath.C_Str();
            const Texture* modelTexture = Texture::LoadShared(fullPath.c_str());
            if (modelTexture) {
                pMaterials[materialIndex].DiffTex = modelTexture;
            } else {
                std::cout << "Diffuse-Textur konnte nicht geladen werden: " << fullPath << "\n";
            }
            
            aiString normalPath;
            if (modelMaterial->GetTextureCount(aiTextureType_NORMALS) > 0 &&
                modelMaterial->GetTexture(aiTextureType_NORMALS, 0, &normalPath) == AI_SUCCESS) 
            {
                std::string fullNormalPath = Path + normalPath.C_Str();
                const Texture* normalTexture = Texture::LoadShared(fullNormalPath.c_str());
                if (normalTexture) {
                    pMaterials[materialIndex].Normal = normalTexture;
                } else {
                    std::cout << "Normalmap konnte nicht geladen werden: " << fullNormalPath << "\n";
                }
            } 
            else {
                std::string temp = diffusePath.C_Str();
                size_t dot = temp.find_last_of('.');
                if (dot != std::string::npos) {
                    std::string guess = Path + temp.substr(0, dot) + "_n.DDS";
                    const Texture* normalTexture = Texture::LoadShared(guess.c_str());
                    if (normalTexture) {
                        pMaterials[materialIndex].Normal = normalTexture;
                    } else {
                        std::cout << "Normalmap-Fallback nicht gefunden: " << guess << "\n";
                    }
                }
            }
        }
    }
}
void Model::calcBoundingBox(const aiScene* pScene, AABB& Box)
{ 
    // Initialisiere die minimalen und maximalen Ecken der Bounding Box
    Box.Min = Vector(numeric_limits<float>::max(), numeric_limits<float>::max(), numeric_limits<float>::max());
    Box.Max = Vector(numeric_limits<float>::lowest(), numeric_limits<float>::lowest(), numeric_limits<float>::lowest());

    // Iteriere über alle Meshes des Modells
    for (size_t i = 0; i < pScene->mNumMeshes; ++i)
    {
        aiMesh* mesh = pScene->mMeshes[i];
        for (size_t j = 0; j < mesh->mNumVertices; ++j)
        {
            aiVector3D vertex = mesh->mVertices[j];
            // Aktualisiere die minimalen und maximalen Koordinaten der Bounding Box
            Box.Min.X = min(Box.Min.X, vertex.x);
            Box.Min.Y = min(Box.Min.Y, vertex.y);
            Box.Min.Z = min(Box.Min.Z, vertex.z);
            Box.Max.X = max(Box.Max.X, vertex.x);
            Box.Max.Y = max(Box.Max.Y, vertex.y);
            Box.Max.Z = max(Box.Max.Z, vertex.z);
        }
    }
}

void Model::loadNodes(const aiScene* pScene)
{
    deleteNodes(&RootNode);
    copyNodesRecursive(pScene->mRootNode, &RootNode);
}

void Model::copyNodesRecursive(const aiNode* paiNode, Node* pNode)
{
    pNode->Name = paiNode->mName.C_Str();
    pNode->Trans = convert(paiNode->mTransformation);
    
    if(paiNode->mNumMeshes > 0)
    {
        pNode->MeshCount = paiNode->mNumMeshes;
        pNode->Meshes = new int[pNode->MeshCount];
        for(unsigned int i=0; i<pNode->MeshCount; ++i)
            pNode->Meshes[i] = (int)paiNode->mMeshes[i];
    }
    
    if(paiNode->mNumChildren <=0)
        return;
    
    pNode->ChildCount = paiNode->mNumChildren;
    pNode->Children = new Node[pNode->ChildCount];
    for(unsigned int i=0; i<paiNode->mNumChildren; ++i)
    {
        copyNodesRecursive(paiNode->mChildren[i], &(pNode->Children[i]));
        pNode->Children[i].Parent = pNode;
    }
}

void Model::applyMaterial( unsigned int index)
{
    if(index>=MaterialCount)
        return;
    
    PhongShader* pPhong = dynamic_cast<PhongShader*>(shader());
    if(!pPhong) {
        //std::cout << "Model::applyMaterial(): WARNING Invalid shader-type. Please apply PhongShader for rendering models.\n";
        return;
    }
    
    Material* pMat = &pMaterials[index];
    pPhong->ambientColor(pMat->AmbColor);
    pPhong->diffuseColor(pMat->DiffColor);
    pPhong->specularExp(pMat->SpecExp);
    pPhong->specularColor(pMat->SpecColor);
    pPhong->diffuseTexture(pMat->DiffTex);

    pPhong->normalTexture(pMat->Normal);
}

void Model::draw(const BaseCamera& Cam)
{
    if(!pShader) {
        std::cout << "BaseModel::draw() no shader found" << std::endl;
        return;
    }
    if (!getIsActive()) { return; }
    
    pShader->modelTransform(transform());
    
    std::list<Node*> DrawNodes;
    DrawNodes.push_back(&RootNode);
    
    while(!DrawNodes.empty())
    {
        Node* pNode = DrawNodes.front();
        Matrix GlobalTransform;
        
        if(pNode->Parent != NULL)
            pNode->GlobalTrans = pNode->Parent->GlobalTrans * pNode->Trans;
        else
            pNode->GlobalTrans = transform() * pNode->Trans;
        
        pShader->modelTransform(pNode->GlobalTrans);
    
        for(unsigned int i = 0; i<pNode->MeshCount; ++i )
        {
            Mesh& mesh = pMeshes[pNode->Meshes[i]];
            mesh.VB.activate();
            mesh.IB.activate();
            applyMaterial(mesh.MaterialIdx);
            pShader->activate(Cam);
            glDrawElements(GL_TRIANGLES, mesh.IB.indexCount(), mesh.IB.indexFormat(), 0);
            mesh.IB.deactivate();
            mesh.VB.deactivate();
        }
        for(unsigned int i = 0; i<pNode->ChildCount; ++i )
            DrawNodes.push_back(&(pNode->Children[i]));
        
        DrawNodes.pop_front();
    }
}

Matrix Model::convert(const aiMatrix4x4& m)
{
    return Matrix(m.a1, m.a2, m.a3, m.a4,
                  m.b1, m.b2, m.b3, m.b4,
                  m.c1, m.c2, m.c3, m.c4,
                  m.d1, m.d2, m.d3, m.d4);
}
/*
void Model::computeLocalBoundingBoxFromMeshes()
{
    BoundingBox.Min = Vector(INFINITY, INFINITY, INFINITY);
    BoundingBox.Max = Vector(-INFINITY, -INFINITY, -INFINITY);

    for (unsigned int m = 0; m < MeshCount; ++m)
    {
        const auto& verts = pMeshes[m].VB.vertices();
        for (const Vector& v : verts)
        {
            if (v.X < BoundingBox.Min.X) BoundingBox.Min.X = v.X;
            if (v.Y < BoundingBox.Min.Y) BoundingBox.Min.Y = v.Y;
            if (v.Z < BoundingBox.Min.Z) BoundingBox.Min.Z = v.Z;

            if (v.X > BoundingBox.Max.X) BoundingBox.Max.X = v.X;
            if (v.Y > BoundingBox.Max.Y) BoundingBox.Max.Y = v.Y;
            if (v.Z > BoundingBox.Max.Z) BoundingBox.Max.Z = v.Z;
        }
    }

    if (BoundingBox.Min.X == INFINITY)
    {
        BoundingBox = AABB::unitBox();
    }

    WorldBoxDirty = true;
}
*/

const AABB& Model::boundingBox() const
{
    // lokale Box aus loadMeshes
    const AABB& local = LocalBoundingBox;

    // 8 Ecken aufstellen
    Vector corners[8] = {
        {local.Min.X, local.Min.Y, local.Min.Z},
        {local.Max.X, local.Min.Y, local.Min.Z},
        {local.Min.X, local.Max.Y, local.Min.Z},
        {local.Max.X, local.Max.Y, local.Min.Z},
        {local.Min.X, local.Min.Y, local.Max.Z},
        {local.Max.X, local.Min.Y, local.Max.Z},
        {local.Min.X, local.Max.Y, local.Max.Z},
        {local.Max.X, local.Max.Y, local.Max.Z}
    };

    // initialisiere Welt-BB
    WorldBoundingBox.Min = Vector( std::numeric_limits<float>::max(),
                        std::numeric_limits<float>::max(),
                        std::numeric_limits<float>::max());
    WorldBoundingBox.Max = Vector( std::numeric_limits<float>::lowest(),
                        std::numeric_limits<float>::lowest(),
                        std::numeric_limits<float>::lowest());

    // jede Ecke mit Transform multiplizieren
    for(int i=0; i<8; ++i)
    {
        Vector w = transform() * corners[i];
        WorldBoundingBox.Min.X = std::min(WorldBoundingBox.Min.X, w.X);
        WorldBoundingBox.Min.Y = std::min(WorldBoundingBox.Min.Y, w.Y);
        WorldBoundingBox.Min.Z = std::min(WorldBoundingBox.Min.Z, w.Z);

        WorldBoundingBox.Max.X = std::max(WorldBoundingBox.Max.X, w.X);
        WorldBoundingBox.Max.Y = std::max(WorldBoundingBox.Max.Y, w.Y);
        WorldBoundingBox.Max.Z = std::max(WorldBoundingBox.Max.Z, w.Z);
    }
    return WorldBoundingBox;
}


