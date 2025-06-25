#include <calculator/equationGUI.h>

#include "imgui/imgui.h"

#include "calculator/equation.h"

EquationGUI::EquationGUI(EquationList* equationList, VariableList* variableList, GraphManager* graphManager)
{
	this->equationList = equationList;
	this->variableList = variableList;
	this->graphManager = graphManager;
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
		Equation* newEquation = new Equation();
		equationList->AddEquation(newEquation);
	}
	
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

	ImGui::BeginChild("Variables");
	std::cout << "Variable count: " << variableList->VariableCount() << std::endl;
	for (int i = 0; i < variableList->VariableCount(); i++)
	{
		ImGui::PushID(uniqueID);
		if(construct_variable_element(i)) i--;
		ImGui::PopID();
		uniqueID++;
	}
	ImGui::EndChild();
	ImGui::End();
}

bool EquationGUI::construct_equation_element(int id)
{
	Equation* equation = equationList->GetEquation(id);
	if (!ImGui::CollapsingHeader(equationList->GetEquationName(id), ImGuiTreeNodeFlags_DefaultOpen)) return false;
	// Type of differential equation
	ImGuiTabBarFlags tab_bar_flags = ImGuiTabBarFlags_None;
    if (ImGui::BeginTabBar("Differential Equation Types", tab_bar_flags))
    {
        if (ImGui::BeginTabItem("First-Order"))
        {
			if (equation->equationType != Equation::FIRST_ORDER)
			{
				equation->equationType = Equation::FIRST_ORDER;
				equation->SetFormula("");
			}
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Second-Order"))
        {
			if (equation->equationType != Equation::SECOND_ORDER)
			{
				equation->equationType = Equation::SECOND_ORDER;
				equation->SetFormula("");
			}
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Multi-Order"))
        {
			if (equation->equationType != Equation::MULTI_ORDER)
			{
				equation->equationType = Equation::MULTI_ORDER;
				equation->SetFormula("");
			}
            ImGui::EndTabItem();
        }
		if (ImGui::TabItemButton("x"))
		{
			std::cout << "WILL DELETE THIS EQUATION!" << std::endl;
			equationList->RemoveEquation(id);
			ImGui::EndTabBar();
			return true;
		}
        ImGui::EndTabBar();
    }
			
	ImGui::Text("Enter an equation to graph!");
	ImGui::Checkbox("Draw Curve", &(graphManager->drawCurve));

	// Update equation's formula through input
	ImGui::InputText("Equation", equation->formulaChar, EQUATION_MAX_LENGTH * sizeof(char));
	//equation->formula = equation->formulaChar;

	// Compile Equation if clicked
	if (ImGui::Button("Compile"))
	{
		std::cout << "COMPILE THIS!\n";
	}

	// Variables
	ImGui::SliderFloat("x0", &(graphManager->x0), -10.0f, 10.0f);
	ImGui::SliderFloat("v0", &(graphManager->v0), -10.0f, 10.0f);

	// Customization
	ImGui::ColorEdit3("color", graphManager->color);
	isHoveringImGui = isHoveringImGui || ImGui::IsWindowHovered() || ImGui::IsAnyItemHovered();

	return false;
}

bool EquationGUI::construct_variable_element(int id)
{
	std::string variableNameStr = variableList->getVariableNameStr(id);
	char* variableNameChar = variableList->getVariableNameChar(id);
	if (!ImGui::CollapsingHeader(variableNameChar, ImGuiTreeNodeFlags_DefaultOpen)) return false;


	// Update variable's name through input
	//ImGui::Text("Variable Name");
	ImGui::InputText("Variable Name", variableList->getVariableNameCache(id), VARIABLE_MAX_NAME_LENGTH * sizeof(char));
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
		int errorCode = variableList->updateVariableName(id, variableList->getVariableNameCache(id));

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
				variableNameStr = variableList->getVariableNameStr(id);
				break;
			default:
				break;
		}
	}

	// Variable Value
	ImGui::SliderFloat("value", variableList->getVariablePtr(variableNameStr), -10.0f, 10.0f);

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