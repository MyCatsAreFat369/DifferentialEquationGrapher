#ifndef TEXT_MANAGER_CLASS_H
#define TEXT_MANAGER_CLASS_H

#include <utils/trie.hpp>

#include <ft2build.h>
#include FT_FREETYPE_H

#include "rendering/text.h"

#include <glm/glm.hpp>
#include <glm/ext/matrix_transform.hpp> // glm::translate, glm::rotate, glm::scale
#include <glm/ext/matrix_clip_space.hpp> // glm::perspective
#include <glm/ext/scalar_constants.hpp> // glm::pi
#include <glm/gtc/type_ptr.hpp>

class TextManager
{
	public:
		TextManager();
		bool initializeTextManager();
		bool loadFont(std::string fontName, std::string fontPath);
		void doneWithFT();

		void updateTextProjection(int width, int height);

		void renderText(std::string font, std::string text,
				float x, float y, glm::vec2 scale, glm::vec3 color);

		void renderFloatScientific(std::string font, float value, float x, float y, glm::vec2 scale, glm::vec3 color,
										glm::vec2 anchor, glm::vec2 padding,
										bool truncateToInt);

		void Delete();
	private:
		FT_Library ft;
		trie::Trie<TextRenderer> fonts;
		glm::mat4 textProjection;
		Shader textShader;
};


#endif