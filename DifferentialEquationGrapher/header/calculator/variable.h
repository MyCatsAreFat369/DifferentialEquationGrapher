#ifndef VARIABLE_CLASS_H
#define VARIABLE_CLASS_H

#include <string>

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

		VariableType variableType;
		char* nameChar;
		char* nameCache;

		float value;
};


#endif