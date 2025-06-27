#include <calculator/equationGUI.h>

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

	isHoveringImGui = false;
	ImGui::SetNextWindowSize(ImVec2(400.0f, (height - 100.0f) / 2));
	ImGui::SetNextWindowPos(ImVec2(0.0f, 100.0f));
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
	ImGui::SameLine();
	if (ImGui::Button("Compile All"))
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

		// Redraw curves
		graphManager->redrawCurves();
	}
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

	ImGui::SetNextWindowSize(ImVec2(400.0f, (height - 100.0f) / 2));
	ImGui::SetNextWindowPos(ImVec2(0.0f, (height + 100.0f) / 2));
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

	if(equationList->EquationCount() <= 0) return;
	EquationDebugPanel(equationList->GetEquation(0));

	//VariableListDebugPanel();
}

bool EquationGUI::construct_equation_element(int id)
{
	Equation* equation = equationList->GetEquation(id);
	int derivativeOrder = equation->derivativeOrder;
	if (!ImGui::CollapsingHeader(equationList->GetEquationName(id), ImGuiTreeNodeFlags_DefaultOpen)) return false;

	// Type of differential equation (First-Order, Second-Order, etc)
	ImGuiTabBarFlags tab_bar_flags = ImGuiTabBarFlags_None;
    if (ImGui::BeginTabBar("Differential Equation Types", tab_bar_flags))
    {
        if (ImGui::BeginTabItem("First-Order"))
        {
			if (equation->equationType != Equation::FIRST_ORDER)
			{
				equation->equationType = Equation::FIRST_ORDER;
				equation->derivativeOrder = 1;

				graphManager->redrawCurves();
			}
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Second-Order"))
        {
			if (equation->equationType != Equation::SECOND_ORDER)
			{
				equation->equationType = Equation::SECOND_ORDER;
				equation->derivativeOrder = 2;

				graphManager->redrawCurves();
			}
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Multi-Order"))
        {
			if (equation->equationType != Equation::MULTI_ORDER)
			{
				equation->equationType = Equation::MULTI_ORDER;
				equation->derivativeOrder = 3;

				graphManager->redrawCurves();
			}
            ImGui::EndTabItem();
        }
		if (ImGui::TabItemButton("x"))
		{
			std::cout << "WILL DELETE THIS EQUATION!" << std::endl;
			equationList->RemoveEquation(id);
			graphManager->redrawCurves();

			ImGui::EndTabBar();
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
		// Update equation text
		equation->formula = equation->formulaChar;

		// Update function name in case it changed
		if (equation->functionName != equation->functionNameChar)
		{
			variableList->renameFunctionVariable(equation->functionName, equation->functionNameChar);
			for (int i = 0; i < equationList->EquationCount(); i++)
			{
				equationList->GetEquation(i)->Compile();
			}
		}
		equation->functionName = equation->functionNameChar;

		// Remove any variables that have the same name as the current function name
		variableList->removeVariable(equation->functionName);

		equation->Compile();

		graphManager->redrawCurves();
	}

	// Variables
	Variable* functionVariable = variableList->getFunctionVariable(equation->functionName);
	for (int i = 0; i < equation->derivativeOrder; i++)
	{
		if(equation->functionName == "") break; // Break if the equation doesn't have a well-defined functionName
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
		if (ImGui::SliderFloat(variableName, &(functionVariable->initialValues[i]), -10.0f, 10.0f))
		{
			//std::cout << "I am changed yay!\n";
			graphManager->redrawCurves();
		}
	}

	// Customization
	ImGui::ColorEdit3("color", equation->color);
	isHoveringImGui = isHoveringImGui || ImGui::IsWindowHovered() || ImGui::IsAnyItemHovered();

	return false;
}

bool EquationGUI::construct_variable_element(int id)
{
	std::string variableNameStr = variableList->getVariableNameStr(id);
	Variable* variable = variableList->getVariable(variableNameStr);
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

	// Variable Value
	if (ImGui::SliderFloat("value", &variable->value, -10.0f, 10.0f))
	{
		graphManager->redrawCurves();
	}

	// Add inputs for changing range
	
	// Customization
	isHoveringImGui = isHoveringImGui || ImGui::IsWindowHovered() || ImGui::IsAnyItemHovered();

	return false;
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
			ImGui::Text(it->second->nameChar);
		}
	}
	if (ImGui::CollapsingHeader("Variable List", ImGuiTreeNodeFlags_DefaultOpen))
	{
		for (int i = 0; i < variableList->VariableCount(); i++)
		{
			Variable* variable = variableList->getVariable(variableList->getVariableNameStr(i));
			ImGui::Text(variable->nameChar);
		}
	}

	ImGui::End();
}