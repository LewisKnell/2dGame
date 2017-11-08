#include "stdafx.h"
#include "resourceManager.h"

namespace LKengine {

	TextureCache ResourceManager::m_textureCache;

	GLTexture ResourceManager::getTexture(const std::string &texturePath)
	{
		return m_textureCache.getTexture(texturePath);
	}

}