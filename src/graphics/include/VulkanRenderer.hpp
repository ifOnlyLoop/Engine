#pragma once

#define GLFW_INCLUDE_VULKAN
#include<GLFW/glfw3.h> 

#include<stdexcept>
#include<set>
#include<vector>
#include<algorithm>
#include<string.h>

#include"./Utilities.hpp"

class VulkanRenderer
{
public:
     VulkanRenderer(/* args */);
    ~VulkanRenderer();

private:
    GLFWwindow * window;
    
    VkInstance instance;
    struct {
        VkPhysicalDevice physicalDevice;
        VkDevice logicalDevice;
    } mainDevice;

    VkQueue 
        graphicsQueue,
        presentationQueue;

    VkSurfaceKHR surface;
    VkSwapchainKHR swapChain;

    VkFormat swapChainImageFormat;
    VkExtent2D swapChainExtent;

    std::vector<SwapChainImage> swapChainImages;


public:
    int init(GLFWwindow * newWindow);
    void kill() {killProcess();}

private:
    // - create methods
    void createInstance();
    void createLogicalDevice();
    void createSurface();
    void createSwapChain();
    // - support methods
    bool checkInstanceExtentionSupport(std::vector<const char*>* checkExtentions);
    bool checkDeviceSuitable(VkPhysicalDevice device);
    bool checkDeviceExtensionSupport(VkPhysicalDevice device);    
    // - get methods
    void getPhysicalDevice();
    void getQueueFamilyIndicies(VkPhysicalDevice device, QueueFamilyIndicies& Indicies);
    void getSwapChainDetails(VkPhysicalDevice device, SwapChainDetails& SwapChainDetails);
    // - swapchain
    void createImageView(VkImage& image, VkFormat& format, VkImageAspectFlags aspectFlags, VkImageView& imageView);
    static VkSurfaceFormatKHR bestSurfaceFormat(const std::vector<VkSurfaceFormatKHR> &formats);
    static VkPresentModeKHR bestPresentMode(const std::vector<VkPresentModeKHR> &modes);
    static VkExtent2D swapExtent(const VkSurfaceCapabilitiesKHR &surfaceCapabilities, GLFWwindow* window);
    // - clean code
    void killProcess();
};

VulkanRenderer::VulkanRenderer(/* args */)
{
}

VulkanRenderer::~VulkanRenderer()
{
    killProcess();
}

int VulkanRenderer::init(GLFWwindow * newWindow)
{
    window = newWindow;
    try
    {
        createInstance();
        createSurface();
        getPhysicalDevice();  
        createLogicalDevice();
        createSwapChain();  
    }
    catch(const std::runtime_error& e)
    {
        printf("ERROR: %s\n", e.what());
        return EXIT_FAILURE;
    }
    
    return EXIT_SUCCESS;
}

void VulkanRenderer::createInstance()
{
    // Info about our app for developers, does not affect program
    VkApplicationInfo appInfo = {};    
        appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        appInfo.pApplicationName = "Vulkan Application";
        appInfo.applicationVersion =  VK_MAKE_VERSION(0,0,1);
        appInfo.pEngineName = "LiFE";
        appInfo.engineVersion = VK_MAKE_VERSION(0,0,1);
        appInfo.apiVersion = VK_API_VERSION_1_1;

    VkInstanceCreateInfo createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        createInfo.pApplicationInfo = &appInfo;
        
    // for glfw
    std::vector<const char*> instanceExtentions = std::vector<const char*>();

    uint32_t glfwExtensionCount = 0;
    const char** glfwExtensions;

    glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);  
    for (size_t i = 0; i < glfwExtensionCount; i++)
    {
        instanceExtentions.push_back(glfwExtensions[i]);
    }

    if (!checkInstanceExtentionSupport(&instanceExtentions))
    {
        throw std::runtime_error("a glfw extention is not supported in vulkan!");
    }

        createInfo.enabledExtensionCount = static_cast<uint32_t>(instanceExtentions.size());
        createInfo.ppEnabledExtensionNames = instanceExtentions.data();    
        // TODO: setup validation layer;
        createInfo.enabledLayerCount = 0;
        createInfo.ppEnabledLayerNames = nullptr;

    if(vkCreateInstance(&createInfo, nullptr, &instance) != VK_SUCCESS) 
    {
        throw std::runtime_error("failed to create vulkan instance\n");
    }
}

