#pragma once
#include <vector>

#include "Camera.h"
#include "Light.h"
#include "Material.h"
#include "RenderUtils.h"

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
    RenderPipeline():m_framebuffer(0), m_colorAttachment(0), m_depthAttachment(0){}

    void SetupCamera();
    void SetupLight();
    
    void DrawOpaque();
    //void DrawTransparent();
    void DrawSkybox();
    void DrawFinal();

    void SetBufferUniform(string name, int index);
    

    std::vector<Buffer*> m_OpaqueRenderList;
    std::vector<Buffer*> m_TransparentRenderList;
    std::vector<Buffer*> m_PostEffectRenderList;
    
    Shader *m_SkyboxShader;
    Texture *m_SkyboxTexture;
    
    Shader *m_FinalShader;
    
    Camera *m_Camera;
    Light *m_Light;

    unsigned int m_framebuffer;
    unsigned int m_colorAttachment;
    unsigned int m_depthAttachment;
    
    static RenderPipeline* m_Instance;
};
