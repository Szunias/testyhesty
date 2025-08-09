#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "VerticalHealthBar.generated.h"

class UProgressBar;

UCLASS()
class DUNGEONOFAPOSTASY_API UVerticalHealthBar : public UUserWidget
{
	GENERATED_BODY()

public:
	void UpdateHealthPercentage(float percentage);

protected:


	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (BindWidget))
	UProgressBar* HealthBar;
	
};
