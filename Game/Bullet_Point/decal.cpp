#include "decal.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm\gtx\rotate_vector.hpp>
#include <random>
#include <ctime>
#include <SDL/SDL.h>
#include <iostream>

#include <LKengine\resourceManager.h>


Decal::Decal()
{
}


Decal::Decal(const glm::vec2& position, 
	const glm::vec2& velocity, 
	float angle, 
	float lifeTotal, 
	float decayRate,
	LKengine::ColorRGBA8 color, 
	float width,
	float height,
	LKengine::TileSheet texture, 
	int tileID)
{

	m_position = position;
	m_velocity = velocity;
	m_angle = angle;
	m_lifeTotal = lifeTotal;
	m_lifeLeft = lifeTotal;
	m_decayRate = decayRate;
	m_color = color;
	m_width = width;
	m_height = height;
	m_decalTexture = texture;
	m_tileID = tileID;
}


Decal::~Decal()
{

}


void Decal::init()
{

	//LKengine::GLTexture texture;

	//// Initialise gun decals textures
	//// Load magazine sprite sheet
	//texture = LKengine::ResourceManager::getTexture("Textures/Weapons/mag.png");
	//// Set the size of decal sprite sheet
	//m_decalTexture.init(texture, glm::ivec2(1, 1));
	//// Initialise decal (particles)
	//m_magParticleBatch = new LKengine::ParticleBatch2D();
	//m_magParticleBatch->init(10, 0.005f,
	//	m_decalTexture,
	//	0,
	//	[](LKengine::Particle2D& particle, float deltaTime)
	//{

	//	particle.position += particle.velocity * deltaTime;
	//	particle.color.a = (GLubyte)(particle.life * 255.0f);

	//	if (particle.velocity.x > 0) {
	//		particle.velocity.x -= 0.1f;
	//	}
	//	if (particle.velocity.y > 0) {
	//		particle.velocity.y -= 0.1f;
	//	}
	//});

	//m_particleEngine.addParticleBatch(m_magParticleBatch);
}


void Decal::update(float deltaTime)
{
	//m_magParticleBatch->update(deltaTime);
}


void Decal::draw(LKengine::SpriteBatch& spriteBatch)
{
	///// Render the particles. Use agentSpriteBatch so we don't need to make
	///// a new sprite batch and so we use the same memory buffer.
	//m_magParticleBatch->draw(&spriteBatch);

}


	//static std::mt19937 randEngine(time(nullptr));
	//static std::uniform_real_distribution<float> randAngle(0.0f, 2.0f * M_PI);

	//glm::vec2 vel(1.0f, 0.0f);
	//LKengine::ColorRGBA8 col(255, 255, 255, 255);

	////float radAngle = angle * M_PI / 180;
	//for (int i = 0; i < numDecals; i++) {
	//	m_magParticleBatch->addParticle(
	//		position,
	//		glm::rotate(vel, randAngle(randEngine)),
	//		0.0f,
	//		col,
	//		size);
	//}
