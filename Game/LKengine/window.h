#pragma once

#include <SDL\SDL.h>
#include <GL\glew.h>
#include <string>


namespace LKengine {

	//set the flags to different bits to allow bitwise operation to pass
	//only one variable in. 0x1 is first bit, 0x2 is second bit etc..
	enum WindowFlags { INVISIBLE = 0x1, FULLSCREEN = 0x2, BORDERLESS = 0x4 };

	class Window
	{
	public:
		Window();
		~Window();

		int createWindow(std::string windowName, int screenWidth, int screenHeight, unsigned int currentFlags);

		int getScreenWidth() { return m_screenWidth; }
		int getScreenHeight() { return m_screenHeight; }

		void swapBuffer();


	private:
		SDL_Window* m_sdlWindow;
		int m_screenWidth, m_screenHeight;

	};

}