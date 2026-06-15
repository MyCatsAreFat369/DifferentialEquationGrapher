#include <calculator/equation.h>

#include <iostream>

#include "calculator/variable.h"
#include "calculator/variableList.h"
#include "calculator/equationList.h"

#define _USE_MATH_DEFINES
#include "math.h"

const float EulerConstant = std::exp(1.0);

const std::string intDigits = "1234567890";
const std::string floatDigits = "1234567890.";
const std::string variableRequirementStr = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz_";
const std::string basicOperatorStr = "()+-/*^";
const int OPERATORS_COUNT = 5;
const std::string _operators[] = {
	"+", "-", "*", "/", "^"
};
const int PREDEFINED_FUNCTION_COUNT = 13;
const std::string predefined_functions[] = {
	"sin", "asin", "cos", "acos",
	"tan", "atan", "sinh", "asinh",
	"cosh", "acosh", "tanh", "atanh",
	"ln"
};

Equation::Equation(EquationList* equationList, VariableList* variableList)
{
	this->formula = "";
	generateCharFromString(this->formula, this->formulaChar, EQUATION_MAX_LENGTH);

	this->functionName = "";
	generateCharFromString(this->functionName, this->functionNameChar, VARIABLE_MAX_NAME_LENGTH);

	this->derivativeOrder = 1;

	this->equationList = equationList;
	this->variableList = variableList;
}

void Equation::SetFormula(char* formula)
{
	this->formula = formula;
	generateCharFromString(this->formula, this->formulaChar, EQUATION_MAX_LENGTH);
}

void Equation::SetFormula(std::string formula)
{
	this->formula = formula;
	generateCharFromString(this->formula, this->formulaChar, EQUATION_MAX_LENGTH);
}

void Equation::setFunctionName(char* functionName)
{
	this->functionName = functionName;
	generateCharFromString(this->functionName, this->functionNameChar, VARIABLE_MAX_NAME_LENGTH);
}

void Equation::setFunctionName(std::string functionName)
{
	this->functionName = functionName;
	generateCharFromString(this->functionName, this->functionNameChar, VARIABLE_MAX_NAME_LENGTH);
}

void Equation::setEquationType(int typeID)
{
	switch (typeID)
	{
		case 0:
			equationType = Equation::FIRST_ORDER;
			break;
		case 1:
			equationType = Equation::SECOND_ORDER;
			break;
		case 2:
			equationType = Equation::MULTI_ORDER;
			break;
		case 3:
			equationType = Equation::NORMAL_EQUATION;
			break;
		default:
			equationType = Equation::FIRST_ORDER;
			break;
	}
}

char* Equation::getEquationLeftSide()
{
	/*
	int j;
	for (j = 0; j < functionName.length(); j++)
	{
		equationLeftSide[j] = functionName[j];
	}
	std::string remainingText = "";
	switch (derivativeOrder)
	{
		case 0:
			remainingText = "(t) ="
		case 1:
			remainingText = "' =";
			break;
		case 2:
			remainingText = "'' =";
			break;
		default:
			remainingText = std::to_string(derivativeOrder) + " =";
			break;
	}
	for (int k = 0; k < remainingText.length(); k++)
	{
		if(j >= 8) break; // safety
		equationLeftSide[j] = remainingText[k];
		j++;
	}
	while(j < 8)
	{
		equationLeftSide[j] = '\0';
		j++;
	}
	return equationLeftSide;
	*/
	std::string equationLeftSide;
	equationLeftSide += functionName;
	switch (derivativeOrder)
	{
		case 0:
			equationLeftSide += "(t) = ";
			break;
		case 1:
			equationLeftSide += "' = ";
			break;
		case 2:
			equationLeftSide += "'' = ";
			break;
		default:
			equationLeftSide += std::to_string(derivativeOrder) + " = ";
			break;
	}
	char buf[16];

	strcpy_s(buf, equationLeftSide.c_str());
	return buf;
}


