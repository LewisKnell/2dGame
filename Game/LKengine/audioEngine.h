#pragma once

#include <string>
#include <map>

#include <SDL\SDL_mixer.h>

namespace LKengine {

	class SoundEffect {
	public:
		friend class AudioEngine;

		/// Plays the sound effect
		/// @param loops: if loops == -1 - loop forever, otherwise specify number of times to loop
		void play(int loops = 0);

	private:
		Mix_Chunk* m_chunk = nullptr;
	};

	class Music {
	public:
		friend class AudioEngine;

		/// Plays the audio file
		/// @param loops: if loops == -1 - loop forever, otherwise play loops times
		void play(int loops = 1);

		static void pause();
		static void stop();
		static void resume();

	private:
		Mix_Music* m_music = nullptr;
	};


	// Manager for all the music and sound effects
	class AudioEngine
	{
	public:
		AudioEngine();
		~AudioEngine();

		void init();
		void destroy();

		void volumeUp(int volumeUpChange);
		void volumeDown(int volumeDownChange);

		int volumeEffects = 30;
		int volumeMusic = 30;

		// Loads sound effect once. If already loaded, returns pointer to sound effect
		SoundEffect loadSoundEffect(const std::string& filePath);
		Music loadMusic(const std::string& filePath);

	private:
		bool m_isInitialised = false;





		std::map<std::string, Mix_Chunk*> m_effectMap;
		std::map<std::string, Mix_Music*> m_musicMap;

	};

}
