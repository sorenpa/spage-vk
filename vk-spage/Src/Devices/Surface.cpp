
#include "Surface.h"

Surface::Surface(const Instance* instance, const Window* window) :
	m_instance(instance),
	m_window(window)
{
	CreateSurface();
}

Surface::~Surface(){
	vkDestroySurfaceKHR(m_instance->GetInstance(), m_surface, nullptr);
}

void Surface::CreateSurface() {
	if (m_window->CreateSurface(m_instance->GetInstance(), nullptr, &m_surface) != VK_SUCCESS) {
		throw std::runtime_error("failed to create window surface!");
	}
}

