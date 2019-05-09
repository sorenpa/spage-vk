#include "TutorialApplication.h"

void TutorialApplication::run() {
	initWindow();
	initVulkan();
	mainLoop();
	cleanup();
}

void TutorialApplication::initWindow() {
	m_window = new Window();
}

void TutorialApplication::initVulkan() {
	m_instance = new Instance();
	m_surface = new Surface(m_instance, m_window);
	m_physicalDevice = new PhysicalDevice(m_instance);
	m_logicalDevice = new LogicalDevice(m_instance, m_physicalDevice, m_surface);
}

void TutorialApplication::mainLoop() {
	while (!m_window->ShouldClose()) {
		m_window->PollEvents();
	}
}

void TutorialApplication::cleanup() {
	m_logicalDevice->~LogicalDevice();
	m_physicalDevice->~PhysicalDevice();
	m_surface->~Surface();
	m_instance->~Instance();
	m_window->~Window();
}