// Fill out your copyright notice in the Description page of Project Settings.


#include "AbstractDoor.h"
#include "InteractiveDoorComponent.h"

// Sets default values
AAbstractDoor::AAbstractDoor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	DoorMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DoorMesh"));
	InteractiveDoorComponent = CreateDefaultSubobject<UInteractiveDoorComponent>(TEXT("InteractiveDoorComponent"));
}

// Called when the game starts or when spawned
void AAbstractDoor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AAbstractDoor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

