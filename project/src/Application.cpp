//
//  Application.cpp
//  ogl4
//
//  Created by Philipp Lensing on 16.09.16.
//  Copyright © 2016 Philipp Lensing. All rights reserved.
//

#include "Application.h"

#include "Coin.h"
#ifdef WIN32
#include <GL/glew.h>
#include <glfw/glfw3.h>
#define _USE_MATH_DEFINES
#include <math.h>
#else
#define GLFW_INCLUDE_GLCOREARB
#define GLFW_INCLUDE_GLEXT
#include <glfw/glfw3.h>
#endif
#include "lineplanemodel.h"
#include "triangleplanemodel.h"
#include "trianglespheremodel.h"
#include "lineboxmodel.h"
#include "triangleboxmodel.h"
#include "model.h"
#include "terrainshader.h"
#include "rgbimage.h"

#include "thirdparty/imgui/imgui.h"
#include "thirdparty/imgui/backends/imgui_impl_glfw.h"
#include "thirdparty/imgui/backends/imgui_impl_opengl3.h"


#ifdef WIN32
#define ASSET_DIRECTORY "../../assets/"
#else
#define ASSET_DIRECTORY "../assets/"
#endif

Application::Application(GLFWwindow* pWin) : pWindow(pWin), Cam(pWin), ShadowGenerator(1024, 1024)
{
    BaseModel* pModel;
    Color c = Color(0.5f, 0.5f, 0.8f);
    Vector a = Vector(0.15f, 0.15f, 0.15f);

/*
    PointLight* pl = new PointLight();
    pl->position(Vector(12,6,20));
    pl->color(c);
    pl->attenuation(a);
    ShaderLightMapper::instance().addLight(pl);
*/

    DirectionalLight* dl = new DirectionalLight();
    //dl->position(Vector(10, 20, 10));
    dl->direction(Vector(-2.3f, -10.8f, 4.0f));
    dl->color(c);
    dl->castShadows(true);
    ShaderLightMapper::instance().addLight(dl);
  
    PointLight* pl = new PointLight();
    pl->position(Vector(-24.20f, 23.0f, 5.80f));
    pl->color(Color(1.0f, 0.0f, 0.3f));
    pl->attenuation(a);
    ShaderLightMapper::instance().addLight(pl);

    pl = new PointLight();
    pl->position(Vector(-10.85f,24.0f,-12.67f));
    pl->color(Color(0.3f, 1.0f, 0.0f));
    pl->attenuation(a);
    ShaderLightMapper::instance().addLight(pl);

    pl = new PointLight();
    pl->position(Vector(30.85f,24.0f,-12.67f));
    pl->color(Color(0.0f, 0.3f, 1.0f));
    pl->attenuation(a);
    ShaderLightMapper::instance().addLight(pl);

    pl = new PointLight();
    pl->position(Vector(-62.7f,17.5f,-4.70f));
    pl->color(Color(0.5f, 0.5f, 0.0f));
    pl->attenuation(a);
    ShaderLightMapper::instance().addLight(pl);

/*    
    SpotLight* sl = new SpotLight();
    sl->position(Vector(-23.5f,15.5f,-40.0f));
    sl->direction(Vector(0.0f,-0.2f,0.8f));
    sl->color(c);
    sl->innerRadius(1.0f);
    sl->outerRadius(15.0f);
    sl->castShadows(true);
    ShaderLightMapper::instance().addLight(sl);
*/

    

    pModel = new Coin(ASSET_DIRECTORY "coin.dae", Vector(-24.70f, 21.0f, 5.80f));
    PhongShader* pPhongShader = new PhongShader();
    pPhongShader->outlineColor(Color(1.0f, 1.0f, 1.0f));
    pPhongShader->outlineWidth(3.0f);
    pModel->shader(pPhongShader, true);
    pModel->shadowCaster(true);
    Models.push_back(pModel);
    ++totalCoins;
    
    pModel = new Coin(ASSET_DIRECTORY "coin.dae", Vector(-10.35f,22.0f,-12.67f));
    pPhongShader = new PhongShader();
    pPhongShader->outlineColor(Color(1.0f, 1.0f, 1.0f));
    pPhongShader->outlineWidth(3.0f);
    pModel->shader(pPhongShader, true);
    pModel->shadowCaster(true);
    Models.push_back(pModel);
    ++totalCoins;
    
    pModel = new Coin(ASSET_DIRECTORY "coin.dae", Vector(30.35f,22.0f,-12.67f));
    pPhongShader = new PhongShader();
    pPhongShader->outlineColor(Color(1.0f, 1.0f, 1.0f));
    pPhongShader->outlineWidth(3.0f);
    pModel->shader(pPhongShader, true);
    pModel->shadowCaster(true);
    Models.push_back(pModel);
    ++totalCoins;
    
    pModel = new Coin(ASSET_DIRECTORY "coin.dae", Vector(-62.2f,15.5f,-4.70f));
    pPhongShader = new PhongShader();
    pPhongShader->outlineColor(Color(1.0f, 1.0f, 1.0f));
    pPhongShader->outlineWidth(3.0f);
    pModel->shader(pPhongShader, true);
    pModel->shadowCaster(true);
    Models.push_back(pModel);
    ++totalCoins;


    pModel = new Model(ASSET_DIRECTORY "skybox.obj", false);
    pModel->shader(new PhongShader(), true);
    pModel->setIsCollidable(false);
    pModel->shadowCaster(false);
    Models.push_back(pModel);


    pTerrain = new Terrain();
    TerrainShader* pTerrainShader = new TerrainShader(ASSET_DIRECTORY);
    pTerrainShader->diffuseTexture(Texture::LoadShared(ASSET_DIRECTORY"aerial_rocks_02_2k/textures/aerial_rocks_02_diff_2k.jpg")); //eiegentlich grass.bmp
    //pTerrainShader->mixTex( //TODO Shader. Hab ich hinzugefügt. um das übehaupt zu laden da wenn ich mir die einzenen Texture angucke sind die nur monoton einfarbig. Es fehlen irgendwie die Feinheiten.
    pTerrain->shader(pTerrainShader, true);
    pTerrain->load(ASSET_DIRECTORY "volcanoJump.png",
        ASSET_DIRECTORY"aerial_rocks_02_2k/textures/aerial_rocks_02_diff_2k.jpg",
        ASSET_DIRECTORY"rocks_ground_06_2k/rocks_ground_06_diff_2k.jpg",
        ASSET_DIRECTORY"rocks_ground_06_2k/rocks_ground_06_diff_2k.jpg",
        ASSET_DIRECTORY"snow_02_2k/textures/snow_02_diff_2k.jpg");
    pTerrain->resizeTerrain(1.0f, 20.0f, 1.0f);
    pTerrain->shadowCaster(true);
    Models.push_back(pTerrain);

    pPlayer = new Player(Models);

    glfwGetWindowSize(pWindow, &width, &height);
    pFramebuffer = new Framebuffer();
    pBufferTexture = new Texture();
    
    m_pPostProcessShader = new PostProcessShader();
    
}
void Application::start()
{
    glEnable(GL_DEPTH_TEST); // enable depth-testing
    glDepthFunc(GL_LESS); // depth-testing interprets a smaller value as "closer"
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glfwSetInputMode(pWindow, GLFW_CURSOR, GLFW_CURSOR_HIDDEN); //geht auch disable
    quadVAO = createQuadVAO();
    
    RGBImage img(width, height);
    pBufferTexture->create(img);
    pFramebuffer->create(true, width, height);
    pFramebuffer->attachColorTarget(*pBufferTexture);

    // ImGui Context erstellen
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;

    // Style 
    ImGui::StyleColorsDark();

    // Backend initialisieren (GLFW + OpenGL3)
    ImGui_ImplGlfw_InitForOpenGL(pWindow, true);
    ImGui_ImplOpenGL3_Init("#version 400");
}

