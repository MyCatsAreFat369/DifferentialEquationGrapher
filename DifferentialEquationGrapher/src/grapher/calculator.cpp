#include <grapher/calculator.h>

float func1(float x, float v, float time)
{
	return -100 * x - v + 10 * time + 50;
}

// position -1 means the left side of the curve came into the screen, and we'll be drawing from right to left to fill in the gap
// vice versa for position 1, where the right side comes into the screen, and we draw from left to right
// position 0 just means redraw the curve but with a new zoom (to account for zooming cases)
Curve* redrawCurve(Curve* oldCurve, int position)
{
	std::cout << "redrawing curve...\n";
	Points* oldPoints = oldCurve->points;
	float newt0, newx0, newv0;
	switch (position)
	{
	case -1:
		newt0 = oldPoints->t0_right;
		newx0 = oldPoints->x0_right;
		newv0 = oldPoints->v0_right;
		break;
	case 0:
		newt0 = oldPoints->t0;
		newx0 = oldPoints->x0;
		newv0 = oldPoints->v0;
		break;
	case 1:
		newt0 = oldPoints->t0_left;
		newx0 = oldPoints->x0_left;
		newv0 = oldPoints->v0_left;
		break;
	default:
		std::cerr << "redrawCurve was called with a position that was neither -1, 0, or 1." << std::endl;
		return oldCurve;
	}
	//getPoints(newPoints, oldPoints->t0, oldPoints->x0, oldPoints->v0, oldCurve->scaleX, oldCurve->scaleY, position);
	Points* newPoints = new Points();
	getPoints(newPoints, oldPoints->t0, oldPoints->x0, oldPoints->v0, oldCurve->scaleX, oldCurve->scaleY, position);
	Curve* newCurve = new Curve(newPoints, oldCurve->scaleX, oldCurve->scaleY);
	newCurve->AttachShaders(oldCurve->vertexShader, oldCurve->fragmentShader);
	newCurve->x = oldCurve->x, newCurve->y = oldCurve->y;

	oldCurve->Delete();
	delete oldCurve;
	return newCurve;
}

void getPoints(Points* points, float t0, float x0, float v0, float zoomX, float zoomY, int position)
{
	switch (position)
	{
		case -1:
			std::cout << "I am drawing the points from the right!!" << std::endl;
			getPointsFromRight(points, t0, x0, v0, zoomX, zoomY);
			return;
		case 0:
			std::cout << "I am drawing the points in middle!!" << std::endl;
			getPointsFromMiddle(points, t0, x0, v0, zoomX, zoomY);
			return;
		case 1:
			std::cout << "I am drawing the points from the left!!" << std::endl;
			getPointsFromLeft(points, t0, x0, v0, zoomX, zoomY);
			return;
		default:
			std::cerr << "getPoints was called with a position that was neither -1, 0 or 1." << std::endl;
			return;
	}
}

void getPointsFromLeft(Points* points, float t0, float x0, float v0, float zoomX, float zoomY) // do this when panning to the right
{
	float dt = (2 * CURVE_RADIUS_PER_WIDTH / zoomX) / (CURVE_POINTS_SIZE - 1);

	float x = x0;
	float v = v0;
	float a;
	float time = t0;
	for (int i = 0; i < CURVE_POINTS_SIZE; i++)
	{
		points->points[i * 6] = time;
		points->points[i * 6 + 1] = x;
		points->points[i * 6 + 2] = 0.0f;
		points->points[i * 6 + 3] = 1.0f;
		points->points[i * 6 + 4] = 1.0f;
		points->points[i * 6 + 5] = 1.0f;

		if (i == 0)
		{
			points->t0_left = time;
			points->x0_left = x;
			points->v0_left = v;
		}
		if (i == (CURVE_POINTS_SIZE - 1) / 2)
		{
			points->t0 = time;
			points->x0 = x;
			points->v0 = v;
		}
		if (i == CURVE_POINTS_SIZE - 1)
		{
			points->t0_right = time;
			points->x0_right = x;
			points->v0_right = v;
			return;
		}

		a = func1(x, v, time);
		v += a * dt;
		x += v * dt;
		time += dt;
	}
}

void getPointsFromMiddle(Points* points, float t0, float x0, float v0, float zoomX, float zoomY)
{
	float dt = (2 * CURVE_RADIUS_PER_WIDTH / zoomX) / (CURVE_POINTS_SIZE - 1);

	float x = x0;
	float v = v0;
	float a;
	float time = t0;
	for (int i = (CURVE_POINTS_SIZE - 1) / 2; i < CURVE_POINTS_SIZE; i++)
	{
		points->points[i * 6] = time;
		points->points[i * 6 + 1] = x;
		points->points[i * 6 + 2] = 0.0f;
		points->points[i * 6 + 3] = 1.0f;
		points->points[i * 6 + 4] = 1.0f;
		points->points[i * 6 + 5] = 1.0f;

		if (i + 1 == CURVE_POINTS_SIZE)
		{
			points->t0_right = time;
			points->x0_right = x;
			points->v0_right = v;
			break;
		}

		a = func1(x, v, time);
		v += a * dt;
		x += v * dt;
		time += dt;
	}
	x = x0;
	v = v0;
	time = t0;
	for (int i = (CURVE_POINTS_SIZE - 1) / 2; i > 0;)
	{
		i--;
		a = func1(x, v, time);
		v -= a * dt;
		x -= v * dt;
		time -= dt;

		if (i == 0)
		{
			points->t0_left = time;
			points->x0_left = x;
			points->v0_left = v;
		}

		points->points[i * 6] = time;
		points->points[i * 6 + 1] = x;
		points->points[i * 6 + 2] = 0.0f;
		points->points[i * 6 + 3] = 1.0f;
		points->points[i * 6 + 4] = 1.0f;
		points->points[i * 6 + 5] = 1.0f;
	}

	points->t0 = t0;
	points->x0 = x0;
	points->v0 = v0;
}

void getPointsFromRight(Points* points, float t0, float x0, float v0, float zoomX, float zoomY) // do this when panning to the left
{
	float dt = (2 * CURVE_RADIUS_PER_WIDTH / zoomX) / (CURVE_POINTS_SIZE - 1);

	float x = x0;
	float v = v0;
	float a;
	float time = t0;
	for (int i = CURVE_POINTS_SIZE; i > 0;)
	{
		i--;

		if (i == CURVE_POINTS_SIZE - 1)
		{
			points->t0_right = time;
			points->x0_right = x;
			points->v0_right = v;
		}
		if (i == (CURVE_POINTS_SIZE - 1) / 2)
		{
			points->t0 = time;
			points->x0 = x;
			points->v0 = v;
		}
		if (i == 0)
		{
			points->t0_left = time;
			points->x0_left = x;
			points->v0_left = v;
		}

		a = func1(x, v, time);
		v -= a * dt;
		x -= v * dt;
		time -= dt;

		points->points[i * 6] = time;
		points->points[i * 6 + 1] = x;
		points->points[i * 6 + 2] = 0.0f;
		points->points[i * 6 + 3] = 1.0f;
		points->points[i * 6 + 4] = 1.0f;
		points->points[i * 6 + 5] = 1.0f;
	}
}