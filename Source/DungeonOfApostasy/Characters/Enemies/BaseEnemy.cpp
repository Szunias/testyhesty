#include "BaseEnemy.h"
#include "../EnemyController.h"
#include "NavigationSystem.h"
#include "Components/SphereComponent.h"
#include "Components/BoxComponent.h"
#include "../Player/PlayerCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "DrawDebugHelpers.h"
#include "../StatsComponent.h"
#include "Components/CapsuleComponent.h"
#include "../../Items/ItemRottenMeat.h"
#include "../../Other/EMessageType.h"
#include "../EnemyManager.h"
#include "Kismet/GameplayStatics.h"

ABaseEnemy::ABaseEnemy()
{
	WarnRadius = 300.f;
	StunDuration = 3.0f;
}

void ABaseEnemy::BeginPlay()
{
	Super::BeginPlay();
	SpawnDefaultController();

	OnAttackConnects.AddDynamic(this, &ABaseEnemy::PerformAttack); // MAKE SURE TO CLEAR THIS IN THE BOSS ENEMY

	// get all colliders
	SphereCollider = GetComponentByClass<USphereComponent>();
	if (!IsValid(SphereCollider))
	{
		UE_LOG(LogTemp, Fatal, TEXT("SphereCollider in BaseEnemy not found!!!"));
	}
	BoxCollider = GetComponentByClass<UBoxComponent>();
	if (!IsValid(BoxCollider))
	{
		UE_LOG(LogTemp, Fatal, TEXT("BoxCollider in BaseEnemy not found!!!"));
	}

	// bind componentoverlap events to colliders
	SphereCollider->OnComponentBeginOverlap.AddDynamic(this, &ABaseEnemy::OnSphereOverlapBegin);
	SphereCollider->OnComponentEndOverlap.AddDynamic(this, &ABaseEnemy::OnSphereOverlapEnd);

	BoxCollider->OnComponentBeginOverlap.AddDynamic(this, &ABaseEnemy::OnBoxOverlapBegin);

	BeginSphereRadius = SphereCollider->GetUnscaledSphereRadius();

	NavSystem = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld());
	EnemyController = Cast<AEnemyController>(GetController());
	BindIdle();
	IdleMovement();

	StatsComponent->OnDeath.AddDynamic(this, &ABaseEnemy::WhenEnemyDies);
	StatsComponent->OnDeathFinished.AddDynamic(this, &ABaseEnemy::WhenEnemyDeathFinished);
}

void ABaseEnemy::OnSphereOverlapBegin(class UPrimitiveComponent* overlappedComp, class AActor* otherActor, class UPrimitiveComponent* otherComp, int32 otherBodyIndex, bool bFromSweep, const FHitResult& sweepResult)
{
	if (otherActor && (otherActor != this) && otherComp && otherActor->IsA<APlayerCharacter>())
	{
		if (Cast<APlayerCharacter>(otherActor)->IsPlayerObscured())
		{
			return;
		}

		BindPlayerTooClose();
		GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Green, "Overlap Begin");
		//GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Green, "Overlap Begin with " + otherActor->GetName() + " and " + otherComp->GetName());
		if (EnemyController)
		{
			EnemyController->StopMovement(); // this forces the AI to stop and OnMoveComplete will be called
		}
		PointOfInterest = otherActor;
		SphereCollider->SetSphereRadius(BeginSphereRadius);
	}
	else if (otherActor && (otherActor != this) && otherComp && otherActor->IsA<AItemRottenMeat>())
	{
		if (IsValid(PointOfInterest))
		{
			return;
		}
		PointOfInterest = otherActor;
		BindPlayerTooClose();
		if (EnemyController)
		{
			EnemyController->StopMovement();
		}
		SphereCollider->SetSphereRadius(BeginSphereRadius / 2.0f);
	}
}


void ABaseEnemy::OnSphereOverlapEnd(class UPrimitiveComponent* overlappedComp, class AActor* otherActor, class UPrimitiveComponent* otherComp, int32 otherBodyIndex)
{
	if (otherActor && (otherActor != this) && otherComp && otherActor->IsA<APlayerCharacter>())
	{
		GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, "Overlap End");
		BindIdle();
		SphereCollider->SetSphereRadius(BeginSphereRadius);
		PointOfInterest = nullptr;
	}
	else if (otherActor && (otherActor != this) && otherComp && otherActor->IsA<AItemRottenMeat>())
	{
		// unset point of interest as the meat
		if (!otherActor->IsActorBeingDestroyed())
		{
			return;
		}
	
		BindIdle();
		if (EnemyController)
		{
			EnemyController->StopMovement();
		}
		PointOfInterest = nullptr;
		SphereCollider->SetSphereRadius(BeginSphereRadius);
	}
}

