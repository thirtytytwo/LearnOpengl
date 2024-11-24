#include "RenderPipeline.h"

#include <glad/glad.h>
RenderPipeline* RenderPipeline::m_Instance = nullptr;

void RenderPipeline::Setup(Camera &camera, Light &light)
{
    m_Camera = &camera;
    m_Light = &light;
    glGenFramebuffers(1, &m_framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, m_framebuffer);
    
    glGenTextures(1, &m_colorAttachment);
    glBindTexture(GL_TEXTURE_2D, m_colorAttachment);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 1920, 1080, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_colorAttachment, 0);
    
    glGenRenderbuffers(1, &m_depthAttachment);
    glBindRenderbuffer(GL_RENDERBUFFER, m_depthAttachment);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, 1920, 1080);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_depthAttachment);

    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        std::cout << "Framebuffer is not complete" << std::endl;
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

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
    for (auto buffer : m_OpaqueRenderList)
    {
        unsigned int uniformBlock = glGetUniformBlockIndex(buffer->shader->ID, "CameraBuffer");
        glUniformBlockBinding(buffer->shader->ID, uniformBlock, 0);
    }
    unsigned int uniformBlock = glGetUniformBlockIndex(m_SkyboxShader->ID, "CameraBuffer");
    glUniformBlockBinding(m_SkyboxShader->ID, uniformBlock, 0);
    
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
        glDeleteBuffers(1, buffer->VAO);
    }
    m_OpaqueRenderList.clear();

    delete(m_SkyboxTexture);
    delete(m_SkyboxShader);

    delete(m_FinalShader);
    
    glDeleteFramebuffers(1, &m_framebuffer);
    glDeleteTextures(1, &m_colorAttachment);
    glDeleteTextures(1, &m_depthAttachment);
}



void RenderPipeline::DrawOpaque()
{
    for (auto buffer : m_OpaqueRenderList)
    {
        buffer->shader->use();
        
        buffer->shader->SetWolrd(buffer->worldMatrix);
        
        for (int i = 0; i < buffer->textures.size(); i++)
        {
            buffer->textures[i]->Active(GL_TEXTURE0 + i);
            buffer->textures[i]->Bind();
        }
        glBindVertexArray(*buffer->VAO);
        glDrawElements(GL_TRIANGLES, buffer->indices, GL_UNSIGNED_INT, 0);
    }
}

void RenderPipeline::DrawSkybox()
{
    glDepthFunc(GL_LEQUAL);
    m_SkyboxShader->use();
    
    m_SkyboxShader->SetInt("Skybox", 0);
    m_SkyboxTexture->Active(GL_TEXTURE0);
    m_SkyboxTexture->Bind();
    DrawCubeMesh();
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

    m_FinalShader->use();
    glBindTexture(GL_TEXTURE_2D, m_colorAttachment);
    DrawQuadMesh();
    glDepthFunc(GL_LESS);
}

