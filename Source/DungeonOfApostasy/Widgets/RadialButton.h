#pragma once

#include "CoreMinimal.h"
#include "Components/Button.h"
#include "RadialButton.generated.h"

UCLASS()
class DUNGEONOFAPOSTASY_API URadialButton : public UButton
{
	GENERATED_BODY()

public:

	void Initialize();

	void OnHover(bool notify = true);
	void OnUnhover(bool notify = true);
	void OnPressed();

	UFUNCTION()
	void OnButtonClicked();

	UFUNCTION()
	void OnButtonHover();

	UFUNCTION()
	void OnButtonUnhover();

	UFUNCTION()
	void OnItemClicked();

	void SetButtonIdx(int idx);
	int GetButtonIdx() const;

	

	void SetDefaultStyles(const FSlateBrush& normal, const FSlateBrush& hovered, const FSlateBrush& pressed);

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnRadialHover, URadialButton*, button);
	UPROPERTY(BlueprintCallable, BlueprintAssignable, Category = "UI")
	FOnRadialHover OnRadialHover;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnRadialClicked, URadialButton*, button);
	UPROPERTY(BlueprintCallable, BlueprintAssignable, Category = "UI")
	FOnRadialClicked OnRadialClicked;

private:
	FButtonStyle ButtonStyle;

	FSlateBrush NormalStyle;
	FSlateBrush HoverStyle;
	FSlateBrush PressedStyle;
	int ButtonIdx;
};
