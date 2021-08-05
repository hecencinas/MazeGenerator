// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "RecursiveDivisionMaze.h"
#include <vector>
#include "MazeActor.generated.h"

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
	TMap<TSubclassOf<AActor>, int> FloorMeshTemplatesAndChance;
	UPROPERTY(EditAnywhere, Category = "Generator")
	TMap<TSubclassOf<AActor>, int> WallMeshTemplatesAndChance;
	UPROPERTY(EditAnywhere, Category = "Generator")
	TMap<TSubclassOf<AActor>, int> DoorFramedWallMeshTemplatesAndChance;

	std::vector<std::vector<AActor*>> FloorMeshes;
	std::vector<std::vector<AActor*>> WallMeshes;
};
