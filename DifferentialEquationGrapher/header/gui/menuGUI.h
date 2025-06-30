#ifndef MENU_GUI_CLASS_H
#define MENU_GUI_CLASS_H

#include "calculator/calculatorDefs.h"
#include "grapher/graphManager.h"

#include "imgui/imgui.h"

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <cerrno>
#include <vector>

//#include "plugins/ImFileBrowser.h"

const int RECENT_FILES_MAX_COUNT = 3;

class MenuGUI
{
	public:
		MenuGUI(EquationList* equationList, VariableList* variableList, GraphManager* graphManager);
		void setEquationShaders(GLuint vertexShader, GLuint fragmentShader);
		void construct(int width, int height);
	private:
		void construct_popup_openfile();
		void construct_popup_savefile();

		void read_file_contents(std::string filename);
		bool write_to_file(std::string filename);

		bool isValidEquation(Equation* equation, Variable* functionVariable);
		bool isValidVariable(Variable* variable);
		
		void open_file(std::string filename);
		bool save_file(std::string filename);

		void split(std::vector<std::string>* vec, std::string str, char regex);

		void read_config_recent();
		void write_config();

		EquationList* equationList;
		VariableList* variableList;
		GraphManager* graphManager;

		std::vector<std::string> fileReadLines;
		std::vector<std::string> fileWriteLines;
		std::vector<std::filesystem::path> filesCurrentlyOpen;
		std::vector<std::string> recentFiles;

		bool* keepWindowActive;

		bool fileOpenPopup = false;
		bool fileSavePopup = false;
		int selectedFileID = -1;
		std::string tmpFileName;
		std::string currentFileName;

		bool doStuff = false;

		//ImGui::FileBrowser mFileDialog;

		//std::string mCurrentFile;
};



#endif