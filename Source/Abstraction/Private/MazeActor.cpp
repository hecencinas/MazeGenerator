// Fill out your copyright notice in the Description page of Project Settings.


#include "MazeActor.h"
#include "Engine/StaticMeshActor.h"
#include "Random.h"

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

	if (MazeGrid == nullptr && FloorMeshes.Num() <= 0 && WallMeshes.Num() <= 0)
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
	MazeGrid = new RecursiveDivisionMaze(Width, Height);

	if (FloorMeshes.Num() > 0) for (TArray<AActor*> FloorRow : FloorMeshes) for (AActor* MeshActor : FloorRow) if (MeshActor) MeshActor->Destroy();
	if (WallMeshes.Num() > 0) for (TArray<AActor*> WallRow : WallMeshes) for (AActor* MeshActor : WallRow) if (MeshActor) MeshActor->Destroy();
	FloorMeshes.Empty(Width);
	WallMeshes.Empty(Width);
	UniqueSpawned.Empty();

	GetDatableTemplatesAndRarity(FloorTemplatesAndRarity, FloorTemplates);
	GetDatableTemplatesAndRarity(WallTemplatesAndRarity, WallTemplates);
	GetDatableTemplatesAndRarity(DoorTemplatesAndRarity, DoorTemplates);

	InstantiateMaze();
}

void AMazeActor::InstantiateMaze()
{
	FVector NextCellCenter = GetActorLocation();
	float BeginX = NextCellCenter.X; //begin collumn

	FVector FrontWallPos = NextCellCenter;
	FrontWallPos.Y += CellSize / 2;

	FVector LeftWallPos = NextCellCenter;
	LeftWallPos.X += CellSize / 2;
	FRotator LeftWallRot = FRotator::ZeroRotator;
	LeftWallRot.Yaw += 90;

	int FloorTemplateIdx = 0;
	int WallTemplateIdx = 0;
	int DoorTemplateIdx = 0;

	for (int x = 0; x < Width; ++x, NextCellCenter.Y += CellSize)
	{
		FloorMeshes.Add(TArray<AActor*>());
		WallMeshes.Add(TArray<AActor*>());
		NextCellCenter.X = BeginX; //begin column

		for (int y = 0; y < Height; ++y, NextCellCenter.X += CellSize)
		{
			FloorTemplateIdx = GetRandomTemplateIndex(FloorTemplates);
			FRotator RandRot = GetRand4OrientationRotator();
			FloorMeshes[x].Add(GetWorld()->SpawnActor<AActor>(FloorTemplates[FloorTemplateIdx].ActorClass, NextCellCenter, RandRot));

			WallTemplateIdx = GetRandomTemplateIndex(WallTemplates);

			DoorTemplateIdx = GetRandomTemplateIndex(DoorTemplates);

			//front wall
			FrontWallPos = NextCellCenter;
			FrontWallPos.Y += CellSize / 2;
			if (x == Width - 1 || !MazeGrid->AreConnected(MazeGrid->GridCells[x][y], MazeGrid->GridCells[x + 1][y]))
			{
				WallMeshes[x].Add(GetWorld()->SpawnActor<AActor>(WallTemplates[WallTemplateIdx].ActorClass, FrontWallPos, FRotator::ZeroRotator));
			}
			else
			{
				WallMeshes[x].Add(GetWorld()->SpawnActor<AActor>(DoorTemplates[DoorTemplateIdx].ActorClass, FrontWallPos, FRotator::ZeroRotator));
			}

			//left wall
			LeftWallPos = NextCellCenter;
			LeftWallPos.X += CellSize / 2;
			if (y == Height - 1 || !MazeGrid->AreConnected(MazeGrid->GridCells[x][y], MazeGrid->GridCells[x][y + 1]))
			{
				WallMeshes[x].Add(GetWorld()->SpawnActor<AActor>(WallTemplates[WallTemplateIdx].ActorClass, LeftWallPos, LeftWallRot));
			}
			else
			{
				WallMeshes[x].Add(GetWorld()->SpawnActor<AActor>(DoorTemplates[DoorTemplateIdx].ActorClass, LeftWallPos, LeftWallRot));
			}
		}
	}

	NextCellCenter = GetActorLocation();
	for (int x = 0; x < Height; ++x, NextCellCenter.X += CellSize)
	{
		WallTemplateIdx = GetRandomTemplateIndex(WallTemplates);

		FVector BackWallPos = NextCellCenter;
		BackWallPos.Y -= CellSize / 2;
		WallMeshes[x].Add(GetWorld()->SpawnActor<AActor>(WallTemplates[WallTemplateIdx].ActorClass, BackWallPos, FRotator::ZeroRotator));
	}

	NextCellCenter = GetActorLocation();
	for (int y = 0; y < Width; ++y, NextCellCenter.Y += CellSize)
	{
		WallTemplateIdx = GetRandomTemplateIndex(WallTemplates);

		auto RightWallPos = NextCellCenter;
		RightWallPos.X -= CellSize / 2;
		WallMeshes[y].Add(GetWorld()->SpawnActor<AActor>(WallTemplates[WallTemplateIdx].ActorClass, RightWallPos, LeftWallRot));
	}
}

