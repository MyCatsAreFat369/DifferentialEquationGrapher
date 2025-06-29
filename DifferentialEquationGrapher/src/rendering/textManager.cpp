#include <rendering/textManager.h>


TextManager::TextManager()
{
	textShader = Shader(
		Shader::CompileShader(GL_VERTEX_SHADER, "res/Shaders/text.vert"),
		Shader::CompileShader(GL_FRAGMENT_SHADER, "res/Shaders/text.frag")
	);
}

bool TextManager::initializeTextManager()
{
	if (FT_Init_FreeType(&ft))
	{
		std::cout << "Could not init FreeType library" << std::endl;
		return false;
	}
	return true;
}

bool TextManager::loadFont(std::string fontName, std::string fontPath)
{
	// insert font
	fonts.insert(fontName, TextRenderer(32));
	if (!fonts[fontName].loadFont(ft, fontPath))
	{
		std::cout << "Could not load font \"" << fontName << "\" at font path \"" << fontPath << "\"" << std::endl;
		return false;
	}
	return true;
}

void TextManager::doneWithFT()
{
	FT_Done_FreeType(ft);
}

void TextManager::updateTextProjection(int width, int height)
{
	textProjection = glm::ortho(0.0f, (float)width, 0.0f, (float)height);
}

void TextManager::renderText(std::string font, std::string text,
				float x, float y, glm::vec2 scale, glm::vec3 color)
{
	textShader.Activate();
	glUniformMatrix4fv(glGetUniformLocation(textShader.ID, "projection"), 1, GL_FALSE, glm::value_ptr(textProjection));

	fonts[font].render(textShader, text, x, y, scale, color);
}

void TextManager::renderFloatScientific(std::string font, float value, float x, float y, glm::vec2 scale, glm::vec3 color,
										glm::vec2 anchor, glm::vec2 padding,
										bool truncateToInt)
{
	textShader.Activate();
	glUniformMatrix4fv(glGetUniformLocation(textShader.ID, "projection"), 1, GL_FALSE, glm::value_ptr(textProjection));

	fonts[font].renderFloatScientific(textShader, value, x, y, scale, color,
									  anchor, padding, truncateToInt);
}

void TextManager::Delete()
{
	fonts.traverse([](TextRenderer tr) -> void {
		tr.Delete();
	});
	fonts.cleanup();
}