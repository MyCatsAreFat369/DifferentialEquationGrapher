#include <calculator/variable.h>


Variable::Variable()
{

}

Variable::Variable(VariableType variableType, std::string name)
{
	this->variableType = variableType;

	this->nameChar = generateNameChar(name);
	this->nameCache = generateNameChar(name);

	this->value = 0.0f;
}


char* generateNameChar(std::string nameStr)
{
	char* name = new char[VARIABLE_MAX_NAME_LENGTH];
	for (int i = 0; i < VARIABLE_MAX_NAME_LENGTH; i++)
	{
		if(i < nameStr.length()) name[i] = nameStr[i];
		else name[i] = '\0';
	}
	return name;
}