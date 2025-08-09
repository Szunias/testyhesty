#pragma once

#include "CoreMinimal.h"
#include "BaseWidget.h"
#include "WinScreen.generated.h"

class UMenuButton;

UCLASS()
class DUNGEONOFAPOSTASY_API UWinScreen : public UBaseWidget
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable)
	virtual void InitializeWidget() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName MainMenuLevelName;
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	UMenuButton* MainMenu_Btn;

private:
	UFUNCTION()
	void GoToMainMenu();

};
