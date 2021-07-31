// Fill out your copyright notice in the Description page of Project Settings.


#include "ObjectiveWorldSubsystem.h"
#include "Kismet/GameplayStatics.h"
#include "Blueprint/UserWidget.h"
#include "ObjectiveComponent.h"

void UObjectiveWorldSubsystem::CreateObjectiveWidget(TSubclassOf<UUserWidget> ObjectiveWidgetClass)
{
	if (ObjectiveWidget == nullptr)
	{
		APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
		ObjectiveWidget = CreateWidget<UUserWidget>(PlayerController, ObjectiveWidgetClass);
	}
}

void UObjectiveWorldSubsystem::DisplayObjectiveWidget()
{
	ensureMsgf(ObjectiveWidget, TEXT("UObjectiveWorldSubsystem::DisplayObjective ObjectiveWidget is nullptr"));
	ObjectiveWidget->AddToViewport();
}

void UObjectiveWorldSubsystem::OnObjectiveStateChanged(UObjectiveComponent* ObjectiveComponent, EObjectiveState ObjectiveState)
{
	DisplayObjectiveWidget();
}

FString UObjectiveWorldSubsystem::GetCurrentObjectiveTitle()
{
	if (!Objectives.IsValidIndex(0) || Objectives[0]->GetObjectiveState() == EObjectiveState::Inactive)
	{
		return TEXT("N/A");
	}

	FString title = Objectives[0]->GetTitle();
	if (Objectives[0]->GetObjectiveState() == EObjectiveState::Completed)
	{
		title += TEXT(" - Completed");
	}

	return title;
}

FString UObjectiveWorldSubsystem::GetCurrentObjectiveDescription()
{
	if (!Objectives.IsValidIndex(0) || Objectives[0]->GetObjectiveState() == EObjectiveState::Inactive)
	{
		return TEXT("N/A");
	}

	FString description = Objectives[0]->GetDescription();

	return description;
}

void UObjectiveWorldSubsystem::AddObjective(UObjectiveComponent* ObjectiveComponent)
{
	check(ObjectiveComponent);

	size_t PreviousSize = Objectives.Num();
	Objectives.AddUnique(ObjectiveComponent);
	if (PreviousSize < Objectives.Num())
	{
		ObjectiveComponent->OnObjectiveStateChanged().AddUObject(this, &UObjectiveWorldSubsystem::OnObjectiveStateChanged);
	}
}

void UObjectiveWorldSubsystem::RemoveObjective(UObjectiveComponent* ObjectiveComponent)
{
	Objectives.Remove(ObjectiveComponent);
}
