#include "Window.h"

const int WIDTH = 800;
const int HEIGHT = 600;

const std::pair<const char **, uint32_t> Window::GetInstanceExtensions()
{
	uint32_t glfwExtensionCount = 0;
	const char **glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
	return std::make_pair(glfwExtensions, glfwExtensionCount);
}

Window::Window()
{
	glfwInit();

	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
	m_window = glfwCreateWindow(WIDTH, HEIGHT, "Vulkan", nullptr, nullptr);
}


Window::~Window()
{
	glfwDestroyWindow(m_window);
	glfwTerminate();
}

bool Window::ShouldClose() 
{
	return glfwWindowShouldClose(m_window);
}

void Window::PollEvents() 
{
	glfwPollEvents();
}
