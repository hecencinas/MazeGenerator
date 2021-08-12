// Fill out your copyright notice in the Description page of Project Settings.

#include "Grid.h"
#include <algorithm>

void Grid::Init(int width, int height, bool isConneccted)
{
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

void Grid::Connect(const std::shared_ptr<Cell> FirstCell, const std::shared_ptr<Cell> SecondCell)
{
    FirstCell->ConnectedCells.insert(SecondCell);
    SecondCell->ConnectedCells.insert(FirstCell);
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

void Grid::Disconnect(const std::shared_ptr<Cell> First, const std::shared_ptr<Cell> Second)
{
    First->ConnectedCells.erase(Second);
    Second->ConnectedCells.erase(First);
}

bool Grid::AreConnected(Point First, Point Second)
{

    std::set<std::shared_ptr<Cell>, std::owner_less<std::shared_ptr<Cell>>>::iterator it;
    for (it = GridCells[First.X][First.Y]->ConnectedCells.cbegin(); it != GridCells[First.X][First.Y]->ConnectedCells.cend(); ++it)
    {
        if((*it)->CellPoint.X == Second.X && (*it)->CellPoint.Y == Second.Y) return true;
    }

    return false;
}

bool Grid::AreConnected(const std::shared_ptr<Cell> First, const std::shared_ptr<Cell> Second)
{
    return AreConnected(First->CellPoint, Second->CellPoint);
}
