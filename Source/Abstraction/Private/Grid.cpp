// Fill out your copyright notice in the Description page of Project Settings.

#include "Grid.h"
#include <algorithm>
#include "Random.h"

// FGRID
void FGrid::Init(int InWidth, int InHeight, bool IsConnected, bool InitAsMaze)
{
	Width = InWidth;
	Height = InHeight;
	
	if (InitAsMaze) IsConnected = true;

	// Generate the Grid
	GridCells.Empty(InWidth);
	for (int X = 0; X < InWidth; ++X)
	{
		GridCells.Add(*(new TArray<TSharedPtr<FCell>>()));
		for (int Y = 0; Y < InHeight; ++Y)
		{
			this->GridCells[X].Add(TSharedPtr<FCell>(new FCell(*(new FPoint(X, Y)))));

			// Connect West
			if (IsConnected && X > 0) this->Connect(this->GridCells[X][Y].ToSharedRef(), this->GridCells[X - 1][Y].ToSharedRef());
			// Connect North
			if (IsConnected && Y > 0) this->Connect(this->GridCells[X][Y].ToSharedRef(), this->GridCells[X][Y - 1].ToSharedRef());
		}
	}
}

void FGrid::Connect(const TSharedRef<FCell> Root, const TArray<TSharedRef<FCell>>& Neighbours)
{
	if (Neighbours.Num() < 1) return;

	for (auto it = Neighbours.begin(); it != Neighbours.end(); ++it)
	{
		if (!Root->ConnectedCells.Contains(*it))
		{
			Root->ConnectedCells.Add(*it);
			(*it)->ConnectedCells.Add(Root);

			OnUpdateGridConnection.ExecuteIfBound(Root->CellPoint, (*it)->CellPoint);
		}
	}
}

void FGrid::Connect(const TSharedRef<FCell> FirstCell, const TSharedRef<FCell> SecondCell)
{
	if (!FirstCell->ConnectedCells.Contains(SecondCell))
	{
		FirstCell->ConnectedCells.Add(SecondCell);
		SecondCell->ConnectedCells.Add(FirstCell);

		OnUpdateGridConnection.ExecuteIfBound(FirstCell->CellPoint, SecondCell->CellPoint);
	}
}

void FGrid::Disconnect(const TSharedRef<FCell> FirstCell, const TSharedRef<FCell> SecondCell)
{
	if (FirstCell->ConnectedCells.Contains(SecondCell))
	{
		FirstCell->ConnectedCells.Remove(SecondCell);
		SecondCell->ConnectedCells.Remove(FirstCell);

		OnUpdateGridConnection.ExecuteIfBound(FirstCell->CellPoint, SecondCell->CellPoint);
	}
}

void FGrid::DisconnectCol(const FPoint& Origin, const int Idx, const int InHeight, const int PathIdx)
{
	if (Origin.X + Idx >= this->Width - 1 || Origin.Y >= this->Height)
		return;

	for (int Y = 0; Y < std::min(InHeight, this->Height - Origin.Y); ++Y)
	{
		if (Y == PathIdx)
			continue;

		this->Disconnect(this->GridCells[Origin.X + Idx][Origin.Y + Y].ToSharedRef(),
			this->GridCells[Origin.X + Idx + 1][Origin.Y + Y].ToSharedRef());
	}
}

void FGrid::DisconnectRow(const FPoint& Origin, const int Idx, const int InWidth, const int PathIdx)
{
	if (Origin.Y + Idx >= this->Height - 1 || Origin.X >= this->Width)
		return;

	for (int X = 0; X < std::min(InWidth, this->Width - Origin.X); ++X)
	{
		if (X == PathIdx)
			continue;

		this->Disconnect(this->GridCells[Origin.X + X][Origin.Y + Idx].ToSharedRef(),
			this->GridCells[Origin.X + X][Origin.Y + Idx + 1].ToSharedRef());
	}
}

