#pragma once

#include "../ew/external/stb_image.h"
#include "../ew/external/glad.h"

unsigned int loadTexture(const char* filePath, int wrapMode, int minFilter, int magFilter);
GLenum getFormat(int numComponents);
GLenum getTextureWrapS(int wrapMode);
GLenum getTextureWrapT(int wrapMode);
GLenum getMinFilter(int filterMode);
GLenum getMagFilter(int filterMode);