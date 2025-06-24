#include <grapher/graphManager.h>

#include "grapher/calculator.h"

#include <cstring>

GraphManager::GraphManager(GLuint vertexShader, GLuint fragmentShader, Input* input,
						   float initialX, float initialY, float initialZoomX, float initialZoomY,
						   float x0, float v0)
{
	this->input = input;

	x = initialX, y = initialY;
	zoomX = initialZoomX, zoomY = initialZoomY;
	if(zoomX == 0.0f) zoomX = 1.0f; // just to be safe

	this->prex0 = x0, this->prev0 = v0;
	this->x0 = x0, this->v0 = v0;

	// Consider moving GraphLines creation before Curve
	graphLines = new GraphLines(vertexShader, fragmentShader, x, y, zoomX, zoomY);
	graphLines->UpdatePosition(x, y);

	// Initialize Curve
	float t0 = x / zoomX;
	Points* points = new Points();
	getPointsFromOrigin(points, t0, x0, v0, zoomX, zoomY);

	curve = new Curve(points, zoomX, zoomY);
	curve->AttachShaders(vertexShader, fragmentShader);
	curve->x = initialX, curve->y = initialY;

	// Initialize pre-positions for panning
	preX = x, preY = y;
	preMouseX = 0.0, preMouseY = 0.0;
	panningState = NONE;
}

void GraphManager::render(int width, int height, bool ignoreMouse)
{
	this->width = width, this->height = height;

	if(!ignoreMouse) pan();
	if(!ignoreMouse) zoom();
	redraw();

	/// Draw graphLines
	graphLines->Draw();

	/// Draw curve
	if (drawCurve)
	{
		curve->Draw(color);
	}
}

void GraphManager::Delete()
{
	graphLines->Delete();
	curve->Delete(true);

	delete graphLines;
	delete curve;
}

void GraphManager::pan()
{
	/// Pan
	// if mouse button not down, return
	if (!input->leftMouseButtonDown())
	{
		panningState = NONE;
		return;
	}
	
	// checks if clicked outside initially, then continuously returns falls if done so
	if (panningState == NONE &&
		(input->getMouseX() < 400 || input->getMouseY() < 100))
	{
		panningState = CLICKED_OUTSIDE;
	}
	if(panningState == CLICKED_OUTSIDE) return;

	// Initializes prepositions since it has clicked inside
	if (panningState == NONE)
	{
		preMouseX = input->getMouseX(), preMouseY = input->getMouseY();
		preX = x, preY = y;
		panningState = CLICKED_INSIDE;
	}

	/// Consider making graphLines the first one to get the new position, or perhaps make the position a variable in main class
	x = preX + (input->getMouseX() - preMouseX) / 800.0;
	y = preY - (input->getMouseY() - preMouseY) / 800.0;
	graphLines->UpdatePosition(x, y);
	curve->UpdatePosition(x, y);
}

void GraphManager::zoom()
{
	/// Zoom
	/// TODO: Make it so that zooming zooms to the mouse, not the center of the screen
	double mouseScrollY = input->getMouseScrollY();
	if (mouseScrollY != 0)
	{
		if (input->getKeyDown(GLFW_KEY_LEFT_SHIFT))
		{
			float scalingFactor = mouseScrollY / 20.0f;
			x += scalingFactor * curve->x;
			zoomX += scalingFactor * zoomX;
		}
		else
		{
			float scalingFactor = mouseScrollY / 20.0f;
			y += scalingFactor * curve->y;
			zoomY += scalingFactor * zoomY;
		}

		graphLines->UpdateZoom(zoomX, zoomY);
		curve->UpdatePosition(x, y);
		curve->UpdateScale(zoomX, zoomY);
	}
}

void GraphManager::redraw()
{
	// Checks if x0 and v0 variables have changed
	if (prex0 != x0 || prev0 != v0)
	{
		prex0 = x0, prev0 = v0;
		curve = redrawCurve(curve, -x / zoomX, x0, v0);
		return;
	}
	/// Check if we should draw new curve
	bool curveRedraw_isTooZoomed = ((curve->points->t0_right - curve->points->t0_left) * curve->scaleX >= 2 * CURVE_MAX_RADIUS_PER_WIDTH);
	bool curveRedraw_middleShows = (abs(curve->points->t0 * curve->scaleX + curve->x) <= 1.0f);
	bool curveRedraw_leftShows = (curve->points->t0_left * curve->scaleX + curve->x >= -1.0f);
	bool curveRedraw_rightShows = (curve->points->t0_right * curve->scaleX + curve->x <= 1.0);

	if ((curveRedraw_leftShows && curveRedraw_rightShows) ||
		(curveRedraw_isTooZoomed) ||
		(curveRedraw_middleShows && (curveRedraw_leftShows || curveRedraw_rightShows)))
	{
		curve = redrawCurve(curve, -x / zoomX, x0, v0);
	}
	else if (curveRedraw_leftShows)
	{
		curve = redrawCurve(curve, -x / zoomX, x0, v0);
	}
	else if (curveRedraw_rightShows)
	{
		std::cout << curve->points->t0 << std::endl;
		curve = redrawCurve(curve, -x / zoomX, x0, v0);
		std::cout << curve->points->t0 << std::endl;
	}
}