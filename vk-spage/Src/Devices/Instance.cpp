#include "Instance.h"

#include "Window.h"

VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
	VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
	VkDebugUtilsMessageTypeFlagsEXT messageType,
	const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
	void* pUserData)
{

	std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;

	return VK_FALSE;
}

Instance::Instance() :
	m_debugMessenger(VK_NULL_HANDLE),
	m_instance(VK_NULL_HANDLE)
{
	SetupLayers();
	SetupExtensions();
	CreateInstance();
	CreateDebugCallback();
}


Instance::~Instance()
{
#ifndef NDEBUG
	DestroyDebugUtilsMessengerEXT(m_instance, m_debugMessenger, nullptr);
#endif // NDEBUG

	vkDestroyInstance(m_instance, nullptr);
}

void Instance::SetupLayers() 
{

#ifdef NDEBUG
	return;
#endif // !DEBUG

	// Check for validationlayer support
	m_instanceLayers = {
		"VK_LAYER_LUNARG_standard_validation",
	};

	uint32_t layerCount;
	vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

	std::vector<VkLayerProperties> availableLayers(layerCount);
	vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

	for (const char* layerName : m_instanceLayers) {
		bool layerFound = false;

		for (const auto& layerProperties : availableLayers) {
			if (strcmp(layerName, layerProperties.layerName) == 0) {
				layerFound = true;
				break;
			}
		}

		if (!layerFound) {
			throw std::runtime_error("validation layers requested, but not available!");
		}
	}
}

void Instance::SetupExtensions() 
{
	// Check for available VK extensions
	uint32_t availableExtensionsCount = 0;
	vkEnumerateInstanceExtensionProperties(nullptr, &availableExtensionsCount, nullptr);

	std::vector<VkExtensionProperties> availableExtensions(availableExtensionsCount);
	vkEnumerateInstanceExtensionProperties(nullptr, &availableExtensionsCount, availableExtensions.data());

	// Get required extensions for GLFW
	/*uint32_t glfwExtensionCount = 0;
	const char** glfwExtensions;
	glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);*/
	auto windowExtensions = Window::GetInstanceExtensions();
	m_instanceExtensions.assign(windowExtensions.first, windowExtensions.first + windowExtensions.second);

	// Get required extensions for validation layers
#ifndef NDEBUG
	m_instanceExtensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
#endif // NDEBUG

	// Check all required extensions are present
	for (const auto& instanceExtension : m_instanceExtensions)
	{
		bool extensionFound = false;

		for (const auto& extensionProperties : availableExtensions) {
			if (strcmp(instanceExtension, extensionProperties.extensionName) == 0) {
				extensionFound = true;
				break;
			}
		}

		if (!extensionFound) {
			throw std::runtime_error("Unable to find required Vulkan Extension: " + std::string(instanceExtension));
		}
	}
}

void Instance::CreateInstance() 
{
	VkApplicationInfo appInfo = {};
	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appInfo.pApplicationName = "vk-spage application";
	appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.pEngineName = "vk-spage";
	appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.apiVersion = VK_API_VERSION_1_0;

	VkInstanceCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	createInfo.pApplicationInfo = &appInfo;
	createInfo.enabledExtensionCount = static_cast<uint32_t>(m_instanceExtensions.size());
	createInfo.ppEnabledExtensionNames = m_instanceExtensions.data();
	createInfo.enabledLayerCount = static_cast<uint32_t>(m_instanceLayers.size());
	createInfo.ppEnabledLayerNames = m_instanceLayers.data();

	VkResult result = vkCreateInstance(&createInfo, nullptr, &m_instance);
	
	if (vkCreateInstance(&createInfo, nullptr, &m_instance) != VK_SUCCESS) {
		throw std::runtime_error("failed to create instance!");
	}
}

void Instance::CreateDebugCallback() 
{
#ifdef NDEBUG
	return;
#endif // !DEBUG


	VkDebugUtilsMessengerCreateInfoEXT createMessengerInfo = {};
	createMessengerInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
	createMessengerInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
	createMessengerInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
	createMessengerInfo.pfnUserCallback = debugCallback;
	createMessengerInfo.pUserData = nullptr; // Optional

	if (CreateDebugUtilsMessengerEXT(m_instance, &createMessengerInfo, nullptr, &m_debugMessenger) != VK_SUCCESS) {
		throw std::runtime_error("failed to set up debug messenger!");
	}
}

VkResult Instance::CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger) {
	auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
	if (func != nullptr) {
		return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
	}
	else {
		return VK_ERROR_EXTENSION_NOT_PRESENT;
	}
}

void Instance::DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator) {
	auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
	if (func != nullptr) {
		func(instance, debugMessenger, pAllocator);
	}
}