// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Engine/DataTable.h"
#include "RecursiveDivisionMaze.h"
#include <vector>
#include "MazeActor.generated.h"

UENUM(BlueprintType)
enum class EActorTemplatesRarity : uint8
{
	Common     UMETA(DisplayName = "Common"),
	Uncommon      UMETA(DisplayName = "Uncommon"),
	Rare   UMETA(DisplayName = "Rare"),
	Unique   UMETA(DisplayName = "Unique"),
};

USTRUCT(BlueprintType)
struct FActorTemplatesAndRarity : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Templates)
	TEnumAsByte<EActorTemplatesRarity> Rarity;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Templates)
	TSubclassOf<AActor> ActorClass;
};

struct FFloorRoom
{
	AActor* FloorActor;
	TArray<AActor*> WallActors;

	AActor* FrontWallActor;
	AActor* BackWallActor;
	AActor* RightWallActor;
	AActor* LeftWallActor;
	AActor* CeilingActor;
};

USTRUCT(BlueprintType)
struct FFloorSettings
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Generator")
	int FloorWidth = 10;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Generator")
	int FloorDepth = 10;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Generator")
	FVector RoomVolume = FVector::OneVector * 1000;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Generator")
	bool HasCeiling = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Generator")
	bool RandomRotateFloors = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Generator")
	UDataTable* FloorTemplatesAndRarity;
	TArray<FActorTemplatesAndRarity> FloorTemplates;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Generator")
	UDataTable* WallTemplatesAndRarity;
	TArray<FActorTemplatesAndRarity> WallTemplates;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Generator")
	UDataTable* DoorTemplatesAndRarity;
	TArray<FActorTemplatesAndRarity> DoorTemplates;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Generator")
	UDataTable* OutsideWallTemplatesAndRarity;
	TArray<FActorTemplatesAndRarity> OutsideWallTemplates;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Generator")
	UDataTable* CeilingTemplatesAndRarity;
	TArray<FActorTemplatesAndRarity> CeilingTemplates;
};

UCLASS()
class ABSTRACTION_API AMazeActor : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AMazeActor();

	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Generator")
	bool GenerateOnBeginPlay = false;

	UFUNCTION(CallInEditor, BlueprintCallable, Category = "Generator")
	void GenerateMaze();

protected:
	virtual void BeginPlay() override;
	void InstantiateMaze();

	RecursiveDivisionMaze* MazeGrid;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Generator")
	FFloorSettings FloorSettings;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	class USceneComponent* RootSceneComponent;

	TArray<TArray<FFloorRoom*>> SpawnedRooms;

	TArray<TSubclassOf<AActor>> UniqueSpawned;

	FRotator GetRand4OrientationRotator();
	void GetDatableTemplatesAndRarity(UDataTable* TemplatesAndRarity, TArray<FActorTemplatesAndRarity>& TemplatesArray);
	int32 GetRandomTemplateIndex(TArray<FActorTemplatesAndRarity> ActorTemplates);

	void UpdateRoom(FPoint FirstRoomConnected, FPoint SecondRoomConnected);
};
