#include "stdafx.h"

#include <SDL/SDL.h>
#include <algorithm>
#include <iostream>

#include "spriteBatch.h"


namespace LKengine {


	Glyph::Glyph(const glm::vec4& destRect, const glm::vec4& uvRect,
		GLuint Texture, float Depth, const ColorRGBA8& color) :
		texture(Texture),
		depth(Depth)
	{
		topLeft.color = color;
		topLeft.setPosition(destRect.x, destRect.y + destRect.w);
		topLeft.setUV(uvRect.x, uvRect.y + uvRect.w);

		bottomLeft.color = color;
		bottomLeft.setPosition(destRect.x, destRect.y);
		bottomLeft.setUV(uvRect.x, uvRect.y);

		bottomRight.color = color;
		bottomRight.setPosition(destRect.x + destRect.z, destRect.y);
		bottomRight.setUV(uvRect.x + uvRect.z, uvRect.y);

		topRight.color = color;
		topRight.setPosition(destRect.x + destRect.z, destRect.y + destRect.w);
		topRight.setUV(uvRect.x + uvRect.z, uvRect.y + uvRect.w);
	}

	Glyph::Glyph(const glm::vec4& destRect, const glm::vec4& uvRect,
		GLuint Texture, float Depth, const ColorRGBA8& color, float angle) :
		texture(Texture),
		depth(Depth)
	{

		glm::vec2 halfDimensions(destRect.z / 2.0f, destRect.w / 2.0f);

		// Get points centred at origin
		glm::vec2 tl(-halfDimensions.x,  halfDimensions.y);
		glm::vec2 bl(-halfDimensions.x, -halfDimensions.y);
		glm::vec2 br( halfDimensions.x, -halfDimensions.y);
		glm::vec2 tr( halfDimensions.x,  halfDimensions.y);

		// Rotate the points
		// Adding halfDimensions puts the point back in top left corner again
		tl = rotatePoint(tl, angle) + halfDimensions;
		bl = rotatePoint(bl, angle) + halfDimensions;
		br = rotatePoint(br, angle) + halfDimensions;
		tr = rotatePoint(tr, angle) + halfDimensions;

		topLeft.color = color;
		topLeft.setPosition(destRect.x + tl.x, destRect.y + tl.y);
		topLeft.setUV(uvRect.x, uvRect.y + uvRect.w);

		bottomLeft.color = color;
		bottomLeft.setPosition(destRect.x + bl.x, destRect.y + bl.y);
		bottomLeft.setUV(uvRect.x, uvRect.y);

		bottomRight.color = color;
		bottomRight.setPosition(destRect.x + br.x, destRect.y + br.y);
		bottomRight.setUV(uvRect.x + uvRect.z, uvRect.y);

		topRight.color = color;
		topRight.setPosition(destRect.x + tr.x, destRect.y + tr.y);
		topRight.setUV(uvRect.x + uvRect.z, uvRect.y + uvRect.w);
	}

	glm::vec2 Glyph::rotatePoint(glm::vec2 pos, float angle)
	{
		glm::vec2 newVec;
		newVec.x = pos.x * cos(angle) - pos.y * sin(angle);
		newVec.y = pos.x * sin(angle) + pos.y * cos(angle);

		return newVec;
	}




	SpriteBatch::SpriteBatch() : 
		m_vao(0), 
		m_vbo(0), 
		m_sortType(GlyphSortType::TEXTURE)
	{
		// Empty
	}


	SpriteBatch::~SpriteBatch()
	{
		// Empty
	}


	void SpriteBatch::init()
	{
		createVertexArray();
	}

	//default parameter TEXTURE if not set
	void SpriteBatch::begin(GlyphSortType sortType /*= GlyphSortType::TEXTURE*/)
	{
		m_sortType = sortType;

		m_renderBatches.clear(); // .clear() changes size of vector back to 0

		m_glyphs.clear();
	}

	void SpriteBatch::end() 
	{
		// Set up all pointers for fast sorting
		m_glyphPointers.resize(m_glyphs.size());
		for (int i = 0; i < m_glyphs.size(); i++)
		{
			m_glyphPointers[i] = &m_glyphs[i];
		}

		sortGlyphs();
		createRenderBatches();
	}

	void SpriteBatch::draw(const glm::vec4& destRect, const glm::vec4& uvRect, GLuint texture, float depth, const ColorRGBA8& color)
	{
		m_glyphs.emplace_back(destRect, uvRect, texture, depth, color);
	}


	void SpriteBatch::draw(const glm::vec4& destRect, const glm::vec4& uvRect, GLuint texture, float depth, const ColorRGBA8& color, float angle)
	{
		m_glyphs.emplace_back(destRect, uvRect, texture, depth, color, angle);
	}


