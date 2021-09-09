// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <vector>
#include <set>
#include "CoreMinimal.h"
#include "Grid.generated.h"

USTRUCT()
struct ABSTRACTION_API FPoint
{
	GENERATED_USTRUCT_BODY()
public:
	FPoint(int X = 0, int Y = 0) : X(X), Y(Y) {}
	int X;
	int Y;
};

USTRUCT()
struct ABSTRACTION_API FCell
{
	GENERATED_USTRUCT_BODY()
public:
	FCell() : CellPoint(0, 0) {}
	FCell(FPoint CellPoint) : CellPoint(CellPoint) {}
	FPoint CellPoint;
	TArray<TSharedRef<FCell>> ConnectedCells;
};

DECLARE_DYNAMIC_DELEGATE_TwoParams(FUpdateGridConnection, FPoint, FirstPoint, FPoint, SecondPoint);

USTRUCT()
struct ABSTRACTION_API FGrid
{
	GENERATED_USTRUCT_BODY()
public:
	FGrid() { Init(0, 0, true, true); }
	FGrid(int Width, int Height, bool IsConnected = true, bool InitAsMaze = true) { Init(Width, Height, IsConnected, InitAsMaze); }

	bool AreConnected(FPoint first, FPoint Second);
	bool AreConnected(TSharedRef<FCell> First, TSharedRef<FCell> Second);

	FUpdateGridConnection OnUpdateGridConnection;

	TArray<TArray<TSharedPtr<FCell>>> GridCells;
	
	static FGrid* GetNewGrid(int InWidth, int InHeight, bool IsConnected = true);
	static FGrid* GetNewMazeGrid(int InWidth, int InHeight);

	void GenerateMaze();
	void GenerateMaze(const FPoint& Origin, const int UsingWidth, const int UsingHeight, bool HorizontalCut = false);

protected:
	virtual void Init(int Width, int Height, bool IsConnected, bool InitAsMaze = true);
	virtual void Connect(const TSharedRef<FCell> FirstCell, const TSharedRef<FCell> SecondCell);
	virtual void Connect(const TSharedRef<FCell> Root, const TArray<TSharedRef<FCell>> &Neighbours);
	virtual void Disconnect(const TSharedRef<FCell> first, const TSharedRef<FCell> second);
	virtual void DisconnectRow(const FPoint& Origin, const int Idx, const int Width, const int PathIdx);
	virtual void DisconnectCol(const FPoint& Origin, const int Idx, const int Height, const int PathIdx);

	int Width;
	int Height;
};


// GRID

class ABSTRACTION_API Grid
{
public:
	Grid(int Width, int Height, bool IsConnected = true, bool InitAsMaze = true) { Init(Width, Height, IsConnected, InitAsMaze); }

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

	FUpdateGridConnection OnUpdateGridConnection;

	std::vector<std::vector<std::shared_ptr<Cell>>> GridCells;

	static Grid* GetNewGrid(int Width, int Height, bool IsConnected = true);
	static Grid* GetNewMazeGrid(int Width, int Height);

	void GenerateMaze(const Point& Origin, const int UsingWidth, const int UsingHeight, bool HorizontalCut = false);

protected:
	virtual void Init(int Width, int Height, bool IsConnected, bool InitAsMaze = true);
	void Connect(const std::shared_ptr<Cell> FirstCell, const std::shared_ptr<Cell> SecondCell);
	void Connect(const std::shared_ptr<Cell> root, const std::vector<std::shared_ptr<Cell>>& neighbours);
	void Disconnect(const std::shared_ptr<Cell> first, const std::shared_ptr<Cell> second);
	void DisconnectRow(const Point& origin, const int idx, const int width, const int pathIdx);
	void DisconnectCol(const Point& origin, const int idx, const int height, const int pathIdx);

	int Width;
	int Height;

	static int MazeIterations;
};
int Grid::MazeIterations = 0;
