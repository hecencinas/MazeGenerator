// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Math/Vector.h"
#include "Curves/CurveFloat.h"
#include "SlidingDoorComponent.generated.h"

class ATriggerBox;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ABSTRACTION_API USlidingDoorComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	USlidingDoorComponent();

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	AActor* Parent;
	APawn* PlayerPawn;

	UPROPERTY(EditAnywhere)
	FVector DesiredTranslation = FVector::ZeroVector;

	FVector StartPosition = FVector::ZeroVector;
	FVector FinalPosition = FVector::ZeroVector;

	UPROPERTY(EditAnywhere)
	float TimeToTranslate;
	float CurrentTranslationTime = 0.0f;

	UPROPERTY(EditAnywhere)
	ATriggerBox* DoorTrigger;

	UPROPERTY(EditAnywhere)
	FRuntimeFloatCurve OpenCurve;

	bool opened = false;
	bool closed = true;
};
