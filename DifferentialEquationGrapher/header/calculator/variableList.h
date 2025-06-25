#ifndef VARIABLE_LIST_CLASS_H
#define VARIABLE_LIST_CLASS_H

#include <unordered_map>
#include <string>

#include "calculator/variable.h"

// Change the many std::vectors to a single vector containing Variable objects

class VariableList
{
	public:
		VariableList();

		void setVariable(std::string name, float value);
		void changeVariableBy(std::string name, float value);
		Variable* getVariable(std::string name);
		std::string getVariableNameStr(int id);

		int updateVariableName(int id, char* newName);
		void removeVariable(std::string name);

		int VariableCount();
	private:
	/*
	std::unordered_map<std::string, float> variableList;
		std::vector<std::string> variableOrder;
		std::vector<char*> variableNames;

		std::vector<char*> variableNameCache;
	*/
		std::unordered_map<std::string, Variable*> variableList;
		std::vector<std::string> variableOrder;

		void addVariableIfNotExists(std::string name);
		

		//char* generateNameChar(std::string nameStr);
};


#endif