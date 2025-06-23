#ifndef TEXTURE_CLASS_H
#define TEXTURE_CLASS_H

#include <glad/glad.h>
#include <stb/stb_image.h>
#include "shaderClass.h"

class Texture2D
{
	public:
		GLuint ID;
		int imgWidth, imgHeight, numberOfColorChannels;
		Texture2D(const char* textureFile, GLenum slot, GLint filterMode, GLint wrapMode, GLenum format, GLenum pixelType);

		void texUnit(Shader shader, const char* uniform, GLuint unit);
		void Bind();
		void Unbind();
		void Delete();
};



#endif