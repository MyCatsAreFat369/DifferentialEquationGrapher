#include <rendering/text.h>


std::string floatToScientificStr(float value, bool truncateCoefficient);

TextRenderer::TextRenderer()
{

}

TextRenderer::TextRenderer(int height)
	: height(height)
{
	std::cout << floatToScientificStr(1.0f, true) << ", " << floatToScientificStr(0.7f, true) << ", " << floatToScientificStr(1.3f, true) << ", " <<
		floatToScientificStr(0.0f, true) << ", " << floatToScientificStr(3000000.0f, true) << ", " << floatToScientificStr(0.0000006f, true) << std::endl;

	std::cout << floatToScientificStr(1.00001f, true) << ", " << floatToScientificStr(4.99999f, true) << ", " << floatToScientificStr(4.9999f, true) << ", "
		<< floatToScientificStr(4.999f, true) << ", " << floatToScientificStr(0.0001f, true) << ", " << floatToScientificStr(0.001f, true) << std::endl;
}

bool TextRenderer::loadFont(FT_Library& ft, std::string path)
{
	FT_Face fontFace;

	if (FT_New_Face(ft, path.c_str(), 0, &fontFace))
	{
		return false;
	}

	// set width and height
	FT_Set_Pixel_Sizes(fontFace, 0, height); // 0 for width so we can dynamically size it

	// load texture
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // disables the byte-alignment (we'll be using 1 byte for a pixel)

	for (unsigned char c = 0; c < 128; c++)
	{
		// load glyph
		if (FT_Load_Char(fontFace, c, FT_LOAD_RENDER))
		{
			continue;
		}

		// generate texture
		GLuint texture;
		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);
		glTexImage2D(
			GL_TEXTURE_2D,
			0,
			GL_RED,
			fontFace->glyph->bitmap.width,
			fontFace->glyph->bitmap.rows,
			0,
			GL_RED,
			GL_UNSIGNED_BYTE,
			fontFace->glyph->bitmap.buffer
		);

		// set texture parameters
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		// store character texture for use
		chars[c] = {
			texture,
			glm::ivec2(fontFace->glyph->bitmap.width, fontFace->glyph->bitmap.rows),
			glm::ivec2(fontFace->glyph->bitmap_left, fontFace->glyph->bitmap_top),
			(unsigned int) fontFace->glyph->advance.x
		};
	}

	FT_Done_Face(fontFace);

	// setup VAO
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, 6 * 4 * sizeof(GLfloat), nullptr, GL_DYNAMIC_DRAW);

	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (void*)0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	// 2d quad requires 6 vertices of 4 float (2 for pos, 2 for tex coords)


	return true;
}


void TextRenderer::render(Shader shader, std::string text, float x, float y, glm::vec2 scale, glm::vec3 color)
{
	shader.Activate();
	glUniform3f(glGetUniformLocation(shader.ID, "textColor"), color.x, color.y, color.z);

	glActiveTexture(GL_TEXTURE0);

	glBindVertexArray(VAO);

	// go through all characters of string
	for (int i = 0, len = text.size(); i < len; i++)
	{
		Character c = chars[text[i]];

		float xPos = x + c.bearing.x * scale.x;
		float yPos = y - (c.size.y - c.bearing.y) * scale.y; // characters might need to be shifted below baseline

		float width = c.size.x * scale.x;
		float height = c.size.y * scale.y;

		// new VBO data
		float vertices[6 * 4] = {
		//	x				y				texX	texY
			xPos,			yPos + height,	0.0f,	0.0f,
			xPos,			yPos,			0.0f,	1.0f,
			xPos + width,	yPos,			1.0f,	1.0f,

			xPos,			yPos + height,	0.0f,	0.0f,
			xPos + width,	yPos,			1.0f,	1.0f,
			xPos + width,	yPos + height,	1.0f,	0.0f
		};

		// setup texture
		glBindTexture(GL_TEXTURE_2D, c.textureID);

		// update VBO data
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferSubData(GL_ARRAY_BUFFER, 0, 6 * 4 * sizeof(GLfloat), vertices);
		// render quad
		glDrawArrays(GL_TRIANGLES, 0, 6);

		// advance cursor
		x += (c.advance >> 6) * scale.x; // multiply by 64
	}

	glBindVertexArray(0);

	glBindTexture(GL_TEXTURE_2D, 0);
}

