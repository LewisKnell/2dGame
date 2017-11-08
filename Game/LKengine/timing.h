#pragma once

namespace LKengine {

	class FPSLimiter {
	public:

		FPSLimiter();

		void init(float maxFPS);

		void setMaxFPS(float maxFPS);

		void beginFrame();

		//end will return the current FPS
		float end();

	private:
		void calculateFPS();

		float m_maxFPS;
		float m_fps;
		float m_frameTime;

		unsigned int m_startTicks;

	};





}