void Equation::replaceVariableInEquation(std::string oldVariable, std::string newVariable)
{
	std::string newFormula;
	std::string tempVariable;
	for (int i = 0; i < formula.size(); i++)
	{
		char letter = formula[i];

		// Check if it's a letter
		if (variableRequirementStr.find(letter) != std::string::npos)
		{
			tempVariable += letter;

			// Check if end of the loop
			if (i + 1 <= formula.size() - 1) continue;

			// This is the end of the loop, do the code below now
			if (tempVariable == oldVariable) newFormula += newVariable;
			else newFormula += tempVariable;
			break;
		}

		// Not a letter: check tempVariable if it's not empty
		if (tempVariable.empty())
		{
			// There's no tempVariable, let's add this number/special character back to our equation
			newFormula += letter;
			continue;
		}

		// Not a letter and tempVariable is defined: let's see if it's our old variable
		if (tempVariable == oldVariable)
		{
			newFormula += newVariable;
			tempVariable = "";
			newFormula += letter;
			continue;
		}

		// Not a letter but tempVariable is not our guy, so let's add it back to the formula
		newFormula += tempVariable;
		tempVariable = "";
		newFormula += letter;
	}

	this->formula = newFormula;
	generateCharFromString(newFormula, this->formulaChar, EQUATION_MAX_LENGTH);
}


void Equation::InitializeCurve(Shader* shader, float x, float y, float zoomX, float zoomY)
{
	Points* points = new Points();
	curve = new Curve(points, x, y, zoomX, zoomY);
	curve->Generate();
	curve->AttachShaders(shader);

	std::cout << "Equation " << formula << " has a curve that is " << curve << "\n";
}

// Consider passing in the variable list 
void Equation::Compile()
{
	invalidEquation = false;

	tokens.clear();
	tokensChar.clear();
	queueChar.clear();

	if(formula.empty()) return;

	std::cout << "The formula is " << formula << std::endl;

	Tokenize();
	Pemdas();
	Postfix();

	// Get token characters
	for (int i = 0; i < tokens.size(); i++)
	{
		std::cout << tokens[i] << "\n";
		char* tokenChar = new char [tokens[i].length() + 1];
		strcpy_s(tokenChar, (tokens[i].length() + 1) * sizeof(char), tokens[i].c_str());
		tokensChar.push_back(tokenChar);
	}

	// Get preQueue characters
	for (int i = 0; i < preQueue.size(); i++)
	{
		char* tokenChar = new char [preQueue[i].length() + 1];
		strcpy_s(tokenChar, (preQueue[i].length() + 1) * sizeof(char), preQueue[i].c_str());
		queueChar.push_back(tokenChar);
	}
	
}

void Equation::ErrorCheck()
{
	//tmpDependencies.clear();
	if (equationType == NORMAL_EQUATION)
	{
		if (!CheckForCircularDependency(std::vector<std::string>()))
		{
			invalidEquation = true;
		}
	}

	// Try evaluating at the end. If evalStack.size() is not equal to 1, then you have an error.
}

bool Equation::CheckForCircularDependency(std::vector<std::string> dependenciesParam)
{
	std::cout << "checking " << functionName << "\n"; // DEBUG
	if(std::find(dependenciesParam.begin(), dependenciesParam.end(), functionName) != dependenciesParam.end()) 
	{
		std::cout << "Found fault at " << functionName << "\n";
		return false;
	}
	dependenciesParam.push_back(functionName);
	// DEBUG
	for (int i = 0; i < dependencies.size(); i++)
	{
		std::cout << dependencies[i];
		if(i + 1 < dependencies.size()) std::cout << ", ";
	}
	std::cout << "\n";

	for (int i = 0; i < dependencies.size(); i++)
	{
		Equation* equation = equationList->GetEquation(dependencies[i]);
		if (equation == nullptr)
		{
			std::cout << "equation " << dependencies[i] << " is nullptr, skipping...\n"; // DEBUG
			continue;
		}

		if(!equation->CheckForCircularDependency(dependenciesParam)) return false;
	}
	std::cout << functionName << " passed the test.\n";
	return true;
}

