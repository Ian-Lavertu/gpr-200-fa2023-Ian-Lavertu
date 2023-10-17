#include "texture.h"
#include "../ew/external/stb_image.h"
#include "../ew/external/glad.h"

unsigned int loadTexture(const char* filePath, int wrapMode, int minFilter, int magFilter) {
	stbi_set_flip_vertically_on_load(true);
	int width, height, numComponents;
	unsigned char* data = stbi_load(filePath, &width, &height, &numComponents, 0);
	if (data == NULL) {
		printf("Failed to load image %s", filePath);
		stbi_image_free(data);
		return 0;
	}
	unsigned int texture;

	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);

	glTexImage2D(GL_TEXTURE_2D, 0, getFormat(numComponents), width, height, 0, getFormat(numComponents), GL_UNSIGNED_BYTE, data);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, getTextureWrapS(wrapMode));
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, getTextureWrapT(wrapMode));
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, getMinFilter(minFilter));
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, getMagFilter(magFilter));

	glGenerateMipmap(GL_TEXTURE_2D);

	glBindTexture(GL_TEXTURE_2D, 0);
	stbi_image_free(data);
	return texture;
}

GLenum getFormat(int numComponents) {
	GLenum format;

	switch (numComponents) {
	case 1:
		format = GL_RED;
		break;
	case 2:
		format = GL_RG;
		break;
	case 3:
		format = GL_RGB;
		break;
	case 4:
		format = GL_RGBA;
		break;
	default:
		format = GL_RED;
		break;
	}

	return format;
}

GLenum getTextureWrapS(int wrapMode) {
	GLenum textureWrapS;

	switch (wrapMode) {
	case 1:
		textureWrapS = GL_REPEAT;
		break;
	case 2:
		textureWrapS = GL_CLAMP_TO_EDGE;
		break;
	default:
		textureWrapS = GL_REPEAT;
		break;
	}

	return textureWrapS;
}

GLenum getTextureWrapT(int wrapMode) {
	GLenum textureWrapT;

	switch (wrapMode) {
	case 1:
		textureWrapT = GL_REPEAT;
		break;
	case 2:
		textureWrapT = GL_CLAMP_TO_EDGE;
		break;
	default:
		textureWrapT = GL_REPEAT;
		break;
	}

	return textureWrapT;
}

GLenum getMinFilter(int filterMode) {
	GLenum minFilter;

	switch (filterMode) {
	case 1:
		minFilter = GL_LINEAR;
		break;
	case 2:
		minFilter = GL_NEAREST;
		break;
	default:
		minFilter = GL_LINEAR_MIPMAP_LINEAR;
		break;
	}

	return minFilter;
}

GLenum getMagFilter(int filterMode) {
	GLenum magFilter;

	switch (filterMode) {
	case 1:
		magFilter = GL_LINEAR;
		break;
	case 2:
		magFilter = GL_NEAREST;
		break;
	default:
		magFilter = GL_LINEAR;
		break;
	}

	return magFilter;
}