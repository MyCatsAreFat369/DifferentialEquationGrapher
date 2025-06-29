#ifndef TEXT_H
#define TEXT_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <ft2build.h>
#include FT_FREETYPE_H

#include <glm/glm.hpp>

#include <string>
#include <map>

#include "rendering/shaderClass.h"
#include "rendering/VAO.h"

typedef struct
{
	unsigned int	textureID;	// texture id storing character
	glm::ivec2		size;		// size of char
	glm::ivec2		bearing;	// distance from origin to top left of char
	unsigned int	advance;	// distance from origin to next origin
} Character;

class TextRenderer
{
	public:
		TextRenderer();
		TextRenderer(int height);

		bool loadFont(FT_Library &ft, std::string path);

		void render(Shader shader, std::string text, float x, float y, glm::vec2 scale, glm::vec3 color);
		void renderFloatScientific(Shader shader, float value, float x, float y, glm::vec2 scale, glm::vec3 color,
								   glm::vec2 anchor, glm::vec2 padding,
								   bool truncateToInt);

		void Delete();

	private:
		int height;

		std::map<char, Character> chars;

		GLuint VAO;
		GLuint VBO;

		glm::vec2 getTextDimensionsScientific(std::string text, glm::vec2 scale);
};

#endif