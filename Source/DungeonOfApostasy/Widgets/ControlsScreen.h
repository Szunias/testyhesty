#pragma once

#include "CoreMinimal.h"
#include "BaseWidget.h"
#include "ControlsScreen.generated.h"

class UButton;
class UImage;
class UMenuButton;

UCLASS()
class DUNGEONOFAPOSTASY_API UControlsScreen : public UBaseWidget
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable)
	virtual void InitializeWidget() override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	UMenuButton* GoBack_Btn;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (BindWidget))
	UImage* Display_Img;

private:
	UFUNCTION()
	void GoBack();
	
};
