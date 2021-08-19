// Fill out your copyright notice in the Description page of Project Settings.


#include "AbstractionPlayerCharacter.h"

// Sets default values
AAbstractionPlayerCharacter::AAbstractionPlayerCharacter()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	HealthComponent = CreateDefaultSubobject<UHealthComponent>(TEXT("HealthComponent"));
}

// Called when the game starts or when spawned
void AAbstractionPlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
void AAbstractionPlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AAbstractionPlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void AAbstractionPlayerCharacter::FellOutOfWorld(const class UDamageType& dmgTyoe)
{
	OnDeath(true);
}

float AAbstractionPlayerCharacter::TakeDamage(float Damage, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	float DamageRes;
	if (&DamageEvent == nullptr)
	{
		TSubclassOf<UDamageType> const ValidDamageClassType = TSubclassOf<UDamageType>(UDamageType::StaticClass());
		FDamageEvent DamageEvent2(ValidDamageClassType);
		DamageRes = Super::TakeDamage(Damage, DamageEvent, EventInstigator, DamageCauser);
	}
	else
	{
		DamageRes = Super::TakeDamage(Damage, DamageEvent, EventInstigator, DamageCauser);
	}

	UE_LOG(LogTemp, Warning, TEXT("AAbstractionPlayerCharacter::TakeDamage %.2f"), DamageRes);
	if (HealthComponent)
	{
		HealthComponent->TakeDamage(Damage);
		if (HealthComponent->IsDead()) OnDeath(false);
	}
	return DamageRes;
}

void AAbstractionPlayerCharacter::OnDeath(bool IsFellOut)
{
	APlayerController* PlayerController = GetController<APlayerController>();
	if (PlayerController) PlayerController->RestartLevel();
	else Destroy();
}
