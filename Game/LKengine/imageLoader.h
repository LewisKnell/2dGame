#pragma once

#include <string>

#include "GLTexture.h"

namespace LKengine {

	class ImageLoader
	{
	public:
		static GLTexture loadPNG(std::string filePath);


	};

}
