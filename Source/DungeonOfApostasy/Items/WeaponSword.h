#pragma once

#include "CoreMinimal.h"
#include "BaseWeapon.h"
#include "WeaponSword.generated.h"

UCLASS()
class DUNGEONOFAPOSTASY_API AWeaponSword : public ABaseWeapon
{
	GENERATED_BODY()
public:
	AWeaponSword();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Stats")
	int SpeedDebuff;

protected:
	virtual void EquipEffect() override;
	virtual void UnequipEffect() override;
	virtual void ThrowEffect() override;
};
