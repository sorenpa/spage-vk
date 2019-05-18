
#include <vulkan/vulkan.h>
#include <vector>
#include <optional>
#include <WeakReference.h>

#include "../Devices/Surface.h"
#include "../Devices/PhysicalDevice.h"
#include "../Devices/LogicalDevice.h"

#pragma once
class SwapChain
{
public:

	const VkExtent2D GetExtent() const { return m_extent; };

	SwapChain(const Surface* surface, const PhysicalDevice* physicalDevice, const LogicalDevice* logicalDevice, const VkExtent2D& extent);
	~SwapChain();

private:

	void ChooseSurfaceFormat();
	void ChoosePhysicalDevicePresentMode();
	void CreateImageViews();

	// Surface
	const Surface* m_surface;
	VkSurfaceCapabilitiesKHR m_surfaceCapabilities;
	VkSurfaceFormatKHR m_surfaceFormat;

	// Physical Device
	const PhysicalDevice* m_physicalDevice;

	// LogicalDevice
	const LogicalDevice* m_logicalDevice;

	// Swap Chain
	VkSwapchainKHR m_swapChain;

	VkExtent2D m_extent;
	VkPresentModeKHR m_presentMode;
	VkSurfaceTransformFlagsKHR m_preTransform;
	VkCompositeAlphaFlagBitsKHR m_compositeAlpha;

	uint32_t m_imageCount;
	
	std::vector<VkImage> m_images;
	std::vector<VkImageView> m_imageViews;
	
	// VkFence m_fenceImage;
	// uint32_t m_activeImageIndex;
};

