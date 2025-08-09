
#include "ItemSuspiciousPotion.h"
#include "../Characters/StatsComponent.h"
#include "../Characters/Player/PlayerCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraComponent.h"

AItemSuspiciousPotion::AItemSuspiciousPotion()
{
	ItemName = "Suspicious Potion";
	IsFatalPotion = false;
	SplashRadius = 400.0f;
}


void AItemSuspiciousPotion::BeginPlay()
{
	Super::BeginPlay();

	SplashParticles = Cast<UNiagaraComponent>(GetDefaultSubobjectByName(TEXT("SplashNiagara")));
	if (!IsValid(SplashParticles))
	{
		UE_LOG(LogTemp, Fatal, TEXT("InteractNiagara in BaseItem not found!!!"));
	}

	SplashParticles->SetVariableFloat("Radius", SplashRadius); // sets the correct duration for the smoke


	HealthEffect = FMath::RandRange(HealthRangeMin, HealthRangeMax);
	StrengthEffect = FMath::RandRange(StrengthRangeMin, StrengthRangeMax);
	LuckEffect = FMath::RandRange(LuckRangeMin, LuckRangeMax);
}

UTexture2D* AItemSuspiciousPotion::GetItemImage()
{
	if (IsFatalPotion)
	{
		return BadPotionImage;
	}

	if (HealthEffect == 0 && StrengthEffect == 0 && LuckEffect == 0) // will only be triggered before BeginPlay
	{
		return ItemImage;
	}
	else if (HealthEffect >= 0 && StrengthEffect >= 0 && LuckEffect >= 0)
	{
		return GoodPotionImage;
	}
	else if (HealthEffect <= 0 && StrengthEffect <= 0 && LuckEffect <= 0)
	{
		return BadPotionImage;
	}

	return ItemImage;
}

void AItemSuspiciousPotion::Use()
{
	APlayerCharacter* pc{ Cast<APlayerCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0)) };
	ExecuteEffect({ pc });
	OnItemUsed.Broadcast();
}

void AItemSuspiciousPotion::Throw()
{
	Mesh->OnComponentBeginOverlap.Clear();
	ShowItem();
	Mesh->OnComponentBeginOverlap.AddDynamic(this, &AItemSuspiciousPotion::OnThrowOverlapBegin);
	Mesh->OnComponentHit.AddDynamic(this, &AItemSuspiciousPotion::OnThrowHit);
}


void AItemSuspiciousPotion::OnThrowOverlapBegin(UPrimitiveComponent* overlappedComp, AActor* otherActor, UPrimitiveComponent* otherComp, int32 otherBodyIndex, bool bFromSweep, const FHitResult& sweepResult)
{
	GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Emerald, "Overlap from: " + otherActor->GetName());
	if (otherActor && (otherActor != this) && !otherActor->IsA<APlayerCharacter>() && otherComp->IsA<USkeletalMeshComponent>())
	{
		Explode();
	}
}

void AItemSuspiciousPotion::OnThrowHit(UPrimitiveComponent* hitComponent, AActor* otherActor, UPrimitiveComponent* otherComponent, FVector NormalImpulse, const FHitResult& hit)
{
	GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Emerald, "hit from: " + otherActor->GetName());
	if (otherActor && (otherActor != this) && !otherActor->IsA<APlayerCharacter>() && (otherComponent->IsA<UStaticMeshComponent>() || otherComponent->IsA<USkeletalMeshComponent>()))
	{
		Explode();
	}
}

void AItemSuspiciousPotion::Explode()
{
	SplashParticles->ActivateSystem();

	FVector pos{ GetActorLocation() };

	TArray<FHitResult> outHits{};
	if (!GetWorld()->SweepMultiByChannel(outHits, pos, pos, FQuat::Identity, ECollisionChannel::ECC_Visibility, FCollisionShape::MakeSphere(SplashRadius)))
		return;

	TArray<ABaseCharacter*> characters{};
	for (const FHitResult& hit : outHits)
	{
		if (AActor* hitActor{ hit.GetActor() }; IsValid(hitActor))
		{
			if (ABaseCharacter* character{ Cast<ABaseCharacter>(hitActor) }; IsValid(character))
			{
				characters.AddUnique(character);
			}
		}
	}

	ActivatePhysics(false);
	ShowItem(false);
	ExecuteEffect(characters);
}

void AItemSuspiciousPotion::ExecuteEffect(TArray<ABaseCharacter*> effectedEntities)
{
	for (ABaseCharacter* currCharacter : effectedEntities)
	{
		UStatsComponent* stats{ currCharacter->GetStatsComponent() };
		if (!IsFatalPotion) // no need to update health if it's fatal -> will auto reset to 1 hp
		{
			stats->UpdateHealth(HealthEffect);
		}
		else
		{
			stats->UpdateHealth(-(stats->GetCurrentHealth() - 1));
		}

		stats->UpdateStrength(StrengthEffect);
		stats->UpdateLuck(LuckEffect);
	}

	HandleDestroy();
}
