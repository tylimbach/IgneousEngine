#pragma once

#include "bve_window.h"

#include <stdexcept>

namespace bve {
	
	BveWindow::BveWindow(int width, int height, std::string name) : width{ width }, height{ height }, windowName{ name } {
		initWindow();
	}

	BveWindow::~BveWindow() {
		glfwDestroyWindow(window);
		glfwTerminate();
	}

	void BveWindow::createWindowSurface(VkInstance instance, VkSurfaceKHR* surface)
	{
		if (glfwCreateWindowSurface(instance, window, nullptr, surface) != VK_SUCCESS) {
			throw std::runtime_error("Failed to create window surface");
		}
	}

	void BveWindow::initWindow() {
		glfwInit();
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

		window = glfwCreateWindow(width, height, windowName.c_str(), nullptr, nullptr);
		glfwSetWindowUserPointer(window, this);
		glfwSetFramebufferSizeCallback(window, frameBufferResizeCallback);
	}

	void BveWindow::frameBufferResizeCallback(GLFWwindow* window, int width, int height) {
		BveWindow* bveWindow = reinterpret_cast<BveWindow*>(glfwGetWindowUserPointer(window));
		bveWindow->frameBufferResized = true;
		bveWindow->width = width;
		bveWindow->height = height;
	}

}