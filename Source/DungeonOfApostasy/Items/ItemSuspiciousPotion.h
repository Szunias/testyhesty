#pragma once

#include "CoreMinimal.h"
#include "BaseItem.h"
#include "ItemSuspiciousPotion.generated.h"

class ABaseCharacter;

UCLASS()
class DUNGEONOFAPOSTASY_API AItemSuspiciousPotion : public ABaseItem
{
	GENERATED_BODY()
public:
	AItemSuspiciousPotion();

	virtual UTexture2D* GetItemImage() override;

	virtual void Use() override;
	virtual void Throw() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int HealthRangeMin;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int HealthRangeMax;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int StrengthRangeMin;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int StrengthRangeMax;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int LuckRangeMin;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int LuckRangeMax;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	UTexture2D* GoodPotionImage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	UTexture2D* BadPotionImage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool IsFatalPotion;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float SplashRadius;

private:
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnThrowOverlapBegin(class UPrimitiveComponent* overlappedComp, class AActor* otherActor, class UPrimitiveComponent* otherComp, int32 otherBodyIndex, bool bFromSweep, const FHitResult& sweepResult);

	UFUNCTION()
	void OnThrowHit(class UPrimitiveComponent* hitComponent, AActor* otherActor, UPrimitiveComponent* otherComponent, FVector NormalImpulse, const FHitResult& hit);
	
	void Explode();

	void ExecuteEffect(TArray<ABaseCharacter*> effectedEntities);

	UPROPERTY()
	UNiagaraComponent* SplashParticles;

	int HealthEffect;
	int StrengthEffect;
	int LuckEffect;
};
