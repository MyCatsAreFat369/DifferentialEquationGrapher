#include <texture2d.h>

Texture2D::Texture2D(const char* textureFile, GLenum slot, GLint filterMode, GLint wrapMode, GLenum format, GLenum pixelType)
{
	unsigned char* bytes = stbi_load(textureFile, &imgWidth, &imgHeight, &numberOfColorChannels, 0);

	// generates and stores texture's ID
	// sets texture as active on the 0th slot
	// binds this texture as current
	glGenTextures(1, &ID);
	glActiveTexture(slot); // change this to fit our needs for many images
	glBindTexture(GL_TEXTURE_2D, ID);

	// sets filter settings for minification and magnification to the mode (for example NEAREST)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filterMode);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filterMode);

	// sets wrap mode to repeat (could do GL_CLAMP_TO_BORDER as well)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapMode);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapMode);

	// generates image
	// generates mimap (smaller versions of the image for when it's viewed from far away)
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imgWidth, imgHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, bytes);
	glGenerateMipmap(GL_TEXTURE_2D);

	// free the bytes
	// bind the texture to 0 so we don't modify our texture object
	stbi_image_free(bytes);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture2D::texUnit(Shader shader, const char* uniform, GLuint unit)
{
	GLuint texUni = glGetUniformLocation(shader.ID, uniform);
	shader.Activate();
	glUniform1i(texUni, unit);
}

void Texture2D::Bind()
{
	glBindTexture(GL_TEXTURE_2D, ID);
}

void Texture2D::Unbind()
{
	glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture2D::Delete()
{
	glDeleteTextures(1, &ID);
}