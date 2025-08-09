#pragma once

#include "CoreMinimal.h"
#include "BaseItem.h"
#include "../Other/ETarotCards.h"
#include "ItemTarotBox.generated.h"

class UTarotSpawnerComponent;
class APlayerCharacter;

UCLASS()
class DUNGEONOFAPOSTASY_API AItemTarotBox : public ABaseItem
{
	GENERATED_BODY()
	
public:
	AItemTarotBox();

	virtual void Use() override;
	virtual void Throw() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tarot Effects")
	int StrengthBuff;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tarot Effects")
	int SpeedBuff;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tarot Effects")
	int LuckBuff;

protected:
	UPROPERTY()
	UTarotSpawnerComponent* TarotSpawnerComponent;

	virtual void BeginPlay() override;

private:
	UFUNCTION()
	void HandleCardsDrawn(ETarotCards pickedCard, float effectDelay);

	UPROPERTY()
	APlayerCharacter* PlayerCharacter;
};
