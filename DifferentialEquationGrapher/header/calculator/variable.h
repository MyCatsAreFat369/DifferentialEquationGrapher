#ifndef VARIABLE_CLASS_H
#define VARIABLE_CLASS_H

#include <string>

#include "calculator/calculatorDefs.h"

const int VARIABLE_MAX_NAME_LENGTH = 4;

enum VariableType
{
	CONSTANT_VARIABLE,
	FUNCTION_VARIABLE
};

char* generateNameChar(std::string nameStr);

class Variable
{
	public:
		Variable();
		Variable(VariableType variableType, std::string name);

		void renameVariable(std::string newName);

		VariableType variableType;
		char* nameChar;
		char* nameCache;

		// For constant variables
		float value;

		// For function variables, a set of values for all derivatives from 0-9
		float derivativeValues[10];
		float initialValues[10];

		void initializeDerivativeValues(int derivativeOrder)
		{
			for (int i = 0; i < derivativeOrder; i++)
			{
				derivativeValues[i] = initialValues[i];
			}
		}
		void changeDerivativeValues(int derivativeOrder, float a, float dt, bool direction)
		{
			if(direction) derivativeValues[derivativeOrder - 1] += a * dt;
			else derivativeValues[derivativeOrder - 1] -= a * dt;
			for (int i = derivativeOrder - 1; i > 0;)
			{
				i--;
				if(direction) derivativeValues[i] += direction * derivativeValues[i + 1] * dt;
				else derivativeValues[i] -= direction * derivativeValues[i + 1] * dt;
			}
		}
};


#endif