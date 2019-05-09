#pragma once

#include <vulkan/vulkan.h>
#include <vector>

#include "instance.h"


class PhysicalDevice
{
public:
	PhysicalDevice(const Instance* instance);
	~PhysicalDevice();

	const VkPhysicalDevice GetPhysicalDevice() const { return m_physicalDevice; };
	const VkPhysicalDeviceFeatures GetFeatures() const { return m_features; };
private:
	void FindDevices();
	void PickDevice();
	bool IsDeviceSuitable(VkPhysicalDevice device);

	const Instance* m_instance;
	std::vector<VkPhysicalDevice> m_devices;

	VkPhysicalDevice m_physicalDevice;
	VkPhysicalDeviceProperties m_properties;
	VkPhysicalDeviceFeatures m_features;
	VkPhysicalDeviceMemoryProperties m_memoryProperties;
	VkSampleCountFlagBits m_msaaSamples;
};

