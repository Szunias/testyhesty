#pragma once

#include "CoreMinimal.h"
#include "BaseWidget.h"
#include "InventoryScreenWidget.generated.h"

class UInventoryItemWidget;
class URadialMenu;
class ABaseItem;

UCLASS()
class DUNGEONOFAPOSTASY_API UInventoryScreenWidget : public UBaseWidget
{
	GENERATED_BODY()

public:

	UFUNCTION()
	void SelectItem(ABaseItem* item);

	void IncludeWeaponItems(bool include);

	virtual void InitializeWidget() override;

	void DisableThrow();

	UFUNCTION(BlueprintCallable)
	bool CloseInventoryItem();

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnItemUsed, ABaseItem*, usedItem);
	UPROPERTY(BlueprintCallable, BlueprintAssignable, Category = "UI")
	FOnItemUsed OnItemUsed;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnItemThrown, ABaseItem*, thrownItem);
	UPROPERTY(BlueprintCallable, BlueprintAssignable, Category = "UI")
	FOnItemThrown OnItemThrown;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnBack);
	UPROPERTY(BlueprintCallable, BlueprintAssignable, Category = "Navigation")
	FOnBack OnBack;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnBackRadialMenu);
	UPROPERTY(BlueprintCallable, BlueprintAssignable, Category = "Navigation")
	FOnBackRadialMenu OnBackRadialMenu;

protected:

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (BindWidget))
	URadialMenu* RadialMenu;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (BindWidget))
	UInventoryItemWidget* InventoryItem;

	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	
	UFUNCTION()
	void OnItemUse(ABaseItem* usedItem);

	UFUNCTION()
	void OnItemThrow(ABaseItem* thrownItem);

private:

	bool CanThrowItem;
};
