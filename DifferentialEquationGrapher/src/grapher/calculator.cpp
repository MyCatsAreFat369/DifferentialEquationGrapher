#include <grapher/calculator.h>

#include "calculator/equation.h"
#include "calculator/variable.h"
#include "calculator/equationList.h"
#include "calculator/variableList.h"

float func1(float x, float v, float time)
{
	//return -100 * x - v + 10 * time + 50;
	return -10 * x;
}

/*
// position -1 means the left side of the curve came into the screen, and we'll be drawing from right to left to fill in the gap
// vice versa for position 1, where the right side comes into the screen, and we draw from left to right
// position 0 just means redraw the curve but with a new zoom (to account for zooming cases)
Curve* redrawCurve(Curve* oldCurve, int position)
{
	std::cout << "redrawing curve...\n";
	Points* oldPoints = oldCurve->points;
	float newt0, newx0, newv0;
	switch (position)
	{
	case -1:
		newt0 = oldPoints->t0_right;
		newx0 = oldPoints->x0_right;
		newv0 = oldPoints->v0_right;
		break;
	case 0:
		newt0 = oldPoints->t0;
		newx0 = oldPoints->x0;
		newv0 = oldPoints->v0;
		break;
	case 1:
		newt0 = oldPoints->t0_left;
		newx0 = oldPoints->x0_left;
		newv0 = oldPoints->v0_left;
		break;
	default:
		std::cerr << "redrawCurve was called with a position that was neither -1, 0, or 1." << std::endl;
		return oldCurve;
	}
	//getPoints(newPoints, oldPoints->t0, oldPoints->x0, oldPoints->v0, oldCurve->scaleX, oldCurve->scaleY, position);
	//Points* newPoints = new Points();
	getPoints(oldCurve->points, oldPoints->t0, oldPoints->x0, oldPoints->v0, oldCurve->scaleX, oldCurve->scaleY, position);
	Curve* newCurve = new Curve(oldCurve->points, oldCurve->scaleX, oldCurve->scaleY);
	newCurve->AttachShaders(oldCurve->vertexShader, oldCurve->fragmentShader);
	newCurve->x = oldCurve->x, newCurve->y = oldCurve->y;

	oldCurve->Delete();
	delete oldCurve;
	return newCurve;
}
*/

Calculator::Calculator(EquationList* equationList, VariableList* variableList)
{
	this->equationList = equationList;
	this->variableList = variableList;
}

Curve* Calculator::redrawCurve(Curve* oldCurve, float t0)
{
	std::cout << "redrawing curve from origin...\n";

	//Variable* functionVariable = variableList->getFunctionVariable(functionName);
	getPointsFromOrigin(oldCurve->points, oldCurve->equation, t0, oldCurve->scaleX, oldCurve->scaleY);

	std::cout << "Old curve's points are " << oldCurve->points << std::endl;
	Curve* newCurve = new Curve(oldCurve->points, oldCurve->scaleX, oldCurve->scaleY);
	newCurve->AttachShaders(oldCurve->vertexShader, oldCurve->fragmentShader);
	newCurve->x = oldCurve->x, newCurve->y = oldCurve->y;
	newCurve->equation = oldCurve->equation;

	oldCurve->Delete();
	delete oldCurve;
	return newCurve;
}

void Calculator::getPointsFromOrigin(Points* points, Equation* equation, float t0, float zoomX, float zoomY)
{
	// t0 is just at 0.0f, get points from middle as usual
	if (t0 > -(CURVE_RADIUS_PER_WIDTH - 1.0f) / zoomX && t0 < (CURVE_RADIUS_PER_WIDTH - 1.0f) / zoomX) // draw in middle
	{
		std::cout << "GETTING FROM MIDDLE\n";
		getPointsFromMiddle(points, equation, 0.0f, zoomX, zoomY);
		return;
	}

	float dt = (2 * CURVE_RADIUS_PER_WIDTH / zoomX) / (CURVE_POINTS_SIZE - 1);

	// here, replace x with x[0], v with x[1], a with x[2] etc
	Variable* functionVariable = variableList->getFunctionVariable(equation->functionName);

	float a;
	functionVariable->initializeDerivativeValues(equation->derivativeOrder);
	float time = 0.0f;
	if (t0 < 0.0f) // Go from the right to the left
	{
		float target = t0 + (CURVE_RADIUS_PER_WIDTH / zoomX);
		while (time > target)
		{
			a = equation->Evaluate(time);

			functionVariable->changeDerivativeValues(equation->derivativeOrder, a, dt, false);

			time -= dt;
		}

		getPointsFromRight(points, equation, time, zoomX, zoomY);
		std::cout << "Got from right, and t0 is now " << points->t0 << std::endl;
		return;
	}
	else // Go from the left to the right
	{
		float target = t0 - (CURVE_RADIUS_PER_WIDTH / zoomX);
		while (time < target)
		{
			a = equation->Evaluate(time);

			functionVariable->changeDerivativeValues(equation->derivativeOrder, a, dt, true);

			time += dt;
		}

		getPointsFromLeft(points, equation, time, zoomX, zoomY);
		std::cout << "Got from left, and t0 is now " << points->t0 << std::endl;
		return;
	}
}

