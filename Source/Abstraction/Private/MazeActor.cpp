// Fill out your copyright notice in the Description page of Project Settings.


#include "MazeActor.h"
#include "Engine/StaticMeshActor.h"
#include "Random.h"

AMazeActor::AMazeActor()
{
	PrimaryActorTick.bCanEverTick = true;

	RootSceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
	RootComponent = RootSceneComponent;
}

void AMazeActor::BeginPlay()
{
	Super::BeginPlay();

	if (GenerateOnBeginPlay)
	{
		UE_LOG(LogTemp, Warning, TEXT("Generating maze at begin play."));
		GenerateMaze();
	}
}

void AMazeActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AMazeActor::GenerateMaze()
{
	MazeGrid = new RecursiveDivisionMaze(FloorSettings.FloorWidth, FloorSettings.FloorDepth);
	MazeGrid->OnUpdateGridConnection.BindDynamic(this, &AMazeActor::UpdateRoom);

	if (SpawnedRooms.Num() > 0)
	{
		for (TArray<FFloorRoom*> FloorRow : SpawnedRooms)
		{
			for (FFloorRoom* FloorRoom : FloorRow)
			{
				if (FloorRoom->FloorActor) FloorRoom->FloorActor->Destroy();
				for (AActor* Wall : FloorRoom->WallActors)
				{
					if (Wall) Wall->Destroy();
				}
			}
			FloorRow.Empty();
		}
	}
	UniqueSpawned.Empty();

	GetDatableTemplatesAndRarity(FloorSettings.FloorTemplatesAndRarity, FloorSettings.FloorTemplates);
	GetDatableTemplatesAndRarity(FloorSettings.WallTemplatesAndRarity, FloorSettings.WallTemplates);
	GetDatableTemplatesAndRarity(FloorSettings.DoorTemplatesAndRarity, FloorSettings.DoorTemplates);
	GetDatableTemplatesAndRarity(FloorSettings.OutsideWallTemplatesAndRarity, FloorSettings.OutsideWallTemplates);
	GetDatableTemplatesAndRarity(FloorSettings.CeilingTemplatesAndRarity, FloorSettings.CeilingTemplates);

	InstantiateMaze();
}

