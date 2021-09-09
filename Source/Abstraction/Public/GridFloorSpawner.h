// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Grid.h"
#include "Engine/DataTable.h"
#include "MazeActor.h" //todo: remove this reference
#include "GridFloorSpawner.generated.h"

//UENUM(BlueprintType)
//enum class EActorTemplatesRarity : uint8
//{
//	Common     UMETA(DisplayName = "Common"),
//	Uncommon      UMETA(DisplayName = "Uncommon"),
//	Rare   UMETA(DisplayName = "Rare"),
//	Unique   UMETA(DisplayName = "Unique"),
//};

//USTRUCT(BlueprintType)
//struct FActorTemplatesAndRarity : public FTableRowBase
//{
//	GENERATED_USTRUCT_BODY()
//
//		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Templates)
//		TEnumAsByte<EActorTemplatesRarity> Rarity;
//	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Templates)
//		TSubclassOf<AActor> ActorClass;
//}; 

//struct FFloorRoom
//{
//	AActor* FloorActor;
//	TArray<AActor*> WallActors;
//}; 

UCLASS()
class ABSTRACTION_API AGridFloorSpawner : public AActor
{
	GENERATED_BODY()

public:
	AGridFloorSpawner();
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(CallInEditor, BlueprintCallable, Category = "Generator")
	void SpawnFloor();
	UFUNCTION(CallInEditor, BlueprintCallable, Category = "Generator")
	void GetNewMazeGrid();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Generator")
	bool GenerateOnBeginPlay = false;

protected:
	virtual void BeginPlay() override;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	class USceneComponent* RootSceneComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Generator")
	FFloorSettings FloorSettings;

	void InstantiateFloorActors();

	FGrid* Grid;

	TArray<TArray<FFloorRoom*>> SpawnedRooms;

	TArray<TSubclassOf<AActor>> UniqueSpawned;

	FRotator GetRand4OrientationRotator();
	void GetDatableTemplatesAndRarity(UDataTable* TemplatesAndRarity, TArray<FActorTemplatesAndRarity>& TemplatesArray);
	int32 GetRandomTemplateIndex(TArray<FActorTemplatesAndRarity> ActorTemplates);

	UFUNCTION()
	void UpdateRoom(FPoint FirstRoomConnected, FPoint SecondRoomConnected);
};
