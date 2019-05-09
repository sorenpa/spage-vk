
#include "Surface.h"

Surface::Surface(const Instance* instance, const Window* window):
	m_instance(instance),
	m_window(window)
{
	CreateSurface();
}

Surface::~Surface(){
	vkDestroySurfaceKHR(m_instance->GetInstance(), m_surface, nullptr);
}

void Surface::CreateSurface() {
	if (glfwCreateWindowSurface(m_instance->GetInstance(), m_window->GetWindow(), nullptr, &m_surface) != VK_SUCCESS) {
		throw std::runtime_error("failed to create window surface!");
	}
}
