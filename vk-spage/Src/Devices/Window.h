#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vector>

class Window
{
public:
	static const std::pair<const char**, uint32_t> GetInstanceExtensions();
	
	GLFWwindow* GetWindow() const { return m_window; };
	;

	Window();
	~Window();

	bool ShouldClose();
	void PollEvents();

	VkExtent2D GetExtend() const;
	VkResult CreateSurface(const VkInstance &instance, const VkAllocationCallbacks *allocator, VkSurfaceKHR *surface) const;

private:

	GLFWwindow* m_window;
};

