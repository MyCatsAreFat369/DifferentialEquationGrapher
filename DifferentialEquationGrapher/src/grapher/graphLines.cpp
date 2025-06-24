#include <grapher/graphLines.h>

#include <future>

GraphLines::GraphLines(GLuint vertexShader, GLuint fragmentShader, float initialPosX, float initialPosY, float initialZoomX, float initialZoomY)
{
	UpdatePosition(initialPosX, initialPosY);
	UpdateZoom(initialZoomX, initialZoomY);

	// Initialize Axes
	axisX = new Line(-1.0f, 0.0f, 2.0f, 0.0f, 0.5f, 0.2f, 0.2f);
	axisX->AttachShaders(vertexShader, fragmentShader);

	axisY = new Line(0.0f, -1.0f, 0.0f, 2.0f, 0.5f, 0.2f, 0.2f);
	axisY->AttachShaders(vertexShader, fragmentShader);

	// Initialize Lines
	linesX = new Line[MAX_NUMBER_OF_LINES_PER_AXIS - 1];
	linesY = new Line[MAX_NUMBER_OF_LINES_PER_AXIS - 1];

	this->vertexShader = vertexShader, this->fragmentShader = fragmentShader;
}

void GraphLines::loadNextLines()
{
	if(allLinesLoaded()) return;
	linesX[linesLoaded] = Line(0.0f, -1.0f, 0.0f, 2.0f);
	linesY[linesLoaded] = Line(-1.0f, 0.0f, 2.0f, 0.0f);

	linesX[linesLoaded].AttachShaders(vertexShader, fragmentShader);
	linesY[linesLoaded].AttachShaders(vertexShader, fragmentShader);

	linesLoaded++;

	std::cout << "Taking a hell lot of time..." << std::endl;
}

bool GraphLines::allLinesLoaded()
{
	return linesLoaded >= linesToLoad();
}

int GraphLines::linesToLoad()
{
	return MAX_NUMBER_OF_LINES_PER_AXIS - 1;
}

void GraphLines::UpdatePosition(float x, float y)
{
	this->x = x;
	this->y = y;
}

void GraphLines::UpdateZoom(float zoomX, float zoomY)
{
	x = (zoomX / this->zoomX) * x;
	y = (zoomY / this->zoomY) * y;

	/// Checks if spacing should be decremented (for each axis)
	float numberOfLinesX;
	do 
	{
		DecrementSpacingX();
		numberOfLinesX = 2 / (spacingCoefficientX * spacingFactorX * zoomX);
	} while (numberOfLinesX < MAX_PRIMARY_LINES_PER_AXIS);
	IncrementSpacingX();
	float numberOfLinesY;
	do
	{
		DecrementSpacingY();
		numberOfLinesY = 2 / (spacingCoefficientY * spacingFactorY * zoomY);
	} while (numberOfLinesY < MAX_PRIMARY_LINES_PER_AXIS);
	IncrementSpacingY();

	/// Checks if spacing should be incremented (for each axis)
	numberOfLinesX = 2 / (spacingCoefficientX * spacingFactorX * zoomX);
	while (numberOfLinesX > MAX_PRIMARY_LINES_PER_AXIS)
	{
		IncrementSpacingX();
		numberOfLinesX = 2 / (spacingCoefficientX * spacingFactorX * zoomX);
	}
	numberOfLinesY = 2 / (spacingCoefficientY * spacingFactorY * zoomY);
	while (numberOfLinesY > MAX_PRIMARY_LINES_PER_AXIS)
	{
		IncrementSpacingY();
		numberOfLinesY = 2 / (spacingCoefficientY * spacingFactorY * zoomY);
	}

	this->zoomX = zoomX;
	this->zoomY = zoomY;
}

