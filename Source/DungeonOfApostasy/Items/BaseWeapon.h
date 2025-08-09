#pragma once

#include "CoreMinimal.h"
#include "BaseItem.h"
#include "BaseWeapon.generated.h"

class ABaseEnemy;

UCLASS()
class DUNGEONOFAPOSTASY_API ABaseWeapon : public ABaseItem
{
	GENERATED_BODY()
	
public:
	ABaseWeapon();

	UFUNCTION(BlueprintCallable)
	virtual void Use() override; // will be treated as "Equip"
	UFUNCTION(BlueprintCallable)
	virtual void Throw() override;
	UFUNCTION(BlueprintCallable)
	void Unequip();

	bool IsWeaponEquipped() const;

	virtual bool CanBeThrown() const override;

protected:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Stats")
	int StrengthBoost;

	virtual void EquipEffect();
	virtual void UnequipEffect();
	virtual void ThrowEffect();

	bool IsEquipped;

	UFUNCTION()
	void OnThrowOverlapBegin(class UPrimitiveComponent* overlappedComp, class AActor* otherActor, class UPrimitiveComponent* otherComp, int32 otherBodyIndex, bool bFromSweep, const FHitResult& sweepResult);

	UFUNCTION()
	void OnThrowHit(class UPrimitiveComponent* hitComponent, AActor* otherActor, UPrimitiveComponent* otherComponent, FVector NormalImpulse, const FHitResult& hit);


private:

	TArray<ABaseEnemy*> HitEnemies;
};
