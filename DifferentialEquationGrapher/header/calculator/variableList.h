#ifndef VARIABLE_LIST_CLASS_H
#define VARIABLE_LIST_CLASS_H

#include <unordered_map>
#include <string>

// Change the many std::vectors to a single vector containing Variable objects

#include "calculator/calculatorDefs.h"

class VariableList
{
	public:
		VariableList(EquationList* equationList);

		bool addVariableIfNotExists(std::string name);
		bool addFunctionVariableIfNotExists(std::string name);

		void setVariable(std::string name, float value);
		void changeVariableBy(std::string name, float value);
		Variable* getVariable(std::string name);
		Variable* getFunctionVariable(std::string name);
		std::string getVariableNameStr(int id);

		int updateVariableName(int id, char* newName);
		void removeVariable(std::string name);

		void renameFunctionVariable(std::string oldName, std::string newName);

		int VariableCount();

		void Delete();

		std::unordered_map<std::string, Variable*> functionVariableList;
	private:
	/*
	std::unordered_map<std::string, float> variableList;
		std::vector<std::string> variableOrder;
		std::vector<char*> variableNames;

		std::vector<char*> variableNameCache;
	*/
		std::unordered_map<std::string, Variable*> variableList;
		
		std::vector<std::string> variableOrder;
		
		EquationList* equationList;

		//char* generateNameChar(std::string nameStr);
};


#endif