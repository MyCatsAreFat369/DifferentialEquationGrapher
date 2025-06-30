#include <grapher/calculator.h>

#include "calculator/equation.h"
#include "calculator/variable.h"
#include "calculator/equationList.h"
#include "calculator/variableList.h"

#include "grapher/graphManager.h"

Calculator::Calculator(EquationList* equationList, VariableList* variableList)
{
	this->equationList = equationList;
	this->variableList = variableList;
}

void Calculator::redrawCurves(float t0, float zoomX, float zoomY, float scaleRatio)
{
	getPointsFromOrigin(t0, zoomX, zoomY, scaleRatio);
	
	for (int i = 0; i < equationList->EquationCount(); i++)
	{
		Equation* equation = equationList->GetEquation(i);
		if(!equation->isValidEquation()) continue;

		Curve* curve = equation->curve;
		
		curve->Flush();
		curve->Generate();
	}
}

void Calculator::getPointsFromOrigin(float t0, float zoomX, float zoomY, float scaleRatio)
{
	// t0 is just at 0.0f, get points from middle as usual
	if (t0 > -(CURVE_RADIUS_PER_WIDTH - scaleRatio) / zoomX && t0 < (CURVE_RADIUS_PER_WIDTH - scaleRatio) / zoomX) // draw in middle
	{
		equationList->initializeFunctionVariables(variableList);
		getPointsFromMiddle(0.0f, zoomX, zoomY);
		return;
	}

	float dt = (2.0f * CURVE_RADIUS_PER_WIDTH / zoomX) / (CURVE_POINTS_SIZE - 1);

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
		return;
	}
}

void Calculator::getPointsFromMiddle(float t0, float zoomX, float zoomY)
{
	float dt = (2 * CURVE_RADIUS_PER_WIDTH / zoomX) / (CURVE_POINTS_SIZE - 1);

	equationList->copyVariablesToCache(variableList);

	float a;
	float time = t0;
	for (int i = (CURVE_POINTS_SIZE - 1) / 2; i < CURVE_POINTS_SIZE; i++)
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
	equationList->copyCacheToVariables(variableList);
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