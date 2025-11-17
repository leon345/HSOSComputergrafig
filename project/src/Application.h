//
//  Application.hpp
//  ogl4
//
//  Created by Philipp Lensing on 16.09.16.
//  Copyright © 2016 Philipp Lensing. All rights reserved.
//

#ifndef Application_hpp
#define Application_hpp

#include <stdio.h>
#include <list>
#include "camera.h"
#include "phongshader.h"
#include "constantshader.h"
#include "vertexbuffer.h"
#include "indexbuffer.h"
#include "basemodel.h"
#include "Framebuffer.h"
#include "Player.h"
#include "PostProcessShader.h"
#include "ShadowMapGenerator.h"
#include "terrain.h"

class Application
{
public:
    typedef std::list<BaseModel*> ModelList;
    Application(GLFWwindow* pWin);
    void start();
    void update(float dtime);
    void draw();
    void end();

protected:
    Camera Cam;
    ModelList Models;
    GLFWwindow* pWindow;
    Terrain* pTerrain;

    int width, height;

    double mouseX, mouseY;
    Player* pPlayer;

    ShadowMapGenerator ShadowGenerator;

    Framebuffer* pFramebuffer;
    Texture* pBufferTexture;
    GLuint quadVAO;
    GLuint createQuadVAO();
    void setupPauseMenuStyle();
    //NUr zum testen
    PostProcessShader* m_pPostProcessShader;

    unsigned int totalCoins = 0;
};

#endif /* Application_hpp */
