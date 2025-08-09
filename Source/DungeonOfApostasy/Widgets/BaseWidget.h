#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "BaseWidget.generated.h"

class APlayerController;
class UMenuButton;

UCLASS()
class DUNGEONOFAPOSTASY_API UBaseWidget : public UUserWidget
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable)
	void SetParentWidget(UBaseWidget* widget);

	UFUNCTION()
	void DisableAutoRegainFocus();

	UFUNCTION(BlueprintCallable)
	void AddWidget();

	UFUNCTION(BlueprintCallable)
	void RestoreFocus(APlayerController* controller = nullptr);

	UFUNCTION(BlueprintCallable)
	void RemoveWidget();

	virtual void InitializeWidget();

	void HideMouse();

	
protected:

	UPROPERTY()
	UBaseWidget* PreviousWidget;

	UPROPERTY()
	UMenuButton* ActiveButton;

	virtual void NativeDestruct() override;

	virtual void OnWidgetFocused();

	UFUNCTION()
	virtual void SetActiveButton(UMenuButton* button);

	UFUNCTION()
	virtual void SetButtonActiveAndFocus(UMenuButton* button);

	bool ShouldAutoRegainFocus;

	virtual void NativeOnAddedToFocusPath(const FFocusEvent& InFocusEvent);
	virtual void NativeOnRemovedFromFocusPath(const FFocusEvent& InFocusEvent);
};