void ABaseEnemy::OnBoxOverlapBegin(class UPrimitiveComponent* overlappedComp, class AActor* otherActor, class UPrimitiveComponent* otherComp, int32 otherBodyIndex, bool bFromSweep, const FHitResult& sweepResult)
{
	if (!IsStunned && otherActor && (otherActor != this) && otherComp && otherActor->IsA<APlayerCharacter>())
	{
		APlayerCharacter* player{ Cast<APlayerCharacter>(otherActor) };
		if (player->GetStatsComponent()->GetHealthPercentage() < FLT_EPSILON)
		{
			return;
		}

		GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Blue, "Begin Battle!");
		TArray<ABaseEnemy*> enemiesInRange;
		enemiesInRange.AddUnique(this);
		WarnEnemiesInRadius(enemiesInRange);
		player->InitiateBattle(enemiesInRange);
	}
}

float ABaseEnemy::GetBattleRadius() const
{
	return GetCapsuleComponent()->GetScaledCapsuleRadius() + AddedBattleRadius;
}

void ABaseEnemy::OnBattleStart()
{
	IsStunned = true;
	GetCharacterMovement()->Deactivate();
	SphereCollider->SetGenerateOverlapEvents(false);
}

void ABaseEnemy::OnBattleEnd()
{
	OnStunBegin.Broadcast();

	FTimerHandle timerReveal{};
	GetWorld()->GetTimerManager().SetTimer(timerReveal, this, &ABaseEnemy::UnStunEnemy, StunDuration);
}

void ABaseEnemy::PrepareAttack(ABaseCharacter* target)
{
	GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Green, "Enemy Prepare Attack");

	TargettedEnemy = target;
	Attack();
}

void ABaseEnemy::PerformAttack()
{
	GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Green, "Enemy Perform Attack");

	UStatsComponent* targetStats{ TargettedEnemy->GetStatsComponent() };
	if (FMath::RandRange(0, 99) >= targetStats->GetCurrentEvasiveness())
	{
		int damageGiven{ StatsComponent->GetDamage() };
		targetStats->UpdateHealth(-damageGiven);
	}
	else
	{
		FString battleMessage{ CharacterName + " missed" };
		OnBattleMessage.Broadcast(battleMessage, EMessageType::Positive);
	}
}

void ABaseEnemy::Attack()
{
	GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Green, "Enemy On Attack");

	OnAttack.Broadcast();
}

bool ABaseEnemy::IsEnemyStunned()
{
	return IsStunned;
}

bool ABaseEnemy::IsBossEnemy() const
{
	return false;
}


//void ABaseEnemy::Tick(float DeltaTime)
//{
//	DrawDebugSphere(GetWorld(), GetActorLocation(), SphereCollider->GetUnscaledSphereRadius(), 12, FColor::Magenta, false, -1.0f, (uint8)0U, 4.0f);
//}

void ABaseEnemy::BindIdle() {}

void ABaseEnemy::IdleMovement() {}


void ABaseEnemy::BindPlayerTooClose() {}

void ABaseEnemy::PlayerTooClose() {}

void ABaseEnemy::WarnEnemiesInRadius(TArray<ABaseEnemy*>& outCloseEnemies)
{
	FCollisionShape sphereCol = FCollisionShape::MakeSphere(WarnRadius);
	TArray<FHitResult> hitResults;

	bool bSweepHit = GetWorld()->SweepMultiByChannel(hitResults, GetActorLocation(), GetActorLocation(), FQuat::Identity, ECC_Pawn, sphereCol);
	
	for (FHitResult& hitResult : hitResults)
	{
		ABaseEnemy* enemyActor{ Cast<ABaseEnemy>(hitResult.GetActor()) };
		if (IsValid(enemyActor) && !enemyActor->IsEnemyStunned() && enemyActor->GetStatsComponent()->GetHealthPercentage() > FLT_EPSILON)
		{
			outCloseEnemies.AddUnique(enemyActor);
			if (outCloseEnemies.Num() >= 3)
			{
				break;
			}
		}
	}
}

void ABaseEnemy::WhenEnemyDies()
{
	GetCharacterMovement()->DisableMovement();
	BoxCollider->OnComponentBeginOverlap.Clear();
	SphereCollider->OnComponentBeginOverlap.Clear();

	AEnemyManager* enemyManager{ Cast<AEnemyManager>(UGameplayStatics::GetActorOfClass(GetWorld(), AEnemyManager::StaticClass())) };
	if (!IsValid(enemyManager))
	{
		UE_LOG(LogTemp, Fatal, TEXT("enemyManager in BaseEnemy not found!!!"));
	}
	enemyManager->AddToDestroyAfterBattle(this);
}

void ABaseEnemy::WhenEnemyDeathFinished()
{
}

void ABaseEnemy::UnStunEnemy()
{
	OnStunEnd.Broadcast();
	APlayerCharacter* pc{ Cast<APlayerCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0)) };
	if (!(pc->IsPlayerInBattle()) && !(pc->GetIsInteracting()))
	{
		GetCharacterMovement()->Activate();
	}
	IsStunned = false;
	SphereCollider->SetGenerateOverlapEvents(true);
	BindIdle();
}
