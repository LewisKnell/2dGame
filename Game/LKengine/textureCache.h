#pragma once

#include <map>

#include "GLTexture.h"

namespace LKengine {

	class TextureCache
	{
	public:
		TextureCache();
		~TextureCache();

		GLTexture getTexture(const std::string &filePath);


	private:
		std::map<std::string, GLTexture> m_textureMap;

	};

}