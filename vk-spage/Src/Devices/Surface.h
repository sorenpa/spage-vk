#pragma once
#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>

#include "Instance.h"
#include "Window.h"

#include <iostream>
#include <cstdlib>
#include <vector>

class Surface
{
public:
	
	const VkSurfaceKHR GetSurface() const { return m_surface; };

	Surface(const Instance* instance, const Window* window);
	~Surface();

	


private:
	void CreateSurface();
	
	const Window* m_window;
	const Instance* m_instance;
	VkSurfaceKHR m_surface;
};

