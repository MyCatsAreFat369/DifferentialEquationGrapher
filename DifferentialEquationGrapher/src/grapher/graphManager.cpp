#include <grapher/graphManager.h>

#include "calculator/equation.h"
#include "calculator/equationList.h"
#include "calculator/variableList.h"

#include "grapher/calculator.h"

#include <cstring>

GraphManager::GraphManager(GLuint vertexShader, GLuint fragmentShader,
						   EquationList* equationList, VariableList* variableList,
						   Calculator* calculator, Input* input,
						   TextManager* textManager,
						   float initialX, float initialY, float initialZoomX, float initialZoomY,
						   int width, int height)
{
	this->vertexShader = vertexShader;
	this->fragmentShader = fragmentShader;

	this->equationList = equationList;
	this->variableList = variableList;
	this->calculator = calculator;
	this->input = input;

	x = initialX, y = initialY;
	zoomX = initialZoomX, zoomY = initialZoomY;
	if(zoomX == 0.0f) zoomX = 1.0f; // just to be safe

	// Consider moving GraphLines creation before Curve
	graphLines = new GraphLines(vertexShader, fragmentShader, textManager,
								x, y, zoomX, zoomY);
	graphLines->UpdatePosition(x, y);
	graphLines->UpdateZoom(width, height, zoomX, zoomY);

	// Initialize pre-positions for panning
	preX = x, preY = y;
	preMouseX = 0.0, preMouseY = 0.0;
	panningState = NONE;

	width = 800;
	height = 800;
}


void GraphManager::redrawCurves()
{
	if(equationList->EquationCount() <= 0) return;

	float scaleRatio = (float)width / height;
	calculator->redrawCurves(-x * scaleRatio / zoomX, zoomX, zoomY, scaleRatio);
}

void GraphManager::render(int width, int height, bool ignoreMouse)
{
	this->width = width, this->height = height;

	if(!ignoreMouse) pan();
	if(!ignoreMouse) zoom();
	redraw();

	/// Draw graphLines
	graphLines->Draw(width, height);

	/// Draw curve
	for (int i = 0; i < equationList->EquationCount(); i++)
	{
		Equation* equation = equationList->GetEquation(i);
		if(!equation->isValidEquation()) continue;
		if(!equation->drawCurve) continue;

		equation->curve->Draw(width, height, equation->color);
	}
}

void GraphManager::Delete()
{
	graphLines->Delete();
	delete graphLines;
}

void GraphManager::pan()
{
	/// Pan
	// if mouse button not down, return
	if (!input->leftMouseButtonDown())
	{
		panningState = NONE;
		return;
	}
	
	// checks if clicked outside initially, then continuously returns falls if done so
	if (panningState == NONE &&
		(input->getMouseX() < 400 || input->getMouseY() < 100))
	{
		panningState = CLICKED_OUTSIDE;
	}
	if(panningState == CLICKED_OUTSIDE) return;

	// Initializes prepositions since it has clicked inside
	if (panningState == NONE)
	{
		preMouseX = input->getMouseX(), preMouseY = input->getMouseY();
		preX = x, preY = y;
		panningState = CLICKED_INSIDE;
	}

	/// Consider making graphLines the first one to get the new position, or perhaps make the position a variable in main class
	x = preX + (input->getMouseX() - preMouseX) / 800.0;
	y = preY - (input->getMouseY() - preMouseY) / 800.0;
	graphLines->UpdatePosition(x, y);
	for (int i = 0; i < equationList->EquationCount(); i++)
	{
		Equation* equation = equationList->GetEquation(i);
		if(!equation->isValidEquation()) continue;

		equation->curve->UpdatePosition(x, y);
	}
}

void GraphManager::zoom()
{
	/// Zoom
	/// TODO: Make it so that zooming zooms to the mouse, not the center of the screen
	double mouseScrollY = input->getMouseScrollY();
	if (mouseScrollY != 0)
	{
		if (input->getKeyDown(GLFW_KEY_LEFT_SHIFT))
		{
			float scalingFactor = mouseScrollY / 20.0f;
			x += scalingFactor * x;
			zoomX += scalingFactor * zoomX;
		}
		else
		{
			float scalingFactor = mouseScrollY / 20.0f;
			y += scalingFactor * y;
			zoomY += scalingFactor * zoomY;
		}

		graphLines->UpdateZoom(width, height, zoomX, zoomY);
		for (int i = 0; i < equationList->EquationCount(); i++)
		{
			Equation* equation = equationList->GetEquation(i);
			if(!equation->isValidEquation()) continue;

			Curve* curve = equation->curve;
			curve->UpdatePosition(x, y);
			curve->UpdateScale(zoomX, zoomY);
		}
	}
}

void GraphManager::redraw()
{
	for (int i = 0; i < equationList->EquationCount(); i++)
	{
		Equation* equation = equationList->GetEquation(i);
		if(!equation->isValidEquation()) continue;

		Curve* curve = equation->curve;

		if(variableList->functionVariableList.find(equation->functionName) == variableList->functionVariableList.end()) continue;

		float scaleRatio = (float)width / height;

		bool curveRedraw_isTooZoomed = ((curve->points->t0_right - curve->points->t0_left) * curve->scaleX >= 2.0f * CURVE_MAX_RADIUS_PER_WIDTH);
		bool curveRedraw_middleShows = (abs(curve->points->t0 * curve->scaleX + curve->x * scaleRatio) <= scaleRatio);
		bool curveRedraw_leftShows = (curve->points->t0_left * curve->scaleX + curve->x * scaleRatio >= -scaleRatio);
		bool curveRedraw_rightShows = (curve->points->t0_right * curve->scaleX + curve->x * scaleRatio <= scaleRatio);

		if (curveRedraw_isTooZoomed ||
			(curveRedraw_middleShows && (curveRedraw_leftShows || curveRedraw_rightShows)))
		{
			std::cout << "I run\n";
			calculator->redrawCurves(-curve->x * scaleRatio / zoomX, zoomX, zoomY, scaleRatio);
		}
		else if (curveRedraw_leftShows)
		{
			std::cout << "Gonna draw left\n";
			calculator->redrawCurves(curve->points->t0 - (CURVE_RADIUS_PER_WIDTH / zoomX), zoomX, zoomY, scaleRatio);
		}
		else if (curveRedraw_rightShows)
		{
			std::cout << "Gonna draw right\n";
			calculator->redrawCurves(curve->points->t0 + (CURVE_RADIUS_PER_WIDTH / zoomX), zoomX, zoomY, scaleRatio);
		}

		break;
	}
}