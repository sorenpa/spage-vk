#pragma once
#define GLFW_INCLUDE_VULKAN

#include "Devices/Instance.h"
#include "Devices/Window.h"
#include "Devices/Surface.h"
#include "Devices/PhysicalDevice.h"
#include "Devices/LogicalDevice.h"


class TutorialApplication {
public:
	void run();
private:
	void initWindow();
	void initVulkan();
	void mainLoop();
	void cleanup();

	Window* m_window;
	Instance* m_instance;
	Surface* m_surface;
	PhysicalDevice* m_physicalDevice;
	LogicalDevice* m_logicalDevice;
};
