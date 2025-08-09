
#include "ControlsScreen.h"
#include "Components/Button.h"
#include "MenuButton.h"

void UControlsScreen::InitializeWidget()
{
	GoBack_Btn->GetButton()->OnClicked.AddDynamic(this, &UControlsScreen::GoBack);

	GoBack_Btn->OnButtonFocusGained.AddDynamic(this, &UControlsScreen::SetActiveButton);
}

void UControlsScreen::GoBack()
{
	RemoveWidget();
}
