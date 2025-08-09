#include "BaseWidget.h"
#include "../Characters/Player/PlayerCharacter.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "MenuButton.h"

void UBaseWidget::SetParentWidget(UBaseWidget* widget)
{
	PreviousWidget = widget;
	AddWidget();
}

void UBaseWidget::DisableAutoRegainFocus()
{
	ShouldAutoRegainFocus = false;
}

void UBaseWidget::AddWidget()
{
	APlayerController* playerController = GetWorld()->GetFirstPlayerController();
	APlayerCharacter* player{ Cast<APlayerCharacter>(playerController->GetCharacter()) };
	if (!IsValid(PreviousWidget) && IsValid(player))
	{
		player->UpdateUICounter(1);
	}
	AddToViewport();
	RestoreFocus(playerController);
}

void UBaseWidget::RestoreFocus(APlayerController* playerController)
{
	if (!IsValid(playerController))
	{
		playerController = GetWorld()->GetFirstPlayerController();
	}	

	ShouldAutoRegainFocus = true;

	FInputModeGameAndUI inputMode{};
	inputMode.SetWidgetToFocus(GetCachedWidget());
	inputMode.SetLockMouseToViewportBehavior(EMouseLockMode::LockAlways);
	playerController->SetInputMode(inputMode);
	GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Emerald, GetName());

	int viewportX;
	int viewportY;
	playerController->GetViewportSize(viewportX, viewportY);
	playerController->SetMouseLocation(viewportX - 1, viewportY - 1);
	playerController->SetShowMouseCursor(false);
}

void UBaseWidget::RemoveWidget()
{
	if (IsValid(PreviousWidget))
	{
		PreviousWidget->RestoreFocus();
	}
	
	APlayerController* playerController = GetWorld()->GetFirstPlayerController();
	APlayerCharacter* player{ Cast<APlayerCharacter>(playerController->GetCharacter()) };
	if (!IsValid(PreviousWidget) && (!IsValid(player) || !player->UpdateUICounter(-1)))
	{
		FInputModeGameOnly inputMode{};
		playerController->SetInputMode(inputMode);
	}
	RemoveFromParent();
}

void UBaseWidget::InitializeWidget()
{
	APlayerController* playerController = GetWorld()->GetFirstPlayerController();
}

void UBaseWidget::HideMouse()
{
	APlayerController* playerController{ GetWorld()->GetFirstPlayerController() };
	playerController->SetMouseLocation(0, 0);
	playerController->SetShowMouseCursor(false);
}

void UBaseWidget::SetActiveButton(UMenuButton* button)
{
	GEngine->AddOnScreenDebugMessage(-1, 0.5f, FColor::Magenta, "Active Button " + button->GetName());
	ActiveButton = button;
	SetDesiredFocusWidget(button);
}

void UBaseWidget::SetButtonActiveAndFocus(UMenuButton* button)
{
	if (IsValid(button))
	{
		button->SetFocus();
		button->SetKeyboardFocus();
		SetActiveButton(button);
	}
}

void UBaseWidget::NativeOnAddedToFocusPath(const FFocusEvent& InFocusEvent)
{
}

void UBaseWidget::NativeOnRemovedFromFocusPath(const FFocusEvent& InFocusEvent)
{
	if (ShouldAutoRegainFocus)
	{
		RestoreFocus();
	}
}

void UBaseWidget::NativeDestruct()
{
	GEngine->ForceGarbageCollection();
}

void UBaseWidget::OnWidgetFocused()
{
	if (IsValid(ActiveButton))
	{
		GEngine->AddOnScreenDebugMessage(-1, 0.5f, FColor::Magenta, "Active Button " + ActiveButton->GetName());
		ActiveButton->SetFocus();
		ActiveButton->SetKeyboardFocus();
	}
}
