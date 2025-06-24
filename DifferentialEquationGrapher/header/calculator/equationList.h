#ifndef EQUATION_LIST_CLASS_H
#define EQUATION_LIST_CLASS_H

#include <iostream>
#include "calculator/equation.h"

class EquationList
{
	public:
		EquationList();

		void AddEquation(Equation* equation);
		void RemoveEquation(int id);
		Equation* GetEquation(int id);
		char* GetEquationName(int id);

		int EquationCount();

		void Delete();
	private:
		std::vector<Equation*> equations;
		std::vector<char*> equationNames;
};


#endif