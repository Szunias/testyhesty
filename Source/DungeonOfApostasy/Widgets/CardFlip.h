#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CardFlip.generated.h"

class UImage;

UCLASS()
class DUNGEONOFAPOSTASY_API UCardFlip : public UUserWidget
{
	GENERATED_BODY()
	
public:

	UFUNCTION()
	void ForceStartFlip();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	UImage* FrontCover;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FStartFlip);
	UPROPERTY(BlueprintAssignable, BlueprintCallable)
	FStartFlip OnStartCardFlip;
	
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FEndFlip);
	UPROPERTY(BlueprintAssignable, BlueprintCallable)
	FEndFlip OnEndCardFlip;

};
