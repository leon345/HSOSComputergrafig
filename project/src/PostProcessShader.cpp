#include "PostProcessShader.h"
#define ASSET_DIRECTORY "../../assets/postProcessingShader/"
PostProcessShader::PostProcessShader()
{
    load(ASSET_DIRECTORY "vpostProcessingShader.glsl", ASSET_DIRECTORY "fpostProcessingShader.glsl");
    assignLocations();
}

PostProcessShader::~PostProcessShader()
{
}

void PostProcessShader::activate(const BaseCamera& Cam) const
{
    BaseShader::activate(Cam);
    if (screenTextureLoc != -1) {
        glUniform1i(screenTextureLoc, 0);
    }


    
}

 void  PostProcessShader::assignLocations()
{
    screenTextureLoc = glGetUniformLocation(ShaderProgram, "u_ScreenTexture");
}
