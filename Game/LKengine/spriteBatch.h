#pragma once

#include <GL\glew.h>
#include <glm\glm.hpp>

#include <vector>

#include "vertex.h"

namespace LKengine {

	enum class GlyphSortType {
		NONE,
		FRONT_TO_BACK,
		BACK_TO_FRONT,
		TEXTURE
	};

	class Glyph {

	public:
		Glyph() {}
		Glyph(const glm::vec4& destRect, const glm::vec4& uvRect,
			GLuint Texture, float Depth, const ColorRGBA8& color);
		Glyph(const glm::vec4& destRect, const glm::vec4& uvRect,
			GLuint Texture, float Depth, const ColorRGBA8& color, float angle);

		GLuint texture;
		float depth;

		Vertex topLeft;
		Vertex bottomLeft;
		Vertex topRight;
		Vertex bottomRight;

	private:
		glm::vec2 rotatePoint(glm::vec2 pos, float angle);
	};


	class RenderBatch
	{
	public:
		RenderBatch(GLuint newOffset, GLuint newNumVertices, GLuint newTexture) :
			offset(newOffset),
			numVertices(newNumVertices),
			texture(newTexture)
		{
		}

		GLuint offset;
		GLuint numVertices;
		GLuint texture;

	};

	class SpriteBatch
	{
	public:
		SpriteBatch();
		~SpriteBatch();

		// Initialises the spritebatch
		void init();

		// Begins the spritebatch
		void begin(GlyphSortType sortType = GlyphSortType::TEXTURE);

		// Ends the spritebatch
		void end();

		// Adds a glyph to the spritebatch
		void draw(const glm::vec4& destRect, const glm::vec4& uvRect, GLuint texture, float depth, const ColorRGBA8& color);

		void draw(const glm::vec4& destRect, const glm::vec4& uvRect, GLuint texture, float depth, const ColorRGBA8& color, float angle);

		void draw(const glm::vec4& destRect, const glm::vec4& uvRect, GLuint texture, float depth, const ColorRGBA8& color, const glm::vec2& dir);
		// Renders the entire spritebatch to the screen
		void renderBatch();

	private:
		// Creates all the needed RenderBatches
		void createRenderBatches();
		//Generates the VAO and VBO
		void createVertexArray();
		// Sort glyps according to m_sortType
		void sortGlyphs();

		static bool compareFrontToBack(Glyph* a, Glyph*b);
		static bool compareBackToFront(Glyph* a, Glyph*b);
		static bool compareTexture(Glyph* a, Glyph*b);

		GLuint m_vao;
		GLuint m_vbo;

		GlyphSortType m_sortType;

		std::vector<Glyph*> m_glyphPointers; ///< This is for sorting
		std::vector<Glyph> m_glyphs; ///< These are the actual glyphs
		std::vector<RenderBatch> m_renderBatches;


	};

}