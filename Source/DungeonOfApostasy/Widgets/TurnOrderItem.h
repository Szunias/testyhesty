#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "TurnOrderItem.generated.h"

class UImage;
class UTexture2D;
class USizeBox;

UCLASS()
class DUNGEONOFAPOSTASY_API UTurnOrderItem : public UUserWidget
{
	GENERATED_BODY()

public:

	void SetImageColor(const FLinearColor& color);
	void SetImage(UTexture2D* image);
	void SetImageSize(float size);

	void SetBoxSize(float size);

protected:
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (BindWidget))
	UImage* Item_Img;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (BindWidget))
	USizeBox* SizeBox;
};
