#ifndef VARIABLE_LIST_CLASS_H
#define VARIABLE_LIST_CLASS_H

#include <unordered_map>
#include <string>

const int VARIABLE_MAX_NAME_LENGTH = 4;

class VariableList
{
	public:
		VariableList();

		void setVariable(std::string name, float value);
		void changeVariableBy(std::string name, float value);
		float* getVariablePtr(std::string name);
		float getVariableValue(std::string name);

		std::string getVariableNameStr(int id);
		char* getVariableNameChar(int id);
		char* getVariableNameCache(int id);
		int updateVariableName(int id, char* newName);

		void removeVariable(std::string name);

		int VariableCount();
	private:
		std::unordered_map<std::string, float> variableList;
		std::vector<std::string> variableOrder;
		std::vector<char*> variableNames;

		std::vector<char*> variableNameCache;

		char* generateNameChar(std::string nameStr);
};


#endif