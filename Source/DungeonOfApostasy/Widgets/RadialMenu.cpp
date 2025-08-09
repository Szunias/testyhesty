// Fill out your copyright notice in the Description page of Project Settings.


#include "RadialMenu.h"
#include "RadialButton.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "Components/CanvasPanelSlot.h"
#include "Blueprint/WidgetTree.h"
#include "Components/CanvasPanel.h"
#include "Components/TextBlock.h"
#include "../Characters/Player/PlayerCharacter.h"
#include "../Characters/InventoryComponent.h"
#include "../Items/BaseItem.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "Styling/SlateBrush.h"
#include "Styling/WidgetStyle.h"
#include "Engine/Texture2D.h"
#include "Input/Reply.h"
#include "Components/Image.h"
#include "../Items/BaseWeapon.h"
#include "Kismet/GameplayStatics.h"

void URadialMenu::InitializeWidget()
{
	ConstructRadialMenu(false);
}

void URadialMenu::ConstructRadialMenu(bool isDesignTime)
{
	IsWidgetFocused = true;
	IsLerping = false;
	UInventoryComponent* inventory{ nullptr };
	int amountOfItems{ AmountOfItems };
	int maxWeight{ AmountOfItems };
	ButtonLocations.Empty();

	if (!isDesignTime)
	{
		Player = Cast<APlayerCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
		if (!IsValid(Player))
		{
			return;
		}
		inventory = Player->GetInventory();
		if (!IsValid(inventory))
		{
			return;
		}
		amountOfItems = inventory->GetCurrentItemAmount();
		maxWeight = inventory->GetMaxWeight();
	}

	TArray<URadialButton*> buttons;

	for (int i{ 0 }; i < amountOfItems; ++i)
	{
		URadialButton* button{ WidgetTree->ConstructWidget<URadialButton>(URadialButton::StaticClass()) };
		Canvas->AddChild(button);
		buttons.Add(button);
		button->SetButtonIdx(i);
	}

	FVector inVect{ 0.f, -1.f, 0.f };
	InitialRotationAxis = inVect.RotateAngleAxis(StartAngle, FVector::UpVector);

	FVector2D anchorMin{ 0.5f, 0.5f };
	FVector2D anchorMax{ 0.5f, 0.5f };	
	FVector2D buttonAlignment{ 0.5f, 0.5f };
	FVector2D lineAlignment{ 0.5f, 1.f };

	FAnchors anchors{};
	anchors.Maximum = anchorMin;
	anchors.Minimum = anchorMax;

	float angleDivision{ 360.f / maxWeight };
	int totalWeight{ 0 };

	for (int i{0}; i < amountOfItems; ++i)
	{
		URadialButton* button{ buttons[i] };

		button->SetNavigationRuleBase(EUINavigation::Left, EUINavigationRule::Stop);
		button->SetNavigationRuleBase(EUINavigation::Right, EUINavigationRule::Stop);
		button->SetNavigationRuleBase(EUINavigation::Down, EUINavigationRule::Stop);
		button->SetNavigationRuleBase(EUINavigation::Up, EUINavigationRule::Stop);
		button->SetNavigationRuleBase(EUINavigation::Next, EUINavigationRule::Stop);
		button->SetNavigationRuleBase(EUINavigation::Previous, EUINavigationRule::Stop);

		button->OnRadialHover.AddDynamic(this, &URadialMenu::OnRadialHover);
		button->OnRadialClicked.AddDynamic(this, &URadialMenu::OnRadialClicked);
		UCanvasPanelSlot* canvasSlot{ UWidgetLayoutLibrary::SlotAsCanvasSlot(button)};
		

		canvasSlot->SetAnchors(anchors);
		canvasSlot->SetAlignment(buttonAlignment);
		canvasSlot->SetZOrder(1);

		double anglePos{ 0.0 };
		double halfAngle{ 0.0 };

		ABaseItem* item{ nullptr };

		if (!isDesignTime && IsValid(inventory))
		{
			button->Initialize();
			item = inventory->GetItemAtSlot(i);
			anglePos = static_cast<double>(totalWeight * angleDivision);
			ApplyRadialButtonSettings(button, item, canvasSlot);
			totalWeight += item->GetItemWeight();
			halfAngle = item->GetItemWeight() / 2.0 * angleDivision;
		}
		else
		{
			anglePos = static_cast<double>(i * angleDivision);
			halfAngle = angleDivision / 2.0;
			ApplyRadialButtonSettings(button, nullptr, canvasSlot);
		}

		FVector rotationPos3D{ InitialRotationAxis.RotateAngleAxis(anglePos + halfAngle, FVector::UpVector) };
		FVector2D rotationPos{ FVector2D{rotationPos3D.X, rotationPos3D.Y} * Radius };		

		canvasSlot->SetPosition(rotationPos);

		AddDivisionLine(anglePos, lineAlignment, anchors);

		if (!(item && item->IsA<ABaseWeapon>()) || ShouldIncludeWeapons)
		{		
			FRadialButtonValues buttonValues{};
			buttonValues.ButtonAngle = FMath::RadiansToDegrees(FMath::Atan2(-rotationPos.Y, -rotationPos.X));
			buttonValues.ButtonRange = halfAngle;
			ButtonLocations.Add(button, buttonValues);
		}
		else
		{
			button->SetIsEnabled(false);
		}	

		if (totalWeight < maxWeight && i == amountOfItems - 1)
		{
			AddDivisionLine(anglePos + 2 * halfAngle, lineAlignment, anchors);
		}
	}
}