float Equation::Evaluate(float time)
{
	evalStack.clear();

	for (int i = 0; i < queue.size(); i++)
	{
		//if(time == 0) std::cout << "Queue[i] is " << &queue[i] << std::endl;
		Token token = queue[i];
		
		//if(time == 0) std::cout << "Token is " << token.tokenStr << std::endl;

		if (token.tokenType == Token::OPERATOR)
		{
			// Gets two previous stack elements
			float stack1 = 0.0f, stack2 = 0.0f;
			if(evalStack.size() >= 1) stack1 = evalStack[0];
			if(evalStack.size() >= 2) stack2 = evalStack[1];

			if (token.tokenStr == "+")
			{
				evalStack.insert(evalStack.begin(), stack2 + stack1);
				evalStack.erase(evalStack.begin() + 1, evalStack.begin() + 3);
				continue;
			}
			if (token.tokenStr == "-")
			{
				evalStack.insert(evalStack.begin(), stack2 - stack1);
				evalStack.erase(evalStack.begin() + 1, evalStack.begin() + 3);
				continue;
			}
			if (token.tokenStr == "*")
			{
				evalStack.insert(evalStack.begin(), stack2 * stack1);
				evalStack.erase(evalStack.begin() + 1, evalStack.begin() + 3);
				continue;
			}
			if (token.tokenStr == "/")
			{
				evalStack.insert(evalStack.begin(), stack2 / stack1);
				evalStack.erase(evalStack.begin() + 1, evalStack.begin() + 3);
				continue;
			}
			if (token.tokenStr == "^")
			{
				evalStack.insert(evalStack.begin(), powf(stack2, stack1));
				evalStack.erase(evalStack.begin() + 1, evalStack.begin() + 3);
				continue;
			}

			continue;
		}

		if (token.tokenType == Token::FUNCTION)
		{
			float stack1 = 0.0f;
			if(evalStack.size() >= 1) stack1 = evalStack[0];

			// SINUSOIDALS
			if (token.tokenStr == "sin" || token.tokenStr == "-sin")
			{
				evalStack.insert(evalStack.begin(), sinf(stack1) * (token.tokenStr[0] == '-' ? -1 : 1));
				evalStack.erase(evalStack.begin() + 1);
				continue;
			}
			if (token.tokenStr == "cos" || token.tokenStr == "-cos")
			{
				evalStack.insert(evalStack.begin(), cosf(stack1) * (token.tokenStr[0] == '-' ? -1 : 1));
				evalStack.erase(evalStack.begin() + 1);
				continue;
			}
			if (token.tokenStr == "tan" || token.tokenStr == "-tan")
			{
				evalStack.insert(evalStack.begin(), tanf(stack1) * (token.tokenStr[0] == '-' ? -1 : 1));
				evalStack.erase(evalStack.begin() + 1);
				continue;
			}
			if (token.tokenStr == "sinh" || token.tokenStr == "-sinh")
			{
				evalStack.insert(evalStack.begin(), sinhf(stack1) * (token.tokenStr[0] == '-' ? -1 : 1));
				evalStack.erase(evalStack.begin() + 1);
				continue;
			}
			if (token.tokenStr == "cosh" || token.tokenStr == "-cosh")
			{
				evalStack.insert(evalStack.begin(), coshf(stack1) * (token.tokenStr[0] == '-' ? -1 : 1));
				evalStack.erase(evalStack.begin() + 1);
				continue;
			}
			if (token.tokenStr == "tanh" || token.tokenStr == "-tanh")
			{
				evalStack.insert(evalStack.begin(), tanhf(stack1) * (token.tokenStr[0] == '-' ? -1 : 1));
				evalStack.erase(evalStack.begin() + 1);
				continue;
			}
			// INVERSE SINUSOIDALS
			if (token.tokenStr == "asin" || token.tokenStr == "-asin")
			{
				evalStack.insert(evalStack.begin(), asinf(stack1) * (token.tokenStr[0] == '-' ? -1 : 1));
				evalStack.erase(evalStack.begin() + 1);
				continue;
			}
			if (token.tokenStr == "acos" || token.tokenStr == "-acos")
			{
				evalStack.insert(evalStack.begin(), acosf(stack1) * (token.tokenStr[0] == '-' ? -1 : 1));
				evalStack.erase(evalStack.begin() + 1);
				continue;
			}
			if (token.tokenStr == "atan" || token.tokenStr == "-atan")
			{
				evalStack.insert(evalStack.begin(), atanf(stack1) * (token.tokenStr[0] == '-' ? -1 : 1));
				evalStack.erase(evalStack.begin() + 1);
				continue;
			}
			if (token.tokenStr == "asinh" || token.tokenStr == "-asinh")
			{
				evalStack.insert(evalStack.begin(), asinhf(stack1) * (token.tokenStr[0] == '-' ? -1 : 1));
				evalStack.erase(evalStack.begin() + 1);
				continue;
			}
			if (token.tokenStr == "acosh" || token.tokenStr == "-acosh")
			{
				evalStack.insert(evalStack.begin(), acoshf(stack1) * (token.tokenStr[0] == '-' ? -1 : 1));
				evalStack.erase(evalStack.begin() + 1);
				continue;
			}
			if (token.tokenStr == "atanh" || token.tokenStr == "-atanh")
			{
				evalStack.insert(evalStack.begin(), atanhf(stack1) * (token.tokenStr[0] == '-' ? -1 : 1));
				evalStack.erase(evalStack.begin() + 1);
				continue;
			}
			// LOGARITHMIC
			if (token.tokenStr == "ln" || token.tokenStr == "-ln")
			{
				evalStack.insert(evalStack.begin(), log(stack1) * (token.tokenStr[0] == '-' ? -1 : 1));
				evalStack.erase(evalStack.begin() + 1);
				continue;
			}
			// USER-DEFINED
			Equation* equation = equationList->GetEquation(token.tokenStr);
			if (equation == nullptr)
			{
				evalStack.insert(evalStack.begin(), 0.0f);
				evalStack.erase(evalStack.begin() + 1);
				continue;
			}
			if (equation->invalidEquation)
			{
				evalStack.insert(evalStack.begin(), 0.0f);
				evalStack.erase(evalStack.begin() + 1);
				continue;
			}

			evalStack.insert(evalStack.begin(), equation->Evaluate(stack1) * (token.tokenStr[0] == '-' ? -1 : 1));
			evalStack.erase(evalStack.begin() + 1);
			continue;
		}

		if (token.tokenStr == "t" || token.tokenStr == "-t")
		{
			evalStack.insert(evalStack.begin(), (token.tokenStr[0] == '-' ? -1 : 1) * time);
			continue;
		}
		if (token.tokenStr == "e" || token.tokenStr == "-e")
		{
			evalStack.insert(evalStack.begin(), (token.tokenStr[0] == '-' ? -1 : 1) * EulerConstant);
			continue;
		}
		if (token.tokenStr == "pi" || token.tokenStr == "-pi")
		{
			evalStack.insert(evalStack.begin(), (token.tokenStr[0] == '-' ? -1 : 1) * M_PI);
			continue;
		}

		if (token.tokenType == Token::VARIABLE)
		{
			Variable* variable = variableList->getVariable(token.tokenStr); // Make special functionality in VariableList that works with y5 etc
			if (variable == nullptr)
			{
				evalStack.insert(evalStack.begin(), 0.0f);
				continue;
			}
			evalStack.insert(evalStack.begin(), (token.tokenStr[0] == '-' ? -1 : 1) * variable->value);
			continue;
		}

		if (token.tokenType == Token::FUNCTION_VARIABLE)
		{
			std::string realFuncName = token.functionName;
			if(realFuncName[0] == '-') realFuncName = realFuncName.substr(1, realFuncName.length() - 1);
			Variable* variable = variableList->getFunctionVariable(realFuncName);
			if (variable == nullptr)
			{
				evalStack.insert(evalStack.begin(), 0.0f);
				continue;
			}
			evalStack.insert(evalStack.begin(), (token.tokenStr[0] == '-' ? -1 : 1) * variable->derivativeValues[token.derivativeNumber]);
			continue;
		}
			
		evalStack.insert(evalStack.begin(), token.value);
	}

	if (evalStack.size() <= 0)
	{
		if(time == 0) std::cout << "SOMETHING WENT WRONG, evalStack SIZE IS 0\n";
		return 0.0f;
	}

	//std::string evalResult = std::to_string(evalStack[0]);
	//resultChar = new char [evalResult.length() + 1];
	//strcpy_s(resultChar, (evalResult.length() + 1) * sizeof(char), evalResult.c_str());
	
	return evalStack[0];
}

