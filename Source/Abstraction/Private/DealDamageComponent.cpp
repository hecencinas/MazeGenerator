// Fill out your copyright notice in the Description page of Project Settings.


#include "DealDamageComponent.h"
#include "AbstractionPlayerCharacter.h"
#include "Components/CapsuleComponent.h"

// Sets default values for this component's properties
UDealDamageComponent::UDealDamageComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	TriggerArea = CreateDefaultSubobject<UCapsuleComponent>(TEXT("TriggerArea"));
}


// Called when the game starts
void UDealDamageComponent::BeginPlay()
{
	Super::BeginPlay();

	TriggerArea->OnComponentBeginOverlap.AddDynamic(this, &UDealDamageComponent::OnOverlapBegin);
	TriggerArea->OnComponentEndOverlap.AddDynamic(this, &UDealDamageComponent::OnOverlapEnd);

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

void UDealDamageComponent::OnOverlapEnd(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBox)
{

}
