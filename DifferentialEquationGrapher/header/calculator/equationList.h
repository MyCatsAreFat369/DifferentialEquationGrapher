#ifndef EQUATION_LIST_CLASS_H
#define EQUATION_LIST_CLASS_H

#include <iostream>

#include <vector>

#include "calculator/calculatorDefs.h"

class EquationList
{
	public:
		EquationList();

		void AddEquation(Equation* equation);
		void RemoveEquation(int id);
		Equation* GetEquation(int id);
		char* GetEquationName(int id);

		int EquationCount();

		bool variableNameExistsAsFunction(std::string name);

		void initializeFunctionVariables(VariableList* variableList);
		void copyVariablesToTemp(VariableList* variableList);
		void copyTempToVariables(VariableList* variableList);

		void Delete();
	private:
		std::vector<Equation*> equations;
		std::vector<char*> equationNames;
};


#endif