bool VulkanRenderer::checkInstanceExtentionSupport(std::vector<const char*>* checkExtentions)
{
    uint32_t extensionCount = 0;
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);

    std::vector<VkExtensionProperties> VkExtensions(extensionCount);
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, VkExtensions.data());

    for(const auto &extention: *checkExtentions)
    {   
        bool flag = false;
        for(const auto& VkExtension: VkExtensions)
            if(strcmp(VkExtension.extensionName, extention))
            {
                flag = true;
                break;
            }
        
        if(!flag)
            return false;
    }
    return true;
}

void VulkanRenderer::createSurface()
{
    if(glfwCreateWindowSurface(instance,window,nullptr,&surface)!=VK_SUCCESS)
    {
        throw std::runtime_error("failed to create glfw vk surface\n");
    }  
    printf("succeeded to creat window surface \n");
}

void VulkanRenderer::createSwapChain()
{
    SwapChainDetails swapChainDetails;
    getSwapChainDetails(mainDevice.physicalDevice, swapChainDetails);

    auto BSF = VulkanRenderer::bestSurfaceFormat(swapChainDetails.formats);
    auto BPM = VulkanRenderer::bestPresentMode(swapChainDetails.presentationModes);
    auto BSE = VulkanRenderer::swapExtent(swapChainDetails.surfaceCapabilities,window);   

    uint32_t imageCount = swapChainDetails.surfaceCapabilities.minImageCount;
    if(
        swapChainDetails.surfaceCapabilities.maxImageCount > 0 && // == 0 limitless
        swapChainDetails.surfaceCapabilities.maxImageCount < imageCount
    ){
        imageCount = swapChainDetails.surfaceCapabilities.maxImageCount;
    }   //std::cout<<imageCount<<std::endl;

    VkSwapchainCreateInfoKHR swapchainCreateInfo = {};
        swapchainCreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        swapchainCreateInfo.imageFormat = BSF.format;
        swapchainCreateInfo.imageColorSpace = BSF.colorSpace;
        swapchainCreateInfo.presentMode = BPM;
        swapchainCreateInfo.imageExtent = BSE;
        swapchainCreateInfo.minImageCount = imageCount;
        swapchainCreateInfo.imageArrayLayers = 1;
        swapchainCreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
        swapchainCreateInfo.preTransform = swapChainDetails.surfaceCapabilities.currentTransform;
        swapchainCreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
        swapchainCreateInfo.clipped = VK_TRUE;
        swapchainCreateInfo.oldSwapchain = VK_NULL_HANDLE;
        swapchainCreateInfo.surface = surface;

    QueueFamilyIndicies indices;
    getQueueFamilyIndicies(mainDevice.physicalDevice,indices);

    if(indices.graphicsFamily != indices.presentationFamily)
    {
        uint32_t queueFamilyIndicies[] = {
            (uint32_t)indices.graphicsFamily,
            (uint32_t)indices.presentationFamily
        };

        swapchainCreateInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        swapchainCreateInfo.queueFamilyIndexCount = 2;
        swapchainCreateInfo.pQueueFamilyIndices = queueFamilyIndicies;
    }   else
    {
        swapchainCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        swapchainCreateInfo.queueFamilyIndexCount = 0;
        swapchainCreateInfo.pQueueFamilyIndices = nullptr;   
    }
    
    if(vkCreateSwapchainKHR(mainDevice.logicalDevice,&swapchainCreateInfo,nullptr,&swapChain) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create swapchain\n");
    }

    swapChainExtent = BSE;
    swapChainImageFormat = BSF.format;

    uint32_t swapChainImageCount;
    vkGetSwapchainImagesKHR(mainDevice.logicalDevice,swapChain,&swapChainImageCount,nullptr);
    std::vector<VkImage> images;
    vkGetSwapchainImagesKHR(mainDevice.logicalDevice,swapChain,&swapChainImageCount,images.data());
    
    for(auto& image:images)
    {
        SwapChainImage swapChainImage = {};
        swapChainImage.image = image;

        createImageView(image,swapChainImageFormat,VK_IMAGE_ASPECT_COLOR_BIT,swapChainImage.imageView);
        swapChainImages.push_back(swapChainImage);
    }
    printf("SwapChain was created\n");
}

void VulkanRenderer::getPhysicalDevice()
{
    uint32_t deviceCount = 0;
    vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);
    
    if(!deviceCount)
    {
        throw std::runtime_error("can not find devices with vulkan support!");
    }

    std::vector<VkPhysicalDevice> deviceList(deviceCount);
    vkEnumeratePhysicalDevices(instance, &deviceCount, deviceList.data());
    
    bool flag = false;

    for (int i = 0; i < deviceCount; i++)
    {
        if(checkDeviceSuitable(deviceList[i]) == true)
        {
            mainDevice.physicalDevice = deviceList[i];
            flag = true;
            break;
        }
    }

    if(!flag)
    {
        throw std::runtime_error("no physical device is suitable!");
    }
    
    printf("physical device with vulkan support was found\n");
}

