#include <calculator/equation.h>

#include <iostream>

Equation::Equation()
{

}

void Equation::SetFormula(char* formula)
{
	this->formula = formula;
	generateFormulaChar();
}

void Equation::SetFormula(std::string formula)
{
	this->formula = formula;
	generateFormulaChar();
}


// Consider passing in the variable list 
void Equation::Compile(VariableList* variableList)
{

}


float Equation::Compute(VariableList* variableList)
{

}

void Equation::Delete()
{
	delete[] formulaChar;
}

void Equation::generateFormulaChar()
{
	for (int i = 0; i < 4096; i++)
	{
		if(i >= formula.length()) formulaChar[i] = '\0';
		else formulaChar[i] = formula[i];
	}
}