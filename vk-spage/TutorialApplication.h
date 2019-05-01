#pragma once
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include "Instance.h"

const int WIDTH = 800;
const int HEIGHT = 600;

class TutorialApplication {
public:
	void run();
private:
	void initWindow();
	void initVulkan();
	void mainLoop();
	void cleanup();

	GLFWwindow* window;
	Instance* isntance;
	
};
