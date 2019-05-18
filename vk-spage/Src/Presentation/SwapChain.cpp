#include "SwapChain.h"


static const std::vector<VkCompositeAlphaFlagBitsKHR> COMPOSITE_ALPHA_FLAGS = { VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR, VK_COMPOSITE_ALPHA_PRE_MULTIPLIED_BIT_KHR,
	VK_COMPOSITE_ALPHA_POST_MULTIPLIED_BIT_KHR, VK_COMPOSITE_ALPHA_INHERIT_BIT_KHR, };

SwapChain::SwapChain(const Surface * surface, const PhysicalDevice * physicalDevice, const LogicalDevice* logicalDevice, const VkExtent2D& extent) :
	//	m_fenceImage(VK_NULL_HANDLE),
	//	m_activeImageIndex(std::numeric_limits<uint32_t>::max()),
	m_extent(extent),
	m_surfaceCapabilities({}),
	m_surfaceFormat({}),
	m_surface(surface),
	m_physicalDevice(physicalDevice),
	m_logicalDevice(logicalDevice),
	m_presentMode(VK_PRESENT_MODE_FIFO_KHR),
	m_imageCount(0),
	m_preTransform(VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR),
	m_compositeAlpha(VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR),
	m_swapChain(VK_NULL_HANDLE)


{

	// Get surface capabilities based on selected Physical Device
	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(m_physicalDevice->GetPhysicalDevice(), m_surface->GetSurface(), &m_surfaceCapabilities);

	// Choose surface format based on selected Physical Device
	ChooseSurfaceFormat();

	// Choose Physical Device present mode
	ChoosePhysicalDevicePresentMode();

	// TODO: The tutorial checks the extend against capabilities for some limits. We might need to do this at some point.

	// The desired imageCount is min +1;
	uint32_t imageCount = m_surfaceCapabilities.minImageCount + 1;

	// maxImageCount = 0 is a special value that means that there is no maximum
	if (m_surfaceCapabilities.maxImageCount > 0 && imageCount > m_surfaceCapabilities.maxImageCount)
	{
		imageCount = m_surfaceCapabilities.maxImageCount;
	}

	if (m_surfaceCapabilities.supportedTransforms & VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR)
	{
		// We prefer a non-rotated transform.
		m_preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
	}
	else
	{
		m_preTransform = m_surfaceCapabilities.currentTransform;
	}

	for (const auto &compositeAlphaFlag : COMPOSITE_ALPHA_FLAGS)
	{
		if (m_surfaceCapabilities.supportedCompositeAlpha & compositeAlphaFlag)
		{
			m_compositeAlpha = compositeAlphaFlag;
			break;
		}
	}

	VkSwapchainCreateInfoKHR swapChainCreateInfo = {};
	swapChainCreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	swapChainCreateInfo.surface = m_surface->GetSurface();
	swapChainCreateInfo.minImageCount = imageCount;
	swapChainCreateInfo.imageFormat = m_surfaceFormat.format;
	swapChainCreateInfo.imageColorSpace = m_surfaceFormat.colorSpace;
	swapChainCreateInfo.imageExtent = m_extent;
	swapChainCreateInfo.imageArrayLayers = 1;
	swapChainCreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
	swapChainCreateInfo.preTransform = static_cast<VkSurfaceTransformFlagBitsKHR>(m_preTransform);
	swapChainCreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	swapChainCreateInfo.compositeAlpha = m_compositeAlpha;
	swapChainCreateInfo.presentMode = m_presentMode;
	swapChainCreateInfo.clipped = VK_TRUE;
	swapChainCreateInfo.oldSwapchain = VK_NULL_HANDLE;

	if (m_surfaceCapabilities.supportedUsageFlags & VK_IMAGE_USAGE_TRANSFER_SRC_BIT)
	{
		swapChainCreateInfo.imageUsage |= VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
	}

	if (m_surfaceCapabilities.supportedUsageFlags & VK_IMAGE_USAGE_TRANSFER_DST_BIT)
	{
		swapChainCreateInfo.imageUsage |= VK_IMAGE_USAGE_TRANSFER_DST_BIT;
	}

	uint32_t graphicsFamily = m_logicalDevice->GetGraphicsFamily();
	uint32_t presentFamily = m_logicalDevice->GetPresentFamily();

	if (graphicsFamily != presentFamily) {
		std::vector<uint32_t> queueFamilies = { graphicsFamily, presentFamily };
		swapChainCreateInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
		swapChainCreateInfo.queueFamilyIndexCount = static_cast<uint32_t>(queueFamilies.size());
		swapChainCreateInfo.pQueueFamilyIndices = queueFamilies.data();
	}
	else {
		swapChainCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
		swapChainCreateInfo.queueFamilyIndexCount = 0; // Optional
		swapChainCreateInfo.pQueueFamilyIndices = nullptr; // Optional
	}

	if (vkCreateSwapchainKHR(m_logicalDevice->GetLogicaDevice(), &swapChainCreateInfo, nullptr, &m_swapChain) != VK_SUCCESS) {
		throw std::runtime_error("failed to create swap chain!");
	}

	// Get images
	vkGetSwapchainImagesKHR(m_logicalDevice->GetLogicaDevice(), m_swapChain, &imageCount, nullptr);
	m_images.resize(imageCount);
	vkGetSwapchainImagesKHR(m_logicalDevice->GetLogicaDevice(), m_swapChain, &imageCount, m_images.data());

	// Create imageViews
	CreateImageViews();

	// VkFenceCreateInfo fenceCreateInfo = {};
	// fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	// vkCreateFence(m_logicalDevice->GetLogicaDevice(), &fenceCreateInfo, nullptr, &m_fenceImage);
}