bool VulkanRenderer::checkDeviceSuitable(VkPhysicalDevice device)
{
    /*
    VkPhysicalDeviceProperties deviceProperties;
    vkGetPhysicalDeviceProperties(device,&deviceProperties); 
    VkPhysicalDeviceFeatures deviceFeatures;
    vkGetPhysicalDeviceFeatures(device,&deviceFeatures); 
    */

    QueueFamilyIndicies indecies;
    getQueueFamilyIndicies(device,indecies);
    
    SwapChainDetails swapChainDetails;
    getSwapChainDetails(device, swapChainDetails);

    return 
        indecies.isValid() && 
        swapChainDetails.isValid() && 
        checkDeviceExtensionSupport(device);
}

void VulkanRenderer::getQueueFamilyIndicies(VkPhysicalDevice device,  QueueFamilyIndicies& indecies)
{
    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr); 
    std::vector<VkQueueFamilyProperties> queueFamilyList(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilyList.data());

    if(!queueFamilyCount)
    {
        return;   
    }

    int idx=0;
    for(const auto& queueFamily:queueFamilyList)
    {
        if(queueFamily.queueCount > 0 && (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT))
        {
            indecies.graphicsFamily = idx;
        }   

        VkBool32 isPresentationSupportKHR = false;
        vkGetPhysicalDeviceSurfaceSupportKHR(device,idx,surface,&isPresentationSupportKHR);

        if(queueFamily.queueCount > 0 && isPresentationSupportKHR)
        {
            indecies.presentationFamily = idx;
        }

        if(indecies.isValid())
        {
            break;
        }
        
        idx++;
    } 
}

void VulkanRenderer::getSwapChainDetails(VkPhysicalDevice device, SwapChainDetails& SwapChainDetails)
{
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device,surface,&SwapChainDetails.surfaceCapabilities);
    
    uint32_t formatCount = 0;
    vkGetPhysicalDeviceSurfaceFormatsKHR(device,surface,&formatCount,nullptr);
    if(formatCount)
    {
        SwapChainDetails.formats.resize(formatCount);
        vkGetPhysicalDeviceSurfaceFormatsKHR(device,surface,&formatCount,SwapChainDetails.formats.data());
    }

    uint32_t modeCount = 0;
    vkGetPhysicalDeviceSurfacePresentModesKHR(device,surface,&modeCount,nullptr);
    if(formatCount)
    {
        SwapChainDetails.presentationModes.resize(modeCount);
        vkGetPhysicalDeviceSurfacePresentModesKHR(device,surface,&modeCount,SwapChainDetails.presentationModes.data());
    }

    //printf("formatCount: %d, modeCount: %d\n",formatCount,modeCount);
}

bool VulkanRenderer::checkDeviceExtensionSupport(VkPhysicalDevice device)
{
    uint32_t extensionCount = 0;
    vkEnumerateDeviceExtensionProperties(device,nullptr,&extensionCount,nullptr);
    
    if(!extensionCount)
    {
        return false;
    }

    std::vector<VkExtensionProperties> extensions(extensionCount);
    vkEnumerateDeviceExtensionProperties(device,nullptr,&extensionCount,extensions.data());

    for (const auto& deviceExtension: deviceExtensions)
    {
        bool hasExtension = false;
        for(const auto& extension : extensions)
        {
            if(strcmp(deviceExtension,extension.extensionName) == 0)
            {
                hasExtension = true;
                break;
            }
        }

        if(!hasExtension)
        {
            printf("Extension %s is not supported\n",deviceExtension);
            return false;
        }
        else
        {
            printf("Extension %s is supported\n",deviceExtension);
        }
        
    }

    return true;
}

