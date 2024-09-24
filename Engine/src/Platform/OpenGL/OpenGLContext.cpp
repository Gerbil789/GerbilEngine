#include "enginepch.h"
#include "Platform/OpenGL/OpenGLContext.h"
#include <GLFW/glfw3.h>
#include <glad/glad.h>

namespace Engine
{
	OpenGLContext::OpenGLContext(GLFWwindow* windowHandle) : m_WindowHandle(windowHandle)
	{
		ASSERT(windowHandle, "Window handle is null!");
	}

	void OpenGLContext::Init()
	{
		ENGINE_PROFILE_FUNCTION();
		glfwMakeContextCurrent(m_WindowHandle);
		int status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
		ASSERT(status, "Failed to initialize Glad!");

		LOG_INFO("--- OpenGL Info ---");
		LOG_INFO(" Vendor:\t {0}", (const char*)glGetString(GL_VENDOR));
		LOG_INFO(" Renderer:\t {0}", (const char*)glGetString(GL_RENDERER));
		LOG_INFO(" Version:\t {0}", (const char*)glGetString(GL_VERSION));
		LOG_INFO("-------------------");
	}

	void OpenGLContext::SwapBuffers()
	{
		ENGINE_PROFILE_FUNCTION();
		glfwSwapBuffers(m_WindowHandle);
	}


}