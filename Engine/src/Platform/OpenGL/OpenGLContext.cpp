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

		ENGINE_LOG_INFO("--- OpenGL Info ---");
		ENGINE_LOG_INFO(" Vendor:\t {0}", (const char*)glGetString(GL_VENDOR));
		ENGINE_LOG_INFO(" Renderer:\t {0}", (const char*)glGetString(GL_RENDERER));
		ENGINE_LOG_INFO(" Version:\t {0}", (const char*)glGetString(GL_VERSION));
		ENGINE_LOG_INFO("-------------------");
	}

	void OpenGLContext::SwapBuffers()
	{
		ENGINE_PROFILE_FUNCTION();
		glfwSwapBuffers(m_WindowHandle);
	}


}