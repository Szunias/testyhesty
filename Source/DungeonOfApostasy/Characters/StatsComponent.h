#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "StatsComponent.generated.h"

class UNiagaraComponent;
class UNiagaraSystem;
class ABaseCharacter;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class DUNGEONOFAPOSTASY_API UStatsComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UStatsComponent();

	// Health
	int GetCurrentHealth();
	void UpdateHealth(int value);
	float GetHealthPercentage();

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnUpdateHealth, float, healthPercentage);
	UPROPERTY(BlueprintCallable, BlueprintAssignable, Category = "Health")
	FOnUpdateHealth OnUpdateHealth;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDamaged);
	UPROPERTY(BlueprintCallable, BlueprintAssignable, Category = "Health")
	FOnDamaged OnTakenDamage;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDeath);
	UPROPERTY(BlueprintCallable, BlueprintAssignable, Category = "Health")
	FOnDeath OnDeath;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDeathFinished);
	UPROPERTY(BlueprintCallable, BlueprintAssignable, Category = "Health")
	FOnDeathFinished OnDeathFinished;

	// Speed
	int GetCurrentSpeed();
	void UpdateSpeed(int value);

	// Luck
	int GetCurrentLuck();
	void UpdateLuck(int value);


	// Strength
	int GetCurrentStrength();
	int GetCurrentStrengthRange();

	int GetDamage();
	void UpdateStrength(int value);


	// Evasiveness
	int GetCurrentEvasiveness();
	void UpdateEvasiveness(int value);

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	// Health
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int MaxHealth;

	// Speed
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int CurrentSpeed;

	// Luck
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int CurrentLuck;

	// Strength

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int CurrentStrength;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int StrengthRange;

	UPROPERTY()
	UNiagaraComponent* EffectHealthParticles;

	UPROPERTY()
	UNiagaraComponent* EffectStrengthParticles;

	UPROPERTY()
	UNiagaraComponent* EffectLuckParticles;

	UPROPERTY()
	UNiagaraComponent* EffectSpeedParticles;

	UPROPERTY()
	UNiagaraComponent* EffectEvasionParticles;

	// Evasiveness
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int CurrentEvasiveness;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effects");
	UNiagaraSystem* HealthUp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effects");
	UNiagaraSystem* HealthDown;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effects");
	UNiagaraSystem* SpeedUp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effects");
	UNiagaraSystem* SpeedDown;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effects");
	UNiagaraSystem* LuckUp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effects");
	UNiagaraSystem* LuckDown;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effects");
	UNiagaraSystem* StrengthUp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effects");
	UNiagaraSystem* StrengthDown;

private:

	// Health
	int CurrentHealth;

	TQueue<UNiagaraSystem*> EffectQueue;

	void AddEffect(UNiagaraSystem* system, UNiagaraComponent* component);

	ABaseCharacter* GetBaseCharacter();
};
