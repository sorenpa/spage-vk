#pragma once

#include "Devices/Instance.h"
#include "Devices/Window.h"
#include "Devices/Surface.h"
#include "Devices/PhysicalDevice.h"
#include "Devices/LogicalDevice.h"

#include "Presentation/SwapChain.h"

#include "Pipelines/PipeLine.h"

class TutorialApplication {
public:
	void run();
private:
	void initWindow();
	void initVulkan();
	void mainLoop();
	void cleanup();

	// DEVICES
	Window* m_window;
	Instance* m_instance;
	Surface* m_surface;
	PhysicalDevice* m_physicalDevice;
	LogicalDevice* m_logicalDevice;

	// PRESENTATION
	SwapChain* m_swapChain;

	// PIPE_LINE
	PipeLine* m_pipeLine;

};
