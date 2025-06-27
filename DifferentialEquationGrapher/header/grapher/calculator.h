#ifndef CALCULATOR_CLASS_H
#define CALCULATOR_CLASS_H

#include "grapher/curve.h"
#include "grapher/points.h"

#include "calculator/calculatorDefs.h"


class Calculator
{
	public:
		Calculator(EquationList* equationList, VariableList* variableList);
		Curve* redrawCurve(Curve* oldCurve, float t0);

		void getPointsFromOrigin(Points* points, Equation* equation, float t0, float zoomX, float zoomY);
		void getPointsFromMiddle(Points* points, Equation* equation, float t0, float zoomX, float zoomY);
		void getPointsFromLeft(Points* points, Equation* equation, float t0, float zoomX, float zoomY);
		void getPointsFromRight(Points* points, Equation* equation, float t0, float zoomX, float zoomY);

	private:
		EquationList* equationList;
		VariableList* variableList;
};

//float func1(float x, float v, float time);
//Curve* redrawCurve(Curve* oldCurve, int position);
//Curve* redrawCurve(Curve* oldCurve, float t0, float x0, float v0);
//void getPoints(Points* points, float t0, float x0, float v0, float zoomX, float zoomY, int position);
//void getPointsFromLeft(Points* points, float t0, float x0, float v0, float zoomX, float zoomY);
//void getPointsFromRight(Points* points, float t0, float x0, float v0, float zoomX, float zoomY);
//void getPointsFromMiddle(Points* points, float t0, float x0, float v0, float zoomX, float zoomY);
//void getPointsFromOrigin(Points* points, float t0, float x0, float v0, float zoomX, float zoomY);

#endif