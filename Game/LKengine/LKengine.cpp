// LKengine.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include <SDL\SDL.h>
#include <GL\glew.h>

#include "LKengine.h"

namespace LKengine {

	int main()
	{
		return 0;
	}

	int init()
	{
		//initialise SDL
		SDL_Init(SDL_INIT_EVERYTHING);

		//tell SDL that we want double buffered window so we don't get
		//any flickering
		SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

		return 0;
	}


}