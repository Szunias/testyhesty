
#pragma once

#include "CoreMinimal.h"
#include "BaseWidget.h"
#include "GameOverScreen.generated.h"

class UMenuButton;

UCLASS()
class DUNGEONOFAPOSTASY_API UGameOverScreen : public UBaseWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	virtual void InitializeWidget() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName BaseLevelName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName MainMenuLevelName;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	UMenuButton* Retry_Btn;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	UMenuButton* GiveUp_Btn;

private:
	UFUNCTION()
	void RetryGame();

	UFUNCTION()
	void GiveUp();
};