void GraphLines::Draw()
{
	int elementidX = 0, elementidY = 0;

	float prePos, currentPos;
	bool isBehindAxis;

	float spacingX = spacingCoefficientX * spacingFactorX * zoomX;
	int secondaryLineCountX = spacingCoefficientX == 2 ? 3 : 4;
	float spacingSmallX = spacingX / (secondaryLineCountX + 1);
	float startingPointX = fmod(x, spacingX);
	currentPos = startingPointX;
	while (currentPos > -1.0f)
	{
		currentPos -= spacingX;
	}
	isBehindAxis = (currentPos < x);
	while (currentPos < 1.0f)
	{
		prePos = currentPos;
		currentPos += spacingSmallX;
		// Draw axis if it's showing
		if (currentPos == x || (isBehindAxis != (currentPos < x)))
		{
			bool previousIsAxis = abs(x - prePos) < abs(x - currentPos);
			axisY->x = previousIsAxis ? prePos : currentPos;
			axisY->Draw();
			isBehindAxis = (currentPos < x);
			if (previousIsAxis && elementidX >= 1)
			{
				linesX[elementidX - 1].x = currentPos;
				linesX[elementidX - 1].SetColor(0.4f, 0.4f, 0.4f);
			}
			continue;
		}

		Line* line = &linesX[elementidX];
		line->x = currentPos;
		if ((elementidX + 1 + !isBehindAxis) % (secondaryLineCountX + 1) == 0)
		{
			line->SetColor(0.6f, 0.6f, 0.6f);
		}
		else
		{
			line->SetColor(0.4f, 0.4f, 0.4f);
		}
		elementidX++;

		if (elementidX >= MAX_NUMBER_OF_LINES_PER_AXIS - 1) // Breaks if exceeds the max number of lines, just in case
		{
			elementidX = MAX_NUMBER_OF_LINES_PER_AXIS - 1;
			break;
		}
	}

	for (int i = 0; i < elementidX; i++)
	{
		linesX[i].Draw();
	}

	float spacingY = spacingCoefficientY * spacingFactorY * zoomY;
	int secondaryLineCountY = spacingCoefficientY == 2 ? 3 : 4;
	float spacingSmallY = spacingY / (secondaryLineCountY + 1);
	float startingPointY = fmod(y, spacingY);
	currentPos = startingPointY;
	while (currentPos > -1.0f)
	{
		currentPos -= spacingY;
	}
	isBehindAxis = (currentPos < y);
	while (currentPos < 1.0f)
	{
		prePos = currentPos;
		currentPos += spacingSmallY;
		// Draw axis if it's showing
		if (currentPos == y || (isBehindAxis != (currentPos < y)))
		{
			bool previousIsAxis = abs(y - prePos) < abs(y - currentPos);
			axisX->y = previousIsAxis ? prePos : currentPos;
			axisX->Draw();
			isBehindAxis = (currentPos < y);
			if (previousIsAxis && elementidY >= 1)
			{
				linesY[elementidY - 1].y = currentPos;
				linesY[elementidY - 1].SetColor(0.4f, 0.4f, 0.4f);
			}
			continue;
		}

		Line* line = &linesY[elementidY];
		line->y = currentPos;
		if ((elementidY + 1 + !isBehindAxis) % (secondaryLineCountY + 1) == 0)
		{
			line->SetColor(0.6f, 0.6f, 0.6f);
		}
		else
		{
			line->SetColor(0.4f, 0.4f, 0.4f);
		}
		elementidY++;

		if (elementidY >= MAX_NUMBER_OF_LINES_PER_AXIS - 1) // Breaks if exceeds the max number of lines, just in case
		{
			elementidY = MAX_NUMBER_OF_LINES_PER_AXIS - 1;
			break;
		}
	}

	for (int i = 0; i < elementidY; i++)
	{
		linesY[i].Draw();
	}
}

void GraphLines::Delete()
{
	axisX->Delete();
	axisY->Delete();
	for (int i = 0; i < MAX_NUMBER_OF_LINES_PER_AXIS - 1; i++)
	{
		linesX[i].Delete();
		linesY[i].Delete();
	}

	delete axisX;
	delete axisY;
	delete[] linesX;
	delete[] linesY;
}

void GraphLines::IncrementSpacingX() { IncrementSpacing(&spacingCoefficientX, &spacingFactorX); }
void GraphLines::IncrementSpacingY() { IncrementSpacing(&spacingCoefficientY, &spacingFactorY); }
void GraphLines::IncrementSpacing(int* spacingCoefficient, float* spacingFactor)
{
	switch (*spacingCoefficient)
	{
		case 1:
			*spacingCoefficient = 2;
			return;
		case 2:
			*spacingCoefficient = 5;
			return;
		case 5:
			*spacingCoefficient = 1;
			*spacingFactor *= 10;
			return;
		default:
			return;
	}
}

void GraphLines::DecrementSpacingX() { DecrementSpacing(&spacingCoefficientX, &spacingFactorX); }
void GraphLines::DecrementSpacingY() { DecrementSpacing(&spacingCoefficientY, &spacingFactorY); }
void GraphLines::DecrementSpacing(int* spacingCoefficient, float* spacingFactor)
{
	switch (*spacingCoefficient)
	{
		case 1:
			*spacingCoefficient = 5;
			*spacingFactor /= 10;
			return;
		case 2:
			*spacingCoefficient = 1;
			return;
		case 5:
			*spacingCoefficient = 2;
			return;
		default:
			return;
	}
}
