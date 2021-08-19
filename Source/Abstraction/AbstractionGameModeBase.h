// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "GameFramework/GameMode.h"
#include "Blueprint/UserWidget.h"
#include "AbstractionGameModeBase.generated.h"

/**
 * 
 */
UCLASS()
class ABSTRACTION_API AAbstractionGameModeBase : public AGameMode
{
	GENERATED_BODY()

public:
	void StartPlay() override;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UUserWidget> ObjectiveWidgetClass;
};
