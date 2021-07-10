// Fill out your copyright notice in the Description page of Project Settings.


#include "InteractiveDoorComponent.h"
#include "Engine/TriggerBox.h"

#include "DrawDebugHelpers.h"

constexpr float FLT_METERS(float meters) { return meters * 100.0f; }

static TAutoConsoleVariable<bool> CVarToggleDebugDoor
(
	TEXT("Abstraction.InteractiveDoorComponent.Debug"),
	false,
	TEXT("Toggle InteractiveDoorComponent debug display."),
	ECVF_Cheat
);

// Sets default values for this component's properties
UInteractiveDoorComponent::UInteractiveDoorComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	DoorState = EDoorState::Closed;
	//CVarToggleDebugDoor.AsVariable()->SetOnChangedCallback(FConsoleVariableDelegate::CreateStatic(&UInteractiveDoorComponent::OnDebugToggled));
}


// Called when the game starts
void UInteractiveDoorComponent::BeginPlay()
{
	Super::BeginPlay();

	Parent = GetOwner();
	PlayerPawn = GetWorld()->GetFirstPlayerController()->GetPawn();

	CurrentTime = 0.0f;
	opened = false;
	closed = true;

	//rotate init
	rotating = DesiredRotation != FRotator::ZeroRotator;

	FinalReverseRotation = FinalRotation = StartRotation = Parent->GetActorRotation();
	FinalRotation += DesiredRotation;
	FinalReverseRotation -= DesiredRotation;
	usingFinalRotation = FinalRotation;

	//translate init
	translating = DesiredTranslation != FVector::ZeroVector;

	FinalReversePosition = FinalPosition = StartPosition = Parent->GetActorLocation();
	FinalPosition += DesiredTranslation;
	FinalReversePosition -= DesiredTranslation;
	usingFinalPosition = FinalPosition;
}

void UInteractiveDoorComponent::RotateInTime(const float TimeRatio)
{
	if (rotating)
	{
		const float RotationAlpha = OpenCurve.GetRichCurveConst()->Eval(TimeRatio);
		const FRotator CurrentRotation = FMath::Lerp(StartRotation, usingFinalRotation, RotationAlpha);
		Parent->SetActorRotation(CurrentRotation);
	}
}

void UInteractiveDoorComponent::TranslateInTime(const float TimeRatio)
{
	if (translating)
	{
		const float TranslationAlpha = OpenCurve.GetRichCurveConst()->Eval(TimeRatio);
		const FVector CurrentPosition = FMath::Lerp(StartPosition, usingFinalPosition, TranslationAlpha);
		Parent->SetActorLocation(CurrentPosition);
	}
}

// Called every frame
void UInteractiveDoorComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	if (!FrontDoorTrigger || !BackDoorTrigger) return;

	const bool frontDoor = FrontDoorTrigger->IsOverlappingActor(PlayerPawn);
	const bool backDoor = BackDoorTrigger->IsOverlappingActor(PlayerPawn);
	if (frontDoor || backDoor) //is he near the door?
	{
		if (closed)
		{
			//should use reversed rotation?
			if (backDoor && UseReverseRotation) usingFinalRotation = FinalReverseRotation;
			else usingFinalRotation = FinalRotation;

			//reversed translation?
			if (backDoor && UseReverseTranslation) usingFinalPosition = FinalReversePosition;
			else usingFinalPosition = FinalPosition;
		}

		if (!opened) //is the door fully open?
		{
			CurrentTime += DeltaTime;
			const float TimeRatio = FMath::Clamp((CurrentTime / TimeToOpen), 0.0f, 1.0f);
			RotateInTime(TimeRatio);
			TranslateInTime(TimeRatio);

			opened = TimeRatio == 1.0f;
			closed = TimeRatio == 0.0f;
			DoorState = (opened) ? EDoorState::Opened : EDoorState::Opening;
		}
	}
	else if (!closed) //is the door fully closed?
	{
		CurrentTime -= DeltaTime;
		const float TimeRatio = FMath::Clamp((CurrentTime / TimeToOpen), 0.0f, 1.0f);
		RotateInTime(TimeRatio);
		TranslateInTime(TimeRatio);

		opened = TimeRatio == 1.0f;
		closed = TimeRatio == 0.0f;

		DoorState = (opened) ? EDoorState::Closed : EDoorState::Closing;
	}
}

void UInteractiveDoorComponent::OnDebugToggled(IConsoleVariable* var)
{
	UE_LOG(LogTemp, Warning, TEXT("Abstraction.UInteractiveDoorComponent OnDebugToggled"));
}

