#include <calculator/equation.h>

#include <iostream>

Equation::Equation()
{
	this->formula = "";
	generateFormulaChar();
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


const std::string intDigits = "1234567890";
const std::string floatDigits = "1234567890.";
const std::string variableRequirementStr = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz_";
// The above string might be the problem in case of a bug cause UTF-8 stuff
const std::string basicOperatorStr = "()+-/*^";
const int OPERATORS_COUNT = 17;
const std::string _operators[] = {
	"+", "-", "*", "/", "^",
	"sin", "-sin", "asin", "-asin",
	"cos", "-cos", "acos", "-acos",
	"tan", "-tan", "atan", "-atan"
};
const int SINUSOIDALS_COUNT = 12;
const std::string sinusoidals[] = {
	"sin", "-sin", "asin", "-asin",
	"cos", "-cos", "acos", "-acos",
	"tan", "-tan", "atan", "-atan"
};

std::string getArcSinusoidalFunc(std::string formula, int i);
std::string getSinusoidalFunc(std::string formula, int i);
std::string getTickMarks(std::string formula, int i);
bool isOperator(std::string token);
bool isSinusoidal(std::string token);
bool isFloat(std::string token);
bool isVariable(std::string token);

// Consider passing in the variable list 
void Equation::Compile(VariableList* variableList)
{
	tokens.clear();
	tokensChar.clear();

	if(formula.empty()) return;

	std::cout << "The formula is " << formula << std::endl;

	Tokenize();
	Pemdas();
	Postfix();

	/*
	for (int i = 0; i < tokens.size(); i++)
	{
		char* tokenChar = new char [tokens[i].length() + 1];
		strcpy_s(tokenChar, (tokens[i].length() + 1) * sizeof(char), tokens[i].c_str());
		tokensChar.push_back(tokenChar);
	}
	*/
	// Get preQueue characters
	for (int i = 0; i < preQueue.size(); i++)
	{
		char* tokenChar = new char [preQueue[i].length() + 1];
		strcpy_s(tokenChar, (preQueue[i].length() + 1) * sizeof(char), preQueue[i].c_str());
		tokensChar.push_back(tokenChar);
	}
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

		// Check if sin, cos, tan or asin, acos, atan
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
			prevTokenType = "sincostan";
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
			prevTokenType = "sincostan";
			continue;
		}

		// Not a sinusoidal function

		// Check if variable
		if (variableRequirementStr.find(letter) != std::string::npos)
		{
			std::cout << "I will do something... letter: " << letter << ", " << tokens.size() << std::endl;
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
				std::string tickMarks = getTickMarks(formula, i);
				tokens[tokens.size() - 1] += tickMarks;
				i += tickMarks.length();
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
			if (letter == '(' && (prevTokenType == "number" || prevTokenType == "variable"))
			{
				tokens.push_back("*");
			}
			tokens.push_back(std::string(1, letter));
			if (letter == '-' && (prevTokenType == "0" || prevTokenType == "operator"))
			{
				prevTokenType = "negativeSign";
			}
			else prevTokenType = "operator";
			continue;
		}

		// Done! next...
	}

	// Done tokenizing!
}

void Equation::Pemdas()
{
	// Check for sinudosial
	for (int i = 0; i < tokens.size(); i++)
	{
		if (isSinusoidal(tokens[i]))
		{
			encloseSinudosialInParenthesis(i);
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

		if (isSinusoidal(nextToken))
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

		if (isSinusoidal(nextToken))
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

		if (isSinusoidal(nextToken))
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
		std::cout << "Token is " << token << std::endl;

		if (isFloat(token) || isVariable(token))
		{
			preQueue.push_back(token);
			continue;
		}
		if (isOperator(token))
		{
			std::cout << "I am an operator! " << token << std::endl;
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

	// Convert queue's tokens to Token objects for faster reading during computation
	for (int i = 0; i < preQueue.size(); i++)
	{
		if (isFloat(preQueue[i]))
		{
			float tokenAsFloat = std::stof(preQueue[i]);
			queue.push_back(Token(Token::CONSTANT, preQueue[i], tokenAsFloat));
			continue;
		}
		if (isVariable(preQueue[i]))
		{
			queue.push_back(Token(Token::VARIABLE, preQueue[i]));
			continue;
		}
		if (isOperator(preQueue[i]))
		{
			queue.push_back(Token(Token::OPERATOR, preQueue[i]));
			continue;
		}
		//std::cout << "MY ELEMENT IS NONE OF THE ABOVE!! LOOK BELOW:\n";
		//std::cout << preQueue[i] << std::endl;
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

	if (isSinusoidal(currentToken))
	{
		tokens.insert(tokens.begin() + i, "(");
	}
	else
	{
		tokens.insert(tokens.begin() + i + 1, "(");
	}
}

void Equation::encloseSinudosialInParenthesis(int i)
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

std::string getArcSinusoidalFunc(std::string formula, int i)
{
	if (i + 3 > formula.length() - 1) return "0";

	std::string possibleFunc = formula.substr(i, 4);

	if(possibleFunc == "asin" || possibleFunc == "acos" || possibleFunc == "atan") return possibleFunc;

	return "0";
}

std::string getSinusoidalFunc(std::string formula, int i)
{
	std::cout << "Boutta get me a sinudosial!\n";
	if(i + 2 > formula.length() - 1) return "0";

	std::string possibleFunc = formula.substr(i, 3);

	if(possibleFunc == "sin" || possibleFunc == "cos" || possibleFunc == "tan") return possibleFunc;

	std::cout << "I failed...\n";
	return "0";
}

std::string getTickMarks(std::string formula, int i)
{
	std::string tickMarkStr = "";
	char nextLetter = '\'';
	while (nextLetter == '\'' && tickMarkStr.length() <= 9)
	{
		i++;
		tickMarkStr += "'";
		if(i + 1 > formula.length() - 1) break;
		nextLetter = formula[i + 1];
	}
	return tickMarkStr;
}

bool isOperator(std::string token)
{
	for (int i = 0; i < OPERATORS_COUNT; i++)
	{
		if(token == _operators[i]) return true;
	}
	return false;
}

bool isSinusoidal(std::string token)
{
	for (int i = 0; i < SINUSOIDALS_COUNT; i++)
	{
		if(token == sinusoidals[i]) return true;
	}
	return false;
}

bool isFloat(std::string token)
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

bool isVariable(std::string token)
{
	// If it's empty it's not a variable
	if(token.empty()) return false;

	// Check if it has letters but is actually sinusoidal
	if(isSinusoidal(token)) return false;

	// Check second digit in case it's a -kA or -bM_a
	// but if it's a y5 or something, don't do it
	if (token.length() >= 2 && token[0] == '-')
	{
		return variableRequirementStr.find(token[1]) != std::string::npos;
	}

	// If single digit
	return variableRequirementStr.find(token[0]) != std::string::npos;
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