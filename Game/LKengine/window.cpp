#include "stdafx.h"
#include "window.h"

#include "myErrors.h"
#include <iostream>

namespace LKengine {

	Window::Window()
	{
	}


	Window::~Window()
	{
	}


	int Window::createWindow(std::string windowName, int screenWidth, int screenHeight, unsigned int currentFlags)
	{
		Uint32 flags = SDL_WINDOW_OPENGL;

		m_screenWidth = screenWidth;
		m_screenHeight = screenHeight;

		if (currentFlags & INVISIBLE) {
			flags |= SDL_WINDOW_HIDDEN;
		}
		if (currentFlags & FULLSCREEN) {
			flags |= SDL_WINDOW_FULLSCREEN_DESKTOP;
		}
		if (currentFlags & BORDERLESS) {
			flags |= SDL_WINDOW_BORDERLESS;
		}

		m_sdlWindow = SDL_CreateWindow(windowName.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, screenWidth, screenHeight, flags);
		if (m_sdlWindow == nullptr) {
			fatalError("SDL Window couldn not be created!");
		}

		//set up our OpenGL context
		SDL_GLContext glContext = SDL_GL_CreateContext(m_sdlWindow);
		if (glContext == nullptr) {
			fatalError("SDL_GL context could not be created!");
		}

		//set up glew
		GLenum error = glewInit();
		if (error != GLEW_OK) {
			fatalError("Could not initialise glew!");
		}

		//check OpenGL Version
		std::printf("***   OpenGL Version: %s   ***\n", glGetString(GL_VERSION));

		//set the background colour
		glClearColor(0.0f, 0.0f, 1.0f, 1.0f);

		//turns off VSync that is automatically limiting program to 60 fps
		SDL_GL_SetSwapInterval(0);
		
		//Enable alpha blending
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		return 0;
	}

	void Window::swapBuffer()
	{
		SDL_GL_SwapWindow(m_sdlWindow);
	}

}