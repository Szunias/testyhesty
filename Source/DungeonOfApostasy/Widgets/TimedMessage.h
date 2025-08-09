
#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "../Other/EMessageType.h"
#include "TimedMessage.generated.h"

class UTextBlock;

UCLASS()
class DUNGEONOFAPOSTASY_API UTimedMessage : public UUserWidget
{
	GENERATED_BODY()

public: 

	void SetMessage(const FString& message, EMessageType messageType);
	void StartCountdown(float countdownDelay);

protected:

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (BindWidget))
	UTextBlock* Message_Tb;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Text Colors")
	FLinearColor DefaultColor;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Text Colors")
	FLinearColor PositiveColor;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Text Colors")
	FLinearColor NegativeColor;

};
