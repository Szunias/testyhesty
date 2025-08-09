#pragma once

#include "CoreMinimal.h"
#include "BaseWidget.h"
#include "InventoryItemWidget.generated.h"

class UCanvasPanel;
class UImage;
class UButton;
class UTextBlock;
class ABaseItem;
class APlayerCharacter;
class UMenuButton;

UCLASS()
class DUNGEONOFAPOSTASY_API UInventoryItemWidget : public UBaseWidget
{
	GENERATED_BODY()


public:
	void SetCurrentItem(ABaseItem* item);

	void EnableThrow(bool enable = true);

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnItemUsed, ABaseItem*, usedItem);
	UPROPERTY(BlueprintCallable, BlueprintAssignable, Category = "UI")
	FOnItemUsed OnItemUsed;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnItemThrown, ABaseItem*, thrownItem);
	UPROPERTY(BlueprintCallable, BlueprintAssignable, Category = "UI")
	FOnItemThrown OnItemThrown;

protected:

	void SetItemName(const FString& name);
	void SetItemDescription(const FString& description);

	UFUNCTION()
	void OnItemUse();

	UFUNCTION()
	void OnItemThrow();

	virtual void NativeConstruct() override;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (BindWidget))
	UCanvasPanel* Canvas;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (BindWidget))
	UImage* Background_Img;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (BindWidget))
	UMenuButton* Use_Btn;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (BindWidget))
	UMenuButton* Throw_Btn;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (BindWidget))
	UTextBlock* Item_Txt;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (BindWidget))
	UTextBlock* Description_Txt;

	UPROPERTY()
	ABaseItem* CurrentSelectedItem;

private:

	UPROPERTY()
	APlayerCharacter* Player;
};
