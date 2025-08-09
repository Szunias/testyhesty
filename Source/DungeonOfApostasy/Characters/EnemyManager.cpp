
#include "EnemyManager.h"
#include "Kismet/GameplayStatics.h"
#include "Enemies/BaseEnemy.h"
#include "StatsComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "../Battle/BattleArea.h"

AEnemyManager::AEnemyManager()
{
	PrimaryActorTick.bCanEverTick = false;
}

void AEnemyManager::BeginPlay()
{
	Super::BeginPlay();

	TArray<AActor*> enemies{};
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ABaseEnemy::StaticClass(), enemies);

	for (AActor* currEnemy : enemies)
	{
		ABaseEnemy* enemy{ Cast<ABaseEnemy>(currEnemy) };
		Enemies.Add(enemy);
	}

	FTimerHandle handle{};
	GetWorld()->GetTimerManager().SetTimer(handle, this, &AEnemyManager::BindRemovingDead, 2.0f);

	ABattleArea* battleArea{ Cast<ABattleArea>(UGameplayStatics::GetActorOfClass(GetWorld(), ABattleArea::StaticClass())) };
	if (!IsValid(battleArea))
	{
		UE_LOG(LogTemp, Fatal, TEXT("battleArea in EnemyManager not found!!!"));
	}
	battleArea->OnBattleEnd.AddDynamic(this, &AEnemyManager::DestroyEnemiesAfterBattle);
}

void AEnemyManager::RemoveDeadEnemy()
{
	for (ABaseEnemy* currEnemy : Enemies)
	{
		UStatsComponent* statsComp{ currEnemy->GetStatsComponent() };
		if (statsComp->GetHealthPercentage() < FLT_EPSILON)
		{
			// dead enemy found, remove it and stop
			Enemies.Remove(currEnemy);
			break;
		}
	}
}

void AEnemyManager::BindRemovingDead()
{
	for (ABaseEnemy* currEnemy : Enemies)
	{
		UStatsComponent* statsComp{ currEnemy->GetStatsComponent() };
		statsComp->OnDeath.AddDynamic(this, &AEnemyManager::RemoveDeadEnemy);
	}
}

void AEnemyManager::AddToDestroyAfterBattle(ABaseEnemy* enemyToDestroy)
{
	EnemiesToDestroy.Add(enemyToDestroy);
}

void AEnemyManager::DestroyEnemiesAfterBattle()
{
	for (ABaseEnemy* currEnemy : EnemiesToDestroy)
	{
		currEnemy->Destroy();
	}

	EnemiesToDestroy.Empty();
}

void AEnemyManager::StopMovementForAllEnemies()
{
	for (ABaseEnemy* currEnemy : Enemies)
	{
		currEnemy->GetCharacterMovement()->Deactivate();
	}
}

void AEnemyManager::ResumeMovementForAllEnemies()
{
	for (ABaseEnemy* currEnemy : Enemies)
	{
		if (currEnemy->IsEnemyStunned()) // don't enable when stunned
		{
			continue;
		}
		currEnemy->GetCharacterMovement()->Activate();
	}
}