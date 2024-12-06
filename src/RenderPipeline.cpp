#include "RenderPipeline.h"

#include <glad/glad.h>

#define FORWARD_RENDER 0
#define DEFFERED_RENDER 1

RenderPipeline* RenderPipeline::m_Instance = nullptr;


void RenderPipeline::Setup(Camera &camera, Light &light)
{
    m_Camera = &camera;
    m_Light = &light;

    SetupRenderTargets();
    
    SetupQuadMesh();
    SetupCubeMesh();

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
#if FORWARD_RENDER
    DrawOpaque();
#elif DEFFERED_RENDER
    DrawOpaqueDeffer();
#endif
    DrawTransparent();
    DrawSkybox();
    DrawFinal();
}
void RenderPipeline::BeginRender()
{
    SetupCamera();
    SetupLight();
}
void RenderPipeline::FinishRender()
{
    for (auto& buffer : m_OpaqueRenderList)
    {
        buffer->textures.clear();
        glDeleteBuffers(1, buffer->VAO);
    }
    for (auto& buffer : m_TransparentRenderList)
    {
        buffer->textures.clear();
        glDeleteBuffers(1, buffer->VAO);
    }
    m_OpaqueRenderList.clear();
    m_TransparentRenderList.clear();
    
    delete(m_SkyboxTexture);
    delete(m_SkyboxShader);

    delete(m_FinalShader);

    for (int i = 0; i < 5; i++)
    {
        glDeleteTextures(1, &m_ColorAttachments[i]);
    }
    glDeleteBuffers(1, &m_DepthAttachment);
}

//Private
void RenderPipeline::SetupCamera()
{
    SetBufferUniform("CameraBuffer", 0);
    
    unsigned int uboMatrices;
    glGenBuffers(1, &uboMatrices);
    glBindBuffer(GL_UNIFORM_BUFFER, uboMatrices);
    glBufferData(GL_UNIFORM_BUFFER, 2 * sizeof(glm::mat4) + sizeof(vec4), NULL, GL_STATIC_DRAW);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    glBindBufferRange(GL_UNIFORM_BUFFER, 0, uboMatrices, 0, 2 * sizeof(glm::mat4) + sizeof(vec4));

    auto proj = m_Camera->GetProjectionMatrix();
    auto view = m_Camera->GetViewMatrix();
    auto position = m_Camera->GetPosition();
    vec4 position4 = vec4(position, 1.0f);

    glBindBuffer(GL_UNIFORM_BUFFER, uboMatrices);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), value_ptr(proj));
    glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4), value_ptr(view));
    glBufferSubData(GL_UNIFORM_BUFFER, 2 * sizeof(glm::mat4), sizeof(vec4), value_ptr(position4));
    
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}
void RenderPipeline::SetupLight()
{
    SetBufferUniform("LightBuffer", 1);
    unsigned int uboLight;
    glGenBuffers(1, &uboLight);
    glBindBuffer(GL_UNIFORM_BUFFER, uboLight);
    glBufferData(GL_UNIFORM_BUFFER, 3 * sizeof(glm::vec4), NULL, GL_STATIC_DRAW);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    glBindBufferRange(GL_UNIFORM_BUFFER, 1, uboLight, 0, 3 * sizeof(glm::vec4));
    
    vec4 position = vec4(m_Light->position, 1.0f);
    vec4 color = vec4(m_Light->color,1.0f);
    vec4 direction = vec4(m_Light->direction, 1.0f);

    glBindBuffer(GL_UNIFORM_BUFFER, uboLight);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(vec4), value_ptr(position));
    glBufferSubData(GL_UNIFORM_BUFFER, sizeof(vec4), sizeof(vec4), value_ptr(color));
    glBufferSubData(GL_UNIFORM_BUFFER, 2 * sizeof(vec4), sizeof(vec4), value_ptr(direction));
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void RenderPipeline::SetupRenderTargets()
{
    //TODO:后续还有renderscale的配置
#if FORWARD_RENDER
    glGenFramebuffers(1, &m_RenderTargetHandles[0]);
    glBindFramebuffer(GL_FRAMEBUFFER, m_RenderTargetHandles[0]);
    
    glGenTextures(1, &m_ColorAttachments[0]);
    glBindTexture(GL_TEXTURE_2D, m_ColorAttachments[0]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 1920, 1080, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_ColorAttachments[0], 0);

    glGenRenderbuffers(1, &m_DepthAttachment);
    glBindRenderbuffer(GL_RENDERBUFFER, m_DepthAttachment);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH32F_STENCIL8, 1920, 1080);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_DepthAttachment);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    
