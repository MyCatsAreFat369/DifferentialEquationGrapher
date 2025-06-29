#ifndef GRAPH_MANAGER_CLASS_H
#define GRAPH_MANAGER_CLASS_H


#include "input.h"
#include "grapher/curve.h"
#include "grapher/graphLines.h"
#include <string>

#include "calculator/calculatorDefs.h"
#include "grapher/grapherDefs.h"

//const int EQUATION_MAX_LENGTH = 4096;

const int CURVE_MAXIMUM_AMOUNT = 99;

class GraphManager
{
	public:
		GraphManager(GLuint vertexShader, GLuint fragmentShader,
					 EquationList* equationList, VariableList* variableList,
					 Calculator* calculator, Input* input,
					 TextManager* textManager,
					 float initialX, float initialY, float initialZoomX, float initialZoomY,
					 int width, int height);

		void redrawCurves();
		void render(int width, int height, bool ignoreMouse);

		void Delete();

		GLuint vertexShader, fragmentShader;
		GraphLines* graphLines;
		//std::vector<Curve*> curves;
		
		float x, y, zoomX, zoomY;

		//bool drawCurve = true;
		//float prex0, prev0, x0, v0;
		//float color[3] = {1.0f, 1.0f, 1.0f};

		//void ClearEquation();

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

		EquationList* equationList;
		VariableList* variableList;
		Calculator* calculator;

		//Curve* curve;

		int width, height;

		double preX, preY;
		double preMouseX = 0.0, preMouseY = 0.0;
		PanningState panningState;
};



#endif