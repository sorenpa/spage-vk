#pragma once
#include <vulkan/vulkan.h>

#include <iostream>
#include <cstdlib>
#include <vector>

class Instance
{
public:
	friend VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
		VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
		VkDebugUtilsMessageTypeFlagsEXT messageType,
		const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
		void* pUserData); 


	Instance();
	~Instance();

	const VkInstance GetInstance() const { return m_instance; };
	const std::vector<const char*> &GetDeviceExtensions() const { return m_deviceExtensions; };
	const std::vector<const char*> &GetValidationLayers() const { return m_instanceLayers; };


private:
	void SetupLayers();
	void SetupExtensions();
	void CreateInstance();
	void CreateDebugCallback();
	
	VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger);
	void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator);

	std::vector<const char*> m_instanceLayers = {};
	std::vector<const char*> m_instanceExtensions;
	std::vector<const char*> m_deviceExtensions;

	VkInstance m_instance;
	VkDebugUtilsMessengerEXT m_debugMessenger;

};

