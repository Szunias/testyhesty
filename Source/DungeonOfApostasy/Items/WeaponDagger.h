#pragma once

#include "CoreMinimal.h"
#include "BaseWeapon.h"
#include "WeaponDagger.generated.h"

UCLASS()
class DUNGEONOFAPOSTASY_API AWeaponDagger : public ABaseWeapon
{
	GENERATED_BODY()
	
public:
	AWeaponDagger();

protected:

	virtual void EquipEffect() override;
	virtual void UnequipEffect() override;
	virtual void ThrowEffect() override;
};
