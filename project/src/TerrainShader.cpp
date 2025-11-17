#include "TerrainShader.h"
#include <string>

TerrainShader::TerrainShader(const std::string& AssetDirectory) : PhongShader(false), Scaling(1,1,1), MixTex(NULL)
{
    std::string VSFile = AssetDirectory + "vsterrain.glsl";
    std::string FSFile = AssetDirectory + "fsterrain.glsl";
    if( !load(VSFile.c_str(), FSFile.c_str()))
        throw std::exception();
    PhongShader::assignLocations();
    specularColor(Color(0,0,0));
    
    MixTexLoc = getParameterID( "MixTex");
    ScalingLoc = getParameterID( "Scaling");
    
    for(int i=0; i<DETAILTEX_COUNT; i++)
    {
        DetailTex[i] =NULL;
        NormalTex[i] =NULL;
        std::string sDetail, sNormal;
        sDetail += "DetailTex[" + std::to_string(i) + "]";
        sNormal += "NormalTex[" + std::to_string(i) + "]";
        DetailTexLoc[i] = getParameterID( sDetail.c_str());
        NormalTexLoc[i] = getParameterID( sNormal.c_str());
    }

}

void TerrainShader::activate(const BaseCamera& Cam) const
{
    PhongShader::activate(Cam);

    int slot=0;
    activateTex(MixTex, MixTexLoc, slot++);

    for(int i=0; i<DETAILTEX_COUNT; i++)
    {
        activateTex(DetailTex[i], DetailTexLoc[i], slot++);
        activateTex(NormalTex[i], NormalTexLoc[i], slot++);
    }
    
    setParameter(ScalingLoc, Scaling);
}

void TerrainShader::deactivate() const
{
    PhongShader::deactivate();
    for(int i=DETAILTEX_COUNT-1; i>=0; i--)
    {
        if(DetailTex[i]&&DetailTexLoc[i]>=0) DetailTex[i]->deactivate();
        if(NormalTex[i]&&NormalTexLoc[i]>=0) NormalTex[i]->deactivate();
    }
        
    if(MixTex) MixTex->deactivate();
}

void TerrainShader::activateTex(const Texture* pTex, GLint Loc, int slot) const
{
    if(pTex && pTex->isValid() && Loc >=0)
    {
        pTex->activate(slot);
        setParameter(Loc, slot);
    }
    
}
