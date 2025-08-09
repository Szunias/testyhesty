#pragma once

#include "CoreMinimal.h"
#include "BaseEnemy.h"
#include "BossRat.generated.h"

class UTarotSpawnerComponent;
class ATargetPoint;
class UNiagaraComponent;

UCLASS()
class DUNGEONOFAPOSTASY_API ABossRat : public ABaseEnemy
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float MovementSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack Chances")
	int NormalAttackChance;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack Chances")
	int TarotAttackChance;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack Chances")
	int ChargeAttackChance;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tarot Attack")
	int StrengthBuff;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tarot Attack")
	int SpeedBuff;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tarot Attack")
	int LuckBuff;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Charge Attack")
	int MaxAmountOfCharges;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool CanRoar;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ATargetPoint* PatrolPoint;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnTarotAttack);
	UPROPERTY(BlueprintCallable, BlueprintAssignable, Category = "Battle")
	FOnTarotAttack OnTarotAttack;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnChargeAttack);
	UPROPERTY(BlueprintCallable, BlueprintAssignable, Category = "Battle")
	FOnChargeAttack OnChargeAttack;
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnReleaseCharges);
	UPROPERTY(BlueprintCallable, BlueprintAssignable, Category = "Battle")
	FOnReleaseCharges OnReleaseCharges;

	virtual void PrepareAttack(ABaseCharacter* target) override;

	virtual void PerformAttack() override;

	virtual bool IsBossEnemy() const override;

	virtual void OnBattleStart() override;
	virtual void OnBattleEnd() override;

protected:
	virtual void BeginPlay() override;

	virtual void BindIdle() override;
	virtual void BindPlayerTooClose() override;
	virtual void IdleMovement() override;
	virtual void PlayerTooClose() override;

	void RotateCorrectly();

	UPROPERTY()
	UTarotSpawnerComponent* TarotSpawnerComponent;

private:
	UFUNCTION(BlueprintCallable)
	void NormalAttack();

	UFUNCTION(BlueprintCallable)
	void TarotDrawAttack();

	UFUNCTION()
	void HandleCardsDrawn(ETarotCards pickedCard, float effectDelay);

	UFUNCTION(BlueprintCallable)
	void Charging();

	UFUNCTION(BlueprintCallable)
	void FinishCharging();

	virtual void WhenEnemyDeathFinished() override;

	int CurrentCharges;

	UPROPERTY()
	UNiagaraComponent* Charge01Particles;
	UPROPERTY()
	UNiagaraComponent* Charge02Particles;
	UPROPERTY()
	UNiagaraComponent* Charge03Particles;
	UPROPERTY()
	UNiagaraComponent* Charge04Particles;
	UPROPERTY()
	UNiagaraComponent* Charge05Particles;
	
	UPROPERTY()
	TArray<UNiagaraComponent*> ChargeParticles;
};
