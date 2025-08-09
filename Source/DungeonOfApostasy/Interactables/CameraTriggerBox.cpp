// Fill out your copyright notice in the Description page of Project Settings.


#include "CameraTriggerBox.h"
#include "Components/BoxComponent.h"
#include "Components/BillboardComponent.h"

// Sets default values
ACameraTriggerBox::ACameraTriggerBox()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComponent"));
	RootComponent = SceneComponent;
	TriggerBox = CreateDefaultSubobject<UBoxComponent>(TEXT("Trigger Box"));
	TriggerBox->SetupAttachment(RootComponent);
	Billboard = CreateDefaultSubobject<UBillboardComponent>(TEXT("Billboard"));
	Billboard->SetupAttachment(RootComponent);
}

UBoxComponent* ACameraTriggerBox::GetTriggerBox()
{
	return TriggerBox;
}

