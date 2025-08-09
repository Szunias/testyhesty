#pragma once

#include "CoreMinimal.h"
#include "BaseWidget.h"
#include "MainMenu.generated.h"

class UMenuButton;
class UControlsScreen;

UCLASS()
class DUNGEONOFAPOSTASY_API UMainMenu : public UBaseWidget
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable)
	virtual void InitializeWidget() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName LevelNameToOpen;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<UControlsScreen> ControlsScreenClass;

protected:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	UMenuButton* Play_Btn;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	UMenuButton* Controls_Btn;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	UMenuButton* Quit_Btn;

private:
	UFUNCTION()
	void PlayGame();

	UFUNCTION()
	void ShowControls();

	UFUNCTION()
	void QuitGame();
};
