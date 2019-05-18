#include "Shader.h"

const std::string COMPILED_SHADER_FOLDER = "Resources/Shaders/Compiled/";

Shader::Shader(const LogicalDevice* logicalDevice, std::string name):
	m_logicalDevice(logicalDevice),
	m_name(name)
{

}


Shader::~Shader()
{
}

VkShaderModule Shader::CreateShaderModule(VkShaderStageFlags stageFlag) {
	auto shaderSpirvCode = LoadShaderFromFile(stageFlag);

	VkShaderModuleCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	createInfo.codeSize = shaderSpirvCode.size();
	createInfo.pCode = reinterpret_cast<const uint32_t*>(shaderSpirvCode.data());

	VkShaderModule shaderModule;
	if (vkCreateShaderModule(m_logicalDevice->GetLogicaDevice(), &createInfo, nullptr, &shaderModule) != VK_SUCCESS) {
		throw std::runtime_error("failed to create shader module!");
	}

	return shaderModule;
}

std::vector<char> Shader::LoadShaderFromFile(VkShaderStageFlags stage) {
	
	std::string filePath = COMPILED_SHADER_FOLDER;
	filePath.append(m_name);

	switch (stage)
	{
	case VK_SHADER_STAGE_VERTEX_BIT:
		filePath.append(".vert.spv");
		break;
	case VK_SHADER_STAGE_FRAGMENT_BIT:
		filePath.append(".frag.spv");
		break;
	case VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT:
		filePath.append(".tesc.spv");
		break;
	case VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT:
		filePath.append(".tese.spv");
		break;
	case VK_SHADER_STAGE_GEOMETRY_BIT:
		filePath.append(".geom.spv");
		break;
	default:
		throw std::runtime_error("failed to load shader module. Unimplemented stage flag was supplied!");
		break;
	}

	std::ifstream file(filePath, std::ios::ate | std::ios::binary);

	if (!file.is_open()) {
		throw std::runtime_error("failed to open shader module file!");
	}

	size_t fileSize = (size_t)file.tellg();
	std::vector<char> buffer(fileSize);

	file.seekg(0);
	file.read(buffer.data(), fileSize);

	file.close();

	return buffer;
}

