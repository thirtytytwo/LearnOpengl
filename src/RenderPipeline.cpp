#include "RenderPipeline.h"

#include <glad/glad.h>
RenderPipeline* RenderPipeline::m_Instance = nullptr;

void RenderPipeline::Setup(Camera &camera, Light &light)
{
    m_Camera = &camera;
    m_Light = &light;
}

void RenderPipeline::EnqueueBuffer(Buffer &buffer, RenderQueue queue)
{
    switch (queue)
    {
    case Background:
        break;
    case Opaque:
        m_OpaqueRenderList.push_back(&buffer);
        break;
    case Transparents:
        m_TransparentRenderList.push_back(&buffer);
        break;
    case PostEffect:
        m_PostEffectRenderList.push_back(&buffer);
        break;
    case Skybox:
        m_SkyboxRender = &buffer;
        break;
    case Final:
        break;
    }
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
    for (auto &buffer : m_OpaqueRenderList)
    {
        buffer->shader.use();
        buffer->shader.SetWolrd(buffer->worldMatrix);
        for (int i = 0; i < buffer->textures.size(); i++)
        {
            buffer->textures[i].Active(GL_TEXTURE0 + i);
            buffer->textures[i].Bind();
        }
        glBindVertexArray(buffer->VAO);
        glDrawElements(GL_TRIANGLES, buffer->indices, GL_UNSIGNED_INT, 0);
    }
}

void RenderPipeline::DrawSkybox()
{
    auto id = GetCubeMesh();
    glDepthFunc(GL_LEQUAL);
    m_SkyboxRender->shader.use();
    m_SkyboxRender->shader.SetInt("Skybox", 0);
    glBindVertexArray(id);
    m_SkyboxRender->textures[0].Active(GL_TEXTURE0);
    m_SkyboxRender->textures[0].Bind();
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);
    glDepthFunc(GL_LESS);
}
