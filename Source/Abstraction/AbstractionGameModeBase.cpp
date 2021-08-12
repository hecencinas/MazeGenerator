// Copyright Epic Games, Inc. All Rights Reserved.


#include "AbstractionGameModeBase.h"
#include "Kismet/GameplayStatics.h"
#include "ObjectiveWorldSubsystem.h"

void AAbstractionGameModeBase::StartPlay()
{
	Super::StartPlay();

	UObjectiveWorldSubsystem* ObjectiveWorldSubsystem = GetWorld()->GetSubsystem<UObjectiveWorldSubsystem>();
	if (ObjectiveWorldSubsystem)
	{
		ObjectiveWorldSubsystem->CreateObjectiveWidget(ObjectiveWidgetClass);
		ObjectiveWorldSubsystem->DisplayObjectiveWidget();
	}

	APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	HealthWidget = CreateWidget<UUserWidget>(PlayerController, HealthWidgetClass);
	HealthWidget->AddToViewport();
}