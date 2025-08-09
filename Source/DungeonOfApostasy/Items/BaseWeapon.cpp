#include "BaseWeapon.h"
#include "Kismet/GameplayStatics.h"
#include "../Characters/Player/PlayerCharacter.h"
#include "../Characters/Enemies/BaseEnemy.h"
#include "../Characters/StatsComponent.h"
#include "Components/SphereComponent.h"

ABaseWeapon::ABaseWeapon()
{
	IsEquipped = false;

	ItemName = "BASEWEAPON";
}

void ABaseWeapon::Use()
{
	if (IsEquipped) // if already equipped -> unequip
	{
		Unequip();
		return;
	}
	IsEquipped = true;
	APlayerCharacter* pc{ Cast<APlayerCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0)) };
	pc->SetCurrentEquippedWeapon(this, MeshToEquip);
	EquipEffect();
}

void ABaseWeapon::Throw()
{
	if (IsEquipped)
	{
		Unequip();
	}
	
	Mesh->OnComponentBeginOverlap.Clear();
	ShowItem();
	Mesh->OnComponentBeginOverlap.AddDynamic(this, &ABaseWeapon::OnThrowOverlapBegin);
	Mesh->OnComponentHit.AddDynamic(this, &ABaseWeapon::OnThrowHit);
}

void ABaseWeapon::Unequip()
{
	IsEquipped = false;
	APlayerCharacter* pc{ Cast<APlayerCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0)) };
	pc->SetCurrentEquippedWeapon(nullptr, nullptr);
	UnequipEffect();
}

bool ABaseWeapon::IsWeaponEquipped() const
{
	return IsEquipped;
}

bool ABaseWeapon::CanBeThrown() const
{
	return !IsEquipped;
}

void ABaseWeapon::EquipEffect() { GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Green, "BaseWeapon Got Equipped"); }

void ABaseWeapon::UnequipEffect(){ GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Green, "BaseWeapon got unequipped"); }

void ABaseWeapon::ThrowEffect(){ GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Green, "BaseWeapon got thrown"); }


void ABaseWeapon::OnThrowOverlapBegin(UPrimitiveComponent* overlappedComp, AActor* otherActor, UPrimitiveComponent* otherComp, int32 otherBodyIndex, bool bFromSweep, const FHitResult& sweepResult)
{
	GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Emerald, "overlap from: " + otherActor->GetName());
	if (otherActor && (otherActor != this) && !HitEnemies.Contains(Cast<ABaseEnemy>(otherActor)) && !otherActor->IsA<APlayerCharacter>() && otherComp->IsA<USkeletalMeshComponent>())
	{
		ABaseEnemy* enemy{ Cast<ABaseEnemy>(otherActor) };
		enemy->GetStatsComponent()->UpdateHealth(-StrengthBoost);
		HitEnemies.Add(enemy);
		HandleDestroy();
	}
}

void ABaseWeapon::OnThrowHit(UPrimitiveComponent* hitComponent, AActor* otherActor, UPrimitiveComponent* otherComponent, FVector NormalImpulse, const FHitResult& hit)
{
	//GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Emerald, "hit from: " + otherActor->GetName());
	if (otherActor && (otherActor != this) && !otherActor->IsA<APlayerCharacter>() && otherComponent->IsA<UStaticMeshComponent>())
	{
		HandleDestroy();
	}
}
