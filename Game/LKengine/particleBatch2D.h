#pragma once

#include <functional>
#include <glm\glm.hpp>
#include "vertex.h"
#include "spriteBatch.h"
#include "GLTexture.h"
#include "tileSheet.h"

namespace LKengine {

	class Particle2D 
	{
	public:

		glm::vec2 position = glm::vec2(0.0f);
		glm::vec2 velocity = glm::vec2(0.0f);
		float angle = 0.0f;
		ColorRGBA8 color;
		float life = 0.0f;
		float width = 0.0f;
	};


	inline void defaultParticleUpdate(Particle2D& particle, float deltaTime)
	{
		particle.position += particle.velocity * deltaTime;
	}



	class ParticleBatch2D
	{
	public:
		ParticleBatch2D();
		~ParticleBatch2D();

		void init(int maxParticles, 
			float decayRate, 
			TileSheet texture,
			int tileID,
			std::function<void(Particle2D&, float)> updateFunc = defaultParticleUpdate);

		void update(float deltaTime);

		void draw(SpriteBatch* spriteBatch);


		void addParticle(const glm::vec2& position,
						const glm::vec2& velocity,
						float angle,
						const ColorRGBA8& color,
						float width);


	private:
		std::function<void(Particle2D&, float)> m_updateFunc;

		int findFreeParticle();

		float m_decayRate = 0.1f; ///< How fast particles disappear
		int m_maxParticles = 0;

		int m_lastFreeParticle = 0;

		int m_tileID;

		TileSheet m_texture;

		Particle2D* m_particles = nullptr;

	};

}