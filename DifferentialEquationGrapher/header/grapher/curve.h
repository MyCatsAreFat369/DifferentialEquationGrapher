#ifndef CURVE_CLASS_H
#define CURVE_CLASS_H

#include <glad/glad.h>
#include "VAO.h"
#include "VBO.h"
#include "EBO.h"
#include "shaderClass.h"
#include "points.h"

class Curve
{
	public:
		Curve(Points* points, float initialScaleX, float initialScaleY);
		void AttachShaders(GLuint vertexShader, GLuint fragmentShader);

		void Draw();
		void Delete();

		Points* points;

		float scaleX = 1.0f, scaleY = 1.0f;
		float x = 0.0f, y = 0.0f;

		GLuint vertexShader, fragmentShader;
	private:
		VAO* VAO1;
		VBO* VBO1;
		EBO* EBO1;
		Shader* shader;
		GLuint scalexID, scaleyID, xID, yID;
};

#endif