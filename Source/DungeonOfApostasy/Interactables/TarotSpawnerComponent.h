#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "../Other/ETarotCards.h"
#include "TarotSpawnerComponent.generated.h"

class ATarotSpawner;
class UTexture2D;
class UBaseWidget;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class DUNGEONOFAPOSTASY_API UTarotSpawnerComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UTarotSpawnerComponent();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<ATarotSpawner> TarotSpawnerClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<ETarotCards, UTexture2D*> CardTextImages;

	ATarotSpawner* DrawCards(const TArray<ETarotCards>& AllowedCards, int luckToUse = -1);

	void PickCard(int index);

	void SetTarotTextImage(UTexture2D* newImage);

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY()
	ATarotSpawner* CachedSpawner;
};
