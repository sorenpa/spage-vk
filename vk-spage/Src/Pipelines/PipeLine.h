#pragma once

#include <vulkan/vulkan.h>
#include <vector>
#include <string>

#include "Shader.h"
#include "../Devices//LogicalDevice.h"
#include "../Presentation//SwapChain.h"

class PipeLine
{
public:
	PipeLine(const LogicalDevice* logicalDevice, const SwapChain* swapChain);
	~PipeLine();
private:
	void CreatePipeLine();

	const LogicalDevice* m_logicalDevice;
	const SwapChain* m_swapChain;

	VkPipelineLayout m_pipelineLayout;
};