bool Equation::isValidEquation()
{
	// Maybe add a boolean that tells you if compilation was successful or not
	return
		!invalidEquation &&
		functionName != "" &&
		(variableList->functionVariableList.find(functionName) != variableList->functionVariableList.end()) &&
		curve != nullptr;
}

void Equation::Delete()
{
	delete[] formulaChar;

	if(curve != nullptr) curve->Delete(true);
	delete curve;
}

void Equation::Tokenize()
{
	std::string prevTokenType = "0";
	for (int i = 0; i < formula.size(); i++)
	{
		char letter = formula[i];
		if(letter == ' ') continue;

		std::string lastToken = "";
		if(tokens.size() >= 1)
		{
			lastToken = tokens[tokens.size() - 1];
		}

		// Check if number
		if (floatDigits.find(letter) != std::string::npos)
		{
			if (prevTokenType == "number" || prevTokenType == "negativeSign" || lastToken == ".")
			{
				tokens[tokens.size() - 1] = lastToken + letter;
			}
			else
			{
				tokens.push_back(std::string(1, letter));
			}
			prevTokenType = "number";
			continue;
		}

		// Not a number

		// Check if function
		std::string possibleFunc = getPossibleFunction(formula, i);
		if (possibleFunc != "0")
		{
			if (prevTokenType == "negativeSign") tokens[tokens.size() - 1] += possibleFunc;
			else
			{
				if(prevTokenType == "number" || prevTokenType == "variable") tokens.push_back("*");
				tokens.push_back(possibleFunc);
			}
			i += possibleFunc.length() - 1;
			prevTokenType = "func";
			continue;
		}
		/*
		std::string possibleArcfunc = getArcSinusoidalFunc(formula, i);
		if (possibleArcfunc != "0")
		{
			if (prevTokenType == "negativeSign") tokens[tokens.size() - 1] += possibleArcfunc;
			else
			{
				if(prevTokenType == "number" || prevTokenType == "variable") tokens.push_back("*");
				tokens.push_back(possibleArcfunc);
			}
			i += 3;
			prevTokenType = "func";
			continue;
		}
		std::string possibleFunc = getSinusoidalFunc(formula, i);
		if (possibleFunc != "0")
		{
			if (prevTokenType == "negativeSign") tokens[tokens.size() - 1] += possibleFunc;
			else
			{
				if(prevTokenType == "number" || prevTokenType == "variable") tokens.push_back("*");
				tokens.push_back(possibleFunc);
			}
			i += 2;
			prevTokenType = "func";
			continue;
		}
		std::string possibleLogFunc = getLogarithmicFunc(formula, i);
		if (possibleLogFunc != "0")
		{
			std::cout << "Yas i got the logarithmic!\n";
			if(prevTokenType == "negativeSign") tokens[tokens.size() - 1] += possibleLogFunc;
			else
			{
				if (prevTokenType == "number" || prevTokenType == "variable") tokens.push_back("*");
				tokens.push_back(possibleLogFunc);
			}
			i += 1;
			prevTokenType = "func";
			continue;
		}
		*/

		// Not a function

		// Check if variable
		if (variableRequirementStr.find(letter) != std::string::npos)
		{
			std::cout << "Got a variable for some reason\n";
			if(prevTokenType == "variable" || prevTokenType == "negativeSign") tokens[tokens.size() - 1] += letter;
			else
			{
				if(prevTokenType == "number") tokens.push_back("*");
				tokens.push_back(std::string(1, letter));
			}

			// Check if nextLetter exists
			if (i + 1 > formula.length() - 1)
			{
				prevTokenType = "variable";
				continue;
			}
			char nextLetter = formula[i + 1];

			if (nextLetter == '\'')
			{
				int tickMarkCount = getTickMarks(formula, i);
				tokens[tokens.size() - 1] += std::to_string(tickMarkCount);
				i += tickMarkCount;
				prevTokenType = "variable";
				continue;
			}
			if (intDigits.find(nextLetter) != std::string::npos)
			{
				tokens[tokens.size() - 1] += std::string(1, nextLetter);
				i += 1;
				prevTokenType = "variable";
				continue;
			}

			// None of the above nextLetters worked
			prevTokenType = "variable";
			continue;
		}

		// Check if operator
		if (basicOperatorStr.find(letter) != std::string::npos)
		{
			if (letter == '(' && prevTokenType == "negativeSign")
			{
				int insertIndex = tokens.size() - 1;
				tokens.insert(tokens.begin() + insertIndex, "0");
				tokens.insert(tokens.begin() + insertIndex, "(");
				tokens.push_back("(");
				int prei = i;
				char nextLetter = ' ';
				int nesting = 1;
				while (i + 1 <= formula.length() - 1 && nesting != 0)
				{
					nextLetter = formula[i + 1];
					if(nextLetter == '(') nesting += 1;
					else if(nextLetter == ')') nesting -= 1;
					i++;
				}
				formula.insert(formula.begin() + i, ')');
				i = prei;
				prevTokenType = "operator";
				continue;
			}
			if (letter == '(' && (prevTokenType == "number" || prevTokenType == "variable" || lastToken == ")"))
			{
				tokens.push_back("*");
			}
			tokens.push_back(std::string(1, letter));
			if (letter == '-' && (prevTokenType == "0" || prevTokenType == "operator") && lastToken != ")")
			{
				prevTokenType = "negativeSign";
			}
			else prevTokenType = "operator";
			continue;
		}

		// Done! next...
	}

	// Done tokenizing!

	/*
	std::cout << "formula is " << formula << ", here're the primitive tokens:\n";
	for (int i = 0; i < tokens.size(); i++)
	{
		std::cout << tokens[i] << "\n";
	}
	*/
}

