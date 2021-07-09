// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Curves/CurveFloat.h"
#include "InteractiveDoorComponent.generated.h"

class ATriggerBox;

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class ABSTRACTION_API UInteractiveDoorComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UInteractiveDoorComponent();

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	AActor* Parent;
	APawn* PlayerPawn;

	UPROPERTY(EditAnywhere)
		FRotator DesiredRotation = FRotator::ZeroRotator;
	FRotator StartRotation = FRotator::ZeroRotator;
	FRotator FinalRotation = FRotator::ZeroRotator;
	FRotator FinalReverseRotation = FRotator::ZeroRotator;
	FRotator usingFinalRotation = FRotator::ZeroRotator;

	UPROPERTY(EditAnywhere)
		FVector DesiredTranslation = FVector::ZeroVector;
	FVector StartPosition = FVector::ZeroVector;
	FVector FinalPosition = FVector::ZeroVector;
	FVector FinalReversePosition = FVector::ZeroVector;
	FVector usingFinalPosition = FVector::ZeroVector;

	UPROPERTY(EditAnywhere)
		bool UseReverseRotation = false;
	UPROPERTY(EditAnywhere)
		bool UseReverseTranslation = false;
	UPROPERTY(EditAnywhere)
		float TimeToOpen;
	float CurrentTime = 0.0f;

	UPROPERTY(EditAnywhere)
		ATriggerBox* FrontDoorTrigger;
	UPROPERTY(EditAnywhere)
		ATriggerBox* BackDoorTrigger;

	UPROPERTY(EditAnywhere)
		FRuntimeFloatCurve OpenCurve;

	bool rotating = false;
	bool translating = false;
	bool opened = false;
	bool closed = true;

	void RotateInTime(float TimeRatio);
	void TranslateInTime(float TimeRatio);
};
