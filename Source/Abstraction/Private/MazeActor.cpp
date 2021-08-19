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

	if (MazeGrid == nullptr || FloorMeshes.size() <= 0 || WallMeshes.size() <= 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("Generating maze at begin play."));
		GenerateMaze();
	}
}

// Called every frame
void AMazeActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AMazeActor::GenerateMaze()
{
	if (Width < 1 || Height < 1)
	{
		UE_LOG(LogTemp, Error, TEXT("Width and Height should be greater than 0."));
		return;
	}
	MazeGrid = new RecursiveDivisionMaze(Width, Height);

	if (FloorMeshTemplatesAndChance.Num() == 0 || WallMeshTemplatesAndChance.Num() == 0 || DoorFramedWallMeshTemplatesAndChance.Num() == 0)
	{
		UE_LOG(LogTemp, Error, TEXT("Templates required."));
		return;
	}

	if (FloorMeshes.size() > 0) for (std::vector<AActor*> FloorRow : FloorMeshes) for (AActor* MeshActor : FloorRow) if (MeshActor) MeshActor->Destroy();
	if (WallMeshes.size() > 0) for (std::vector<AActor*> WallRow : WallMeshes) for (AActor* MeshActor : WallRow) if (MeshActor) MeshActor->Destroy();
	FloorMeshes.clear();
	WallMeshes.clear();

	InstantiateMaze();
}

