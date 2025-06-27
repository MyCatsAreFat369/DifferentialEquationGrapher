#include <calculator/variableList.h>

#include "calculator/equation.h"
#include "calculator/variable.h"
#include "calculator/equationList.h"

const std::string intDigits = "1234567890";
const int CONSTANTS_COUNT = 2;
const std::string constants[] = {"e", "pi"};

VariableList::VariableList(EquationList* equationList)
{
	this->equationList = equationList;
}

bool VariableList::addVariableIfNotExists(std::string name)
{
	// Check if the variable is inside of there before thinking about creating it
	if (variableList.find(name) != variableList.end()) return true;

	if(name.size() > VARIABLE_MAX_NAME_LENGTH) return false;

	if(name == "t") return false;

	// Returns if it's a constant type (e, pi)
	for (int i = 0; i < CONSTANTS_COUNT; i++)
	{
		if(name == constants[i]) return false;
	}

	// Return if it's a function variable already
	if(functionVariableList.find(name) != functionVariableList.end()) return false;

	// Add it
	variableOrder.push_back(name);
	variableList[name] = new Variable(CONSTANT_VARIABLE, name);

	std::cout << "WHY AM I ADDING!\n";
	return true;
}

bool VariableList::addFunctionVariableIfNotExists(std::string name)
{
	// Check if the function variable is inside of there before thinking about creating it
	if(functionVariableList.find(name) != functionVariableList.end()) return true;


	if(name.size() <= 0 || name.size() > VARIABLE_MAX_NAME_LENGTH) return false;

	// Returns if it's a constant type (e, pi)
	for (int i = 0; i < CONSTANTS_COUNT; i++)
	{
		if(name == constants[i]) return false;
	}

	// Add it
	functionVariableList[name] = new Variable(FUNCTION_VARIABLE, name);

	return true;
}

void VariableList::setVariable(std::string name, float value)
{
	if(!addVariableIfNotExists(name)) return;

	variableList[name]->value = value;
}

void VariableList::changeVariableBy(std::string name, float value)
{
	if(!addVariableIfNotExists(name)) return;

	variableList[name]->value += value;
}




Variable* VariableList::getVariable(std::string name)
{
	if(!addVariableIfNotExists(name)) return variableList[variableOrder[0]];

	return variableList[name];
}

Variable* VariableList::getFunctionVariable(std::string name)
{
	if(functionVariableList.find(name) == functionVariableList.end()) return functionVariableList[equationList->GetEquation(0)->functionName];

	return functionVariableList[name];
}

std::string VariableList::getVariableNameStr(int id)
{
	if(id < 0 || id >= VariableCount()) return "ERROR";
	return variableOrder[id];
}





int VariableList::updateVariableName(int id, char* newName)
{
	if(id < 0 || id >= VariableCount()) return -1;

	if (variableList.find(newName) != variableList.end() ||
		functionVariableList.find(newName) != functionVariableList.end()) return 1;

	Variable* variable = variableList[variableOrder[id]];

	variableList.erase(variableOrder[id]);

	std::string newNameStr = newName;
	variableOrder[id] = newName;
	variableList[newNameStr] = variable;
	variableList[newNameStr]->nameChar = generateNameChar(newName);

	return 0;
}

void VariableList::removeVariable(std::string name)
{
	if(variableList.find(name) == variableList.end()) return;

	std::cout << "I am gonna remove " << name << std::endl;
	Variable* variable = variableList[name];
	variableList.erase(name);
	delete variable;

	for (int i = 0; i < variableOrder.size(); i++)
	{
		if(variableOrder[i] != name) continue;

		variableOrder.erase(std::next(std::begin(variableOrder), i));
		return;
	}
}


void VariableList::renameFunctionVariable(std::string oldName, std::string newName)
{
	if (functionVariableList.find(oldName) == functionVariableList.end())
	{
		functionVariableList[newName] = new Variable(FUNCTION_VARIABLE, newName);
		return;
	}

	Variable* variable = functionVariableList[oldName];
	variable->renameVariable(newName);
	functionVariableList.erase(oldName);
	functionVariableList[newName] = variable;
}





int VariableList::VariableCount()
{
	return variableOrder.size();
}

void VariableList::Delete()
{
	/*
	for (int i = 0; i < variableOrder.size(); i++)
	{
		Variable* variable = variableList[variableOrder[i]];
		delete variable;
	}

	variableOrder.clear();
	variableList.clear();

	std::unordered_map<std::string, Variable*>::iterator it;

	for (it = functionVariableList.begin(); it != functionVariableList.end(); it++)
	{
		Variable* variable = it->second;
		delete variable;
	}

	functionVariableList.clear();
	*/
}