#elif DEFFERED_RENDER
    //生成主framebuffer
    glGenFramebuffers(1, &m_RenderTargetHandles[0]);
    glBindFramebuffer(GL_FRAMEBUFFER, m_RenderTargetHandles[0]);

    glGenTextures(1, &m_ColorAttachments[0]);
    glBindTexture(GL_TEXTURE_2D, m_ColorAttachments[0]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 1920, 1080, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_ColorAttachments[0], 0);
    
    glGenRenderbuffers(1, &m_DepthAttachment);
    glBindRenderbuffer(GL_RENDERBUFFER, m_DepthAttachment);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH32F_STENCIL8, 1920, 1080);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    //生成GBuffer
    glGenFramebuffers(1, &m_RenderTargetHandles[1]);
    glBindFramebuffer(GL_FRAMEBUFFER, m_RenderTargetHandles[1]);

    //position
    glGenTextures(1, &m_ColorAttachments[1]);
    glBindTexture(GL_TEXTURE_2D, m_ColorAttachments[1]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, 1920, 1080, 0, GL_RGB, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, m_ColorAttachments[1], 0);
    //normal
    glGenTextures(1, &m_ColorAttachments[2]);
    glBindTexture(GL_TEXTURE_2D, m_ColorAttachments[2]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, 1920, 1080, 0, GL_RGB, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, m_ColorAttachments[2], 0);
    //albedo+specular
    glGenTextures(1, &m_ColorAttachments[3]);
    glBindTexture(GL_TEXTURE_2D, m_ColorAttachments[3]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1920, 1080, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, GL_TEXTURE_2D, m_ColorAttachments[3], 0);

    GLuint attachments[3] = {GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3};
    glDrawBuffers(3, attachments);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    
    
#endif
    
}

void RenderPipeline::DrawOpaque()
{

    SetRenderTarget(std::move(m_RenderTargetHandles[0]), RenderTargetAction::Clear, RenderTargetAction::Clear, DepthAction::Less);
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

void RenderPipeline::DrawOpaqueDeffer()
{
    SetRenderTarget(std::move(m_RenderTargetHandles[1]), Clear, DontCare, DepthAction::Less);
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
    SetRenderTarget(std::move(m_RenderTargetHandles[0]), Clear, DontCare, DepthAction::OFF);
    Shader DefferLitShader("DefferLit");
    DefferLitShader.use();
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_ColorAttachments[1]);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, m_ColorAttachments[2]);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, m_ColorAttachments[3]);

    DrawQuadMesh();
}

void RenderPipeline::DrawTransparent()
{
    SetRenderTarget(std::move(m_RenderTargetHandles[0]), RenderTargetAction::DontCare, RenderTargetAction::DontCare, DepthAction::Less);
    for (auto buffer : m_TransparentRenderList)
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
    SetRenderTarget(std::move(m_RenderTargetHandles[0]), DontCare, DontCare, DepthAction::LessEqual);
    m_SkyboxShader->use();
    
    m_SkyboxShader->SetInt("Skybox", 0);
    m_SkyboxTexture->Active(GL_TEXTURE0);
    m_SkyboxTexture->Bind();
    DrawCubeMesh();
}
void RenderPipeline::DrawFinal()
{
    SetRenderTarget(0, Clear, DontCare, DepthAction::OFF);

    m_FinalShader->use();
    glBindTexture(GL_TEXTURE_2D, m_ColorAttachments[0]);
    DrawQuadMesh();
}

void RenderPipeline::SetBufferUniform(string name, int index)
{
    for (auto buffer : m_OpaqueRenderList)
    {
        unsigned int uniformBlock = glGetUniformBlockIndex(buffer->shader->ID, name.c_str());
        glUniformBlockBinding(buffer->shader->ID, uniformBlock, index);
    }
    unsigned int uniformBlock = glGetUniformBlockIndex(m_SkyboxShader->ID, name.c_str());
    glUniformBlockBinding(m_SkyboxShader->ID, uniformBlock, index);
}



