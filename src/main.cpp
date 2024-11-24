#define STB_IMAGE_IMPLEMENTATION

#include <glad/glad.h>
#include <glfw3.h>
#include <iostream>
#include "Shader.h"
#include "Camera.h"
#include "Light.h"
#include "GameObject.h"
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
    testObject.Setup();
	camera.SetPosition(glm::vec3(0.0f, 5.0f, 3.0f));

    RenderPipeline::GetInstance().Setup(camera, light);
    //Framebuffer 章节
    
    // Shader shader = Shader("ResolveToScreen");
    // shader.use();
    // shader.SetInt("screenTexture", 0);
    // Buffer skyboxBuffer = Buffer();
    // Material skyboxMaterial = Material(, "Skybox");
    // skyboxMaterial.Setup(skyboxBuffer);
    // RenderPipeline::GetInstance().EnqueueBuffer(, Skybox);
    //
    // Texture skyboxTexture = Texture("sunshine", Cube);
    // Buffer skyboxBuffer = Buffer();
    // Material skyboxMaterial = Material({skyboxTexture}, "Skybox");
    // skyboxMaterial.Setup(skyboxBuffer);
    // RenderPipeline::GetInstance().EnqueueBuffer(skyboxBuffer, Skybox);

    
    while (!glfwWindowShouldClose(window))
    {
        float curFrame = static_cast<float>(glfwGetTime());
        deltaTime = curFrame - lastFrame;
        lastFrame = curFrame;

        KeyboradInput(window);
        camera.SetAspectRatio((screenWidth / screenHeight));
        
        RenderPipeline::GetInstance().BeginRender();
        RenderPipeline::GetInstance().Render();
        
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    RenderPipeline::GetInstance().FinishRender();
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

