#include "stdafx.h"
#include <iostream>

#include "textureCache.h"
#include "imageLoader.h"

namespace LKengine {

	TextureCache::TextureCache()
	{
	}


	TextureCache::~TextureCache()
	{
	}


	GLTexture TextureCache::getTexture(const std::string &filePath)
	{
		//lookup the texture and see if it's in the map
		auto iterator = m_textureMap.find(filePath);

		//check if it's not in the map
		if (iterator == m_textureMap.end())
		{
			//load the texture
			GLTexture newTexture = ImageLoader::loadPNG(filePath);

			//insert it into the map
			m_textureMap.insert(make_pair(filePath, newTexture));

			///output if texture has been successfully loaded
			std::cout << "Loaded texture!\n";
			return newTexture;
		}

		///turn on to debug if texture has used cached texture
		//std::cout << "Used cached texture!\n";
		//if finds texture, returns the second part (the texture not the key)
		return iterator->second;
	}

}