void Equation::Pemdas()
{
	// Check for functions
	for (int i = 0; i < tokens.size(); i++)
	{
		if (isFunction(tokens[i]))
		{
			encloseFunctionInParenthesis(i);
			i++;
		}
	}
	// Check for exponents
	for (int i = 0; i < tokens.size(); i++)
	{
		// Continue if not a ^ or if the next token is at the end of the list
		if (tokens[i] != "^") continue;

		std::string nextToken = "";
		if(i + 1 <= tokens.size() - 1) nextToken = tokens[i + 1];

		if (isFunction(nextToken))
		{
			encloseInParenthesis(i, true);
		}
		else encloseInParenthesis(i, false);
		// Extra i++ due to how this is structured
		i++;
	}

	// Check for multiplication
	for (int i = 0; i < tokens.size(); i++)
	{
		if(tokens[i] != "*") continue;

		std::string nextToken = "";
		if(i + 1 <= tokens.size() - 1) nextToken = tokens[i + 1];

		if (isFunction(nextToken))
		{
			encloseInParenthesis(i, true);
		}
		else encloseInParenthesis(i, false);

		i++;
	}

	// Check for division
	for (int i = 0; i < tokens.size(); i++)
	{
		if(tokens[i] != "/") continue;

		std::string nextToken = "";
		if(i + 1 <= tokens.size() - 1) nextToken = tokens[i + 1];

		if (isFunction(nextToken))
		{
			encloseInParenthesis(i, true);
		}
		else encloseInParenthesis(i, false);

		i++;
	}

	// Check for subtraction ( PEMDAS applies! see: -1 - (1 - 3) vs (-1 - 1) - 3 )
	for (int i = 0; i < tokens.size(); i++)
	{
		if(tokens[i] != "-") continue;

		std::string nextToken = "";
		if(i + 1 <= tokens.size() - 1) nextToken = tokens[i + 1];

		if (isFunction(nextToken))
		{
			encloseInParenthesis(i, true);
		}
		else encloseInParenthesis(i, false);

		i++;
	}


	// Done!
}

