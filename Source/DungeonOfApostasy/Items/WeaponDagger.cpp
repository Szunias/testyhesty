#include "WeaponDagger.h"
#include "Kismet/GameplayStatics.h"
#include "../Characters/Player/PlayerCharacter.h"
#include "../Characters/StatsComponent.h"

AWeaponDagger::AWeaponDagger()
{
	ItemName = "Dagger";
}

void AWeaponDagger::EquipEffect()
{
	APlayerCharacter* pc{ Cast<APlayerCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0)) };
	UStatsComponent* stats{ pc->GetStatsComponent() };
	stats->UpdateStrength(StrengthBoost);
	OnItemUsed.Broadcast();
}

void AWeaponDagger::UnequipEffect()
{
	APlayerCharacter* pc{ Cast<APlayerCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0)) };
	UStatsComponent* stats{ pc->GetStatsComponent() };
	stats->UpdateStrength( - StrengthBoost);
}

void AWeaponDagger::ThrowEffect()
{
	// do throw effect
}
