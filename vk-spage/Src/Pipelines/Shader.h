#pragma once

#include <vulkan/vulkan.h>
#include <string>
#include <vector>
#include <fstream>

#include "../Devices/LogicalDevice.h"

class Shader
{
public:
	Shader(const LogicalDevice* logicalDevice, std::string name);
	~Shader();

	VkShaderModule CreateShaderModule(VkShaderStageFlags stageFlag);
private:

	std::vector<char> LoadShaderFromFile(VkShaderStageFlags stageFlags);

	const LogicalDevice* m_logicalDevice;

	std::string m_name;
};

