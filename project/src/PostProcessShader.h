#pragma once
#include "baseshader.h"

class PostProcessShader : public BaseShader
{
public:
    PostProcessShader();
    virtual ~PostProcessShader();
    void activate(const BaseCamera& Cam) const override;
protected:
     void assignLocations();
private:
    GLint screenTextureLoc;
};
