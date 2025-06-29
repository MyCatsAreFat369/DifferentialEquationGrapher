#ifndef GRAPH_LINES_CLASS_H
#define GRAPH_LINES_CLASS_H

#include "grapher/line.h"
#include "rendering/textManager.h"

const int MAX_PRIMARY_LINES_PER_AXIS = 10;
const int MAX_NUMBER_OF_LINES_PER_AXIS = 5 * (MAX_PRIMARY_LINES_PER_AXIS + 1) - 1;
// comes from the formula n + 4 * (n + 1) = 5 * (n + 1) - 1
// 4 is the number of lines between primary lines

class GraphLines
{
	public:
		GraphLines(GLuint vertexShader, GLuint fragmentShader, TextManager* textManager,
				   float initialPosX, float initialPosY, float initialZoomX, float initialZoomY);

		float x = 0.0f, y = 0.0f;
		float zoomX, zoomY;

		// make the cartesian spacing 1.0, 2.0, 5.0, 10.0 etc between every line
		int spacingCoefficientX = 1, spacingCoefficientY = 1;
		float spacingFactorX = 1.0f, spacingFactorY = 1.0f;

		// Loading progress
		int linesLoaded = 0;

		void loadNextLines();
		bool allLinesLoaded();
		int linesToLoad();

		void UpdatePosition(float x, float y);
		void UpdateZoom(int width, int height, float zoomX, float zoomY);
		void Draw(int width, int height);
		void Delete();
	private:
		// axes
		Line* axisX;
		Line* axisY;

		// lines surrounding the axes
		Line* linesX;
		Line* linesY;

		GLuint vertexShader, fragmentShader;
		TextManager* textManager;

		// Increment/Decrement spacing for each axis

		void IncrementSpacingX();
		void IncrementSpacingY();
		void IncrementSpacing(int* spacingCoefficient, float* spacingFactor);

		void DecrementSpacingX();
		void DecrementSpacingY();
		void DecrementSpacing(int* spacingCoefficient, float* spacingFactor);
};


#endif