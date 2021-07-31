// Fill out your copyright notice in the Description page of Project Settings.


#include "InteractiveDoor.h"
#include "Components/BoxComponent.h"
#include "Components/SceneComponent.h"
#include "Components/StaticMeshComponent.h"
#include "TimerManager.h"

// Sets default values
AInteractiveDoor::AInteractiveDoor()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RootSceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
	RootComponent = RootSceneComponent;

	FrontDoorTrigger = CreateDefaultSubobject<UBoxComponent>(TEXT("FrontDoorTrigger"));
	FrontDoorTrigger->SetupAttachment(RootComponent);

	FrontDoorTrigger->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	FrontDoorTrigger->SetCollisionObjectType(ECollisionChannel::ECC_WorldStatic);
	FrontDoorTrigger->SetCollisionResponseToAllChannels(ECR_Ignore);
	FrontDoorTrigger->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

	BackDoorTrigger = CreateDefaultSubobject<UBoxComponent>(TEXT("BackDoorTrigger"));
	BackDoorTrigger->SetupAttachment(RootComponent);

	BackDoorTrigger->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	BackDoorTrigger->SetCollisionObjectType(ECollisionChannel::ECC_WorldStatic);
	BackDoorTrigger->SetCollisionResponseToAllChannels(ECR_Ignore);
	BackDoorTrigger->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

	DoorMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DoorMesh"));
	DoorMesh->SetupAttachment(RootComponent);

	DoorState = EDoorState2::Closed;
}

// Called when the game starts or when spawned
void AInteractiveDoor::BeginPlay()
{
	Super::BeginPlay();

	FrontDoorTrigger->OnComponentBeginOverlap.AddDynamic(this, &AInteractiveDoor::OnBeginOverlap);
	BackDoorTrigger->OnComponentBeginOverlap.AddDynamic(this, &AInteractiveDoor::OnBeginOverlap);
	FrontDoorTrigger->OnComponentEndOverlap.AddDynamic(this, &AInteractiveDoor::OnEndOverlap);
	BackDoorTrigger->OnComponentEndOverlap.AddDynamic(this, &AInteractiveDoor::OnEndOverlap);

	PlayerPawn = GetWorld()->GetFirstPlayerController()->GetPawn();

	CurrentTime = 0.0f;

	//rotate init
	rotating = DesiredRotation != FRotator::ZeroRotator;

	FinalReverseRotation = FinalRotation = StartRotation = DoorMesh->GetRelativeRotation();
	FinalRotation += DesiredRotation;
	FinalReverseRotation -= DesiredRotation;
	usingFinalRotation = FinalRotation;

	//translate init
	translating = DesiredTranslation != FVector::ZeroVector;

	FinalReversePosition = FinalPosition = StartPosition = DoorMesh->GetRelativeLocation();
	FinalPosition += DesiredTranslation;
	FinalReversePosition -= DesiredTranslation;
	usingFinalPosition = FinalPosition;

	DoorTimerDelegate.BindUFunction(this, FName("AnimateDoor"));
}

// Called every frame
void AInteractiveDoor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AInteractiveDoor::OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!FrontDoorTrigger || !BackDoorTrigger) return;

	if (DoorState != EDoorState2::Opened)
	{
		GetWorldTimerManager().ClearTimer(DoorTimerHandle);
		DoorTimerHandle = GetWorldTimerManager().SetTimerForNextTick(DoorTimerDelegate);
	}
}

void AInteractiveDoor::OnEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex)
{
	if (!FrontDoorTrigger || !BackDoorTrigger) return;

	if (DoorState != EDoorState2::Closed)
	{
		GetWorldTimerManager().ClearTimer(DoorTimerHandle);
		DoorTimerHandle = GetWorldTimerManager().SetTimerForNextTick(DoorTimerDelegate);
	}
}

void AInteractiveDoor::AnimateDoor()
{
	float DeltaTime = GetWorld()->GetDeltaSeconds();

	const bool frontDoor = FrontDoorTrigger->IsOverlappingActor(PlayerPawn);
	const bool backDoor = BackDoorTrigger->IsOverlappingActor(PlayerPawn);
	if (frontDoor || backDoor) //is he near the door?
	{
		if (DoorState == EDoorState2::Closed)
		{
			//should use reversed rotation?
			if (backDoor && UseReverseRotation) usingFinalRotation = FinalReverseRotation;
			else usingFinalRotation = FinalRotation;

			//reversed translation?
			if (backDoor && UseReverseTranslation) usingFinalPosition = FinalReversePosition;
			else usingFinalPosition = FinalPosition;
		}

		if (DoorState != EDoorState2::Opened) //is the door not fully open?
		{
			CurrentTime = FMath::Clamp((CurrentTime + DeltaTime), 0.0f, TimeToOpen);;
			const float TimeRatio = FMath::Clamp((CurrentTime / TimeToOpen), 0.0f, 1.0f);
			RotateInTime(TimeRatio);
			TranslateInTime(TimeRatio);

			DoorState = (TimeRatio >= 1.0f) ? EDoorState2::Opened : EDoorState2::Opening;
			if (DoorState == EDoorState2::Opening) DoorTimerHandle = GetWorldTimerManager().SetTimerForNextTick(DoorTimerDelegate);
		}
	}
	else if (DoorState != EDoorState2::Closed) //is the door not fully closed?
	{
		CurrentTime = FMath::Clamp((CurrentTime - DeltaTime), 0.0f, TimeToOpen);;
		const float TimeRatio = FMath::Clamp((CurrentTime / TimeToOpen), 0.0f, 1.0f);
		RotateInTime(TimeRatio);
		TranslateInTime(TimeRatio);

		DoorState = (TimeRatio <= 0.0f) ? EDoorState2::Closed : EDoorState2::Closing;
		if (DoorState == EDoorState2::Closing) DoorTimerHandle = GetWorldTimerManager().SetTimerForNextTick(DoorTimerDelegate);
	}
}

void AInteractiveDoor::RotateInTime(const float TimeRatio)
{
	if (rotating)
	{
		const float RotationAlpha = OpenCurve.GetRichCurveConst()->Eval(TimeRatio);
		const FRotator CurrentRotation = FMath::Lerp(StartRotation, usingFinalRotation, RotationAlpha);
		DoorMesh->SetRelativeRotation(CurrentRotation);
	}
}

void AInteractiveDoor::TranslateInTime(const float TimeRatio)
{
	if (translating)
	{
		const float TranslationAlpha = OpenCurve.GetRichCurveConst()->Eval(TimeRatio);
		const FVector CurrentPosition = FMath::Lerp(StartPosition, usingFinalPosition, TranslationAlpha);
		DoorMesh->SetRelativeLocation(CurrentPosition);
	}
}

