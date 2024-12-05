#pragma once
#include <vector>

#include "Camera.h"
#include "Light.h"
#include "Material.h"

class RenderPipeline
{
public:
    static RenderPipeline& GetInstance()
    {
        if (m_Instance == nullptr)
        {
            m_Instance = new RenderPipeline();
        }
        return *m_Instance;
    }
    ~RenderPipeline()
    {
        delete m_Instance;
    }

    void Setup(Camera &camera, Light &light);
    void BeginRender();
    void Render();
    void FinishRender();
    void EnqueueBuffer(Buffer &buffer, RenderQueue queue);
    
    // Delete copy constructor and assignment operator
    RenderPipeline(const RenderPipeline&) = delete;
    RenderPipeline& operator=(const RenderPipeline&) = delete;
    
private:
    RenderPipeline(): m_SkyboxShader(nullptr),
                      m_SkyboxTexture(nullptr),
                      m_FinalShader(nullptr),
                      m_Camera(nullptr),
                      m_Light(nullptr), m_ColorAttachments{1,2,3,4,5}, m_DepthAttachment(0), m_RenderTargetHandles{1,2}{}

    void SetupCamera();
    void SetupLight();
    void SetupRenderTargets();
    
    void DrawOpaque();
    void DrawOpaqueDeffer();
    void DrawTransparent();
    void DrawSkybox();
    void DrawFinal();
    
    void SetBufferUniform(string name, int index);


    std::vector<Buffer*> m_OpaqueRenderList;
    std::vector<Buffer*> m_TransparentRenderList;
    std::vector<Buffer*> m_PostEffectRenderList;

    Shader* m_SkyboxShader;
    Texture* m_SkyboxTexture;

    Shader* m_FinalShader;

    Camera* m_Camera;
    Light* m_Light;

    GLuint m_ColorAttachments[5]; //color, position, normal ,albedo+specular
    GLuint m_DepthAttachment;
    GLuint m_RenderTargetHandles[2];
    
    static RenderPipeline* m_Instance;
};
