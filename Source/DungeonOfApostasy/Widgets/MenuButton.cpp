
#include "MenuButton.h"
#include "ControllerButton.h"
#include "Math/Color.h"
#include "Engine/Texture2D.h"
#include "Styling/SlateBrush.h"
#include "Styling/WidgetStyle.h"
#include "Blueprint/WidgetTree.h"
#include "Components/textblock.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/Image.h"

void UMenuButton::InitializeButton()
{
	FButtonStyle buttonStyle{ Button->GetStyle() };

	FSlateBrush normalBrush{};
	normalBrush.SetResourceObject(NormalTexture);
	normalBrush.TintColor = NormalColor;
	buttonStyle.SetNormal(normalBrush);

	FSlateBrush hoverBrush{};
	hoverBrush.SetResourceObject(HoverTexture);
	hoverBrush.TintColor = HoverColor;
	buttonStyle.SetHovered(hoverBrush);

	FSlateBrush pressedBrush{};
	pressedBrush.SetResourceObject(PressedTexture);
	pressedBrush.TintColor = PressedColor;
	buttonStyle.SetPressed(pressedBrush);

	FSlateBrush disableBrush{};
	disableBrush.SetResourceObject(DisableTexture);
	disableBrush.TintColor = DisableColor;
	buttonStyle.SetDisabled(disableBrush);

	Button->SetStyle(buttonStyle);
	ButtonTextBlock->SetText(FText::FromString(ButtonText));

	ButtonTextBlock->SetFont(ButtonTextFont);
	ButtonTextBlock->SetColorAndOpacity(ButtonTextColor);
	ButtonTextBlock->SetShadowOffset(ButtonShadowOffset);
	ButtonTextBlock->SetShadowColorAndOpacity(ButtonShadowColor);
	UCanvasPanelSlot* textSlot{ UWidgetLayoutLibrary::SlotAsCanvasSlot(ButtonTextBlock) };
	textSlot->SetPosition(TextPosition);

	//Icon
	Icon->SetBrushFromTexture(IconTexture);
	SetNormalIconColor();
	if (IsValid(IconTexture))
	{
		UCanvasPanelSlot* iconSlot{ UWidgetLayoutLibrary::SlotAsCanvasSlot(Icon) };
		iconSlot->SetPosition(IconPosition);
		iconSlot->SetSize(FVector2D{ static_cast<float>(IconTexture->GetSizeX()), static_cast<float>(IconTexture->GetSizeY()) });		

		FScriptDelegate hoverDelegate;
		hoverDelegate.BindUFunction(this, "SetHoverIconColor");
		Button->OnHovered.AddUnique(hoverDelegate);

		FScriptDelegate normalDelegate;
		normalDelegate.BindUFunction(this, "SetNormalIconColor");
		Button->OnUnhovered.AddUnique(normalDelegate);

		FScriptDelegate pressedDelegate;
		pressedDelegate.BindUFunction(this, "SetPressedIconColor");
		Button->OnPressed.AddUnique(pressedDelegate);
	}	
}

void UMenuButton::ExecuteButton()
{
	if (GetIsEnabled())
	{
		Button->OnClicked.Broadcast();
	}	
}

void UMenuButton::NativeOnAddedToFocusPath(const FFocusEvent& InFocusEvent)
{
	GEngine->AddOnScreenDebugMessage(-1, 0.5f, FColor::Cyan, "Button gained focus " + GetName());
	Button->HoverButton();
	OnButtonFocusGained.Broadcast(this);
}

void UMenuButton::NativeOnRemovedFromFocusPath(const FFocusEvent& InFocusEvent)
{
	GEngine->AddOnScreenDebugMessage(-1, 0.5f, FColor::Cyan, "Button lost focus " + GetName());
	Button->UnhoverButton();
	OnButtonFocusLost.Broadcast(this);
}

UButton* UMenuButton::GetButton()
{
	return Button;
}

void UMenuButton::SetButtonText(const FString& text)
{
	ButtonTextBlock->SetText(FText::FromString(text));
}

void UMenuButton::EnableButton(bool enable)
{
	SetIsEnabled(enable);
	Button->SetIsEnabled(enable);
}

void UMenuButton::ClickButton()
{
	Button->ClickButton();
}

void UMenuButton::SetHoverIconColor()
{
	Icon->SetColorAndOpacity(IconHoverColor);
}

void UMenuButton::SetNormalIconColor()
{
	Icon->SetColorAndOpacity(IconNormalColor);
}

void UMenuButton::SetPressedIconColor()
{
	Icon->SetColorAndOpacity(IconPressedColor);
}

void UMenuButton::SetEnableIcon(bool enable)
{
	if (enable)
	{
		Icon->SetColorAndOpacity(IconNormalColor);
	}
	else
	{
		Icon->SetColorAndOpacity(IconDisabledColor);
	}
}
