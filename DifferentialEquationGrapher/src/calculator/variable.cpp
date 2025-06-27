#include <calculator/variable.h>


char* generateNameChar(std::string nameStr);


Variable::Variable()
{

}

Variable::Variable(VariableType variableType, std::string name)
{
	this->variableType = variableType;

	this->nameChar = generateNameChar(name);
	this->nameCache = generateNameChar(name);

	this->value = 0.0f;

	/*
	if (variableType == FUNCTION_VARIABLE)
	{
		for (int i = 0; i < 10; i++)
		{

		}
	}
	*/
	
}

void Variable::renameVariable(std::string newName)
{
	this->nameChar = generateNameChar(newName);
	this->nameCache = generateNameChar(newName);
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