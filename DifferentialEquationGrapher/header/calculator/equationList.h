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
		Equation* GetEquation(std::string functionName);
		char* GetEquationName(int id);

		int EquationCount();

		bool variableNameExistsAsDifferentialEquation(std::string name);
		bool variableNameExistsAsNormalEquation(std::string name);

		void initializeFunctionVariables(VariableList* variableList);
		void copyVariablesToTemp(VariableList* variableList);
		void copyTempToVariables(VariableList* variableList);
		void copyVariablesToCache(VariableList* variableList);
		void copyCacheToVariables(VariableList* variableList);

		void Delete();
	private:
		std::vector<Equation*> equations;
		std::vector<char*> equationNames;
};


#endif