// Fill out your copyright notice in the Description page of Project Settings.

#include "RecursiveDivisionMaze.h"
#include <random>
#include "Random.h"

void RecursiveDivisionMaze::Init(int width, int height, bool isConneccted, bool InitAsMaze)
{
	Grid::Init(width, height, isConneccted, InitAsMaze);

	std::mt19937 RandomInt;

	GenerateMaze(RandomInt, Point(0, 0), width, height, false);
}

void RecursiveDivisionMaze::GenerateMaze(std::mt19937& RandomInt, const Point& Origin, const int UsingWidth, const int UsingHeight, bool HorizontalCut)
{
	if (UsingWidth < 2 || UsingHeight < 2) return;

	int WallIdx = (HorizontalCut) ? Random::NextInt(UsingHeight - 1, 0) : Random::NextInt(UsingWidth - 1, 0);
	int DoorIdx = (HorizontalCut) ? Random::NextInt(UsingWidth, 0) : Random::NextInt(UsingHeight, 0);

	if (HorizontalCut)
	{
		DisconnectRow(Origin, WallIdx, UsingWidth, DoorIdx);

		GenerateMaze(RandomInt, Origin, UsingWidth, WallIdx + 1, !HorizontalCut);
		GenerateMaze(RandomInt, Point(Origin.X, Origin.Y + WallIdx + 1), UsingWidth, UsingHeight - WallIdx - 1, !HorizontalCut);
	}
	else
	{
		DisconnectCol(Origin, WallIdx, UsingHeight, DoorIdx);

		GenerateMaze(RandomInt, Origin, WallIdx + 1, UsingHeight, !HorizontalCut);
		GenerateMaze(RandomInt, Point(Origin.X + WallIdx + 1, Origin.Y), UsingWidth - WallIdx - 1, UsingHeight, !HorizontalCut);
	}
}
