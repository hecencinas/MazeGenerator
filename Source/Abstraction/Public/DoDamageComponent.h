// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/BoxComponent.h"
#include "DoDamageComponent.generated.h"

/**
 * 
 */
UCLASS(ClassGroup = "Collision", hidecategories = (Object, LOD, Lighting, TextureStreaming), editinlinenew, meta = (DisplayName = "Deal Damage", BlueprintSpawnableComponent))
class ABSTRACTION_API UDoDamageComponent : public UBoxComponent
{
	GENERATED_BODY()
	
public:
	// Sets default values for this component's properties
	UDoDamageComponent();

	UFUNCTION()
	virtual void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION(BlueprintCallable)
	void SetTrapActive(bool Active);


protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere)
		float BaseDamage = 50.0f;
};
