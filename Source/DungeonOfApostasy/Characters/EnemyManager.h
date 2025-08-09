#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EnemyManager.generated.h"

class ABaseEnemy;

UCLASS()
class DUNGEONOFAPOSTASY_API AEnemyManager : public AActor
{
	GENERATED_BODY()
	
public:	
	AEnemyManager();

	UFUNCTION(BlueprintCallable)
	void StopMovementForAllEnemies();

	UFUNCTION(BlueprintCallable)
	void ResumeMovementForAllEnemies();

	void AddToDestroyAfterBattle(ABaseEnemy* enemyToDestroy);

protected:
	virtual void BeginPlay() override;

private:
	UFUNCTION()
	void RemoveDeadEnemy();

	UFUNCTION()
	void BindRemovingDead();

	UFUNCTION()
	void DestroyEnemiesAfterBattle();

	TArray<ABaseEnemy*> Enemies;

	TArray<ABaseEnemy*> EnemiesToDestroy;
};
