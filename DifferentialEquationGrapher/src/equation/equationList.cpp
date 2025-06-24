#include <calculator/equationList.h>

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
		std::cout << name[10] << " -> " << iStr[0] << std::endl;
		name[10] = iStr[0];
		std::cout << name[10] << std::endl;
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
	equationNames.erase(std::next(std::begin(equationNames), id));
}

Equation* EquationList::GetEquation(int id)
{
	if(id < 0 || id >= equations.size()) return nullptr;

	return equations[id];
}

char* EquationList::GetEquationName(int id)
{
	if(id < 0 || id >= equationNames.size()) return nullptr;

	return equationNames[id];
}

int EquationList::EquationCount()
{
	return equations.size();
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