FRotator AMazeActor::GetRand4OrientationRotator()
{
	FRotator RandRot = FRotator::ZeroRotator;

	int OrientationRand = Random::NextInt(4, 0);// rand() % 4;

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

void AMazeActor::GetDatableTemplatesAndRarity(UDataTable* TemplatesAndRarity, TArray<FActorTemplatesAndRarity>& TemplatesArray)
{
	TArray<FActorTemplatesAndRarity> Templates;
	TemplatesAndRarity->ForeachRow<FActorTemplatesAndRarity>("FillTemplate", [&Templates](const FName& Key, const FActorTemplatesAndRarity& Value)
		{
			Templates.Add(Value);
		});
	auto floorTemplatesQty = TemplatesAndRarity->GetRowNames().Num();
	TemplatesArray.Empty(TemplatesAndRarity->GetRowNames().Num());
	TemplatesArray = Templates;
}

int32 AMazeActor::GetRandomTemplateIndex(TArray<FActorTemplatesAndRarity> ActorTemplates)
{
	int32 RarityGrade = Random::NextInt(100, 1); //(rand() % 100) + 1;
	EActorTemplatesRarity Rarity;
	if (RarityGrade < 10) Rarity = EActorTemplatesRarity::Unique;
	else if (RarityGrade < 25) Rarity = EActorTemplatesRarity::Rare;
	else if (RarityGrade < 40) Rarity = EActorTemplatesRarity::Uncommon;
	else Rarity = EActorTemplatesRarity::Common;

	int32 randIdx = 0;
	TArray<FActorTemplatesAndRarity> FilteredTemplates;

	switch (Rarity)
	{
	case EActorTemplatesRarity::Unique:
		randIdx = Random::NextInt(ActorTemplates.Num(), 0); //rand() % ActorTemplates.Num();
		while (UniqueSpawned.Contains(ActorTemplates[randIdx].ActorClass))
		{
			randIdx = Random::NextInt(ActorTemplates.Num(), 0); //rand() % ActorTemplates.Num();
		}
		if (ActorTemplates[randIdx].Rarity == EActorTemplatesRarity::Unique)
		{
			UniqueSpawned.Add(ActorTemplates[randIdx].ActorClass);
		}
		break;

	case EActorTemplatesRarity::Rare:
		FilteredTemplates = ActorTemplates.FilterByPredicate(
			[&](const FActorTemplatesAndRarity& Template)
			{
				return Template.Rarity != EActorTemplatesRarity::Unique;
			});

		randIdx = Random::NextInt(FilteredTemplates.Num(), 0); //rand() % FilteredTemplates.Num();
		randIdx = ActorTemplates.IndexOfByPredicate(
			[&](const FActorTemplatesAndRarity& Template)
			{
				return Template.ActorClass == FilteredTemplates[randIdx].ActorClass;
			});
		break;

	case EActorTemplatesRarity::Uncommon:
		FilteredTemplates = ActorTemplates.FilterByPredicate(
			[&](const FActorTemplatesAndRarity& Template)
			{
				return Template.Rarity != EActorTemplatesRarity::Unique && Template.Rarity != EActorTemplatesRarity::Rare;
			});
		randIdx = Random::NextInt(FilteredTemplates.Num(), 0); //rand() % FilteredTemplates.Num();
		randIdx = ActorTemplates.IndexOfByPredicate(
			[&](const FActorTemplatesAndRarity& Template)
			{
				return Template.ActorClass == FilteredTemplates[randIdx].ActorClass;
			});
		break;

	default: //case EActorTemplatesRarity::Common
		FilteredTemplates = ActorTemplates.FilterByPredicate(
			[&](const FActorTemplatesAndRarity& Template)
			{
				return Template.Rarity == EActorTemplatesRarity::Common;
			});
		randIdx = Random::NextInt(FilteredTemplates.Num(), 0); //rand() % FilteredTemplates.Num();
		randIdx = ActorTemplates.IndexOfByPredicate(
			[&](const FActorTemplatesAndRarity& Template)
			{
				return Template.ActorClass == FilteredTemplates[randIdx].ActorClass;
			});
		break;
	}

	return randIdx;
}
