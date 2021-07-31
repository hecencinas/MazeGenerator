// Fill out your copyright notice in the Description page of Project Settings.


#include "ObjectiveComponent.h"
#include "ObjectiveWorldSubsystem.h"

// Sets default values for this component's properties
UObjectiveComponent::UObjectiveComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	ObjectiveState = EObjectiveState::Inactive;
}

// Called when the game starts
void UObjectiveComponent::BeginPlay()
{
	Super::BeginPlay();

	UObjectiveWorldSubsystem* ObjectiveWorldSubsystem = GetWorld()->GetSubsystem<UObjectiveWorldSubsystem>();
	if (ObjectiveWorldSubsystem)
	{
		ObjectiveWorldSubsystem->AddObjective(this);
	}
}

void UObjectiveComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	UObjectiveWorldSubsystem* ObjectiveWorldSubsystem = GetWorld()->GetSubsystem<UObjectiveWorldSubsystem>();
	if (ObjectiveWorldSubsystem)
	{
		ObjectiveWorldSubsystem->RemoveObjective(this);
	}

	Super::EndPlay(EndPlayReason);
}

// Called every frame
void UObjectiveComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UObjectiveComponent::SetObjectiveState(EObjectiveState NewState)
{
	if (ObjectiveState != NewState)
	{
		ObjectiveState = NewState;
		ObjectiveStateChangedEvent.Broadcast(this, ObjectiveState);
	}
}

