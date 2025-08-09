#pragma once

#include "CoreMinimal.h"
#include "../BaseCharacter.h"
#include "BaseEnemy.generated.h"

class AEnemyController;
class UNavigationSystemV1;
class USphereComponent;
class UBoxComponent;

UCLASS()
class DUNGEONOFAPOSTASY_API ABaseEnemy : public ABaseCharacter
{
	GENERATED_BODY()
public:
	ABaseEnemy();


	UPROPERTY(VisibleAnywhere)
	USphereComponent* SphereCollider;
	
	UPROPERTY(VisibleAnywhere)
	UBoxComponent* BoxCollider;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnStunBegin);
	UPROPERTY(BlueprintCallable, BlueprintAssignable, Category = "Event")
	FOnStunBegin OnStunBegin;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnStunEnd);
	UPROPERTY(BlueprintCallable, BlueprintAssignable, Category = "Event")
	FOnStunEnd OnStunEnd;

	UFUNCTION()
	void OnSphereOverlapBegin(class UPrimitiveComponent* overlappedComp, class AActor* otherActor, class UPrimitiveComponent* otherComp, int32 otherBodyIndex, bool bFromSweep, const FHitResult& sweepResult);
	
	UFUNCTION()
	void OnSphereOverlapEnd(class UPrimitiveComponent* overlappedComp, class AActor* otherActor, class UPrimitiveComponent* otherComp, int32 otherBodyIndex);

	UFUNCTION()
	void OnBoxOverlapBegin(class UPrimitiveComponent* overlappedComp, class AActor* otherActor, class UPrimitiveComponent* otherComp, int32 otherBodyIndex, bool bFromSweep, const FHitResult& sweepResult);

	float GetBattleRadius() const;

	virtual void OnBattleStart() override;
	virtual void OnBattleEnd() override;

	virtual void PrepareAttack(ABaseCharacter* target) override;
	virtual void PerformAttack() override;
	
	virtual void Attack() override;

	bool IsEnemyStunned();
	virtual bool IsBossEnemy() const;

protected:
	virtual void BeginPlay() override;
	
	//virtual void Tick(float DeltaTime) override;

	virtual void BindIdle();
	virtual void IdleMovement();
	virtual void BindPlayerTooClose();
	virtual void PlayerTooClose();

	virtual void WarnEnemiesInRadius(TArray<ABaseEnemy*>& outCloseEnemies);

	UNavigationSystemV1* NavSystem;

	AEnemyController* EnemyController;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float WarnRadius;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float AddedBattleRadius;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float StunDuration;

	UPROPERTY()
	AActor* PointOfInterest;

	UFUNCTION()
	virtual void WhenEnemyDies();
	UFUNCTION()
	virtual void WhenEnemyDeathFinished();
private:

	UFUNCTION()
	void UnStunEnemy();

	bool IsStunned;

	float BeginSphereRadius;

	
};
