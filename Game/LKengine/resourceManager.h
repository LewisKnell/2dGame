#pragma once

#include <string>

#include "textureCache.h"

namespace LKengine {

	class ResourceManager
	{
	public:
		static GLTexture getTexture(const std::string &texturePath);


	private:
		static TextureCache m_textureCache;

	};

}
