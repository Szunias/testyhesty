
#include "WeaponSword.h"
#include "Kismet/GameplayStatics.h"
#include "../Characters/Player/PlayerCharacter.h"
#include "../Characters/StatsComponent.h"

AWeaponSword::AWeaponSword()
{
	ItemName = "Sword";
}

void AWeaponSword::EquipEffect()
{
	APlayerCharacter* pc{ Cast<APlayerCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0)) };
	UStatsComponent* stats{ pc->GetStatsComponent() };
	stats->UpdateStrength(StrengthBoost);
	stats->UpdateSpeed(-SpeedDebuff);
	OnItemUsed.Broadcast();
}

void AWeaponSword::UnequipEffect()
{
	APlayerCharacter* pc{ Cast<APlayerCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0)) };
	UStatsComponent* stats{ pc->GetStatsComponent() };
	stats->UpdateStrength(-StrengthBoost);
	stats->UpdateSpeed(SpeedDebuff);
}

void AWeaponSword::ThrowEffect()
{
	// do throw effect
}
