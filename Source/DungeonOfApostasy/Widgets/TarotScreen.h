#pragma once

#include "CoreMinimal.h"
#include "BaseWidget.h"
#include "../Other/ETarotCards.h"
#include "TarotScreen.generated.h"


class UButton;
class UTexture2D;
class ATarotSpawner;
class UImage;
class UMenuButton;
class UCardFlip;
class UCanvasPanel;

UCLASS()
class DUNGEONOFAPOSTASY_API UTarotScreen : public UBaseWidget
{
	GENERATED_BODY()
public:

	void InitializeWidget(ATarotSpawner* tarotSpawner);

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float DelayBetweenReveal;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float DurationForDestroy;

	void SetTarotTextImage(UTexture2D* newImage);

protected:

	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FEnterAnimationEnd);
	UPROPERTY(BlueprintAssignable, BlueprintCallable)
	FEnterAnimationEnd OnEnterAnimationEnd;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FExitAnimationBegin);
	UPROPERTY(BlueprintAssignable, BlueprintCallable)
	FExitAnimationBegin OnExitAnimationBegin;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FTextShowBegin);
	UPROPERTY(BlueprintAssignable, BlueprintCallable)
	FTextShowBegin OnTextShowBegin;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	UCanvasPanel* Canvas;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	UMenuButton* Card01_Btn;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	UMenuButton* Card02_Btn;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	UMenuButton* Card03_Btn;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	UMenuButton* Card04_Btn;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	UMenuButton* Card05_Btn;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	UImage* Text_Img;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	UImage* CardFront1_Img;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	UImage* CardFront2_Img;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	UImage* CardFront3_Img;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	UImage* CardFront4_Img;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	UImage* CardFront5_Img;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UTexture2D* CardBackTexture;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<ETarotCards, UTexture2D*> CardsImagesMap;


	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<UCardFlip> NormalFlipClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<UCardFlip> PickedFlipClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<UCardFlip> CardShrinkClass;


private:
	virtual void InitializeWidget() override;

	UFUNCTION()
	void EnterAnimationFinished();

	UFUNCTION()
	void PlayExitAnimationWithDelay();

	UFUNCTION()
	void AllCardsRevealed();

	UFUNCTION()
	void OnButtonClick(int index);

	void SetEnableAllCards(bool isEnabled);

	void ChangeAllCardsVisibility(bool isHidden);

	void PickCard(int index);

	void DestroySystem();

	void RevealAllCards();


	UFUNCTION()
	void GrowCard(int index);
	UFUNCTION()
	void ShrinkCard(int index);

	TArray<UMenuButton*> CardButtons;

	TArray<UCardFlip*> FlipButtons;

	TArray<UImage*> CardFrontImages;

	TArray<ETarotCards> DrawnCards;

	UPROPERTY()
	ATarotSpawner* TarotSpawner;

	bool HasCardBeenPicked;

	float GrowthStrength;

	int PickedIndex;
};
