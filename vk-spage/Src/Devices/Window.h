#pragma once

#include <GLFW/glfw3.h>
#include <vector>

class Window
{
public:
	static const std::pair<const char**, uint32_t> GetInstanceExtensions();
	GLFWwindow* GetWindow() const { return m_window; };

	Window();
	~Window();

	bool ShouldClose();
	void PollEvents();

private:

	GLFWwindow* m_window;
};

