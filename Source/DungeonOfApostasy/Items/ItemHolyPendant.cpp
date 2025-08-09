
#include "ItemHolyPendant.h"
#include "../Characters/StatsComponent.h"
#include "../Characters/Player/PlayerCharacter.h"
#include "Kismet/GameplayStatics.h"

AItemHolyPendant::AItemHolyPendant()
{
	ItemName = "Holy Pendant";

}

void AItemHolyPendant::Use()
{
	APlayerCharacter* pc{ Cast<APlayerCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0)) };
	UStatsComponent* stats{ pc->GetStatsComponent() };
	GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, "Luck before use: " + FString::FromInt(stats->GetCurrentLuck()));
	stats->UpdateLuck(LuckBoost);
	GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, "Luck after use: " + FString::FromInt(stats->GetCurrentLuck()));
	OnItemUsed.Broadcast();
}

void AItemHolyPendant::Throw()
{
	APlayerCharacter* pc{ Cast<APlayerCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0)) };
	UStatsComponent* stats{ pc->GetStatsComponent() };
	GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, "Luck before throw: " + FString::FromInt(stats->GetCurrentLuck()));
	stats->UpdateLuck(-LuckBoost);
	GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, "Luck after throw: " + FString::FromInt(stats->GetCurrentLuck()));

	Mesh->OnComponentBeginOverlap.Clear();
	ShowItem(true);
	Mesh->OnComponentBeginOverlap.AddDynamic(this, &AItemHolyPendant::OnThrowOverlapBegin);
	Mesh->OnComponentHit.AddDynamic(this, &AItemHolyPendant::OnThrowHit);
}

void AItemHolyPendant::OnThrowOverlapBegin(UPrimitiveComponent* overlappedComp, AActor* otherActor, UPrimitiveComponent* otherComp, int32 otherBodyIndex, bool bFromSweep, const FHitResult& sweepResult)
{
	if (otherActor && (otherActor != this) && !otherActor->IsA<APlayerCharacter>() && otherComp->IsA<USkeletalMeshComponent>())
	{
		HandleDestroy();
	}
}

void AItemHolyPendant::OnThrowHit(UPrimitiveComponent* hitComponent, AActor* otherActor, UPrimitiveComponent* otherComponent, FVector NormalImpulse, const FHitResult& hit)
{
	if (otherActor && (otherActor != this) && !otherActor->IsA<APlayerCharacter>() && (otherComponent->IsA<UStaticMeshComponent>() || otherComponent->IsA<USkeletalMeshComponent>()))
	{
		HandleDestroy();
	}
}