SwapChain::~SwapChain()
{
	for (auto imageView : m_imageViews) {
		vkDestroyImageView(m_logicalDevice->GetLogicaDevice(), imageView, nullptr);
	}

	vkDestroySwapchainKHR(m_logicalDevice->GetLogicaDevice(), m_swapChain, nullptr);
}

void SwapChain::ChooseSurfaceFormat() {
	uint32_t surfaceFormatCount;
	vkGetPhysicalDeviceSurfaceFormatsKHR(m_physicalDevice->GetPhysicalDevice(), m_surface->GetSurface(), &surfaceFormatCount, nullptr);

	// PhysicalDevice selection checks for availablility of at least one surface format, but we double check here to be safe.
	if (surfaceFormatCount == 0) {
		throw std::runtime_error("failed to find any surface formats!");
	}

	std::vector<VkSurfaceFormatKHR> surfaceFormats(surfaceFormatCount);
	vkGetPhysicalDeviceSurfaceFormatsKHR(m_physicalDevice->GetPhysicalDevice(), m_surface->GetSurface(), &surfaceFormatCount, surfaceFormats.data());

	// The best case scenario is that the surface has no preferred format, 
	// which Vulkan indicates by only returning one VkSurfaceFormatKHR entry 
	// which has its format member set to VK_FORMAT_UNDEFINED.
	if ((surfaceFormatCount == 1) && (surfaceFormats[0].format == VK_FORMAT_UNDEFINED))
	{
		 m_surfaceFormat.format = VK_FORMAT_B8G8R8A8_UNORM;
		 m_surfaceFormat.colorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
	}
	else
	{
		// Iterate over the list of available surface format and
		// check for the presence of VK_FORMAT_B8G8R8A8_UNORM
		bool found_B8G8R8A8_UNORM = false;

		for (auto &&surfaceFormat : surfaceFormats)
		{
			if (surfaceFormat.format == VK_FORMAT_B8G8R8A8_UNORM)
			{
				m_surfaceFormat.format = surfaceFormat.format;
				m_surfaceFormat.colorSpace = surfaceFormat.colorSpace;
				found_B8G8R8A8_UNORM = true;
				break;
			}
		}

		// In case VK_FORMAT_B8G8R8A8_UNORM is not available
		// select the first available color format
		if (!found_B8G8R8A8_UNORM)
		{
			 m_surfaceFormat.format = surfaceFormats[0].format;
			 m_surfaceFormat.colorSpace = surfaceFormats[0].colorSpace;
		}
	}


}

void SwapChain::ChoosePhysicalDevicePresentMode() {

	uint32_t presentModeCount = 0;
	vkGetPhysicalDeviceSurfacePresentModesKHR(m_physicalDevice->GetPhysicalDevice(), m_surface->GetSurface(), &presentModeCount, nullptr);

	// PhysicalDevice selection checks for availablility of at least one present mode, but we double check here to be safe.
	if (presentModeCount == 0) {
		throw std::runtime_error("failed to find any present modes for the selected physical device!");
	}

	std::vector<VkPresentModeKHR> physicalDevicePresentModes(presentModeCount);
	vkGetPhysicalDeviceSurfacePresentModesKHR(m_physicalDevice->GetPhysicalDevice(), m_surface->GetSurface(), &presentModeCount, physicalDevicePresentModes.data());
	
	for (const auto &presentMode : physicalDevicePresentModes)
	{
		if (presentMode == VK_PRESENT_MODE_MAILBOX_KHR)
		{
			m_presentMode = presentMode;
			break;
		}
	
		if (m_presentMode != VK_PRESENT_MODE_MAILBOX_KHR && presentMode == VK_PRESENT_MODE_IMMEDIATE_KHR)
		{
			m_presentMode = presentMode;
		}
	}

}

void SwapChain::CreateImageViews() {
	m_imageViews.resize(m_images.size());

	for (size_t i = 0; i < m_images.size(); i++) {
		VkImageViewCreateInfo createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		createInfo.image = m_images[i];
		createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		createInfo.format = m_surfaceFormat.format;
		createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		createInfo.subresourceRange.baseMipLevel = 0;
		createInfo.subresourceRange.levelCount = 1;
		createInfo.subresourceRange.baseArrayLayer = 0;
		createInfo.subresourceRange.layerCount = 1;

		if (vkCreateImageView(m_logicalDevice->GetLogicaDevice(), &createInfo, nullptr, &m_imageViews[i]) != VK_SUCCESS) {
			throw std::runtime_error("failed to create image views!");
		}
	}
}