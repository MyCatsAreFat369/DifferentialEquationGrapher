#include <calculator/variableList.h>


VariableList::VariableList()
{

}

void VariableList::setVariable(std::string name, float value)
{
	if(name.size() > VARIABLE_MAX_NAME_LENGTH) return;
	if (variableList.find(name) == variableList.end())
	{
		variableOrder.push_back(name);

		variableNames.push_back(generateNameChar(name));
		variableNameCache.push_back(generateNameChar(name));
	}

	variableList[name] = value;
}

void VariableList::changeVariableBy(std::string name, float value)
{
	if(name.size() > VARIABLE_MAX_NAME_LENGTH) return;
	if(variableList.find(name) == variableList.end())
	{
		variableList[name] = 0.0f;
		variableOrder.push_back(name);
		
		variableNames.push_back(generateNameChar(name));
		variableNameCache.push_back(generateNameChar(name));
	}

	variableList[name] += value;
}

float* VariableList::getVariablePtr(std::string name)
{
	if(name.size() > VARIABLE_MAX_NAME_LENGTH) return nullptr;
	if(variableList.find(name) == variableList.end()) 
	{
		variableList[name] = 0.0f;
		variableOrder.push_back(name);
		
		variableNames.push_back(generateNameChar(name));
		variableNameCache.push_back(generateNameChar(name));
	}

	return &variableList[name];
}

float VariableList::getVariableValue(std::string name)
{
	return *getVariablePtr(name);
}

std::string VariableList::getVariableNameStr(int id)
{
	if(id < 0 || id >= VariableCount()) return "ERROR";
	return variableOrder[id];
}

char* VariableList::getVariableNameChar(int id)
{
	if(id < 0 || id >= VariableCount()) return nullptr;
	return variableNames[id];
}

char* VariableList::getVariableNameCache(int id)
{
	if(id < 0 || id >= VariableCount()) return nullptr;
	return variableNameCache[id];
}

int VariableList::updateVariableName(int id, char* newName)
{
	if(id < 0 || id >= VariableCount()) return -1;
	if (variableList.find(newName) != variableList.end()) return 1;

	float variableValue = variableList[variableOrder[id]];

	variableList.erase(variableOrder[id]);

	std::string newNameStr = newName;
	variableList[newNameStr] = variableValue;
	variableOrder[id] = newName;
	variableNames[id] = generateNameChar(variableOrder[id]);

	return 0;
}

void VariableList::removeVariable(std::string name)
{
	variableList.erase(name);
	for (int i = 0; i < variableOrder.size(); i++)
	{
		if(variableOrder[i] != name) continue;

		variableOrder.erase(std::next(std::begin(variableOrder), i));
		variableNames.erase(std::next(std::begin(variableNames), i));
		variableNameCache.erase(std::next(std::begin(variableNameCache), i));
		return;
	}
}

int VariableList::VariableCount()
{
	return variableOrder.size();
}

char* VariableList::generateNameChar(std::string nameStr)
{
	char* name = new char[VARIABLE_MAX_NAME_LENGTH];
	for (int i = 0; i < VARIABLE_MAX_NAME_LENGTH; i++)
	{
		if(i < nameStr.length()) name[i] = nameStr[i];
		else name[i] = '\0';
	}
	return name;
}