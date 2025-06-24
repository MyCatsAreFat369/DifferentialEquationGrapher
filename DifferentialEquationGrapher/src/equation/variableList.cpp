#include <calculator/variableList.h>


VariableList::VariableList()
{

}

void VariableList::setVariable(std::string name, float value)
{
	variableList[name] = value;
}

void VariableList::changeVariableBy(std::string name, float value)
{
	if(variableList.find(name) == variableList.end()) variableList[name] = 0.0f;

	variableList[name] += value;
}

float VariableList::getVariable(std::string name)
{
	if(variableList.find(name) == variableList.end()) variableList[name] = 0.0f;

	return variableList[name];
}