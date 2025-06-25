#include <calculator/variableList.h>


VariableList::VariableList()
{

}

void VariableList::addVariableIfNotExists(std::string name)
{
	if (variableList.find(name) == variableList.end())
	{
		variableOrder.push_back(name);

		variableList[name] = new Variable(CONSTANT_VARIABLE, name);
	}
}

void VariableList::setVariable(std::string name, float value)
{
	if(name.size() > VARIABLE_MAX_NAME_LENGTH) return;

	addVariableIfNotExists(name);

	variableList[name]->value = value;
}

void VariableList::changeVariableBy(std::string name, float value)
{
	if(name.size() > VARIABLE_MAX_NAME_LENGTH) return;
	
	addVariableIfNotExists(name);

	variableList[name]->value += value;
}




Variable* VariableList::getVariable(std::string name)
{
	if(name.size() > VARIABLE_MAX_NAME_LENGTH) return variableList[variableOrder[0]];

	addVariableIfNotExists(name);

	return variableList[name];
}

std::string VariableList::getVariableNameStr(int id)
{
	if(id < 0 || id >= VariableCount()) return "ERROR";
	return variableOrder[id];
}





int VariableList::updateVariableName(int id, char* newName)
{
	if(id < 0 || id >= VariableCount()) return -1;
	if (variableList.find(newName) != variableList.end()) return 1;

	Variable* variable = variableList[variableOrder[id]];

	variableList.erase(variableOrder[id]);

	std::string newNameStr = newName;
	variableOrder[id] = newName;
	variableList[newNameStr] = variable;
	variableList[newNameStr]->nameChar = generateNameChar(newName);

	return 0;
}

void VariableList::removeVariable(std::string name)
{
	Variable* variable = variableList[name];
	variableList.erase(name);
	delete variable;

	for (int i = 0; i < variableOrder.size(); i++)
	{
		if(variableOrder[i] != name) continue;

		variableOrder.erase(std::next(std::begin(variableOrder), i));
		return;
	}
}





int VariableList::VariableCount()
{
	return variableOrder.size();
}