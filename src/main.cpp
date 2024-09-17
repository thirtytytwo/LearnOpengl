#include<glad/glad.h>
#include<glfw3.h>
#include <iostream>
#include "Shader.h"
#include "Camera.h"
#include "GameObject.h"
#include "Light.h"

void FramebufferSizeCallback(GLFWwindow* window, int width, int height);

int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(800, 600, "LearnOpenGL", NULL, NULL);
    if(window == NULL)
    {
        std::cout << "Failed to create a window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, FramebufferSizeCallback);

    if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to init glad" << std::endl;
        return -1;
    }
    
	glEnable(GL_DEPTH_TEST);
	Camera camera = Camera();
    Light light = Light();
    GameObject testObject = GameObject(vec3(0.f), vec3(0.0f), vec3(0.5f), "nanosuit");
	camera.SetPosition(glm::vec3(0.0f, 1.0f, 3.0f));

    
    
    while (!glfwWindowShouldClose(window))
    {
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		testObject.Render(camera, light);
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    
    //链接上去了之后就不需要着色对象了
    //glDeleteVertexArrays(1, &VAO);
    //glDeleteBuffers(1, &VBO);
    //glDeleteBuffers(1, &EBO);
    
    glfwTerminate();
    return 0;
}

void FramebufferSizeCallback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

