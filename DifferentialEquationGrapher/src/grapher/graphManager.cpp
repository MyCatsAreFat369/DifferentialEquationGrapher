#include <grapher/graphManager.h>

#include "calculator/equation.h"
#include "calculator/equationList.h"
#include "calculator/variableList.h"

#include "grapher/calculator.h"

#include <cstring>

GraphManager::GraphManager(GLuint vertexShader, GLuint fragmentShader,
						   EquationList* equationList, VariableList* variableList,
						   Calculator* calculator, Input* input,
						   float initialX, float initialY, float initialZoomX, float initialZoomY)
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
	graphLines = new GraphLines(vertexShader, fragmentShader, x, y, zoomX, zoomY);
	graphLines->UpdatePosition(x, y);

	// Initialize Curve

	//this->prex0 = x0, this->prev0 = v0;
	//this->x0 = x0, this->v0 = v0;
	
	//float t0 = x / zoomX;
	//Points* points = new Points();
	//getPointsFromOrigin(points, t0, x0, v0, zoomX, zoomY);

	//curve = new Curve(points, zoomX, zoomY);
	//curve->AttachShaders(vertexShader, fragmentShader);
	//curve->x = initialX, curve->y = initialY;

	// Initialize pre-positions for panning
	preX = x, preY = y;
	preMouseX = 0.0, preMouseY = 0.0;
	panningState = NONE;

	width = 800;
	height = 800;

	/*
	for (int i = 0; i < CURVE_MAXIMUM_AMOUNT; i++)
	{
		//std::cout << "Creating a new Curve object...\n";
		Points* points = new Points();
		Curve* curve = new Curve(points, initialZoomX, initialZoomY);
		curve->Generate();
		curve->AttachShaders(vertexShader, fragmentShader);
		//std::cout << curve->points << std::endl;

		curves.push_back(curve);
	}
	*/
	
}

/*
void GraphManager::addCurve(Equation* equation)
{
	Points* points = new Points();
	Curve* curve = new Curve(points, zoomX, zoomY);
	curve->Generate();
	curve->AttachShaders(vertexShader, fragmentShader);

	curves.push_back(curve);
}
*/


void GraphManager::redrawCurves()
{
	//std::cout << "This works\n";
	//std::cout << equationList << std::endl;
	calculator->redrawCurves(-x / zoomX, zoomX, zoomY);
	/*
	for (int i = 0; i < equationList->EquationCount(); i++)
	{
		//std::cout << i << " : " << equationList->EquationCount();
		Equation* equation = equationList->GetEquation(i);
		//std::cout << " : " << equation << std::endl;
		Curve* curve = equation->curve;

		if(variableList->functionVariableList.find(equation->functionName) == variableList->functionVariableList.end()) continue;

		std::cout << "Function is still gonna run: " << equation->functionName << std::endl;

		//std::cout << "Points: " << curve->points << std::endl;

		//std::cout << "Points: " << curve->points << std::endl;
	}
	*/
}

void GraphManager::render(int width, int height, bool ignoreMouse)
{
	this->width = width, this->height = height;
	
	//std::cout << "I am a bad person: " << curves[0]->points << std::endl;

	if(!ignoreMouse) pan();
	if(!ignoreMouse) zoom();
	//redraw();

	/// Draw graphLines
	graphLines->Draw();

	/// Draw curve
	for (int i = 0; i < equationList->EquationCount(); i++)
	{
		Equation* equation = equationList->GetEquation(i);
		if(!equation->isValidEquation()) continue;
		if(!equation->drawCurve) continue;

		equation->curve->Draw(equation->color);
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

		graphLines->UpdateZoom(zoomX, zoomY);
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

		bool curveRedraw_isTooZoomed = ((curve->points->t0_right - curve->points->t0_left) * curve->scaleX >= 2 * CURVE_MAX_RADIUS_PER_WIDTH);
		bool curveRedraw_middleShows = (abs(curve->points->t0 * curve->scaleX + curve->x) <= 1.0f);
		bool curveRedraw_leftShows = (curve->points->t0_left * curve->scaleX + curve->x >= -1.0f);
		bool curveRedraw_rightShows = (curve->points->t0_right * curve->scaleX + curve->x <= 1.0);

		if (curveRedraw_leftShows || curveRedraw_rightShows ||
			curveRedraw_isTooZoomed ||
			(curveRedraw_middleShows && (curveRedraw_leftShows || curveRedraw_rightShows)))
		{
			calculator->redrawCurves(-x / zoomX, zoomX, zoomY);
		}

		break;

		/*
		if ((curveRedraw_leftShows && curveRedraw_rightShows) ||
			(curveRedraw_isTooZoomed) ||
			(curveRedraw_middleShows && (curveRedraw_leftShows || curveRedraw_rightShows)))
		{
			calculator->redrawCurves(-x / zoomX, zoomX, zoomY);
		}
		else if (curveRedraw_leftShows)
		{
			calculator->redrawCurves(-x / zoomX, zoomX, zoomY);
		}
		else if (curveRedraw_rightShows)
		{
			calculator->redrawCurves(-x / zoomX, zoomX, zoomY);
		}
		*/
		
	}
}