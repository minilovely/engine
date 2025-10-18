#include "glad.h"
#include "GLFW/glfw3.h"
#include <iostream>
#include "Window.h"
#include "InputSystem.h"
#include "CameraSystem.h"



Window::Window(int width, int height, const char* title)
				:width(width),height(height)
{
	InitGLFW();
	handle = glfwCreateWindow(width, height, title, nullptr, nullptr);
	if (!handle)
	{
		std::cout << "window creates failed" << std::endl;
		glfwTerminate();
	}
	else
	{
		std::cout << "window creates success" << std::endl;

	}
	glfwMakeContextCurrent(handle);
	glfwSetWindowUserPointer(handle, this);
	InitGLAD();
	//设置CallBack函数
	glfwSetFramebufferSizeCallback(handle, framebufferSize_callback);
	glfwSetKeyCallback(handle, key_callback);
	glfwSetCursorPosCallback(handle, cursor_callback);
	glfwSetMouseButtonCallback(handle, MouseButtonCallback);
	glfwSetScrollCallback(handle, scroll_callback);
	//设置鼠标显示方式
	glfwSetInputMode(handle, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

}

Window::~Window()
{
	glfwDestroyWindow(handle);
	glfwTerminate();
}

bool Window::shouldClose()
{
	return glfwWindowShouldClose(handle);
}

void Window::PollEvent()
{
	glfwPollEvents();
	Input::Tick();
}

void Window::SwapBuffers()
{
	glfwSwapBuffers(handle);
}
//初始化GLFW
void Window::InitGLFW()
{
	if (!glfwInit())
	{
		std::cout << "glfw init failed" << std::endl;
		glfwTerminate();
	}
	//set version of OpenGL
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
}

void Window::InitGLAD()
{
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "glad init failed" << std::endl;
		glfwTerminate();
	}
}


void Window::framebufferSize_callback(GLFWwindow* window, int width, int height)
{
	Window* handle = static_cast<Window*>(glfwGetWindowUserPointer(window));
	handle->width = width;
	handle->height = height;
	glViewport(0, 0, width, height);
	CameraSystem::Instance().OnWindowResized(width, height);
}

void Window::key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	Input::setKey(key, action != GLFW_RELEASE);
}

void Window::cursor_callback(GLFWwindow* window, double x, double y)
{
	Input::setMousePos(static_cast<float>(x), static_cast<float>(y));
}
void Window::MouseButtonCallback(GLFWwindow* win, int btn, int action, int mods)
{
	Input::setMouseButton(btn, action != GLFW_RELEASE);
}
void Window::scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	Input::setTotalOffset(static_cast<float>(yoffset));
}
