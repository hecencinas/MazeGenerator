// Fill out your copyright notice in the Description page of Project Settings.


#include "SlidingDoorComponent.h"
#include "Engine/TriggerBox.h"

// Sets default values for this component's properties
USlidingDoorComponent::USlidingDoorComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void USlidingDoorComponent::BeginPlay()
{
	Super::BeginPlay();

	Parent = GetOwner();
	PlayerPawn = GetWorld()->GetFirstPlayerController()->GetPawn();
	FinalPosition = StartPosition = Parent->GetActorLocation();
	FinalPosition += DesiredTranslation;
	CurrentTranslationTime = 0.0f;
	opened = false;
	closed = true;
}


// Called every frame
void USlidingDoorComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (PlayerPawn)
	{
		const bool inFronOfDoor = DoorTrigger->IsOverlappingActor(PlayerPawn);
		if (inFronOfDoor)
		{
			if (!opened)
			{
				CurrentTranslationTime += DeltaTime;
				const float TimeRatio = FMath::Clamp((CurrentTranslationTime / TimeToTranslate), 0.0f, 1.0f);
				const float TranslationAlpha = OpenCurve.GetRichCurveConst()->Eval(TimeRatio);
				const FVector CurrentPosition = FMath::Lerp(StartPosition, FinalPosition, TranslationAlpha);
				Parent->SetActorLocation(CurrentPosition);

				opened = TimeRatio == 1.0;
				closed = TimeRatio == 0.0;
			}
		}
		else if (!closed)
		{
			CurrentTranslationTime -= DeltaTime;
			const float TimeRatio = FMath::Clamp((CurrentTranslationTime / TimeToTranslate), 0.0f, 1.0f);
			const float TranslationAlpha = OpenCurve.GetRichCurveConst()->Eval(TimeRatio);
			const FVector CurrentPosition = FMath::Lerp(StartPosition, FinalPosition, TranslationAlpha);
			Parent->SetActorLocation(CurrentPosition);

			opened = TimeRatio == 1.0;
			closed = TimeRatio == 0.0;
		}
	}
}

