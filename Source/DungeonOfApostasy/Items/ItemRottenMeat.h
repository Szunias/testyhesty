#pragma once

#include "CoreMinimal.h"
#include "BaseItem.h"
#include "ItemRottenMeat.generated.h"

UCLASS()
class DUNGEONOFAPOSTASY_API AItemRottenMeat : public ABaseItem
{
	GENERATED_BODY()
public:
	AItemRottenMeat();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int ConsumeDamage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float EnemyEatTimer;

	virtual void Use() override;
	virtual void Throw() override;

	virtual void BeginPlay() override;

private:
	UFUNCTION()
	void OnThrowOverlapBegin(class UPrimitiveComponent* overlappedComp, class AActor* otherActor, class UPrimitiveComponent* otherComp, int32 otherBodyIndex, bool bFromSweep, const FHitResult& sweepResult);

	UFUNCTION()
	void OnThrowHit(class UPrimitiveComponent* hitComponent, AActor* otherActor, UPrimitiveComponent* otherComponent, FVector NormalImpulse, const FHitResult& hit);

	UFUNCTION()
	void StopOnLowVelocity();

	UFUNCTION()
	void DestroyItem();

	void ActivateMeat();

	bool IsBeingEaten;

};
