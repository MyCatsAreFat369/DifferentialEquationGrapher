#ifndef CURVE_CLASS_H
#define CURVE_CLASS_H

#include <glad/glad.h>
#include "rendering/VAO.h"
#include "rendering/VBO.h"
#include "rendering/EBO.h"
#include "rendering/shaderClass.h"
#include "points.h"

#include "calculator/calculatorDefs.h"

class Curve
{
	public:
		Curve(Points* points, float initialScaleX, float initialScaleY);
		void AttachShaders(GLuint vertexShader, GLuint fragmentShader);

		void UpdatePosition(float x, float y);
		void UpdateScale(float scaleX, float scaleY);

		void Draw(float* color);
		void Delete(bool deletePoints = false);

		Equation* equation;
		Points* points;

		float scaleX = 1.0f, scaleY = 1.0f;
		float x = 0.0f, y = 0.0f;

		GLuint vertexShader, fragmentShader;
	private:
		VAO* VAO1;
		VBO* VBO1;
		EBO* EBO1;
		Shader* shader;
		GLuint scalexID, scaleyID, xID, yID, colorID;
};

#endif