void Equation::Postfix()
{
	stack.clear();
	preQueue.clear();

	for (int i = 0; i < tokens.size(); i++)
	{
		std::string token = tokens[i];

		if (isFloat(token) || isVariable(token))
		{
			preQueue.push_back(token);
			continue;
		}
		if (isOperator(token) || isFunction(token))
		{
			if (stack.size() >= 1)
			{
				while (stack[0] != "(" && stack[0] != token)
				{
					preQueue.push_back(stack[0]);
					stack.erase(std::next(stack.begin(), 0));
					if(stack.size() <= 0) break; // Break if no more stack elements
				}
			}
			stack.insert(stack.begin(), token);
			continue;
		}
		if (token == "(")
		{
			stack.insert(stack.begin(), token);
			continue;
		}
		if (token == ")")
		{
			if (stack.size() >= 1)
			{
				while(stack[0] != "(")
				{
					preQueue.push_back(stack[0]);
					stack.erase(std::next(stack.begin(), 0));
					if(stack.size() <= 0) break;
				}
			}
			if (stack.size() >= 1)
			{
				if (stack[0] == "(")
				{
					stack.erase(std::next(stack.begin(), 0));
				}
			}
		}
	}

	while (stack.size() >= 1)
	{
		preQueue.push_back(stack[0]);
		stack.erase(std::next(stack.begin(), 0));
	}

	queue.clear();
	dependencies.clear();

	// Convert queue's tokens to Token objects for faster reading during computation
	for (int i = 0; i < preQueue.size(); i++)
	{
		std::string token = preQueue[i];
		if (isFloat(token))
		{
			float tokenAsFloat = std::stof(token);
			queue.push_back(Token(Token::CONSTANT, token, tokenAsFloat));
			continue;
		}
		if (isOperator(token))
		{
			queue.push_back(Token(Token::OPERATOR, token));
			continue;
		}
		if (isFunction(token))
		{
			queue.push_back(Token(Token::FUNCTION, token));

			if(!isPredefinedFunction(token))
			{
				dependencies.push_back(token);
			}
			continue;
		}
		if (isVariable(token))
		{
			// bools for determining the type of variable
			bool endOfTokenIsInt = intDigits.find(token[token.length() - 1]) != std::string::npos;
			bool isDifferentialEquation = equationList->variableNameExistsAsDifferentialEquation(token) || endOfTokenIsInt;

			// If it's a normal variable
			if(!isDifferentialEquation)
			{
				queue.push_back(Token(Token::VARIABLE, token));
				variableList->addVariableIfNotExists(token);
				continue;
			}

			// It's a function variable

			// Normal equations don't allow differential function variables, so add the variable as the number 0
			if (equationType == NORMAL_EQUATION)
			{
				queue.push_back(Token(Token::CONSTANT, "0.0", 0.0f));
				continue;
			}

			// Add the function variable
			if(endOfTokenIsInt)
			{
				queue.push_back(Token(Token::FUNCTION_VARIABLE, token));
				queue[queue.size() - 1].functionName = token.substr(0, token.length() - 1);
				queue[queue.size() - 1].derivativeNumber = token[token.length() - 1] - '0';
			}
			else
			{
				queue.push_back(Token(Token::FUNCTION_VARIABLE, token));
				queue[queue.size() - 1].functionName = token;
				queue[queue.size() - 1].derivativeNumber = 0;
			}

			continue;
		}
		
		std::cout << "MY ELEMENT IS NONE OF THE ABOVE!! LOOK BELOW:\n";
		std::cout << preQueue[i] << std::endl;
	}
	
	// Done!
}