void AMazeActor::InstantiateMaze()
{
	FloorMeshes.resize(Width);
	WallMeshes.resize(Width);


	FVector NextCellCenter = FVector::ZeroVector;
	float BeginX = NextCellCenter.X; //begin column

	FVector FrontWallPos = NextCellCenter;
	FrontWallPos.Y += CellSize / 2;

	FVector LeftWallPos = NextCellCenter;
	LeftWallPos.X += CellSize / 2;
	FRotator LeftWallRot = FRotator::ZeroRotator;
	LeftWallRot.Yaw += 90;

	srand(time(NULL));
	TArray<TSubclassOf<AActor>> FloorTemplates;
	FloorMeshTemplatesAndChance.GenerateKeyArray(FloorTemplates);
	TArray<TSubclassOf<AActor>> WallTemplates;
	WallMeshTemplatesAndChance.GenerateKeyArray(WallTemplates);
	TArray<TSubclassOf<AActor>> DoorTemplates;
	DoorFramedWallMeshTemplatesAndChance.GenerateKeyArray(DoorTemplates);

	TArray<int> FloorChances;
	FloorMeshTemplatesAndChance.GenerateValueArray(FloorChances);
	TArray<int> WallChances;
	WallMeshTemplatesAndChance.GenerateValueArray(WallChances);
	TArray<int> DoorChances;
	DoorFramedWallMeshTemplatesAndChance.GenerateValueArray(DoorChances);

	int FloorIdx = 0;
	int WallIdx = 0;
	int DoorFrameIdx = 0;
	//enforcing first templates to be default, in case all the other template reach 0 it'll be a safe point
	FloorChances[0] = -1;
	WallChances[0] = -1;
	DoorChances[0] = -1;


	for (int x = 0; x < Width; ++x, NextCellCenter.Y += CellSize)
	{
		FloorMeshes[x].resize(Height);

		if (x == 0) WallMeshes[x].resize(Height * 2 + Width + 1);
		else WallMeshes[x].resize(Height * 2 + 1);

		NextCellCenter.X = BeginX; //begin column
		for (int y = 0; y < Height; ++y, NextCellCenter.X += CellSize)
		{
			FloorIdx = rand() % FloorMeshTemplatesAndChance.Num();
			while (FloorChances[FloorIdx] == 0) FloorIdx = rand() % FloorMeshTemplatesAndChance.Num();

			WallIdx = rand() % WallMeshTemplatesAndChance.Num();
			while (WallChances[WallIdx] == 0) WallIdx = rand() % WallMeshTemplatesAndChance.Num();

			DoorFrameIdx = rand() % DoorFramedWallMeshTemplatesAndChance.Num();
			while (DoorChances[DoorFrameIdx] == 0) DoorFrameIdx = rand() % DoorFramedWallMeshTemplatesAndChance.Num();

			auto RandRot = GetRand4OrientationRotator();
			FloorMeshes[x].push_back(GetWorld()->SpawnActor<AActor>(FloorTemplates[FloorIdx], NextCellCenter, RandRot));
			FloorChances[FloorIdx]--;

			//front wall
			FrontWallPos = NextCellCenter;
			FrontWallPos.Y += CellSize / 2;
			if (x == Width - 1 || !MazeGrid->AreConnected(MazeGrid->GridCells[x][y], MazeGrid->GridCells[x + 1][y]))
			{
				WallMeshes[x].push_back(GetWorld()->SpawnActor<AActor>(WallTemplates[WallIdx], FrontWallPos, FRotator::ZeroRotator));
				WallChances[WallIdx]--;
			}
			else
			{
				WallMeshes[x].push_back(GetWorld()->SpawnActor<AActor>(DoorTemplates[DoorFrameIdx], FrontWallPos, FRotator::ZeroRotator));
				DoorChances[DoorFrameIdx]--;
			}

			//left wall
			LeftWallPos = NextCellCenter;
			LeftWallPos.X += CellSize / 2;
			if (y == Height - 1 || !MazeGrid->AreConnected(MazeGrid->GridCells[x][y], MazeGrid->GridCells[x][y + 1]))
			{
				WallMeshes[x].push_back(GetWorld()->SpawnActor<AActor>(WallTemplates[WallIdx], LeftWallPos, LeftWallRot));
				WallChances[WallIdx]--;
			}
			else
			{
				WallMeshes[x].push_back(GetWorld()->SpawnActor<AActor>(DoorTemplates[DoorFrameIdx], LeftWallPos, LeftWallRot));
				DoorChances[DoorFrameIdx]--;
			}
		}
	}

	NextCellCenter = FVector::ZeroVector;
	for (int x = 0; x < Height; ++x, NextCellCenter.X += CellSize)
	{
		WallIdx = rand() % WallMeshTemplatesAndChance.Num();
		while (WallChances[WallIdx] == 0) WallIdx = rand() % WallMeshTemplatesAndChance.Num();
		WallChances[WallIdx]--;

		auto BackWallPos = NextCellCenter;
		BackWallPos.Y -= CellSize / 2;
		WallMeshes[x].push_back(GetWorld()->SpawnActor<AActor>(WallTemplates[WallIdx], BackWallPos, FRotator::ZeroRotator));
	}

	NextCellCenter = FVector::ZeroVector;
	for (int y = 0; y < Width; ++y, NextCellCenter.Y += CellSize)
	{
		WallIdx = rand() % WallMeshTemplatesAndChance.Num();
		while (WallChances[WallIdx] == 0) WallIdx = rand() % WallMeshTemplatesAndChance.Num();
		WallChances[WallIdx]--;

		auto RightWallPos = NextCellCenter;
		RightWallPos.X -= CellSize / 2;
		WallMeshes[y].push_back(GetWorld()->SpawnActor<AActor>(WallTemplates[WallIdx], RightWallPos, LeftWallRot));
	}
}


FRotator AMazeActor::GetRand4OrientationRotator()
{
	FRotator RandRot = FRotator::ZeroRotator;

	int OrientationRand = rand() % 4;

	switch (OrientationRand)
	{
	/*case 0:
		RandRot.Yaw += 0;
		break;*/
	case 1:
		RandRot.Yaw += 90;
		break;
	case 2:
		RandRot.Yaw += 180;
		break;
	case 3:
		RandRot.Yaw += 270;
		break;
	}
	return RandRot;
}