void Calculator::getPointsFromMiddle(Points* points, Equation* equation, float t0, float zoomX, float zoomY)
{
	float dt = (2 * CURVE_RADIUS_PER_WIDTH / zoomX) / (CURVE_POINTS_SIZE - 1);

	std::cout << "Function Name: " << equation->functionName << std::endl;
	Variable* functionVariable = variableList->getFunctionVariable(equation->functionName);

	float a;
	functionVariable->initializeDerivativeValues(equation->derivativeOrder);
	float time = t0;
	for (int i = (CURVE_POINTS_SIZE - 1) / 2; i < CURVE_POINTS_SIZE; i++)
	{
		points->points[i * 6] = time;
		points->points[i * 6 + 1] = functionVariable->derivativeValues[0];
		points->points[i * 6 + 2] = 0.0f;
		points->points[i * 6 + 3] = 1.0f;
		points->points[i * 6 + 4] = 1.0f;
		points->points[i * 6 + 5] = 1.0f;

		if (i + 1 == CURVE_POINTS_SIZE)
		{
			points->t0_right = time;
			break;
		}

		a = equation->Evaluate(time);
		functionVariable->changeDerivativeValues(equation->derivativeOrder, a, dt, true);
		time += dt;
	}
	functionVariable->initializeDerivativeValues(equation->derivativeOrder);
	time = t0;
	for (int i = (CURVE_POINTS_SIZE - 1) / 2; i > 0;)
	{
		i--;
		a = equation->Evaluate(time);
		functionVariable->changeDerivativeValues(equation->derivativeOrder, a, dt, false);
		time -= dt;

		if (i == 0)
		{
			points->t0_left = time;
		}

		points->points[i * 6] = time;
		points->points[i * 6 + 1] = functionVariable->derivativeValues[0];
		points->points[i * 6 + 2] = 0.0f;
		points->points[i * 6 + 3] = 1.0f;
		points->points[i * 6 + 4] = 1.0f;
		points->points[i * 6 + 5] = 1.0f;
	}

	points->t0 = t0;
}

void Calculator::getPointsFromLeft(Points* points, Equation* equation, float t0, float zoomX, float zoomY) // do this when panning to the right
{
	float dt = (2 * CURVE_RADIUS_PER_WIDTH / zoomX) / (CURVE_POINTS_SIZE - 1);

	Variable* functionVariable = variableList->getFunctionVariable(equation->functionName);

	float a;
	functionVariable->initializeDerivativeValues(equation->derivativeOrder);
	float time = t0;
	for (int i = 0; i < CURVE_POINTS_SIZE; i++)
	{
		points->points[i * 6] = time;
		points->points[i * 6 + 1] = functionVariable->derivativeValues[0];
		points->points[i * 6 + 2] = 0.0f;
		points->points[i * 6 + 3] = 1.0f;
		points->points[i * 6 + 4] = 1.0f;
		points->points[i * 6 + 5] = 1.0f;

		if (i == 0)
		{
			points->t0_left = time;
		}
		if (i == (CURVE_POINTS_SIZE - 1) / 2)
		{
			points->t0 = time;
		}
		if (i == CURVE_POINTS_SIZE - 1)
		{
			points->t0_right = time;
			return;
		}

		a = equation->Evaluate(time);
		functionVariable->changeDerivativeValues(equation->derivativeOrder, a, dt, true);
		time += dt;
	}
}

void Calculator::getPointsFromRight(Points* points, Equation* equation, float t0, float zoomX, float zoomY) // do this when panning to the left
{
	float dt = (2 * CURVE_RADIUS_PER_WIDTH / zoomX) / (CURVE_POINTS_SIZE - 1);

	Variable* functionVariable = variableList->getFunctionVariable(equation->functionName);

	float a;
	functionVariable->initializeDerivativeValues(equation->derivativeOrder);
	float time = t0;
	for (int i = CURVE_POINTS_SIZE; i > 0;)
	{
		i--;

		if (i == CURVE_POINTS_SIZE - 1)
		{
			points->t0_right = time;
		}
		if (i == (CURVE_POINTS_SIZE - 1) / 2)
		{
			points->t0 = time;
		}
		if (i == 0)
		{
			points->t0_left = time;
		}

		a = equation->Evaluate(time);
		functionVariable->changeDerivativeValues(equation->derivativeOrder, a, dt, false);
		time -= dt;

		points->points[i * 6] = time;
		points->points[i * 6 + 1] = functionVariable->derivativeValues[0];
		points->points[i * 6 + 2] = 0.0f;
		points->points[i * 6 + 3] = 1.0f;
		points->points[i * 6 + 4] = 1.0f;
		points->points[i * 6 + 5] = 1.0f;
	}
}

/*
void getPoints(Points* points, float t0, float x0, float v0, float zoomX, float zoomY, int position)
{
	switch (position)
	{
		case -1:
			std::cout << "I am drawing the points from the right!!" << std::endl;
			getPointsFromRight(points, t0, x0, v0, zoomX, zoomY);
			return;
		case 0:
			std::cout << "I am drawing the points in middle!!" << std::endl;
			getPointsFromMiddle(points, t0, x0, v0, zoomX, zoomY);
			return;
		case 1:
			std::cout << "I am drawing the points from the left!!" << std::endl;
			getPointsFromLeft(points, t0, x0, v0, zoomX, zoomY);
			return;
		default:
			std::cerr << "getPoints was called with a position that was neither -1, 0 or 1." << std::endl;
			return;
	}
}
*/