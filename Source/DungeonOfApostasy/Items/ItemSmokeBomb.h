#pragma once

#include "CoreMinimal.h"
#include "BaseItem.h"
#include "ItemSmokeBomb.generated.h"

class ABaseCharacter;
class APlayerCharacter;
class USphereComponent;
class UNiagaraComponent;

UCLASS()
class DUNGEONOFAPOSTASY_API AItemSmokeBomb : public ABaseItem
{
	GENERATED_BODY()
public:
	AItemSmokeBomb();

	virtual void Use() override;
	virtual void Throw() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float SmokeDuration;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int BattleTurnDuration;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int EvasivenessBoost;

private:
	virtual void BeginPlay() override;
	bool IsBattleActive();

	void OverworldEffect(FVector location);
	void BattleEffect(ABaseCharacter* character);

	void EndItem();

	APlayerCharacter* GetPlayerCharacter();

	UFUNCTION()
	void OnSmokeSphereOverlapBegin(class UPrimitiveComponent* overlappedComp, class AActor* otherActor, class UPrimitiveComponent* otherComp, int32 otherBodyIndex, bool bFromSweep, const FHitResult& sweepResult);

	UFUNCTION()
	void OnSmokeSphereOverlapEnd(class UPrimitiveComponent* overlappedComp, class AActor* otherActor, class UPrimitiveComponent* otherComp, int32 otherBodyIndex);

	UFUNCTION()
	void OnThrowOverlapBegin(class UPrimitiveComponent* overlappedComp, class AActor* otherActor, class UPrimitiveComponent* otherComp, int32 otherBodyIndex, bool bFromSweep, const FHitResult& sweepResult);
	
	UFUNCTION()
	void OnThrowHit(class UPrimitiveComponent* hitComponent, AActor* otherActor, UPrimitiveComponent* otherComponent, FVector NormalImpulse, const FHitResult& hit);

	UFUNCTION()
	void ResetBattleEffect();

	void Explode();

	UPROPERTY()
	USphereComponent* SmokeSphere;

	UPROPERTY()
	UNiagaraComponent* SmokeParticles;

	ABaseCharacter* ModifiedCharacter;
};
