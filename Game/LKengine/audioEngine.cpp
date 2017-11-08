#include "stdafx.h"
#include "audioEngine.h"

#include <iostream> // < delete me when not debugging / refactoring

#include "myErrors.h"

namespace LKengine {

	void SoundEffect::play(int loops /* = 0 */)
	{
		if (Mix_PlayChannel(-1, m_chunk, loops) == -1)
		{
			if (Mix_PlayChannel(0, m_chunk, loops))
			{
				fatalError("Mix_PlayChannel error: " + std::string(Mix_GetError()));
			}
		}
	}

	void Music::play(int loops) {
		Mix_PlayMusic(m_music, loops);
	}

	void Music::pause() {
		Mix_PauseMusic();
	}

	void Music::stop() {
		Mix_HaltMusic();
	}

	void Music::resume() {
		Mix_ResumeMusic();
	}


	AudioEngine::AudioEngine()
	{
		// Empty
	}


	AudioEngine::~AudioEngine()
	{
		destroy();
	}


	void AudioEngine::init()
	{
		if (m_isInitialised)
		{
			fatalError("Tried to initialise Audio Engine twice!\n");
		}
		// Parameter can be a bitwise combination of MIX_INIT_FAC
		// MIX_INIT_MOD, MIX_INIT_MP£, MIX_INIT_OGG
		if (Mix_Init(MIX_INIT_MP3 | MIX_INIT_OGG) == 1)
		{
			fatalError("Mix_init error: " + std::string(Mix_GetError()));
		}

		// Double DEFAULT_FREQUENCY for MP3, but gives good performance
		if (Mix_OpenAudio(MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT, 2, 1024) == 1)
		{
			fatalError("Mix_OpenAudio error: " + std::string(Mix_GetError()));
		}

		m_isInitialised = true;
		Mix_Volume(-1, volumeEffects);
		Mix_VolumeMusic(volumeMusic);

	}
	void AudioEngine::destroy()
	{
		if (m_isInitialised)
		{
			m_isInitialised = false;

			for (auto it = m_effectMap.begin(); it != m_effectMap.end(); it++) {
				Mix_FreeChunk(it->second);
			}

			for (auto it = m_musicMap.begin(); it != m_musicMap.end(); it++) {
				Mix_FreeMusic(it->second);
			}

			m_effectMap.clear();
			m_musicMap.clear();
			Mix_CloseAudio();
			Mix_Quit();
		}
	}


	SoundEffect AudioEngine::loadSoundEffect(const std::string& filePath)
	{
		// Try to find the audio in the cache
		auto it = m_effectMap.find(filePath);

		SoundEffect effect;

		if (it == m_effectMap.end())
		{
			// Failed to find it , must load
			Mix_Chunk* chunk = Mix_LoadWAV(filePath.c_str());
			// Check for errors
			if (chunk == nullptr)
			{
				fatalError("Mix_LoadWAV error: " + std::string(Mix_GetError()));
			}

			effect.m_chunk = chunk;
			m_effectMap[filePath] = chunk;
		}
		else
		{
			// It already cached
			effect.m_chunk = it->second;
		}

		return effect;
	}


	Music AudioEngine::loadMusic(const std::string& filePath)
	{
		// Try to find the audio in the cache
		auto it = m_musicMap.find(filePath);

		Music music;

		if (it == m_musicMap.end())
		{
			// Failed to find it , must load
			Mix_Music* mixMusic = Mix_LoadMUS(filePath.c_str());
			// Check for errors
			if (mixMusic == nullptr)
			{
				fatalError("Mix_LoadMUS error: " + std::string(Mix_GetError()));
			}

			music.m_music = mixMusic;
			m_musicMap[filePath] = mixMusic;
		}
		else
		{
			// It already cached
			music.m_music = it->second;
		}

		return music;
	}


	void AudioEngine::volumeUp(int volumeUpChange)
	{
		// Max volume is 128, check if the current effect volume is less than 128 - the amount to change colume by
		if (volumeEffects < MIX_MAX_VOLUME)
		{
			volumeEffects += volumeUpChange;
			// Stop volume going above the Max Volume
			if (volumeEffects > MIX_MAX_VOLUME) { volumeEffects = MIX_MAX_VOLUME; }
		}
		else
		{
			volumeEffects = MIX_MAX_VOLUME;
		}

		if (volumeMusic < MIX_MAX_VOLUME)
		{
			volumeMusic += volumeUpChange;
			if (volumeMusic > MIX_MAX_VOLUME) { volumeMusic = MIX_MAX_VOLUME; }
		}
		else
		{
			volumeMusic = MIX_MAX_VOLUME;
		}

		// Set the volumes
		Mix_Volume(-1, volumeEffects);
		Mix_VolumeMusic(volumeMusic);

		std::cout << "Effects Volume  is: " << volumeEffects << std::endl;
		std::cout << "Music Volume  is: " << volumeMusic << std::endl;
	}


	void AudioEngine::volumeDown(int volumeDownChange)
	{
		if (volumeEffects > 0)
		{
			volumeEffects -= volumeDownChange;
			// Stop volume going below 0
			if (volumeEffects < 0) { volumeEffects = 0; }
		}
		else
		{
			volumeEffects = 0;
		}
		if (volumeMusic > 0)
		{
			volumeMusic -= volumeDownChange;
			// Stop volume going below 0
			if (volumeMusic < 0) { volumeMusic = 0; }
		}
		else
		{

			volumeMusic = 0;
		}
		// Set the volumes
		Mix_Volume(-1, volumeEffects);
		Mix_VolumeMusic(volumeMusic);
		std::cout << "Effects Volume is: " << volumeEffects << std::endl;
		std::cout << "Music Volume is: " << volumeMusic << std::endl;
	}

}