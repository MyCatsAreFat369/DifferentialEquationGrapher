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
const std::string _operators[] = {
	"+", "-", "*", "/", "^", "sin", "cos", "tan", "asin", "acos", "atan"
};
const std::string sinudosials[] = {
	"sin", "-sin", "asin", "-asin",
	"cos", "-cos", "acos", "-acos",
	"tan", "-tan", "atan", "-atan"
};

std::string getArcSinudosialFunc(std::string formula, int i);
std::string getSinudosialFunc(std::string formula, int i);
std::string getTickMarks(std::string formula, int i);

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

	for (int i = 0; i < tokens.size(); i++)
	{
		char* tokenChar = new char [tokens[i].length() + 1];
		strcpy_s(tokenChar, (tokens[i].length() + 1) * sizeof(char), tokens[i].c_str());
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
		std::string possibleArcfunc = getArcSinudosialFunc(formula, i);
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
		std::string possibleFunc = getSinudosialFunc(formula, i);
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

		// Not a sinudosial function

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
		if (isSinudosial(tokens[i]))
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

		if (isSinudosial(nextToken))
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

		if (isSinudosial(nextToken))
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

		if (isSinudosial(nextToken))
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
	tokens.insert(tokens.begin() + i + 1, "(");
}

void Equation::encloseSinudosialInParenthesis(int i)
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
	tokens.insert(tokens.begin() + i, "(");
}

std::string getArcSinudosialFunc(std::string formula, int i)
{
	if (i + 3 > formula.length() - 1) return "0";

	std::string possibleFunc = formula.substr(i, 4);

	if(possibleFunc == "asin" || possibleFunc == "acos" || possibleFunc == "atan") return possibleFunc;

	return "0";
}

std::string getSinudosialFunc(std::string formula, int i)
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

bool isSinudosial(std::string token)
{
	return std::find(sinudosials->begin(), sinudosials->end(), token) != sinudosials->end();
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