// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CameraSpawn.generated.h"

class ACameraTrigger;

UCLASS()
class DUNGEONOFAPOSTASY_API ACameraSpawn : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACameraSpawn();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Start Camera")
	ACameraTrigger* StartCamera;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
};
