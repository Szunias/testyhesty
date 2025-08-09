
#include "ItemSmokeBomb.h"
#include "../Characters/StatsComponent.h"
#include "../Characters/Player/PlayerCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Components/SphereComponent.h"
#include "Components/CapsuleComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"
#include "../Battle/BattleArea.h"

AItemSmokeBomb::AItemSmokeBomb()
{
	ItemName = "Smoke Bomb";
}

void AItemSmokeBomb::BeginPlay()
{
	Super::BeginPlay();

	SmokeSphere = GetComponentByClass<USphereComponent>();
	if (!IsValid(SmokeSphere))
	{
		UE_LOG(LogTemp, Fatal, TEXT("SmokeSphere in SmokeBomb Item not found!!!"));
	}
	SmokeSphere->SetGenerateOverlapEvents(false);
	SmokeSphere->OnComponentBeginOverlap.AddDynamic(this, &AItemSmokeBomb::OnSmokeSphereOverlapBegin);
	SmokeSphere->OnComponentEndOverlap.AddDynamic(this, &AItemSmokeBomb::OnSmokeSphereOverlapEnd);

	SmokeParticles = Cast<UNiagaraComponent>(GetDefaultSubobjectByName(TEXT("SmokeNiagara")));
	if (!IsValid(SmokeParticles))
	{
		UE_LOG(LogTemp, Fatal, TEXT("SmokeParticles in SmokeBomb Item not found!!!"));
	}
}

void AItemSmokeBomb::Use()
{
	if (IsBattleActive())
	{
		BattleEffect(GetPlayerCharacter());
	}
	else
	{
		APlayerCharacter* pc{ GetPlayerCharacter() };
		FVector location{ pc->GetActorLocation() };
		location.Z -= pc->GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight();
		OverworldEffect(location);
		OnItemUsed.Broadcast();
	}
}

void AItemSmokeBomb::Throw()
{
	if (IsBattleActive())
	{
		BattleEffect(GetPlayerCharacter()->GetTargettedEnemy());
	}
	else
	{
		ShowItem(true);
		Mesh->OnComponentBeginOverlap.AddDynamic(this, &AItemSmokeBomb::OnThrowOverlapBegin);
		Mesh->OnComponentHit.AddDynamic(this, &AItemSmokeBomb::OnThrowHit);
	}
}

bool AItemSmokeBomb::IsBattleActive()
{
	APlayerCharacter* pc{ GetPlayerCharacter()};
	if (!IsValid(pc))
	{
		return false;
	}
	else
	{
		return pc->IsPlayerInBattle();
	}
}

void AItemSmokeBomb::OverworldEffect(FVector location)
{
	Mesh->SetVisibility(true);
	SetActorLocation(location);
	SmokeParticles->SetVariableFloat("LoopDuration", 2.0f); // sets the correct duration for the smoke
	SmokeParticles->SetVariableFloat("SpawnRate", 30.0f); // density of the smoke
	SmokeParticles->ActivateSystem();
	SmokeParticles->SetVariableFloat("Radius", SmokeSphere->GetUnscaledSphereRadius() - 1.0f); // sets the correct duration for the smoke
	// THE CORRECT DURATION IS NOT FIXED YET

	SmokeParticles->SetVariableFloat("LoopDuration", SmokeDuration); // sets the correct duration for the smoke
	SmokeParticles->ActivateSystem();
	SmokeSphere->SetGenerateOverlapEvents(true);

	FTimerHandle handle{};
	float smokeDelay{ 2.0f }; // this we delay the destruction of the item until the smoke is gone
	GetWorld()->GetTimerManager().SetTimer(handle, this, &AItemSmokeBomb::EndItem, SmokeDuration + smokeDelay);
}

