#include <gui/menuGUI.h>

#include "imgui/imgui.h"

#include "plugins/ImFileBrowser.h"

#include <iostream>


MenuGUI::MenuGUI(EquationList* equationList, VariableList* variableList)
{
    this->equationList = equationList;
    this->variableList = variableList;
}

void MenuGUI::construct(int width, int height)
{
    ImGuiWindowFlags window_flags = 0;
	window_flags |= ImGuiWindowFlags_NoMove;
	window_flags |= ImGuiWindowFlags_NoResize;
	window_flags |= ImGuiWindowFlags_NoCollapse;
	window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus;
    window_flags |= ImGuiWindowFlags_MenuBar;
	keepWindowActive = NULL;

	ImGui::SetNextWindowPos(ImVec2(0.0f, 0.0f));
	ImGui::SetNextWindowSize(ImVec2(width, 100.0f));
	ImGui::Begin("Differential Equation Grapher", keepWindowActive, window_flags);
	if (ImGui::BeginMenuBar())
    {
        if (ImGui::BeginMenu("File"))
        {
            if (ImGui::MenuItem("New"))
            {
                std::cout << "Should clear the board, or make a new one!\n";
            }
            if (ImGui::MenuItem("Open", NULL))
            {
                std::cout << "Should open files\n";
            }
            if (ImGui::BeginMenu("Recent"), NULL)
            {
                ImGui::Checkbox("I do nice stuff", &doStuff);
                ImGui::Text("Yay!");
                ImGui::EndMenu();
            }
            ImGui::SeparatorText("More stuff");
            ImGui::MenuItem("I am nice", NULL);
            ImGui::SetItemTooltip("I am an expository piece of text!");
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Edit"))
        {
            //ImGui::MenuItem("", NULL);

            ImGui::EndMenu();
        }
        //if (ImGui::MenuItem("MenuItem")) {} // You can also use MenuItem() inside a menu bar!
        ImGui::EndMenuBar();
    }
	ImGui::End();
}