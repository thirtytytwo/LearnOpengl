#pragma once
#include <vector>

#include "Camera.h"
#include "Light.h"
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
    void Render();
    void EnqueueBuffer(Buffer &buffer);
    
    // Delete copy constructor and assignment operator
    RenderPipeline(const RenderPipeline&) = delete;
    RenderPipeline& operator=(const RenderPipeline&) = delete;
private:
    RenderPipeline(){}

    void Prepare();
    void DrawOpaque();
    void DrawTransparent();
    void DrawSkybox();
    void DrawFinal();
    

    std::vector<Buffer*> m_RenderList;
    Camera *m_Camera;
    Light *m_Light;
    
    static RenderPipeline* m_Instance;
};
