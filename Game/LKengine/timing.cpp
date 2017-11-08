#include "stdafx.h"

#include <SDL\SDL.h>

#include "timing.h"

namespace LKengine {


	FPSLimiter::FPSLimiter()
	{
	}

	void FPSLimiter::init(float maxFPS)
	{
		setMaxFPS(maxFPS);
	}


	void FPSLimiter::setMaxFPS(float maxFPS)
	{
		m_maxFPS = maxFPS;
	}


	void FPSLimiter::beginFrame()
	{
		m_startTicks = SDL_GetTicks();
	}


	float FPSLimiter::end()
	{
		calculateFPS();

		float frameTicks = SDL_GetTicks() - m_startTicks;
		//limit the fps to the max FPS
		if (1000.0f / m_maxFPS > frameTicks)
		{
			SDL_Delay(1000.0f / m_maxFPS - frameTicks);
		}

		return m_fps;
	}


	void FPSLimiter::calculateFPS()
	{
		static const int NUM_SAMPLES = 10;
	static float frameTimes[NUM_SAMPLES];
	static int currentFrame = 0;

	static float previousTicks = SDL_GetTicks();

	float currentTicks;
	currentTicks = SDL_GetTicks();

	m_frameTime = currentTicks - previousTicks;
	frameTimes[currentFrame % NUM_SAMPLES] = m_frameTime;

	previousTicks = currentTicks;

	int count;

	currentFrame++;
		if (currentFrame < NUM_SAMPLES) {
			count = currentFrame;
		} else {
			count = NUM_SAMPLES;
		}

		float frameTimeAverage = 0;
		for (int i = 0; i < count; i++)
		{
			frameTimeAverage += frameTimes[i];
		}
		frameTimeAverage /= count;

		if (frameTimeAverage > 0) {
			m_fps = 1000.0f / frameTimeAverage;
		} else {
			m_fps = 60;
		}
	}


}