// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <vector>
#include <set>
#include "CoreMinimal.h"

/**
 *
 */
class ABSTRACTION_API Grid
{
public:
	Grid(int Width, int Height, bool IsConnected = true) { Init(Width, Height, IsConnected); }

	struct Point
	{
	public:
		Point(int X = 0, int Y = 0) : X(X), Y(Y) {}
		int X;
		int Y;
	};

	class Cell
	{
	public:
		Cell(Point CellPoint) : CellPoint(CellPoint) {}
		const Point CellPoint;
		std::set<std::shared_ptr<Cell>, std::owner_less<std::shared_ptr<Cell>>> ConnectedCells;
	};

	bool AreConnected(Point first, Point Second);
	bool AreConnected(const std::shared_ptr<Cell> First, const std::shared_ptr<Cell> Second);

	std::vector<std::vector<std::shared_ptr<Cell>>> GridCells;

protected:
	virtual void Init(int Width, int Height, bool IsConnected);
	void Connect(const std::shared_ptr<Cell> FirstCell, const std::shared_ptr<Cell> SecondCell);
	void Connect(const std::shared_ptr<Cell> root, const std::vector<std::shared_ptr<Cell>>& neighbours);
	void Disconnect(const std::shared_ptr<Cell> first, const std::shared_ptr<Cell> second);
	void DisconnectRow(const Point& origin, const int idx, const int width, const int pathIdx);
	void DisconnectCol(const Point& origin, const int idx, const int height, const int pathIdx);

	int Width;
	int Height;
};
