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
	m_physicalDevice = new PhysicalDevice(m_instance, m_surface);
	m_logicalDevice = new LogicalDevice(m_instance, m_physicalDevice, m_surface);

	m_swapChain = new SwapChain(m_surface, m_physicalDevice, m_logicalDevice, m_window->GetExtend());

	m_pipeLine = new PipeLine(m_logicalDevice, m_swapChain);
}

void TutorialApplication::mainLoop() {
	while (!m_window->ShouldClose()) {
		m_window->PollEvents();
	}
}

void TutorialApplication::cleanup() {
	m_pipeLine->~PipeLine();
	m_swapChain->~SwapChain();
	m_logicalDevice->~LogicalDevice();
	m_physicalDevice->~PhysicalDevice();
	m_surface->~Surface();
	m_instance->~Instance();
	m_window->~Window();
}