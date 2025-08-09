
#include "WinScreen.h"
#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"
#include "MenuButton.h"

void UWinScreen::InitializeWidget()
{
	MainMenu_Btn->GetButton()->OnClicked.AddDynamic(this, &UWinScreen::GoToMainMenu);
	MainMenu_Btn->OnButtonFocusGained.AddDynamic(this, &UWinScreen::SetActiveButton);
	AddWidget();
}

void UWinScreen::GoToMainMenu()
{
	UGameplayStatics::OpenLevel(GetWorld(), MainMenuLevelName);
}
