#pragma once
#include "Shader.h"
#include "Texture.h"

enum RenderQueue
{
    Background = 100,
    Opaque = 200,
    Transparents = 300,
    PostEffect = 400,
};

enum RenderTargetAction
{
    DontCare = 0,
    Clear = 1
};

enum DepthAction
{
    OFF = 0,
    Always = 1,
    Less = 2,
    Equal = 3,
    LessEqual = 4,
};

struct Buffer
{
    Buffer();

    std::vector<Texture*> textures;
    Shader *shader;
    mat4 worldMatrix;
    unsigned int *VAO;
    unsigned int indices;

};
inline Buffer::Buffer(){}

static unsigned int cubeVAO;
static void SetupCubeMesh()
{

    float skyboxVertices[] = {
        // positions          
        -1.0f,  1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

        -1.0f,  1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f,  1.0f
    };


    unsigned int cubeVBO;
    glGenVertexArrays(1, &cubeVAO);
    glGenBuffers(1, &cubeVBO);
    glBindVertexArray(cubeVAO);
    glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}
static void DrawCubeMesh()
{
    glBindVertexArray(cubeVAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);
}

static unsigned int quadVAO;
static void SetupQuadMesh()
{
 float quadVertices[] = {
  // vertex attributes for a quad that fills the entire screen in Normalized Device Coordinates.
  // positions   // texCoords
  -1.0f, 1.0f, 0.0f, 1.0f,
  -1.0f, -1.0f, 0.0f, 0.0f,
  1.0f, -1.0f, 1.0f, 0.0f,

  -1.0f, 1.0f, 0.0f, 1.0f,
  1.0f, -1.0f, 1.0f, 0.0f,
  1.0f, 1.0f, 1.0f, 1.0f
 };
 // screen quad VAO
 unsigned int quadVBO;
 glGenVertexArrays(1, &quadVAO);
 glGenBuffers(1, &quadVBO);
 glBindVertexArray(quadVAO);
 glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
 glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
 glEnableVertexAttribArray(0);
 glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
 glEnableVertexAttribArray(1);
 glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
 glBindBuffer(GL_ARRAY_BUFFER, 0);
}
static void DrawQuadMesh()
{
    glBindVertexArray(quadVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
}

static void SetRenderTarget(GLuint target, RenderTargetAction colorAction, RenderTargetAction depthAction, DepthAction action)
{
    glBindFramebuffer(GL_FRAMEBUFFER, target);
    auto clearFlag = colorAction == RenderTargetAction::Clear ? GL_COLOR_BUFFER_BIT : 0;
    clearFlag |= depthAction == RenderTargetAction::Clear ? GL_DEPTH_BUFFER_BIT : 0;
    if (clearFlag != 0)
    {
        glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
        glClear(clearFlag);   
    }
    switch (action)
    {
    case OFF:
        glDisable(GL_DEPTH_TEST);
        break;
    case Always:
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_ALWAYS);
        break;
    case Less:
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LESS);
        break;
    case Equal:
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_EQUAL);
        break;
    case LessEqual:
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LEQUAL);
        break;
    default:
        glDisable(GL_DEPTH_TEST);
        break;
    }
}
