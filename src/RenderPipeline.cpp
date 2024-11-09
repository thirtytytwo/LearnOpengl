#include "RenderPipeline.h"

#include <glad/glad.h>
RenderPipeline* RenderPipeline::m_Instance = nullptr;

void RenderPipeline::Setup(Camera &camera, Light &light)
{
    m_Camera = &camera;
    m_Light = &light;
}

void RenderPipeline::Render()
{
    Prepare();
    DrawOpaque();
    DrawTransparent();
    DrawSkybox();
    DrawFinal();
}

void RenderPipeline::DrawOpaque()
{
    for (auto &buffer : m_RenderList)
    {
        if (buffer->queue == Opaque)
        {
            buffer.shader.use();
            buffer.shader.SetWolrd();
            buffer.shader.SetCameraProps(*m_Camera);
            buffer.shader.SetLight(*m_Light);
            for (int i = 0; i < buffer->textures.size(); i++)
            {
                buffer->textures[i].Active(GL_TEXTURE0 + i);
                buffer->textures[i].Bind();
            }
            glBindVertexArray(buffer->VAO);
            glDrawElements(GL_TRIANGLES, buffer->indices, GL_UNSIGNED_INT, 0);
        }
    }
}

void RenderPipeline::DrawSkybox()
{
    auto ID = GetCubeMesh();
    
}
