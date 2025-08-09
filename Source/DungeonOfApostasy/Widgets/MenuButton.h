#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MenuButton.generated.h"

class UControllerButton;
class UTexture2D;
struct FLinearColor;
class UTextBlock;
class UButton;
class UImage;

UCLASS()
class DUNGEONOFAPOSTASY_API UMenuButton : public UUserWidget
{
	GENERATED_BODY()


public:
	UButton* GetButton();

	void SetButtonText(const FString& text);

	void EnableButton(bool enable);


	UFUNCTION(BlueprintCallable)
	void ClickButton();

	UFUNCTION()
	void SetHoverIconColor();

	UFUNCTION()
	void SetNormalIconColor();

	UFUNCTION()
	void SetPressedIconColor();
	
	void SetEnableIcon(bool enable);

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnFocusGained, UMenuButton*, button);
	UPROPERTY(BlueprintCallable, BlueprintAssignable, Category = "Focus")
	FOnFocusGained OnButtonFocusGained;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnFocusLost, UMenuButton*, button);
	UPROPERTY(BlueprintCallable, BlueprintAssignable, Category = "Focus")
	FOnFocusGained OnButtonFocusLost;

protected:

	UFUNCTION(BlueprintCallable)
	void InitializeButton();

	UFUNCTION(BlueprintCallable)
	void ExecuteButton();

	virtual void NativeOnAddedToFocusPath(const FFocusEvent& InFocusEvent);
	virtual void NativeOnRemovedFromFocusPath(const FFocusEvent& InFocusEvent);

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (BindWidget))
	UControllerButton* Button;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (BindWidget))
	UTextBlock* ButtonTextBlock;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "ButtonText")
	FString ButtonText;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "ButtonText")
	FVector2D TextPosition;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "ButtonTextures")
	UTexture2D* NormalTexture;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "ButtonTextures")
	FLinearColor NormalColor;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "ButtonTextures")
	UTexture2D* HoverTexture;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "ButtonTextures")
	FLinearColor HoverColor;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "ButtonTextures")
	UTexture2D* PressedTexture;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "ButtonTextures")
	FLinearColor PressedColor;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "ButtonTextures")
	UTexture2D* DisableTexture;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "ButtonTextures")
	FLinearColor DisableColor;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "ButtonText")
	FSlateFontInfo ButtonTextFont;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "ButtonText")
	FLinearColor ButtonTextColor;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "ButtonText")
	FVector2D ButtonShadowOffset;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "ButtonText")
	FLinearColor ButtonShadowColor;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "ButtonIcon", meta = (BindWidget))
	UImage* Icon;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "ButtonIcon")
	UTexture2D* IconTexture;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "ButtonIcon")
	FVector2D IconPosition;	

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "ButtonIcon")
	FLinearColor IconNormalColor;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "ButtonIcon")
	FLinearColor IconHoverColor;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "ButtonIcon")
	FLinearColor IconPressedColor;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "ButtonIcon")
	FLinearColor IconDisabledColor;

};
