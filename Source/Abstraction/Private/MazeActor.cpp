// Fill out your copyright notice in the Description page of Project Settings.


#include "MazeActor.h"
#include "Engine/StaticMeshActor.h"

// Sets default values
AMazeActor::AMazeActor()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RootSceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
	RootComponent = RootSceneComponent;
}

// Called when the game starts or when spawned
void AMazeActor::BeginPlay()
{
	Super::BeginPlay();

	if (MazeGrid == nullptr) GenerateMaze();
}

// Called every frame
void AMazeActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AMazeActor::GenerateMaze()
{
	if (FloorMeshTemplate == nullptr || WallMeshTemplate == nullptr || DoorFramedWallMeshTemplate == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("Templates static meshes required."));
		return;
	}
	if (Width < 1 || Height < 1)
	{
		UE_LOG(LogTemp, Error, TEXT("Width and Height should be greater than 0."));
		return;
	}


	if (MazeGrid != nullptr) {} //todo: reset maze, destroy old objects or reuse then
	MazeGrid = new RecursiveDivisionMaze(Width, Height);

	if (FloorMeshes.size() > 0) for (std::vector<AActor*> FloorRow : FloorMeshes) for (AActor* MeshActor : FloorRow) if (MeshActor) MeshActor->Destroy();
	if (WallMeshes.size() > 0) for (std::vector<AActor*> WallRow : WallMeshes) for (AActor* MeshActor : WallRow) if (MeshActor) MeshActor->Destroy();

	FloorMeshes.clear();
	FloorMeshes.resize(Width);
	WallMeshes.clear();
	WallMeshes.resize(Width);


	FVector NextCellCenter = FVector::ZeroVector;

	FVector FrontWallPos = NextCellCenter;
	FrontWallPos.Y += CellSize / 2;

	FVector LeftWallPos = NextCellCenter;
	LeftWallPos.X += CellSize / 2;
	FRotator LeftWallRot = FRotator::ZeroRotator;
	LeftWallRot.Yaw += 90;

	float BeginX = NextCellCenter.X;

	for (int x = 0; x < Width; ++x, NextCellCenter.Y += CellSize)
	{
		FloorMeshes[x].resize(Height);

		if (x == 0) WallMeshes[x].resize(Height * 2 + Width + 1);
		else WallMeshes[x].resize(Height * 2 + 1);

		NextCellCenter.X = BeginX; //begin column
		for (int y = 0; y < Height; ++y, NextCellCenter.X += CellSize)
		{
			AStaticMeshActor* NewFloorMesh = GetWorld()->SpawnActor<AStaticMeshActor>(AStaticMeshActor::StaticClass(), NextCellCenter, FRotator::ZeroRotator);
			FloorMeshes[x].push_back(NewFloorMesh);
			NewFloorMesh->SetMobility(EComponentMobility::Stationary);
			UStaticMeshComponent* MeshComponent = NewFloorMesh->GetStaticMeshComponent();
			if (MeshComponent) MeshComponent->SetStaticMesh(FloorMeshTemplate);

			//determine front wall template
			auto UsingWallTemplate = DoorFramedWallMeshTemplate;
			if (x == Width - 1) UsingWallTemplate = WallMeshTemplate;
			else if (!MazeGrid->AreConnected(MazeGrid->GridCells[x][y], MazeGrid->GridCells[x + 1][y])) UsingWallTemplate = WallMeshTemplate;

			FrontWallPos = NextCellCenter;
			FrontWallPos.Y += CellSize / 2;
			AStaticMeshActor* NewFrontWallMesh = GetWorld()->SpawnActor<AStaticMeshActor>(AStaticMeshActor::StaticClass(), FrontWallPos, FRotator::ZeroRotator);
			WallMeshes[x].push_back(NewFrontWallMesh);
			NewFrontWallMesh->SetMobility(EComponentMobility::Stationary);
			UStaticMeshComponent* FrontWallMeshComponent = NewFrontWallMesh->GetStaticMeshComponent();
			if (FrontWallMeshComponent) FrontWallMeshComponent->SetStaticMesh(UsingWallTemplate);

			//determine front wall template
			UsingWallTemplate = DoorFramedWallMeshTemplate;
			if (y == Height - 1) UsingWallTemplate = WallMeshTemplate;
			else if (!MazeGrid->AreConnected(MazeGrid->GridCells[x][y], MazeGrid->GridCells[x][y + 1])) UsingWallTemplate = WallMeshTemplate;

			LeftWallPos = NextCellCenter;
			LeftWallPos.X += CellSize / 2;
			AStaticMeshActor* NewLeftWallMesh = GetWorld()->SpawnActor<AStaticMeshActor>(AStaticMeshActor::StaticClass(), LeftWallPos, LeftWallRot);
			WallMeshes[x].push_back(NewLeftWallMesh);
			NewLeftWallMesh->SetMobility(EComponentMobility::Stationary);
			UStaticMeshComponent* LeftWallMeshComponent = NewLeftWallMesh->GetStaticMeshComponent();
			if (LeftWallMeshComponent) LeftWallMeshComponent->SetStaticMesh(UsingWallTemplate);
		}
	}

	NextCellCenter = FVector::ZeroVector;
	for (int x = 0; x < Height; ++x, NextCellCenter.X += CellSize)
	{
		auto BackWallPos = NextCellCenter;
		BackWallPos.Y -= CellSize / 2;
		AStaticMeshActor* NewFrontWallMesh = GetWorld()->SpawnActor<AStaticMeshActor>(AStaticMeshActor::StaticClass(), BackWallPos, FRotator::ZeroRotator);
		WallMeshes[0].push_back(NewFrontWallMesh);
		NewFrontWallMesh->SetMobility(EComponentMobility::Stationary);
		UStaticMeshComponent* FrontWallMeshComponent = NewFrontWallMesh->GetStaticMeshComponent();
		if (FrontWallMeshComponent) FrontWallMeshComponent->SetStaticMesh(WallMeshTemplate);
	}

	NextCellCenter = FVector::ZeroVector;
	for (int y = 0; y < Width; ++y, NextCellCenter.Y += CellSize)
	{
		auto RightWallPos = NextCellCenter;
		RightWallPos.X -= CellSize / 2;
		AStaticMeshActor* NewLeftWallMesh = GetWorld()->SpawnActor<AStaticMeshActor>(AStaticMeshActor::StaticClass(), RightWallPos, LeftWallRot);
		WallMeshes[y].push_back(NewLeftWallMesh);
		NewLeftWallMesh->SetMobility(EComponentMobility::Stationary);
		UStaticMeshComponent* LeftWallMeshComponent = NewLeftWallMesh->GetStaticMeshComponent();
		if (LeftWallMeshComponent) LeftWallMeshComponent->SetStaticMesh(WallMeshTemplate);
	}
}