	void SpriteBatch::draw(const glm::vec4& destRect, const glm::vec4& uvRect, GLuint texture, float depth, const ColorRGBA8& color, const glm::vec2& dir)
	{
		const glm::vec2 right(1.0f, 0.0f);
		float angle = acos(glm::dot(right, dir));
		if (dir.y < 0.0f) {
			angle = -angle;
		}
		//std::cout << angle << std::endl;
		m_glyphs.emplace_back(destRect, uvRect, texture, depth, color, angle);
	}


	void SpriteBatch::renderBatch()
	{
		glBindVertexArray(m_vao);

		for (int i = 0; i < m_renderBatches.size(); i++)
		{
			glBindTexture(GL_TEXTURE_2D, m_renderBatches[i].texture);

			glDrawArrays(GL_TRIANGLES, m_renderBatches[i].offset, m_renderBatches[i].numVertices);
		}

		glBindVertexArray(0);
	}


	void SpriteBatch::createRenderBatches()
	{
		std::vector <Vertex> vertices;
		vertices.resize(m_glyphPointers.size() * 6);

		if (m_glyphPointers.empty()) {
			return;
		}

		int offset = 0;
		int cv = 0; //current vertex
		m_renderBatches.emplace_back(offset, 6, m_glyphPointers[0]->texture);
		vertices[cv++] = m_glyphPointers[0]->topLeft;
		vertices[cv++] = m_glyphPointers[0]->bottomLeft;
		vertices[cv++] = m_glyphPointers[0]->bottomRight;
		vertices[cv++] = m_glyphPointers[0]->bottomRight;
		vertices[cv++] = m_glyphPointers[0]->topRight;
		vertices[cv++] = m_glyphPointers[0]->topLeft;
		offset += 6;

		for (int cg = 1; cg < m_glyphPointers.size(); cg++)
		{
			//if texture is different, place new render batch otherwise use the same texture
			if (m_glyphPointers[cg]->texture != m_glyphPointers[cg - 1]->texture) {
				m_renderBatches.emplace_back(offset, 6, m_glyphPointers[cg]->texture);
			}
			else {
				m_renderBatches.back().numVertices += 6;
			}
			vertices[cv++] = m_glyphPointers[cg]->topLeft;
			vertices[cv++] = m_glyphPointers[cg]->bottomLeft;
			vertices[cv++] = m_glyphPointers[cg]->bottomRight;
			vertices[cv++] = m_glyphPointers[cg]->bottomRight;
			vertices[cv++] = m_glyphPointers[cg]->topRight;
			vertices[cv++] = m_glyphPointers[cg]->topLeft;
			offset += 6;
		}

		glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
		//orphan the buffer
		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), nullptr, GL_DYNAMIC_DRAW);
		//upload the data
		glBufferSubData(GL_ARRAY_BUFFER, 0, vertices.size() * sizeof(Vertex), vertices.data());

		glBindBuffer(GL_ARRAY_BUFFER, 0);

	}


	void SpriteBatch::createVertexArray()
	{

		if (m_vao == 0) {
			glGenVertexArrays(1, &m_vao);
		}
		glBindVertexArray(m_vao);

		if (m_vbo == 0) {
		glGenBuffers(1, &m_vbo);
		}
		glBindBuffer(GL_ARRAY_BUFFER, m_vbo);

		//enable out GLSL variables for the shaders
		glEnableVertexAttribArray(0); //position
		glEnableVertexAttribArray(1); //colour
		glEnableVertexAttribArray(2); //uv

		//position attribute pointer
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, position));
		//colour attribute pointer
		glVertexAttribPointer(1, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(Vertex), (void*)offsetof(Vertex, color));
		//uv attribute pointer
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, uv));

		glBindVertexArray(0);

	}

	void SpriteBatch::sortGlyphs()
	{
		switch (m_sortType) {
		case GlyphSortType::BACK_TO_FRONT:
			std::stable_sort(m_glyphPointers.begin(), m_glyphPointers.end(), compareBackToFront);
			break;
		case GlyphSortType::FRONT_TO_BACK:
			std::stable_sort(m_glyphPointers.begin(), m_glyphPointers.end(), compareFrontToBack);
			break; 
		case GlyphSortType::TEXTURE:
			std::stable_sort(m_glyphPointers.begin(), m_glyphPointers.end(), compareTexture);
			break;
		}

	}

	bool SpriteBatch::compareFrontToBack(Glyph* a, Glyph*b)
	{
		return (a->depth < b->depth);
	}


	bool SpriteBatch::compareBackToFront(Glyph* a, Glyph*b)
	{
		return (a->depth > b->depth);
	}


	bool SpriteBatch::compareTexture(Glyph* a, Glyph*b)
	{
		return (a->texture < b->texture);
	}

}