// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ObjectiveComponent.generated.h"

UENUM()
enum class EObjectiveState
{
	Inactive = 0		UMETA(DisplayName = "Inactive"),
	Active = 1		UMETA(DisplayName = "Active"),
	Completed = 2	UMETA(DisplayName = "Completed"),
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ABSTRACTION_API UObjectiveComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UObjectiveComponent();

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable)
	const FString& GetTitle() const { return Title; }
	UFUNCTION(BlueprintCallable)
	const FString& GetDescription() const { return Description; }

	UFUNCTION(BlueprintCallable)
	EObjectiveState GetObjectiveState() const { return ObjectiveState; }

	DECLARE_EVENT_TwoParams(FObjectiveComponent, FObjectiveStateChanged, UObjectiveComponent*, EObjectiveState)
	FObjectiveStateChanged& OnObjectiveStateChanged() { return ObjectiveStateChangedEvent; }

	void SetObjectiveState(EObjectiveState NewState);

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UPROPERTY(EditAnywhere)
	FString Title;
	UPROPERTY(EditAnywhere)
	FString Description;

	UPROPERTY(EditAnywhere)
	EObjectiveState ObjectiveState;
	
	FObjectiveStateChanged ObjectiveStateChangedEvent;
};
