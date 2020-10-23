#pragma once

#include<vector>

const std::vector<const char*> deviceExtensions =
{
    VK_KHR_SWAPCHAIN_EXTENSION_NAME
};

struct QueueFamilyIndicies
{
    int graphicsFamily = -1;
    int presentationFamily = -1;

    bool isValid()
    {
        return 
            graphicsFamily >= 0 &&
            presentationFamily >= 0;
    }
};

struct SwapChainDetails
{
    VkSurfaceCapabilitiesKHR surfaceCapabilities;    
    std::vector<VkSurfaceFormatKHR> formats;  // image format
    std::vector<VkPresentModeKHR> presentationModes;

    bool isValid()
    {
        return 
            formats.size() > 0 &&
            presentationModes.size() > 0;
    } 
};

struct SwapChainImage
{
    VkImage image;
    VkImageView imageView;
};
