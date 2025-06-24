#ifndef GRAPH_MANAGER_CLASS_H
#define GRAPH_MANAGER_CLASS_H


#include "input.h"
#include "grapher/curve.h"
#include "grapher/graphLines.h"
#include <string>

//const int EQUATION_MAX_LENGTH = 4096;

class GraphManager
{
	public:
		GraphManager(GLuint vertexShader, GLuint fragmentShader, Input* input,
					 float initialX, float initialY, float initialZoomX, float initialZoomY,
					 float x0, float v0);

		void render(int width, int height, bool ignoreMouse);

		void Delete();

		GraphLines* graphLines;
		
		float x, y, zoomX, zoomY;

		bool drawCurve = true;
		//char* equation = new char[EQUATION_MAX_LENGTH];
		float prex0, prev0, x0, v0;
		float color[3] = {1.0f, 1.0f, 1.0f};

		void ClearEquation();

		enum PanningState
		{
			NONE,
			CLICKED_OUTSIDE,
			CLICKED_INSIDE
		};
	private:
		void pan();
		void zoom();
		void redraw();

		Input* input;

		Curve* curve;

		int width, height;

		double preX, preY;
		double preMouseX = 0.0, preMouseY = 0.0;
		PanningState panningState;
};



#endif