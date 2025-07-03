#include <gui/equationGUI.h>

#include "imgui/imgui.h"

#include <unordered_map>

#include "calculator/variable.h"
#include "calculator/variableList.h"
#include "calculator/equation.h"
#include "calculator/equationList.h"

#include "calculator/tooltips.h"

EquationGUI::EquationGUI(EquationList* equationList, VariableList* variableList, GraphManager* graphManager)
{
	this->equationList = equationList;
	this->variableList = variableList;
	this->graphManager = graphManager;

	keepWindowActive = NULL;
}

void EquationGUI::construct(int width, int height)
{
	ImGuiWindowFlags window_flags = 0;
	window_flags |= ImGuiWindowFlags_NoMove;
	window_flags |= ImGuiWindowFlags_NoResize;
	window_flags |= ImGuiWindowFlags_NoCollapse;
	window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus;
	keepWindowActive = NULL;
	keepTabsActive = NULL;

	isHoveringImGui = false;
	ImGui::SetNextWindowSize(ImVec2(400.0f, (height - 40.0f) / 2));
	ImGui::SetNextWindowPos(ImVec2(0.0f, 40.0f));
	ImGui::Begin("Equations", keepWindowActive, window_flags);
	if (ImGui::Button("Create New Equation"))
	{
		Equation* newEquation = new Equation(equationList, variableList);
		newEquation->InitializeCurve(graphManager->vertexShader, graphManager->fragmentShader,
									 graphManager->x, graphManager->y,
									 graphManager->zoomX, graphManager->zoomY);
		newEquation->derivativeOrder = 1;

		equationList->AddEquation(newEquation);
		graphManager->redrawCurves();
	}
	/*
	ImGui::SameLine();
	if (ImGui::Button("Compile All"))
	{
		compile_all_equations();
	}
	*/
	ImGui::SameLine();
	CreateTooltip("(?)", equationTooltip);
	
	ImGui::BeginChild("Equations");
	int uniqueID = 0;
	for (int i = 0; i < equationList->EquationCount(); i++)
	{
		ImGui::PushID(uniqueID);
		if(construct_equation_element(i)) i--;
		ImGui::PopID();
		uniqueID++;
	}
	ImGui::EndChild();
	ImGui::End();

	ImGui::SetNextWindowSize(ImVec2(400.0f, (height - 40.0f) / 2));
	ImGui::SetNextWindowPos(ImVec2(0.0f, (height + 40.0f) / 2));
	ImGui::Begin("Variables", keepWindowActive, window_flags);
	if (ImGui::Button("Create New Variable"))
	{
		variableList->setVariable("", 0.0f);
	}
	ImGui::SameLine();
	CreateTooltip("(?)", variableTooltip);

	ImGui::BeginChild("Variables");
	for (int i = 0; i < variableList->VariableCount(); i++)
	{
		ImGui::PushID(uniqueID);
		if(construct_variable_element(i)) i--;
		ImGui::PopID();
		uniqueID++;
	}
	ImGui::EndChild();
	ImGui::End();

	//if(equationList->EquationCount() <= 0) return;
	//EquationDebugPanel(equationList->GetEquation(0));

	//VariableListDebugPanel();
}

