// Fill out your copyright notice in the Description page of Project Settings.


#include "RotatingDoorComponent.h"
#include "Engine/TriggerBox.h"

// Sets default values for this component's properties
URotatingDoorComponent::URotatingDoorComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void URotatingDoorComponent::BeginPlay()
{
	Super::BeginPlay();

	Parent = GetOwner();
	PlayerPawn = GetWorld()->GetFirstPlayerController()->GetPawn();
	FinalBackRotation = FinalRotation = StartRotation = Parent->GetActorRotation();
	FinalRotation += DesiredRotation;
	FinalBackRotation -= DesiredRotation;
	usingFinalRotation = FinalRotation;
	CurrentRotationTime = 0.0f;
	opened = false;
	closed = true;
}


// Called every frame
void URotatingDoorComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (PlayerPawn)
	{
		const bool frontDoor = FrontDoorTrigger->IsOverlappingActor(PlayerPawn);
		if (frontDoor || BackDoorTrigger->IsOverlappingActor(PlayerPawn))
		{
			if (closed)
			{
				if (frontDoor || !UseBackRotation)
				{
					usingFinalRotation = FinalRotation;
				}
				else
				{
					usingFinalRotation = FinalBackRotation;
				}
			}

			if (!opened)
			{
				CurrentRotationTime += DeltaTime;
				const float TimeRatio = FMath::Clamp((CurrentRotationTime / TimeToRotate), 0.0f, 1.0f);
				const float RotationAlpha = OpenCurve.GetRichCurveConst()->Eval(TimeRatio);
				const FRotator CurrentRotation = FMath::Lerp(StartRotation, usingFinalRotation, RotationAlpha);
				Parent->SetActorRotation(CurrentRotation);

				opened = TimeRatio == 1.0;
				closed = TimeRatio == 0.0;
			}
		}
		else if (!closed)
		{
			CurrentRotationTime -= DeltaTime;
			const float TimeRatio = FMath::Clamp((CurrentRotationTime / TimeToRotate), 0.0f, 1.0f);
			const float RotationAlpha = OpenCurve.GetRichCurveConst()->Eval(TimeRatio);
			const FRotator CurrentRotation = FMath::Lerp(StartRotation, usingFinalRotation, RotationAlpha);
			Parent->SetActorRotation(CurrentRotation);

			opened = TimeRatio == 1.0;
			closed = TimeRatio == 0.0;
		}
	}
}