void AItemSmokeBomb::BattleEffect(ABaseCharacter* character)
{
	// change the evasiveness of the character for x amount of times
	character->GetStatsComponent()->UpdateEvasiveness(EvasivenessBoost);
	ABattleArea* battleArena{Cast<ABattleArea>(UGameplayStatics::GetActorOfClass(GetWorld(), ABattleArea::StaticClass()))};
	if (IsValid(battleArena))
	{
		GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Yellow, "battle area found, applying effect");
		FTurnEffect currentEffect{};
		currentEffect.EffectDuration = BattleTurnDuration;
		currentEffect.EffectType = EEffectType::Evasiveness;
		currentEffect.EffectValue = 1;
		currentEffect.EffectDescription = "evasion boost";
		currentEffect.OnEffectEnded.AddDynamic(this, &AItemSmokeBomb::ResetBattleEffect);

		battleArena->ApplyEffect(character, currentEffect);

		APlayerCharacter* pc{ GetPlayerCharacter() };
		FVector location{ pc->GetActorLocation() };
		location.Z -= pc->GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight();
		SetActorLocation(location);

		// the duration also affects the duration it takes to spawn the particles
		// we spawn short lived particles first; then spawn longer particles
		// this way the short particles die by the time the longer particles have formed

		SmokeParticles->SetVariableFloat("SpawnRate", 10.0f); // density of the smoke
		SmokeParticles->SetVariableFloat("LoopDuration", 3.0f); // we don't know the duration of the turns, pick big number
		SmokeParticles->SetVariableFloat("Radius", character->GetCapsuleComponent()->GetUnscaledCapsuleRadius() * 2.0f);
		SmokeParticles->ActivateSystem();

		SmokeParticles->SetVariableFloat("LoopDuration", 50.0); // we don't know the duration of the turns, pick big number
		SmokeParticles->ActivateSystem();

	
		FTimerHandle handle{};
		GetWorld()->GetTimerManager().SetTimer(handle, FTimerDelegate::CreateLambda([&]
				{
					SmokeParticles->SetVariableFloat("LoopDuration", 1000.0f); // we don't know the duration of the turns, pick big number
					SmokeParticles->ActivateSystem();
				}
		), 3.0f, false);


		ModifiedCharacter = character;
	}
}

void AItemSmokeBomb::EndItem()
{
	Destroy();
}

APlayerCharacter* AItemSmokeBomb::GetPlayerCharacter()
{
	return Cast<APlayerCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
}

void AItemSmokeBomb::OnSmokeSphereOverlapBegin(UPrimitiveComponent* overlappedComp, AActor* otherActor, UPrimitiveComponent* otherComp, int32 otherBodyIndex, bool bFromSweep, const FHitResult& sweepResult)
{
	if (otherActor && (otherActor != this) && otherActor->IsA<APlayerCharacter>())
	{
		Cast<APlayerCharacter>(otherActor)->SetPlayerObscured(true);
	}
}

void AItemSmokeBomb::OnSmokeSphereOverlapEnd(UPrimitiveComponent* overlappedComp, AActor* otherActor, UPrimitiveComponent* otherComp, int32 otherBodyIndex)
{
	if (otherActor && (otherActor != this) && otherActor->IsA<APlayerCharacter>())
	{
		Cast<APlayerCharacter>(otherActor)->SetPlayerObscured(false);
	}
}

void AItemSmokeBomb::OnThrowOverlapBegin(UPrimitiveComponent* overlappedComp, AActor* otherActor, UPrimitiveComponent* otherComp, int32 otherBodyIndex, bool bFromSweep, const FHitResult& sweepResult)
{
	if (otherActor && (otherActor != this) && !otherActor->IsA<APlayerCharacter>() && otherComp->IsA<USkeletalMeshComponent>())
	{
		Explode();
	}
}

void AItemSmokeBomb::OnThrowHit(UPrimitiveComponent* hitComponent, AActor* otherActor, UPrimitiveComponent* otherComponent, FVector NormalImpulse, const FHitResult& hit)
{
	if (otherActor && (otherActor != this) && !otherActor->IsA<APlayerCharacter>() && (otherComponent->IsA<UStaticMeshComponent>() || otherComponent->IsA<USkeletalMeshComponent>()))
	{
		Explode();
	}
}

void AItemSmokeBomb::ResetBattleEffect()
{
	// destroy everything particle based
	// reset character evasiveness
	GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Yellow, "turn effect is done");

	if (!IsValid(ModifiedCharacter))
	{
		return;
	}
	ModifiedCharacter->GetStatsComponent()->UpdateEvasiveness(-EvasivenessBoost);

	EndItem();
}

void AItemSmokeBomb::Explode()
{
	ActivatePhysics(false);
	OverworldEffect(GetActorLocation());
	ShowItem(false);
}

