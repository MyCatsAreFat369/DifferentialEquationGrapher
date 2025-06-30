#ifndef EQUATIONS_CLASS_H
#define EQUATIONS_CLASS_H

#include <string>
#include <unordered_map>

#include "calculator/calculatorDefs.h"
#include "grapher/curve.h"

const int EQUATION_MAX_LENGTH = 4096;

class Token
{
	public:
		enum TokenType
		{
			CONSTANT,
			VARIABLE,
			FUNCTION,
			OPERATOR
		};

		Token(TokenType tokenType, std::string tokenStr, float value = 0.0f)
		{
			this->tokenType = tokenType;
			this->tokenStr = tokenStr;
			this->value = value;
		}

		TokenType tokenType;
		std::string tokenStr;
		float value; // if CONSTANT
		std::string functionName; // if FUNCTION
		int derivativeNumber; // if FUNCTION
};

class Equation
{
	public:
		Equation(EquationList* equationList, VariableList* variableList);
		void SetFormula(char* formula);
		void SetFormula(std::string formula);
		
		void setFunctionName(char* functionName);
		void setFunctionName(std::string functionName);

		void generateCharFromString(std::string str, char* charArray, int size);

		char* getEquationLeftSide();

		void replaceVariableInEquation(std::string oldVariable, std::string newVariable);

		void InitializeCurve(GLuint vertexShader, GLuint fragmentShader, float x, float y, float zoomX, float zoomY);

		void Compile();
		float Evaluate(float time);

		bool isValidEquation();

		void Delete(); 

		std::string formula = "";
		char* formulaChar = new char[4096];

		std::string functionName = "";
		char* functionNameChar = new char[4];

		int derivativeOrder = 1;
		char* derivativeOrderChar = new char[6];

		Curve* curve;
		bool drawCurve = true;
		float color[3] = {1.0f, 1.0f, 1.0f};

		enum EquationType
		{
			FIRST_ORDER,
			SECOND_ORDER,
			MULTI_ORDER
		};

		EquationType equationType = FIRST_ORDER;

		std::vector<std::string> tokens;
		std::vector<std::string> stack;
		std::vector<std::string> preQueue;
		std::vector<Token> queue;
		std::vector<float> evalStack;

		std::vector<char*> tokensChar;
		std::vector<char*> queueChar;
		char* resultChar;
	private:
		void Tokenize();
		void Pemdas();
		void Postfix();
		
		void encloseInParenthesis(int i, bool ignoreNextToken);
		void encloseFunctionInParenthesis(int i);

		EquationList* equationList;
		VariableList* variableList;
		char equationLeftSide[8];
};

#endif