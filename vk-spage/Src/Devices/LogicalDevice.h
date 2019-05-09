#pragma once

#include <vulkan/vulkan.h>
#include <optional>

#include "PhysicalDevice.h"
#include "Surface.h"


class LogicalDevice
{
public:
	LogicalDevice(const Instance* instance, const PhysicalDevice* physicalDevice, const Surface* surface);
	~LogicalDevice();

private:
	void CreateQueueIndices();

	void CreateLogicalDevice();

	const Instance* m_instance;
	const PhysicalDevice* m_physicalDevice;
	const Surface* m_surface;

	VkDevice m_logicalDevice;
	VkPhysicalDeviceFeatures m_enabledFeatures;

	VkQueueFlags m_supportedQueues;
	uint32_t m_graphicsFamily;
	uint32_t m_presentFamily;
	uint32_t m_computeFamily;
	uint32_t m_transferFamily;

	VkQueue m_graphicsQueue;
	VkQueue m_presentQueue;
	VkQueue m_computeQueue;
	VkQueue m_transferQueue;

};

