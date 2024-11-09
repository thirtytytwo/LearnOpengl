#include<glad/glad.h>
#include<glfw3.h>
#include <iostream>
#include "Shader.h"
#include "Camera.h"
#include "GameObject.h"
#include "Light.h"
#include "RenderPipeline.h"

void FramebufferSizeCallback(GLFWwindow* window, int width, int height);
void MouseCallback(GLFWwindow* window, double xposIn, double yposIn);
void KeyboradInput(GLFWwindow* window);

Camera camera = Camera();

float screenWidth = 1920;
float screenHeight = 1080;

float deltaTime = 0.f;
float lastFrame = 0.f;

bool firstMouse = true;
float lastX = screenWidth/ 2.0f;
float lastY = screenHeight/ 2.0f;
float mouseSensitive = 0.2f;
int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(screenWidth, screenHeight, "LearnOpenGL", NULL, NULL);
    if(window == NULL)
    {
        std::cout << "Failed to create a window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, FramebufferSizeCallback);
    glfwSetCursorPosCallback(window, MouseCallback);

    if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to init glad" << std::endl;
        return -1;
    }
    
    Light light = Light();
    GameObject testObject = GameObject(vec3(0.f), vec3(0.0f), vec3(0.5f));
    testObject.SetupMeshRenderer("nanosuit");
	camera.SetPosition(glm::vec3(0.0f, 5.0f, 3.0f));

    RenderPipeline::GetInstance().Setup(camera, light);
    //Framebuffer 章节
    
    Shader shader = Shader("ResolveToScreen");
    shader.use();
    shader.SetInt("screenTexture", 0);
    
    unsigned int framebuffer;
    glGenFramebuffers(1, &framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

    Texture skyboxTexture = Texture("sunshine", Cube);
    Buffer* skyboxBuffer = new Buffer();
    Material skyboxMaterial = Material({skyboxTexture}, "Skybox");
    skyboxMaterial.Setup(*skyboxBuffer);
    RenderPipeline::GetInstance().EnqueueBuffer(*skyboxBuffer, Skybox);

    

    //在这里 color 和 depth 是不同的格式去绑定，color是texture depth 是renderbuffer，应该是color可以后续我们用于采样，depth不需要所以用renderbuffer(猜测)
    unsigned int colorAttachment;
    glGenTextures(1, &colorAttachment);
    glBindTexture(GL_TEXTURE_2D, colorAttachment);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, screenWidth, screenHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorAttachment, 0);

    unsigned int depthAttachment;
    glGenRenderbuffers(1, &depthAttachment);
    glBindRenderbuffer(GL_RENDERBUFFER, depthAttachment);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, screenWidth, screenHeight);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, depthAttachment);

    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        std::cout << "Framebuffer is not complete" << std::endl;
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    while (!glfwWindowShouldClose(window))
    {
        float curFrame = static_cast<float>(glfwGetTime());
        deltaTime = curFrame - lastFrame;
        lastFrame = curFrame;

        KeyboradInput(window);
        camera.SetAspectRatio((screenWidth / screenHeight));
        
        
        glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		testObject.Render(camera, light);

        RenderPipeline::GetInstance().Render();
        //物体已经渲染到指定的framebuffer上，现在需要把framebuffer中的值拷贝到最终的屏幕上
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glDisable(GL_DEPTH_TEST); // disable depth test so screen-space quad isn't discarded due to depth test.
        // clear all relevant buffers
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f); // set clear color to white (not really necessary actually, since we won't be able to see behind the quad anyways)
        glClear(GL_COLOR_BUFFER_BIT);

        shader.use();
        glBindVertexArray(quadVAO);
        glBindTexture(GL_TEXTURE_2D, colorAttachment);	// use the color attachment texture as the texture of the quad plane
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glDeleteVertexArrays(1, &quadVAO);
    glDeleteBuffers(1, &quadVBO);
    glDeleteRenderbuffers(1, &depthAttachment);
    glDeleteFramebuffers(1, &framebuffer);
    glfwTerminate();
    return 0;
}

void FramebufferSizeCallback(GLFWwindow* window, int width, int height)
{
    screenHeight = static_cast<float>(height);
    screenWidth = static_cast<float>(width);
    glViewport(0, 0, width, height);
}

void KeyboradInput(GLFWwindow* window)
{
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, true);
    }

    if(glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    {
        camera.Translate(vec3(0, 0, -1), 5.f, deltaTime);
    }
    if(glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    {
        camera.Translate(vec3(0, 0, 1), 5.f, deltaTime);
    }
    if(glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    {
        camera.Translate(vec3(1, 0, 0), 5.f, deltaTime);
    }
    if(glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    {
        camera.Translate(vec3(-1, 0, 0), 5.f, deltaTime);
    }
    if(glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
    {
        camera.Translate(vec3(0, -1, 0), 5.f, deltaTime);
    }
    if(glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
    {
        camera.Translate(vec3(0, 1, 0), 5.f, deltaTime);
    }
}

void MouseCallback(GLFWwindow* window, double xposIn, double yposIn)
{
    if(!glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT))
    {
        firstMouse = true;
        return;
    }
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;

    camera.Rotate(xoffset * mouseSensitive, yoffset * mouseSensitive);
}