void Application::update(float dtime)
{
    pPlayer->handleInput(pWindow, dtime);
    if (pPlayer->getIsGamePaused())
    {
        pPlayer->update(dtime);

        for (BaseModel* model : Models)
        {
            Coin* coin = dynamic_cast<Coin*>(model);
            if (coin)
            {
                coin->update(dtime);
            }
        }
        
        Cam.setPosition(pPlayer->getPosition());
        Cam.setTarget(pPlayer->getPosition()+pPlayer->getForwardDirection());
        Cam.update();
    }
}

void Application::draw()
{

// Neue ImGui-Frame starten (sollte vor alles andere in draw gemacht werden) 
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    // 1. HUD Fenster (immer sichtbar, oben links)
    ImGui::SetNextWindowBgAlpha(0.0f); // transparent
    ImGui::Begin("HUD", NULL, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize |
                               ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings |
                               ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav);
    ImGui::SetWindowPos(ImVec2(10, 10)); // Position oben links
    if (pPlayer->getCollectedCoins() == totalCoins)
    {
        ImGui::Text("All Coins Collected");
    } else
    {
        ImGui::Text("Coins: %d / %d", (int)pPlayer->getCollectedCoins(), (int)totalCoins);
    }
    ImGui::End();

    // GUI element AI generiert
    // 2. Pause-Menü (nur wenn pausiert, zentriert)
    if (!pPlayer->getIsGamePaused())
    {
        // Bildschirm-Overlay für Pause-Effekt
        ImGui::SetNextWindowPos(ImVec2(0, 0));
        ImGui::SetNextWindowSize(ImVec2(width, height));
        ImGui::Begin("PauseOverlay", NULL, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | 
                                          ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoSavedSettings |
                                          ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav);
        
        // Semi-transparenter dunkler Hintergrund
        ImDrawList* draw_list = ImGui::GetWindowDrawList();
        draw_list->AddRectFilled(ImVec2(0, 0), ImVec2(width, height), IM_COL32(0, 0, 0, 120));
        
        ImGui::End();

        // Hauptpause-Menü (zentriert)
        float windowWidth = 400.0f;
        float windowHeight = 300.0f;
        ImGui::SetNextWindowPos(ImVec2((width - windowWidth) * 0.5f, (height - windowHeight) * 0.5f));
        ImGui::SetNextWindowSize(ImVec2(windowWidth, windowHeight));
        
        // Stylisches Design für das Pause-Menü
        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 15.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 2.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(20, 20));
        ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.1f, 0.1f, 0.15f, 0.95f));
        ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0.3f, 0.6f, 1.0f, 0.8f));
        
        ImGui::Begin("PauseMenu", NULL, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize |
                                       ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoSavedSettings);

        // Titel
        ImGui::PushFont(NULL); // Falls du größere Fonts hast
        ImGui::SetCursorPosX((windowWidth - ImGui::CalcTextSize("GAME PAUSED").x) * 0.5f - 20);
        ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.5f, 1.0f), "GAME PAUSED");
        ImGui::PopFont();
        
        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();

        // Controls/Info
        ImGui::TextWrapped("Controls:");
        ImGui::BulletText("Press 'ESC' to resume game");
        ImGui::BulletText("WASD to move");
        ImGui::BulletText("Mouse to look around");
        ImGui::BulletText("Space to jump");
        ImGui::BulletText("Shift to run, Ctrl to walk slow");
        
        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();

        // Stats
        ImGui::Text("Game Stats:");
        ImGui::Text("Coins collected: %d / %d", (int)pPlayer->getCollectedCoins(), (int)totalCoins);
        ImGui::Text("Position: %.1f, %.1f, %.1f", pPlayer->getPosition().X, pPlayer->getPosition().Y, pPlayer->getPosition().Z);

        ImGui::End();
        
        // Style zurücksetzen
        ImGui::PopStyleColor(2);
        ImGui::PopStyleVar(3);
    }
   
    // 1) Shadow-Pass (Depth only in Shadow FBOs)
    
    ShadowGenerator.generate(Models);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    
    
    // glViewport(0, 0, width, height);
    
    
    // 2. Main Pass: normales Rendern mit Phong
    
    // glEnable(GL_DEPTH_TEST);
    
    ShaderLightMapper::instance().activate();
    pFramebuffer->activate();
    GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    assert(status == GL_FRAMEBUFFER_COMPLETE);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    const size_t shadowCount = ShaderLightMapper::instance().lights().size();
    for(auto it = Models.begin(); it != Models.end(); ++it)
    {
        PhongShader* phong = dynamic_cast<PhongShader*>((*it)->shader());
        if (!phong) continue;

        for (size_t i = 0; i < shadowCount; ++i)
        {
            phong->shadowMap(i,
                             ShadowGenerator.getShadowMap(i),
                             ShadowGenerator.getShadowMatrix(i));
        }
    }
    

    // 2. setup shaders and draw models
    for (ModelList::iterator it = Models.begin(); it != Models.end(); ++it)
    {
        if (!(*it)->getIsActive()) continue;
        (*it)->draw(Cam);
    }


    // 2. setup shaders and draw models

    
    pFramebuffer->deactivate();
    ShaderLightMapper::instance().deactivate();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    // glViewport(0, 0, width, height);

    //glDisable(GL_DEPTH_TEST);
    

    GLuint textureId = pFramebuffer->getAttachedTexture()->ID();
    //TODO Shader Binden für Postprocessing
    m_pPostProcessShader->activate(Cam);
    
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureId);
            
    glBindVertexArray(quadVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
            
    // TODO Shader deaktivieren m_pPostProcessShader->deactivate();
    m_pPostProcessShader->deactivate();
    
    // Rendering der ImGui Elemente
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    
    // 3. check once per frame for opengl errors
    GLenum Error = glGetError();
    assert(Error == 0);
}

void Application::end()
{
    for (ModelList::iterator it = Models.begin(); it != Models.end(); ++it)
        delete* it;

    Models.clear();
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

GLuint Application::createQuadVAO()
{
    GLuint quadVAO, quadVBO;
    float quadVertices[] = {
        // positions     // texCoords
        -1.0f, 1.0f, 0.0f, 1.0f,
        -1.0f, -1.0f, 0.0f, 0.0f,
        1.0f, -1.0f, 1.0f, 0.0f,

        -1.0f, 1.0f, 0.0f, 1.0f,
        1.0f, -1.0f, 1.0f, 0.0f,
        1.0f, 1.0f, 1.0f, 1.0f
    };
    glGenVertexArrays(1, &quadVAO);
    glGenBuffers(1, &quadVBO);
    glBindVertexArray(quadVAO);
    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glBindVertexArray(0);
    return quadVAO;
}

void Application::setupPauseMenuStyle()
{
    ImGuiStyle& style = ImGui::GetStyle();
    
    // Runde Ecken für Buttons
    style.FrameRounding = 8.0f;
    style.GrabRounding = 8.0f;
    
}
