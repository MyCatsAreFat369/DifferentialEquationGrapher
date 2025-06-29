#ifndef LINE_CLASS_H
#define LINE_CLASS_H

#include "rendering/VAO.h"
#include "rendering/VBO.h"
#include "rendering/EBO.h"
#include "rendering/shaderClass.h"


class Line
{
	public:
		Line();
		Line(float x, float y, float scaleX, float scaleY);
		Line(float x, float y, float scaleX, float scaleY, float r, float g, float b);
		void AttachShaders(GLuint vertexShader, GLuint fragmentShader);

		void SetColor(float r, float g, float b);
		void Draw(int width, int height);
		void Delete();
		float x = 0.0f, y = 0.0f, scaleX = 1.0f, scaleY = 1.0f, r = 1.0f, g = 1.0f, b = 1.0f;
	private:
		VAO* VAO1;
		VBO* VBO1;
		Shader* shader;
		GLuint xID, yID, scalexID, scaleyID, colorID;
		GLuint widthID, heightID;
};


#endif