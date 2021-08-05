// Fill out your copyright notice in the Description page of Project Settings.


#include "DealDamageComponent.h"
#include "AbstractionPlayerCharacter.h"
#include "Components/BoxComponent.h"

// Sets default values for this component's properties
UDealDamageComponent::UDealDamageComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	TriggerArea = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerArea"));
}


// Called when the game starts
void UDealDamageComponent::BeginPlay()
{
	Super::BeginPlay();

}

void UDealDamageComponent::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
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

void UDealDamageComponent::SetTrapActive(bool Active)
{
	if (Active)
	{
		TriggerArea->OnComponentBeginOverlap.AddDynamic(this, &UDealDamageComponent::OnOverlapBegin);

		TArray<AActor*> OverlappingActors;
		TriggerArea->GetOverlappingActors(OverlappingActors);
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
		TriggerArea->OnComponentBeginOverlap.RemoveDynamic(this, &UDealDamageComponent::OnOverlapBegin);
	}
}
