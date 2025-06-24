#ifndef VARIABLE_LIST_CLASS_H
#define VARIABLE_LIST_CLASS_H

#include <unordered_map>
#include <string>

class VariableList
{
	public:
		VariableList();

		void setVariable(std::string name, float value);
		void changeVariableBy(std::string name, float value);
		float getVariable(std::string name);
	private:
		std::unordered_map<std::string, float> variableList;
};


#endif