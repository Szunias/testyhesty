
#pragma once

#include "CoreMinimal.h"
#include "BaseItem.h"
#include "ItemHolyPendant.generated.h"

UCLASS()
class DUNGEONOFAPOSTASY_API AItemHolyPendant : public ABaseItem
{
	GENERATED_BODY()
public:
	AItemHolyPendant();

	virtual void Use() override;
	virtual void Throw() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int LuckBoost;

private:

	UFUNCTION()
	void OnThrowOverlapBegin(class UPrimitiveComponent* overlappedComp, class AActor* otherActor, class UPrimitiveComponent* otherComp, int32 otherBodyIndex, bool bFromSweep, const FHitResult& sweepResult);

	UFUNCTION()
	void OnThrowHit(class UPrimitiveComponent* hitComponent, AActor* otherActor, UPrimitiveComponent* otherComponent, FVector NormalImpulse, const FHitResult& hit);



};
