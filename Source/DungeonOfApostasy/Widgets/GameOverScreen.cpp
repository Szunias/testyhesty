
#include "GameOverScreen.h"
#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"
#include "MenuButton.h"

void UGameOverScreen::InitializeWidget()
{
	Retry_Btn->GetButton()->OnClicked.AddDynamic(this, &UGameOverScreen::RetryGame);
	GiveUp_Btn->GetButton()->OnClicked.AddDynamic(this, &UGameOverScreen::GiveUp);

	Retry_Btn->OnButtonFocusGained.AddDynamic(this, &UGameOverScreen::SetActiveButton);
	GiveUp_Btn->OnButtonFocusGained.AddDynamic(this, &UGameOverScreen::SetActiveButton);

	AddWidget();
}

void UGameOverScreen::RetryGame()
{
	APlayerController* playerController{ GetWorld()->GetFirstPlayerController() };
	playerController->SetInputMode(FInputModeGameOnly{});
	playerController->SetShowMouseCursor(false);
	UGameplayStatics::OpenLevel(GetWorld(), BaseLevelName);
}

void UGameOverScreen::GiveUp()
{
	UGameplayStatics::OpenLevel(GetWorld(), MainMenuLevelName);
}