bool EquationGUI::construct_equation_element(int id)
{
	Equation* equation = equationList->GetEquation(id);
	int derivativeOrder = equation->derivativeOrder;
	if (!ImGui::CollapsingHeader(equationList->GetEquationName(id), ImGuiTreeNodeFlags_DefaultOpen))
	{
		equation->init = true;
		return false;
	}

	// Type of differential equation (First-Order, Second-Order, etc)
	ImGuiTabBarFlags tab_bar_flags = ImGuiTabBarFlags_None;
	static const char* equationTypeNames[4] = {"First-Order", "Second-Order", "Multi-Order", "Normal"};
    if (ImGui::BeginTabBar("Differential Equation Types", tab_bar_flags))
    {
		int equationTypeID = equation->equationType;
		for (int i = 0; i < 4; i++)
		{
			bool itemClicked = false;
			if (ImGui::BeginTabItem(equationTypeNames[i], keepTabsActive,
				equationTypeID == i && !equation->init ? ImGuiTabItemFlags_SetSelected : ImGuiTabItemFlags_None))
			{
				itemClicked = ImGui::IsItemClicked() || itemClicked;
				ImGui::EndTabItem();
			}
			itemClicked = ImGui::IsItemClicked() || itemClicked;
			if (itemClicked && equation->equationType != i)
			{
				std::cout << "prev equationType was " << equation->equationType << " and now it is " << i << "\n";
				equation->setEquationType(i);
				equation->derivativeOrder = (i == 3 ? 0 : i + 1); // derivativeOrder is 0 when NORMAL_EQUATION otherwise 1, 2, or 3

				graphManager->redrawCurves();
			}
			
		}
		// consider changing this to use equationTypeID and a loop!
		if (ImGui::TabItemButton("x"))
		{
			std::cout << "WILL DELETE THIS EQUATION!" << std::endl;
			variableList->removeFunctionVariable(equation->functionName);
			equationList->RemoveEquation(id);
			compile_all_equations();

			ImGui::EndTabBar();

			equation->init = true;
			return true;
		}
        ImGui::EndTabBar();
    }
	
	// Draw Curve checkbox
	ImGui::Checkbox("Draw Curve", &(equation->drawCurve));

	// Input function name and derivative order
	ImGui::SetNextItemWidth(40.0f);
	ImGui::InputText("Function Name", equation->functionNameChar, VARIABLE_MAX_NAME_LENGTH * sizeof(char));
	if (equation->equationType == Equation::MULTI_ORDER)
	{
		ImGui::SameLine();
		if (ImGui::SliderInt("Derivative Order", &equation->derivativeOrder, 3, 10))
		{
			graphManager->redrawCurves();
		}
	}
	// Update equation's formula through input
	ImGui::Text(equation->getEquationLeftSide());
	ImGui::SameLine();
	ImGui::InputText("Equation", equation->formulaChar, EQUATION_MAX_LENGTH * sizeof(char));

	// Compile Equation if clicked
	if (ImGui::Button("Compile"))
	{
		std::string tmpStr = equation->functionNameChar;
		if (tmpStr == equation->functionName || variableList->functionVariableList.find(tmpStr) == variableList->functionVariableList.end())
		{
			compile_all_equations();
		}
		else
		{
			// notify user of error (make error GUI popups)
			std::cout << "Not letting ya!\n";
		}
	}
	ImGui::SameLine();
	if (ImGui::Button(equation->collapse ? "Expand" : "Collapse"))
	{
		equation->collapse = !equation->collapse;
	}
	if (equation->collapse)
	{
		equation->init = true;
		return false;
	}

	// Variables
	Variable* functionVariable = variableList->getFunctionVariable(equation->functionName);
	// tmpConditional ensures that if we press Edit on a variable and another one was previously being edited, we don't get duplicate elements
	bool tmpConditional = false;
	if (functionVariable != nullptr)
	{
		if(functionVariable->currentlyEditingID >= equation->derivativeOrder) functionVariable->currentlyEditingID = -1;
	}
	for (int i = 0; i < equation->derivativeOrder; i++)
	{
		if(functionVariable == nullptr) break; // Break if the equation doesn't have a well-defined functionName
		ImGui::PushID(i);

		char variableName[8];
		int j;
		for (j = 0; j < equation->functionName.length(); j++)
		{
			variableName[j] = equation->functionName[j];
		}
		variableName[j] = '0' + i;
		variableName[j + 1] = '(';
		variableName[j + 2] = '0';
		variableName[j + 3] = ')';

		//std::cout << "I am initial values! " << functionVariable->derivativeValues[i] << std::endl;

		// Consider changing its width to windowidth - c
		if (ImGui::SliderFloatWithSteps(variableName, &(functionVariable->initialValues[i]),
			functionVariable->initialValueSettings[i * 3], functionVariable->initialValueSettings[i * 3 + 1],
			functionVariable->initialValueSettings[i * 3 + 2], "%.3f"))
		{
			//float step = functionVariable->initialValueSettings[i * 3 + 2];
			//functionVariable->initialValues[i] = step * floorf(functionVariable->initialValues[i] / step);
			//std::cout << "I am changed yay!\n";
			graphManager->redrawCurves();
		}
		ImGui::SameLine();
		if (ImGui::Button(functionVariable->currentlyEditingID == i ? "Done" : "Edit"))
		{
			if(functionVariable->currentlyEditingID == i) functionVariable->currentlyEditingID = -1;
			else functionVariable->currentlyEditingID = i;
		}

		if (functionVariable->currentlyEditingID != i || tmpConditional)
		{
			ImGui::PopID();
			continue;
		}
		tmpConditional = true;

		float itemWidth = (ImGui::GetWindowWidth() - 150.0f) / 3;
		ImGui::SetNextItemWidth(itemWidth);
		ImGui::InputFloat("min", &(functionVariable->initialValueSettings[i * 3]));
		ImGui::SameLine();
		ImGui::SetNextItemWidth(itemWidth);
		ImGui::InputFloat("max", &(functionVariable->initialValueSettings[i * 3 + 1]));
		ImGui::SameLine();
		ImGui::SetNextItemWidth(itemWidth);
		ImGui::InputFloat("step", &(functionVariable->initialValueSettings[i * 3 + 2]));

		ImGui::PopID();
	}

	// Customization
	ImGui::ColorEdit3("color", equation->color);
	isHoveringImGui = isHoveringImGui || ImGui::IsWindowHovered() || ImGui::IsAnyItemHovered();

	equation->init = true;
	return false;
}

