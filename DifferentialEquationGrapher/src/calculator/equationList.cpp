#include <calculator/equationList.h>

#include <string>

#include "calculator/equation.h"

const std::string intDigits = "1234567890";

EquationList::EquationList()
{

}

void EquationList::AddEquation(Equation* equation)
{
	// Doesn't allow for more than 99 equations
	if(equations.size() >= 99) return;

	char* name = _strdup("Equation 00");
	std::string iStr = std::to_string(equations.size() + 1);
	if (iStr.length() == 1)
	{
		name[9] = iStr[0];
		name[10] = ' ';
	}
	else if (iStr.length() == 2)
	{
		name[9] = iStr[0];
		name[10] = iStr[1];
	}
	else return; // Returns if something goes wrong

	equations.push_back(equation);
	equationNames.push_back(name);
	std::cout << name << std::endl;
}

void EquationList::RemoveEquation(int id)
{
	// Returns if id is not within boundaries of equation list
	if(id < 0 || id >= equations.size()) return;

	equations[id]->Delete();
	delete equations[id];

	equations.erase(std::next(std::begin(equations), id));
	equationNames.pop_back();
	//equationNames.erase(std::next(std::begin(equationNames), id));

	for (int i = 0; i < equations.size(); i++)
	{
		std::cout << equationNames[i] << ": " << equations[i]->formula << std::endl;
	}
}

Equation* EquationList::GetEquation(int id)
{
	if(id < 0 || id >= equations.size()) return equations[0];

	return equations[id];
}

char* EquationList::GetEquationName(int id)
{
	if(id < 0 || id >= equationNames.size()) return equationNames[0];

	return equationNames[id];
}

int EquationList::EquationCount()
{
	return equations.size();
}

bool EquationList::variableNameExistsAsFunction(std::string name)
{
	// Return if it's in derivative form (y1, f5, etc)
	if(intDigits.find(name[name.length() - 1]) != std::string::npos) return true;

	// Check if function variable
	for (int i = 0; i < EquationCount(); i++)
	{
		Equation* equation = GetEquation(i);
		if(equation->functionName == name) return true;
	}

	return false;
}

void EquationList::Delete()
{
	for (int i = 0; i < equations.size(); i++)
	{
		equations[i]->Delete();
		delete equations[i];
	}
	equations.clear();
}