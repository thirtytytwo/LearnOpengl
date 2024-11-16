#include "RenderPipeline.h"

#include <glad/glad.h>
RenderPipeline* RenderPipeline::m_Instance = nullptr;

void RenderPipeline::Setup(Camera &camera, Light &light)
{
    m_Camera = &camera;
    m_Light = &light;
    SetRenderTarget(m_framebuffer, m_colorAttachment, m_depthAttachment, i8vec2(1920, 1080));

    //Setup Skybox
    m_SkyboxShader = new Shader("Skybox");
    m_SkyboxTexture = new Texture("sunshine", Cube);

    //Setup Final
    m_FinalShader = new Shader("ResolveToScreen");
    m_FinalShader->use();
    m_FinalShader->SetInt("screenTexture", 0);
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
    }
}


void RenderPipeline::Render()
{
    DrawOpaque();
    //DrawTransparent();
    DrawSkybox();
    DrawFinal();
}

void RenderPipeline::BeginRender()
{
    
    unsigned int uboMatrices;
    glGenBuffers(1, &uboMatrices);
    glBindBuffer(GL_UNIFORM_BUFFER, uboMatrices);
    glBufferData(GL_UNIFORM_BUFFER, 2 * sizeof(glm::mat4), NULL, GL_STATIC_DRAW);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    glBindBufferRange(GL_UNIFORM_BUFFER, 0, uboMatrices, 0, 2 * sizeof(glm::mat4));

    auto proj = m_Camera->GetProjectionMatrix();
    auto view = m_Camera->GetViewMatrix();

    glBindBuffer(GL_UNIFORM_BUFFER, uboMatrices);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), value_ptr(proj));
    glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4), value_ptr(view));

    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    glBindFramebuffer(GL_FRAMEBUFFER, m_framebuffer);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void RenderPipeline::FinishRender()
{
    for (auto& buffer : m_OpaqueRenderList)
    {
        buffer->textures.clear();
        glDeleteBuffers(1, &buffer->VAO);
    }
    m_OpaqueRenderList.clear();

    delete(m_SkyboxTexture);
    delete(m_SkyboxShader);
    glDeleteBuffers(1, &GetCubeMesh());

    delete(m_FinalShader);
    glDeleteBuffers(1, &GetQuadMesh());
    
    glDeleteFramebuffers(1, &m_framebuffer);
    glDeleteTextures(1, &m_colorAttachment);
    glDeleteTextures(1, &m_depthAttachment);
}



void RenderPipeline::DrawOpaque()
{
    for (auto& buffer : m_OpaqueRenderList)
    {
        buffer->shader.use();

        unsigned int uniformBlock = glGetUniformBlockIndex(buffer->shader.ID, "CameraBuffer");
        glUniformBlockBinding(buffer->shader.ID, uniformBlock, 0);
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
    m_SkyboxShader->use();

    unsigned int uniformBlock = glGetUniformBlockIndex(m_SkyboxShader->ID, "CameraBuffer");
    glUniformBlockBinding(m_SkyboxShader->ID, uniformBlock, 0);
    
    m_SkyboxShader->SetInt("Skybox", 0);
    m_SkyboxTexture->Active(GL_TEXTURE0);
    m_SkyboxTexture->Bind();
    glBindVertexArray(id);
    glDrawArrays(GL_TRIANGLES, 0, static_cast<int>(id));
    glBindVertexArray(0);
    glDepthFunc(GL_LESS);
}

void RenderPipeline::DrawFinal()
{
    //物体已经渲染到指定的framebuffer上，现在需要把framebuffer中的值拷贝到最终的屏幕上
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glDisable(GL_DEPTH_TEST); // disable depth test so screen-space quad isn't discarded due to depth test.
    // clear all relevant buffers
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f); // set clear color to white (not really necessary actually, since we won't be able to see behind the quad anyways)
    glClear(GL_COLOR_BUFFER_BIT);
    
    auto id = GetQuadMesh();

    m_FinalShader->use();

    glBindVertexArray(id);
    glBindTexture(GL_TEXTURE_2D, m_colorAttachment);
    glDrawArrays(GL_TRIANGLES, 0, static_cast<int>(id));
    glBindVertexArray(0);
    glDepthFunc(GL_LESS);
}