void URadialMenu::AddDivisionLine(float angle, FVector2D alignment, FAnchors anchors)
{
	UImage* lineImage{ WidgetTree->ConstructWidget<UImage>(UImage::StaticClass()) };

	Canvas->AddChild(lineImage);
	UCanvasPanelSlot* lineImageSlot{ UWidgetLayoutLibrary::SlotAsCanvasSlot(lineImage) };

	FVector2D imageSize{ static_cast<float>(RadialLineTexture->GetSizeX()),  static_cast<float>(RadialLineTexture->GetSizeY()) };
	lineImageSlot->SetSize(imageSize);
	lineImage->SetBrushFromTexture(RadialLineTexture);

	lineImageSlot->SetAnchors(anchors);
	lineImageSlot->SetAlignment(alignment);
	lineImage->SetRenderTransformPivot(alignment);
	lineImage->SetRenderTransformAngle(static_cast<float>(angle));

	FVector rotationPos3DImage{ InitialRotationAxis.RotateAngleAxis(angle, FVector::UpVector) };
	FVector2D rotationPosImage{ FVector2D{rotationPos3DImage.X, rotationPos3DImage.Y} *LineRadius };

	lineImageSlot->SetPosition(rotationPosImage);
}

void URadialMenu::SelectItem(FVector2D vector)
{
	const float minLengthSqr{ 0.5625f };
	if (vector.SquaredLength() < minLengthSqr || IsLerping || !IsWidgetFocused)
	{
		return;
	}

	float currentAngle{static_cast<float>(FMath::RadiansToDegrees(FMath::Atan2(vector.Y, -vector.X)))};	
	
	URadialButton* currentButton{ nullptr };
	for (auto& button : ButtonLocations)
	{
		float buttonRangeAngle{ button.Value.ButtonAngle - currentAngle};
		WrapAngle(buttonRangeAngle);
		if (FMath::Abs(buttonRangeAngle) < button.Value.ButtonRange)
		{
			currentButton = button.Key;
			break;
		}
	}
	if (SelectedButton != currentButton)
	{
		if (IsValid(SelectedButton))
		{
			SelectedButton->OnUnhover();
			SelectedButton = nullptr;
		}
		if (IsValid(currentButton))
		{
			SelectedButton = currentButton;
			SelectedButton->OnHover();
		}
	}

	Arrow->SetRenderTransformAngle(currentAngle - 90.f);
}

void URadialMenu::ClickItem()
{
	if (IsValid(this) && IsValid(SelectedButton))
	{
		GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Blue, SelectedButton->GetName());
		SelectedButton->OnItemClicked();
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Blue, "No button found");
	}
}

void URadialMenu::OnRadialHover(URadialButton* button)
{
	if (SelectedButton != button)
	{
		if (IsValid(SelectedButton))
		{
			SelectedButton->OnUnhover();
		}
		SelectedButton = button;

		if (!IsLerping)
		{
			IsLerping = true;
			LerpArrow();
		}
	}
}