void AMazeActor::InstantiateMaze()
{
	FVector NextCellCenter = GetActorLocation();
	float BeginY = NextCellCenter.Y;

	FRotator Rot90Degrees = FRotator::ZeroRotator;
	Rot90Degrees.Yaw += 90;

	int FloorTemplateIdx = 0;
	int WallTemplateIdx = 0;
	int DoorTemplateIdx = 0;
	int OutsideWallTemplateIdx = 0;

	SpawnedRooms.Empty();
	for (int x = 0; x < FloorSettings.FloorWidth; ++x, NextCellCenter.X += FloorSettings.RoomVolume.X)
	{
		SpawnedRooms.Add(TArray<FFloorRoom*>());
		NextCellCenter.Y = BeginY;

		for (int y = 0; y < FloorSettings.FloorDepth; ++y, NextCellCenter.Y += FloorSettings.RoomVolume.Y)
		{
			FFloorRoom* NewRoom = new FFloorRoom();

			FloorTemplateIdx = GetRandomTemplateIndex(FloorSettings.FloorTemplates);
			FRotator RandRot = FRotator::ZeroRotator;
			if (FloorSettings.RandomRotateFloors) RandRot = GetRand4OrientationRotator();
			NewRoom->FloorActor = GetWorld()->SpawnActor<AActor>(FloorSettings.FloorTemplates[FloorTemplateIdx].ActorClass, NextCellCenter, RandRot);

			WallTemplateIdx = GetRandomTemplateIndex(FloorSettings.WallTemplates);
			DoorTemplateIdx = GetRandomTemplateIndex(FloorSettings.DoorTemplates);
			OutsideWallTemplateIdx = GetRandomTemplateIndex(FloorSettings.OutsideWallTemplates);

			//right wall
			FVector RightWallPos = NextCellCenter;
			RightWallPos.Y += FloorSettings.RoomVolume.Y / 2;
			if (y == FloorSettings.FloorDepth - 1)
			{
				OutsideWallTemplateIdx = GetRandomTemplateIndex(FloorSettings.OutsideWallTemplates);
				NewRoom->RightWallActor = GetWorld()->SpawnActor<AActor>(FloorSettings.OutsideWallTemplates[OutsideWallTemplateIdx].ActorClass, RightWallPos, FRotator::ZeroRotator);
			}
			else if (!MazeGrid->AreConnected(MazeGrid->GridCells[x][y], MazeGrid->GridCells[x][y + 1]))
			{
				WallTemplateIdx = GetRandomTemplateIndex(FloorSettings.WallTemplates);
				NewRoom->RightWallActor = GetWorld()->SpawnActor<AActor>(FloorSettings.WallTemplates[WallTemplateIdx].ActorClass, RightWallPos, FRotator::ZeroRotator);
			}
			else
			{
				DoorTemplateIdx = GetRandomTemplateIndex(FloorSettings.DoorTemplates);
				NewRoom->RightWallActor = GetWorld()->SpawnActor<AActor>(FloorSettings.DoorTemplates[DoorTemplateIdx].ActorClass, RightWallPos, FRotator::ZeroRotator);
			}

			//front wall
			FVector FrontWallPos = NextCellCenter;
			FrontWallPos.X += FloorSettings.RoomVolume.X / 2;
			if (x == FloorSettings.FloorWidth - 1)
			{
				OutsideWallTemplateIdx = GetRandomTemplateIndex(FloorSettings.OutsideWallTemplates);
				NewRoom->FrontWallActor = GetWorld()->SpawnActor<AActor>(FloorSettings.OutsideWallTemplates[OutsideWallTemplateIdx].ActorClass, FrontWallPos, Rot90Degrees);
			}
			else if (!MazeGrid->AreConnected(MazeGrid->GridCells[x][y], MazeGrid->GridCells[x + 1][y]))
			{
				WallTemplateIdx = GetRandomTemplateIndex(FloorSettings.WallTemplates);
				NewRoom->FrontWallActor = GetWorld()->SpawnActor<AActor>(FloorSettings.WallTemplates[WallTemplateIdx].ActorClass, FrontWallPos, Rot90Degrees);
			}
			else
			{
				DoorTemplateIdx = GetRandomTemplateIndex(FloorSettings.DoorTemplates);
				NewRoom->FrontWallActor = GetWorld()->SpawnActor<AActor>(FloorSettings.DoorTemplates[DoorTemplateIdx].ActorClass, FrontWallPos, Rot90Degrees);
			}

			//left wall
			if (y == 0)
			{
				OutsideWallTemplateIdx = GetRandomTemplateIndex(FloorSettings.OutsideWallTemplates);
				FVector BackWallPos = NextCellCenter;
				BackWallPos.Y -= FloorSettings.RoomVolume.Y / 2;
				NewRoom->LeftWallActor = GetWorld()->SpawnActor<AActor>(FloorSettings.OutsideWallTemplates[OutsideWallTemplateIdx].ActorClass, BackWallPos, FRotator::ZeroRotator);
			}

			//back wall
			if (x == 0)
			{
				OutsideWallTemplateIdx = GetRandomTemplateIndex(FloorSettings.OutsideWallTemplates);
				FVector BackWallPos = NextCellCenter;
				BackWallPos.X -= FloorSettings.RoomVolume.X / 2;
				NewRoom->BackWallActor = GetWorld()->SpawnActor<AActor>(FloorSettings.OutsideWallTemplates[OutsideWallTemplateIdx].ActorClass, BackWallPos, Rot90Degrees);
			}

			if (FloorSettings.HasCeiling)
			{
				int CeilingTemplateIdx = GetRandomTemplateIndex(FloorSettings.CeilingTemplates);

				if (FloorSettings.RandomRotateFloors) RandRot = GetRand4OrientationRotator();

				FVector CeilingPos = NextCellCenter;
				CeilingPos.Z += FloorSettings.RoomVolume.Z;

				NewRoom->CeilingActor = GetWorld()->SpawnActor<AActor>(FloorSettings.CeilingTemplates[CeilingTemplateIdx].ActorClass, CeilingPos, RandRot);
			}

			SpawnedRooms[x].Add(NewRoom);
		}
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
	int32 RarityGrade = Random::NextInt(100, 1);
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
		randIdx = Random::NextInt(ActorTemplates.Num(), 0);
		while (UniqueSpawned.Contains(ActorTemplates[randIdx].ActorClass))
		{
			randIdx = Random::NextInt(ActorTemplates.Num(), 0);
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

		randIdx = Random::NextInt(FilteredTemplates.Num(), 0);
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
		randIdx = Random::NextInt(FilteredTemplates.Num(), 0);
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
		randIdx = Random::NextInt(FilteredTemplates.Num(), 0);
		randIdx = ActorTemplates.IndexOfByPredicate(
			[&](const FActorTemplatesAndRarity& Template)
			{
				return Template.ActorClass == FilteredTemplates[randIdx].ActorClass;
			});
		break;
	}

	return randIdx;
}

void AMazeActor::UpdateRoom(FPoint FirstRoomConnected, FPoint SecondRoomConnected)
{
	UE_LOG(LogTemp, Warning, TEXT("Room updated"));
}
