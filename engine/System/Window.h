#pragma once
//这里使用struct是因为在GLFW内部实现就是用的struct
struct GLFWwindow;

class Window
{
public:
	Window(int width, int height, const char* title);
	~Window();
	//禁止复制操作
	Window(Window&) = delete;
	Window& operator=(const Window&) = delete;

	bool shouldClose();
	int getWidth() const { return width; }
	int getHeight() const { return height; }
	void PollEvent();
	void SwapBuffers();

private:
	int width;
	int height;
	
	GLFWwindow* handle = nullptr;

	//成员函数
	void InitGLFW();
	void InitGLAD();

	//Window类只负责将获取到的输入设备数据传递给Input类
	static void framebufferSize_callback(GLFWwindow* window, int width, int height);
	static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
	static void cursor_callback(GLFWwindow* window, double xpos, double ypos);
	static void MouseButtonCallback(GLFWwindow* win, int btn, int action, int mods);
	static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
};