bool FGrid::AreConnected(FPoint First, FPoint Second)
{
	for (auto ConstIt = GridCells[First.X][First.Y]->ConnectedCells.CreateConstIterator(); ConstIt; ++ConstIt)
	{
		if (ConstIt->Get().CellPoint.X == Second.X && ConstIt->Get().CellPoint.Y) return true;
	}

	return false;
}

bool FGrid::AreConnected(const TSharedRef<FCell> First, const TSharedRef<FCell> Second)
{
	return First->ConnectedCells.Contains(Second);
}

FGrid* FGrid::GetNewGrid(int Width, int Height, bool IsConnected)
{
	return new FGrid(Width, Height, IsConnected);
}

FGrid* FGrid::GetNewMazeGrid(int Width, int Height)
{
	FGrid* NewGrid = new FGrid(Width, Height, true, true);
	NewGrid->GenerateMaze(FPoint(0, 0), Width, Height, true);
	return NewGrid;
}

void FGrid::GenerateMaze()
{
	GenerateMaze(FPoint(0, 0), Width, Height, true);
}

void FGrid::GenerateMaze(const FPoint& Origin, const int UsingWidth, const int UsingHeight, bool HorizontalCut)
{
	if (UsingWidth < 2 || UsingHeight < 2) return;

	int WallIdx = (HorizontalCut) ? Random::NextInt(UsingHeight - 1, 0) : Random::NextInt(UsingWidth - 1, 0);
	int DoorIdx = (HorizontalCut) ? Random::NextInt(UsingWidth, 0) : Random::NextInt(UsingHeight, 0);

	if (HorizontalCut)
	{
		DisconnectRow(Origin, WallIdx, UsingWidth, DoorIdx);

		GenerateMaze(Origin, UsingWidth, WallIdx + 1, !HorizontalCut);
		GenerateMaze(FPoint(Origin.X, Origin.Y + WallIdx + 1), UsingWidth, UsingHeight - WallIdx - 1, !HorizontalCut);
	}
	else
	{
		DisconnectCol(Origin, WallIdx, UsingHeight, DoorIdx);

		GenerateMaze(Origin, WallIdx + 1, UsingHeight, !HorizontalCut);
		GenerateMaze(FPoint(Origin.X + WallIdx + 1, Origin.Y), UsingWidth - WallIdx - 1, UsingHeight, !HorizontalCut);
	}
}


// GRID

void Grid::Init(int width, int height, bool isConneccted, bool InitAsMaze)
{
	if (InitAsMaze) isConneccted = true;

	// Generate the Grid
	this->GridCells.resize(width);
	for (int X = 0; X < width; ++X)
	{
		this->GridCells[X].reserve(height);
		for (int Y = 0; Y < height; ++Y)
		{
			this->GridCells[X].push_back(std::shared_ptr<Cell>(new Cell(*(new Point(X, Y)))));

			// Connect West
			if (isConneccted && X > 0) this->Connect(this->GridCells[X][Y], this->GridCells[X - 1][Y]);
			// Connect North
			if (isConneccted && Y > 0) this->Connect(this->GridCells[X][Y], this->GridCells[X][Y - 1]);
		}
	}
}

void Grid::Connect(const std::shared_ptr<Cell> Root, const std::vector<std::shared_ptr<Cell>>& Neighbours)
{
	if (Neighbours.size() < 1) return;

	for (auto it = Neighbours.begin(); it != Neighbours.end(); ++it)
	{
		Root->ConnectedCells.insert(*it);
		(*it)->ConnectedCells.insert(Root);
	}
}

void Grid::Connect(const std::shared_ptr<Cell> FirstCell, const std::shared_ptr<Cell> SecondCell)
{
	FirstCell->ConnectedCells.insert(SecondCell);
	SecondCell->ConnectedCells.insert(FirstCell);

	//if (OnUpdateGridConnection.IsBound()) OnUpdateGridConnection.Broadcast(FirstCell->CellPoint, SecondCell->CellPoint);
}