void VulkanRenderer::createLogicalDevice()
{    
    QueueFamilyIndicies indices;
    getQueueFamilyIndicies(mainDevice.physicalDevice, indices);
    
    VkPhysicalDeviceFeatures physicalDeviceFeatures = {};

    // can not create QueueCreateInfo with same family idx
    std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
    std::set<int> queueFamiliesIndices = 
    {
        indices.graphicsFamily,
        indices.presentationFamily
    };  
         
    for(int idx: queueFamiliesIndices)
    {
        float priority = 1.0f;    
        VkDeviceQueueCreateInfo queueCreateInfo = {};
            queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
            queueCreateInfo.queueFamilyIndex = idx;
            queueCreateInfo.queueCount = 1;
            queueCreateInfo.pQueuePriorities = &priority;

        queueCreateInfos.push_back(queueCreateInfo);
    }

    VkDeviceCreateInfo deviceCreateInfo = {};
        deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        deviceCreateInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
        deviceCreateInfo.pQueueCreateInfos = queueCreateInfos.data();
        deviceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
        deviceCreateInfo.ppEnabledExtensionNames = deviceExtensions.data(); 
        deviceCreateInfo.pEnabledFeatures = &physicalDeviceFeatures;

    VkResult result = vkCreateDevice(mainDevice.physicalDevice, &deviceCreateInfo, nullptr, &mainDevice.logicalDevice);

    if(result != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create logical device!");
    }

    vkGetDeviceQueue(mainDevice.logicalDevice, indices.graphicsFamily, 0, &graphicsQueue);
    vkGetDeviceQueue(mainDevice.logicalDevice, indices.presentationFamily, 0, &presentationQueue);

    printf("logical device was created\n");
}

void VulkanRenderer::killProcess()
{
    vkDestroyInstance(instance,nullptr);
    vkDestroyDevice(mainDevice.logicalDevice,nullptr);
    vkDestroySurfaceKHR(instance,surface,nullptr);
    vkDestroySwapchainKHR(mainDevice.logicalDevice,swapChain,nullptr);
    
    for(auto& image:swapChainImages)
        vkDestroyImageView(mainDevice.logicalDevice,image.imageView,nullptr);
    
    printf("Memory Released\n");
}

VkSurfaceFormatKHR VulkanRenderer::bestSurfaceFormat(const std::vector<VkSurfaceFormatKHR> &formats)
    {
        VkSurfaceFormatKHR best = formats[0];

        if(formats.size() == 0 && formats[0].format == VK_FORMAT_UNDEFINED)
        {
            best = { VK_FORMAT_R8G8B8A8_UNORM, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR };
        }
        
        else for(const auto& format:formats)
        {
            if(
                (format.format == VK_FORMAT_R8G8B8A8_UNORM  || 
                 format.format == VK_FORMAT_B8G8R8A8_UNORM) &&
                format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR   
            ){
                best = format;
                break;
            }
        }
        
        return best;
    }; 
    
VkPresentModeKHR VulkanRenderer::bestPresentMode(const std::vector<VkPresentModeKHR> &modes)
{
    for(const auto& mode:modes)
    {
        if(mode == VK_PRESENT_MODE_MAILBOX_KHR)
        {
            return mode;
        }
    }
    return VK_PRESENT_MODE_FIFO_KHR;
};
    
VkExtent2D VulkanRenderer::swapExtent(const VkSurfaceCapabilitiesKHR &surfaceCapabilities, GLFWwindow* window)
{
    if(surfaceCapabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
    {
        return surfaceCapabilities.currentExtent;
    }        

    int width, height;
    glfwGetFramebufferSize(window,&width,&height);
    
    VkExtent2D newExtent = {};
        newExtent.width  = static_cast<u_int32_t>(width);
        newExtent.height = static_cast<u_int32_t>(height);
    
    newExtent.width  = std::max(
        surfaceCapabilities.minImageExtent.width,
        std::min(
            newExtent.width ,
            surfaceCapabilities.maxImageExtent.width
        )
    );
    
    newExtent.height  = std::max(
        surfaceCapabilities.minImageExtent.height,
        std::min(
            newExtent.height ,
            surfaceCapabilities.maxImageExtent.height
        )
    );
    
    return newExtent;
};

void VulkanRenderer::createImageView(VkImage& image, VkFormat& format, VkImageAspectFlags aspectFlags, VkImageView& imageView)
{
    VkImageViewCreateInfo viewCreateInfo = {};
        viewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        viewCreateInfo.image = image;
        viewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        viewCreateInfo.format = format;

        viewCreateInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
        viewCreateInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
        viewCreateInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
        viewCreateInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
        
        viewCreateInfo.subresourceRange.aspectMask = aspectFlags;
        viewCreateInfo.subresourceRange.baseMipLevel = 0;
        viewCreateInfo.subresourceRange.levelCount = 1;
        viewCreateInfo.subresourceRange.baseArrayLayer = 0;
        viewCreateInfo.subresourceRange.layerCount = 1;
        
        if(vkCreateImageView(mainDevice.logicalDevice,&viewCreateInfo,nullptr,&imageView) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to create Image View\n");
        }
        
}