// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "InteractiveDoor.generated.h"

UENUM()
enum class EDoorState2
{
	Closed = 0	UMETA(DisplayName = "Closed"),
	Opened = 1	UMETA(DisplayName = "Opened"),
	Closing = 2	UMETA(DisplayName = "Closing"),
	Opening = 3	UMETA(DisplayName = "Opening"),
	Locked = 4	UMETA(DisplayName = "Locked")
};
UCLASS()
class ABSTRACTION_API AInteractiveDoor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AInteractiveDoor();

	virtual void Tick(float DeltaTime) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	class UBoxComponent* FrontDoorTrigger;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	class UBoxComponent* BackDoorTrigger;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	class UStaticMeshComponent* DoorMesh;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	class UStaticMeshComponent* DoorFrameMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	class USceneComponent* RootSceneComponent;

	UFUNCTION()
	void OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	void OnEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

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
	FRuntimeFloatCurve OpenCurve;
	UPROPERTY(BlueprintReadOnly)
	EDoorState2 DoorState;

	bool rotating = false;
	bool translating = false;

	void RotateInTime(float TimeRatio);
	void TranslateInTime(float TimeRatio);

	UFUNCTION()
	void AnimateDoor();

	FTimerDelegate DoorTimerDelegate;
	FTimerHandle DoorTimerHandle;
};
