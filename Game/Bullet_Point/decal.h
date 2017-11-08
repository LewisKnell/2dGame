#pragma once


#include <LKengine\particleEngine2D.h>
#include <LKengine\particleBatch2D.h>



class Decal
{
public:
	Decal();
	Decal(const glm::vec2& position,
		const glm::vec2& velocity,
		float angle,
		float lifeTotal,
		float decayRate,
		LKengine::ColorRGBA8 color,
		float width,
		float height,
		LKengine::TileSheet texture,
		int tileID);
	~Decal();

	void init();

	void update(float deltaTime);

	void draw(LKengine::SpriteBatch& spriteBatch);

	// Getters
	glm::vec2 getPosition() { return m_position; }
	glm::vec2 getVelocity() { return m_velocity; }
	float getAngle() { return m_angle; }
	float getLifeTotal() { return m_lifeTotal; }
	float getLifeLeft() { return m_lifeLeft; }
	float getDecayRate() { return m_decayRate; }
	LKengine::ColorRGBA8 getColor() { return m_color; }
	float getWidth() { return m_width; }
	float getHeight() { return m_height; }
	int getTileID() { return m_tileID; }

	// Setters
	void setPosition(glm::vec2 pos) { m_position = pos; }
	void setVelocity(glm::vec2 vel) { m_velocity = vel; }
	void setLifeLeft(float life) { m_lifeLeft = life; }
	void setColor(LKengine::ColorRGBA8 color) { m_color = color; }


protected:

	glm::vec2 m_position;
	glm::vec2 m_velocity;
	float m_lifeTotal;
	float m_lifeLeft;
	float m_decayRate;


private:

	LKengine::SpriteBatch m_spriteBatch;
	LKengine::ParticleEngine2D m_particleEngine;
	LKengine::ParticleBatch2D* m_magParticleBatch;

	LKengine::TileSheet m_decalTexture;
	int m_tileID;


	float m_angle;
	LKengine::ColorRGBA8 m_color;
	float m_width;
	float m_height;
};

