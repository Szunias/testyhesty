// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CameraTriggerBox.generated.h"

class UBoxComponent;
class UBillboardComponent;
class USceneComponent;

UCLASS()
class DUNGEONOFAPOSTASY_API ACameraTriggerBox : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACameraTriggerBox();

	UBoxComponent* GetTriggerBox();

protected:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Trigger")
	USceneComponent* SceneComponent;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Trigger")
	UBoxComponent* TriggerBox;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Billboard")
	UBillboardComponent* Billboard;
};
