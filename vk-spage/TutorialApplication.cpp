#include "TutorialApplication.h"

void TutorialApplication::run() {
	initWindow();
	initVulkan();
	mainLoop();
	cleanup();
}

void TutorialApplication::initWindow() {
	glfwInit();

	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
	window = glfwCreateWindow(WIDTH, HEIGHT, "Vulkan", nullptr, nullptr);
}

void TutorialApplication::initVulkan() {
	isntance = new Instance();
}

void TutorialApplication::mainLoop() {
	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();
	}
}

void TutorialApplication::cleanup() {
	glfwDestroyWindow(window);
	glfwTerminate();

	isntance->~Instance();
}