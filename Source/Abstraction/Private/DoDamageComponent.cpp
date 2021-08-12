// Fill out your copyright notice in the Description page of Project Settings.


#include "DoDamageComponent.h"
#include "AbstractionPlayerCharacter.h"

UDoDamageComponent::UDoDamageComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UDoDamageComponent::BeginPlay()
{
	Super::BeginPlay();
	SetTrapActive(true);
}

void UDoDamageComponent::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor == GetOwner()) return;

	AAbstractionPlayerCharacter* PlayerCharacter = Cast<AAbstractionPlayerCharacter>(OtherActor);
	if (PlayerCharacter)
	{
		TSubclassOf<UDamageType> const ValidDamageClassType = TSubclassOf<UDamageType>(UDamageType::StaticClass());
		FDamageEvent DamageEvent(ValidDamageClassType);

		PlayerCharacter->TakeDamage(BaseDamage, DamageEvent, nullptr, GetOwner());
	}
}

void UDoDamageComponent::SetTrapActive(bool Active)
{
	if (Active)
	{
		OnComponentBeginOverlap.AddDynamic(this, &UDoDamageComponent::OnOverlapBegin);

		TArray<AActor*> OverlappingActors;
		GetOverlappingActors(OverlappingActors);
		if (OverlappingActors.Num() > 0)
		{
			int idx = OverlappingActors.Find(GetWorld()->GetFirstPlayerController()->GetPawn());
			if (idx != INDEX_NONE)
			{
				AAbstractionPlayerCharacter* PlayerCharacter = Cast<AAbstractionPlayerCharacter>(OverlappingActors[idx]);
				if (PlayerCharacter)
				{
					TSubclassOf<UDamageType> const ValidDamageClassType = TSubclassOf<UDamageType>(UDamageType::StaticClass());
					FDamageEvent DamageEvent(ValidDamageClassType);

					PlayerCharacter->TakeDamage(BaseDamage, DamageEvent, nullptr, GetOwner());
				}
			}
		}
	}
	else
	{
		OnComponentBeginOverlap.RemoveDynamic(this, &UDoDamageComponent::OnOverlapBegin);
	}
}

