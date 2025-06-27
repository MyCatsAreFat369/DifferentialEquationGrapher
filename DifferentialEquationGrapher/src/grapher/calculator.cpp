#include <grapher/calculator.h>

#include "calculator/equation.h"
#include "calculator/variable.h"
#include "calculator/equationList.h"
#include "calculator/variableList.h"

#include "grapher/graphManager.h"

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

void Calculator::redrawCurves(float t0, float zoomX, float zoomY)
{
	//std::cout << "redrawing curve from origin...\n";

	getPointsFromOrigin(t0, zoomX, zoomY);
	
	for (int i = 0; i < equationList->EquationCount(); i++)
	{
		Equation* equation = equationList->GetEquation(i);
		if(!equation->isValidEquation()) continue;

		Curve* curve = equation->curve;
		/*
		if (i == 1)
		{
			Points* points = curve->points;
			for (int k = 0; k < CURVE_POINTS_SIZE; k++)
			{
				std::cout << points->points[k * 6] << ", " << points->points[k * 6 + 1] << ", " << points->points[k * 6 + 2] << ", "
					<< points->points[k * 6 + 3] << ", " << points->points[k * 6 + 4] << ", " << points->points[k * 6 + 5] << "\n";
			}
		}
		*/
		
		curve->Flush();
		curve->Generate();
	}
}

void Calculator::getPointsFromOrigin(float t0, float zoomX, float zoomY)
{
	// t0 is just at 0.0f, get points from middle as usual
	if (t0 > -(CURVE_RADIUS_PER_WIDTH - 1.0f) / zoomX && t0 < (CURVE_RADIUS_PER_WIDTH - 1.0f) / zoomX) // draw in middle
	{
		//std::cout << "GETTING FROM MIDDLE\n";
		getPointsFromMiddle(0.0f, zoomX, zoomY);
		return;
	}

	float dt = (2 * CURVE_RADIUS_PER_WIDTH / zoomX) / (CURVE_POINTS_SIZE - 1);

	// here, replace x with x[0], v with x[1], a with x[2] etc

	equationList->initializeFunctionVariables(variableList);
	
	float a;
	float time = 0.0f;
	if (t0 < 0.0f) // Go from the right to the left
	{
		float target = t0 + (CURVE_RADIUS_PER_WIDTH / zoomX);
		while (time > target)
		{
			equationList->copyVariablesToTemp(variableList);
			for (int i = 0; i < equationList->EquationCount(); i++)
			{
				Equation* equation = equationList->GetEquation(i);
				if(!equation->isValidEquation()) continue;

				Variable* functionVariable = variableList->getFunctionVariable(equation->functionName);

				a = equation->Evaluate(time);
				functionVariable->changeDerivativeValues(equation->derivativeOrder, a, dt, false);
			}
			equationList->copyTempToVariables(variableList);

			time -= dt;
		}

		getPointsFromRight(time, zoomX, zoomY);
		//std::cout << "Got from right, and t0 is now " << points->t0 << std::endl;
		return;
	}
	else // Go from the left to the right
	{
		float target = t0 - (CURVE_RADIUS_PER_WIDTH / zoomX);
		while (time < target)
		{
			equationList->copyVariablesToTemp(variableList);
			for (int i = 0; i < equationList->EquationCount(); i++)
			{
				Equation* equation = equationList->GetEquation(i);
				if(!equation->isValidEquation()) continue;

				Variable* functionVariable = variableList->getFunctionVariable(equation->functionName);

				a = equation->Evaluate(time);
				functionVariable->changeDerivativeValues(equation->derivativeOrder, a, dt, true);
			}
			equationList->copyTempToVariables(variableList);

			time += dt;
		}

		getPointsFromLeft(time, zoomX, zoomY);
		//std::cout << "Got from left, and t0 is now " << points->t0 << std::endl;
		return;
	}
}

