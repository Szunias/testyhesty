#pragma once

#include "CoreMinimal.h"
#include "BaseWidget.h"
#include "SpawnedItemScreen.generated.h"

class UButton;
class UImage;
class UTextBlock;
class ABaseItem;
class UMenuButton;

UCLASS()
class DUNGEONOFAPOSTASY_API USpawnedItemScreen : public UBaseWidget
{
	GENERATED_BODY()
public:
	void InitializeWidget(bool HasEnoughSpaceInInventory, ABaseItem* itemToShow);

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FButtonResult, bool, pickUp);
	UPROPERTY(BlueprintAssignable)
	FButtonResult OnOptionPicked;

protected:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (BindWidget))
	UTextBlock* ItemName_Txt;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (BindWidget))
	UTextBlock* ItemDescription_Txt;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (BindWidget))
	UImage* Item_Img;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	UMenuButton* PickUp_Btn;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	UMenuButton* Drop_Btn;

private:
	virtual void InitializeWidget() override;

};
