#ifndef EQUATIONS_CLASS_H
#define EQUATIONS_CLASS_H

#include <string>
#include <unordered_map>

#include "calculator/variableList.h"

const int EQUATION_MAX_LENGTH = 4096;

class Equation
{
	public:
		Equation();
		void SetFormula(char* formula);
		void SetFormula(std::string formula);
		void Compile(VariableList* variableList);
		float Compute(VariableList* variableList);

		void Delete();

		std::string formula = "";
		char* formulaChar = new char[4096];

		enum EquationType
		{
			FIRST_ORDER,
			SECOND_ORDER,
			MULTI_ORDER
		};

		EquationType equationType = FIRST_ORDER;

		std::vector<std::string> tokens;
		std::vector<char*> tokensChar;
	private:
		void Tokenize();
		void Pemdas();
		void Postfix();
		
		void encloseInParenthesis(int i, bool ignoreNextToken);
		void encloseSinudosialInParenthesis(int i);

		void generateFormulaChar();
};


#endif