void URadialMenu::OnRadialClicked(URadialButton* button)
{
	if (SelectedButton != button || IsLerping)
	{
		return;
	}
	UInventoryComponent* inventory{ Player->GetInventory() };
	if (IsValid(inventory))
	{
		int itemIdx{ button->GetButtonIdx() };
		ABaseItem* item{ inventory->GetItemAtSlot(itemIdx) };
		if (!IsValid(item))
		{
			return;
		}
		OnItemSelected.Broadcast(item);
	}
}

void URadialMenu::RestoreButtonFocus()
{
	if (IsValid(SelectedButton))
	{
		SelectedButton->SetFocus();
		SelectedButton->SetKeyboardFocus();
	}	
}

void URadialMenu::IncludeWeapons(bool include)
{
	ShouldIncludeWeapons = include;
}

void URadialMenu::NativeOnAddedToFocusPath(const FFocusEvent& InFocusEvent)
{
	IsWidgetFocused = true;
}

void URadialMenu::NativeOnRemovedFromFocusPath(const FFocusEvent& InFocusEvent)
{
	IsWidgetFocused = false;
}

void URadialMenu::WrapAngle(float& angle) const
{
	if (angle > 180.f)
	{
		angle -= 360.f;
	}
	else if (angle < -180.f)
	{
		angle += 360.f;
	}
}

void URadialMenu::ApplyRadialButtonSettings(URadialButton* button, ABaseItem* item, UCanvasPanelSlot* slot)
{
	FSlateBrush normal{};
	normal.DrawAs = ESlateBrushDrawType::Image;
	normal.TintColor = FLinearColor{ 0.5f, 0.5f, 0.5f };
	normal.OutlineSettings.RoundingType = ESlateBrushRoundingType::HalfHeightRadius;
	normal.OutlineSettings.bUseBrushTransparency = false;

	FSlateBrush hovered{};
	hovered.DrawAs = ESlateBrushDrawType::Image;	
	hovered.OutlineSettings.RoundingType = ESlateBrushRoundingType::HalfHeightRadius;
	hovered.OutlineSettings.bUseBrushTransparency = false;

	FSlateBrush pressed{};
	pressed.DrawAs = ESlateBrushDrawType::Image;
	pressed.OutlineSettings.RoundingType = ESlateBrushRoundingType::HalfHeightRadius;
	pressed.OutlineSettings.bUseBrushTransparency = false;

	if (IsValid(item))
	{
		UTexture2D* image{ item->GetItemImage() };
		if (IsValid(image))
		{
			slot->SetSize(FVector2D{ static_cast<float>(image->GetSizeX()), static_cast<float>(image->GetSizeY()) });
			normal.SetResourceObject(image);
			hovered.SetResourceObject(image);
			pressed.SetResourceObject(image);
		}
	}
	button->SetDefaultStyles(normal, hovered, pressed);
}

void URadialMenu::StartArrowLerp()
{
	IsLerping = true;
	LerpArrow();
}

void URadialMenu::LerpArrow()
{
	float buttonAngle{ ButtonLocations.FindRef(SelectedButton).ButtonAngle - 90.f};
	float arrowAngle{ Arrow->GetRenderTransformAngle() };
	WrapAngle(buttonAngle);
	WrapAngle(arrowAngle);
	
	float tickDelta{ static_cast<float>(UGameplayStatics::GetWorldDeltaSeconds(GetWorld())) };
	float rotationDelta{ tickDelta * ArrowRotationSpeed };

	if (FMath::Abs(buttonAngle - arrowAngle) > rotationDelta)
	{
		float angleInbetween{ buttonAngle - arrowAngle };
		WrapAngle(angleInbetween);
		float sign{ angleInbetween >= 0.f ? 1.f : -1.f };
		

		Arrow->SetRenderTransformAngle(arrowAngle + rotationDelta * sign);
		GetWorld()->GetTimerManager().SetTimerForNextTick(this, &URadialMenu::LerpArrow);
	}
	else
	{
		Arrow->SetRenderTransformAngle(buttonAngle);
		
		IsLerping = false;
	}
}
