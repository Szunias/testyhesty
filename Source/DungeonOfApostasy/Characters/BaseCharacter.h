#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "../Other/FTurnEffect.h"
#include "BaseCharacter.generated.h"

class UStatsComponent;
class UTexture2D;

UCLASS()
class DUNGEONOFAPOSTASY_API ABaseCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	ABaseCharacter();

	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UStatsComponent* GetStatsComponent();

	ABaseCharacter* GetTargettedEnemy();

	const FString& GetCharacterName() const;

	virtual void OnBattleStart();
	virtual void OnBattleEnd();

	virtual void PrepareAttack(ABaseCharacter* target);

	virtual void Attack();

	UFUNCTION()
	virtual void PerformAttack();

	UFUNCTION(BlueprintCallable)
	UTexture2D* GetCharacterImage();

	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnAttack);
	UPROPERTY(BlueprintCallable, BlueprintAssignable, Category = "Battle")
	FOnAttack OnAttack;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnAttackConnects);
	UPROPERTY(BlueprintCallable, BlueprintAssignable, Category = "Battle")
	FOnAttackConnects OnAttackConnects;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnAttackFinished);
	UPROPERTY(BlueprintCallable, BlueprintAssignable, Category = "Battle")
	FOnAttackFinished OnAttackFinished;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnApplyEffect, ABaseCharacter*, target, FTurnEffect, effect);
	UPROPERTY(BlueprintCallable, BlueprintAssignable, Category = "Battle")
	FOnApplyEffect OnApplyEffect;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnBattleMessage, FString, message, EMessageType, effectType);
	UPROPERTY(BlueprintCallable, BlueprintAssignable, Category = "Battle")
	FOnBattleMessage OnBattleMessage;

protected:

	virtual void BeginPlay() override;

	UPROPERTY()
	UStatsComponent* StatsComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	UTexture2D* CharacterImage;

	UPROPERTY()
	ABaseCharacter* TargettedEnemy;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	FString CharacterName;
};
