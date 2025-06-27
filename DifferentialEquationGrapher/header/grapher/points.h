#ifndef POINTS_CLASS_H
#define POINTS_CLASS_H

#include <glad/glad.h>

// this constant should always be odd, or else the program breaks!
const int CURVE_POINTS_SIZE = 2047;
const double CURVE_RADIUS_PER_WIDTH = 5.0;
const double CURVE_MAX_RADIUS_PER_WIDTH = 10.0;

class Points
{
	public:
		Points()
		{
			points = new GLfloat[CURVE_POINTS_SIZE * 6];
			indices = new GLuint[(CURVE_POINTS_SIZE - 1) * 2];

			t0 = 0.0f;
			t0_left = 0.0f;
			t0_right = 0.0f;
		}

		void Delete()
		{
			delete[] points;
			delete[] indices;
		}

		GLfloat* points;
		GLuint* indices;


		float t0, t0_left, t0_right;
};

#endif