void Calculator::getPointsFromMiddle(float t0, float zoomX, float zoomY)
{
	float dt = (2 * CURVE_RADIUS_PER_WIDTH / zoomX) / (CURVE_POINTS_SIZE - 1);

	//std::cout << "Function Name: " << equation->functionName << std::endl;
	//std::cout << "I'm not broken.\n";
	//Variable* functionVariable = variableList->getFunctionVariable(equation->functionName);

	equationList->initializeFunctionVariables(variableList);

	float a;
	//functionVariable->initializeDerivativeValues(equation->derivativeOrder);
	float time = t0;
	for (int i = (CURVE_POINTS_SIZE - 1) / 2; i < CURVE_POINTS_SIZE; i++)
	{
		equationList->copyVariablesToTemp(variableList);
		for (int j = 0; j < equationList->EquationCount(); j++)
		{
			Equation* equation = equationList->GetEquation(j);
			if(!equation->isValidEquation()) continue;

			Points* points = equation->curve->points;
			//std::cout << variableList << " and " << equation << std::endl;
			Variable* functionVariable = variableList->getFunctionVariable(equation->functionName);

			points->points[i * 6] = time;
			points->points[i * 6 + 1] = functionVariable->derivativeValuesTemp[0];
			points->points[i * 6 + 2] = 0.0f;
			points->points[i * 6 + 3] = 1.0f;
			points->points[i * 6 + 4] = 1.0f;
			points->points[i * 6 + 5] = 1.0f;

			if (i == (CURVE_POINTS_SIZE - 1) / 2)
			{
				points->t0 = time;
			}
			if (i == CURVE_POINTS_SIZE - 1)
			{
				points->t0_right = time;
				continue;
			}

			a = equation->Evaluate(time);
			functionVariable->changeDerivativeValues(equation->derivativeOrder, a, dt, true);
		}
		equationList->copyTempToVariables(variableList);

		time += dt;
	}
	equationList->initializeFunctionVariables(variableList);
	time = t0;
	for (int i = (CURVE_POINTS_SIZE - 1) / 2; i > 0;)
	{
		i--;
		time -= dt;
		equationList->copyVariablesToTemp(variableList);
		for (int j = 0; j < equationList->EquationCount(); j++)
		{
			Equation* equation = equationList->GetEquation(j);
			if(!equation->isValidEquation()) continue;

			Points* points = equation->curve->points;
			Variable* functionVariable = variableList->getFunctionVariable(equation->functionName);

			a = equation->Evaluate(time);
			functionVariable->changeDerivativeValues(equation->derivativeOrder, a, dt, false);

			if (i == 0)
			{
				points->t0_left = time;
			}

			points->points[i * 6] = time;
			points->points[i * 6 + 1] = functionVariable->derivativeValuesTemp[0];
			points->points[i * 6 + 2] = 0.0f;
			points->points[i * 6 + 3] = 1.0f;
			points->points[i * 6 + 4] = 1.0f;
			points->points[i * 6 + 5] = 1.0f;
		}
		equationList->copyTempToVariables(variableList);
	}
}

void Calculator::getPointsFromLeft(float t0, float zoomX, float zoomY) // do this when panning to the right
{
	float dt = (2 * CURVE_RADIUS_PER_WIDTH / zoomX) / (CURVE_POINTS_SIZE - 1);

	equationList->initializeFunctionVariables(variableList);

	float a;
	float time = t0;
	for (int i = 0; i < CURVE_POINTS_SIZE; i++)
	{
		equationList->copyVariablesToTemp(variableList);
		for (int j = 0; j < equationList->EquationCount(); j++)
		{
			Equation* equation = equationList->GetEquation(j);
			if(!equation->isValidEquation()) continue;

			Points* points = equation->curve->points;
			Variable* functionVariable = variableList->getFunctionVariable(equation->functionName);

			points->points[i * 6] = time;
			points->points[i * 6 + 1] = functionVariable->derivativeValuesTemp[0];
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
				continue;
			}

			a = equation->Evaluate(time);
			functionVariable->changeDerivativeValues(equation->derivativeOrder, a, dt, true);
		}
		equationList->copyTempToVariables(variableList);
		
		time += dt;
	}
}

void Calculator::getPointsFromRight(float t0, float zoomX, float zoomY) // do this when panning to the left
{
	float dt = (2 * CURVE_RADIUS_PER_WIDTH / zoomX) / (CURVE_POINTS_SIZE - 1);

	equationList->initializeFunctionVariables(variableList);

	float a;
	float time = t0;
	for (int i = CURVE_POINTS_SIZE; i > 0;)
	{
		i--;
		time -= dt;

		equationList->copyVariablesToTemp(variableList);
		for (int j = 0; j < equationList->EquationCount(); j++)
		{
			Equation* equation = equationList->GetEquation(j);
			if(!equation->isValidEquation()) continue;

			Points* points = equation->curve->points;
			Variable* functionVariable = variableList->getFunctionVariable(equation->functionName);

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
		
			points->points[i * 6] = time;
			points->points[i * 6 + 1] = functionVariable->derivativeValuesTemp[0];
			points->points[i * 6 + 2] = 0.0f;
			points->points[i * 6 + 3] = 1.0f;
			points->points[i * 6 + 4] = 1.0f;
			points->points[i * 6 + 5] = 1.0f;
		}
		equationList->copyTempToVariables(variableList);
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