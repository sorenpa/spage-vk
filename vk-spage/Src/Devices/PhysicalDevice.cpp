#include "PhysicalDevice.h"



PhysicalDevice::PhysicalDevice(const Instance *instance, const Surface* surface) :
	m_instance(instance),
	m_surface(surface),
	m_physicalDevice(VK_NULL_HANDLE),
	m_properties({}),
	m_features({}),
	m_memoryProperties({}),
	m_msaaSamples(VK_SAMPLE_COUNT_1_BIT)
{
	FindDevices();
	PickDevice();
}


PhysicalDevice::~PhysicalDevice()
{
}

void PhysicalDevice::FindDevices() {
	uint32_t deviceCount = 0;
	vkEnumeratePhysicalDevices(m_instance->GetInstance(), &deviceCount, nullptr);

	if (deviceCount == 0) {
		throw std::runtime_error("failed to find GPUs with Vulkan support!");
	}

	m_devices.resize(deviceCount);
	vkEnumeratePhysicalDevices(m_instance->GetInstance(), &deviceCount, m_devices.data());
}

void PhysicalDevice::PickDevice() {
	

	for (const auto& device : m_devices) {
		if (IsDeviceSuitable(device)) {

			VkPhysicalDeviceProperties deviceProperties;
			vkGetPhysicalDeviceProperties(device, &deviceProperties);

			VkPhysicalDeviceFeatures deviceFeatures;
			vkGetPhysicalDeviceFeatures(device, &deviceFeatures);

			m_physicalDevice = device;
			m_properties = deviceProperties;
			m_features = deviceFeatures;

			break;
		}
	}

	if (m_physicalDevice == VK_NULL_HANDLE) {
		throw std::runtime_error("failed to find a suitable GPU!");
	}
}

bool PhysicalDevice::IsDeviceSuitable(VkPhysicalDevice physicalDevice) {

	VkPhysicalDeviceProperties deviceProperties;
	vkGetPhysicalDeviceProperties(physicalDevice, &deviceProperties);

	VkPhysicalDeviceFeatures deviceFeatures;
	vkGetPhysicalDeviceFeatures(physicalDevice, &deviceFeatures);

	uint32_t extensionPropertyCount;
	vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &extensionPropertyCount, nullptr);

	std::vector<VkExtensionProperties> extensionProperties(extensionPropertyCount);
	vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &extensionPropertyCount, extensionProperties.data());

	// Check extensions
	bool extensionsSupported = true;
	for (const auto& requiredExtension : m_instance->GetDeviceExtensions()) {
		bool extensionFound = false;
		for (const auto& deviceExtension : extensionProperties) {
			if (strcmp(requiredExtension, deviceExtension.extensionName) == 0)
			{
				extensionFound = true;
				break;
			}
		}

		if (!extensionFound)
		{
			extensionsSupported = false;
			break;
		}
	}

	// Check for surface support
	uint32_t presentModeCount;
	vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, m_surface->GetSurface(), &presentModeCount, nullptr);

	uint32_t surfaceFormatCount;
	vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, m_surface->GetSurface(), &surfaceFormatCount, nullptr);

	bool surfaceSupported = presentModeCount > 0 && surfaceFormatCount > 0;

	return deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU && 
		   deviceFeatures.geometryShader && 
		   extensionsSupported && 
		   surfaceSupported;

}

