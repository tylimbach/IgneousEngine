#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <string>

namespace bve
{
	class BveWindow
	{
	public:
		BveWindow(int width, int height, std::string name);
		~BveWindow();

		BveWindow(const BveWindow&) = delete;
		BveWindow& operator=(const BveWindow&) = delete;
		BveWindow(const BveWindow&&) = delete;
		BveWindow& operator=(const BveWindow&&) = delete;

		bool shouldClose() { return glfwWindowShouldClose(window); }
		VkExtent2D getExtent() { return {static_cast<uint32_t>(width_), static_cast<uint32_t>(height_)}; }
		bool wasWindowResized() { return frameBufferResized_; }
		void resetWindowResizedFlag() { frameBufferResized_ = false; }

		void createWindowSurface(VkInstance instance, VkSurfaceKHR* surface);

		GLFWwindow* getGLFWWindow() { return window; }

	private:
		static void frameBufferResizeCallback(GLFWwindow* window, int width, int height);
		void initWindow();

		int width_;
		int height_;
		bool frameBufferResized_ = false;

		std::string windowName_;
		GLFWwindow* window;
	};
}
