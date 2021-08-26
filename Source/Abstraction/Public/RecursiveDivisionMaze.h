// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Grid.h"
#include <random>
#include "CoreMinimal.h"

/**
 * 
 */
class ABSTRACTION_API RecursiveDivisionMaze : public Grid
{
public:
	RecursiveDivisionMaze(int Width, int Height, bool IsConnected = true, int Seed = 0) : Grid(Width, Height, IsConnected), Seed(Seed) { Init(Width, Height, IsConnected); }

protected:
	virtual void Init(int Width, int Height, bool IsConnected) override;

	virtual void GenerateMaze(std::mt19937& UsingRandomInt, const Point& Origin, const int UsingWidth, const int UsingHeight, bool HorizontalCut);
	int Seed;
};

