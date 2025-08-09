#pragma once

#include "CoreMinimal.h"
#include "BaseWidget.h"
#include "RadialMenu.generated.h"

class URadialButton;
class UCanvasPanel;
class UImage;
class ABaseItem;
class APlayerCharacter;
class UCanvasPanelSlot;

USTRUCT(BlueprintType)
struct FRadialButtonValues
{
	GENERATED_BODY()

	float ButtonAngle;
	float ButtonRange;
};

UCLASS()
class DUNGEONOFAPOSTASY_API URadialMenu : public UBaseWidget
{

	GENERATED_BODY()

public:

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnItemSelected, ABaseItem*, item);
	UPROPERTY(BlueprintCallable, BlueprintAssignable, Category = "Inventory")
	FOnItemSelected OnItemSelected;

	virtual void InitializeWidget() override;
	void IncludeWeapons(bool include = true);

	UFUNCTION(BlueprintCallable)
	void RestoreButtonFocus();
	
protected:

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Radial")
	int Radius;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Radial")
	int LineRadius;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Radial")
	int AmountOfItems;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Radial")
	float ItemSize;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, meta = (UIMin = "0.0", UIMax = "300.0"), Category = "Radial")
	int StartAngle;

	UFUNCTION(BlueprintCallable)
	void ConstructRadialMenu(bool isDesignTime);

	UFUNCTION(BlueprintCallable)
	void SelectItem(FVector2D vector);

	UFUNCTION(BlueprintCallable)
	void ClickItem();

	UFUNCTION(BlueprintCallable)
	void OnRadialHover(URadialButton* button);

	UFUNCTION(BlueprintCallable)
	void OnRadialClicked(URadialButton* button);

	virtual void NativeOnAddedToFocusPath(const FFocusEvent& InFocusEvent);
	virtual void NativeOnRemovedFromFocusPath(const FFocusEvent& InFocusEvent);

	TMap<URadialButton*, FRadialButtonValues> ButtonLocations;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (BindWidget))
	UCanvasPanel* Canvas;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (BindWidget))
	UImage* Image;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (BindWidget))
	UImage* Arrow;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	UTexture2D* RadialLineTexture;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Radial")
	float ArrowRotationSpeed;

private:

	void AddDivisionLine(float angle, FVector2D alignment, FAnchors anchors);

	void WrapAngle(float& angle) const;

	void ApplyRadialButtonSettings(URadialButton* button, ABaseItem* item, UCanvasPanelSlot* slot);

	void StartArrowLerp();

	void LerpArrow();

	UPROPERTY()
	URadialButton* SelectedButton;

	UPROPERTY()
	APlayerCharacter* Player;

	FVector InitialRotationAxis;

	bool IsLerping;
	bool IsWidgetFocused;

	bool ShouldIncludeWeapons;
};
