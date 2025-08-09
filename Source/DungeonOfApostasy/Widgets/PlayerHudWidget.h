#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PlayerHudWidget.generated.h"

class UVerticalHealthBar;

UCLASS()
class DUNGEONOFAPOSTASY_API UPlayerHudWidget : public UUserWidget
{
	GENERATED_BODY()

public:

	UFUNCTION()
	void SetCurrentHealthPercentage(float percentage);

protected:

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (BindWidget))
	UVerticalHealthBar* VerticalHealthBar;
	
};
