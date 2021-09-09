// Fill out your copyright notice in the Description page of Project Settings.


#include "HealthComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Blueprint/UserWidget.h"
#include "Misc/OutputDeviceNull.h"

// Sets default values for this component's properties
UHealthComponent::UHealthComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;
}

void UHealthComponent::BeginPlay()
{
	Super::BeginPlay();

	CurrentHealth = MaxHealth;

	if (HealthWidgetClass)
	{
		APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
		HealthWidget = CreateWidget<UUserWidget>(PlayerController, HealthWidgetClass);
		HealthWidget->AddToViewport();

		CallWidgetUpdateHealth();
	}
}

void UHealthComponent::TakeDamage(float Damage)
{
	CurrentHealth -= Damage;
	CallWidgetUpdateHealth();
}

void UHealthComponent::CallWidgetUpdateHealth()
{
	if (HealthWidgetClass)
	{
			FOutputDeviceNull OutputDeviceNull;

			const FString CmdAndParams = FString::Printf(TEXT("UpdateHealth %.2f %.2f"), CurrentHealth, MaxHealth); //update health
			HealthWidget->CallFunctionByNameWithArguments(*CmdAndParams, OutputDeviceNull, nullptr, true);
	}
}