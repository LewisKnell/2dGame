#pragma once

#include <glm\glm.hpp>
#include "GLTexture.h"

namespace LKengine {

	class TileSheet {
	public:

		void init(const LKengine::GLTexture texture, glm::ivec2& tileDims) {
			this->texture = texture;
			this->dims = tileDims;
		}


		// Bottom left is 0. Goes to the right then up a row and from the left again
		glm::vec4 getUVs(int index) {
			int xTile = index % dims.x;
			int yTile = index / dims.x;

			glm::vec4 uvs;
			uvs.x = xTile / (float)dims.x;
			uvs.y = yTile / (float)dims.y;
			uvs.z = 1.0f / dims.x; ///< Width
			uvs.w = 1.0f / dims.y; ///< Height

			return uvs;
		}

		// Top left is 0. Goes to the right then down a row and from the left again
		glm::vec4 getUVsTopLeft(int index) {
			int xTile = index % dims.x;
			int yTile = index / dims.x;

			glm::vec4 uvs;
			uvs.x = xTile / (float)dims.x;
			uvs.y = 1 - (yTile / (float)dims.y) - (1.0f / dims.y);
			uvs.z = 1.0f / dims.x; ///< Width
			uvs.w = 1.0f / dims.y; ///< Height

			return uvs;
		}

		GLTexture texture;
		glm::ivec2 dims;
	};

}