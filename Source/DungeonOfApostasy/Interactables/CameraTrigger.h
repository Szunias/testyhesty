#pragma once

#include "CoreMinimal.h"
#include "Camera/CameraActor.h"
#include "CameraTrigger.generated.h"

class ACameraTriggerBox;

UCLASS()
class DUNGEONOFAPOSTASY_API ACameraTrigger : public ACameraActor
{
	GENERATED_BODY()

public:
	ACameraTrigger();

	void SwitchToCamera();

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* overlappedComp, AActor* otherActor, UPrimitiveComponent* otherComp, int32 otherBodyIndex, bool bFromSweep, const FHitResult& sweepResult);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera Triggers")
	TArray<ACameraTriggerBox*> CameraTriggers;
};
