#include <gui/menuGUI.h>
#include <filesystem>
namespace fs = std::filesystem;

#include "calculator/equation.h"
#include "calculator/variable.h"
#include "calculator/equationList.h"
#include "calculator/variableList.h"


MenuGUI::MenuGUI(EquationList* equationList, VariableList* variableList, GraphManager* graphManager)
{
    this->equationList = equationList;
    this->variableList = variableList;
    this->graphManager = graphManager;

    //mCurrentFile = "< ... >";

    //mFileDialog.SetTitle("Open mesh");
    //mFileDialog.SetFileFilters({ ".fbx", ".obj" });

    read_config_recent();
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
	ImGui::SetNextWindowSize(ImVec2(width, 40.0f));
	ImGui::Begin("Differential Equation Grapher", keepWindowActive, window_flags);
	if (ImGui::BeginMenuBar())
    {
        if (ImGui::BeginMenu("File"))
        {
            if (ImGui::MenuItem("New"))
            {
                std::cout << "Should clear the board, or make a new one!\n";
                // Add clearing board logic
            }
            if (ImGui::MenuItem("Open"))
            {
                std::cout << "Should open files\n";
                fileOpenPopup = true;
            }
            if (ImGui::BeginMenu("Recent"))
            {
                for (int i = 0; i < recentFiles.size(); i++)
                {
                    if (ImGui::Selectable(recentFiles[i].c_str()))
                    {
                        std::cout << "Should open that recent file!\n";
                        // Add opening recent file logic
                    }
                }
                ImGui::EndMenu();
            }
            ImGui::Separator();
            if (ImGui::MenuItem("Save"))
            {
                std::cout << "Should save files\n";
                if(currentFileName == "") fileSavePopup = true;
                else if (!save_file(currentFileName))
                {
                    std::cout << "COULDNT SAVE FOR SOME REASON\n";
                    // Output an error for the user
                }
            }
            if (ImGui::MenuItem("Save as"))
            {
                std::cout << "Should save as\n";
                fileSavePopup = true;
            }
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Edit"))
        {
            ImGui::Text("Nothing here yet...");

            ImGui::EndMenu();
        }
        //if (ImGui::MenuItem("MenuItem")) {} // You can also use MenuItem() inside a menu bar!
        ImGui::EndMenuBar();
    }

    if (fileOpenPopup)
    {
        fileOpenPopup = false;
        ImGui::OpenPopup("Browse Files");

        filesCurrentlyOpen.clear();
        for (const auto& entry : fs::directory_iterator("res/Graphs/"))
        {
            std::cout << entry.path().filename().string() << std::endl;
            filesCurrentlyOpen.push_back(entry.path());
        }
    }
    construct_popup_openfile();

    if (fileSavePopup)
    {
        fileSavePopup = false;
        ImGui::OpenPopup("Save File");

        filesCurrentlyOpen.clear();
        for (const auto& entry : fs::directory_iterator("res/Graphs/"))
        {
            std::cout << entry.path().string() << std::endl;
            filesCurrentlyOpen.push_back(entry.path());
        }
    }

    construct_popup_savefile();

	ImGui::End();
}

void MenuGUI::construct_popup_openfile()
{
    // Always center this window when appearing
    ImVec2 center = ImGui::GetMainViewport()->GetCenter();
    ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

    if (ImGui::BeginPopupModal("Browse Files", NULL, ImGuiWindowFlags_AlwaysAutoResize))
    {
        ImGui::Text("Select a file");
        ImGui::Separator();

        /*
        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));
        ImGui::Checkbox("Don't ask me next time", &dont_ask_me_next_time);
        ImGui::PopStyleVar();
        */
        for (int i = 0; i < filesCurrentlyOpen.size(); i++)
        {
            char buf[128];
            sprintf_s(buf, filesCurrentlyOpen[i].filename().string().c_str());
            if (ImGui::Selectable(buf, selectedFileID == i, ImGuiSelectableFlags_DontClosePopups))
            {
                selectedFileID = i;
            }
        }
        
        //ImGui::Text("File 2...");
        //ImGui::Text("File 3...");

        if (ImGui::Button("Open", ImVec2(120, 0)))
        {
            read_file_contents(filesCurrentlyOpen[selectedFileID].filename().string());
            // add open_file() function when you're finished making it
            open_file(filesCurrentlyOpen[selectedFileID].filename().string());
            ImGui::CloseCurrentPopup();
        }
        ImGui::SetItemDefaultFocus();
        ImGui::SameLine();
        if (ImGui::Button("Cancel", ImVec2(120, 0)))
        {
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }
}

void MenuGUI::construct_popup_savefile()
{
    // Always center this window when appearing
    ImVec2 center = ImGui::GetMainViewport()->GetCenter();
    ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

    if (ImGui::BeginPopupModal("Save File", NULL, ImGuiWindowFlags_AlwaysAutoResize))
    {
        ImGui::Text("Enter a file name");
        ImGui::Separator();

        /*
        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));
        ImGui::Checkbox("Don't ask me next time", &dont_ask_me_next_time);
        ImGui::PopStyleVar();
        */
        static char filename[128];

        for (int i = 0; i < filesCurrentlyOpen.size(); i++)
        {
            char buf[128];
            sprintf_s(buf, filesCurrentlyOpen[i].filename().string().c_str());
            if (ImGui::Selectable(buf, selectedFileID == i, ImGuiSelectableFlags_DontClosePopups))
            {
                selectedFileID = i;
                tmpFileName = buf;
                tmpFileName.copy(filename, 128);
                if(tmpFileName.length() < 128) filename[tmpFileName.length()] = '\0';
            }
        }

        if (ImGui::InputText("File name", filename, 128))
        {
            std::cout << "I need testing! File name is being entered\n";
            selectedFileID = -1;
        }

        if (ImGui::Button("Save", ImVec2(120, 0)))
        {
            // Add functionality where if you try to rewrite a file it warns you
            if(save_file(filename))
            {
                currentFileName = filename;
                ImGui::CloseCurrentPopup();
            }
        }
        ImGui::SetItemDefaultFocus();
        ImGui::SameLine();
        if (ImGui::Button("Cancel", ImVec2(120, 0)))
        {
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }
}

void MenuGUI::read_file_contents(std::string filename)
{
    std::string line;
	std::ifstream in("res/Graphs/" + filename);

    fileReadLines.clear();
    
    while (std::getline(in, line))
    {
        fileReadLines.push_back(line);
        std::cout << line << std::endl;
    }

    in.close();
}

bool MenuGUI::write_to_file(std::string filename)
{
    if (fileWriteLines.empty())
    {
        std::cout << "Could not write to file " << filename << " as fileWriteLines is empty." << std::endl;
        return false;
    }

    std::ofstream out("res/Graphs/" + filename);

    if(!out.good()) return false;

    std::string contents;
    for (int i = 0; i < fileWriteLines.size(); i++)
    {
        contents += fileWriteLines[i];
        if(i + 1 < fileWriteLines.size()) contents += "\n";
    }

    out << contents;

    out.close();

    return true;
}

void MenuGUI::open_file(std::string filename)
{
    while (equationList->EquationCount() >= 1)
	{
        equationList->RemoveEquation(0);
	}
    variableList->Clear();

    read_file_contents(filename);

    Equation* tempEquation = new Equation(equationList, variableList);
    Variable* tempFunctionVariable = new Variable();
    Variable* tempVariable = new Variable();
    
    std::string line;
    std::string mode;
    std::vector<std::string> tokens;
    int phase = -2;
    for (int i = 0; i < fileReadLines.size(); i++)
    {
        line = fileReadLines[i];

        if (line == "[Equation]")
        {
            if (phase != -2)
            {
                phase = -1;
                i -= 2; // Will become i - 1 on the next frame, where phase = -1 will be run. Then, when that phase is done, will become i and see this
                continue;
            }
            mode = "Equation";
            phase = 0;
            continue;
        }
        if (line == "[Variable]")
        {
            if (phase != -2)
            {
                phase = -1;
                i -= 2; // same note as above
                continue;
            }
            mode = "Variable";
            phase = 0;
            continue;
        }

        if (mode == "Equation")
        {
            Equation* newEquation;
            Variable* newFunctionVariable;
            int tmpInt;
            switch (phase)
            {
                case -1:
                    if (!isValidEquation(tempEquation, tempFunctionVariable))
                    {
                        std::cout << "Not valid for some reason.\n";
                        break;
                    }
                    newEquation = new Equation(equationList, variableList);
                    newEquation->setFunctionName(tempEquation->functionName);
                    newEquation->SetFormula(tempEquation->formula);
                    newEquation->equationType = tempEquation->equationType;
                    std::cout << newEquation->equationType << " is the equationType for this equation!\n";
                    newEquation->derivativeOrder = tempEquation->derivativeOrder;
                    newEquation->InitializeCurve(graphManager->vertexShader, graphManager->fragmentShader,
                                                 graphManager->x, graphManager->y,
                                                 graphManager->zoomX, graphManager->zoomY);
                    newEquation->drawCurve = tempEquation->drawCurve;
                    for (int i = 0; i < 3; i++)
                    {
                        newEquation->color[i] = tempEquation->color[i];
                    }
                    equationList->AddEquation(newEquation);

                    variableList->renameFunctionVariable("", tempFunctionVariable->nameChar);
                    newFunctionVariable = variableList->getFunctionVariable(tempFunctionVariable->nameChar);
                    if (newFunctionVariable == nullptr)
                    {
                        std::cout << "New function variable couldn't be initialized for some reason.\n";
                        equationList->RemoveEquation(equationList->EquationCount() - 1);
                        phase = -2;
                        break;
                    }
                    for (int i = 0; i < 10; i++)
                    {
                        newFunctionVariable->initialValues[i] = tempFunctionVariable->initialValues[i];
                    }
                    for (int i = 0; i < 30; i++)
                    {
                        newFunctionVariable->initialValueSettings[i] = tempFunctionVariable->initialValueSettings[i];
                    }
                    phase = -2;
                    break;
                    
                case 0:
                    // Function Name
                    if(line.length() >= 1 && line.length() <= VARIABLE_MAX_NAME_LENGTH)
                    {
                        tempEquation->functionName = line;
                        tempEquation->generateCharFromString(tempEquation->functionName, tempEquation->functionNameChar, VARIABLE_MAX_NAME_LENGTH);

                        tempFunctionVariable->renameVariable(line);
                    }
                    else
                    {
                        std::cout << "Couldn't load functionName: " << line << std::endl;
                    }
                    phase++;
                    break;
                case 1:
                    // Formula
                    if (line.length() <= EQUATION_MAX_LENGTH)
                    {
                        tempEquation->formula = line;
                        tempEquation->generateCharFromString(tempEquation->formula, tempEquation->formulaChar, EQUATION_MAX_LENGTH);
                    }
                    else
                    {
                        std::cout << "Couldn't load formula: " << line << std::endl;
                    }
                    phase++;
                    break;
                case 2:
                    // EquationType
                    tmpInt = std::stoi(line);
                    switch (tmpInt)
                    {
                        case 0:
                            tempEquation->equationType = Equation::FIRST_ORDER;
                            break;
                        case 1:
                            tempEquation->equationType = Equation::SECOND_ORDER;
                            break;
                        case 2:
                            tempEquation->equationType = Equation::MULTI_ORDER;
                            break;
                        case 3:
                            std::cout << "Loaded a NORMAL EQUATION!!\n";
                            tempEquation->equationType = Equation::NORMAL_EQUATION;
                            break;
                        default:
                            std::cout << "Couldn't load equation type; defaulting to FIRST_ORDER..." << std::endl;
                            tempEquation->equationType = Equation::FIRST_ORDER;
                            break;
                    }
                    phase++;
                    break;
                case 3:
                    // derivativeOrder
                    tmpInt = std::stoi(line);
                    if(tmpInt >= 0 && tmpInt <= 10) tempEquation->derivativeOrder = tmpInt;
                    else
                    {
                        std::cout << "derivativeOrder not found for line " << line << " and int " << tmpInt << "\n";
                    }
                    phase++;
                    break;
                case 4:
                    // drawCurve
                    tempEquation->drawCurve = (line == "1");
                    std::cout << "drawCurve = " << tempEquation->drawCurve << "\n";
                    phase++;
                    break;
                case 5:
                    // Color
                    tokens.clear();
                    split(&tokens, line, ',');
                    for (int i = 0; i < tokens.size(); i++)
                    {
                        if (i >= 3)
                        {
                            std::cout << "More than 3 color values were found.\n";
                            break;
                        }
                        tempEquation->color[i] = std::stof(tokens[i]);
                        std::cout << "Setting tempEquation->color[" << i << "] to " << tempEquation->color[i] << "\n";
                    }
                    phase++;
                    break;
                case 6:
                    // Initial Values
                    tokens.clear();
                    split(&tokens, line, ',');
                    for (int i = 0; i < tokens.size(); i++)
                    {
                        if (i >= 10)
                        {
                            std::cout << "More than 10 initial values were found.\n";
                            break;
                        }
                        tempFunctionVariable->initialValues[i] = std::stof(tokens[i]);
                        std::cout << "Setting initialValues[" << i << "] to " << tempFunctionVariable->initialValues[i] << "\n";
                    }
                    phase++;
                    break;
                case 7:
                    // Initial Value Settings
                    tokens.clear();
                    split(&tokens, line, ',');
                    for (int i = 0; i < tokens.size(); i++)
                    {
                        if (i >= 30)
                        {
                            std::cout << "More than 30 initial value settings were found.\n";
                            break;
                        }
                        tempFunctionVariable->initialValueSettings[i] = std::stof(tokens[i]);
                        std::cout << "Setting initialValueSettings[" << i << "] to " << tempFunctionVariable->initialValueSettings[i] << "\n";
                    }
                    phase++;
                    break;
                case 8:
                    // Collapse
                    tempEquation->collapse = (line == "1");
                    std::cout << "collapse = " << tempEquation->collapse << "\n";
                    phase = -1;
                    break;
                default:
                    if(phase != -2) std::cout << "Uh oh, something went wrong.\n";
                    break;
            }
        }
        if (mode == "Variable")
        {
            Variable* newVariable;
            switch (phase)
            {
                case -1:
                    if (!isValidVariable(tempVariable))
                    {
                        std::cout << "Not valid for some reason.\n";
                        break;
                    }

                    variableList->setVariable(tempVariable->nameChar, tempVariable->value);
                    newVariable = variableList->getVariable(tempVariable->nameChar);
                    if (newVariable == nullptr)
                    {
                        std::cout << "New variable couldn't be initialized for some reason.\n";
                        phase = -2;
                        break;
                    }

                    for (int i = 0; i < 3; i++)
                    {
                        newVariable->settings[i] = tempVariable->settings[i];
                    }
                    phase = -2;
                    break;
                case 0:
                    if (line.length() >= 1 && line.length() <= VARIABLE_MAX_NAME_LENGTH)
                    {
                        tempVariable->renameVariable(line);
                        std::cout << "Variable name: " << line << "\n";
                    }
                    else
                    {
                        std::cout << "Failed to set variable name " << line << ". It wasn't 1-4 characters long.\n";
                    }
                    phase++;
                    break;
                case 1:
                    tempVariable->value = std::stof(line);
                    std::cout << "Set temp variable value to " << tempVariable->value << "\n";
                    phase++;
                    break;
                case 2:
                    tokens.clear();
                    split(&tokens, line, ',');
                    for (int i = 0; i < tokens.size(); i++)
                    {
                        if (i >= 3)
                        {
                            std::cout << "More than 3 variable settings values were found.\n";
                            break;
                        }
                        tempVariable->settings[i] = std::stof(tokens[i]);
                        std::cout << "Setting value for settings[" << i << "] to " << tempVariable->settings[i] << "\n";
                    }
                    phase = -1;
                    if(i + 1 == fileReadLines.size()) fileReadLines.push_back("\n");
                    break;
            }
        }
    }
    // Compile everything at the end

	for (int i = 0; i < equationList->EquationCount(); i++)
	{
		Equation* equation = equationList->GetEquation(i);

        std::cout << "equationType = " << equation->equationType << "\n";

		equation->Compile();
	}

	// Redraw curves
	graphManager->redrawCurves();

    tempEquation->Delete();
    delete tempEquation;
    delete tempFunctionVariable;
    delete tempVariable;

    make_file_recent(filename);
    
    write_config();
}

bool MenuGUI::isValidEquation(Equation* equation, Variable* functionVariable)
{
    if(equation == nullptr || functionVariable == nullptr) return false;
    if (equation->functionName == "" || functionVariable->nameChar == "") return false;
    if(variableList->functionVariableList.find(functionVariable->nameChar) != variableList->functionVariableList.end()) return false;
    return true;
}

bool MenuGUI::isValidVariable(Variable* variable)
{
    if (variable == nullptr) return false;
    if (variable->nameChar == "") return false;
    return true;
}

bool MenuGUI::save_file(std::string filename)
{
    fileWriteLines.clear();
    for (int i = 0; i < equationList->EquationCount(); i++)
    {
        Equation* equation = equationList->GetEquation(i);
        if(!equation->isValidEquation()) continue;

        fileWriteLines.push_back("[Equation]");
        fileWriteLines.push_back(equation->functionName);
        fileWriteLines.push_back(equation->formula);

        fileWriteLines.push_back(std::to_string(equation->equationType));
        fileWriteLines.push_back(std::to_string(equation->derivativeOrder));

        fileWriteLines.push_back(equation->drawCurve ? "1" : "0");
        fileWriteLines.push_back(
            std::to_string(equation->color[0]) + "," +
            std::to_string(equation->color[1]) + "," +
            std::to_string(equation->color[2])
        );

        Variable* functionVariable = variableList->getFunctionVariable(equation->functionName);
        std::string tmpStr;
        for (int j = 0; j < 10; j++)
        {
            tmpStr += std::to_string(functionVariable->initialValues[j]);
            if(j < 9) tmpStr += ",";
        }
        fileWriteLines.push_back(tmpStr);
        tmpStr = "";
        for (int j = 0; j < 30; j++)
        {
            tmpStr += std::to_string(functionVariable->initialValueSettings[j]);
            if(j < 29) tmpStr += ",";
        }
        fileWriteLines.push_back(tmpStr);

        fileWriteLines.push_back("\n");
    }
    for (int i = 0; i < variableList->VariableCount(); i++)
    {
        Variable* variable = variableList->getVariable(variableList->getVariableNameStr(i));
        if (variable == nullptr)
        {
            std::cout << "Couldn't save variable with name " << variableList->getVariableNameStr(i) << " for some reason.\n";
            continue;
        }

        fileWriteLines.push_back("[Variable]");
        
        fileWriteLines.push_back(variable->nameChar);
        fileWriteLines.push_back(std::to_string(variable->value));
        fileWriteLines.push_back(
            std::to_string(variable->settings[0]) + "," +
            std::to_string(variable->settings[1]) + "," +
            std::to_string(variable->settings[2])
        );
    }

    bool success = write_to_file(filename);
    if(!success) return false;

    make_file_recent(filename);
    
    write_config();
    return true;
}

void MenuGUI::split(std::vector<std::string>* vec, std::string str, char regex)
{
    std::string tmpStr;
    for (int i = 0; i < str.length(); i++)
    {
        if(str[i] == regex && tmpStr != "")
        {
            vec->push_back(tmpStr);
            tmpStr = "";
        }
        else tmpStr += str[i];
    }
    if(tmpStr != "") vec->push_back(tmpStr);
}

void MenuGUI::make_file_recent(std::string filename)
{
    for (int i = 0; i < recentFiles.size(); i++)
    {
        if (recentFiles[i] == filename)
        {
            if(i == 0) return;

            recentFiles.erase(recentFiles.begin() + i);
            recentFiles.insert(recentFiles.begin(), filename);
            return;
        }
        if (i + 1 == recentFiles.size())
        {
            if(recentFiles.size() >= RECENT_FILES_MAX_COUNT) recentFiles.erase(recentFiles.begin() + i);
            recentFiles.insert(recentFiles.begin(), filename);
            return;
        }
    }
}

void MenuGUI::read_config_recent()
{
    std::cout << "Reading config's recent files...";

    recentFiles.clear();
    std::string line;
    std::ifstream in("res/config/config.txt");

    bool startReading = false;
    while (std::getline(in, line))
    {
        if (line == "[Recent]")
        {
            std::cout << "Recent worked.\n";
            startReading = true;
        }
        else if(line[0] == '[') break;
        if(!startReading) continue;

        std::ifstream f("res/Graphs/" + line);
        if (!f.good())
        {
            f.close();
            continue;
        }

        recentFiles.push_back(line);
    }

    in.close();
}

void MenuGUI::write_config()
{
    std::ofstream out("res/config/config.txt");

    out << "[Recent]" << std::endl;
    for (int i = 0; i < recentFiles.size(); i++)
    {
        out << recentFiles[i] << std::endl;
    }
    out << "\n";

    out.close();
}