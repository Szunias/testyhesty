
#include "MainMenu.h"
#include "Components/Button.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "ControlsScreen.h"
#include "MenuButton.h"

void UMainMenu::InitializeWidget()
{
	Play_Btn->GetButton()->OnClicked.AddDynamic(this, &UMainMenu::PlayGame);
	Controls_Btn->GetButton()->OnClicked.AddDynamic(this, &UMainMenu::ShowControls);
	Quit_Btn->GetButton()->OnClicked.AddDynamic(this, &UMainMenu::QuitGame);

	Play_Btn->OnButtonFocusGained.AddDynamic(this, &UMainMenu::SetActiveButton);
	Controls_Btn->OnButtonFocusGained.AddDynamic(this, &UMainMenu::SetActiveButton);
	Quit_Btn->OnButtonFocusGained.AddDynamic(this, &UMainMenu::SetActiveButton);

	AddWidget();
}

void UMainMenu::PlayGame()
{
	APlayerController* playerController{ GetWorld()->GetFirstPlayerController() };
	playerController->SetInputMode(FInputModeGameOnly{});
	playerController->SetShowMouseCursor(false);
	UGameplayStatics::OpenLevel(GetWorld(), LevelNameToOpen);
}

void UMainMenu::ShowControls()
{
	// when the controls menu is finished, spawn it here.
	UControlsScreen* controlsScreen{ CreateWidget<UControlsScreen>(GetWorld(), ControlsScreenClass) };
	controlsScreen->InitializeWidget();
	controlsScreen->SetParentWidget(this);
}

void UMainMenu::QuitGame()
{
	UKismetSystemLibrary::QuitGame(GetWorld(), GetWorld()->GetFirstPlayerController(), EQuitPreference::Quit, false);
}
