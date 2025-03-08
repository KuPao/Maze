#pragma once
#include "LineSeg.h"
#include <cmath>
static double   rad(double num) { return num / 180.0 * 3.14159; };
static double   deg(double num) { return num * 180.0 / 3.14159; };
static float   rad(float num) { return (float)(num / 180.0 * 3.14159); };
static float   deg(float num) { return (float)(num * 180.0 / 3.14159); };
class Frustum {
public:
	LineSeg edgelines[2];
	Frustum(float viewangle, float fov, float pos_x, float pos_y)
	{
		edgelines[0] = LineSeg(pos_x, pos_y, pos_x + 1 * cos(rad(viewangle - fov / 2)), pos_y + 1 * sin(rad(viewangle - fov / 2)));
		edgelines[0].end[0] += (edgelines[0].end[0] - edgelines[0].start[0]) * 100;
		edgelines[0].end[1] += (edgelines[0].end[1] - edgelines[0].start[1]) * 100;
		edgelines[1] = LineSeg(pos_x, pos_y, pos_x + 1 * cos(rad(viewangle + fov / 2)), pos_y + 1 * sin(rad(viewangle + fov / 2)));
		edgelines[1].end[0] += (edgelines[1].end[0] - edgelines[1].start[0]) * 100;
		edgelines[1].end[1] += (edgelines[1].end[1] - edgelines[1].start[1]) * 100;
	}
	Frustum(float start_x, float start_y, float right_end_x, float right_end_y, float left_end_x, float left_end_y)
	{
		edgelines[0] = LineSeg(
			start_x, start_y, right_end_x, right_end_y);
		edgelines[0].end[0] +=
			(edgelines[0].end[0] - edgelines[0].start[0]) * 100;
		edgelines[0].end[1] +=
			(edgelines[0].end[1] - edgelines[0].start[1]) * 100;
		edgelines[1] = LineSeg(start_x, start_y, left_end_x, left_end_y);
		edgelines[1].end[0] +=
			(edgelines[1].end[0] - edgelines[1].start[0]) * 100;
		edgelines[1].end[1] +=
			(edgelines[1].end[1] - edgelines[1].start[1]) * 100;
	}
};