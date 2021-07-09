// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AbstractDoor.generated.h"

class UInteractiveDoorComponent;
class UStaticMeshComponent;

UCLASS()
class ABSTRACTION_API AAbstractDoor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AAbstractDoor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, NoClear)
	UStaticMeshComponent* DoorMesh;
	UPROPERTY(EditAnywhere, NoClear)
	UInteractiveDoorComponent* InteractiveDoorComponent;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
