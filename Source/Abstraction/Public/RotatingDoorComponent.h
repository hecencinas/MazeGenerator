// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Curves/CurveFloat.h"
#include "RotatingDoorComponent.generated.h"

class ATriggerBox;

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class ABSTRACTION_API URotatingDoorComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	URotatingDoorComponent();

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	AActor* Parent;

	UPROPERTY(EditAnywhere)
	FRotator DesiredRotation = FRotator::ZeroRotator;
	FRotator StartRotation = FRotator::ZeroRotator;
	FRotator FinalRotation = FRotator::ZeroRotator;
	FRotator FinalBackRotation = FRotator::ZeroRotator;
	FRotator usingFinalRotation = FRotator::ZeroRotator;;

	UPROPERTY(EditAnywhere)
	bool UseBackRotation = false;
	UPROPERTY(EditAnywhere)
	float TimeToRotate;
	float CurrentRotationTime = 0.0f;

	UPROPERTY(EditAnywhere)
	ATriggerBox* FrontDoorTrigger;
	UPROPERTY(EditAnywhere)
	ATriggerBox* BackDoorTrigger;

	UPROPERTY(EditAnywhere)
	FRuntimeFloatCurve OpenCurve;

	bool opened = false;
	bool closed = true;
	APawn* PlayerPawn;
};