void TextRenderer::renderFloatScientific(Shader shader, float value, float x, float y, glm::vec2 scale, glm::vec3 color,
										 glm::vec2 anchor, glm::vec2 padding,
										 bool truncateToInt)
{
	std::string textValue = floatToScientificStr(value, truncateToInt);
	glm::vec2 dimensions = getTextDimensionsScientific(textValue, scale);

	shader.Activate();
	glUniform3f(glGetUniformLocation(shader.ID, "textColor"), color.x, color.y, color.z);

	glActiveTexture(GL_TEXTURE0);

	glBindVertexArray(VAO);

	// go through all characters of string
	float exponentScaleFactor = 1.0f;
	float xScaleFactor = 1.0f;
	for (int i = 0, len = textValue.size(); i < len; i++)
	{
		Character c = chars[textValue[i]];
		if (textValue[i] == '^')
		{
			y += c.bearing.y * scale.y;
			exponentScaleFactor = 0.5f;
			continue;
		}
		if (textValue[i] == 'x') xScaleFactor = 0.5f;
		else xScaleFactor = 1.0f;
		
		// normal position
		float xPos = x + c.bearing.x * scale.x * exponentScaleFactor * xScaleFactor;
		float yPos = y - (c.size.y - c.bearing.y) * scale.y * exponentScaleFactor * xScaleFactor; // (characters may need to be shifted below baseline)
		// apply anchor
		xPos -= (anchor.x + 1) * (dimensions.x / 2);
		yPos -= (anchor.y + 1) * (dimensions.y / 2);
		// apply padding
		xPos -= anchor.x * padding.x;
		yPos -= anchor.y * padding.y;
		/*
		if (i == 0)
		{
			std::cout << "dimensions: " << dimensions.x << ", " << dimensions.y << "\n";
			std::cout << "xPos: " << xPos << ", yPos: " << yPos << "\n";
		}
		*/

		// customize the 'x' for multiplication
		if(xScaleFactor == 0.5f) yPos += 0.25f * c.bearing.y;

		float width = c.size.x * scale.x * exponentScaleFactor * xScaleFactor;
		float height = c.size.y * scale.y * exponentScaleFactor * xScaleFactor;

		// new VBO data
		float vertices[6 * 4] = {
		//	x				y				texX	texY
			xPos,			yPos + height,	0.0f,	0.0f,
			xPos,			yPos,			0.0f,	1.0f,
			xPos + width,	yPos,			1.0f,	1.0f,

			xPos,			yPos + height,	0.0f,	0.0f,
			xPos + width,	yPos,			1.0f,	1.0f,
			xPos + width,	yPos + height,	1.0f,	0.0f
		};

		// setup texture
		glBindTexture(GL_TEXTURE_2D, c.textureID);

		// update VBO data
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferSubData(GL_ARRAY_BUFFER, 0, 6 * 4 * sizeof(GLfloat), vertices);
		// render quad
		glDrawArrays(GL_TRIANGLES, 0, 6);

		// advance cursor
		x += (c.advance >> 6) * scale.x * exponentScaleFactor * xScaleFactor; // multiply by 64
	}

	glBindVertexArray(0);

	glBindTexture(GL_TEXTURE_2D, 0);
}

glm::vec2 TextRenderer::getTextDimensionsScientific(std::string text, glm::vec2 scale)
{
	glm::vec2 dimensions = glm::vec2(0.0f, 0.0f);

	float exponentScaleFactor = 1.0f;
	float xScaleFactor = 1.0f;
	float x = 0.0f, y = 0.0f;
	for (int i = 0, len = text.size(); i < len; i++)
	{
		Character c = chars[text[i]];
		if (text[i] == '^')
		{
			y += c.bearing.y * scale.y;
			exponentScaleFactor = 0.5f;
			continue;
		}
		if (text[i] == 'x') xScaleFactor = 0.5f;
		else xScaleFactor = 1.0f;

		float height = y + c.bearing.y * scale.y * exponentScaleFactor * xScaleFactor;
		if(height > dimensions.y) dimensions.y = height;

		x += (c.advance >> 6) * scale.x * exponentScaleFactor * xScaleFactor;
	}
	dimensions.x = x;

	return dimensions;
}

void TextRenderer::Delete()
{
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
}


std::string floatToScientificStr(float value, bool truncateCoefficient)
{
	if (value == 0.0f)
	{
		if(truncateCoefficient) return "0";
		return "0.0";
	}

	std::string textValue;

	if (abs(value) >= 0.0001 && abs(value) < 10000)
	{
		value = roundf(value * 10000) / 10000;
		textValue = std::to_string(value);
		if (textValue.find('.') != std::string::npos)
		{
			int subLength = textValue.length();
			for (int i = textValue.length(); i > 0;)
			{
				i--;
				if(textValue[i] == '0') subLength--;
				else if (textValue[i] == '.')
				{
					subLength--;
					break;
				}
				else break;
			}
			textValue = textValue.substr(0, subLength);
		}
		return textValue;
	}

	// Depending on the float, only one or the other loop will run
	int exponent = 0;
	while (abs(value) >= 10.0f) // For floats with a positive exponent
	{
		value /= 10.0f;
		exponent++;
	}
	while (abs(value) < 1.0f) // For floats with a negative exponent
	{
		value *= 10.0f;
		exponent--;
	}

	textValue = std::to_string(value).substr(0, (truncateCoefficient ? 1 : 3) + (value < 0 ? 1 : 0));
	textValue += "x10^";
	textValue += std::to_string(exponent);
	return textValue;
}