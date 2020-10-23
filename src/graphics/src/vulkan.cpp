//g++ vulkan.cpp -o result -lglfw -lvulkan && ./result

#define GLFW_INCLUDE_VULKAN
#include<GLFW/glfw3.h> 

#include<stdexcept>
#include<vector>
#include<iostream>
#include<string>

#include"../include/VulkanRenderer.hpp"

GLFWwindow * window;
VulkanRenderer vulkanRenderer;

void initWindow(std::string wName = "Window", const int width = 800, const int height = 600)
{
    if(!glfwInit())
    {

    }

    glfwWindowHint(GLFW_CLIENT_API,GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    window = glfwCreateWindow(width, height, wName.c_str(), nullptr, nullptr);
}

int main() 
{
    initWindow();
    if(vulkanRenderer.init(window) == EXIT_FAILURE)
    {
        return EXIT_FAILURE;    
    }

    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();
    }

    printf("Window is Closed\n");
    vulkanRenderer.kill();    

    glfwDestroyWindow(window);
    glfwTerminate();
    
    return 0;
}
