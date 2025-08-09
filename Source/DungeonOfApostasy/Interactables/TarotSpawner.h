#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "../Other/ETarotCards.h"
#include "../Other/ECardValue.h"
#include "TarotSpawner.generated.h"

class UTarotScreen;
class UTexture2D;
class UBaseWidget;

UCLASS()
class DUNGEONOFAPOSTASY_API ATarotSpawner : public AActor
{
	GENERATED_BODY()
	
public:	
	ATarotSpawner();

	void SetAllowedCards(const TArray<ETarotCards>& cards);

	void PickCard(int index, float effectDelay);

	UFUNCTION(BlueprintCallable)
	void DebugPrintDrawnCards();

	const TArray<ETarotCards>& GetDrawnCards();

	void DestroySpawner();

	void SetTarotTextImage(UTexture2D* newImage);

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FResultHandler, ETarotCards, pickedCard, float, effectDelay);
	UPROPERTY(BlueprintAssignable)
	FResultHandler ResultHandlerDelegate;

	int Luck;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<ETarotCards, ECardValue> CardsValue;

	UPROPERTY(EditAnywhere, blueprintReadWrite)
	TSubclassOf<UTarotScreen> TarotScreenClass;

protected:
	virtual void BeginPlay() override;

private:
	void DrawTheCards();

	ETarotCards DrawRandomCardFromPile(TArray<ETarotCards>& pileToDrawFrom, const TArray<ETarotCards>& refillPile);
	void PitySystem(TArray<ETarotCards>& pileToDrawFrom, const TArray<ETarotCards>& refillPile);

	TArray<ETarotCards> AllowedCards;
	TArray<ETarotCards> PositiveAllowedCards;
	TArray<ETarotCards> NegativeAllowedCards;
	TArray<ETarotCards> DrawnCards;

	int MaxAmountOfCards;

	UPROPERTY()
	UTarotScreen* TarotScreenWidget;
};