void Grid::Disconnect(const std::shared_ptr<Cell> FirstCell, const std::shared_ptr<Cell> SecondCell)
{
	FirstCell->ConnectedCells.erase(SecondCell);
	SecondCell->ConnectedCells.erase(FirstCell);

	//if(OnUpdateGridConnection.IsBound()) OnUpdateGridConnection.Broadcast(FirstCell->CellPoint, SecondCell->CellPoint);
}

void Grid::DisconnectCol(const Point& Origin, const int Idx, const int height, const int PathIdx)
{
	if (Origin.X + Idx >= this->Width - 1 || Origin.Y >= this->Height)
		return;

	for (int Y = 0; Y < std::min(height, this->Height - Origin.Y); ++Y)
	{
		if (Y == PathIdx)
			continue;

		this->Disconnect(this->GridCells[Origin.X + Idx][Origin.Y + Y],
			this->GridCells[Origin.X + Idx + 1][Origin.Y + Y]);
	}
}

void Grid::DisconnectRow(const Point& Origin, const int Idx, const int width, const int PathIdx)
{
	if (Origin.Y + Idx >= this->Height - 1 || Origin.X >= this->Width)
		return;

	for (int X = 0; X < std::min(width, this->Width - Origin.X); ++X)
	{
		if (X == PathIdx)
			continue;

		this->Disconnect(this->GridCells[Origin.X + X][Origin.Y + Idx],
			this->GridCells[Origin.X + X][Origin.Y + Idx + 1]);
	}
}

bool Grid::AreConnected(Point First, Point Second)
{

	std::set<std::shared_ptr<Cell>, std::owner_less<std::shared_ptr<Cell>>>::iterator it;
	for (it = GridCells[First.X][First.Y]->ConnectedCells.cbegin(); it != GridCells[First.X][First.Y]->ConnectedCells.cend(); ++it)
	{
		if ((*it)->CellPoint.X == Second.X && (*it)->CellPoint.Y == Second.Y) return true;
	}

	return false;
}

bool Grid::AreConnected(const std::shared_ptr<Cell> First, const std::shared_ptr<Cell> Second)
{
	return AreConnected(First->CellPoint, Second->CellPoint);
}

Grid* Grid::GetNewGrid(int Width, int Height, bool IsConnected)
{
	return new Grid(Width, Height, IsConnected);
}

Grid* Grid::GetNewMazeGrid(int Width, int Height)
{
	MazeIterations = 0;
	Grid* NewGrid = new Grid(Width, Height, true, true);
	//NewGrid->GenerateMaze(Point(0, 0), Width, Height, false);
	return NewGrid;
}

void Grid::GenerateMaze(const Point& Origin, const int UsingWidth, const int UsingHeight, bool HorizontalCut)
{
	if (UsingWidth < 2 || UsingHeight < 2) return;

	int WallIdx = (HorizontalCut) ? Random::NextInt(UsingHeight - 1, 0) : Random::NextInt(UsingWidth - 1, 0);
	int DoorIdx = (HorizontalCut) ? Random::NextInt(UsingWidth, 0) : Random::NextInt(UsingHeight, 0);

	if (HorizontalCut)
	{
		DisconnectRow(Origin, WallIdx, UsingWidth, DoorIdx);

		GenerateMaze(Origin, UsingWidth, WallIdx + 1, !HorizontalCut);
		GenerateMaze(Point(Origin.X, Origin.Y + WallIdx + 1), UsingWidth, UsingHeight - WallIdx - 1, !HorizontalCut);
	}
	else
	{
		DisconnectCol(Origin, WallIdx, UsingHeight, DoorIdx);

		GenerateMaze(Origin, WallIdx + 1, UsingHeight, !HorizontalCut);
		GenerateMaze(Point(Origin.X + WallIdx + 1, Origin.Y), UsingWidth - WallIdx - 1, UsingHeight, !HorizontalCut);
	}
}