bool EquationGUI::construct_variable_element(int id)
{
	std::string variableNameStr = variableList->getVariableNameStr(id);
	Variable* variable = variableList->getVariable(variableNameStr);
	if(variable == nullptr) return false;
	if(variable->variableType == FUNCTION_VARIABLE) return false;

	// Make sure header name isn't x, because we have an x button
	char* variableNameChar = variable->nameChar;
	if(variableNameStr == "x")
	{
		variableNameChar[1] = ' ';
	}
	if (!ImGui::CollapsingHeader(variableNameChar, ImGuiTreeNodeFlags_DefaultOpen)) return false;


	// Update variable's name through input
	//ImGui::Text("Variable Name");
	ImGui::InputText("Variable Name", variable->nameCache, VARIABLE_MAX_NAME_LENGTH * sizeof(char));
	ImGui::SameLine();
	if (ImGui::SmallButton("x"))
	{
		std::cout << "WILL DELETE THIS VARIABLE!" << std::endl;
		variableList->removeVariable(variableNameStr);
		return true;
	}
	// Update variable name if clicked
	if (ImGui::Button("Update Variable Name"))
	{
		int errorCode = variableList->updateVariableName(id, variable->nameCache);

		switch (errorCode)
		{
			case -1:
				// Internal error
				break;
			case 1:
				// User error
				break;
			case 0:
				// Compile all equations please
				for (int k = 0; k < equationList->EquationCount(); k++)
				{
					Equation* equation = equationList->GetEquation(k);
					equation->replaceVariableInEquation(variableNameStr, variableList->getVariableNameStr(id));
					equation->Compile();
				}
				variableNameStr = variableList->getVariableNameStr(id);
				break;
			default:
				break;
		}
	}

	if (variable->currentlyEditingID == 1)
	{
		if (ImGui::InputFloat("value ", &(variable->value)))
		{
			variable->value = variable->settings[2] * floorf(variable->value / variable->settings[2]);
			if(variable->value > variable->settings[1]) variable->value = variable->settings[1];
			if(variable->value < variable->settings[0]) variable->value = variable->settings[0];

			graphManager->redrawCurves();
		}
	}
	else if (ImGui::SliderFloatWithSteps("value", &variable->value, variable->settings[0], variable->settings[1], variable->settings[2], "%.3f"))
	{
		graphManager->redrawCurves();
	}
	ImGui::SameLine();
	if (ImGui::Button(variable->currentlyEditingID == 0 ? "Type in" : "Done"))
	{
		if(variable->currentlyEditingID == 0) variable->currentlyEditingID = 1;
		else variable->currentlyEditingID = 0;
	}

	// Add inputs for changing range
	float itemWidth = (ImGui::GetWindowWidth() - 150.0f) / 3;
	ImGui::SetNextItemWidth(itemWidth);
	ImGui::InputFloat("min", &(variable->settings[0]));
	ImGui::SameLine();
	ImGui::SetNextItemWidth(itemWidth);
	ImGui::InputFloat("max", &(variable->settings[1]));
	ImGui::SameLine();
	ImGui::SetNextItemWidth(itemWidth);
	ImGui::InputFloat("step", &(variable->settings[2]));
	
	// Customization
	isHoveringImGui = isHoveringImGui || ImGui::IsWindowHovered() || ImGui::IsAnyItemHovered();

	return false;
}

