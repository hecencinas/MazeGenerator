// Fill out your copyright notice in the Description page of Project Settings.

#include <random>
#include "RecursiveDivisionMaze.h"

void RecursiveDivisionMaze::Init(int width, int height, bool isConneccted)
{
	Grid::Init(width, height, isConneccted);

	std::mt19937 RandomInt;
	if (Seed < 1)
	{
		srand(time(NULL));
		RandomInt = std::mt19937(rand());
	}
	else
	{
		RandomInt = std::mt19937(Seed);
	}
	GenerateMaze(RandomInt, Point(0, 0), width, height, false);
}

void RecursiveDivisionMaze::GenerateMaze(std::mt19937& RandomInt, const Point& Origin, const int UsingWidth, const int UsingHeight, bool HorizontalCut)
{
	if (UsingWidth < 2 || UsingHeight < 2) return;

	int WallIdx = (HorizontalCut) ? RandomInt() % (UsingHeight - 1) : RandomInt() % (UsingWidth - 1);
	int DoorIdx = (HorizontalCut) ? RandomInt() % UsingWidth : RandomInt() % UsingHeight;

	UE_LOG(LogTemp, Warning, TEXT("HorizontalCut=%d; WallIdx=%d; DoorIdx=%d"), HorizontalCut, WallIdx, DoorIdx);

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
