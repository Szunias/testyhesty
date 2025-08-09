#pragma once

#include "InteractableInterface.generated.h"

class UBillboardComponent;

UINTERFACE(MinimalAPI, Blueprintable)
class UInteractableInterface : public UInterface
{
	GENERATED_BODY()
};

class IInteractableInterface
{
	GENERATED_BODY()
public:
	virtual void Interact();

	UFUNCTION()
	virtual void OnInteractOverlapBegin(class UPrimitiveComponent* overlappedComp, class AActor* otherActor, class UPrimitiveComponent* otherComp, int32 otherBodyIndex, bool bFromSweep, const FHitResult& sweepResult);

	UFUNCTION()
	virtual void OnInteractOverlapEnd(class UPrimitiveComponent* overlappedComp, class AActor* otherActor, class UPrimitiveComponent* otherComp, int32 otherBodyIndex);
	
	// UPROPERTY is not allowed inside interfaces
	//UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UBillboardComponent* InteractIcon;
};