void EquationGUI::compile_all_equations()
{
	// First deal with variables and stuff
	for (int i = 0; i < equationList->EquationCount(); i++)
	{
		Equation* equation = equationList->GetEquation(i);

		// Update equation text
		equation->formula = equation->formulaChar;

		// Update function name in case it changed
		if (equation->functionName != equation->functionNameChar)
		{
			variableList->renameFunctionVariable(equation->functionName, equation->functionNameChar);
		}
		equation->functionName = equation->functionNameChar;

		// Remove any variables that have the same name as the current function name
		variableList->removeVariable(equation->functionName);
	}

	// Compile everything
	for (int i = 0; i < equationList->EquationCount(); i++)
	{
		Equation* equation = equationList->GetEquation(i);

		equation->Compile();
	}

	// Check for errors
	for (int i = 0; i < equationList->EquationCount(); i++)
	{
		Equation* equation = equationList->GetEquation(i);

		equation->ErrorCheck();
	}

	// Redraw curves
	graphManager->redrawCurves();
}

void EquationGUI::CreateTooltip(const char* name, const char* desc)
{
	ImGui::TextDisabled(name);
    if (ImGui::BeginItemTooltip())
    {
        ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
        ImGui::TextUnformatted(desc);
        ImGui::PopTextWrapPos();
        ImGui::EndTooltip();
    }
}

void EquationGUI::EquationDebugPanel(Equation* equation)
{
	ImGui::SetNextWindowSize(ImVec2(300.0f, 600.0f));
	ImGui::Begin("Equation tokens");
	if (ImGui::Button("Evaluate expression"))
	{
		expressionEvaluation = equation->Evaluate(0.0f);
	}
	if (ImGui::Button("Evaluate expression 10,000 times"))
	{
		for (int i = 0; i < 1000000; i++)
		{
			equation->Evaluate(0.0f);
		}
		std::cout << "Done!\n";
	}
	char buffer[64];
	int ret = snprintf(buffer, sizeof buffer, "%f", expressionEvaluation);
	ImGui::Text("Result: ");
	ImGui::SameLine();
	ImGui::Text(buffer);
	if (ImGui::CollapsingHeader("Tokens List", ImGuiTreeNodeFlags_DefaultOpen))
	{
		for (int i = 0; i < equation->tokensChar.size(); i++)
		{
			ImGui::Text(equation->tokensChar[i]);
		}
	}
	if (ImGui::CollapsingHeader("Queue List", ImGuiTreeNodeFlags_DefaultOpen))
	{
		for (int i = 0; i < equation->queueChar.size(); i++)
		{
			ImGui::Text(equation->queueChar[i]);
		}
	}
	ImGui::End();
}

void EquationGUI::VariableListDebugPanel()
{
	ImGui::SetNextWindowSize(ImVec2(400.0f, 600.0f));
	ImGui::Begin("Function Variable List");
	if (ImGui::CollapsingHeader("Function Variable List", ImGuiTreeNodeFlags_DefaultOpen))
	{
		std::unordered_map<std::string, Variable*>::iterator it;

		for (it = variableList->functionVariableList.begin(); it != variableList->functionVariableList.end(); it++)
		{
			std::cout << variableList->functionVariableList.size() << ", " << it->first << "\n";
			ImGui::Text(it->second->nameChar);
		}
	}
	if (ImGui::CollapsingHeader("Variable List", ImGuiTreeNodeFlags_DefaultOpen))
	{
		for (int i = 0; i < variableList->VariableCount(); i++)
		{
			Variable* variable = variableList->getVariable(variableList->getVariableNameStr(i));
			if(variable == nullptr) continue;
			ImGui::Text(variable->nameChar);
		}
	}

	ImGui::End();
}