// Please do i++ after calling this
void Equation::encloseInParenthesis(int i, bool ignoreNextToken)
{
	int prei = i;
	i++;

	int nesting = 0;
	while (i <= tokens.size() - 1 && (nesting != 0 || i <= prei + 1 + ignoreNextToken))
	{
		if(tokens[i] == "(") nesting++;
		else if(tokens[i] == ")") nesting--;
		i++;
	}
	tokens.insert(tokens.begin() + i, ")");
	i = prei;
	i--;
	nesting = 0;
	while (i >= 0 && (nesting != 0 || i >= prei - 1))
	{
		if(tokens[i] == "(") nesting++;
		else if(tokens[i] == ")") nesting--;
		i--;
	}
	std::string currentToken = "";
	if(i >= 0) currentToken = tokens[i];

	if (isFunction(currentToken))
	{
		tokens.insert(tokens.begin() + i, "(");
	}
	else
	{
		tokens.insert(tokens.begin() + i + 1, "(");
	}
}

void Equation::encloseFunctionInParenthesis(int i)
{
	int prei = i;
	i++;

	int nesting = 0;
	while (i <= tokens.size() - 1 && (nesting != 0 || i <= prei + 1))
	{
		if(tokens[i] == "(") nesting++;
		else if(tokens[i] == ")") nesting--;
		i++;
	}
	tokens.insert(tokens.begin() + i, ")");
	i = prei;
	tokens.insert(tokens.begin() + i, "(");
}

