#include "LogicalDevice.h"


LogicalDevice::LogicalDevice(const Instance* instance, const PhysicalDevice* physicalDevice, const Surface* surface):
	m_physicalDevice(physicalDevice),
	m_instance(instance),
	m_surface(surface),
	m_logicalDevice(VK_NULL_HANDLE),
	m_supportedQueues(0),
	m_graphicsFamily(0),
	m_presentFamily(0),
	m_computeFamily(0),
	m_transferFamily(0),
	m_graphicsQueue(VK_NULL_HANDLE),
	m_presentQueue(VK_NULL_HANDLE),
	m_computeQueue(VK_NULL_HANDLE),
	m_transferQueue(VK_NULL_HANDLE)
{
	CreateQueueIndices();
	CreateLogicalDevice();
}


LogicalDevice::~LogicalDevice()
{
	vkDestroyDevice(m_logicalDevice, nullptr);
}

void LogicalDevice::CreateQueueIndices() {
	uint32_t queueFamilyCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(m_physicalDevice->GetPhysicalDevice(), &queueFamilyCount, nullptr);

	std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
	vkGetPhysicalDeviceQueueFamilyProperties(m_physicalDevice->GetPhysicalDevice(), &queueFamilyCount, queueFamilies.data());

	int32_t graphicsFamily = -1;
	int32_t presentFamily = -1;
	int32_t computeFamily = -1;
	int32_t transferFamily = -1;

	for (uint32_t i = 0; i < queueFamilyCount; i++)
	{
		// Check for graphics support.
		if (queueFamilies[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)
		{
			graphicsFamily = i;
			m_graphicsFamily = i;
			m_supportedQueues |= VK_QUEUE_GRAPHICS_BIT;
		}

		// Check for presentation support.
		VkBool32 presentSupport = VK_FALSE;
		vkGetPhysicalDeviceSurfaceSupportKHR(m_physicalDevice->GetPhysicalDevice(), i, m_surface->GetSurface(), &presentSupport);

		if (queueFamilies[i].queueCount > 0 && presentSupport)
		{
			presentFamily = i;
			m_presentFamily = i;
		}

		// Check for compute support.
		if (queueFamilies[i].queueFlags & VK_QUEUE_COMPUTE_BIT)
		{
			computeFamily = i;
			m_computeFamily = i;
			m_supportedQueues |= VK_QUEUE_COMPUTE_BIT;
		}

		// Check for transfer support.
		if (queueFamilies[i].queueFlags & VK_QUEUE_TRANSFER_BIT)
		{
			transferFamily = i;
			m_transferFamily = i;
			m_supportedQueues |= VK_QUEUE_TRANSFER_BIT;
		}

		if (graphicsFamily != -1 && presentFamily != -1 && computeFamily != -1 && transferFamily != -1)
		{
			break;
		}
	}

	if (graphicsFamily == -1 || presentFamily == -1)
	{
		throw std::runtime_error("Failed to find queue family supporting VK_QUEUE_GRAPHICS_BIT");
	}
}

void LogicalDevice::CreateLogicalDevice() 
{

	auto physicalDeviceFeatures = m_physicalDevice->GetFeatures();

	std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
	float queuePriorities[] = { 0.0f };

	if (m_supportedQueues & VK_QUEUE_GRAPHICS_BIT)
	{
		VkDeviceQueueCreateInfo graphicsQueueCreateInfo = {};
		graphicsQueueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		graphicsQueueCreateInfo.queueFamilyIndex = m_graphicsFamily;
		graphicsQueueCreateInfo.queueCount = 1;
		graphicsQueueCreateInfo.pQueuePriorities = queuePriorities;
		queueCreateInfos.emplace_back(graphicsQueueCreateInfo);
	}
	else
	{
		m_graphicsFamily = VK_NULL_HANDLE;
	}

	if (m_supportedQueues & VK_QUEUE_COMPUTE_BIT && m_computeFamily != m_graphicsFamily)
	{
		VkDeviceQueueCreateInfo computeQueueCreateInfo = {};
		computeQueueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		computeQueueCreateInfo.queueFamilyIndex = m_computeFamily;
		computeQueueCreateInfo.queueCount = 1;
		computeQueueCreateInfo.pQueuePriorities = queuePriorities;
		queueCreateInfos.emplace_back(computeQueueCreateInfo);
	}
	else
	{
		m_computeFamily = m_graphicsFamily;
	}

	if (m_supportedQueues & VK_QUEUE_TRANSFER_BIT && m_transferFamily != m_graphicsFamily && m_transferFamily != m_computeFamily)
	{
		VkDeviceQueueCreateInfo transferQueueCreateInfo = {};
		transferQueueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		transferQueueCreateInfo.queueFamilyIndex = m_transferFamily;
		transferQueueCreateInfo.queueCount = 1;
		transferQueueCreateInfo.pQueuePriorities = queuePriorities;
		queueCreateInfos.emplace_back(transferQueueCreateInfo);
	}
	else
	{
		m_transferFamily = m_graphicsFamily;
	}

	VkPhysicalDeviceFeatures enabledFeatures = {};

	// Enable sample rate shading filtering if supported.
	if (physicalDeviceFeatures.sampleRateShading)
	{
		enabledFeatures.sampleRateShading = VK_TRUE;
	}

	// Fill mode non solid is required for wireframe display.
	if (physicalDeviceFeatures.fillModeNonSolid)
	{
		enabledFeatures.fillModeNonSolid = VK_TRUE;

		// Wide lines must be present for line width > 1.0f.
		if (physicalDeviceFeatures.wideLines)
		{
			enabledFeatures.wideLines = VK_TRUE;
		}
	}
	else
	{
		throw std::runtime_error("Selected GPU does not support wireframe pipelines!");
	}

	if (physicalDeviceFeatures.samplerAnisotropy)
	{
		enabledFeatures.samplerAnisotropy = VK_TRUE;
	}
	else
	{
		throw std::runtime_error("Selected GPU does not support sampler anisotropy!");
	}

	if (physicalDeviceFeatures.textureCompressionBC)
	{
		enabledFeatures.textureCompressionBC = VK_TRUE;
	}
	else if (physicalDeviceFeatures.textureCompressionASTC_LDR)
	{
		enabledFeatures.textureCompressionASTC_LDR = VK_TRUE;
	}
	else if (physicalDeviceFeatures.textureCompressionETC2)
	{
		enabledFeatures.textureCompressionETC2 = VK_TRUE;
	}

	if (physicalDeviceFeatures.vertexPipelineStoresAndAtomics)
	{
		enabledFeatures.vertexPipelineStoresAndAtomics = VK_TRUE;
	}
	else
	{
		throw std::runtime_error("Selected GPU does not support vertex pipeline stores and atomics!");
	}

	if (physicalDeviceFeatures.fragmentStoresAndAtomics)
	{
		enabledFeatures.fragmentStoresAndAtomics = VK_TRUE;
	}
	else
	{
		throw std::runtime_error("Selected GPU does not support fragment stores and atomics!");
	}

	if (physicalDeviceFeatures.shaderStorageImageExtendedFormats)
	{
		enabledFeatures.shaderStorageImageExtendedFormats = VK_TRUE;
	}
	else
	{
		throw std::runtime_error("Selected GPU does not support shader storage extended formats!");
	}

	if (physicalDeviceFeatures.shaderStorageImageWriteWithoutFormat)
	{
		enabledFeatures.shaderStorageImageWriteWithoutFormat = VK_TRUE;
	}
	else
	{
		throw std::runtime_error("Selected GPU does not support shader storage write without format!");
	}

	//enabledFeatures.shaderClipDistance = VK_TRUE;
	//enabledFeatures.shaderCullDistance = VK_TRUE;

	if (physicalDeviceFeatures.geometryShader)
	{
		enabledFeatures.geometryShader = VK_TRUE;
	}
	else
	{
		throw std::runtime_error("Selected GPU does not support geometry shaders!");
	}

	if (physicalDeviceFeatures.tessellationShader)
	{
		enabledFeatures.tessellationShader = VK_TRUE;
	}
	else
	{
		throw std::runtime_error("Selected GPU does not support tessellation shaders!");
	}

	if (physicalDeviceFeatures.multiViewport)
	{
		enabledFeatures.multiViewport = VK_TRUE;
	}
	else
	{
		throw std::runtime_error("Selected GPU does not support multi viewports!");
	}

	VkDeviceCreateInfo deviceCreateInfo = {};
	deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	deviceCreateInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
	deviceCreateInfo.pQueueCreateInfos = queueCreateInfos.data();
	deviceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(m_instance->GetDeviceExtensions().size());
	deviceCreateInfo.ppEnabledExtensionNames = m_instance->GetDeviceExtensions().data();
	deviceCreateInfo.pEnabledFeatures = &enabledFeatures;

	deviceCreateInfo.enabledLayerCount = 0;
#ifndef NDEBUG
	deviceCreateInfo.enabledLayerCount = static_cast<uint32_t>(m_instance->GetValidationLayers().size());
	deviceCreateInfo.ppEnabledLayerNames = m_instance->GetValidationLayers().data();
#endif // NDEBUG

	if (vkCreateDevice(m_physicalDevice->GetPhysicalDevice(), &deviceCreateInfo, nullptr, &m_logicalDevice) != VK_SUCCESS) {
		throw std::runtime_error("failed to create logical device!");
	}

	vkGetDeviceQueue(m_logicalDevice, m_graphicsFamily, 0, &m_graphicsQueue);
	vkGetDeviceQueue(m_logicalDevice, m_presentFamily, 0, &m_presentQueue);
	vkGetDeviceQueue(m_logicalDevice, m_computeFamily, 0, &m_computeQueue);
	vkGetDeviceQueue(m_logicalDevice, m_transferFamily, 0, &m_transferQueue);
}
