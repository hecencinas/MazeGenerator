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

UCLASS()
class ABSTRACTION_API AMazeActor : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AMazeActor();

	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, Category = "Generator")
	int Width = 10;
	UPROPERTY(EditAnywhere, Category = "Generator")
	int Height = 10;
	UPROPERTY(EditAnywhere, Category = "Generator")
	int CellSize = 1000;

	UFUNCTION(CallInEditor, Category = "Generator")
	void GenerateMaze();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	void InstantiateMaze();

	RecursiveDivisionMaze* MazeGrid;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	class USceneComponent* RootSceneComponent;

	UPROPERTY(EditAnywhere, Category = "Generator")
	UDataTable* FloorTemplatesAndRarity;
	UPROPERTY(EditAnywhere, Category = "Generator")
	UDataTable* WallTemplatesAndRarity;
	UPROPERTY(EditAnywhere, Category = "Generator")
	UDataTable* DoorTemplatesAndRarity;

	TArray<FActorTemplatesAndRarity> FloorTemplates;
	TArray<FActorTemplatesAndRarity> WallTemplates;
	TArray<FActorTemplatesAndRarity> DoorTemplates;

	TArray<TArray<AActor*>> FloorMeshes;
	TArray<TArray<AActor*>> WallMeshes;

	TArray<TSubclassOf<AActor>> UniqueSpawned;

	FRotator GetRand4OrientationRotator();
	void GetDatableTemplatesAndRarity(UDataTable* TemplatesAndRarity, TArray<FActorTemplatesAndRarity>& TemplatesArray);
	int32 GetRandomTemplateIndex(TArray<FActorTemplatesAndRarity> ActorTemplates);
};
