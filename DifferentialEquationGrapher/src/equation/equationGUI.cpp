#include <calculator/equationGUI.h>

#include "imgui/imgui.h"

#include "calculator/equation.h"

EquationGUI::EquationGUI(EquationList* equationList, GraphManager* graphManager)
{
	this->equationList = equationList;
	this->graphManager = graphManager;
}

void EquationGUI::construct(int width, int height)
{
	ImGuiWindowFlags window_flags = 0;
	window_flags |= ImGuiWindowFlags_NoMove;
	window_flags |= ImGuiWindowFlags_NoResize;
	window_flags |= ImGuiWindowFlags_NoCollapse;
	window_flags |= ImGuiWindowFlags_MenuBar;
	window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus;
	keepWindowActive = NULL;

	isHoveringImGui = false;
	ImGui::SetNextWindowSize(ImVec2(400.0f, height - 100.0f));
	ImGui::SetNextWindowPos(ImVec2(0.0f, 100.0f));
	ImGui::Begin("I am the future!!", keepWindowActive, window_flags);
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
}

bool EquationGUI::construct_equation_element(int id)
{
	Equation* equation = equationList->GetEquation(id);
	if (!ImGui::CollapsingHeader(equationList->GetEquationName(id))) return false;
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