/*
std::string getArcSinusoidalFunc(std::string formula, int i)
{
	if (i + 3 > formula.length() - 1) return "0";

	std::string possibleFunc = formula.substr(i, 4);

	if(possibleFunc == "asin" || possibleFunc == "acos" || possibleFunc == "atan") return possibleFunc;

	return "0";
}

std::string getSinusoidalFunc(std::string formula, int i)
{
	if(i + 2 > formula.length() - 1) return "0";

	std::string possibleFunc = formula.substr(i, 3);

	if(possibleFunc == "sin" || possibleFunc == "cos" || possibleFunc == "tan") return possibleFunc;

	return "0";
}

std::string getLogarithmicFunc(std::string formula, int i)
{
	if(i + 1 > formula.length() - 1) return "0";

	std::string possibleFunc = formula.substr(i, 2);

	if(possibleFunc == "ln") return possibleFunc;

	return "0";
}
*/

int Equation::getTickMarks(std::string formula, int i)
{
	int tickMarkCount = 0;
	char nextLetter = '\'';
	while (nextLetter == '\'' && tickMarkCount < 9)
	{
		i++;
		tickMarkCount++;
		if(i + 1 > formula.length() - 1) break;
		nextLetter = formula[i + 1];
	}
	return tickMarkCount;
}

bool Equation::isOperator(std::string token)
{
	for (int i = 0; i < OPERATORS_COUNT; i++)
	{
		if(token == _operators[i]) return true;
	}
	return false;
}

std::string Equation::getPossibleFunction(std::string formula, int i)
{
	// Start with big and end with small
	for (int j = 5; j >= 1; j--)
	{
		if(i + j - 1 > formula.length() - 1) continue; // remainder of formula doesn't contain a string that long? continue

		std::string possibleFunc = formula.substr(i, j);

		if(j == 2 && possibleFunc == "ln") return possibleFunc;
		if(j == 3 && (possibleFunc == "sin" || possibleFunc == "cos" || possibleFunc == "tan")) return possibleFunc;
		if(j == 4 && (possibleFunc == "asin" || possibleFunc == "acos" || possibleFunc == "atan" ||
						possibleFunc == "sinh" || possibleFunc == "cosh" || possibleFunc == "tanh")) return possibleFunc;
		if(j == 5 && (possibleFunc == "asinh" || possibleFunc == "acosh" || possibleFunc == "atanh")) return possibleFunc; // These run first
		if(j == 5) continue; // continues if j == 5 for custom functions, since the character limit for names is 4

		if(equationList->variableNameExistsAsNormalEquation(possibleFunc)) return possibleFunc;
	}
	return "0";
}

bool Equation::isPredefinedFunction(std::string token)
{
	for (int i = 0; i < PREDEFINED_FUNCTION_COUNT; i++)
	{
		if(token == predefined_functions[i] || token == "-" + predefined_functions[i]) return true;
	}
	return false;
}

bool Equation::isFunction(std::string token)
{
	// check pre-defined functions and user-defined functions
	return isPredefinedFunction(token) || equationList->variableNameExistsAsNormalEquation(token);
}

bool Equation::isFloat(std::string token)
{
	// If token empty it's not a number
	if(token.empty()) return false;

	// Check second digit in case it's a -10 or -4
	// but if it's a y5 or something, don't do it
	if (token.length() >= 2 && token[0] == '-')
	{
		return floatDigits.find(token[1]) != std::string::npos;
	}

	// If single digit
	return floatDigits.find(token[0]) != std::string::npos;
}

bool Equation::isVariable(std::string token)
{
	// If it's empty it's not a variable
	if(token.empty()) return false;

	// Check if it has letters but is actually a function
	if(isFunction(token)) return false;

	// Check second digit in case it's a -kA or -bM_a
	// but if it's a y5 or something, don't do it
	if (token.length() >= 2 && token[0] == '-')
	{
		return variableRequirementStr.find(token[1]) != std::string::npos;
	}

	// If single digit
	return variableRequirementStr.find(token[0]) != std::string::npos;
}

void Equation::generateCharFromString(std::string str, char* charArray, int size)
{
	for (int i = 0; i < size; i++)
	{
		if(i >= str.length()) charArray[i] = '\0';
		else charArray[i] = str[i];
	}
}